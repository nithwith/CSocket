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

typedef struct client{
  struct sockaddr_in addr;	/* Client remote address */
  int connfd;			/* Connection file descriptor */
  int uid;			/* Client unique identifier */
  char name[32];			/* Client name */
  struct roomer *currRoom;
} clientOn;

typedef struct Roomer{
  struct client *cli1, *cli2;			/*  */
  int open;
  //struct Roomer *YouCanOpenTheDoor;			/*  */
} Room;

void *connection_handler(void *client);
void *gameIA(void *client);
void *createRoom(void *);
void *printMenu();
void *gameMulti(void *Room);
void AddClient(clientOn *c);//AJoute le client au groupe de client connecter
void AddToQueue(int c);//AJoute le client au groupe de client en queue
void DelClient(int);
void delToqueue(int c);
//Initialise le tab de clients connectés
clientOn *clients[10];
int inQueue[10];
int nbInqueue = 0;



int main(int argc , char *argv[])
{
  system("clear");
  static int uid = 10;
  int socket_desc , client_sock , c , *new_sock;
  struct sockaddr_in server , client;

  //==============INIT CLIENT==========================//
  for (int i = 0; i < 10; i++)
  {
    clients[i] = NULL;
    inQueue[i] = NULL;
  }

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
    clientOn *cli = (clientOn *)malloc(sizeof(clientOn));
    cli->addr = client;
    cli->connfd = client_sock;
    cli->uid = uid++;
    cli->currRoom = NULL;
    AddClient(cli);
    printf("Nouveau Client: %d\n",cli->uid );
    if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) cli) < 0)
    {
      perror("could not create thread");
      return 1;
    }
    //pthread_join( sniffer_thread , NULL);
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
  clientOn *cli = (clientOn*)client;
  char Fullmsg[4000],client_message[2000],intro[255], push[255];
  int choice, read_size, bool = 0, ok = 0, i =0;
  strcat(intro,"Welcome to CSJANKEN\n");
  strcat(intro, "------[MENU]------\n");
  strcat(intro, "--- [1] Jouer contre l'IA ---\n");
  strcat(intro, "--- [2] Jouer contre un autre joueur ---\n");
  strcat(intro, "--- [3] Quitter ---\n");
  send(cli->connfd , intro , strlen(intro), 0);
  while(bool == 0) {
    if ((read_size = recv(cli->connfd , client_message , 10 , 0)) > 0 ) {
      choice = atoi(client_message);
      switch (choice) {

        // JEU CONTRE L'IA
        case 1:
        bool = 1;
        gameIA(cli);
        break;

        // JEU MULTI
        case 2:
        bool = 1;
        nbInqueue++;
        printf("nbbqueueueu %d\n", nbInqueue);
        AddToQueue(cli->uid);
        if (nbInqueue == 2) {
          nbInqueue = 0;
          int tb=0;
          while (clients[i]) {
            printf("room %d\n", clients[i]->currRoom);
            if((clients[i]->uid != cli->uid) && (clients[i]->currRoom != 0 && ((Room*)clients[i]->currRoom)->open == 1)) {
              snprintf(push, sizeof(push), "Joueur %d seul , rejoignons le\n",clients[i]->uid);
              write(cli->connfd, push , strlen(push));
              memset(push, 0 , sizeof push);
              cli->currRoom = (Room*)clients[i]->currRoom;
              ((Room*)clients[i]->currRoom)->open = 0;
              ((Room*)clients[i]->currRoom)->cli2 = cli;
              return 0;

            }
            i++;
          }
        } else {
          strcat(push, "En attente d'un 2eme joueur\n");
          write(cli->connfd, push , strlen(push));
          memset(push, 0 , sizeof push);
          printf("Création d'une room\n" );
          Room *temp;
          temp = malloc(sizeof(Room));
          temp->open = 1;
          temp->cli1 = cli;
          cli->currRoom = temp;
          while(1) {
            if (((Room*)cli->currRoom)->cli2 != 0) {
              //Lancement du jeu multi
              gameMulti(cli->currRoom);
              return 0;
            }
          }
          free(temp);
        }
        break;
        case 3:
        bool = 1;
        DelClient(cli->uid);
        // close(cli->connfd);
        break;
        default:
        bool = 0;
        write(cli->connfd, "Mauvais choix, rééssayez:\n", strlen("Mauvais choix, rééssayez:\n"));
      }
    }
  }
  close(cli->connfd);
  //free(cli->connfd);
}

