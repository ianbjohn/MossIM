#include "moss.h"
#include "bbst.h"


//data that the server needs
bbst_t* active_sockets;
int client_colors[MAX_CLIENTS]; //what color IDs (used in client.c) correspond to each client
int sin_size = sizeof(struct sockaddr_in);
int num_clients;
int first_available_client;


void client_handler(int sock) {
  msg_t recv_message;

  if (recv(sock, &recv_message, msg_length, 0) < 0) {
    perror("Error receiving message.");
    exit(1);
  }

  if (recv_message.msg_type == MT_JOIN)
    mass_send(&recv_message);
  else if (recv_message.msg_type == MT_LEAVE) {
    close(sock);
    bbst_remove(active_sockets, sock);
    num_clients--;
    mass_send(&recv_message);
  } else if (recv_message.msg_type == MT_REG)
    mass_send(&recv_message);
}


void mass_send(msg_t* message) {
  //(traverse the tree in order, send the message to each client)
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
    perror("Error binding socket.");
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

  //set up epoll - add the listening socket
  int epfd = epoll_create(MAX_CLIENTS + 1);
  if (epfd < 0) {
    perror("epoll_create");
    exit(1);
  }
  struct epoll_event handler_event, ready_events[MAX_CLIENTS + 1];
  handler_event.events = EPOLLIN;
  handler_event.data.fd = sock;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &handler_event) < 0) {
    perror("epoll_ctl");
    exit(1);
  }

  printf("Waiting for connections...\n");

  while (1) {
    //poll to see which sockets are ready
    int num_ready_socks = epoll_wait(epfd, ready_events, 1, -1);
    if (num_ready_socks < 0) {
      perror("epoll_wait");
      exit(1);
    }

    for (int i = 0; i < num_ready_socks; i++) {
      if (ready_events[i].data.fd == sock) {
        //new client has joined the server
        //maybe split this up into its own function as well.
        struct sockaddr_in client_addr;
        char address_string[INET_ADDRSTRLEN];

        int temp_client_sock = accept(sock, (struct sockaddr* ) &client_addr, &sin_size);
        inet_ntop(AF_INET, &client_addr.sin_addr, address_string, INET_ADDRSTRLEN); //convert to IPv4 dot format
        if (temp_client_sock < 0) {
          if (errno == ENFILE) {
            //no more connections are available
            printf("%s tried to join. Max # of clients already connected.\n", address_string);
            continue;
          }
        }

        printf("New connection from %s!\n", address_string);
        bbst_add(active_sockets, temp_client_sock);
        num_clients++;
        handler_event.events = EPOLLIN;
        handler_event.data.fd = temp_client_sock;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, temp_client_sock, &handler_event) < 0) {
          perror("epoll_ctl");
          exit(1);
        }
      } else {
        //data received from an already-connected client
        //(set up a thread to process the data)
        client_handler(ready_events[i].data.fd);
      }
    }
  }

  close_client_sockets();
  close(sock);
}

void close_client_socks() {
  //delete each member of the tree
  //(in-order traversal, close the socket and delete the node)
}
