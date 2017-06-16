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

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
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

static int loop(t_client_connection *con)
{
    SDL_bool please_quit = SDL_FALSE;
    SDL_Event e;
    int want_stop = 0;

    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
            please_quit = SDL_TRUE;
        else if (e.type == SDL_KEYDOWN)
        {
            if (e.key.keysym.sym == SDLK_ESCAPE)
                please_quit = SDL_TRUE;
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            if (e.button.button == 1)
            {
                SDL_PauseAudioDevice(devid_in, SDL_FALSE);
                want_stop = 0;
            }
        }
        else if (e.type == SDL_MOUSEBUTTONUP)
        {
            if (e.button.button == 1)
            {
                SDL_PauseAudioDevice(devid_in, SDL_TRUE);
                want_stop = 1;
            }
        }
    }

    if (SDL_GetAudioDeviceStatus(devid_in) == SDL_AUDIO_PLAYING)
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    else
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    if (please_quit)
    {
        /* stop playing back, quit. */
        SDL_Log("Shutting down.\n");
        SDL_PauseAudioDevice(devid_in, 1);
        SDL_CloseAudioDevice(devid_in);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    //Uint8 buf[BUF_SIZE];
    //Uint32 br;

    if (want_stop)
    {
    	int32_t end_mark = 0;
		send(con->sock, &end_mark, sizeof(end_mark), 0);

	    char server_reply[BUF_SIZE * 2] = {0};

		memset(server_reply, 0, BUF_SIZE * 2);
	    if(recv(con->sock, server_reply, BUF_SIZE * 2, 0) < 0)
	    {
	        perror("recv failed");
	        return (-1);
	    }
	    printf("\nServer reply :\n");
	    printf("%s\n", server_reply);
	    parse_reply(server_reply);
	}

    return 1;
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

    window = SDL_CreateWindow("testaudiocapture", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 240, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    devcount = SDL_GetNumAudioDevices(SDL_TRUE);
    for (i = 0; i < devcount; i++)
        SDL_Log(" Capture device #%d: '%s'\n", i, SDL_GetAudioDeviceName(i, SDL_TRUE));

    SDL_zero(wanted);
    wanted.freq = 16000;
    wanted.format = AUDIO_S16LSB;
    wanted.channels = 1;
    wanted.samples = 4096;
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

    while (42)
    {
        if (loop(con) == 0)
        	break ;
        SDL_Delay(16);
    }
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
