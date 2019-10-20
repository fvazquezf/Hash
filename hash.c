#define _POSIX_C_SOURCE 200809L // strdup()
#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>



#define FACTOR_DE_CARGA 1
#define MULTIPLICADOR_TABLA_CAPACIDAD 2
#define TABLA_CAPACIDAD_INICIAL 16

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
	hash_destruir_dato_t destruir_dato;

};

struct hash_iter {
	size_t cantidad_actual;
	const hash_t* hash;
	lista_iter_t* iter_lista;
	size_t posicion_iter;
};

/* ******************************************************************
 *          		      AUXILIARES
 * *****************************************************************/

lista_t** crea_tabla_hash(size_t capacidad){
	lista_t** tabla_h = malloc(sizeof(lista_t*) * capacidad);
	if (tabla_h == NULL){
		return NULL;
	}
	size_t auxiliar = 0;
	for (int i = 0; i < capacidad; ++i){
		tabla_h[i] = lista_crear();
		if(tabla_h[i] == NULL){
			auxiliar = i;
			i = capacidad;
		}
	}
	if (auxiliar != 0){
		for (int i = 0; i < auxiliar; ++i){
			lista_destruir(tabla_h[i], NULL);
		}
		free(tabla_h);
		return NULL;
	}
	return tabla_h;
}

size_t hashing(const char *str,size_t capacidad_tabla){
	size_t h = 5381;
	while (*str){
		h = ((h * 10) + *str++) - '0' ;
	}
	
	return h%capacidad_tabla;
}

