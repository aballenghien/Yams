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
#define NB_VALEUR_DE 6
#define BUF_SIZE 256

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
void jouer_partie_yams(joueur joueurs[], char *buffer);
void lancer_des(joueur joueurs[], int numJoueur, int tab_des[]);
void calculer_score(int numJoueur, int numPartie, int tab_des[], int *tab_score);
void initialiser_tab_des(int tab_des[]);
int read_client(int sock, char *buffer);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
	hostent*		ptr_hote; 			/* les infos recuperees sur la machine hote */
    servent*		ptr_service; 			/* les infos recuperees sur le service de la machine */
    char 			machine[TAILLE_MAX_NOM+1]; 	/* nom de la machine locale */
    int 			nb_connexions;
    char 			buffer[BUF_SIZE];
    joueur 			joueurs[NB_JOUEURS];
    char 			messageJoueur[25];

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
        // créer des thread et les rattacher au joueur tant que le nombre de joueur n'est pas atteint
        if (nb_connexions < NB_JOUEURS){
			if( pthread_create( &joueurs[nb_connexions].thread_joueur , NULL ,  connection_handler , (void*) &client_sock) < 0)
			{
				perror("could not create thread");
				return 1;
			}
			//Now join the thread , so that we dont terminate before the thread
			//pthread_join( thread_id , NULL);
			
			puts("Handler assigned");
			// on attribue un socket, un thread et un numéro à chaque joueur puis on leur annonce
			joueurs[nb_connexions].socket = client_sock;
			joueurs[nb_connexions].numeroJ = nb_connexions+1;
			sprintf(messageJoueur,"Vous êtes le joueur : %d \n",nb_connexions+1);
			write(client_sock,messageJoueur, strlen(messageJoueur));
			
		}else{
			// indiquer au client que le nombre de joueur est atteint
			sprintf(buffer, "erreur");
			write(client_sock,buffer, strlen(buffer));
			sprintf(buffer,"Une partie est déjà commencée");
			write(client_sock,buffer, strlen(buffer));
		}        
        
        nb_connexions = nb_connexions + 1;
        puts("accept");
        if(nb_connexions == NB_JOUEURS){
			// débuter la partie une fois que tous les joueurs ont rejoint la partie
			sleep(5);
			jouer_partie_yams(joueurs, buffer);
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
    char *buffer , client_message[2000];  
    return 0;
} 

void jouer_partie_yams(joueur joueurs[], char *buffer)
{
	int numPartie;
	int numJoueur;
	int tab_des[NB_DES];
	int tab_score[NB_JOUEURS][NB_PARTIES+1];
	
	numPartie = 1;
	int i;
	char *message;
	for(i=0; i< NB_JOUEURS; i++)
	{
		sprintf(buffer,"Bonjour tout le monde\n");
		write(joueurs[i].socket,buffer, strlen(buffer));
	}
	
	//les joueurs joueront  en tout 10 fois chacun leur tour 
	for(numPartie = 1; numPartie <= NB_PARTIES; numPartie++)
	{
		for (numJoueur = 1; numJoueur <= NB_JOUEURS; numJoueur++)
		{
			// remplissage du tableau avec des valeurs aléatoire du dés
			lancer_des(joueurs, numJoueur, tab_des);
		}
	}
	
}

void lancer_des(joueur joueurs[], int numJoueur, int tab_des[]){
	char tour[26];
	char *buffer, client_message[1];
	int i, read_size;
	int k;
	char lance_de_des[19];
	int ok;
	char de[1];
	
	// on attend que le joueur soir prêt pour lancer les dés
	//ne fonctionne pas pour l'instant
	for (i = 0; i < NB_JOUEURS; i++)
	{
		sprintf(tour, "C'est le tour du joueur %d", numJoueur);
		write(joueurs[i].socket, tour, strlen(tour));
		tour[25] = 0;
	}
	puts("test"); 
	sprintf(buffer, "\n C'est à votre tour de lancer les dés, êtes vous prêt?Y/N \n");
	printf("%s",buffer);
	write(joueurs[numJoueur-1].socket, buffer, strlen(buffer));
	ok = 0;
	do{
		
		read_size = read_client(joueurs[numJoueur-1].socket, buffer);
		if(read_size > 0)
		{
			
			toupper(buffer);
			if(strcmp(buffer, "Y")==0)
			{			
				ok = 1;
			}else{
				buffer = "êtes vous prêt?Y/N \n";
				write(joueurs[numJoueur-1].socket, buffer, strlen(buffer));
			}
		}
	}while(ok == 0);
	
	if(ok == 1)
	{
		// on remplit le tableau avec 5 valeurs aléatoires comprises entre 1 et 6
		initialiser_tab_des(tab_des);
		
		for(k = 0; k < NB_DES; k++)
		{
			tab_des[k] = rand()%(NB_VALEUR_DE-1)+1;
			sprintf(de,"%d",tab_des[k]);
			strcat(lance_de_des, de);
			strcat(lance_de_des, "|");
		}
		strcat(lance_de_des,"\n");
		
		//on affiche le tableau côté client
		for (i = 0; i < NB_JOUEURS; i++)
		{
			buffer = "Résultat du lancer du joueur: \n ";
			write(joueurs[i].socket, buffer, strlen(buffer));
			write(joueurs[i].socket, lance_de_des, strlen(lance_de_des));
		}
		
		
	}
	
}

void initialiser_tab_des(int tab_des[])
{
	int i;
	for(i = 0; i < NB_DES; i++)
	{
		tab_des[i] = 0;
	}
	
}

int read_client(int sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, sizeof(buffer), 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}


















