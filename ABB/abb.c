#include "abb.h"
//#include "lista.h"
#include <stdlib.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/
typedef struct nodo{
	struct nodo_t* izq;
	struct nodo_t* der;
	void* dato;
	const char* clave;
}nodo_t;

struct abb{
	nodo_t* raiz;
	abb_comparar_clave_t comparar;
	abb_destruir_dato_t destruir;
	size_t cantidad;
};

int (*abb_comparar_clave_t) (const char *, const char *);
void (*abb_destruir_dato_t) (void *);

struct abb_iter abb_iter_t;

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
nodo_t* crearNodo(const char* clave, void* dato){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (nodo == NULL){
		return NULL;
	}
	nodo->izq = NULL;
	nodo->der = NULL;
	nodo->dato = dato;
	nodo->clave = clave
	return nodo;
}


bool guardar(nodo_t* raiz, abb_t* arbol, const char *clave, void *dato){
	if(raiz == NULL){
		raiz = crearNodo(clave, dato);
		if (raiz == NULL){
			return false;
		}
		arbol->cantidad ++;
		return true;
	}
	if(arbol->comparar(raiz->clave, clave) == 0){
		arbol->destruir(raiz->dato);
		raiz->dato == dato;
		arbol->cantidad ++;
		return true;
	}
	if(arbol->comparar(clave, raiz->der->clave) < 0){
		// revisar comparaaciones, no estoy seguro si lo arme bien o al reves
			return abb_guardar(raiz->izq, clave, dato);
	}
	return abb_guardar(raiz->der, arbol, clave, dato);
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
	return guardar(arbol->raiz, arbol, clave, dato);
	
}

void* borrar_menos_1_hijo(nodo_t* raiz){
		nodo_t* n_auxiliar = raiz;
		void* d_auxiliar = raiz->dato;
		if(raiz->izq == NULL){
			raiz = raiz->der;
		}
		else{
			raiz = raiz->izq;
		}
		free(n_auxiliar);
		return d_auxiliar;
}

void* borrar_con_2_hijos(nodo_t* raiz){
	nodo_t* reemplazante = raiz->der
	while(reemplazante != NULL){
		reemplazante = reemplazante->izq;
	}
	const char* r_clave = reemplazante->clave;
	void* r_dato = borrar_menos_1_hijo(reemplazante);
	raiz->clave = r_clave;
	void* auxiliar = raiz->dato;
	raiz->dato = r_dato;
	return auxiliar;
}

void* borrar(nodo_t* raiz, abb_t* arbol, const char *clave){
	if(raiz == NULL){
		return NULL;
	}
	if(arbol->comparar(raiz->clave, clave) == 0){
		if(raiz->izq != NULL && raiz->der != NULL){
			return borrar_con_2_hijos(raiz);
		}
		return borrar_menos_1_hijo(raiz);
	}
	if(arbol->comparar(clave, raiz->der->clave) < 0){
		// revisar comparaaciones, no estoy seguro si lo arme bien o al reves
		return abb_borrar(raiz->izq, clave, dato);
	}
	return abb_borrar(raiz->der, arbol, clave, dato);
}

void *abb_borrar(abb_t *arbol, const char *clave){
	return borrar(arbol->raiz, arbol, clave);
}

