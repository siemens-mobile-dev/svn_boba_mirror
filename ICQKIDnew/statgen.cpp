#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <string.h>
#include "statgen.h"

extern unsigned short ClientsCount;
extern char *StatOutput_Path;

void UpdateStatPage()
{
	if(!StatOutput_Path)return;

	FILE *html = fopen("stat_template.txt","r");
	if(!html)
	{
		printf("\nCannot open template, exiting...\n");
		return;
	}
	fseek(html,0,SEEK_END);
	int fs = ftell(html);
	fseek(html,0,SEEK_SET);
	char *htm_unparsed = (char*) malloc(fs+1);
	memset(htm_unparsed, 0, fs+1);
	fread(htm_unparsed, fs, 1, html);
	fclose(html);
	int bc;
	char *htm_parsed = (char*) malloc(bc = fs+1+16);
	memset(htm_parsed, 0, bc);
	sprintf(htm_parsed, htm_unparsed, ClientsCount);
	//printf(htm_parsed);
	int parsed_len = strlen(htm_parsed);
	
	FILE *html_out =  fopen(StatOutput_Path, "w");
	fwrite(htm_parsed, parsed_len, 1, html_out);
	fclose(html_out);
	free(htm_parsed);
	free(htm_unparsed);
}
