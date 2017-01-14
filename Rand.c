#include <stdio.h>
#include <stdlib.h>
#include "labyrinthAPI.h"
#include <unistd.h>
#include <time.h>

extern int debug;/* hack to enable debug messages */


//////////////////////////////////////////////////////////////////////////// MISE EN SITUATION /////////////////////////////////////////////////////////////////////////////////
/*
Bienvenue dans le jeu du Labyrinthe Rand crée par Fatine Bentires Alj et Marie-Claire Heng. Ce jeu a pour but de modéliser un déplacement aléatoire d'un joueur vers un trésor. Le trésor ce trouve au milieu du Labyrinthe et le but est de le récupérer en évitant les murs dont le Labyrinthe est constitué. Notre programme est constitué de 3 parties importantes. La première correspond à la structure qui va garder en mémoire les informations du Labyrinthe. La seconde correspond aux fonctions que l'on utilise afin que le joueur puisse se déplacer et enfin la dernière correspond à la fonction MAIN qui va modéliser les commandes à effectuer en faisant appel aux autres fonctions. 


*/


/////////////////////////////////////////////////////////////////////////// PARTIE UNE : STRUCTURE //////////////////////////////////////////////////////////////////////////////

/*
Prémices: Dans notre programme nous avons décidé de passer par les structures et l'allocation dynamique avec pour but de faire des structures que nous pourrions utiliser pour le A star.
Cette structure a été faite afin de garder une trace des grandes variables du jeu c'est-à-dire le labyrinthe, la taille du labyrinthe, la position du joueur, la position du trésor et la position de l'adversaire. Ces informations sont indispensables afin de reconstituer notre Labyrinthe dans un premier temps et permettre un déplacement du joueur dans un second temps. 

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




/////////////////////////////////////////////////////////////////////////   PARTIE DEUX : LES FONCTIONS ////////////////////////////////////////////////////////////////////////

/*
Cette partie peut être départagée en deux sous parties. Dans un premier temps nous avons crée une fonction de type t_labyrinthe afin de reconstituer notre Labyrinthe et dans un second temps nous avons crée une fonction make_move qui va permettre au joueur de se déplacer.

*/





//////////////////////////////////////////////////////// PARTIE DEUX A : LE LABYRINTHE  /////////////////////////////////////////////////////////////////////
/*
Prémices: Cette fonction va nous permettre de reconstituer les "grandes variables" du Labyrinthe. Mais commençons par rappeler les fonctions fournies et qui nous sont utiles. Nous avons 3 fonctions qui nous permettent de récuperer des informations sur le labyrinthe nécessaires dans cette partie: WaitForLabyrinthe qui va nous permettre de récupérer le nom ainsi que la taille du labyrinthe, getLabyrinth qui permet de définir 0 pour case vide et 1 pour murs et qui permet de renvoyer des informations sur qui commence et enfin printLabyrinth qui va permettre d'afficher notre Labyrinthe. Nous avons donc crée fonction de type t_labyrinthe qui va permettre de récuperer toutes les informations fournies par ces trois fonctions. Dans un premier temps nous faisons appel à cette fonction waitForLabyrinthe puis définissons par allocation dynamique une place pour notre labyrinthe que nous appelons labData. Une fois la place définie, nous utilisons notre structure pour indiquer que le Labyrinthe à considérer est labData, puis définissons les différentes positions des joueurs et du trésor conformément aux instructions données dans le Mini-Projet. Enfin nous définissons une variable player qui va renvoyer 0 si on commence et 1 si c'est l'adversaire qui commence. Le but de cette fonction est donc de définir les variables de bases ainsi que de renvoyer player. Pour l'algorithme random il aurait tout à fait été possible d'effectuer le même code sans structures mais le but était de nous avancer pour le A*.
 

*/

