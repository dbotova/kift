/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   recognize.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbotova <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/18 16:15:32 by dbotova           #+#    #+#             */
/*   Updated: 2017/06/18 16:21:56 by dbotova          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kift_client.h"

static void				AudioCallback(void *userdata, Uint8 *stream, int len)
{
	t_client_connection	*con;
	int32_t				num_samples;
	int					rc;

	con = (t_client_connection*)userdata;
	num_samples = len / 2;
	send(con->sock, &num_samples, sizeof(num_samples), 0);
	rc = send(con->sock, stream, len, 0);
}

void					recognize(t_client_connection *con)
{
	const char			*devname;
	SDL_AudioSpec		wanted;
	char				server_reply[BUF_SIZE * 2];

	devname = NULL;
	memset(server_reply, 0, BUF_SIZE * 2);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
			"Couldn't initialize SDL: %s\n", SDL_GetError());
	SDL_zero(wanted);
	wanted.freq = 16000;
	wanted.format = AUDIO_S16LSB;
	wanted.channels = 1;
	wanted.samples = BUF_SIZE / 2;
	wanted.callback = AudioCallback;
	wanted.userdata = con;
	SDL_zero(g_spec);
	g_devid_in = SDL_OpenAudioDevice(NULL, SDL_TRUE, &wanted, &g_spec, 0);
	if (!g_devid_in)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
			"Couldn't open an audio device for capture: %s!\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
	SDL_PauseAudioDevice(g_devid_in, SDL_FALSE);
	printf("Sending...\n");
	while (42)
	{
		memset(server_reply, 0, BUF_SIZE * 2);
		if (recv(con->sock, server_reply, BUF_SIZE * 2, 0) < 0)
		{
			perror("recv failed");
			return ;
		}
		printf("\nServer reply :\n");
		printf("%s\n", server_reply);
		if (parse_reply(server_reply) == 1)
			break ;
	}
	SDL_Log("Shutting down.\n");
	SDL_PauseAudioDevice(g_devid_in, 1);
	SDL_CloseAudioDevice(g_devid_in);
	SDL_Quit();
}
