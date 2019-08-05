#include <pthread.h>
#include "test.h"


//data that the server needs
int client_socks[MAX_CLIENTS];
struct sockaddr_in client_addresses[MAX_CLIENTS];
int active_sockets[MAX_CLIENTS] = {0}; //which sockets are currently active
int client_colors[MAX_CLIENTS]; //what color IDs (used in client.c) correspond to eacch client
int sin_size = sizeof(struct sockaddr_in);
int num_clients;
int first_available_client;

//used for passing data to a thread
typedef struct {
  int id;
  int sock;
  struct sockaddr_in sin_addr;
} chandler_t;


void* client_handler(void* arg) {
  //our thread that actually handles sending/receiving of messages
  //pass the socket and sockaddr_in as arguments
  
  msg_t recv_message;
  int left = 0;

  while (left == 0) {
    if (recv(((chandler_t* ) arg)->sock, &recv_message, msg_size, MSG_DONTWAIT) == -1) {
      if (errno != EWOULDBLOCK) {
        //something happened besides no data being sent over the socket
        perror("Error receiving message. (%d)", errno);
        exit(1);
      }
    } else {
      recv_message.color = client_colors[((chandler_t* ) arg)->id];
      if (recv_message.msg_type == MT_JOIN) {
        mass_send(&recv_message);
      } else if (recv_message.msg_type == MT_LEAVE) {
        close(((chandler_t* ) arg)->sock);
        client_socks[((chandler_t* ) arg)->id] = 0;
        active_sockets[((chandler_t* ) arg)->id] = 0;
        num_clients--;
        first_available_client = find_first_available_client();
        mass_send(&recv_message);
        left = 1;
      } else if (recv_message.msg_type == MT_REG) {
        mass_send(&recv_message);
      }
    }
  }

  pthread_exit(0);
}


void mass_send(msg_t* message) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (active_sockets[i] == 1) {
      if (send(client_socks[i], message, msg_size, MSG_DONTWAIT) < 0) {
        if (errno != EWOULDBLOCK) {
          perror("send");
        }
      }
    }
  }
}


void server() {
  printf("Hello.\n");

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
  char server_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &address.sin_addr, server_ip, INET_ADDRSTRLEN);
  printf("Bound.");

  //listen for incoming connections
  listen(sock, MAX_CLIENTS);

  //initialize server data
  num_clients = 0;
  first_available_client = 0;
  chandler_t client_handler_args[MAX_CLIENTS];

  printf("Waiting for connections...\n");

  //set up multithreading
  pthread_t thread_ids[MAX_CLIENTS];
  pthread_attr_t thread_attribs[MAX_CLIENTS];

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
    client_colors[first_available_client] = rand() % 7; //generate a random number 0-6 corresponding to a client color
    //set up new thread
    client_handler_args[first_available_client].id = first_available_client;
    client_handler_args[first_available_client].sock = client_socks[first_available_client];
    client_handler_args[first_available_client].sin_addr = client_addresses[first_available_client];
    pthread_attr_init(&thread_attribs[first_available_client]);
    pthread_create(&thread_ids[first_available_client], &thread_attribs[first_available_client],
          client_handler, &client_handler_args[first_available_client]);
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
