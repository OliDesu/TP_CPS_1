#include "mem.h"
#include "common.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

// constante définie dans gcc seulement
#ifdef __BIGGEST_ALIGNMENT__
#define ALIGNMENT __BIGGEST_ALIGNMENT__
#else
#define ALIGNMENT 16
#endif

#define ALIGN_V(add,align) ((((__intptr_t)(add))+(align)-1) &~ (align-1))


struct fb {
	size_t size;
	struct fb* next;
	/* ... */
}fb;

struct fb* head_zl;
struct fb* head_zo;

void add (struct fb **liste, struct fb *z){
	if (*liste == NULL){
		z->next = NULL;
		*liste = z;
	}
	else{
		if (*liste > z){
			z->next = *liste;
			*liste = z;
		}
		else{
			struct fb *zone_courante = *liste;
			while (zone_courante->next != NULL && zone_courante->next < z){
				zone_courante = zone_courante->next;
			}
			z->next = zone_courante->next;
			zone_courante->next = z;
		}
	}
}

void supp (struct fb **liste, struct fb *z){
	if (*liste != NULL){
		struct fb *zone_courante = *liste;
		if (zone_courante == z){
			*liste = zone_courante->next;
		}
		else{
			while (zone_courante->next != z && zone_courante->next != NULL){
				zone_courante = zone_courante->next;
			}
			zone_courante->next = z->next;
		}
	}
}

struct fb* recherche (struct fb **liste, struct fb *z){
	if(*liste == NULL){
		return NULL;
	}
	struct fb *zone_courante = *liste;
	while (  zone_courante != z&&zone_courante != NULL){
		zone_courante = zone_courante->next;
	}
	return zone_courante;
}

void mem_init(void* mem, size_t taille)
{
	mem = get_memory_adr();
	taille = get_memory_size();
	head_zl = mem;
	head_zl->size = taille;
	head_zl->next = NULL;
	mem_fit(&mem_fit_first);
}

void mem_show(void (*print)(void *, size_t, int)) {
	void *zc = get_memory_adr();
	void *fin = zc + get_memory_size();
	struct fb* zl = head_zl;

	while (zc < fin) {
		size_t taille = ((struct fb*)zc)->size;
		int i = 0;
		if (zc == zl){
			i = 1;
			zl = zl->next;
		}
		print(zc, taille, i);
		zc = zc + taille;
	}
}

static mem_fit_function_t *mem_fit_fn;
void mem_fit(mem_fit_function_t *f) {
	mem_fit_fn=f;
}

void *mem_alloc(size_t taille) {
	__attribute__((unused)) 
	size_t taille_zo = (ALIGN_V(sizeof(struct fb),ALIGNMENT) + ALIGN_V(taille,ALIGNMENT));
	struct fb * zl = mem_fit_fn(head_zl, taille_zo);
    if(zl == NULL){
			return NULL;
		}
    size_t taille_zl = zl->size;
    if(taille_zl - taille_zo >= sizeof(struct fb)){
        struct fb* new_zl = (struct fb*)((char*)zl + taille_zo);
        new_zl->size = taille_zl - taille_zo;
        add(&head_zl, new_zl);
        supp(&head_zl, zl);
        zl->size = taille_zo;
        add(&head_zo,zl);
    }else{
        supp(&head_zl,zl);
        zl->size = taille_zo;
        add(&head_zo,zl);
    }
    return zl;
}


void mem_free(void* mem) {
    struct fb* zo = recherche(&head_zo,(struct fb*)mem);
    if(zo != NULL){
        add(&head_zo,zo);
        supp(&head_zl,zo);

        struct fb* zone_courante = head_zl;
        if (zone_courante != NULL){
            while (zone_courante->next != NULL){
                if( (char *)zone_courante->next == (zone_courante->size + (char *)zone_courante)){
                    zone_courante->size = zone_courante->size + zone_courante->next->size;
                    zone_courante->next = zone_courante->next->next;
                }else{
                    zone_courante = zone_courante->next;
                }
            }
        }
    }
}


struct fb* mem_fit_first(struct fb *list, size_t size) {
	struct fb* zone_courante = list;
	while((zone_courante->size < sizeof(struct fb) + size) && (zone_courante->next != NULL) ){
		zone_courante = zone_courante->next;
	}
	return zone_courante;
}

/* Fonction à faire dans un second temps
 * - utilisée par realloc() dans malloc_stub.c
 * - nécessaire pour remplacer l'allocateur de la libc
 * - donc nécessaire pour 'make test_ls'
 * Lire malloc_stub.c pour comprendre son utilisation
 * (ou en discuter avec l'enseignant)
 */
size_t mem_get_size(void *zone) {
	/* zone est une adresse qui a été retournée par mem_alloc() */

	/* la valeur retournée doit être la taille maximale que
	 * l'utilisateur peut utiliser dans cette zone */
	return 0;
}

/* Fonctions facultatives
 * autres stratégies d'allocation
 */
struct fb* mem_fit_best(struct fb *list, size_t size) {
	struct fb* z;
	struct fb* zone_courante = list;
	
	while(zone_courante->size < size && zone_courante != NULL  ){
		zone_courante = zone_courante->next;
	}
	z = zone_courante;
	while(zone_courante != NULL){
		if(zone_courante->size > size && zone_courante->size < z->size){
			z = zone_courante;
		}
		z = z->next;
	}
	return z;
}

struct fb* mem_fit_worst(struct fb *list, size_t size) {
	struct fb* z;
	struct fb* zone_courante = list;
	

	while( zone_courante->size < size && zone_courante != NULL){
		zone_courante = zone_courante->next;
	}
	z = zone_courante;
	while(zone_courante != NULL){
		if(zone_courante->size > size && zone_courante->size > z->size){
			z = zone_courante;
		}
		z = z->next;
	}
	return z;
}
