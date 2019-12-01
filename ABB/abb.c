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
	const abb_t* arbol;
	pila_t* pila;
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

void borrar(nodo_t** dir_nodo_busqueda, destruir_n destruir_nodo){
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
	if (destruir_nodo != NULL){
		destruir_nodo(nodo_busqueda,NULL);
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

bool apilar_todo_izquierda(pila_t* pila, nodo_t* raiz){
	while (raiz != NULL){
		if (pila_apilar(pila,raiz) == false){
			return false;
		}
		raiz = raiz->izq;
	}
	return true;
}

bool iterar_in_order(nodo_t* raiz,visitar_t visitar, void* extra, const char* ini, const char* fin, abb_comparar_clave_t comparar){
	if (raiz == NULL){
		return true;
	}
	if(comparar(raiz->clave, ini) >= 0){
		if (iterar_in_order(raiz->izq,visitar,extra, ini, fin, comparar) == false){
			return false;
		}
		if(comparar(raiz->clave, fin) <= 0){
			if (visitar(raiz->clave,raiz->dato,extra) == false){
				return false;
			}
		}
	}
	if(comparar(raiz->clave, fin) <= 0){
		return iterar_in_order(raiz->der,visitar, extra, ini, fin, comparar);
	}
	return false;
}

/* ******************************************************************
 *                PRIMITIVAS DE ABB ITERADOR INTERNO
 * *****************************************************************/

void abb_in_order_rango(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra, const char* ini, const char* fin){
	iterar_in_order(arbol->raiz,visitar,extra, ini, fin, arbol->comparar);
}

/* ******************************************************************
 *                PRIMITIVAS DE ABB ITERADOR EXTERNO
 * *****************************************************************/

abb_iter_t *abb_iter_in_crear(const abb_t *arbol){
	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	if (iter == NULL){
		return NULL;
	}
	pila_t* pila = pila_crear();
	if (pila == NULL){
		free(iter);
		return NULL;
	}
	if (apilar_todo_izquierda(pila,arbol->raiz) == false){
		pila_destruir(pila);
		free(iter);
		return NULL;
	}
	iter->arbol = arbol;
	iter->pila = pila;
	return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter){
	if (abb_iter_in_al_final(iter)){
		return false;
	}
	nodo_t* nodo_actual = pila_desapilar(iter->pila);
	return apilar_todo_izquierda(iter->pila,nodo_actual->der);
}
const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
	if (abb_iter_in_al_final(iter)){
		return NULL;
	}
	nodo_t* nodo_actual = pila_ver_tope(iter->pila);
	return nodo_actual->clave;
}
bool abb_iter_in_al_final(const abb_iter_t *iter){
	return pila_esta_vacia(iter->pila);
}

void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->pila);
	free(iter);
}
/*
bool visitar(const char* clave, void* dato, void* extra){
	pila_apilar((pila_t*)extra, dato);
	return true;
}

void pruebas_abb_interno(){
	abb_t* abb = abb_crear(strcmp, NULL);

	char* arreglo[11] = {"200", "100", "300", "50", "150", "250", "350", "125", "175", "225", "275"};
	for(int i = 0; i < 11; i++ ){
		abb_guardar(abb, arreglo[i], arreglo[i]);
	}
	pila_t* pila = pila_crear();
	abb_in_order_rango(abb, visitar, (void*)pila, "175", "275");
	while(!pila_esta_vacia(pila)){
		printf("%s\n", (char*)pila_desapilar(pila));
	}
	pila_destruir(pila);
	abb_destruir(abb);
	return;
}
*/