void *gameMulti(void *r) {

  const char Mooves[3][15]={"Papier","Ciseaux","Pierre"};
  pthread_t tid1, tid2;
  Room *room = (Room*)r;
  char multi_push[255], message1[1], message2[1];
  int game, win1, win2, tie, lose1, lose2, choice1, choice2, read_size, bool1 = 0, bool2 = 0;
  clientOn *cli1 = room->cli1;
  clientOn *cli2 = room->cli2;
  strcat(multi_push, "------ [MODE MULTI] ------\n");
  write(cli1->connfd, multi_push, strlen(multi_push));
  write(cli2->connfd, multi_push, strlen(multi_push));
  snprintf(multi_push, sizeof(multi_push), "Joueur %d VERSUS Joueur %d\n", cli1->uid, cli2->uid);
  write(cli1->connfd, multi_push, strlen(multi_push));
  write(cli2->connfd, multi_push, strlen(multi_push));
  memset(multi_push, 0, sizeof multi_push);

  for (game = 0; game < 3 ; game++) {
    choice1 = 0;
    choice2 = 0;
    bool1 = 0;
    bool2 = 0;
    strcat(multi_push,"Rentrez 1 pour Papier, 2 pour ciseaux, 3 pour pierre\n");
    write(cli1->connfd , multi_push , strlen(multi_push));
    write(cli2->connfd , multi_push , strlen(multi_push));
    memset(multi_push, 0, sizeof multi_push);
    while(bool1 == 0 && bool2 == 0){
      if ((read_size = recv(cli1->connfd ,message1 , 10 , 0)) > 0 ){
        choice1 = atoi(message1);
        if (choice1 == 1 || choice1 == 2 || choice1 == 3 ) {
          printf("joeur 1 a joué %d\n", choice1);
          bool1 = 1;
        } else {
          strcat(multi_push,"Choix inconnu, rééssayez:\n");
          write(cli1->connfd , multi_push , strlen(multi_push));
          memset(multi_push, 0, sizeof multi_push);
        }
      }
      if ((read_size = recv(cli2->connfd ,message2 , 10 , 0)) > 0 ){
        choice2 = atoi(message2);
        if (choice2 == 1 || choice2 == 2 || choice2 == 3 ) {
          printf("joeur 2 a joué %d\n", choice2);
          bool2 = 1;
        } else {
          strcat(multi_push,"Choix inconnu, rééssayez:\n");
          write(cli2->connfd , multi_push , strlen(multi_push));
          memset(multi_push, 0, sizeof multi_push);
        }
      }
    }

    snprintf(multi_push, sizeof(multi_push), "[VOUS]: %s \n", Mooves[choice1 - 1]);
    write(cli1->connfd , multi_push , strlen(multi_push));
    snprintf(multi_push, sizeof(multi_push), "[VOUS]: %s \n", Mooves[choice2 - 1]);
    write(cli2->connfd , multi_push , strlen(multi_push));
    snprintf(multi_push, sizeof(multi_push), "[AUTRE JOUEUR]: %s \n", Mooves[choice2 - 1]);
    write(cli1->connfd , multi_push , strlen(multi_push));
    snprintf(multi_push, sizeof(multi_push), "[AUTRE JOUEUR]: %s \n", Mooves[choice1 - 1]);
    write(cli2->connfd , multi_push , strlen(multi_push));
    memset(multi_push, 0, sizeof multi_push);

  }
  delToqueue(cli1->uid);
  delToqueue(cli2->uid);
}



