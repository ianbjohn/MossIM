#include <ncurses.h>
#include "moss.h"

#define BACKSPACE 127
#define ENTER 10
#define TL_CORNER 201
#define HOR_EDGE 205
#define TR_CORNER 187
#define VERT_EDGE 186
#define BL_CORNER 200
#define BR_CORNER 188
#define MAX_MESSAGES 30 //how many received messages can be displayed at any given time


//used for testing to make sure client is receiving messages properly
int count = 0;


//Our thread for receiving data
//args is just the socket FD
void* client_read_handler(void* args) {
  //only have to worry about 1 file descriptor currently - the receiving socket
  //once the pipe to receive client-side data is implemented, add epolling
  int sock = (int) args;

  WINDOW* message_win = newwin(MAX_MESSAGES + 2, 100, 0, 0);
  refresh();
  wborder(message_win, '|', '|', '-', '-', '+', '+', '+', '+');
  wrefresh(message_win);

  //have a shifting list of recieved messages
  msg_t received_messages[MAX_MESSAGES];
  int rm_index = 0; //will start out at 0, once it gets to MAX_MESSAGES, stop incrementing

  char msg_buffer[MSG_DATA_MAX_LENGTH]; //used for holding a message's text
  msg_t recv_message; //used to receive data, then copied into the queue

  while (1) {
    //if a message is received, print it
    //TODO: split this and the main thread up into their own files, for better organization.
    if (recv_msg(sock, &recv_message, msg_length) < 0) {
      perror("Error receiving message.");
      exit(1);
    } else {
      count++;
      wclear(message_win);
      wborder(message_win, '|', '|', '-', '-', '+', '+', '+', '+');

      //if more than MAX_MESSAGES messages have been received, shift the list down
      //copy recv_message into the current end of thelist
      if (rm_index == MAX_MESSAGES) {
        for (int i = 1; i < MAX_MESSAGES; i++) {
          memcpy(&received_messages[i - 1], &received_messages[i], msg_length);
        }
        memcpy(&received_messages[MAX_MESSAGES - 1], &recv_message, msg_length);
      } else {
        memcpy(&received_messages[rm_index], &recv_message, msg_length);
        rm_index++;
      }

      for (int i = 0; i < rm_index; i++) {
        //print time
        mvwprintw(message_win, i + 1, 1, "%02d/%02d/%04d %02d:%02d:%02d",
  received_messages[i].time_sent.tm_mon, received_messages[i].time_sent.tm_mday,
  received_messages[i].time_sent.tm_year + 1900, received_messages[i].time_sent.tm_hour,
  received_messages[i].time_sent.tm_min, received_messages[i].time_sent.tm_sec);

        //give the username a color
        wattron(message_win, COLOR_PAIR(received_messages[i].color));
        if (received_messages[i].msg_type == MT_JOIN) {
          mvwprintw(message_win, i + 1, 20, "       %10s",
  received_messages[i].username);
          wattroff(message_win, COLOR_PAIR(received_messages[i].color));
          mvwprintw(message_win, i + 1, 37, " HAS JOINED THE CHAT!");
        } else if (received_messages[i].msg_type == MT_LEAVE) {
          mvwprintw(message_win, i + 1, 20, "       %10s",
  received_messages[i].username);
          wattroff(message_win, COLOR_PAIR(received_messages[i].color));
          mvwprintw(message_win, i + 1, 37, " HAS LEFT THE CHAT.");
        } else {
          mvwprintw(message_win, i + 1, 20, "  %10s: ",
          received_messages[i].username);
          wattroff(message_win, COLOR_PAIR(received_messages[i].color));
          mvwprintw(message_win, i + 1, 34, received_messages[i].data);
        }
      }
      wrefresh(message_win);
    }
  }

  pthread_exit(0);
}


