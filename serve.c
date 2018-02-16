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
    new_sock = malloc(1);
    *new_sock = client_sock;

    if( pthread_create( &sniffer_thread , NULL ,  lobby , (void*) new_sock) < 0)
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




void *lobby(void *socket_desc) {
  //Get the socket descriptor
  int sock = *(int*)socket_desc;
  int read_size;
  char *message, client_message[200];

  if (read_size == recv(sock , client_message , 2000 , 0) < 0) {
    puts("Error while getting the pseudo");
  }
  printf("Player %s connected to the game\n", client_message);
  message = malloc(sizeof(char) * strlen("Bienvenue sur CJanken !") + 200);
  strcpy(message, "Bienvenue sur CJanken !");
  strcat(message, client_message);
  write(sock , message , strlen(message));
  message = "1: Join Room (soon free places)\n2: create Room\n3: Exit";
  write(sock, message, strlen(message));
  //Receive a message from client
  while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
  {
    //   //Send the message back to client
      puts(client_message);
    //   // write(sock , client_message , strlen(client_message));
  }

  if(read_size == 0)
  {
    puts("Client disconnected");
    fflush(stdout);
  }
  else if(read_size == -1)
  {
    perror("recv failed");
  }


  free(socket_desc);
  return 0;
}

void *createRoom(void *socket_desc){
  int sock = *(int*)socket_desc;
  pthread_t sniffer_thread;

  free(socket_desc);
}
