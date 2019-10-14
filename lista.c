#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#define LARGO_INICIAL 0

// Definición del struct nodo
struct nodo{
	void *valor_nodo;
	struct nodo *prox;
};
typedef struct nodo nodo_t;

// Definición del struct lista
 
struct lista{
	nodo_t *prim;
	nodo_t *ult;
	size_t largo;
};

// Implementación de primitivas de lista

lista_t *lista_crear(void){
	lista_t* lista = malloc(sizeof(lista_t));
	if (!lista) return NULL;
	lista->prim = NULL;
	lista->ult = NULL;
	lista->largo = LARGO_INICIAL;
	return lista;
}

bool lista_esta_vacia(const lista_t *lista){
	return lista_largo(lista) == LARGO_INICIAL;
}

bool lista_insertar_primero(lista_t *lista, void *dato){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (!nodo) return false;
	
	nodo->valor_nodo = dato;

	if (lista_esta_vacia(lista)){
		nodo->prox = NULL;
		lista->ult = nodo;
	} else {
		nodo->prox = lista->prim;
	}
	
	lista->prim = nodo;
	lista->largo++;

	return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (!nodo) return false;

	nodo->valor_nodo = dato;
	nodo->prox = NULL;
	if (lista_esta_vacia(lista)){
		lista->prim = nodo;
	} else {
		lista->ult->prox = nodo;
	}
	lista->ult = nodo;	
	lista->largo++;
	return true;
}

void *lista_borrar_primero(lista_t *lista){
	if (lista_esta_vacia(lista)) return NULL;
	nodo_t* nodo_prim = lista->prim;
	nodo_t* nodo_prox = nodo_prim->prox;
	if (!nodo_prox) lista->ult = NULL;
	void* aux = nodo_prim->valor_nodo;
	lista->prim = nodo_prox;
	free(nodo_prim);
	lista->largo--;
	return aux;
}

void *lista_ver_primero(const lista_t *lista){
	if (lista_esta_vacia(lista)) return NULL;
	return lista->prim->valor_nodo;
}

void *lista_ver_ultimo(const lista_t* lista){
	if (lista_esta_vacia(lista)) return NULL;
	return lista->ult->valor_nodo;
}

size_t lista_largo(const lista_t *lista){
	return lista->largo;
}

void lista_destruir(lista_t *lista, void destruir_dato(void *)){
	while (!lista_esta_vacia(lista)){
		void* dato_lista = lista_borrar_primero(lista);
		if (destruir_dato) destruir_dato(dato_lista);
	}
	free(lista);
}

// Definición del struct iterador de lista
struct lista_iter{
	lista_t* lista_original;
	nodo_t* anterior;
	nodo_t* actual;
};

// Implementación de primitivas de iterador de lista externo

lista_iter_t *lista_iter_crear(lista_t *lista){
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));
	if (!iter) return NULL;
	iter->lista_original = lista;
	iter->anterior = NULL;
	iter->actual = lista->prim;
	return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter){
	if (lista_iter_al_final(iter)) return false;
	iter->anterior = iter->actual;
	iter->actual = iter->actual->prox;
	return true;	
}

void *lista_iter_ver_actual(const lista_iter_t *iter){
	if (lista_iter_al_final(iter)) return NULL;
	return iter->actual->valor_nodo;
}

bool lista_iter_al_final(const lista_iter_t *iter){
	return iter->actual == NULL;
}

void *lista_iter_borrar(lista_iter_t *iter){
	if (lista_iter_al_final(iter)) return NULL;
	nodo_t* aux = iter->actual;
	void* valor = aux->valor_nodo;
	if (!iter->anterior){		
		if (!iter->actual->prox) iter->lista_original->ult = NULL;
		iter->lista_original->prim = iter->actual->prox; 
	} else {
		if (!iter->actual->prox) iter->lista_original->ult = iter->anterior;
		iter->anterior->prox = iter->actual->prox;
	}
	iter->actual = iter->actual->prox;
	free(aux);
	iter->lista_original->largo--;
	return valor;
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato){
	nodo_t* nuevo_nodo = malloc(sizeof(nodo_t));
	if (!nuevo_nodo) return false;
	nuevo_nodo->valor_nodo = dato;
	if (!iter->anterior){
		if (lista_iter_al_final(iter)) iter->lista_original->ult = nuevo_nodo;
		nuevo_nodo->prox = iter->lista_original->prim;
		iter->lista_original->prim = nuevo_nodo;
	} else {
		if (lista_iter_al_final(iter)){
			iter->lista_original->ult = nuevo_nodo;
			nuevo_nodo->prox = NULL;
		} else nuevo_nodo->prox = iter->actual;
		iter->anterior->prox = nuevo_nodo;
	}
	iter->actual = nuevo_nodo;
	iter->lista_original->largo++;
	return true;
}

void lista_iter_destruir(lista_iter_t *iter){
	free(iter);
}

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){
	nodo_t* nodo_actual = lista->prim;
	while(nodo_actual){
		void* valor_actual = nodo_actual->valor_nodo;
		if (!visitar(valor_actual,extra)) return;
		nodo_actual = nodo_actual->prox;
	}
}
