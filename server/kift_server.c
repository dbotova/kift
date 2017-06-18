/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kift_server.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbotova <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/05 16:36:39 by dbotova           #+#    #+#             */
/*   Updated: 2017/06/05 16:36:40 by dbotova          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kift_server.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <sphinxbase/err.h>
#include <sphinxbase/ad.h>
#include "pocketsphinx.h"

static ps_decoder_t *ps;
static cmd_ln_t *config;
static t_utt_states utt_state = UTT_STATE_WAITING;

static int  server_accept_client(t_connection *con)
{
    int c;

    c = 0;
    puts("Waiting for incoming connections...");
    con->client_sock = accept(con->socket_desc, (struct sockaddr *)&con->client, (socklen_t*)&c);
    if (con->client_sock < 0)
    {
        perror("accept failed");
        return (-1);
    }
    puts("Connection accepted");
    return (0);
}

static int read_data(int16 *buf, int num_samples, int socket)
{
	char *buf_pointer = (char*)buf;
	int num_bytes = num_samples * sizeof(int16);

	while(num_bytes)
	{
		int rc = recv(socket, buf_pointer, num_bytes, 0);
		if (rc < 0)
			return rc;
		if (rc > 0)
		{
			num_bytes -= rc;
			buf_pointer += rc;
			continue ;
		}
		return 0;
	}
	return num_samples;
}

static int read_samples(ps_decoder_t *ps, int num_samples, int socket)
{
	int16 client_message[BUF_SIZE];

	while (num_samples)
	{
		int rc = read_data(client_message, (num_samples > BUF_SIZE) ? BUF_SIZE : num_samples, socket);
		if (rc > 0)
		{
			uint8 in_speech = 0;
			ps_process_raw(ps, client_message, rc, FALSE, FALSE);
            in_speech = ps_get_in_speech(ps);
        	if (in_speech && (utt_state == UTT_STATE_WAITING))
        	{
        		utt_state = UTT_STATE_LISTENING;
            	printf("Listening...\n");
        	}
        	if (!in_speech && (utt_state == UTT_STATE_LISTENING))
        	{
        		utt_state = UTT_STATE_FINISHED;
            	printf("Finished, processing...\n");
        	}

			num_samples -= rc;
		}
		else
			return rc;
	}
	return 1;
}

static int kift_listen(t_connection *con)
{
    int read_size = 0;

    ps_decoder_t *ps;
    cmd_ln_t *config;
    char const *hyp, *uttid;
    int rv;
    int score;

    while (42)
    {
    	pid_t pid = 0;

	    if (server_accept_client(con) < 0)
        	exit(-1);

        pid = fork();
        if (pid < 0)
        {
        	printf("fork() failed with error %d\n", errno);
        	exit(2);
        }
        if(pid > 0)
        {
            close(con->client_sock);
            printf ("Keep Waiting...\n");
            continue;
        }

        break ;
    }

    config = cmd_ln_init(NULL, ps_args(), TRUE,
                 "-hmm", MODELDIR "/en-us/en-us",
                 "-lm", "9382.lm",
                 "-dict", "9382.dic",
                 "-logfn", "kift.log",
                 NULL);
    
    if (config == NULL) {
        fprintf(stderr, "Failed to create config object, see log for  details\n");
        return -1;
    }
    
    ps = ps_init(config);
    if (ps == NULL) {
        fprintf(stderr, "Failed to create recognizer, see log for  details\n");
        return -1;
    }


    while(utt_state < UTT_STATE_QUIT)
    {
	    rv = ps_start_utt(ps);
	    utt_state = UTT_STATE_WAITING;
	    int32 num_samples = 0;
	    do
	    {
		    read_size = 0;
		    int rc = recv(con->client_sock, &num_samples, sizeof(num_samples), 0);
		    if (rc <= 0)
		    {
		    	printf("Error or disconnected (%d) errno=%d\n", rc, errno);
	 		   	utt_state = UTT_STATE_ERROR;
		    	break;
		    }
		    printf("%d samples (rc = %d)\n", num_samples, rc);
		    if (num_samples)
		    {
	 		   	read_size = read_samples(ps, num_samples, con->client_sock);
	 		   	if (read_size != 1)
	 		   	{
	 		   		printf("Error reading samples: %d\n", read_size);
	 		   		utt_state = UTT_STATE_ERROR;
	 		   	}
		    }
	    }
	    while(utt_state < UTT_STATE_FINISHED);
	 
	 	if (utt_state == UTT_STATE_FINISHED)
	 	{
			rv = ps_end_utt(ps);
			hyp = ps_get_hyp(ps, &score);
			printf("Recognized: %s\n", hyp);
			if (hyp)
				send(con->client_sock , hyp, strlen(hyp), 0);
			if (hyp && strcmp(hyp, "SHUTDOWN") == 0)
				utt_state = UTT_STATE_QUIT;
			else
				utt_state = UTT_STATE_WAITING;
		}
		else
		{
			printf("Terminating connection due to error.\n");
			break;
		}
	}

    ps_free(ps);
    cmd_ln_free_r(config);
    return (0);
}

static int init_connnect(t_connection *con)
{
    con->socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (con->socket_desc == -1)
    {
        puts("Could not create socket");
        return (-1);
    }
    con->server.sin_family = AF_INET;
    con->server.sin_addr.s_addr = INADDR_ANY;
    con->server.sin_port = htons(8888);



	int enable = 1;
	if (setsockopt(con->socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	    perror("setsockopt(SO_REUSEADDR) failed");

    if(bind(con->socket_desc, (struct sockaddr *)&con->server, sizeof(con->server)) < 0)
    {
        perror("bind failed. Error");
        return (-1);
    }
    puts("KIFT server has been started");
    return (0);
}

static int kift_connect(t_connection **con)
{
    *con = malloc(sizeof(t_connection));
    if (init_connnect(*con) < 0)
    {
        perror("Error occured");
        exit(-1);
    }
    listen((*con)->socket_desc , 3);
    if (kift_listen(*con) < 0)
        return (-1);
    return (0);
}

int main(int argc, char *argv[])
{
	t_connection *con;

    con = NULL;
    kift_connect(&con);
    if (con->client_sock < 1)
        return (1);
    SMART_FREE(con);

    return (0);
}

