
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
// ERROR ON AT TEST 0 6 AND 7
//Move value for random failure on block

typedef struct Roomer{
	int p1_uid,p2_uid,game,played1,played2;			/*  */
	struct Roomer *YouCanOpenTheDoor;			/*  */
} Rooms;

typedef struct {
	struct sockaddr_in addr;	/* Client remote address */
	int connfd;			/* Connection file descriptor */
	int uid;			/* Client unique identifier */
	char name[32];			/* Client name */
  struct Roomer *CurrRoom;
} clientOn;

//============ global parameter ========//
#define MAX_CLIENTS	50
Rooms *RootRooms = NULL;
static unsigned int NbCLient = 0;
static int uid = 10;

clientOn *clients[MAX_CLIENTS];
void AddClient(clientOn *c);//AJoute le client au groupe de client connecter
void *connection_handler(void *socket_desc);//Allow the conention ofmultiple client in the same time
int hostname_to_ip(char *  , char *);// DNS name to IP
int command_analyzer(char *);//CHeck and move depending on user input
int isValidIpAddress(char *ipAddress);//test if an @IP is valid


int main(int argc, char *argv[]){
  //convert hostname to gethostbyname
  // if 2 parameter server if one client
  int socket_desc, new_socket, c, port, read_size,i;
  pthread_t sniffer_thread;
  struct sockaddr_in server,client;
  char ip[25],server_message[1025];
  static int uid = 10;

  if (argc == 2)
  {
    printf("Server mode\n");
    port = atoi(argv[1]);
    if( port < 3000 || port > 3999 ){printf("Port must be in the interval[3000:3999]"); return 1;}
    strncpy(ip, "127.0.0.1\0",25);
  }else {printf("Invalid  number of arguments\n"); return 1;}


  //==============INIT CLIENT==========================//
  for (i = 0; i < MAX_CLIENTS; i++)
  {
    clients[i] = NULL;
  }



  //Create socket
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1)
  {
    printf("Could not create socket\n");
  }

  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(ip);
  server.sin_port = htons( port );
  //Bind
  if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
  {
    puts("bind failed, port already used ? ");
    return 1;
  }
  puts("bind done");
  //Listen
  if(listen(socket_desc , 3) < 0){
    printf("Failed to listen.\n");
  }
  printf("Waiting for incoming connections...");
  c = sizeof(struct sockaddr_in);
  // WAITING FOR CONNECTION

  while(1)
  {
    new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    puts("Connection accepted");
    if((NbCLient+1) == MAX_CLIENTS){
      printf("Max client reached.\n Connection refused\n");
      close(new_socket);
    }else{
      //=========INIT CLIENT CONNECTION===========//
      clientOn *cli = (clientOn *)malloc(sizeof(clientOn));
      cli->addr = client;
  		cli->connfd = new_socket;
      cli->uid = uid++;
      cli->CurrRoom = NULL;
      //=========##################========//
      sprintf(cli->name, "%d", cli->uid);
      AddClient(cli);
      if( pthread_create( &sniffer_thread , NULL ,  connection_handler, (void*) cli) < 0)
      {
        perror("could not create thread");
        return 1;
      }
      //Now join the thread , so that we dont terminate before the thread
      //pthread_join( sniffer_thread , NULL);
      puts("Handler assigned");
    }
  }

  return 0;
}

//==============CLIENTS HANDLING =============//
void AddClient(clientOn *c){
  int i=0;
  while(clients[i]){
    i++;
  }
  clients[i] = c;
}
void DelClient(int id){
  int i=0;
  while(clients[i]->uid != id){
    i++;
  }
  free(clients[i]);
  clients[i] = NULL;
  NbCLient--;
}
//CHECKING INPUT AND SENDING MSG TO CLIENT, MAIN FUNCTION


int command_analyzer(char *cmd){
  int rez;
  if(!strncmp(cmd,"pierre",6)){
    rez = 1;
  }else if(!strncmp(cmd,"feuille",7)){
    rez = 2;
  }else if(!strncmp(cmd,"ciseau",6)){
    rez = 3;
  }else{
    rez = 0;
  }
  return rez;
}

int room_selection(void *client){
  int sizeUsed=0,i=1,ok=0;
  clientOn *cli = (clientOn*)client;
  char msg[4000];
  Rooms *Temp;
  Temp = RootRooms;

  if(Temp != NULL ){
    while(Temp && ok == 0 && sizeUsed <= 3700){
      if(Temp->p2_uid != 0){
        sizeUsed+=snprintf(msg+sizeUsed,4000-sizeUsed,"Rooms %d is full. Player %d and %d are playing inside\n",i,Temp->p1_uid,Temp->p2_uid);
        i++;
        Temp = Temp->YouCanOpenTheDoor;
      }else{
        ok=1;
        sizeUsed+=snprintf(msg+sizeUsed,4000-sizeUsed,"Rooms %d is available. Player %d is waiting inside, let's join him\n",i,Temp->p1_uid);
      }
    }
    if(ok != 0){
      Temp->p2_uid = cli->uid;
      cli->CurrRoom = Temp;
      sizeUsed+=snprintf(msg+sizeUsed,4000-sizeUsed,"Some rooms are free :) !\n");
      write(cli->connfd,msg,strlen(msg));
    }
  }
  if(ok == 0){
    sizeUsed += snprintf(msg+sizeUsed,4000-sizeUsed,"\nAll rooms are full or no rooms exist.\nCreating a room\n Waiting for another player to connect.\n");
    write(cli->connfd,msg,strlen(msg));
    printf("All rooms are full or no rooms.\n");
    Temp = malloc(sizeof(Rooms));
    Temp->p1_uid  = cli->uid;
    Temp->p2_uid  = 0;
    Temp->game    = 0;
    Temp->played1 = 0;
    Temp->played2 = 0;
    Temp->YouCanOpenTheDoor = RootRooms;
    cli->CurrRoom = Temp;
    RootRooms = Temp;
  }

  return ok;
}



