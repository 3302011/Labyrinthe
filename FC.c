#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


char* alloc_2D_array(int sizex,int sizey)
{
  char*lab=(char*)calloc(sizex*sizey,sizeof(char));
    
  return lab;
}

char* create_table(int line,int column)
{
  char* tab=alloc_2D_array(column,line);
  int i,j,k;
  time_t t;
  /* Intializes random number generator */
  srand((unsigned) time(&t));
  for(k=0;k<100;k++)
    {
      i=rand()%line;
      j=rand()%column;

      //if(i!=line/2 && j!=column/2)
      tab[i*column+j]=1;

      
    }
  tab[line/2*column+column/2]=0;
  tab[line/2*column+0]='@';
  return tab;
}



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

//////////////////////////// STRUCTURE POUR L'OPENLIST ET CLOSELIST ////////////////////////////

typedef struct cases
{
  int x,y;
  int xp,yp; //coordonnées des cases précédentes
  int cost;
  int heuristique;
  struct cases* next; // va permettre de passer au suivant
}Case;

////////////////////////////// STRUCTURE POUR NE PAS PERDRE LES INFORMATIONS DES CASES PRECEDENTES ////////////////////////////////




///////////////////  ETAPE UNE: CREATION D'UNE FILE VIDE /////////////////////////

Case *List_empty()
{

  return NULL;

}

Case* annule()// fonction qui va donner des valeurs fictives à l'openlist sinon pb de NULL
{
  Case* c= (Case*)malloc(sizeof(Case)); // pr avoir de la mémoire -> place mettre infos
  c->x=-8;
  c->y=-10;
  c->cost=-100;
  c->heuristique=-4;
  c->next = NULL;
  return c;

}

////////////////////  ETAPE DEUX: AJOUTER UN ELEMENT A UNE FILE VIDE ////////


/////////////////////////  FONCTION POUR CALCULER LE CHEMIN VIA DISTANCE DE MANHATTAN ////////

int manhattan(int x1, int x2, int y1, int y2) // calcule la distance de Manhattan
{
  return abs(x1-x2) + abs(y1-y2);
}


void nouvelle_case (Case* c, int x, int y, int xp, int yp, int cost, int heuristique)
{
  c->x=x;
  c->y=y;
  c->xp=xp;
  c->yp=yp;
  c->cost=cost;
  c->heuristique=heuristique;
  c->next = NULL;


}

////////////////////////// TROISIEME ETAPE: RECUPERATION DU PLUS PETIT  /////////////////////////////////
/*Prémices: Cette fonction va nous permettre de comparer deux noeuds voisins. Un critère de séléction: l'heuristique. Plus l'heuristique est faible et plus le noeuds est "bon". Ce noeud sera ajouté à la closelist par la suite.

 */

Case* min (Case* Liste)
{
  if(Liste!=NULL)
    {
      Case* min=annule(); // on copie pour ne pas perdre de données
      Case* tmp=Liste->next; // on commence à l'élément suivant car il ne sert à rien de comparer le premier avec lui même
      min->x=Liste->x;
      min->y=Liste->y;
      min->xp=Liste->xp;
      min->yp=Liste->yp;
      min->cost=Liste->cost;
      min->heuristique=Liste->heuristique;
      min->next = NULL;


      while(tmp!=NULL)
	{
	  if(tmp->heuristique < min->heuristique) //heuristique ou cost à voir
	    {
	      min->x=tmp->x;
	      min->y=tmp->y;
	      min->xp=tmp->xp;
	      min->yp=tmp->yp;
	      min->cost=tmp->cost;
	      min->heuristique=tmp->heuristique;
	      min->next = NULL;// le minimum devient l'élément en entrée
	    }

	  tmp=tmp->next; // on passe à l'élément suivant
	}
      //min->next=NULL; 
      return min; // retourne le plus petit élément
    }
  else
    return NULL;

}

////////////////// QUATRIEME ETAPE : AJOUT ET EXTRACTION D'ELEMENT (EX DE L'OPENLIST POUR LES AJOUTER DANS LA CLOSELIST) ///////
int list_find(Case* L,int x,int y,int xp, int yp)//cette fonction envoie 1 si le point (x,y) est déjà dans la liste L 0 sinon
{
  Case* Liste=L;
  while(Liste!=NULL)
    {
      if(Liste->x==x&&Liste->y==y && Liste->xp==xp && Liste->yp==yp)
	return 1;
      Liste=Liste->next;
    }
  return 0;
}


