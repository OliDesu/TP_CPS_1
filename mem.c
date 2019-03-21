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

#define ALIGN_V(add,align) (((intptr_t)(add))+((align)-1) &~ (align-1))


struct fb {
	size_t size;
	struct fb* next;
	/* ... */
}fb;

fb* head;

void mem_init(void* mem, size_t taille)
{
	mem = get_memory_adr();
	taille = get_memory_size();
	/*assert(mem == get_memory_adr());
	assert(taille == get_memory_size());*/
	/* ... */
	head = mem;
	head->size = taille;
	head->next = NULL;
	mem_fit(&mem_fit_first);
}

void mem_show(void (*print)(void *, size_t, int)) {
	void *zc = get_memory_adr();
	void *fin = zc + get_memory_size();
	struct fb* zl = head;

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
	/* ... */
	__attribute__((unused)) /* juste pour que gcc compile ce squelette avec -Werror */

	struct fb* zo = malloc(sizeof(fb));

	size_t taille_totale = (ALIGN_V(taille,ALIGNEMENT) + ALIGN_V(sizeof(size_t),ALIGNEMENT)):
	zo->size = taille_totale;

	struct fb* fb=mem_fit_fn(head, taille);
	struct fb* zl = malloc(sizeof(fb));

	if(fb == NULL){
		return NULL;
	}
	else{
		
	}
	/* ... */
	return NULL;
}


void mem_free(void* mem) {
}


struct fb* mem_fit_first(struct fb *list, size_t size) {
	while((list->next != NULL) && (list->size < sizeof(struct fb) + size)){
		list = list->next;
	}
	return list;
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
	return NULL;
}

struct fb* mem_fit_worst(struct fb *list, size_t size) {
	return NULL;
}
