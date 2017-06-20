/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kift_client.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbotova <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/05 16:39:32 by dbotova           #+#    #+#             */
/*   Updated: 2017/06/18 18:49:41 by dbotova          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kift_client.h"

SDL_AudioSpec			g_spec;
SDL_AudioDeviceID		g_devid_in = 0;

static int				init_connect(t_client_connection *con, char *addr)
{
	con->sock = socket(PF_INET, SOCK_STREAM, 0);
	if (con->sock == -1)
	{
		printf("Could not create socket\n");
		return (-1);
	}
	puts("Socket created");
	con->server.sin_addr.s_addr = inet_addr(addr ? addr : "127.0.0.1");
	con->server.sin_family = AF_INET;
	con->server.sin_port = htons(8888);
	if (connect(con->sock, (struct sockaddr *)&con->server,
		sizeof(con->server)) < 0)
	{
		perror("connect failed. Error");
		return (-1);
	}
	return (0);
}

int						main(int argc, char **argv)
{
	t_client_connection	*con;

	con = malloc(sizeof(t_client_connection));
	if (argc && init_connect(con, argv[1]) < 0)
		return (-1);
	printf("Connected\n");
	recognize(con);
	close(con->sock);
	SMART_FREE(con);
	return (0);
}
