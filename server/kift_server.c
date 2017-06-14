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
static FILE *rawfd;

/* Sleep for specified msec */
// static void sleep_msec(int32 ms)
// {
//     struct timeval tmo;

//     tmo.tv_sec = 0;
//     tmo.tv_usec = ms * 1000;

//     select(0, NULL, NULL, NULL, &tmo);
// }

// /*
//  * Main utterance processing loop:
//  *     for (;;) {
//  *        start utterance and wait for speech to process
//  *        decoding till end-of-utterance silence will be detected
//  *        print utterance result;
//  *     }
//  */
// static void parse_hyp (char const *hyp)
// {
//     if (strcmp(hyp, "OKAY KIFT") == 0)
//         system("../../SAM/sam Yes master");

//     if (strcmp(hyp, "OPEN SAFARI") == 0)
//         system("/Applications/Safari.app/Contents/MacOS/Safari & sleep 1");

//     if (strcmp(hyp, "SHUTDOWN") == 0)
//     {
//         system("../../SAM/sam OKaey master");
//         exit(0);
//     }
// }

// static void recognize()
// {
//     ad_rec_t *ad;
//     int16 adbuf[2048];
//     uint8 utt_started, in_speech;
//     int32 k;
//     char const *hyp;

//     if ((ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"),
//                           (int) cmd_ln_float32_r(config,
//                                                  "-samprate"))) == NULL)
//         printf("Failed to open audio device\n");
//     if (ad_start_rec(ad) < 0)
//         printf("Failed to start recording\n");

//     if (ps_start_utt(ps) < 0)
//         printf("Failed to start utterance\n");
//     utt_started = FALSE;
//     printf("Ready....\n");

//     while (42)
//     {
//         if ((k = ad_read(ad, adbuf, 2048)) < 0)
//             printf("Failed to read audio\n");
//         ps_process_raw(ps, adbuf, k, FALSE, FALSE);
//         in_speech = ps_get_in_speech(ps);
//         if (in_speech && !utt_started)
//         {
//             utt_started = TRUE;
//             printf("Listening...\n");
//         }
//         if (!in_speech && utt_started)
//         {
//             /* speech -> silence transition, time to start new utterance  */
//             ps_end_utt(ps);
//             hyp = ps_get_hyp(ps, NULL );
//             if (hyp != NULL)
//             {
//                 printf("%s\n", hyp);
//                 fflush(stdout);
//             }

//            parse_hyp(hyp);

//             if (ps_start_utt(ps) < 0)
//                 printf("Failed to start utterance\n");
//             utt_started = FALSE;
//             printf("Ready....\n");
//         }
//         sleep_msec(100);
//     }

//     ad_close(ad);
// }

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

static int kift_listen(t_connection *con)
{
    int c;

    c = 0;
    int read_size = 0;


    ps_decoder_t *ps;
    cmd_ln_t *config;
    //FILE *fh;
    char const *hyp, *uttid;
    //int16 buf[512];
    int rv;
    int score;
    int16 client_message[BUF_SIZE];

    config = cmd_ln_init(NULL, ps_args(), TRUE,
                 "-hmm", MODELDIR "/en-us/en-us",
                 "-lm", MODELDIR "/en-us/en-us.lm.bin",
                 "-dict", MODELDIR "/en-us/cmudict-en-us.dict",
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



    c = sizeof(struct sockaddr_in);
    if (server_accept_client(con) < 0)
        exit(-1);
    rv = ps_start_utt(ps);
    while((read_size = recv(con->client_sock, client_message, BUF_SIZE , 0)) >= 0)
    {
    	// ******* FUN STARTS HERE
        if (read_size > 0)
        {
           	rv = ps_process_raw(ps, client_message, read_size, FALSE, FALSE);
           	write(con->client_sock , "got something\n", strlen("got something\n"));
        }

        else
        {
            puts("Client disconnected");
            fflush(stdout);
            if (server_accept_client(con) < 0)
                exit(-1);
        }
    }
    if(read_size == -1)
        perror("recv failed");

	rv = ps_end_utt(ps);
    hyp = ps_get_hyp(ps, &score);
    printf("Recognized: %s\n", hyp);
    write(con->client_sock , hyp, strlen(hyp));
    ps_free(ps);
    cmd_ln_free_r(config);

    exit(0);
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
    if(bind(con->socket_desc, (struct sockaddr *)&con->server, sizeof(con->server)) < 0)
    {
        perror("bind failed. Error");
        return (-1);
    }
    puts("Hydar server has been started");
    return (0);
}

static int kift_connect(t_connection *con, int is_d)
{
    pid_t pid;

    con = malloc(sizeof(t_connection));
    if (init_connnect(con) < 0)
    {
        perror("Error occured");
        exit(-1);
    }
    listen(con->socket_desc , 3);
    // if (is_d)
    // {
    //     if ((pid = fork()) < 0)
    //         exit(-1);
    //     if (pid == 0)
    //         exit(daemon_listen(con));
    //     else
    //         exit(0);
    // }
    //else if (kift_listen(con) < 0)
    if (kift_listen(con) < 0)
        return (-1);
    return (0);
}

int main(int argc, char *argv[])
{
	t_connection *con;
    int is_d;

    is_d = 0;
    con = NULL;
    if (argc == 2 && strcmp(argv[1], "-D") == 0)
        is_d = 1;
    kift_connect(con, is_d);
    if (con->client_sock < 1)
        return (1);
    SMART_FREE(con);

    return (0);

    // char const *cfg;

    // config = cmd_ln_init(config, ps_args(), TRUE,
    //              "-hmm", MODELDIR "/en-us/en-us",
    //              "-lm", "9816.lm",
    //              "-dict", "9816.dic",
    //              "-logfn", "kift.log",
    //              NULL);
    // ps = ps_init(config);
    // if (ps == NULL)
    // {
    //     cmd_ln_free_r(config);
    //     return 1;
    // }

    // recognize();

    // ps_free(ps);
    // cmd_ln_free_r(config);

    // return 0;
}

