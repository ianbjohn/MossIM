#ifndef MOSS_H
#define MOSS_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CLIENTS 16
#define MSG_LENGTH 256
#define UNAME_LENGTH 10
#define MT_REG 0
#define MT_JOIN 1
#define MT_LEAVE 2

typedef struct {
  int msg_type; //0 - regular message, 1 - join, 2 - leave
  char username[UNAME_LENGTH];
  int color; //what color the username should be
  struct tm time_sent;
  char data[MSG_LENGTH];
} msg_t;

void client();
void* client_handler();
void server();
void close_client_sockets();
void mass_send(msg_t* message);
int find_first_available_client();

extern int msg_size;//differnet from MSG_LENGTH, which is how big the payload of the message can be

#endif
