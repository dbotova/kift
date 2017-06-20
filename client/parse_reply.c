/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_reply.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbotova <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/18 16:07:55 by dbotova           #+#    #+#             */
/*   Updated: 2017/06/18 16:12:26 by dbotova          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kift_client.h"

static void	prepare_google_term(char *term)
{
	while (*term)
	{
		if (*term == ' ')
			*term = '+';
		term++;
	}
}

int			parse_reply(char *hyp)
{
	int		result;
	char	command[BUF_SIZE];

	result = 0;
	SDL_PauseAudioDevice(g_devid_in, SDL_TRUE);
	if (strstr(hyp, "OKAY KIFT") || strstr(hyp, "PREEV’AT KIFT"))
		system("../../SAM/sam Yes master");
	else if (strstr(hyp, "OPEN SAFARI") && !strstr(hyp, "GOOGLE"))
		system("/Applications/Safari.app/Contents/MacOS/Safari & sleep 1");


	else if (strstr(hyp, "SET AN ALARM") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam beep... beep... beep");
	else if (strstr(hyp, "SEND EMAIL") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam Call the mail man");
	else if (strstr(hyp, "SEND SMS") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam Do I lool like your smart phone?");
	else if (strstr(hyp, "CHECK WEATHER") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam The weather is fine for me");
	else if (strstr(hyp, "CHECK EVENTS") && !strstr(hyp, "GOOGLE"))
	{
		system("../../SAM/sam I can help you to get out here");
		system("open https://www.google.com/search?q=events+near+me&ie=utf-8&oe=utf-8");
	}
	else if (strstr(hyp, "CHECK TRAFFIC") && !strstr(hyp, "GOOGLE"))
	{
		system("../../SAM/sam Are we going for a ride?");
		system("open https://www.google.com/search?q=events+near+me&ie=utf-8&oe=utf-8");
	}
	else if (strstr(hyp, "WHO IS CONNECTED?") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam It is only me... I feel lonely... Everyone needs friends");
	else if (strstr(hyp, "PLAY MUSIC") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam La... la la... tu-tu-tu... la! ... Is it good?");
	else if (strstr(hyp, "SEARCH THE WEB FOR") && !strstr(hyp, "GOOGLE"))
	{
		system("../../SAM/sam Let me search it for you");
		prepare_google_term(strstr(hyp, "SEARCH THE WEB FOR") + 18);
		sprintf(command, "%s%s%s", "open ", "https://www.google.com/#q=", strstr(hyp, "SEARCH THE WEB FOR") + 18);
		system(command);
	}
	else if (strstr(hyp, "CHECK HISTORY") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam Thank you but no... Not today");


	else if (strstr(hyp, "WHO YOU ARE?") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam I am a  robot. Your  KIFT slave");
	else if (strstr(hyp, "PLAY \"MASTER OF PUPPETS\"") && !strstr(hyp, "GOOGLE"))
		system("open https://www.youtube.com/watch?v=S7blkui3nQc");
	else if (strstr(hyp, "TELL ME A STORY") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam You were  to  busy  to tell  me any");
	else if (strstr(hyp, "STUPID") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam That was mean");
	else if (strstr(hyp, "HA-HA-HA") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam What is so funny?");
	else if (strstr(hyp, "STOP IT") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam I am sorry");
	else if (strstr(hyp, "DO YOU LOVE ME?") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam It is a ... bad place to look for a love");
	else if (strstr(hyp, "FIND ME COFFEE") && !strstr(hyp, "GOOGLE"))
	{
		system("../../SAM/sam Check the map");
		system("open https://www.google.com/search?q=starbucks+near+me&ie=utf-8&oe=utf-8");
	}
	else if (strstr(hyp, "SET TIMER") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam 3 ... 2 ... 1 ... BOOM!");
	else if (strstr(hyp, "TURN OFF LIGHT") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam Close your eyes ... Do you see the darckness?");
	else if (strstr(hyp, "TURN ON LIGHT") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam Open your eyes ... Did it work?");
	else if (strstr(hyp, "WHERE CAN I GET BUBBLE TEA?") && !strstr(hyp, "GOOGLE"))
		system("../../SAM/sam Bubble tea has to much sweets... Stay away from it!");
	else if (strstr(hyp, "GOOGLE"))
	{
		system("../../SAM/sam Let me google it for you");
		prepare_google_term(strstr(hyp, "GOOGLE") + 7);
		sprintf(command, "%s%s%s", "open ", "https://www.google.com/#q=", strstr(hyp, "GOOGLE") + 7);
		system(command);
	}
	else if (strstr(hyp, "SHUTDOWN") && !strstr(hyp, "GOOGLE"))
	{
		system("../../SAM/sam OKaey master");
		result = 1;
	}
	SDL_PauseAudioDevice(g_devid_in, SDL_FALSE);
	return (result);
}
