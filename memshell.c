#include "mem.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>

#define TAILLE_BUFFER 128

void aide()
{
  fprintf(stderr,"Aide :\n");
  fprintf(stderr,"Saisir l'une des commandes suivantes\n");
  fprintf(stderr,"\n");
  fprintf(stderr,"a taille  :   allouer un bloc de la taille souhaitee\n");
  fprintf(stderr,"l adresse :   librer un bloc alloue precedemment\n");
  fprintf(stderr,"i         :   afficher la liste des emplacements memoire "
                               "inocupes\n");
  fprintf(stderr,"o         :   afficher la liste des emplacements memoire "
                               "occupes\n");
  fprintf(stderr,"M         :   afficher la liste de tous les emplacements "
                               "memoire (libres et occupes)\n");
  fprintf(stderr,"m         :   afficher le dump de la memoire\n");
  fprintf(stderr,"h         :   afficher cette aide\n");
  fprintf(stderr,"q         :   quitter ce programme\n");
  fprintf(stderr,"t         :   tests\n");
  fprintf(stderr,"\n");
}

void afficher_zone(void *adresse, size_t taille, int free)
{
  printf("Zone %s, Adresse : %lu, Taille : %lu\n", free?"libre":"occupee",
         adresse - get_memory_adr(), (unsigned long) taille);
}

void afficher_zone_libre(void *adresse, size_t taille, int free)
{
  if (free)
	  afficher_zone(adresse, taille, 1);
}

void afficher_zone_occupee(void *adresse, size_t taille, int free)
{
  if (!free)
	  afficher_zone(adresse, taille, 0);
}

int main()
{
  char buffer[TAILLE_BUFFER];
  char commande;
  char *adresse;
  void *ptr;
  int offset;
  int taille, i;

  aide();
  mem_init(get_memory_adr(),get_memory_size());

  while (1) {
      printf("? ");
      fflush(stdout);
      commande = getchar();
      switch (commande) {
        case 'a':
          scanf ("%d",&taille);
          ptr = mem_alloc(taille);
	  if (ptr == NULL)
              printf("Echec de l'allocation\n");
	  else
              printf("Memoire allouee en %d\n", (int) (ptr-get_memory_adr()));
          break;
        case 'l':
          scanf ("%d", &offset);
	  mem_free(get_memory_adr()+offset);
	  printf("Memoire liberee\n");
          break;
        case 'i':
	  mem_show(afficher_zone_libre);
	  break;
        case 'o':
	  mem_show(afficher_zone_occupee);
	  break;
        case 'M':
	  mem_show(afficher_zone);
	  break;
        case 'm':
          printf("[ ");
	        adresse = get_memory_adr();
          for (i=0; i<get_memory_size(); i++)
          	printf("%d ", adresse[i]);
          printf("]\n");
	  break;
        case 'h':
          aide();
          break;
        case 'q':
          exit(0);
          break;
        case 't':
            printf("On commence tout d'abbord par verifier que la mémoire du début est bien vide ou non \n");
            mem_init(get_memory_adr(),get_memory_size());
            //Puis on l'affiche
            printf("Et elle vaut  : \n");
            mem_show(afficher_zone);
      
            //On va commencer par faire une allocation simple 
            printf("ALLOCATION SIMPLE \n");
            ptr = mem_alloc(1234);
            if (ptr == NULL)
              printf("Echec de l'allocation\n");
           else
              printf("La mémoire a bien été allouée en %d\n\n", (int) (ptr-get_memory_adr()));
           printf("On affiche la mémoire : \n");
           mem_show(afficher_zone);
            //On va commencer par voir comment notre programme réagit si l'on fait une allocation alors que la mémoire est saturée
            printf("Comportement du programme si l'on alloue une zone comprenant une zone deja allouée : \n");
           ptr = mem_alloc(127984);
           if (ptr == NULL)
              printf("L'allocation n'a pas pu se faire car la mémoire est pleine \n");
           else
              printf("La mémoire a bien été allouée en %d \n", (int) (ptr-get_memory_adr()));
           mem_show(afficher_zone);
           mem_free(get_memory_adr());

           //Nous allons maintenant essayer d'allouer une zone de mémoire qui dépasse la zone totale
            printf("Allocation d'une zone mémoire qui dépasse la zone totale :\n ");
            ptr = mem_alloc(128000);
            if (ptr == NULL)
              printf("L'allocation n'a pas pu se faire \n");
            else
              printf("La mémoire a bien été allouée en %d \n", (int) (ptr-get_memory_adr()));
          
            mem_show(afficher_zone);
              //On va maintenant essayer de voir ce qui se produit une fois que l'on fait une allocation dans une mémoire qui est plein au préalable
            printf("Allocation sur une mémoire deja allouée \n");
            ptr = mem_alloc(234);
            if (ptr == NULL)
              printf("L'allocation n'a pas pu se faire\n");
            else
              printf("La mémoire a bien été allouée en %d\n", (int) (ptr-get_memory_adr()));
        
            //On va maintenant liberer l'entièreté de la mémoire
            printf("Libération de l'entièreté de la mémoire \n");
            mem_free(get_memory_adr());
            //On va maintenant afficher la mémoire : 
            mem_show(afficher_zone);
           default:
                fprintf(stderr,"Commande inconnue !\n");
            }
            /* vide ce qu'il reste de la ligne dans le buffer d'entree */
            fgets(buffer,TAILLE_BUFFER,stdin);
        }
  return 0;
}
