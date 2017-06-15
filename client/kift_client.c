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
    con->sock = socket(PF_INET , SOCK_STREAM , 0);
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

static off_t fsize(const char *filename)
{
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1; 
}

static int read_server(t_client_connection *con)
{
    char server_reply[BUF_SIZE * 2] = {0};

    FILE *fh;
    int16_t buf[BUF_SIZE];
    size_t nsamp;
     
    //while(42)
    //{
        off_t size = fsize("goforward.raw"); //check for errors
        int32_t num_samples = size / 2;

        fh = fopen("goforward.raw", "rb");
    	if (fh == NULL)
    	{
        	fprintf(stderr, "Unable to open input file goforward.raw\n");
        	return -1;
    	}
    	// check for errors
    	send(con->sock, &num_samples, sizeof(num_samples), 0);
		while (!feof(fh))
		{
			nsamp = fread(buf, 2, BUF_SIZE, fh);
			size_t s = send(con->sock, buf, nsamp * 2, 0);
			printf("\n\n\n sent %zu \n", s);
		}

    	memset(server_reply, 0, BUF_SIZE * 2);
        if(recv(con->sock, server_reply, BUF_SIZE * 2, 0) < 0)
        {
            perror("recv failed");
            return (-1);
        }
        printf("\nServer reply :\n");
        printf("%s\n", server_reply);
        fclose(fh);
    //}
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
