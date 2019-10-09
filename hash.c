#include "hash.h"
#include "lista.h"
#include <stdlib.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/
typedef campo{
	char* clave;
	void* valor;
}campo_t;

struct hash{
	lista_t** listas;
	size_t capacidad;
	size_t cantidad;
	destruir_dato ;//no tengo ni idea como definir la funcion
};

/* ******************************************************************
 *          		      AUXILIARES
 * *****************************************************************/
 //la saque de stasckoverflow, parece confiable pero no creo que funcione por el <<
 unsigned long hashing(unsigned char *str){
	unsigned long h = 5381;
	int c;
	
	while (c = *str++){
		h = ((h << 5) + h) + c;
	}
	return h;
}
 
/* ******************************************************************
 *                PRIMITIVAS DE HASH
 * *****************************************************************/
 
 //no se si vale la pena inicializar las listas aca o mientras las vamos necesitando
 hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t hash = malloc(sizeof(hash_t));
	if (hash == NULL){
		return NULL;
	}
	hash->capacidad = 16; //crear constante si necesario, creo que sea potencia de 2 es bueno
	hash->cantidad = 0;
	//inicialiszar la funcion
	return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	campo_t* campo = malloc(sizeof(campo_t));
	if (campo == NULL){
		return false;
	}
	campo->clave = *clave;
	campo->dato = dato;
	
}

size_t hash_cantidad(const hash_t *hash){
		return hash->cantidad;
}
