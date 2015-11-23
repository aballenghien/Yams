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
#include<unistd.h>

#define TAILLE_MAX_NOM 256
#define NB_JOUEURS 2
#define NB_DES 5
#define NB_PARTIES 10
#define NB_VALEUR_DE 6
#define BUF_SIZE 256
#define PT_YAMS 50
#define PT_CARRE 40
#define PT_BRELAN 10
#define PT_FULL 30

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
void lancer_des(joueur joueurs[], int numJoueur, int tab_des[], int numPartie);
void calculer_score(int numJoueur, int numPartie, int tab_des[], int tab_score[NB_PARTIES+1][NB_JOUEURS]);
void initialiser_tab_des(int tab_des[]);
void initialiser_tab_score(int tab_score[NB_PARTIES+1][NB_JOUEURS]);
void afficher_score(int tab_score[NB_PARTIES+1][NB_JOUEURS]);
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
    fd_set 			rdfs;

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
	int tab_score[NB_PARTIES+1][NB_JOUEURS];
	fd_set rdfs;
	
	
	numPartie = 1;
	int i;
	char *message;
	for(i=0; i< NB_JOUEURS; i++)
	{
		sprintf(buffer,"Bonjour tout le monde\n");
		write(joueurs[i].socket,buffer, strlen(buffer));
	}
	// Initialiser le tableau des scores
	initialiser_tab_score(tab_score);
	//les joueurs joueront  en tout 10 fois chacun leur tour 
	for(numPartie = 0; numPartie < NB_PARTIES; numPartie++)
	{
		for (numJoueur = 0; numJoueur < NB_JOUEURS; numJoueur++)
		{			
	
		/*	FD_ZERO(&rdfs);
			FD_SET(STDIN_FILENO, &rdfs);
			FD_SET(joueurs[numJoueur*/
			// remplissage du tableau avec des valeurs aléatoire du dés
			lancer_des(joueurs, numJoueur, tab_des, numPartie+1);
			calculer_score(numJoueur,numPartie,tab_des,tab_score);
			
		}
	}
	
}

