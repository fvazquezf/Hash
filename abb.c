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

nodo_t* buscar(nodo_t* raiz,const char *clave,abb_comparar_clave_t comparar){
	if (raiz == NULL){
		return NULL;
	}
	if (comparar(clave,raiz->clave)<0){
		return buscar(raiz->izq,clave,comparar);
	}
	else if (comparar(clave,raiz->clave)>0){
		return abb_guardar(raiz->der,clave,comparar);
	} else {
		return raiz;
	}

}
bool guardar(nodo_t* raiz, abb_t* arbol, const char *clave, void *dato){
	if(raiz == NULL){
		raiz = crearNodo(clave, dato);
		if (raiz == NULL){
			return false;
		}
	}
	else if (arbol->comparar(clave,raiz->clave)<0){
		return abb_guardar(raiz->izq,arbol,clave,dato);
	}
	else if (arbol->comparar(clave,raiz->clave)>0){
		return abb_guardar(raiz->der,arbol,clave,dato);
	} else {
		arbol->destruir(raiz->dato);
		raiz->dato == dato;
	}
	arbol->cantidad ++;
	return true;
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
	return guardar(arbol->raiz, arbol, clave, dato);
	
}

void *abb_borrar(abb_t *arbol, const char *clave){
	return borrar(arbol->raiz, arbol, clave);
}

void *abb_obtener(const abb_t *arbol, const char *clave){
	nodo_t* nodo_busqueda = buscar(arbol->raiz,clave,arbol->comparar);
	if (nodo_busqueda == NULL){
		return NULL;
	}
	return nodo_busqueda->dato;
}

bool abb_pertenece(const abb_t *arbol, const char *clave){
	nodo_t* nodo_busqueda = buscar(arbol->raiz,clave,arbol->comparar);
	if (nodo_busqueda == NULL){
		return false;
	}
	return true;
}