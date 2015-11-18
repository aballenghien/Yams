/*
    C socket server example, handles multiple clients using threads
    Compile
    gcc server.c -lpthread -o server
*/
 
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include<netdb.h>

#define TAILLE_MAX_NOM 256
#define NB_JOUEURS 2
#define NB_DES 5
#define NB_PARTIES 10

typedef struct hostent hostent;
typedef struct servent servent;
typedef struct 
{
	int numeroJ;
	pthread_t thread_joueur;
	int socket;
}
joueur;


 
//the thread function
void *connection_handler(void *);
void jouer_partie_yams(joueur *);
void lancer_des(joueur *, int numJoueur, int *tab_des[]);
void calculer_score(int numJoueur, int numPartie, int *tab_des[], int *tab_score[][]);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
	hostent*		ptr_hote; 			/* les infos recuperees sur la machine hote */
    servent*		ptr_service; 			/* les infos recuperees sur le service de la machine */
    char 			machine[TAILLE_MAX_NOM+1]; 	/* nom de la machine locale */
    int 			nb_connexions;
    char 			*message;
    joueur 			joueurs[NB_JOUEURS];
    char 			messageJoueur[25];
    int 			tab_des[NB_DES];
    int				tab_score[NB_JOUEURS][NB_PARTIES+1];

    gethostname(machine,TAILLE_MAX_NOM);		/* recuperation du nom de la machine */
    
    /* recuperation de la structure d'adresse en utilisant le nom */
    if ((ptr_hote = gethostbyname(machine)) == NULL) {
		perror("erreur : impossible de trouver le serveur a partir de son nom.");
		exit(1);
    }
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
	bcopy((char*)ptr_hote->h_addr, (char*)&server.sin_addr, ptr_hote->h_length);
    server.sin_family		= ptr_hote->h_addrtype; 	/* ou AF_INET */
    server.sin_addr.s_addr	= INADDR_ANY; 			/* ou AF_INET */

    //Prepare the sockaddr_in structure
    //server.sin_family = AF_INET;
    //server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 5000 );

    printf("numero de port pour la connexion au serveur : %d \n", 
		   ntohs(server.sin_port) /*ntohs(ptr_service->s_port)*/);
     
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
	pthread_t thread_id;
	nb_connexions = 0;
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)))
    {
        puts("Connection accepted");
        if (nb_connexions < NB_JOUEURS){
			if( pthread_create( &joueurs[nb_connexions].thread_joueur , NULL ,  connection_handler , (void*) &client_sock) < 0)
			{
				perror("could not create thread");
				return 1;
			}
			//Now join the thread , so that we dont terminate before the thread
			//pthread_join( thread_id , NULL);
			
			puts("Handler assigned");
			joueurs[nb_connexions].socket = client_sock;
			joueurs[nb_connexions].numeroJ = nb_connexions+1;
			sprintf(messageJoueur,"Vous êtes le joueur : %d \n",nb_connexions+1);
			write(client_sock,messageJoueur, strlen(messageJoueur));
			
		}else{
			message = "Une partie est déjà commencée";
			write(client_sock,message, strlen(message));
		}        
        
        nb_connexions = nb_connexions + 1;
        puts("accept");
        if(nb_connexions == NB_JOUEURS){
			puts("sortie");
			sleep(5);
			jouer_partie_yams(joueurs);
		}
    }
    
    
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    
 
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    int sock = *(int*)socket_desc;
    char *message , client_message[2000];  
    return 0;
} 

void jouer_partie_yams(joueur *joueurs)
{
	int numPartie;
	int numJoueur;
	
	numPartie = 1;
	puts ("jouer_partie_yams");
	int i;
	char *message;
	for(i=0; i< NB_JOUEURS; i++)
	{
		message = "Bonjour tout le monde\n";
		write(joueurs[i].socket,message, strlen(message));
	}
	
	for(numPartie = 1; i <= NB_PARTIES; numPartie++)
	{
	}
	
}