void lancer_des(joueur joueurs[], int numJoueur, int tab_des[], int numPartie){
	char tour[26];
	char *buffer, client_message[1], bufferTest[66];
	int i, read_size;
	int k;
	char lance_de_des[19];
	int ok;
	char de[1];
	
	// on attend que le joueur soir prêt pour lancer les dés
	//ne fonctionne pas pour l'instant
	for (i = 0; i < NB_JOUEURS; i++)
	{
		sprintf(tour, "Partie n°%d: C'est le tour du joueur %d", numPartie, joueurs[numJoueur].numeroJ);
		write(joueurs[i].socket, tour, strlen(tour));
		tour[25] = 0;
	}
	sprintf(bufferTest, "\n C'est à votre tour de lancer les dés, êtes vous prêt?Y/N \n");
	write(joueurs[numJoueur].socket, bufferTest, strlen(bufferTest));
	ok = 0;
	do{
		
		read_size = read_client(joueurs[numJoueur].socket, bufferTest);
		if(read_size > 0)
		{
			
			toupper(buffer);
			if(strcmp(bufferTest, "Y")==0)
			{			
				ok = 1;
			}else{
				buffer = "êtes vous prêt?Y/N \n";
				write(joueurs[numJoueur].socket, bufferTest, strlen(bufferTest));
			}
		}
	}while(ok == 0);
	
	if(ok == 1)
	{
		// on remplit le tableau avec 5 valeurs aléatoires comprises entre 1 et 6
		initialiser_tab_des(tab_des);
		int m = 0;
		lance_de_des[m] = 0;
		for(k = 0; k < NB_DES; k++)
		{
			tab_des[k] = rand()%(NB_VALEUR_DE-1)+1;
			sprintf(de,"%d",tab_des[k]);
			strcat(lance_de_des, de);
			strcat(lance_de_des, "|");			
			m = m+2;
			lance_de_des[m] = 0;
			puts(lance_de_des);
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

void initialiser_tab_score(int tab_score[NB_PARTIES+1][NB_JOUEURS])
{
	int i,j ; // parcourir le tableau à deux dimensions
	for (i=1; i<= NB_PARTIES+1;i++)
	{
	   for (j=1; j<=NB_JOUEURS; j++)
	   {
	      tab_score[i][j] = 0;	
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

void calculer_score(int numJoueur, int numPartie, int tab_des[], int tab_score[NB_PARTIES+1][NB_JOUEURS])
{
	int Brelan, Carre, Full, Yams, Double, Chance;
	int Val1, Val2, Val3, Val4, Val5, Val6;
	int nb_point;
	int i;
	
	// Initialisation des variables
	Val1=0;Val2=0;Val3=0;Val4=0;Val5=0;Val6=0;
	Brelan=0;Carre=0;Full=0;Yams=0;Double=0;Chance=0;

	// Parcours du tableau des dés pour classer les différentes valeurs obtenues
	for(i=0;i< NB_DES;i++){
		switch (tab_des[i]){
		   case 1: Val1++;break;
		   case 2: Val2++;break;
		   case 3: Val3++;break;
		   case 4: Val4++;break;
		   case 5: Val5++;break;
		   case 6: Val6++;break;
		}
		Chance = Chance + tab_des[i];
	}

	// Calcul du score
	// le YAMS consiste à avoir les 5 dés d'une même valeur
	if (Val6 == 5 || Val5 == 5 || Val4 == 5 || Val3 == 5 || Val2 == 5 || Val1 == 5){
	   Yams = 1;
	   nb_point = Chance + PT_YAMS;
	}   
	// le CARRE consiste à avoir 4 dés d'une même valeur
	if (Val6 == 4 || Val5 == 4 || Val4 == 4 || Val3 == 4 || Val2 == 4 || Val1 == 4){
	   Carre = 1;
	   nb_point = Chance + PT_CARRE;
	}
	// le BRELAN consiste à avoir 3 dés d'une même valeur
	if (Val6 == 3 || Val5 == 3 || Val4 == 3 || Val3 == 3 || Val2 == 3 || Val1 == 3){
	   Brelan = 1;
	   nb_point = Chance + PT_BRELAN;
	}
	if (Val6 == 2 || Val5 == 2 || Val4 == 2 || Val3 == 2 || Val2 == 2 || Val1 == 2){
	   Double = 1;
	   // Le FULL consiste à avoir un brelan et un double
	   if (Brelan == 1){
	      Full = 1;
	      nb_point = Chance + PT_FULL;
	   }
	}
	if ((Yams != 1 && Brelan != 1 && Full != 1 && Carre != 1)&&
	   (Val6 <= 2 || Val5 <= 2 || Val4 <= 2 || Val3 <= 2 || Val2 <= 2 || Val1 <= 2)){
	         nb_point = Chance ;
	}

	// Remplissage du tableau des scores
	tab_score[numPartie][numJoueur] = nb_point;
	tab_score[NB_PARTIES+1][numJoueur] = tab_score[NB_PARTIES+1][numJoueur] + nb_point;

	//Afficher les scores aux joueurs
	//afficher_score(tab_score);
   
}

/*void afficher_score(int tab_score){
	char partie;
	//Concatener chaque ligne du tableau en une chaîne de caractère
	//Afficher la ligne de la partie en cours + les lignes précédentes
	for (k=1;k <= numPartie;k++){
	   sprintf(partie,"partie %d : ",numPartie);
	   for (j=1;j<= numJoueur;j++){
	      strcat(partie, tab_score[k][j]);			
	   }
	   strcat(partie, "\n");	
	}
	// Afficher la ligne du total
	for (j=1;j<= numJoueur;j++){
	   strcat(partie,tab_score[NB_PARTIE][j]);
	}
}*/

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


