void *connection_handler(void *client)
{

  srand(time(NULL)); // initialisation of rand
  //Get the socket descriptor
  clientOn *cli = (clientOn*)client;
  int rez=0,read_size,rn,Ready=0,rx=0,Round,wait;
  char Fullmsg[4000],client_message[2000],intro[255];
  const char Mooves[4][15]={"Echec","Pierre","Feuille","Ciseau"};
  //Send some messages to the client
  strcat(intro,"Welcome to CSJANKEN.\n");
  write(cli->connfd , intro , strlen(intro));

  //Receive a message from client
  while(1){
    if(cli->CurrRoom !=NULL){
      if(cli->CurrRoom->p1_uid != 0 && cli->CurrRoom->p2_uid != 0){
        Round = 0;
        wait  = 0;
        while( (read_size = recv(cli->connfd , client_message , 2000 , 0)) > 0 )
        {
          //reassemble message
          client_message[read_size]='\0';
          memcpy(&Fullmsg[rx],client_message,sizeof(client_message));
          rx+=read_size;
          //strcat(beg,client_message);
          if(strstr(Fullmsg,"\r\n")!=NULL && wait == 0){
            rez = command_analyzer(Fullmsg);
            printf("Read size : %d\n",read_size);
            printf("Mouvement : %15s\n",Mooves[rez]);
            //IF CORRECT WAIT AND EVALUATE MOVE FROM THE OTHER GUY

            if(rez != 0){
              wait = 1;
              write(cli->connfd , strcat("You've played : ",Mooves[rez]) , strlen(Mooves[rez])+17);
              if(cli->uid == cli->CurrRoom->p1_uid){
                cli->CurrRoom->played1 = rez;
              }else{
                cli->CurrRoom->played2 = rez;
              }

            }else{
              write(cli->connfd , "Wrong moves, retype your move.\n" , 50);
            }
          }else if (wait == 1){
            write(cli->connfd , "Waiting for the(o) her player.\n" , 31);
            sleep(1);
            if(cli->uid == cli->CurrRoom->p1_uid){
              if(cli->CurrRoom->played2 != 0){
                wait=2;
              }
            }else{
              if(cli->CurrRoom->played1 != 0){
                wait=2;
              }
            }
          }else if (wait == 2){
            //EVALUTE AND CHECK WINNER
            //LOCAL VAR + game var is enough
						int win;
            win = EvalGame(cli->CurrRoom->played1,cli->CurrRoom->played2);
            //2 null, 1 win 12, 0 win 1
            if(cli->uid == cli->CurrRoom->p1_uid){
              cli->CurrRoom->played1=0;
              cli->CurrRoom->game++;
            }else{
              cli->CurrRoom->played2=0;
            }
            memset(Fullmsg,0,sizeof(Fullmsg));
            rx    = 0;
            wait  = 0;
          }
          read_size=0;
          memset(client_message,0,sizeof(client_message));
        }
      }else{
        sleep(1);
      }
    }else{
      Ready=room_selection(client);
    }
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

  //Free the socket pointer
  free(cli);
  return 0;
}

int EvalGame(int p1,int p2){
  //1 pierre 2 feuille 3 ciseau
  int win=0;
  if(p1 == 3){
    if(p2 == 1){
      win = 0;
    }else if(p2 == 2){
      win = 1;
    }else{
      win = 2;
    }
  }else if(p1 == 2){
    if(p2 == 1){
      win = 1;
    }else if(p2 == 3){
      win = 0;
    }else{
      win = 2;
    }
  }else{
    if(p2 == 2){
      win = 1;
    }else if(p2 == 3)
    {
      win = 0;
    }else{
      win = 2;
    }
  }
  return win;
}
//===================CHECKING CONNECTION PART========================
//Test IP
int isValidIpAddress(char *ipAddress)
{
  struct sockaddr_in sa;
  int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
  return result != 0;
}

int hostname_to_ip(char * hostname , char* ip)
{
  struct hostent *he;
  struct in_addr **addr_list;
  int i;

  if ( (he = gethostbyname( hostname ) ) == NULL)
  {
    // get the host info
    herror("gethostbyname");
    return 1;
  }

  addr_list = (struct in_addr **) he->h_addr_list;

  for(i = 0; addr_list[i] != NULL; i++)
  {
    //Return the first one;
    strcpy(ip , inet_ntoa(*addr_list[i]) );
    return 0;
  }

  return 1;
}
