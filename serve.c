/*
C socket server example, handles multiple clients using threads
*/

#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

/*
roue-> skate-> velo-> voiture-> avion-> fusée
1) Demander le pseudo de l'utilisateur (handle multithread)
3) Affichage des rooms + demande de creation d'une room dans cet affichage
2) Creation d'une room -> les joueurs rejoignent un thread commun
4)
*/

void *connection_handler(void *);
void *lobby(void *);
void *createRoom(void *);
void *printMenu();


int main(int argc , char *argv[])
{

  system("clear");
  int socket_desc , client_sock , c , *new_sock;
  struct sockaddr_in server , client;

  //Create socket
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1)
  {
    printf("Could not create socket");
  }
  puts("Socket created");

  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( 8888 );

  //Bind
  if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
  {
    //print the error message
    perror("bind failed. Error");
    return 1;
  }
  puts("bind done");

  //Listen
  listen(socket_desc , 3);

  //Accept and incoming connection
  puts("Waiting for incoming connections...");
  c = sizeof(struct sockaddr_in);
  while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
  {
    puts("Connection accepted");
    pthread_t sniffer_thread;

    if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) client_sock) < 0)
    {
      perror("could not create thread");
      return 1;
    }
  }

  if (client_sock < 0)
  {
    perror("accept failed");
    return 1;
  }
  return 0;
}

void *connection_handler(void *client)
{
  char Fullmsg[4000],client_message[2000],intro[255];
  strcat(intro,"Welcome to CSJANKEN\n");
  write(client , intro , strlen(intro));
  
}


void *game(void *client)
{
  char Fullmsg[4000],client_message[2000],intro[255];
  strcat(intro,"Welcome to CSJANKEN\n");
  write(client , intro , strlen(intro));

}