int mylabyrinthe(t_labyrinthe* current) 
{
   
  int sizeX,sizeY;
  char labName[50];
  waitForLabyrinth("DO_NOTHING timeout=100 rotation=True", labName, &sizeX, &sizeY);
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



//////////////////////////////////////////////////////// PARTIE DEUX B : LE DEPLACEMENT //////////////////////////////////////////////////////////////////////////
/*
Prémices: Une fois le labyrinthe définit, il nous est temps de passer aux mouvements du joueur. C'est là où la structure crée va être particulièrement importante. Dans un premier temps, il nous faut créer des nombres aléatoires. Pour cela on utilise la commande rand()%4 + 4 qui va nous générer des nombres aléatoires entre 4 et 7. Le 4 correspond au déplacement vers le haut, le 5 vers le bas, le 6 vers la gauche et le 7 vers la droite comme il est indiqué dans les instructions du projet. Le 8 correspond au do_nothing donc aucun déplacement et nous avons décidé qu'il ne servait à rien que notre rand ne fasse rien donc il n'est pas la peine de générer un nombre aléatoire pour ce dernier. Une fois le nombre généré, on demande au joueur de se déplacer si la case est vide. On ajoute également une option due à la rotatibilité du labyrinthe. Si le joueur se retrouve à l'extrémité il lui est possible de passer de l'autre côté grâce à la fonction modulo.

*/

void make_move(t_labyrinthe* current, t_move *move, int energy) // prend en entrée la structure ainsi que t_move qui nous permet d'utiliser MOVE_UP/RIGHT/DOWN/LEFT
{ 

  int test=0; // variable à 0 qui va nous permettre de rentrer dans la boucle
  int deplacement=0; // permet de tester les conditions de positivités pour les rotations
  int boucle=0; 
  char* Laby; 

  while(test!=1)
    {
      move->type=rand()%9;
        
      if(move->type==4)
	{
	  if(current->position_y-1<0) // si le déplacement vers le haut qu'on demande entraine une position négative
	    deplacement=current->sizeY-1; // on se déplace de l'autre côté du labyrinthe
	  else 
	    deplacement=current->position_y-1; // sinon on se déplace

	  // on essaie aller en haut, donc on verifie si la case [position_x -1, position_y] est valide
	  if(current->lab[deplacement*current->sizeX + current->position_x]==0)
	    {
	      current->position_y=deplacement; 
	      move->type = MOVE_UP;
	      energy++; 
	      test=1;
	    }
	    
	}
      
        
      if(move->type==5)
        {
	  if(current->lab[(current->position_y+1)%(current->sizeY) *current->sizeX + current->position_x]==0)
            {
	      current->position_y=(current->position_y+1)%(current->sizeY);
                
	      move->type=MOVE_DOWN;
	      energy++; 
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
              energy++; 
	      test=1; 
	    }
	    
	}
      
      
      if(move->type==7)
	{      

	  if(current->lab[(current->position_y) *current->sizeX + (current->position_x+1)%(current->sizeX)]==0)
	    {
	      current->position_x= (current->position_x+1)%(current->sizeX);      
	      move->type=MOVE_RIGHT; 
	      energy++;
	      test=1; 
	    }
	    
	}
      
      
      if(move->type==8)
	{
	        
	  energy++;  
	  test=1; 
        }
        

      if(move->type==0)
	{
	  if(energy>=5){
	    move->type=ROTATE_LINE_LEFT;
	    move->value=rand()%current->sizeY; 
	    boucle=0; 
	    while(boucle<current->sizeX-1)
	      {
		current->lab[move->value*current->sizeX + boucle]=current->lab[move->value*current->sizeX +boucle+1];
		boucle++;
	      }
	    Laby=current->lab[move->value*current->sizeX + current->sizeX-1];
	    getLabyrinth(Laby); 

	    energy=energy-5; 
	    test=1;  }

	  else 
	    move->type=rand()%9; 

	}


      if(move->type==1)
	{
	  if(energy>=5){
	    move->type=ROTATE_LINE_RIGHT;
	    move->value=rand()%current->sizeY; 
	    boucle=0; 

	    while(boucle<current->sizeX-1)
	      {
		current->lab[move->value*current->sizeX+ boucle]=current->lab[move->value*current->sizeX +boucle-1];
		boucle--; 
	      }
	    Laby=current->lab[move->value*current->sizeX];
	    getLabyrinth(Laby); 
	    energy=energy-5; 
	    test=1; }

          else 
	    move->type=rand()%9; 

	}


      if(move->type==2)
	{
          if(energy>=5)
	    {move->type=ROTATE_COLUMN_UP;
	      move->value=rand()%current->sizeX; 

	      boucle= 0;
	      while(boucle< current->sizeY-1)
		{
		  current->lab[boucle*current->sizeX + move->value]=current->lab[(boucle+1)*current->sizeX + move->value];
		  boucle++; 
		}
	      Laby=current->lab[(current->sizeY-1)*current->sizeX + move->value];
	      getLabyrinth(Laby); 

	      energy=energy-5; 
	      test=1; }
	  else 
	    move->type=rand()%9; 

	}



      if(move->type==3)
	{
	  if(energy>=5){
	    move->type=ROTATE_COLUMN_DOWN; 
	    move->value=rand()%current->sizeX; 

        
	    boucle=current->sizeY-1;
	    while(boucle>0)
	      {
		current->lab[boucle*current->sizeX + move->value]=current->lab[(boucle-1)*current->sizeX + move->value];
		boucle--;
	      }
	    Laby=current->lab[move->value]; 
	    getLabyrinth(Laby); 
	    energy=energy-5; 
	    test=1; }

	  else 
	    move->type=rand()%9; 

	}


      printf("vide ou pas:%d %d %d",current->lab[(current->position_y*current->sizeX+current->position_x)], current->position_x, current->position_y); 
    }


}



/////////////////////////////////////////////////////////////////// TROISIEME PARTIE : MAIN FUNCTION ///////////////////////////////////////////////////////////////////////////////

/*
 
 Cette fonction est l'une des fonctions les plus importantes. C'est avec cette fonction main que l'on va faire appel aux deux fonctions crées précédement. Cette fonction MAIN était en partie fournie. Nous avons ajouté quelques commandes que nous avons détaillé plus bas.
 
*/



int main()
{
  srand(time(NULL)); 
  
  /* name of the labyrinth */
  debug=1;// donne les infos les plus importantes
  int player; 
  int energy; /* data of the labyrinth */
  t_return_code ret = MOVE_OK;/* indicates the status of the previous move */
  t_move move;/* a move */
  t_labyrinthe* Laby=(t_labyrinthe*)malloc(sizeof(t_labyrinthe));  // crée de la place pour un labyrinthe par allocation dynamique
  
  /* connection to the server */
  connectToServer( "pc4001.polytech.upmc.fr", 1234,"Shab");
  
  
  /* wait for a game, and retrieve informations about it */
         
  player=mylabyrinthe(Laby); // mylabyrinthe renvoie 0 si c'est à nous de joueur, 1 sinon. Ceci permet de jouer alténativement
  
  if(player==0)
    energy=0; 


  else 
    energy=2; 




  do {
      
     
    printLabyrinth(); // permet d'afficher le labyrinthe


    if (player==1)// l'adversaire joue
      {
	ret = getMove( &move);
	player=0; // si l'adversaire a joué c'est à nous de jouer par la suite donc player=1 devient player=0
      }
        
    else
      {
	make_move(Laby,&move,energy); // ceci est la fonction que nous avons crée qui permet de faire bouger le joueur.
	printf("move %d %d %d %d \n",move.type,move.value, Laby->position_x,Laby->position_y); 
	ret=sendMove(move); // ceci permet d'envoyer le coup que l'on a fait en appelant ret
	
	player=1; // afin que chacun joue à son tour 
      }
      
        
  }while(ret==MOVE_OK);  // tant que la partie n'est pas finie on joue 


  if ((player ==1 && ret == MOVE_WIN) || (player==0 && ret == MOVE_LOSE))
    printf("Shab win the game\n");
  
  
  free(Laby); // Il ne faut pas oublier de libérer la mémoire
  
  
  closeConnection(); // Fin de la connection


  return EXIT_SUCCESS; // Permet de renvoyer une erreur si le jeu ne s'est pas fini correctement
}


//////////////////////////////////////////////////////////////// END OF THE PROGRAM ///////////////////////////////////////////////////////////////////
