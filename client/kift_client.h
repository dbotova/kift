/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kift_client.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbotova <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/05 16:39:36 by dbotova           #+#    #+#             */
/*   Updated: 2017/06/05 16:39:37 by dbotova          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KIFT_CLIENT_H
# define KIFT_CLIENT_H
# include <stdio.h>
# include <stdint.h>
# include <string.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/stat.h>
# define BUF_SIZE (1024)  // 512 samples 16 bits each.
# define SMART_FREE(x) {free(x); x = NULL;}

typedef struct	s_client_connection
{
	int			sock;
	int 		has_data;
	struct		sockaddr_in server;
}				t_client_connection;

#endif
