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


static int parse_reply (char *hyp)
{
	int result = 0;

	SDL_PauseAudioDevice(devid_in, SDL_TRUE);
    if (strstr(hyp, "OKAY KIFT") || strstr(hyp, "PREEV’AT KIFT"))
        system("../../SAM/sam Yes master");

    else if (strstr(hyp, "OPEN SAFARI"))
        system("/Applications/Safari.app/Contents/MacOS/Safari & sleep 1");

    else if (strstr(hyp, "WHO YOU ARE?"))
        system("../../SAM/sam I am a  robot. Your  KIFT  project  product");

     else if (strstr(hyp, "TELL ME A STORY"))
        system("../../SAM/sam You were  to  busy  to tell  me any");

    else if (strstr(hyp, "STUPID"))
        system("../../SAM/sam That was mean");

    else if (strstr(hyp, "HA-HA-HA"))
        system("../../SAM/sam What is so funny?");

    else if (strstr(hyp, "STOP IT"))
        system("../../SAM/sam I am sorry");

    else if (strstr(hyp, "DO YOU LOVE ME?"))
        system("../../SAM/sam It is a  bad place   to   look   for   a love");

    else if (strstr(hyp, "SHUTDOWN"))
    {
        system("../../SAM/sam OKaey master");
        result = 1;
    }
 	
 	SDL_PauseAudioDevice(devid_in, SDL_FALSE);
    return result;
}

void AudioCallback(void*  userdata,
                       Uint8* stream,
                       int    len)
{
	t_client_connection *con = (t_client_connection*)userdata;

    int32_t num_samples = len / 2;
    send(con->sock, &num_samples, sizeof(num_samples), 0);
    int rc = send(con->sock, stream, len, 0);
    printf("%d of %d bytes sent.\n", rc, len);
}


static void recognize(t_client_connection *con)
{
    const char *devname = NULL;
    SDL_AudioSpec wanted;

    /* Enable standard application logging */
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    /* Load the SDL library */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());

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

    while (42)
    {
	    char server_reply[BUF_SIZE * 2] = {0};

		memset(server_reply, 0, BUF_SIZE * 2);
	    if(recv(con->sock, server_reply, BUF_SIZE * 2, 0) < 0)
	    {
	        perror("recv failed");
	        return;
	    }

	    printf("\nServer reply :\n");
	    printf("%s\n", server_reply);
	    if (parse_reply(server_reply) == 1)
	    	break;
	}

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
