#include <pthread.h>
#include <sys/epoll.h>
#include "test.h"


//data that the server needs
int client_socks[MAX_CLIENTS];
int active_sockets[MAX_CLIENTS] = {0}; //which sockets are currently active
int epfd_client; //used for polling the client sockets
struct epoll_event handler_event, ready_events[MAX_CLIENTS];
int client_colors[MAX_CLIENTS]; //what color IDs (used in client.c) correspond to eacch client
int sin_size = sizeof(struct sockaddr_in);
int num_clients;
int first_available_client;


//our thread that actually handles the sending/receiving of messages
void* client_handler() {
  //initialize polling
  epfd_client = epoll_create(1);
  handler_event.events = EPOLLIN;

  msg_t recv_message;
  int num_ready_socks;

  while (1) {
    //poll to see which sockets are ready
    num_ready_socks = epoll_wait(epfd_client, ready_events, 1, -1);

    for (int i = 0; i < num_ready_socks; i++) {
      if (recv(ready_events[i].data.fd, &recv_message, msg_size, 0) < 0) {
        perror("recv");
        exit(1);
      }

      if (recv_message.msg_type == MT_JOIN)
        mass_send(&recv_message);
      else if (recv_message.msg_type == MT_LEAVE) {
        close(ready_events[i].data.fd);
        active_sockets[0] = 0; //MAP SOCKETS TO IDS, OR HAVE THE ID IN THE MESSAGE STRUCTURE
        num_clients--;
        first_available_client = find_first_available_client();
        mass_send(&recv_message);
      } else if (recv_message.msg_type == MT_REG) 
        mass_send(&recv_message);
    }
  }

  pthread_exit(0);
}


void mass_send(msg_t* message) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (active_sockets[i] == 1) {
      if (send(client_socks[i], message, msg_size, 0) < 0) {
        printf("Error sending message. (%d)\n", errno);
        exit(1);
      }
    }
  }
}


void server() {
  printf("Hello.\n"); //print version number or something here

  srand(time(0)); //initialize random seed for PRNG

  //create TCP socket
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  //ask the user for a port to bind to
  unsigned short port;
  printf("Please enter a port: ");
  scanf("%d", &port);

  //specity address and host
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htonl(INADDR_ANY);

  //bind socket
  if (bind(sock, (struct sockaddr* ) &address, sin_size) < 0) {
    printf("Error binding socket. (%d)\n", errno);
    exit(1);
  }
  //char server_ip[INET_ADDRSTRLEN]; //used for testing to confirm the bound IP address
  //inet_ntop(AF_INET, &address.sin_addr, server_ip, INET_ADDRSTRLEN);
  printf("Bound.\n");

  //listen for incoming connections
  listen(sock, MAX_CLIENTS);

  //initialize server data
  num_clients = 0;
  first_available_client = 0;

  printf("Waiting for connections...\n");

  //set up client handler thread
  pthread_t client_thread;
  pthread_attr_t client_thread_attrib;
  pthread_attr_init(&client_thread_attrib);
  pthread_create(&client_thread, &client_thread_attrib, client_handler, NULL);

  while (1) {
    //check and see if any new clients have joined the server
    struct sockaddr_in client_addr;
    char address_string[INET_ADDRSTRLEN];

    int temp_client_sock = accept(sock, (struct sockaddr* ) &client_addr, &sin_size);
    if (temp_client_sock < 0) {
      if (errno == ENFILE) {
        //no more connections are available
        inet_ntop(AF_INET, &client_addr.sin_addr, address_string, INET_ADDRSTRLEN);
        printf("%s tried to join. Max # of clients already connected.\n", address);
        continue;
      }
    }

    inet_ntop(AF_INET, &client_addr.sin_addr, address_string, INET_ADDRSTRLEN); //convert to IPv4 dot format
    printf("New connection from %s!\n", address_string);
    first_available_client = find_first_available_client();
    active_sockets[first_available_client] = 1; //mark as active
    client_socks[first_available_client] = temp_client_sock;
    handler_event.events = EPOLLIN;
    handler_event.data.fd = temp_client_sock;
    if (epoll_ctl(epfd_client, EPOLL_CTL_ADD, temp_client_sock, &handler_event) < 0) {
      perror("epoll_ctl");
      exit(1);
    }
    num_clients++;
  }

  close_client_sockets();
  close(sock);
}

void close_client_socks() {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (active_sockets[i]) {
      close(client_socks[i]);
    }
  }
}

int find_first_available_client() {
  //linear searches the list of clients for the first free one. Return -1 if none are free
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (active_sockets[i] == 0) {
      return i;
    }
  }
  return -1;
}
