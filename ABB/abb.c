#define _POSIX_C_SOURCE 200809L // strdup()
#include "abb.h"
#include <stdlib.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/
typedef struct nodo{
	struct nodo_t* izq;
	struct nodo_t* der;
	void* dato;
	const char* clave;
} nodo_t;

struct abb{
	nodo_t* raiz;
	abb_comparar_clave_t comparar;
	abb_destruir_dato_t destruir;
	size_t cantidad;
};

int (*abb_comparar_clave_t) (const char *, const char *);
void (*abb_destruir_dato_t) (void *);
struct abb_iter abb_iter_t;
typedef void (*destruir_nodo) (nodo_t*, abb_destruir_dato_t);

/* ******************************************************************
 *          		      AUXILIARES
 * *****************************************************************/

nodo_t* crearNodo(const char* clave, void* dato){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (nodo == NULL){
		return NULL;
	}
	char* copia_clave = strdup(clave);
	if (copia_clave == NULL){
		free(nodo);
		return NULL;
	}
	nodo->izq = NULL;
	nodo->der = NULL;
	nodo->dato = dato;
	nodo->clave = copia_clave;
	return nodo;
}

void destruir_nodos_post(nodo_t* raiz,abb_destruir_dato_t destruir)
{
	if (raiz == NULL){
		return;
	}
	destruir_nodos_post(raiz->izq,destruir);
	destruir_nodos_post(raiz->der,destruir);
	destruir_nodo(raiz,destruir);
}

nodo_t** buscar(nodo_t** dir_raiz,const char *clave,abb_comparar_clave_t comparar){
	nodo_t* raiz = (nodo_t*) dir_raiz;
	if (raiz == NULL){
		return NULL;
	}
	if (comparar(clave,raiz->clave)<0){
		return buscar(&(raiz->izq),clave,comparar);
	}
	else if (comparar(clave,raiz->clave)>0){
		return abb_guardar(&(raiz->der),clave,comparar);
	} else {
		return dir_raiz;
	}

}

void destruir_nodo(nodo_t* nodo_abb, abb_destruir_dato_t destruir){
	if (destruir != NULL){
		destruir(nodo_abb->dato);
	}
	free(nodo_abb->clave);
	free(nodo_abb);
}

void* borrar_nodo(nodo_t** dir_nodo_busqueda, destruir_nodo destruir_nodo){
	nodo_t* nodo_busqueda = (nodo_t*) dir_nodo_busqueda;
	nodo_t* reemplazante;
	if(nodo_busqueda->izq != NULL && nodo_busqueda->der == NULL){ //Solo tiene hijo izq
		reemplazante = nodo_busqueda->izq;
	}
	else if (nodo_busqueda->izq == NULL && nodo_busqueda->der != NULL){ //Solo tiene hijo der
		reemplazante = nodo_busqueda->der;
	}
	else if (nodo_busqueda->izq == NULL && nodo_busqueda->der == NULL){ //No tiene hijos
		reemplazante = NULL;
	}
	else { //Tiene dos hijos, busco reemplazante
		reemplazante = nodo_busqueda->der;
		if (reemplazante->izq == NULL){ // el reemplazante no tiene hijo izquierdo, llamo recursivamente para borrar el derecho
			borrar_nodo(&(nodo_busqueda->der),NULL);
		} else { //Busco al menor yendo todo izquierda
			while (reemplazante->izq->izq != NULL){
				reemplazante = reemplazante->izq;
			}	
			nodo_t* aux = reemplazante->izq;
			borrar_nodo(&(reemplazante->izq),NULL);
			reemplazante = aux;
		}
		//Al reemplazante, le asigno los hijos del que quiero borrar
		reemplazante->izq = nodo_busqueda->izq;
		reemplazante->der = nodo_busqueda->der;
	}
	if (destruir_nodo != NULL){
		destruir_nodo(nodo_busqueda,NULL);
	}
	//swap entre reemplazante y el que quiero borrar
	(nodo_t*)dir_nodo_busqueda = reemplazante;
}


/* ******************************************************************
 *                PRIMITIVAS DE ABB
 * *****************************************************************/
 
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
	abb_t* abb = malloc(sizeof(abb_t));
	if (abb == NULL){
		return NULL;
	}
	abb->raiz = NULL;
	abb->comparar = cmp;
	abb->destruir = destruir_dato;
	abb->cantidad = 0;
	return abb;
}


bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
	nodo_t** dir_nodo_busqueda = buscar(&(arbol->raiz),clave,arbol->comparar);
	nodo_t* nodo_busqueda = (nodo_t*) dir_nodo_busqueda;
	if (nodo_busqueda == NULL){
		nodo_busqueda = crearNodo(clave,dato);
		if (nodo_busqueda == NULL){
			return false;
		}
		*dir_nodo_busqueda = nodo_busqueda;
	} else {
		if (arbol->destruir != NULL){
			arbol->destruir(nodo_busqueda->dato);
		}
		nodo_busqueda->dato = dato;
	}
	arbol->cantidad++;
	return true;
}

void *abb_borrar(abb_t *arbol, const char *clave){
	nodo_t** dir_nodo_busqueda = buscar(&(arbol->raiz),clave,arbol->comparar);
	nodo_t* nodo_busqueda = (nodo_t*) dir_nodo_busqueda;
	if (nodo_busqueda == NULL){
		return NULL;
	}
	void* auxiliar = nodo_busqueda->dato;
	borrar(dir_nodo_busqueda,destruir_nodo);
	arbol->cantidad--;
	return auxiliar;
}

void *abb_obtener(const abb_t *arbol, const char *clave){
	nodo_t** dir_nodo_busqueda = buscar(&(arbol->raiz),clave,arbol->comparar);
	nodo_t* nodo_busqueda = (nodo_t*) dir_nodo_busqueda;
	if (nodo_busqueda == NULL){
		return NULL;
	}
	return nodo_busqueda->dato;
}

bool abb_pertenece(const abb_t *arbol, const char *clave){
	nodo_t** dir_nodo_busqueda = buscar(&(arbol->raiz),clave,arbol->comparar);
	nodo_t* nodo_busqueda = (nodo_t*) dir_nodo_busqueda;
	return nodo_busqueda != NULL;
}

size_t abb_cantidad(abb_t *arbol){
	return arbol->cantidad;
}

void abb_destruir(abb_t *arbol){
	destruir_nodos_post(arbol->raiz, arbol->destruir);
	free(arbol);
}