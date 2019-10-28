#include "pila.h"
#include <stdlib.h>
#define MIN_CAPACIDAD 100
#define MULTIPLICADOR_PILA_AGRANDAR 2
#define VALOR_DIVISOR_PILA_ACHICAR 2
#define VALOR_DIVISOR_MINIMO_PILA_ACHICAR 4
/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* FUNCION AUXILIAR REDIMENSION DE PILA*/
bool pila_redimensionar(pila_t* pila,size_t nueva_capacidad){
	if (nueva_capacidad < MIN_CAPACIDAD){
		nueva_capacidad = MIN_CAPACIDAD;
	}
	void **nuevos_datos = realloc(pila->datos,nueva_capacidad * sizeof(void *));
	if (!nuevos_datos){
		return false;
	}
	pila->datos = nuevos_datos;
	pila->capacidad = nueva_capacidad;
	return true;
}

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/
pila_t* pila_crear(void){
	pila_t* pila = malloc(sizeof(pila_t));
	if (!pila){
		return NULL;
	}
	pila->datos = malloc(MIN_CAPACIDAD*sizeof(void *));
	if (!(pila->datos)){
		free(pila);
		return NULL;
	}
	pila->cantidad = 0;
	pila->capacidad = MIN_CAPACIDAD;
	return pila;
}
void pila_destruir(pila_t *pila){
	if (pila){
		free(pila->datos);
		free(pila);
	}
}
bool pila_esta_vacia(const pila_t *pila){
	return pila->cantidad == 0;
}
bool pila_apilar(pila_t *pila, void* valor){
	//Si es necesario redimensionar
	if (pila->cantidad == pila->capacidad){
		if(!pila_redimensionar(pila,pila->capacidad * MULTIPLICADOR_PILA_AGRANDAR)){
			return false;
		}
	}
	pila->datos[pila->cantidad] = valor;
	pila->cantidad++;
	return true;
	
}

void *pila_ver_tope(const pila_t *pila){
	if (pila_esta_vacia(pila)){
		return NULL;
	}	
	return pila->datos[(pila->cantidad)-1];
}

void *pila_desapilar(pila_t *pila){
	if (pila_esta_vacia(pila)){
		return NULL;
	}
	pila->cantidad--;
	void *p_res = pila->datos[pila->cantidad];
	if ((pila->capacidad > MIN_CAPACIDAD) && (pila->cantidad <= (pila->capacidad/VALOR_DIVISOR_MINIMO_PILA_ACHICAR))) {
		pila_redimensionar(pila, pila->capacidad / VALOR_DIVISOR_PILA_ACHICAR);
	}
	return p_res;

}
