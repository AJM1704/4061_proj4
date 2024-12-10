#include "../include/utils.h"
#include "../include/server.h"
#include <arpa/inet.h> // inet_addr()
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h> // read(), write(), close()
#include <unistd.h>

// TODO: Declare a global variable to hold the file descriptor for the server
// socket
int master_fd;
// TODO: Declare a global variable to hold the mutex lock for the server socket
pthread_mutex_t server_socket_mutex = PTHREAD_MUTEX_INITIALIZER;
// TODO: Declare a gloabl socket address struct to hold the address of the
// server
struct sockaddr_in global_serv_addr;

/*
################################################
##############Server Functions##################
################################################
*/

/**********************************************
 * init
   - port is the number of the port you want the server to be
     started on
   - initializes the connection acception/handling system
   - if init encounters any errors, it will call exit().
************************************************/
void init(int port) {
  printf("called init\n");
  // TODO: create an int to hold the socket file descriptor
  int server_fd;
  // TODO: create a sockaddr_in struct to hold the address of the server
  struct sockaddr_in serv_addr;

  /**********************************************
   * IMPORTANT!
   * ALL TODOS FOR THIS FUNCTION MUST BE COMPLETED FOR THE INTERIM
   *SUBMISSION!!!!
   **********************************************/

  // TODO: Create a socket and save the file descriptor to sd (declared above)
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == 0) {
    perror("Socket failed");
    exit(EXIT_FAILURE);
  }
  // TODO: Change the socket options to be reusable using setsockopt().
  int enable = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) <
      0) {
    perror("setsockopt failed");
    exit(EXIT_FAILURE);
  }

  // Define the address
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);

  // TODO: Bind the socket to the provided port.
  if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }

  // TODO: Mark the socket as a pasive socket. (ie: a socket that will be used
  // to receive connections)

  if (listen(server_fd, SOMAXCONN) < 0) {
    perror("Listen failed");
    exit(EXIT_FAILURE);
  }

  // We save the file descriptor to a global variable so that we can use it in
  // accept_connection().
  // TODO: Save the file descriptor to the global variable master_fd
  master_fd = server_fd;
  global_serv_addr = serv_addr;

  printf("UTILS.O: Server Started on Port %d with IP %s\n", port, inet_ntoa(global_serv_addr.sin_addr));
  fflush(stdout);
  printf("exited init\n");
}

/**********************************************
 * accept_connection - takes no parameters
   - returns a file descriptor for further request processing.
   - if the return value is negative, the thread calling
     accept_connection must should ignore request.
***********************************************/

int accept_connection(void) {
  printf("called accept conn\n");
  // TODO: create a sockaddr_in struct to hold the address of the new connection
  struct sockaddr_in client_addr;
  socklen_t addr_len = sizeof(client_addr);

  /**********************************************
   * IMPORTANT!
   * ALL TODOS FOR THIS FUNCTION MUST BE COMPLETED FOR THE INTERIM
   *SUBMISSION!!!!
   **********************************************/

  // TODO: Aquire the mutex lock
  pthread_mutex_lock(&server_socket_mutex);
  // TODO: Accept a new connection on the passive socket and save the fd to
  // newsock
  int new_sock_fd =
      accept(master_fd, (struct sockaddr *)&client_addr, &addr_len);
  if (new_sock_fd < 0) {
    perror("Accept failed");
    pthread_mutex_unlock(&server_socket_mutex);
    return -1;
  }
  // TODO: Release the mutex lock
  pthread_mutex_unlock(&server_socket_mutex);

  // TODO: Return the file descriptor for the new client connection
  return new_sock_fd;
  printf("exited accept conn\n");
}

/**********************************************
 * send_file_to_client
   - socket is the file descriptor for the socket
   - buffer is the file data you want to send
   - size is the size of the file you want to send
   - returns 0 on success, -1 on failure
************************************************/
int send_file_to_client(int socket, char *buffer, int size) {
  printf("called send fiel\n");
  // TODO: create a packet_t to hold the packet data
  packet_t packet = {.size = size};

  // TODO: send the file size packet

  if (send(socket, &packet, sizeof(packet), 0) < 0) {
    printf("Failed to send filesize packet!");
    return -1;
  };

  // TODO: send the file data

  if (send(socket, &buffer, size, 0) < 0) {
    printf("Failed to send filesize packet!");
    return -1;
  };

  // TODO: return 0 on success, -1 on failure

  printf("exited sftc\n");
  return 0;
}

