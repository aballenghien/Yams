//test lancé dés + calcul score

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

typedef struct 
{
	int numeroJ;
	pthread_t thread_joueur;
	int socket;
}
joueur;


 
//les fonctions

//void jouer_partie_yams(joueur joueurs[]);
void lancer_des(joueur joueurs[], int tab_des[]);
//void calculer_score(int numJoueur, int numPartie, int *tab_des, int *tab_score);
void initialiser_tab_des(int tab_des[]);
void initialiser_tab_score (int tab_score[NB_PARTIES+1][NB_JOUEURS]);

int main()
{
    
    char 			*message;
    joueur 			joueurs[NB_JOUEURS];
    char 			messageJoueur[25];
    int				tab_des[5];
    int				tab_score[NB_PARTIES+1][NB_JOUEURS];

	initialiser_tab_score(tab_score);	
	lancer_des(joueurs,tab_des);
}

void lancer_des(joueur joueurs[], int tab_des[]){
	char tour[26];
	//char *message, client_message[1];
	int i;
	char lance_de_des[19];
	char messageJoueur[25];
	//int ok;
	
	/* on attend que le joueur soir prêt pour lancer les dés
	ne fonctionne pas pour l'instant
	for (i = 0; i < NB_JOUEURS; i++)
	{
		sprintf(tour, "C'est le tour du joueur %d", numJoueur);
		write(joueurs[i].socket, tour, strlen(tour));
	}
	message = "\n C'est à votre tour de lancer les dés, êtes vous prêt?Y/N \n";
	write(joueurs[numJoueur-1].socket, message, strlen(message));
	ok = 0;
	do{
		
		read_size = recv(joueurs[numJoueur-1].socket, client_message, 1,0);
		if(read_size > 0)
		{
			printf("réponse");
			toupper(client_message);
			if(strcmp(client_message, "Y")==0)
			{			
				ok = 1;
			}else{
				message = "êtes vous prêt?Y/N \n";
				write(joueurs[numJoueur-1].socket, message, strlen(message));
			}
		}
	}while(ok == 0);
	
	if(ok == 1)
	{*/
		// on remplit le tableau avec 5 valeurs aléatoire comprises en tre 1 et 6
		initialiser_tab_des(tab_des);
		for(i = 0; i < NB_DES; i++)
		{
			tab_des[i] = rand()%(NB_VALEUR_DE-1)+1;
			//sprintf(lance_de_des,"%d",tab_des[i]);
			//strcat(lance_de_des, "|");
		}
		
		//on affiche le tableau côté client
		//for (i = 0; i < NB_JOUEURS; i++)
		for (i = 0; i<NB_DES;i++)
		{
			//strcpy(messageJoueur,"Résultat du lancer: du joueur \n");
			//printf("%s \n",messageJoueur);
			printf("%d |",tab_des[i]);			
			//printf("%s",lance_de_des);			
			//write(joueurs[i].socket, message, strlen(message));
			//write(joueurs[i].socket, lance_de_des, strlen(lance_de_des));
		}
		
		
	//}
	
}

void initialiser_tab_des(int tab_des[])
{
	int i;
	for(i = 0; i < NB_DES; i++)
	{
		tab_des[i] = 0;
	}
	
}

void initialiser_tab_score (int tab_score[NB_PARTIES+1][NB_JOUEURS])
{
	int i,j ; // parcourir le tableau à deux dimensions
	for (i=0; i<= NB_PARTIES;i++)
	{
	   for (j=0; j<NB_JOUEURS; j++)
	   {
	      tab_score[i][j] = 0;
              printf("%d ",tab_score[i][j]);	
	   }	
	}
}
