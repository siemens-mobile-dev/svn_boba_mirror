// Stat_Generator.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#ifndef __FreeBSD__
 #include <malloc.h>
#else
 #include <stdlib.h>
#endif
#include <memory.h>
#include <errno.h>
#include <string.h>

#define BLOCK_SIZE 5

void Parse_HTML(char *Clients_Num)
{
	FILE *html = fopen("template.txt","r");
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
	char *htm_parsed = (char*) malloc(bc = fs+1+5); // ¬р€д ли будет больше п€тизначного числа
	memset(htm_parsed, 0, bc);
	sprintf(htm_parsed, htm_unparsed, Clients_Num);
	printf(htm_parsed);
	free(htm_parsed);
	free(htm_unparsed);
}


int main(int argc, char* argv[])
{
	char fname[]="naticq_log.txt";
	FILE *hF=fopen(fname, "rb+");
	if(!hF)return -1;
	if(fseek(hF,-5,SEEK_END ))printf("Error %d", errno);
	void *buf=malloc(BLOCK_SIZE*2);
	memset(buf, 0, BLOCK_SIZE*2);
	if(!fread(buf,BLOCK_SIZE,1,hF))
	{
		free(buf);
		fclose(hF);
		return -2;
	}
	//printf("Data=%s\n",buf);
	// ѕолучили строчку, в которой есть число коннектов, ищем ":"
	int i;
	char *cc;
	for(i=0;i<BLOCK_SIZE;i++)
	{
		cc =(char*)buf+i;
		if(*cc==':')break;
	}
	char *num = (char*)malloc(10);
	strcpy(num, cc+2);
	//printf("C=_%s_",num);
	free(buf);
	
	// “еперь открываем шаблон HTML и в него записываем то что нужно
	Parse_HTML(num);
	free(num);
	fclose(hF);
	return 0;
}
