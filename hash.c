#define _POSIX_C_SOURCE 200809L // strdup()
#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>

#define FACTOR_DE_CARGA 1
#define MULTIPLICADOR_TABLA_CAPACIDAD 2
#define DIVISOR_MINIMO_ACHICAR 4
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
	for (size_t i = 0; i < capacidad; ++i){
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

bool hash_redimensionar(hash_t* hash,size_t n_capacidad){
	lista_t** tabla_nueva = crea_tabla_hash(n_capacidad);
	if (tabla_nueva == NULL){
		return false;
	}
	lista_t** tabla_h = hash->tabla_h;
	for (int i = 0; i < hash->capacidad; ++i){
		while(lista_esta_vacia(tabla_h[i]) == false){
			campo_t* campo = lista_borrar_primero(tabla_h[i]);
			wrapper_hash_guardar(tabla_nueva,n_capacidad,campo->clave,campo->valor);
			borrar_campo(campo);
		}
		lista_destruir(tabla_h[i], NULL);
	}
	free(tabla_h);
	hash->tabla_h = tabla_nueva;
	return true;
}

void proxima_posicion_valida_iter(lista_t** tabla,size_t* posicion,lista_iter_t** iter_lista){
	while(lista_iter_al_final(*iter_lista)){
		lista_iter_destruir(*iter_lista);
		*posicion = *posicion + 1;
		*iter_lista = lista_iter_crear(tabla[*posicion]);
	}	
} 

void* devuelve_valor(lista_iter_t* iter,const char *clave,bool borrar_devolver,size_t* int_cantidad){
	while(lista_iter_al_final(iter) == false){
		campo_t* campoAux = lista_iter_ver_actual(iter);
		if (strcmp(clave ,campoAux->clave) == 0){
			if (borrar_devolver){
				campo_t* campo = lista_iter_borrar(iter);
				void* auxiliar = campo->valor;
				borrar_campo(campo);
				*int_cantidad = *int_cantidad-1;
				lista_iter_destruir(iter);
				return auxiliar;
			}
			lista_iter_destruir(iter);
			return campoAux->valor;
		}
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);
	return NULL;
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
		if (!hash_redimensionar(hash,hash->capacidad*MULTIPLICADOR_TABLA_CAPACIDAD)){
			return false;
		}
		hash->capacidad = hash->capacidad * MULTIPLICADOR_TABLA_CAPACIDAD;
	}
	if(hash_pertenece(hash, clave)){
		void* dato_antiguo = hash_borrar(hash, clave);
		if(hash->destruir_dato != NULL){
			hash->destruir_dato(dato_antiguo);
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
	//Redimension tabla
	if (hash->capacidad > TABLA_CAPACIDAD_INICIAL && hash->cantidad <= hash->capacidad/DIVISOR_MINIMO_ACHICAR){
		if (!hash_redimensionar(hash,hash->capacidad/MULTIPLICADOR_TABLA_CAPACIDAD)){
			return false;
		}
		hash->capacidad = hash->capacidad / MULTIPLICADOR_TABLA_CAPACIDAD;
	}
	size_t posicion = hashing(clave,hash->capacidad);
	lista_iter_t* iter = lista_iter_crear(hash->tabla_h[posicion]);
	return devuelve_valor(iter,clave,true,&(hash->cantidad));
}

void *hash_obtener(const hash_t *hash, const char *clave){
	if (hash->cantidad == 0){
		return NULL;
	}
	size_t posicion = hashing(clave,hash->capacidad);
	lista_iter_t* iter = lista_iter_crear(hash->tabla_h[posicion]);
	return devuelve_valor(iter,clave,false,NULL);
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
	if (hash_iter == NULL){
		return NULL;
	}
	hash_iter->cantidad_actual = 0;
	hash_iter->hash = hash;
	size_t posicionLista = 0;
	lista_iter_t* auxiliar = lista_iter_crear(hash->tabla_h[posicionLista]);
	if(hash->cantidad != 0){
		proxima_posicion_valida_iter(hash->tabla_h,&posicionLista,&auxiliar);
	} 
	hash_iter->iter_lista = auxiliar;
	hash_iter->posicion_iter = posicionLista;
	return hash_iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	if(hash_iter_al_final(iter)){
		return false;
	}
	lista_iter_avanzar(iter->iter_lista);
	iter->cantidad_actual ++;
	if(hash_iter_al_final(iter)){
		return false;
	}
	proxima_posicion_valida_iter(iter->hash->tabla_h,&(iter->posicion_iter),&(iter->iter_lista));
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
