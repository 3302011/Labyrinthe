#include <stdio.h>
#include <stdlib.h>
#include "labyrinthAPI.h"
#include <unistd.h>
#include <time.h>

extern int debug;	/* hack to enable debug messages */

///////////////// STRUCTURE POUR GARDER EN MEMOIRE LES INFORMATIONS IMPORTANTES  //////////////////////
/*
Prémices: Dans notre programme nous avons décidé de passer par les structures et l'allocation dynamique. Nous avons crée une structure dans un premier temps. Cette structure a été faite  afin de garder une trace des grandes variables du jeu c'est-à-dire le labyrinthe, la taille du labyrinthe, la position du joueur, la position du trésor et la position de l'adversaire. 

 */

 typedef struct labyrinthe
	    {
	      char * lab;
	      int sizeX;
	      int sizeY;
	      int position_x; 
	      int position_y; 	
	      int position_tresor_x; 
	      int position_tresor_y; 
	      int position_adversaire_x; 
	      int position_adversaire_y; 

	    }t_labyrinthe; 


////////////// UTILISATION DE LA STRUCTURE POUR GARGER EN MEMOIRE LES INFORMATIONS SUR LE LABYRINTHE //////
/*
Prémices: La première étape est de "former" le labyrinthe. La labyrinthe en lui même a déjà été crée. 3 fonctions fournies permettent de récupérer les données du labyrinthe. WaitForLabyrinth permet de récupérer le nom et la taille (ainsi que d'attenre une partie), getLabyrinth permet de récupérer les données initiales c'est-à-dire de remplir la structure avec des 0 si les cases sont vides et 1 si elles correspondent à des murs. Enfin la fonction printLabyrinth permet d'afficher le labyrinthe.  
Notre but ici va être de créer de la place en mémoire grâce à l'allocation dynamique pour un labyrinthe que l'on a appelé labData ainsi qu'utiliser la structure définie au préalable pour définir les positions initiales des joueurs et du trésor dans le labyrinthe. En réalité ceci était déjà fait mais utiliser cette structure nous donne la possibilité d'avoir les modifications à jours. 

 */

int mylabyrinthe(t_labyrinthe* current) 
    {
   
      int sizeX,sizeY;
      char labName[50];
      waitForLabyrinth("DO_NOTHING timeout=100 rotation=False", labName, &sizeX, &sizeY);
      char* labData;
      labData = (char*) malloc( sizeX * sizeY );
      int player= getLabyrinth(labData); 

       current->lab=labData;
       current->sizeX=sizeX; 
       current->sizeY=sizeY; 
       current->position_x=0; 
       current->position_y=sizeY/2; 
       current->position_tresor_x=sizeX/2; 
       current->position_tresor_y=sizeY/2;
       current->position_adversaire_x=sizeX-1; 
       current->position_adversaire_y=sizeY/2; 
       return player;   
 }

///////////////////// FONCTION QUI PERMET A NOTRE JOUEUR DE SE DEPLACER ///////////////////
/*
Prémices: Une fois le labyrinthe défini, il nous est temps de passer au mouvement du joueur. C'est là où la structure crée va être particulièrement importante. Dans un premier temps, il nous faut créer des mouvements aléatoires. On décide de générer un nombre aléatoire entre 4 et 7. Ces nombres ont été choisis en concordance avec ce qui était défini par défaut. Une fois le nombre généré, si il s'agit de 4 et qu'il n'y a pas de mur le joueur se déplace vers le haut, de même si il s'agit d'un 5 et qu'il n'y a pas de mur il se déplace vers le bas, si c'est un 6 vers la gauche et si c'est un 7 vers la droite. On ajoute également une option due à la rotatibilité du labyrinthe qui est que si le joueur se retrouve à l'extrémité il lui est possible de passer de l'autre côté.  


 */

void make_move(t_labyrinthe* current, t_move *move)
{ 

  int test=0; 
  int deplacement=0; // permet de tester les conditions de positivités pour l'entrée dans la boucle

  while(test!=1)
    {
      move->type=rand()%4 + 4;  // va générer un nombre aléatoire entre 4 et 7. Une fois le chiffre généré, si il correspond à 4 le déplacement ce fera vers le haut 
 
      if(move->type==4)
	{
	  if(current->position_y-1<0)
	    deplacement=current->sizeY-1;
	  else 
	    deplacement=current->position_y-1; 

	    // on essaie aller en haut, donc on verifie si la case [position_x -1, position_y] est valide
	    if(current->lab[deplacement*current->sizeX + current->position_x]==0)
	      {
		current->position_y=deplacement; 
		move->type = MOVE_UP; 
		test=1; 
	      }
	  
	}
      
      
      if(move->type==6) 
	{

	  if(current->position_x-1<0)
	    deplacement=current->sizeX-1;
	  else 
	    deplacement=current->position_x-1; 


	  if(current->lab[(current->position_y) *current->sizeX + deplacement]==0)
	    {
	      current->position_x=deplacement;	      
	      move->type=MOVE_LEFT; 
	      test=1; 
	    }
	  
	}
      
      
      if(move->type==7)
	{      

	  if(current->lab[(current->position_y) *current->sizeX + (current->position_x+1)%(current->sizeX)]==0)
	    {
	      current->position_x= (current->position_x+1)%(current->sizeX);	      
	      move->type=MOVE_RIGHT; 
	      test=1; 
	    }
	  
	}
      
      
      if(move->type==5)
	{
	  if(current->lab[(current->position_y+1)%(current->sizeY) *current->sizeX + current->position_x]==0)
	    {
	      current->position_y=(current->position_y+1)%(current->sizeY);
	      
	      move->type=MOVE_DOWN;
	  test=1; 
	    }
	  
	}
    }

  printf("vide ou pas:%d %d %d",current->lab[(current->position_y*current->sizeX+current->position_x)], current->position_x, current->position_y); 

}



///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////// MAIN FUNCTION ///////////////////////////////////////////////////////

int main()
{
   srand(time(NULL)); 
	
   /* name of the labyrinth */
   debug=1;// par defaut donne les infos les plus importantes 
        int player; 						/* data of the labyrinth */
	t_return_code ret = MOVE_OK;		/* indicates the status of the previous move */
	t_move move;						/* a move */
	t_labyrinthe* Laby=(t_labyrinthe*)malloc(sizeof(t_labyrinthe));  
	
	/* connection to the server */
	connectToServer( "pc4001.polytech.upmc.fr", 1234,"Polyuser01");
	
	
	/* wait for a game, and retrieve informations about it */
         
	player=mylabyrinthe(Laby); 	
	
	do {
	  
	  /* display the labyrinth */
	  printLabyrinth();


	  if (player==1)	/* The opponent plays */
	    {
		ret = getMove( &move);
		//playMove( &lab, move); 
		player=0; 
	      }
	    
	    else
	      {
		make_move(Laby,&move); 
		printf("move %d %d %d \n",move.type, Laby->position_x,Laby->position_y); 
		ret=sendMove(move);
		
		player=1; // afin que chacun joue à son tour 
	      }
	  
	    
	}while(ret==MOVE_OK);  // tant que la partie n'est pas finie on joue 


	if ((player ==1 && ret == MOVE_WIN) || (player==0 && ret == MOVE_LOSE))
	  printf("I lose the game\n");
	
	/* we do not forget to free the allocated array */
	free(Laby);
	
	
	/* end the connection, because we are polite */
	closeConnection();


	return EXIT_SUCCESS;
}

///////////////////////////////////////////// END OF THE PROGRAM ////////////////////////////////////
