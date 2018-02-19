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

struct arg_struct {
    int choice;
    int socket;
};

void *connection_handler(void *client);
void *gameIA(void *client);
void *createRoom(void *);
void *printMenu();
void *gameMulti(void *Room);
void AddClient(clientOn *c);//AJoute le client au groupe de client connecter
void AddToQueue(int c);//AJoute le client au groupe de client en queue
void DelClient(int);
void delToqueue(int c);
void *askChoice(void *client);
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
          printf("avant malloc\n" );
          temp = malloc(sizeof(Room*));
          printf("apres malloc\n" );
          temp->open = 1;
          printf("apres malloc\n" );
          temp->cli1 = cli;
          printf("apres malloc\n" );
          cli->currRoom = temp;
          printf("apres malloc\n" );
          while(1) {
            if (((Room*)cli->currRoom)->cli2 != 0) {

              printf("apres malloc\n" );
              //Lancement du jeu multi
              gameMulti(cli->currRoom);
              return 0;
            }
          }
        }
        break;

        //CLOSE GAME
        case 3:

        strcat(push, "10");
        write(cli->connfd, push , strlen(push));

        bool = 1;
        DelClient(cli->uid);
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
void *askChoice(void *client)
{
  char multi_push[255], message[1];
  clientOn *cli = (clientOn*)client;
  int bool = 0 , choice1;
  strcat(multi_push,"Rentrez 1 pour Papier, 2 pour ciseaux, 3 pour pierre\n");
  write(cli->connfd , multi_push , strlen(multi_push));
  memset(multi_push, 0, sizeof multi_push);
  while(bool == 0){
    if ( recv(cli->connfd ,message , 10 , 0) > 0 ){
      choice1 = atoi(message);
      if (choice1 == 1 || choice1 == 2 || choice1 == 3 ) {
        strcat(multi_push,"En attente du jeu de l'autre joueur...\n");
        write(cli->connfd , multi_push , strlen(multi_push));
        memset(multi_push, 0, sizeof multi_push);
        bool = 1;
        return (void*) choice1;
      } else {
        strcat(multi_push,"Choix inconnu, rééssayez:\n");
        write(cli->connfd , multi_push , strlen(multi_push));
        memset(multi_push, 0, sizeof multi_push);
      }
    }
  }
}