void client() {
  char msg_buffer[MSG_DATA_MAX_LENGTH]; //used for holding a message's text
  msg_t send_message;

  //set up random seed
  srand(time(0));

  printf("Hello.\n");

  //set up TCP socket
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  //ask user for server's IP address
  char server_ip[INET_ADDRSTRLEN];
  printf("Enter Server IP address: ");
  scanf("%s", server_ip);

  //ask for a port as well
  unsigned short port;
  printf("Enter Server port: ");
  scanf("%d", &port);

  //specify address and port
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  inet_pton(AF_INET,  server_ip, &address.sin_addr); //convert the IP string to a number
  //printf("%d\n", address.sin_addr.s_addr); //for testing

  printf("Connecting...\n");

  //connect to server
  if (connect(sock, (struct sockaddr* ) &address, sizeof(address)) < 0) {
    perror("Could not connect to server.");
    exit(1);
  }
  printf("Connected.\n");

  //ask the client for a username
  printf("Please specify a username: ");
  scanf("%s", send_message.username);

  //set up ncurses
  initscr();
  if (has_colors() == 0) {
    endwin();
    printf("Color not supported by terminal.\n");
    exit(1);
  }

  start_color();
  init_pair(0, COLOR_RED, COLOR_BLACK);
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);
  init_pair(3, COLOR_BLUE, COLOR_BLACK);
  init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(5, COLOR_CYAN, COLOR_BLACK);
  init_pair(6, COLOR_WHITE, COLOR_BLACK);

  WINDOW* input_win = newwin(7, 100, (MAX_MESSAGES + 2) + 1, 0);
  refresh();
  wborder(input_win, '|', '|', '-', '-', '+', '+', '+', '+');
  wrefresh(input_win);

  //let the server know that we've joined
  send_message.msg_type = MT_JOIN;
  send_message.color = rand() % 7; //pick a random color to start out with
  time_t timething;
  time(&timething);
  struct tm *timer = localtime(&timething);
  memcpy(&send_message.time_sent, timer, sizeof(struct tm));
  //TODO: Might not actually need the message length in the structure field. Might be okay to just pass it to the send_msg argument.
  send_message.length = msg_length - MSG_DATA_MAX_LENGTH;
  if (send_msg(sock, &send_message, send_message.length) < 0) {
    perror("Error sending message.");
    exit(1);
  }

  //Create the receive handler thread here
  pthread_t rh_thread; //thread for our read handler
  if (pthread_create(&rh_thread, NULL, client_read_handler, (void*) sock) < 0) { //No attributes needed, pass the sock FD to the thread
    perror("Error creating thread.");
    exit(1);
  }

  //Wait for user input to send to the server
  while (1) {
    msg_t recv_message; //used to receive data, then copied into the queue

    //wait until the user hits ENTER
    mvwgetnstr(input_win, 1, 1, msg_buffer, MSG_DATA_MAX_LENGTH);

    int leave = 0;
    int msg_data_length = strlen(msg_buffer) + 1;

    //send message over socket
    if (strcmp(msg_buffer, "!leave") == 0) {
      send_message.msg_type = MT_LEAVE;
      leave = 1;
    } else {
      send_message.msg_type = MT_REG;
      memcpy(send_message.data, msg_buffer, msg_data_length);
    }
    time(&timething);
    timer = localtime(&timething);
    memcpy(&send_message.time_sent, timer, sizeof(struct tm));
    send_message.length = msg_length - (MSG_DATA_MAX_LENGTH - msg_data_length);
    if (send_msg(sock, &send_message, send_message.length) < 0) {
      perror("Error sending message.");
      exit(1);
    }

    if (leave == 1)
      break;

    //clear the input screen
    wclear(input_win);
    wborder(input_win, '|', '|', '-', '-', '+', '+', '+', '+'); //gets erased turing wclear
    wrefresh(input_win);
  }

  //close socket
  close(sock);
  endwin();
  printf("%d\n", count);
  printf("Goodbye.\n");
}
