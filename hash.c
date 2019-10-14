#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>

#define FACTOR_DE_CARGA 1
#define MULTIPLICADOR_TABLA_CAPACIDAD 2
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
	lista_t** tabla_h;
	size_t capacidad;
	size_t cantidad;
	hash_destruir_dato_t* destruir_dato;
};

/* ******************************************************************
 *          		      AUXILIARES
 * *****************************************************************/

lista_t** crea_tabla_hash(size_t capacidad){
	lista_t** tabla_h = malloc(capacidad*sizeof(lista_t));
	if (tabla_h == NULL){
		return NULL
	}
	rellena_tabla_null(tabla_h);
	return tabla_h;
}
void rellena_tabla_null(lista_t** tabla_h, size_t capacidad){
	for (int i = 0; i < capacidad; ++i){
		tabla_h[i] = NULL;
	}
}
unsigned long hashing(unsigned char *str,size_t capacidad){
	unsigned long h = 5381;
	int c;
	while (c = *str++){
		h = ((h << 5) + h) + c;
	}
	return h%capacidad;
}

campo_t* crear_campo(const char *clave, void *dato){
	campo_t* campo = malloc(sizeof(campo_t));
	if (campo == NULL){
		return NULL;
	}
	char *copia_clave = strdup(clave);
	if (copia_clave == NULL){
		return NULL;
	}
	campo->clave = copia_clave;
	campo->valor = dato;
	return campo;
}

void borrar_campo(campo_t* campo){
	free(campo->clave);
	free(campo);
}

bool wrapper_hash_guardar(lista_t** tabla_h,size_t capacidad, const char *clave, void *dato){
	campo_t* campo = crear_campo(clave,dato);
	if (campo == NULL){
		return false;
	}
	size_t posicion = hashing(campo->clave,capacidad);
	if (tabla_h[posicion] == NULL){
		tabla_h[posicion] = lista_crear();
	}

	//Entra si fallo el lista_crear()
	if (tabla_h[posicion] == NULL){
		borrar_campo(campo);
		return false;	
	}

	//Entra si fallo lista_insertar()
	if (!lista_insertar_primero(tabla_h[posicion],(void*)campo)){
		borrar_campo(campo);
		if(lista_esta_vacia(tabla_h[posicion])){
			lista_destruir(tabla_h[posicion],NULL);
		}
		return false;
	}
	return true;
}

bool hash_redimensionar(lista_t** tabla_h,size_t capacidad){
	lista_t** tabla_nueva = crea_tabla_hash(capacidad);
	if (tabla_nueva == NULL){
		return false;
	}
	for (int i = 0; i < capacidad; ++i){
		if (tabla_h[i] != NULL){
			campo_t* campo = lista_borrar_primero(tabla_h[i]);
			while (campo != NULL){
				//Que hay que hacer si falla la copia?
				//¿Llamamos a tabla_destruir ?
				wrapper_hash_guardar(tabla_nueva,capacidad,campo->clave,campo->valor);
			}
		}
	}
	tabla_h = tabla_nueva;
	return true;
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
	hash->tabla_h = crea_tabla_hash(hash->capacidad);
	if (hash->tabla_h == NULL){
		free(hash);
	}
	return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	if ((hash->cantidad / hash->capacidad) > FACTOR_DE_CARGA){
		bool redimension = hash_redimensionar(hash->tabla_h,hash->capacidad*MULTIPLICADOR_TABLA_CAPACIDAD);
		if (!redimension){
			return false;
		}
	}
	if (!wrapper_hash_guardar(hash->tabla_h,hash->capacidad,clave,dato)){
		return false;
	}
	hash->cantidad++;
	return true;
}

void *hash_obtener(const hash_t *hash, const char *clave){
	size_t posicion = hashing(clave,hash->capacidad);
	lista_t* balde = hash->tabla_h[posicion];
	if (balde == NULL){
		return NULL;
	}
	nodo_t* registro = balde->prim;
	while (registro){
		campo_t* campo = (campo_t*)registro->valor_nodo;
		if (strcmp(campo->clave,clave) == 0){
			return campo->valor;
		}
		registro = registro->prox;
	}
	return NULL;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	return hash_obtener(hash,clave) != NULL;
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}

void *hash_borrar(hash_t *hash, const char *clave){
}