/**********************************************
 * get_request_server
   - fd is the file descriptor for the socket
   - filelength is a pointer to a size_t variable that will be set to the length
of the file
   - returns a pointer to the file data
************************************************/
char *get_request_server(int fd, size_t *filelength) {
  printf("called getreqserv\n");
  // TODO: create a packet_t to hold the packet data
  packet_t packet;

  // TODO: receive the response packet
  if (recv(fd, &packet, sizeof(packet), 0) < 0) {
    printf("Failed to recieve filesize packet!");
  };

  // TODO: get the size of the image from the packet
  int img_size = packet.size;
  char *image_buffer = malloc(img_size * sizeof(char));

  // TODO: recieve the file data and save into a buffer variable.
  if (recv(fd, &image_buffer, img_size, 0) < 0) {
    printf("Failed to recieve image data!");
  };

  // TODO: return the buffer
  printf("exited grs\n");
  return image_buffer;
}

/*
################################################
##############Client Functions##################
################################################
*/

/**********************************************
 * setup_connection
   - port is the number of the port you want the client to connect to
   - initializes the connection to the server
   - if setup_connection encounters any errors, it will call exit().
************************************************/
int setup_connection(int port) {
  printf("called setconn\n");
  // TODO: create a sockaddr_in struct to hold the address of the server
  struct sockaddr_in sockaddr;

  // TODO: create a socket and save the file descriptor to sockfd
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    printf("Error making socket");
    return -1;
  }

  // TODO: assign IP, PORT to the sockaddr_in struct
  sockaddr.sin_port = htons(port);
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr = global_serv_addr.sin_addr;

  // TODO: connect to the server

  if (connect(fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
    printf("Error connecting to server!");
    return -1;
  }

  printf("exited setconn\n");
  return fd;
  // TODO: return the file descriptor for the socket
}

/**********************************************
 * send_file_to_server
   - socket is the file descriptor for the socket
   - file is the file pointer to the file you want to send
   - size is the size of the file you want to send
   - returns 0 on success, -1 on failure
************************************************/
int send_file_to_server(int socket, FILE *file, int size) {
  printf("called sfts\n");
  // TODO: send the file size packet

  packet_t size_packet = {.size = size};
  if (send(socket, &size_packet, sizeof(size_packet), 0) < 0) {
    printf("Error sending size packet");
    return -1;
  }

  // TODO: send the file data

  // TODO: return 0 on success, -1 on failure
  printf("exited sfts\n");
  return 0;
}

/**********************************************
 * receive_file_from_server
   - socket is the file descriptor for the socket
   - filename is the name of the file you want to save
   - returns 0 on success, -1 on failure
************************************************/
int receive_file_from_server(int socket, const char *filename) {
  printf("called rffs\n");
  // TODO: create a buffer to hold the file data
  char *buf;

  // TODO: open the file for writing binary data
  FILE *fp = fopen(filename, "wb");

  // TODO: create a packet_t to hold the packet data
  packet_t packet;

  // TODO: receive the response packet
  if (recv(socket, &packet, sizeof(packet), 0) < 0) {
      printf("Error recieving packet size from server");
      return -1;
    };

  // TODO: get the size of the image from the packet
  int img_size = packet.size;
  buf = malloc(img_size);

  // TODO: recieve the file data and write it to the file
  if (recv(socket, &buf, img_size, 0) < 0) {
    printf("error recieving image data from server");
    return -1;
  }

  if (fwrite(buf, sizeof(char), img_size, fp) < 0) {
    printf("error writing image data to file");
    return -1;
  };

  // TODO: return 0 on success, -1 on failure
  printf("exited rfts\n");
  return 0;
}
