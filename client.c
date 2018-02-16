/*
C ECHO client example using sockets
*/

// gcc -o s serve.c && gcc -o c client.c
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <stdlib.h>

//Global sock
int sock;

int main(int argc , char *argv[])
{
  system("clear");
  struct sockaddr_in server;
  char message[1000] , server_reply[2000];

  //Create socket
  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1)
  {
    printf("Could not create socket");
  }
  puts("Socket created");

  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(argv[1]));

  //Connect to remote server
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
  {
    perror("connect failed. Error");
    return 1;
  }

  printf("Connected...!\n" );

  while (1) {
    if (recv(sock , server_reply , sizeof(server_reply), 0) < 0) {
      printf("Receive failed");
    }
    printf("%s\n", server_reply);
  }
  close(sock);
  return 0;
}
