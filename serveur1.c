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
#define NB_PARTIES 2
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


 
// liste des fonctionnalités présentes
void *connection_handler(void *);
void jouer_partie_yams(joueur joueurs[], char *buffer);
void lancer_des(joueur joueurs[], int numJoueur, int tab_des[], int numPartie);
void calculer_score(joueur joueurs[], int numJoueur, int numPartie, int tab_des[], int tab_score[NB_PARTIES+1][NB_JOUEURS]);
void initialiser_tab_des(int tab_des[]);
void initialiser_tab_score(int tab_score[NB_PARTIES+1][NB_JOUEURS]);
void afficher_score(joueur joueurs[],int numPartie, int numJoueur, int tab_score[NB_PARTIES+1][NB_JOUEURS]);
int read_client(int sock, char *buffer);
void initialiser_ligne_score(char ligne_scores[]);
void afficher_des(int tab_des[], joueur joueurs[]);
void initialiser_lance_de_des(char lance_de_des[]);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
	hostent*		ptr_hote; 			/* les infos recuperees sur la machine hote */
    servent*		ptr_service; 			/* les infos recuperees sur le service de la machine */
    char 			machine[TAILLE_MAX_NOM+1]; 	/* nom de la machine locale */
    int 			nb_connexions;
    char 			buffer[BUF_SIZE];
    joueur 			joueurs[NB_JOUEURS]; // tableau des joueurs connectés
    char 			messageJoueur[25]; // Assignation du numéro du joueur
    char			partie_terminee[23];
    fd_set 			rdfs;
    int i;

    gethostname(machine,TAILLE_MAX_NOM);		/* recuperation du nom de la machine */
    
    /* recuperation de la structure d'adresse en utilisant le nom */
    if ((ptr_hote = gethostbyname(machine)) == NULL) {
		perror("Erreur : impossible de trouver le serveur a partir de son nom.\n");
		exit(1);
    }
     
    //Création du socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Le socket n'a pas pu être créé\n");
    }
    puts("Socket créé\n");
     
	bcopy((char*)ptr_hote->h_addr, (char*)&server.sin_addr, ptr_hote->h_length);
    server.sin_family		= ptr_hote->h_addrtype; 	
    server.sin_addr.s_addr	= INADDR_ANY; 			

    //Configuration ud socket
    server.sin_port = htons( 5000 );

    printf("Numero de port pour la connexion au serveur : %d\n", ntohs(server.sin_port));
     
    //Liaison du socket au serveur
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("Echec de la liaison\n");
        return 1;
    }
    puts("Liaison effectuée\n");
     
    //Ecoute et attente de connexions
    listen(socket_desc , 3);
      
     
    //Acceptation des connexions entrantes
    puts("Attente des connexions...\n");
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	nb_connexions = 0;
	// réception d'une connexion, création d'un thread et association du socket
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)))
    {
		puts("Connexion en cours de traitement\n");
        // créer des thread et les rattacher au joueur tant que le nombre de joueurs n'est pas atteint
        if (nb_connexions < NB_JOUEURS){
			if( pthread_create( &joueurs[nb_connexions].thread_joueur , NULL ,  connection_handler , (void*) &client_sock) < 0)
			{
				perror("Le thread n'est pas créé\n");
				return 1;
			}
			
			// on attribue un socket, un thread et un numéro à chaque joueur puis on leur annonce
			joueurs[nb_connexions].socket = client_sock;
			joueurs[nb_connexions].numeroJ = nb_connexions+1;
			sprintf(messageJoueur,"Vous êtes le joueur : %d\n",nb_connexions+1);
			write(client_sock,messageJoueur, strlen(messageJoueur));
			
	}else{
			// indiquer au client que le nombre de joueur est atteint
			sprintf(buffer, "erreur");
			write(client_sock,buffer, strlen(buffer));
			sprintf(buffer,"Une partie est déjà commencée\n");
			write(client_sock,buffer, strlen(buffer));
		}        
        
		puts("Connexion établie\n");
        nb_connexions = nb_connexions + 1;
        if(nb_connexions == NB_JOUEURS){
			// débuter la partie une fois que tous les joueurs ont rejoint la partie
			sleep(5);
			jouer_partie_yams(joueurs, buffer);
			sprintf(partie_terminee,"La partie est terminée\n");
			for(i=0; i< NB_JOUEURS; i++)
			{
				write(joueurs[i].socket,partie_terminee, strlen(partie_terminee));
			}	
			exit(0);
		}
    }
    
    
    if (client_sock < 0)
    {
        perror("Erreur lors de la connexion\n");
        return 1;
    }
    
 
    return 0;
}
 