void *gameMulti(void *r) {

  const char Mooves[3][15]={"Papier","Ciseaux","Pierre"};
  pthread_t tid1, tid2;
  void *choice1 = 0;
  void *choice2 = 0;
  Room *room = (Room*)r;
  char multi_push[500], message1[1], message2[1];
  int game, win1= 0, win2= 0, tie= 0, lose1= 0, lose2= 0, read_size, res1, res2;
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
    pthread_create(&tid1, NULL, askChoice, cli1);
    pthread_create(&tid2, NULL, askChoice, cli2);
    pthread_join(tid1, &choice1);
    pthread_join(tid2, &choice2);
    printf("choice1 %d\n", (int)choice1);
    printf("choice2 %d\n", (int)choice2);
    snprintf(multi_push, sizeof(multi_push), "[VOUS]: %s \n", Mooves[((int)choice1) - 1]);
    write(cli1->connfd , multi_push , strlen(multi_push));
    snprintf(multi_push, sizeof(multi_push), "[VOUS]: %s \n", Mooves[((int)choice2) - 1]);
    write(cli2->connfd , multi_push , strlen(multi_push));
    snprintf(multi_push, sizeof(multi_push), "[AUTRE JOUEUR]: %s \n", Mooves[((int)choice2) - 1]);
    write(cli1->connfd , multi_push , strlen(multi_push));
    snprintf(multi_push, sizeof(multi_push), "[AUTRE JOUEUR]: %s \n", Mooves[((int)choice1) - 1]);
    write(cli2->connfd , multi_push , strlen(multi_push));
    memset(multi_push, 0, sizeof multi_push);

    if(((int)choice1) == 1 && ((int)choice2)== 1){
      strcat(multi_push, "Papier contre papier, égalité!\n");
      tie++;
    }
    else if(((int)choice1) ==1 && ((int)choice2)== 2){
      snprintf(multi_push, sizeof(multi_push), " Papier contre ciseaux , le joueur %d l'emporte !\n",cli2->uid);
      lose1++;win2++;
    }
    else if(((int)choice1) == 1 && ((int)choice2)== 3){
      snprintf(multi_push, sizeof(multi_push), " Papier contre pierre , le joueur %d l'emporte !\n",cli1->uid);
      win1++;lose2++;
    }
    else if(((int)choice1) == 2 && ((int)choice2) == 1){
      snprintf(multi_push, sizeof(multi_push), " Ciseaux contre papier , le joueur %d l'emporte !\n",cli1->uid);
      win1++;lose2++;
    }
    else if(((int)choice1) == 2 && ((int)choice2) == 2){
      strcat(multi_push,"Ciseaux contre ciseaux, égalité!\n");
      tie++;
    }
    else if(((int)choice1) == 2 && ((int)choice2) == 3){
      snprintf(multi_push, sizeof(multi_push), " Ciseaux contre pierre , le joueur %d l'emporte !\n",cli2->uid);
      lose1++;win2++;
    }
    else if(((int)choice1) == 3 && ((int)choice2)== 1){
      snprintf(multi_push, sizeof(multi_push), " Pierre contre papier , le joueur %d l'emporte !\n",cli2->uid);
      lose1++;win2++;
    }
    else if(((int)choice1) == 3 && ((int)choice2)== 2){
      snprintf(multi_push, sizeof(multi_push), "Pierre contre ciseaux , le joueur %d l'emporte !\n",cli1->uid);
      strcat(multi_push, "Pierre contre ciseaux, victoire !\n");
      win1++;lose2++;
    }
    else if(((int)choice1) == 3 && ((int)choice2)== 3){
      strcat(multi_push,"Pierre contre pierre, égalité!\n");
      tie++;
    }
    write(cli1->connfd , multi_push , strlen(multi_push));
    write(cli2->connfd , multi_push , strlen(multi_push));
    memset(multi_push, 0, sizeof multi_push);
  }

  memset(multi_push, 0, sizeof multi_push);
  res1 = win1 + tie - lose1;
  res2 = win2 + tie - lose2;
  if (res1 > res2) {
    snprintf(multi_push, sizeof(multi_push), "Joueur %d a gagné le match!\n",cli1->uid);
  } else if ( res2 > res1) {
    snprintf(multi_push, sizeof(multi_push), "Joueur %d a gagné le match!\n",cli2->uid);
  } else if (res2 == res1){
    strcat(multi_push, "Egalité!\n");
  }
  write(cli1->connfd, multi_push, strlen(multi_push));
  write(cli2->connfd, multi_push, strlen(multi_push));
  memset(multi_push, 0, sizeof multi_push);
  strcat(multi_push, "Retour au menu du jeu !\n");
  write(cli1->connfd, multi_push, strlen(multi_push));
  write(cli2->connfd, multi_push, strlen(multi_push));
  delToqueue(cli1->uid);
  delToqueue(cli2->uid);
  if( pthread_create( &tid1 , NULL ,  connection_handler , (void*) cli1) < 0)
  {
    perror("could not create thread");
    return 1;
  }
  if( pthread_create( &tid2 , NULL ,  connection_handler , (void*) cli2) < 0)
  {
    perror("could not create thread");
    return 1;
  }
}



void *gameIA(void *client)
{
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
    write(cli->connfd , push , strlen(push));
    memset(push, 0, sizeof push);

  }

  memset(push, 0, sizeof push);
  cx = snprintf(push, sizeof(push), "Résultats:\nVictoire(s): %d \nDéfaite(s): %d\nNul(s): %d\n",win, lose, tie);
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

        //strcat(push,"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        //write(cli->connfd , push , strlen(push));

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
