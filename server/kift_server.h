/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kift_server.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbotova <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/05 16:36:51 by dbotova           #+#    #+#             */
/*   Updated: 2017/06/05 16:36:52 by dbotova          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KIFT_SERVER_H
# define KIFT_SERVER_H
# include <sys/resource.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/wait.h>
# include <signal.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <string.h>
# include <sys/wait.h>
# include <signal.h>
# define BUF_SIZE 1000
# define SMART_FREE(x) {free(x); x = NULL;}

typedef struct	s_connection
{
	int			socket_desc;
	int			client_sock;
	char		client_message[BUF_SIZE];
	struct		sockaddr_in server;
	struct		sockaddr_in client;
} t_connection;

typedef enum
{
	UTT_STATE_WAITING = 0,
	UTT_STATE_LISTENING,
	UTT_STATE_FINISHED,
	UTT_STATE_ERROR,
	UTT_STATE_MAX,
	UTT_STATE_QUIT
} t_utt_states;
#endif
