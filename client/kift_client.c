/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kift_client.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbotova <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/05 16:39:32 by dbotova           #+#    #+#             */
/*   Updated: 2017/06/05 16:39:33 by dbotova          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kift_client.h"

// static int read_input(char **str)
// {
// 	size_t bufsize = BUF_SIZE;
// 	ssize_t len = 0;

// 	len = getline(str, &bufsize, stdin);
// 	if (len <= 0)
// 		return (-1);

// 	(*str)[--len] = 0;
// 	return (0);
// }

static int init_connect(t_client_connection *con)
{
    con->sock = socket(AF_INET , SOCK_STREAM , 0);
    if (con->sock == -1)
    {
        printf("Could not create socket\n");
        return (-1);
    }
    puts("Socket created");
    con->server.sin_addr.s_addr = inet_addr("127.0.0.1");
    con->server.sin_family = AF_INET;
    con->server.sin_port = htons(8888);
    if (connect(con->sock, (struct sockaddr *)&con->server, sizeof(con->server)) < 0)
    {
        perror("connect failed. Error");
        return (-1);
    }
    return (0);
}

static int read_server(t_client_connection *con)
{
    //char *message = (char*)malloc(sizeof(char) * BUF_SIZE);
    char server_reply[BUF_SIZE * 2] = {0};

    FILE *fh;
    int buf[BUF_SIZE];
    size_t nsamp;
     
    //message = NULL;
    while(42)
    {
    	// FUN STARTS HERE
        //printf("\nEnter message :\n");
        //read_input(&message);
        // if (strcmp(message, "exit") == 0)
        //     break ;
        fh = fopen("goforward.raw", "rb");
    	if (fh == NULL)
    	{
        	fprintf(stderr, "Unable to open input file goforward.raw\n");
        	return -1;
    	}
    	 while (!feof(fh))
    	 {
    	 	nsamp = fread(buf, 2, BUF_SIZE, fh);
    	 	send(con->sock, buf, nsamp, 0);
    	 }

        if(send(con->sock, buf, nsamp, 0) < 0)
        {
            perror("Send failed");
            return (-1);
        }
        if(recv(con->sock, server_reply, BUF_SIZE * 2, 0) < 0)
        {
            perror("recv failed");
            return (-1);
        }
        printf("\nServer reply :\n");
        printf("%s", server_reply);
        //SMART_FREE(message);
        fclose(fh);
    }
    return (0);
}

int main()
{
    t_client_connection *con;

    con = malloc(sizeof(t_client_connection));
    if (init_connect(con) < 0)
        return (-1);
    printf("Connected\n");
    read_server(con);
    close(con->sock);
    SMART_FREE(con);
    return (0);
}