int list_find2(Case* L,int x,int y)//cette fonction envoie 1 si le point (x,y) est déjà dans la liste L 0 sinon
{
  Case* Liste=L;
  while(Liste!=NULL)
    {
      if(Liste->x==x&&Liste->y==y)
	return 1;
      Liste=Liste->next;
    }
  return 0;
}

int list_find_next(Case* L,int xp,int yp)//cette fonction envoie 1 si le point (x,y) est déjà dans la liste L 0 sinon
{
  Case* Liste=L;
  while(Liste!=NULL)
    {
      if(Liste->xp==xp&&Liste->yp==yp)
	return 1;
      Liste=Liste->next;
    }
  return 0;
}

Case* list_add(Case* L, Case* nouveau,int* elem)
{

  if(list_find(L,nouveau->x,nouveau->y, nouveau->xp, nouveau->yp)==0)
    {
      Case* Liste=(Case*)malloc(sizeof(Case));
      Liste=nouveau;

      if(L==NULL)
        {
          L=nouveau;
          return L;
        }else
	{
	  Liste->next=L;
	  L=Liste;
	}
      //printf("\nElem:%d\n",(*elem));
      (*elem)++;
    }
  return L;

}


Case* extraire (Case* element,int* x, int* y, int* cost, int* heuristique)
{


  if(element==NULL)
    return NULL;

  if(element->x==*x && element->y==*y)
    {
      Case* tmp;
      element=element->next;
      tmp=element;
      tmp->next=NULL; //si next nul on sait que c'est une case au lieu d'une liste
      return tmp;

    }

  element->next=extraire(element,x,y,cost,heuristique);
  return element;



}


void afficher(Case* element,char* tab) // permet d'afficher
{
  Case* copie=element;

  while(copie!=NULL)
    {
      printf("Case:%d x:%d y:%d xp:%d yp:%d cost:%d heuristique:%d\n",tab[copie->y*15+copie->x],copie->x,copie->y,copie->xp, copie->yp,copie->cost,copie->heuristique);
      copie=copie->next;

    }

}

Case* delete (Case* element,Case* element_supr)
{

  if(element==NULL)
    return NULL;

  if(element->x==element_supr->x&& element->y==element_supr->y)
    {
      Case* tmp;
      tmp=element->next;
      //free(element);
      //printf("\nDete\n");
      return tmp;

    }

  element->next=delete(element->next,element_supr);
  return element;


}



////////////// UTILISATION DE LA STRUCTURE POUR GARGER EN MEMOIRE LES INFORMATIONS SUR LE LABYRINTHE //////
/*
 Prémices: La première étape est de "former" le labyrinthe. La labyrinthe en lui même a déjà été crée. 3 fonctions fournies permettent de récupérer les données du labyrinthe. WaitForLabyrinth permet de récupérer le nom et la taille (ainsi que d'attenre une partie), getLabyrinth permet de récupérer les données initiales c'est-à-dire de remplir la structure avec des 0 si les cases sont vides et 1 si elles correspondent à des murs. Enfin la fonction printLabyrinth permet d'afficher le labyrinthe.
 Notre but ici va être de créer de la place en mémoire grâce à l'allocation dynamique pour un labyrinthe que l'on a appelé labData ainsi qu'utiliser la structure définie au préalable pour définir les positions initiales des joueurs et du trésor dans le labyrinthe. En réalité ceci était déjà fait mais utiliser cette structure nous donne la possibilité d'avoir les modifications à jours.

*/

int mylabyrinthe(t_labyrinthe* current)
{

  int sizeX=15; 
  int sizeY=20; 

    
  char* labData;
  labData = (char*) malloc( sizeX * sizeY );
  int player=0;
  labData=create_table(sizeY,sizeX);
  current->lab=labData;
  current->sizeX=sizeX;
  current->sizeY=sizeY;
  if(player==0)
    current->position_x=0;
  else
    current->position_x=sizeX-1; 

  current->position_y=sizeY/2;

  current->position_tresor_x=sizeX/2;
  current->position_tresor_y=sizeY/2;
  current->position_adversaire_x=sizeX-1;
  current->position_adversaire_y=sizeY/2;
  return player;
}

