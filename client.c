/*-----------------------------------------------------------
Client a lancer apres le serveur avec la commande :
client <adresse-serveur> <message-a-transmettre>
------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>


#define BUF_SIZE 256
#define PORT 5000

typedef struct sockaddr 	sockaddr;
typedef struct sockaddr_in 	sockaddr_in;
typedef struct hostent 		hostent;
typedef struct servent 		servent;

 
void write_server(int sock, const char *buffer);
int read_server(int sock, char *buffer);
int str_istr (const char *cs, const char *ct);
int main(int argc, char **argv) {
  
    int 	socket_descriptor, 	/* descripteur de socket */
		longueur; 		/* longueur d'un buffer utilisé */
    sockaddr_in adresse_locale; 	/* adresse de socket local */
    hostent *	ptr_host; 		/* info sur une machine hote */
    servent *	ptr_service; 		/* info sur service */
    char 	buffer[BUF_SIZE];
    char *	prog; 			/* nom du programme */
    char *	host; 			/* nom de la machine distante */
    char *	mesg; 			/* message envoyé */
    fd_set rdfs;
     
    if (argc != 2) {
	perror("Usage : client <adresse-serveur>");
	exit(1);
    }
   
    prog = argv[0];
    host = argv[1];
    
    printf("Nom de l'executable : %s \n", prog);
    printf("Adresse du serveur  : %s \n", host);
    
    if ((ptr_host = gethostbyname(host)) == NULL) {
	perror("Erreur : impossible de trouver le serveur a partir de son adresse.");
	exit(1);
    }
    
    /* copie caractere par caractere des infos de ptr_host vers adresse_locale */
    bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
    adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */
 
    

    /*utilisation d'un nouveau numero de port */
    adresse_locale.sin_port = htons(PORT);
    
    printf("Numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port));
    
    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	perror("Erreur : impossible de creer la socket de connexion avec le serveur.");
	exit(1);
    }
    
    /* tentative de connexion au serveur dont les infos sont dans adresse_locale */
    if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
	perror("Erreur : impossible de se connecter au serveur.");
	exit(1);
    }
    
    printf("Connexion etablie avec le serveur. \n");
    printf("*************************************************************************\n");
   
    /* lecture de la reponse en provenance du serveur */
	while(1){
		FD_ZERO(&rdfs); 
		FD_SET(STDIN_FILENO, &rdfs);
		FD_SET(socket_descriptor, &rdfs);
		
		if(select(socket_descriptor+ 1, &rdfs, NULL,  NULL, NULL )== -1)
		{
			perror("Socket vide");
			exit(1);
		}
		
		if(FD_ISSET(STDIN_FILENO, &rdfs))
		{
			fgets(buffer, BUF_SIZE - 1, stdin);
			{
				char *p = NULL;
				p = strstr(buffer, "\n");
				if(p != NULL)
				{
					*p = 0;
				}else{
					buffer[BUF_SIZE - 1] = 0;
				}
			}
			write_server(socket_descriptor, buffer);
			
		}else if(FD_ISSET(socket_descriptor, &rdfs))
		{
			int n = read_server(socket_descriptor, buffer);
			if(n == 0)
			{
				printf("Serveur déconnecté");
				break;
			}
			printf("%s",buffer);
		}
	
	}
	
    
    
    close(socket_descriptor);
    
    printf("Connexion avec le serveur fermee, fin du programme.\n");
    
    exit(0);
    
}


int read_server(int sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      exit(1);
   }

   buffer[n] = 0;

   return n;
}

void write_server(int sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(1);
   }
}

int str_istr (const char *ct,const char *cs)
{
   int index = -1;
   if (cs != NULL && ct != NULL)
   {
      char *ptr_pos = NULL;

      ptr_pos = strstr (cs, ct);
      if (ptr_pos != NULL)
      {
         index = ptr_pos - cs;
      }
   }
   return index;
}