/*
 * Fonction pour établir la connexion
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
	char delim_partie[75];
	fd_set rdfs;	
	int i;
	
	if (NB_JOUEURS > 1)
	{
		for(i=0; i< NB_JOUEURS; i++)
		{
			sprintf(buffer,"Bonjour tout le monde\n");
			write(joueurs[i].socket,buffer, strlen(buffer));
		}
	}
	// Initialiser le tableau des scores
	initialiser_tab_score(tab_score);
	//les joueurs joueront  en tout 10 fois chacun leur tour 
	for(numPartie = 0; numPartie < NB_PARTIES; numPartie++)
	{
		for (numJoueur = 0; numJoueur < NB_JOUEURS; numJoueur++)
		{		
			// remplissage du tableau avec des valeurs aléatoire du dés
			lancer_des(joueurs, numJoueur, tab_des, numPartie+1);
			calculer_score(joueurs, numJoueur,numPartie,tab_des,tab_score);
			//Afficher les scores aux joueurs
			afficher_score(joueurs,numPartie,numJoueur,tab_score);
		}
		for(i=0; i< NB_JOUEURS; i++)
		{
			sprintf(delim_partie,"-----------------------------------------------------------------------\n");
			write(joueurs[i].socket,delim_partie, strlen(delim_partie));
		}
	}
	
	
}

void lancer_des(joueur joueurs[], int numJoueur, int tab_des[], int numPartie){
	char tour[20];
	char *buffer,  bufferTest[66];
	int i, read_size;
	int k;
	char lance_de_des[1];
	int ok;
	
	// on attend que le joueur soit prêt pour lancer les dés
	if( NB_JOUEURS > 1)
	{
		for (i = 0; i < NB_JOUEURS; i++)
		{
			sprintf(tour, "Partie n°%d: C'est le tour du joueur %d\n", numPartie, joueurs[numJoueur].numeroJ);
			write(joueurs[i].socket, tour, strlen(tour));
			tour[25] = 0;
		}
	}
	sprintf(bufferTest, "C'est à votre tour de lancer les dés, êtes vous prêt?Y/N\n");
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
				buffer = "Etes vous prêt?Y/N\n";
				write(joueurs[numJoueur].socket, buffer, strlen(buffer));
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
		}
		afficher_des(tab_des, joueurs);
	}
	
}

void afficher_des(int tab_des[],joueur joueurs[])
{

		int k,i;
		char lance_de_des[0];
		char buffer[31];
		char valeur_de[2];
		lance_de_des[0] = 0;
		for(k = 0; k < NB_DES; k++)
		{
			sprintf(valeur_de,"%d|",tab_des[k]);
			strcat(lance_de_des,valeur_de);
			
		}
		strcat(lance_de_des,"\n");
		//on affiche le tableau côté client
		for (i = 0; i < NB_JOUEURS; i++)
		{
			sprintf(buffer,"Résultat du lancé du joueur:\n");
			write(joueurs[i].socket, buffer, strlen(buffer));
			write(joueurs[i].socket, lance_de_des, strlen(lance_de_des));
		}
		printf("taille du tableau lance_de_des %d \n",strlen(lance_de_des));
		// contrôle coté serveur
		printf("dernier lancé : ");
		for(k = 0; k < NB_DES; k++)
		{
			printf("%d|",tab_des[k]); 			
		}
		printf("\n");
}

void initialiser_tab_score(int tab_score[NB_PARTIES+1][NB_JOUEURS])
{
	int i,j ; // parcourir le tableau à deux dimensions
	for (i=0; i<= NB_PARTIES;i++)
	{
	   for (j=0; j<NB_JOUEURS; j++)
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

void calculer_score(joueur joueurs[],int numJoueur, int numPartie, int tab_des[], int tab_score[NB_PARTIES+1][NB_JOUEURS])
{
	int brelan, carre, full, yams, doubles, chance;
	int val1, val2, val3, val4, val5, val6;
	int nb_point;
	int i;
	char points[3];
	char buffer[43];
	
	// Initialisation des variables
	val1=0;val2=0;val3=0;val4=0;val5=0;val6=0;
	brelan=0;carre=0;full=0;yams=0;doubles=0;chance=0;
	nb_point = 0;
	// Parcours du tableau des dés pour classer les différentes valeurs obtenues
	for(i=0;i< NB_DES;i++){
		switch (tab_des[i]){
		   case 1:val1++;break;
		   case 2:val2++;break;
		   case 3:val3++;break;
		   case 4:val4++;break;
		   case 5:val5++;break;
		   case 6:val6++;break;
		   
		}
		chance = chance + tab_des[i];
	}

	// Calcul du score
	// le yams consiste à avoir les 5 dés d'une même valeur
	if ((val6 == 5) || (val5 == 5) || (val4 == 5) || (val3 == 5) || (val2 == 5) || (val1 == 5)){
	   yams = 1;
	   nb_point = chance + PT_YAMS;
	   for(i=0; i< NB_JOUEURS; i++)
		{
			sprintf(buffer,"++++++++++++++++YAMS !!!+++++++++++++++\n");
			write(joueurs[i].socket,buffer, strlen(buffer));
		}
	}   
	// le CARRE consiste à avoir 4 dés d'une même valeur
	if ((val6 == 4) || (val5 == 4) || (val4 == 4) || (val3 == 4) || (val2 == 4) || (val1 == 4)){
	   carre = 1;
	   nb_point = chance + PT_CARRE;
	   for(i=0; i< NB_JOUEURS; i++)
		{
			sprintf(buffer,"+++++++++++++++CARRE !!!+++++++++++++++\n");
			write(joueurs[i].socket,buffer, strlen(buffer));
		}
	}
	// le BRELAN consiste à avoir 3 dés d'une même valeur
	if ((val6 == 3) || (val5 == 3) || (val4 == 3) || (val3 == 3) || (val2 == 3) || (val1 == 3)){
	   brelan = 1;
	   nb_point = chance + PT_BRELAN;
	   for(i=0; i< NB_JOUEURS; i++)
		{
			sprintf(buffer,"+++++++++++++++BRELAN !!!+++++++++++++++\n");
			write(joueurs[i].socket,buffer, strlen(buffer));
		}
	}
	if ((val6 == 2) || (val5 == 2) || (val4 == 2) || (val3 == 2) || (val2 == 2) || (val1 == 2)){
	   doubles = 1;
	   // Le FULL consiste à avoir un brelan et un double
	   if (brelan == 1){
	      full = 1;
	      nb_point = chance + PT_FULL;
	      for(i=0; i< NB_JOUEURS; i++)
			{
				sprintf(buffer,"+++++++++++++++FULL !!!+++++++++++++++\n");
				write(joueurs[i].socket,buffer, strlen(buffer));
			}
	   }
	}
	if (((yams != 1) && (brelan != 1) && (full != 1) && (carre != 1))&&
	   ((val6 <= 2) || (val5 <= 2) || (val4 <= 2) || (val3 <= 2) || (val2 <= 2) || (val1 <= 2))){
	         nb_point = chance ;
	}
	
	
	// Remplissage du tableau des scores
	tab_score[numPartie][numJoueur] = nb_point;
	tab_score[NB_PARTIES][numJoueur] = tab_score[NB_PARTIES][numJoueur] + nb_point;
   
}

void afficher_score(joueur joueurs[],int numPartie, int numJoueur, int tab_score[NB_PARTIES+1][NB_JOUEURS])
{
	int l; // parcours de la ligne de la partie en cours
	int c; // parcours de chaque client
	char partie[10];
	char ligne_scores[NB_JOUEURS*8];
	char score[4];
	char total[6];
	// affichage côté client
	for (l = 0; l <= numPartie; l++){
		initialiser_ligne_score(ligne_scores);
		for (c = 0; c < NB_JOUEURS; c++)
		{	
			sprintf(score,"%d|", tab_score[l][c]);		
			strcat(ligne_scores, score);
		}
		strcat(ligne_scores, "\n");
		for(c = 0; c < NB_JOUEURS; c ++)
		{
			partie[0] = 0;
			sprintf(partie,"Partie %d:",l+1);	
			write(joueurs[c].socket, partie, strlen(partie));
			write(joueurs[c].socket, ligne_scores, strlen(ligne_scores));
		}
		
	}
	//contrôle côté serveur
	for (l = 0; l <= numPartie; l++){
		printf("partie %d:", l+1);
		for (c = 0; c < NB_JOUEURS; c++)
		{	
			printf("%d |",tab_score[l][c]);		
			
		}
		printf("\n");
	}
	initialiser_ligne_score(ligne_scores);
	for (c = 0; c < NB_JOUEURS; c++)
	{	
		sprintf(score,"%d|", tab_score[NB_PARTIES][c]);		
		strcat(ligne_scores, score);
	}
	strcat(ligne_scores, "\n");
	for(c = 0; c < NB_JOUEURS; c ++)
	{
		sprintf(total,"Total:   ");
		write(joueurs[c].socket, total, strlen(total));
		write(joueurs[c].socket, ligne_scores, strlen(ligne_scores));
	}
	
	
}

void initialiser_ligne_score(char ligne_scores[])
{
	int i;
	for(i = 0; i < NB_JOUEURS*2; i++)
	{
		ligne_scores[i] = 0;
	}
}

void initialiser_lance_de_des(char lance_de_des[])
{
	int i;
	for(i = 0; i < NB_JOUEURS*2; i++)
	{
		lance_de_des[i] = 0;
	}
}

int read_client(int sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, sizeof(buffer), 0)) < 0)
   {
      perror("recv()");
      //si le message reçu est une erreur : déconnexion
      n = 0;
   }

   buffer[n] = 0;

   return n;
}


















