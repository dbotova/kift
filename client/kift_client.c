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

#include <SDL.h>

#include <stdlib.h>

static SDL_AudioSpec spec;
static SDL_AudioDeviceID devid_in = 0;


static void parse_reply (char *hyp)
{
    if (strcmp(hyp, "OKAY KIFT") == 0)
        system("../../SAM/sam Yes master");

    if (strcmp(hyp, "OPEN SAFARI") == 0)
        system("/Applications/Safari.app/Contents/MacOS/Safari & sleep 1");

    if (strcmp(hyp, "SEND E-MAIL") == 0)
        system("../../SAM/sam I can't do it");

    if (strcmp(hyp, "SHUTDOWN") == 0)
    {
        system("../../SAM/sam OKaey master");
        exit(0);
    }
}

void AudioCallback(void*  userdata,
                       Uint8* stream,
                       int    len)
{
	t_client_connection *con = (t_client_connection*)userdata;
	con->has_data = 1;

    int32_t num_samples = len / 2;
    send(con->sock, &num_samples, sizeof(num_samples), 0);
    int rc = send(con->sock, stream, len, 0);
    printf("%d of %d bytes sent.\n", rc, len);
}


static void recognize(t_client_connection *con)
{
    const char *devname = NULL;
    SDL_AudioSpec wanted;
    int devcount;
    int i;

    /* Enable standard application logging */
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    /* Load the SDL library */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
        //return (1);
    }

    devcount = SDL_GetNumAudioDevices(SDL_TRUE);
    for (i = 0; i < devcount; i++)
        SDL_Log(" Capture device #%d: '%s'\n", i, SDL_GetAudioDeviceName(i, SDL_TRUE));

    SDL_zero(wanted);
    wanted.freq = 16000;
    wanted.format = AUDIO_S16LSB;
    wanted.channels = 1;
    wanted.samples = BUF_SIZE / 2;
    wanted.callback = AudioCallback;
    wanted.userdata = con;

    SDL_zero(spec);

    devid_in = SDL_OpenAudioDevice(NULL, SDL_TRUE, &wanted, &spec, 0);
    if (!devid_in)
    {
    	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't open an audio device for capture: %s!\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    SDL_PauseAudioDevice(devid_in, SDL_FALSE);

    // Wait for server to tell us what to do while feeding it
    // with audio samples.
    printf("Sending...\n");

    char server_reply[BUF_SIZE * 2] = {0};

	memset(server_reply, 0, BUF_SIZE * 2);
    if(recv(con->sock, server_reply, BUF_SIZE * 2, 0) < 0)
    {
        perror("recv failed");
        return;
    }

    printf("\nServer reply :\n");
    printf("%s\n", server_reply);
    parse_reply(server_reply);

    SDL_Log("Shutting down.\n");
    SDL_PauseAudioDevice(devid_in, 1);
    SDL_CloseAudioDevice(devid_in);
    SDL_Quit();
}



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
    con->has_data = 0;
    if (connect(con->sock, (struct sockaddr *)&con->server, sizeof(con->server)) < 0)
    {
        perror("connect failed. Error");
        return (-1);
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
    recognize(con);
    close(con->sock);
    SMART_FREE(con);
    return (0);
}
