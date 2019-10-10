#include "hash.h"
#include "lista.h"
#include <stdlib.h>

#define TABLA_CAPACIDAD_INICIAL 16
#define TABLA_CANTIDAD_INICIAL 0

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/
struct campo{
	char* clave;
	void* valor;
};
typedef struct campo campo_t;

struct hash{
	lista_t** tabla_h; //Arreglo de listas enlazadas. No estoy seguro si es así o void**
	size_t capacidad;
	size_t cantidad;
	hash_destruir_dato_t destruir_dato;
};


/* ******************************************************************
 *          		      AUXILIARES
 * *****************************************************************/
 unsigned long hashing(unsigned char *str){
	unsigned long h = 5381;
	int c;
	while (c = *str++){
		h = ((h << 5) + h) + c;
	}
	return h;
}

void rellena_tabla_null(lista_t** hash, size_t capacidad){
 	//Se rellena con NULL la tabla
	for (int i = 0; i < capacidad; ++i){
		tabla_h[i] = NULL;
	}
}
 
/* ******************************************************************
 *						PRIMITIVAS DE HASH
 * *****************************************************************/
 
 hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if (hash == NULL){
		return NULL;
	}
	
	hash->capacidad = TABLA_CAPACIDAD_INICIAL;
	hash->tabla_h = malloc(hash->capacidad*sizeof(lista_t));
	if (tabla_h == NULL){
		free(hash);
		return NULL;
	}
	hash->cantidad = TABLA_CANTIDAD_INICIAL;
	hash->destruir_dato = destruir_dato;
	rellena_tabla_null(hash->tabla_h,hash->capacidad);
	return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	campo_t* campo = malloc(sizeof(campo_t));
	if (campo == NULL){
		return false;
	}
	campo->clave = *clave;
	campo->dato = dato;
	size_t posicion = hashing(clave)%hash->capacidad;

	if (hash->tabla_h[posicion] == NULL){
		tabla_h[posicion] = lista_crear();
	}
	bool bool_resultado = lista_insertar_primero(tabla_h[posicion],(void*)campo);
	hash->cantidad++;
	return bool_resultado;
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}
