#include "moss.h"

int msg_length = sizeof(msg_t);

//high-level send and receive functions
int send_msg(int sock, void* buffer, int buf_size) {
  int count = 0;

  while (count < buf_size) {
    int s = send(sock, buffer + count, buf_size - count, 0);
    if (s < 0) return -1;
    count += s;
  }

  return 0;
}

int recv_msg(int sock, void* buffer, int buf_size) {
  //receive the first packet - the length field will tell us how many more bytes to wait for
  int count = recv(sock, buffer, buf_size, 0);
  int length = ((int* ) buffer)[0]; //extract the total length of the message
  length -= count; //subtract what we just got

  while (1) {
    int r = recv(sock, buffer + count, buf_size, 0);
    if (r < 0) return -1;
    if (r == 0) break;
    count += r;
  }

  return 0;
}