void *gameIA(void *client)
{

  //srand(time(NULL));
  clientOn *cli = (clientOn*)client;
  const char Mooves[3][15]={"Papier","Ciseaux","Pierre"};
  int choice, read_size, cx;
  char push[255];
  char client_message[1];
  int win = 0;
  int tie = 0;
  int lose = 0;
  int game = 0;
  int bool = 0;
  int ai;

  for(game = 0 ; game < 3 ; game++) {
    bool = 0;
    ai = rand() % 3;
    strcat(push,"Rentrez 1 pour Papier, 2 pour ciseaux, 3 pour pierre\n");
    write(cli->connfd , push , strlen(push));
    memset(push, 0, sizeof push);

    while(bool == 0) {
      if ((read_size = recv(cli->connfd , client_message , 10 , 0)) > 0 ){
        choice = atoi(client_message);
        if (choice == 1 || choice == 2 || choice == 3 ) {
          bool = 1;
        } else {
          strcat(push,"Choix inconnu, rééssayez:\n");
          write(cli->connfd , push , strlen(push));
          memset(push, 0, sizeof push);
        }
      }
    }
    cx = snprintf(push, sizeof(push), "[VOUS]: %s \n", Mooves[choice - 1]);
    write(cli->connfd , push , strlen(push));
    cx = snprintf(push, sizeof(push), "[IA]: %s \n", Mooves[ai]);
    write(cli->connfd , push , strlen(push));
    memset(push, 0, sizeof push);
    if(choice == 1 && ai == 0){
      strcat(push, "Papier contre papier, égalité!\n");
      tie++;
    }
    else if(choice ==1 && ai== 1){
      strcat(push,"Papier contre ciseaux, perdu...\n");
      lose++;
    }
    else if(choice == 1 && ai == 2){
      strcat(push,"Papier contre pierre, victoire !\n");
      win++;
    }
    else if(choice == 2 && ai == 0){
      strcat(push, "Ciseaux contre papier, victoire !\n");
      win++;
    }
    else if(choice == 2 && ai == 1){
      strcat(push,"Ciseaux contre ciseaux, égalité!\n");
      tie++;
    }
    else if(choice == 2 && ai == 2){
      strcat(push,"Ciseaux contre pierre, perdu...\n");
      lose++;
    }
    else if( choice == 3 && ai == 0){
      strcat(push,"Pierre contre papier, perdu...\n");
      lose++;
    }
    else if( choice == 3 && ai == 1){
      strcat(push, "Pierre contre ciseaux, victoire !\n");
      win++;
    }
    else if(choice == 3 && ai == 2){
      strcat(push,"Pierre contre pierre, égalité!\n");
      tie++;
    }
    printf("UN TOUR\n");
    write(cli->connfd , push , strlen(push));
    memset(push, 0, sizeof push);

  }

  memset(push, 0, sizeof push);
  strcat(push,"Résultats :\n");
  write(cli->connfd, push, strlen(push));
  cx = snprintf(push, sizeof(push), "Victoire(s): %d \nDéfaite(s): %d\nNul(s): %d\n",win, lose, tie);
  write(cli->connfd, push, strlen(push));
  memset(push, 0, sizeof push);
  if (win > lose) {
    strcat(push, "Vous avez gagné le match !\n");
  } else if ( win < lose) {
    strcat(push, "Vous avez perdu le match !\n");
  } else {
    strcat(push, "C'est un match nul !\n");
  }
  write(cli->connfd, push, strlen(push));
  memset(push, 0, sizeof push);
  bool = 0;
  strcat(push, "Voulez vous rejouer?(1 pour Non/ 2 pour Oui) \n");
  write(cli->connfd, push, strlen(push));
  while (bool == 0) {
    if ((recv(cli->connfd, client_message , 10 , 0)) > 0 ) {
      choice = atoi(client_message);
      if(choice == 1 || choice == 2) {
        switch (choice) {
          case 1:
          bool = 1;
          *connection_handler(cli);
          return NULL;
          break;
          case 2:
          bool = 1;
          gameIA(cli);
          return NULL;
          break;
          default:
          write(cli->connfd, "Mauvais choix, rééssayez: \n", strlen("Mauvais choix, rééssayez: \n"));
        }
      } else {
        write(cli->connfd, "Mauvais choix, rééssayez: \n", strlen("Mauvais choix, rééssayez: \n"));
      }
    }
  }
}

void AddClient(clientOn *c){
  int i=0;
  while(clients[i]){
    i++;
  }
  clients[i] = c;
}


void AddToQueue(int c){
  int i=0;
  while(inQueue[i]){
    i++;
  }
  inQueue[i] = c;
}

void delToqueue(int c){
  int i = 0;
  while (inQueue[i] != c) {
    i++;
  }
  inQueue[i] = NULL;
}

void DelClient(int id){
  int i=0;
  while(clients[i]->uid != id){
    i++;
  }
  close(clients[i]->connfd);
  free(clients[i]);
  clients[i] = NULL;
}
