#define _POSIX_C_SOURCE 200809L // strdup()
#include <stdlib.h>
#include <string.h>
#include "abb.h"
#include "pila.h"

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/
typedef struct nodo{
	struct nodo *izq, *der;
	void* dato;
	char* clave;
} nodo_t;

struct abb{
	nodo_t* raiz;
	abb_comparar_clave_t comparar;
	abb_destruir_dato_t destruir;
	size_t cantidad;
};

struct abb_iter{
	pila_t* pila_abb;
	const abb_t* arbol;
};

typedef void (*destruir_n)(nodo_t*, abb_destruir_dato_t);
typedef bool (*visitar_t)(const char *, void *, void *);

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

void destruir_nodo(nodo_t* nodo_abb, abb_destruir_dato_t destruir){
	if (destruir != NULL){
		destruir(nodo_abb->dato);
	}
	free(nodo_abb->clave);
	free(nodo_abb);
}

void destruir_nodos_post(nodo_t* raiz,abb_destruir_dato_t destruir){
	if (raiz == NULL){
		return;
	}
	destruir_nodos_post(raiz->izq,destruir);
	destruir_nodos_post(raiz->der,destruir);
	destruir_nodo(raiz,destruir);
}

nodo_t** buscar(nodo_t** dir_raiz,const char *clave,abb_comparar_clave_t comparar){
	nodo_t* raiz = *dir_raiz;
	if (raiz == NULL || comparar(clave,raiz->clave) == 0){
		return dir_raiz;
	}
	else if (comparar(clave,raiz->clave)<0){
		return buscar(&(raiz->izq),clave,comparar);
	} else {
		return buscar(&(raiz->der),clave,comparar);
	}

}

void borrar(nodo_t** dir_nodo_busqueda, destruir_n fdestruir_nodo){
	nodo_t* nodo_busqueda = * dir_nodo_busqueda;
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
			borrar(&(nodo_busqueda->der),NULL);
		} else { //Busco al menor yendo todo izquierda
			while (reemplazante->izq->izq != NULL){
				reemplazante = reemplazante->izq;
			}	
			nodo_t* aux = reemplazante->izq;
			borrar(&(reemplazante->izq),NULL);
			reemplazante = aux;
		}
		//Al reemplazante, le asigno los hijos del que quiero borrar
		reemplazante->izq = nodo_busqueda->izq;
		reemplazante->der = nodo_busqueda->der;
	}
	if (fdestruir_nodo != NULL){
		fdestruir_nodo(nodo_busqueda,NULL);
	}
	//swap entre reemplazante y el que quiero borrar
	*dir_nodo_busqueda = reemplazante;
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
	nodo_t* nodo_busqueda = *dir_nodo_busqueda;
	if (nodo_busqueda == NULL){ //El se encontro el valor con esa clave
		nodo_busqueda = crearNodo(clave,dato);
		if (nodo_busqueda == NULL){
			return false;
		}
		*dir_nodo_busqueda = nodo_busqueda;
		arbol->cantidad++;
	} else { //Se encontro y se reemplaza
		if (arbol->destruir != NULL){
			arbol->destruir(nodo_busqueda->dato);
		}
		nodo_busqueda->dato = dato;
	}
	return true;
}

void *abb_borrar(abb_t *arbol, const char *clave){
	nodo_t** dir_nodo_busqueda = buscar(&(arbol->raiz),clave,arbol->comparar);
	nodo_t* nodo_busqueda = * dir_nodo_busqueda;
	if (nodo_busqueda == NULL){
		return NULL;
	}
	void* auxiliar = nodo_busqueda->dato;
	borrar(dir_nodo_busqueda,destruir_nodo); // llamo a borrar recursivamente
	arbol->cantidad--;
	return auxiliar;
}

void *abb_obtener(const abb_t *arbol, const char *clave){
	nodo_t* raiz = arbol->raiz;
	nodo_t** dir_nodo_busqueda = buscar(&(raiz),clave,arbol->comparar);
	nodo_t* nodo_busqueda = * dir_nodo_busqueda;
	if (nodo_busqueda == NULL){
		return NULL;
	}
	return nodo_busqueda->dato;
}

bool abb_pertenece(const abb_t *arbol, const char *clave){
	nodo_t* raiz = arbol->raiz;
	nodo_t** dir_nodo_busqueda = buscar(&(raiz),clave,arbol->comparar);
	nodo_t* nodo_busqueda = * dir_nodo_busqueda;
	return nodo_busqueda != NULL;
}

size_t abb_cantidad(abb_t *arbol){
	return arbol->cantidad;
}

void abb_destruir(abb_t *arbol){
	destruir_nodos_post(arbol->raiz,arbol->destruir);
	free(arbol);
}

/* ******************************************************************
 *          	      AUXILIARES DE ABB ITERADOR
 * *****************************************************************/

bool apilar_todo_izquierda(pila_t* pila_abb, nodo_t* raiz){
	while (raiz != NULL){
		if (pila_apilar(pila_abb,raiz) == false){
			return false;
		}
		raiz = raiz->izq;
	}
	return true;
}

bool iterar_in_order(nodo_t* raiz,visitar_t visitar, void* extra){
	if (raiz == NULL){
		return true;	
	} 
	if (iterar_in_order(raiz->izq,visitar,extra) == false){
		return false;
	}
	if (visitar(raiz->clave,raiz->dato,extra) == false){
		return false;
	}
	return iterar_in_order(raiz->der,visitar,extra);
}

/* ******************************************************************
 *                PRIMITIVAS DE ABB ITERADOR INTERNO
 * *****************************************************************/

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra){
	iterar_in_order(arbol->raiz,visitar,extra);
}

/* ******************************************************************
 *                PRIMITIVAS DE ABB ITERADOR EXTERNO
 * *****************************************************************/

abb_iter_t *abb_iter_in_crear(const abb_t *arbol){
	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	if (iter == NULL){
		return NULL;	
	} 
	pila_t* pila_abb = pila_crear();
	if (pila_abb == NULL){
		free(iter);
		return NULL;
	}
	if (apilar_todo_izquierda(pila_abb,arbol->raiz) == false){
		pila_destruir(pila_abb);
		free(iter);
		return NULL;
	}
	iter->arbol = arbol;
	iter->pila_abb = pila_abb;
	return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter){
	if (abb_iter_in_al_final(iter)){
		return false;
	}
	nodo_t* nodo_actual = pila_desapilar(iter->pila_abb);
	return apilar_todo_izquierda(iter->pila_abb,nodo_actual->der);
}
const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
	if (abb_iter_in_al_final(iter)){
		return NULL;
	}
	nodo_t* nodo_actual = pila_ver_tope(iter->pila_abb);
	return nodo_actual->clave;
}
bool abb_iter_in_al_final(const abb_iter_t *iter){
	return pila_esta_vacia(iter->pila_abb);
}

void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->pila_abb);
	free(iter);
}


