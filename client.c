#include <ncurses.h>
#include "test.h"

#define BACKSPACE 127
#define ENTER 10
#define TL_CORNER 201
#define HOR_EDGE 205
#define TR_CORNER 187
#define VERT_EDGE 186
#define BL_CORNER 200
#define BR_CORNER 188
#define MAX_MESSAGES 30 //how many received messages can be displayed at any given time


void client() {
  char msg_buffer[MSG_LENGTH]; //used for holding a message's text
  int msg_buffer_pos = 0;
  msg_t send_message, recv_message;

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

  //connect to server
  if (connect(sock, (struct sockaddr* ) &address, sizeof(address)) < 0) {
    printf("Could not connect to server. (%d)\n", errno);
    exit(1);
  }
  printf("Connected.\n");

  //ask the client for a username
  printf("Please specify a username: ");
  scanf("%s", send_message.username);

  //set up ncurses
  initscr();
  nodelay(stdscr, 1); //don't wait for user input
  noecho(); //don't echo input characters
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

  WINDOW* message_win = newwin(MAX_MESSAGES + 2, 100, 0, 0);
  WINDOW* input_win = newwin(7, 100, (MAX_MESSAGES + 2) + 1, 0);
  refresh();
  //wborder(message_win, VERT_EDGE, VERT_EDGE, HOR_EDGE, HOR_EDGE, TL_CORNER, TR_CORNER, BL_CORNER, BR_CORNER);
  //wborder(input_win, VERT_EDGE, VERT_EDGE, HOR_EDGE, HOR_EDGE, TL_CORNER, TR_CORNER, BL_CORNER, BR_CORNER);
  wborder(message_win, '|', '|', '-', '-', '+', '+', '+', '+');
  wborder(input_win, '|', '|', '-', '-', '+', '+', '+', '+');
  wrefresh(message_win);
  wrefresh(input_win);

  //let the server know that we've joined
  send_message.msg_type = MT_JOIN;
  send_message.color = rand() % 7; //pick a random color to start out with
  time_t timething;
  time(&timething);
  struct tm *timer = localtime(&timething);
  memcpy(&send_message.time_sent, timer, sizeof(struct tm));
  if (send(sock, &send_message, msg_size, 0) < 0) {
    printf("Error sending message. (%d)", errno);
    exit(1);
  }

  //have a shifting list of recieved messages
  msg_t received_messages[MAX_MESSAGES];
  int rm_index = 0; //will start out at 0, once it gets to 9, stop incrementing

  //do stuff
  while (1) {
    int enter_pressed = 0;
    int c;
    msg_t recv_message; //used to receive data, then copied into the queue

    msg_buffer[msg_buffer_pos] == '\0'; //refresh and print user input every frame, since automatic echo is disabled. (Probably a better way to do this.)
    mvwprintw(input_win, 1, 1, "%s", msg_buffer);
    wmove(input_win, 1, msg_buffer_pos + 1);
    wrefresh(input_win);

    //when the user types, add it to dthe buffer.
    if ((c = getch()) != ERR) {
      if (c == ENTER && msg_buffer_pos > 0) {
        int leave = 0;
        msg_buffer[msg_buffer_pos] = '\0';
        //send message over socket
        memcpy(send_message.data, msg_buffer, strlen(msg_buffer) + 1);
        if (strcmp(msg_buffer, "!leave") == 0) {
          send_message.msg_type = MT_LEAVE;
          leave = 1;
        } else {
          send_message.msg_type = MT_REG;
        }
        time(&timething);
        timer = localtime(&timething);
        memcpy(&send_message.time_sent, timer, sizeof(struct tm));
        send(sock, &send_message, msg_size, 0);
        if (leave == 1) {
          break;
        }
        //clear the buffer
        memset(msg_buffer, 0, MSG_LENGTH * sizeof(char));
        msg_buffer_pos = 0;
        enter_pressed = 1;
        wclear(input_win);
        wborder(input_win, '|', '|', '-', '-', '+', '+', '+', '+'); //gets erased turing wclear
      } else if (c == BACKSPACE && msg_buffer_pos > 0) {
        msg_buffer[--msg_buffer_pos] = '\0';
        wclear(input_win); //will be refreshed next tick to undisplay the backspaced character
        wborder(input_win, '|', '|', '-', '-', '+', '+', '+', '+');
      } else if (c != BACKSPACE && msg_buffer_pos < MSG_LENGTH - 1) {
        msg_buffer[msg_buffer_pos] = c;
        msg_buffer[++msg_buffer_pos] = '\0';
      }
    }

    //if message received, print it
    //eventually, add polling on this too. Have the main thread be the bottom window waiting for input (without delay)
      //and the top window be its own thread, polling the receiving socket.
    //That way, there's no busy waiting, and the window doesn't have to be re-drawn every single frame unnecessarily.
    //Might require some global / shared memory, but I think it'll be worth it.
    //Also, split the two functions up into their own files, for better organization.
    if (recv(sock, &recv_message, msg_size, MSG_DONTWAIT) < 0) {
      if (errno != EWOULDBLOCK) {
        printf("Error receiving message. (%d)\n", errno);
        exit(1);
      }
    } else {
      wclear(message_win);
      wborder(message_win, '|', '|', '-', '-', '+', '+', '+', '+');

      //if more than MAX_MESSAGES messages have been received, shift the list down
      //copy recv_message into the current end of thelist
      if (rm_index == MAX_MESSAGES) {
        for (int i = 1; i < MAX_MESSAGES; i++) {
          memcpy(&received_messages[i - 1], &received_messages[i], msg_size);
        }
        memcpy(&received_messages[MAX_MESSAGES - 1], &recv_message, msg_size);
      } else {
        memcpy(&received_messages[rm_index], &recv_message, msg_size);
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

  //close socket
  close(sock);
  endwin();
  printf("Goodbye.\n");
}
