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
#define PT_YAMS 50
#define PT_CARRE 40
#define PT_BRELAN 10
#define PT_FULL 30

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
void calculer_score(int numJoueur, int numPartie, int tab_des[], int tab_score[NB_PARTIES+1][NB_JOUEURS]);
void initialiser_tab_des(int tab_des[]);
void initialiser_tab_score (int tab_score[NB_PARTIES+1][NB_JOUEURS]);

int main()
{
    
    char 			*message;
    joueur 			joueurs[NB_JOUEURS];
    char 			messageJoueur[25];
    int				tab_des[5];
    int				tab_score[NB_PARTIES+1][NB_JOUEURS];
    int				numJoueur;
	int			numPartie; 

	initialiser_tab_score(tab_score);	
	lancer_des(joueurs,tab_des);
	calculer_score(numJoueur,numPartie,tab_des,tab_score);
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
		// on remplit le tableau avec 5 valeurs aléatoire comprises entre 1 et 6
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
	for (i=0; i<= NB_PARTIES+1;i++)
	{
	   for (j=0; j<NB_JOUEURS; j++)
	   {
	      tab_score[i][j] = 0;	
	   }	
	}
	printf("total : %d ", tab_score[NB_PARTIES+1][NB_JOUEURS]); 
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
	printf("valeur 1 : %d \n",Val1);
	printf("valeur 2 : %d \n",Val2);
	printf("valeur 3 : %d \n",Val3);
	printf("valeur 4 : %d \n",Val4);
	printf("valeur 5 : %d \n",Val5);
	printf("valeur 6 : %d \n",Val6);
	printf("chance : %d \n",Chance);

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
	printf(" nombre de point : %d ", nb_point);
	numPartie = 1;
	numJoueur = 2;
	// Remplissage du tableau des scores
	tab_score[numPartie][numJoueur] = nb_point;
	printf(" total joueur : %d ", tab_score[NB_PARTIES+1][numJoueur]);
	tab_score[NB_PARTIES+1][numJoueur] = tab_score[NB_PARTIES+1][numJoueur] + nb_point;
	
	printf("Resultat joueur : %d ", tab_score[numPartie][numJoueur]);
	printf("total : %d ",tab_score[NB_PARTIES+1][numJoueur]);
   
}