campo_t* crear_campo(const char *clave, void *dato){
	campo_t* campo = malloc(sizeof(campo_t));
	if (campo == NULL){
		return NULL;
	}
	char *copia_clave = strdup(clave);
	if (copia_clave == NULL){
		free(campo);
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
	if (lista_insertar_primero(tabla_h[posicion],(void*)campo) == false){
		borrar_campo(campo);
		return false;
	}
	return true;
}

bool hash_redimensionar(lista_t** tabla_h,size_t capacidad){
	lista_t** tabla_nueva = crea_tabla_hash(capacidad);
	if (tabla_nueva == NULL){
		return false;
	}
	campo_t* campo;
	for (int i = 0; i < (capacidad/MULTIPLICADOR_TABLA_CAPACIDAD); ++i){
		while(lista_esta_vacia(tabla_h[i]) == false){
			campo = lista_borrar_primero(tabla_h[i]);
			wrapper_hash_guardar(tabla_nueva,capacidad,campo->clave,campo->valor);
			borrar_campo(campo);
		}
		lista_destruir(tabla_h[i], NULL);
	}
	free(tabla_h);
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
	hash->cantidad = 0;
	hash->tabla_h = crea_tabla_hash(hash->capacidad);
	if (hash->tabla_h == NULL){
		free(hash);
	}
	hash->destruir_dato = destruir_dato;
	return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	if ((hash->cantidad / hash->capacidad) > FACTOR_DE_CARGA){
		bool redimension = hash_redimensionar(hash->tabla_h,hash->capacidad*MULTIPLICADOR_TABLA_CAPACIDAD);
		if (!redimension){
			return false;
		}
		hash->capacidad = hash->capacidad * MULTIPLICADOR_TABLA_CAPACIDAD;
	}
	if(hash_pertenece(hash, clave)){
		void* dato = hash_borrar(hash, clave);
		if(hash->destruir_dato != NULL){
			hash->destruir_dato(dato);
		}
	}
	if (!wrapper_hash_guardar(hash->tabla_h,hash->capacidad,clave,dato)){
		return false;
	}
	hash->cantidad++;
	return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
	if (hash->cantidad == 0){
		return NULL;
	}
	size_t posicion = hashing(clave,hash->capacidad);
	lista_iter_t* iter = lista_iter_crear(hash->tabla_h[posicion]);
	campo_t* campoAux;
	while(lista_iter_al_final(iter) == false){
		campoAux = lista_iter_ver_actual(iter);
		if (strcmp(clave ,campoAux->clave) == 0){
			campo_t* campo = lista_iter_borrar(iter);
			void* auxiliar = campo->valor;
			borrar_campo(campo);
			lista_iter_destruir(iter);
			hash->cantidad --;
			return auxiliar;
		}
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);
	return NULL;
}

void *hash_obtener(const hash_t *hash, const char *clave){
	if (hash->cantidad == 0){
		return NULL;
	}
	size_t posicion = hashing(clave,hash->capacidad);
	lista_iter_t* iter = lista_iter_crear(hash->tabla_h[posicion]);
	campo_t* campoAux;
	while(lista_iter_al_final(iter) == false){
		campoAux = lista_iter_ver_actual(iter);
		if (strcmp(clave ,campoAux->clave) == 0){
			lista_iter_destruir(iter);
			return campoAux->valor;
		}
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);
	return NULL;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	if (hash->cantidad == 0){
		return false;
	}
	size_t posicion = hashing(clave,hash->capacidad);
	lista_iter_t* iter = lista_iter_crear(hash->tabla_h[posicion]);
	campo_t* campoAux;
	while(lista_iter_al_final(iter) == false){
		campoAux = lista_iter_ver_actual(iter);
		if (strcmp(clave ,campoAux->clave) == 0){
			lista_iter_destruir(iter);
			return true;
		}
		lista_iter_avanzar(iter);
		
	}
	lista_iter_destruir(iter);
	return false;
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}

void hash_destruir(hash_t *hash){
	campo_t* campo = NULL;
	for (int i = 0; i < hash->capacidad; ++i){
		while(lista_esta_vacia(hash->tabla_h[i]) == false){
			campo = lista_borrar_primero(hash->tabla_h[i]);
			if (hash->destruir_dato != NULL){
				hash->destruir_dato(campo->valor);
			}
			borrar_campo(campo);
		}
		lista_destruir(hash->tabla_h[i], NULL);
	}
	free(hash->tabla_h);
	free(hash);
}

/* ******************************************************************
 *						ITERADOR DE HASH
 * *****************************************************************/
 
hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t* hash_iter = malloc(sizeof(hash_iter_t));
	hash_iter->cantidad_actual = 0;
	hash_iter->hash = hash;
	int posicionLista = 0;
	lista_iter_t* auxiliar = lista_iter_crear(hash->tabla_h[posicionLista]);
	if(hash->cantidad != 0){
		while(lista_iter_al_final(auxiliar)){
			lista_iter_destruir(auxiliar);
			posicionLista++;
			auxiliar = lista_iter_crear(hash->tabla_h[posicionLista]);
		}
	} 
	hash_iter->iter_lista = auxiliar;
	hash_iter->posicion_iter = posicionLista;
	return hash_iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	if(hash_iter_al_final(iter)){
		return false;
	}
	if(lista_iter_al_final(iter->iter_lista) == false){
		if (lista_iter_avanzar(iter->iter_lista));{
			iter->cantidad_actual ++;
			return true;
		}
		return false;
	}
	while(lista_iter_al_final(iter->iter_lista)){
		lista_iter_destruir(iter->iter_lista);
		iter->posicion_iter ++;
		iter->iter_lista = lista_iter_crear(iter->hash->tabla_h[iter->posicion_iter]);
	}
	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if(hash_iter_al_final(iter)){
		return NULL;
	}
	campo_t* campo = lista_iter_ver_actual(iter->iter_lista);
	return campo->clave;
}

bool hash_iter_al_final(const hash_iter_t *iter){
	return iter->cantidad_actual == iter->hash->cantidad;
}

void hash_iter_destruir(hash_iter_t* iter){
	lista_iter_destruir(iter->iter_lista);
	free(iter);
}