Case* get_last(Case* Liste)
{
  if(Liste==NULL)
    return NULL; 

  Case* last=Liste; // on copie  
  Case* next=Liste->next; // on recupère l'élément suivant 

  while(next!=NULL)
    {
      last=next; 
      next=next->next; 


    }
  return last; 

}


Case* build_chemin(Case* closelist, int sizeX, int sizeY)
{/*
Case* tmp=closelist;
int i=0,j=0;
char tab[10000][10000]; //on stock le chemin dans un tableau
int size=0; 
Case* last=get_last(closelist); 
while(last!=NULL)
{
tmp=delete(tmp,last);
last=get_last(closelist);
//if(last==NULL)printf("\nNULL\n"); 
//else printf("\nNot NULL\n");
printf("\nBuild chemin\n");
//afficher(last,tab); 
tab[i][j]=last->xp;
j++;
tab[i][j]=last->yp;
i++;
j=0;
size++;
}

for(i=0;i<size;i++)
printf("Xp:%d Yp:%d\n",tab[i][i],tab[i][i+1]);
 */
  return NULL; 


}


////////////////////////// FONCTION QUI PERMET DE PARCOURIR LE LABYRINTHE AFIN DE CREER L'OPENLIST ET CLOSELIST /////////
/*
 Cette fonction va avoir pour but de remplir les cases du labyrinthe avec +1 à chaque fois qu'on a une case voisine qui est vide
 Cela veut dire que le cost va valoir +1 à chaque fois qu'on avance et qu'il ne s'agit pas d'un mur. Ceci nous permettra plus tard de choisir le chemin avec le cost le plus faible.
*/
/*principe: On ajoute les éléments dans l'openlist si les cases sont vides. Concernant un groupement de cases voisines, si le cost d'une case est le plus faible alors nous ajouterons cette case dans la closelist



 */
//première étape: donner un cost aux cases et les explorer





void print_laby(char* tab,int sizeX,int sizeY)
{
  int i,j;
  for(i=0;i<sizeY;i++)
    {
      printf("\n");
      for(j=0;j<sizeX;j++)
	{
	  printf("%d ",tab[i*sizeX+j]);
	}
    }
}

