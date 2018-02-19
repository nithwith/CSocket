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


  //Affichage Menu accueil
  if (recv(sock , server_reply , sizeof(server_reply), 0) < 0) {
    printf("Receive failed");
  }
  printf("%s\n", server_reply);

    //Lancement du jeu
    while(1)
    {
      char message[1000];
      char server_reply[2000];

      //Vider les tableaux en attente de la boucle suivante
      memset (message, 0, sizeof (message));
      memset (server_reply, 0, sizeof (server_reply));

      printf("Enter votre choix : ");
      scanf("%s" , message);

      //Send some data
      if( send(sock , message , strlen(message) , 0) < 0)
      {
          puts("Send failed");
          return 1;
      }

      //Receive a reply from the server
      if( recv(sock , server_reply , 2000 , 0) < 0)
      {
          puts("recv failed");
          break;
      }

      //Attente d'un second message pour annoncer l'arrivée d'un nouveau joueur
      //Ne marche pas
      if(&server_reply[0] == "En attente d'un 2eme joueur"){
        if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
            break;
        }
      }

      //SIGNAL CLOSE
      int choice = atoi(server_reply);
      if(choice == 10){
        printf("Fermeture....\n" );
        break;
      }

      puts(server_reply);
    }
  close(sock);
  return 0;
}
