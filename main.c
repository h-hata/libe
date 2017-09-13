/* =========================================================
 Name        : main.c
 Author      : H.Hata
 Version     :
 Copyright   : OLT
 Description : Ansi-style
========================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "hsvr.h"
static void do_accept(SESSION *s)
{
	S_settimer(s,5);
}
static void response(SESSION *s,int len)
{
	char *buff;
	int sz=len+512;
	int slen,hlen;
	int ret;
	buff=malloc((size_t)sz);
	memset(buff,0,(size_t)sz);
	sprintf(buff,
"HTTP/1.1 200 OK\r\n"
"Content-Length: %d\r\n" 
"Connection: Close\r\n"
"Content-Type: application/octet-stream\r\n"
"\r\n",len);
	hlen=(int)strlen(buff);
	slen=hlen+len;
	s=S_search(s->host,s->port);
	ret=S_send(s,(unsigned char *)buff,(size_t)slen);
	free(buff);
	S_close(s);
	if(ret!=0)
		printf("ret=%d\n",ret);
}
static void do_read(SESSION *s)
{
#ifdef HTTP
	if(s->blen>50) response(s,1000000);
#else
	printf("%s\n",s->buff);
	if(s->blen>0){
		s->buff[0]='!';
		S_send(s,s->buff,s->blen);
	}
#endif
}
static void do_timeout(SESSION *s)
{
	printf("TIMEOUT %d \n",s->id);
	S_close(s);
}
static void do_term(SESSION *s)
{
	printf("TERM %d\n",s->id);
}
static void callback(CTYPE type,SESSION *s)
{
	switch(type){
	case TYP_ACCEPT:
		do_accept(s);
		break;
	case TYP_READ:
		do_read(s);
		break;
	case TYP_TIMEOUT:
		do_timeout(s);
		break;
	case TYP_TERM:
		do_term(s);
		break;
	default:
		break;
	}
}
static void daemonize(void)
{
	int ret;
	//1回目
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);
	ret=fork();
	if(ret>0){
		//親プロセス
		exit(EXIT_SUCCESS);
	}else if(ret<0){
		exit(1);
	}
	//2回目
	ret=fork();
	if(ret>0){
		//親プロセス
		exit(EXIT_SUCCESS);
	}else if(ret<0){
		exit(1);
	}
}
int main(int argc,char **argv)
{
	uint16_t port=12345;
	int daemon=0;
	int multi=4;
	int limit=1000;
	int tls=0;
	char *priv="pem/server.key";
	char *cert="pem/vng1_256.crt";
	printf("Start");
	if(daemon==1){
		daemonize();
	}
	S_start(port,limit,multi,callback,tls,cert,priv);
	return EXIT_SUCCESS;
}