Case*  meilleur_chemin(t_labyrinthe* current)
{
  Case* openlist= annule();  // on crée l'openlist vide qui va contenir les cases vides
  Case* closelist=List_empty(); // on crée la closelist qui va contenir le chemin menant au trésor
  int us_x=current->position_x;
  int us_y=current->position_y;
  int tresor_x=current->position_tresor_x;
  int tresor_y=current->position_tresor_y;
  int cost=0;
  int voisin=0,elem_closedlist=0;
  int heuristique;
  Case* copie=annule();
  Case* small;
 
  heuristique=manhattan(us_x,tresor_x,us_y,tresor_y); // Retourne la distance entre les deux qui correpond à l'heuristique
  nouvelle_case(openlist,us_x,us_y,-1,-1,0,heuristique); // on rajoute dans l'openlist les coordonnées du point de départ
  // list_add(openlist,copie);
  // création de l'openlist
    
  while(openlist!=NULL) // tant que l'openlist n'est pas vide si la case est vide et qu'on ne l'a pas encore exploré on l'ajoute à l'openlist
    {

      //1ère étape 
      small=min(openlist);

      closelist=list_add(closelist,small,&elem_closedlist); // va ajouter à closelist le minimum de openlist
      // quand on ajoute un element à la closelist on la supprime de l'openlist
      //afficher(openlist); 
      /*printf("Openlist before del\n");
afficher(openlist);
      */
      openlist=delete(openlist,small);
      
      /*count=count+1;
	printf("ok count=%d\n",count);
	if(count>15)
	  break;
      */
      //look at all reachable neighbours and add them to the openlist
      // Case* element_extrait=extraire(openlist,&us_x,&us_y,&cost,&heuristique); // on extrait le premier élément de la file puis les suivants


      //2nd étape 
      
      if(small->x==tresor_x && small->y==tresor_y)
	{
	  Case* chemin=build_chemin(closelist,current->sizeX,current->sizeY); 
	  printf("Closelit:\n");
	  afficher(closelist,current->lab);
	  if(closelist==NULL)
	    printf("\nCloselist est vide");
	  printf("Chemin:\n");
	  return chemin; 

	}



      //3eme étape 
      us_x=small->x;
      us_y=small->y;

      if(list_find_next(closelist,us_x,us_y)==0 &&list_find2(closelist,us_x,(us_y-1+current->sizeX)%current->sizeX)==0 && current->lab[((us_y-1+current->sizeX)%current->sizeX)*current->sizeX + us_x]==0) // si la case du haut est vide
        {
	  int h = manhattan(us_x,tresor_x, (us_y-1+current->sizeX)%current->sizeX, tresor_y);
	  nouvelle_case(copie,us_x,(us_y-1+current->sizeX)%current->sizeX,us_x,us_y,++cost,h); // on rajoute dans l'openlist les coordonnées du point de départ

	  openlist=list_add(openlist,copie,&voisin);
	  cost--; 

        }
     
      copie=annule();

      if(list_find_next(closelist,us_x,us_y)==0 && list_find2(closelist,us_x,(us_y+1)%(current->sizeX) *current->sizeX )==0 && current->lab[(us_y+1)%(current->sizeX) *current->sizeX + us_x]==0) // si la case du bas
        {

	  int h = manhattan(us_x, tresor_x,(us_y+1)%(current->sizeX),  tresor_y);
	  nouvelle_case(copie,us_x,(us_y+1)%(current->sizeX),us_x,us_y,++cost,h); // on rajoute dans l'openlist les coordonnées du point de départ
          openlist=list_add(openlist,copie,&voisin);
	  cost--; 

        }
      //printf("Openlist add 2\n");
      //afficher(openlist);
      copie=annule();
      if(list_find_next(closelist,us_x,us_y)==0 &&list_find2(closelist,(us_x-1+current->sizeY)%current->sizeY,us_y)==0 && current->lab[(us_y)*current->sizeX + (us_x-1+current->sizeY)%current->sizeY]==0) // si la case de gauche
        {

	  int h = manhattan((us_x-1+current->sizeY)%current->sizeY,tresor_x, us_y,tresor_y);
	  nouvelle_case(copie,(us_x-1+current->sizeY)%current->sizeY,us_y,us_x,us_y,++cost,h); // on rajoute dans l'openlist les coordonnées du point de départ

	  openlist=list_add(openlist,copie,&voisin);
	  cost--;
             
        }
      //printf("Openlist add 3\n");
      //afficher(openlist);
      copie=annule();

      if(list_find_next(closelist,us_x,us_y)==0 &&list_find2(closelist,(us_x+1)%(current->sizeY),us_y)==0 && current->lab[(us_y) *current->sizeX + (us_x+1)%(current->sizeY)]==0) // droite
        {

	  int h = manhattan((us_x+1)%(current->sizeY),tresor_x, us_y,tresor_y);
	  nouvelle_case(copie,(us_x+1)%(current->sizeY),us_y,us_x,us_y,++cost,h); // on rajoute dans l'openlist les coordonnées du point de départ

	  openlist=list_add(openlist,copie,&voisin);
	  cost--;

        }
      //printf("\nCost before:%d\n",cost);
      if(voisin!=0) cost++;
      //printf("\nCost after:%d\n",cost);
      voisin=0;
      //printf("Openlist add 4\n");
      //afficher(openlist);
      copie=annule();
      //choose the best step to do by looking for the minimum of the F=cost + heuristics in the openlist (which does not include our own position)
        

    }
   

  return closelist;
}




int main()
{
  int sizeX=15; 
  int sizeY=20; 

  t_labyrinthe* Laby=(t_labyrinthe*)malloc(sizeof(t_labyrinthe));
  mylabyrinthe(Laby);

  print_laby(Laby->lab, sizeX, sizeY);
  printf("\n"); 

  meilleur_chemin(Laby);

  return 1; 


}







