/*-----------------------------------------------------------
Client a lancer apres le serveur avec la commande :
client <adresse-serveur> <message-a-transmettre>
------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>


typedef struct sockaddr 	sockaddr;
typedef struct sockaddr_in 	sockaddr_in;
typedef struct hostent 		hostent;
typedef struct servent 		servent;

int main(int argc, char **argv) {

  int 	socket_descriptor, 	/* descripteur de socket */
  longueur; 		/* longueur d'un buffer utilisé */
  sockaddr_in adresse_locale; 	/* adresse de socket local */
  hostent *	ptr_host; 		/* info sur une machine hote */
  servent *	ptr_service; 		/* info sur service */
  char 	buffer[256];
  char *	prog; 			/* nom du programme */
  char *	host; 			/* nom de la machine distante */

  if (argc != 2) {
    perror("usage : client <adresse-serveur>");
    exit(1);
  }

  prog = argv[0];
  host = argv[1];

  if ((ptr_host = gethostbyname(host)) == NULL) {
    perror("erreur : impossible de trouver le serveur a partir de son adresse.");
    exit(1);
  }

  /* copie caractere par caractere des infos de ptr_host vers adresse_locale */
  bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
  adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */


/*-----------------------------------------------------------*/
/* SOLUTION 2 : utiliser un nouveau numero de port */
adresse_locale.sin_port = htons(5000);
/*-----------------------------------------------------------*/

printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port));

/* creation de la socket */
if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
  perror("erreur : impossible de creer la socket de connexion avec le serveur.");
  exit(1);
}

/* tentative de connexion au serveur dont les infos sont dans adresse_locale */
if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
  perror("erreur : impossible de se connecter au serveur.");
  exit(1);
}
printf("Connected to %s\n", ptr_host);

/* lecture de la reponse en provenance du serveur */
while((longueur = read(socket_descriptor, buffer, sizeof(buffer))) > 0) {
  printf("reponse du serveur : \n");
  write(1,buffer,longueur);
}

close(socket_descriptor);


}