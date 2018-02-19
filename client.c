/*
C ECHO client example using sockets
*/

// gcc -o s serve.c && gcc -o c client.c
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <stdlib.h>
#include<pthread.h>

//Global sock
int sock;

void *getInputStream(){
  char server_reply[1];
  while (1) {
    memset (server_reply, 0, sizeof (server_reply));
    if( recv(sock , server_reply , 2000 , 0) < 0)
    {
      puts("recv failed");
      break;
    }
    printf("%s\n",server_reply);
    int choice = atoi(server_reply);
    if(choice == 10){
      printf("Fermeture....\n" );
      exit(0);
    }
  }

}

void *sendDataStream(){
  char message[1000];
  while (1) {
    memset (message, 0, sizeof (message));
    scanf("%s" , message);
    if( send(sock , message , strlen(message) , 0) < 0)
    {
      puts("Send failed");
      return 1;
    }
  }

}

int main(int argc , char *argv[])
{
  system("clear");
  pthread_t sniffer_thread1, sniffer_thread2;
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
  //Lancement du jeu
  while(1)
  {
    if( pthread_create( &sniffer_thread1 , NULL ,  getInputStream, NULL) < 0)
    {
      perror("could not create thread");
      return 1;
    }
    if( pthread_create( &sniffer_thread2 , NULL ,  sendDataStream, NULL) < 0)
    {
      perror("could not create thread");
      return 1;
    }
    //Vider les tableaux en attente de la boucle suivante

    //memset (server_reply, 0, sizeof (server_reply));


    //Send some data
    // if( send(sock , message , strlen(message) , 0) < 0)
    // {
    //   puts("Send failed");
    //   return 1;
    // }

    //Receive a reply from the server
    // if( recv(sock , server_reply , 2000 , 0) < 0)
    // {
    //   puts("recv failed");
    //   break;
    // }

    //SIGNAL CLOSE

  }
  close(sock);
  return 0;
}
