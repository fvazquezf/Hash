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
void rellena_tabla_null(lista_t** tabla_h, size_t capacidad){
	for (int i = 0; i < capacidad; ++i){
		tabla_h[i] = NULL;
	}
}

lista_t** crea_tabla_hash(size_t capacidad){
	lista_t** tabla_h = malloc(capacidad*sizeof(lista_t*));
	if (tabla_h == NULL){
		return NULL;
	}
	rellena_tabla_null(tabla_h,capacidad);
	return tabla_h;
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
	if (lista_insertar_primero(tabla_h[posicion],(void*)campo) == false){
		borrar_campo(campo);
		if(lista_esta_vacia(tabla_h[posicion])){
			lista_destruir(tabla_h[posicion],NULL);
		}
		return false;
	}
	return true;
}


bool hash_redimensionar(hash_t* hash){
	size_t nueva_capacidad = hash->capacidad*MULTIPLICADOR_TABLA_CAPACIDAD;
	lista_t** tabla_vieja = hash->tabla_h;
	lista_t** tabla_nueva = crea_tabla_hash(nueva_capacidad);
	if (tabla_nueva == NULL){
		return false;
	}
	for (int i = 0; i < hash->capacidad; ++i){
		if (tabla_vieja[i] != NULL){
			campo_t* campo = lista_borrar_primero(tabla_vieja[i]);
			while (campo != NULL){
				wrapper_hash_guardar(tabla_nueva,hash->capacidad,campo->clave,campo->valor);
			}
		}
	}
	
	hash->tabla_h = tabla_nueva;
	return true;
}

nodo_t* hash_buscar(const hash_t *hash, const char *clave){
	size_t posicion = hashing(clave,hash->capacidad);
	lista_t* balde = hash->tabla_h[posicion];
	if (balde == NULL){
		return NULL;
	}
	nodo_t* nodo_actual = lista->prim;
	while(nodo_actual){
		if(!strcmp((char*)nodo_actual->valor_nodo->clave,clave)){
			return nodo_actual;
		}
		nodo_actual = nodo_actual->prox;	
	}
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
	hash->tabla_h = crea_tabla_hash(hash->capacidad);
	if (hash->tabla_h == NULL){
		free(hash);
	}
	return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	if ((hash->cantidad / hash->capacidad) > FACTOR_DE_CARGA){
		if (!hash_redimensionar(hash)){
			return false;
		}
	}
	if (!hash_pertenece(hash,clave)){
		if (!wrapper_hash_guardar(hash->tabla_h,hash->capacidad,clave,dato)){
			return false;
		}	
	} else{
		nodo_t* nodo_busqueda = hash_buscar(hash.clave);
		nodo_busqueda->valor_nodo->valor = dato;
	}
	hash->cantidad++;
	return true;
}

void *hash_obtener(const hash_t *hash, const char *clave){
	nodo_t* nodo_busqueda = hash_buscar(hash,clave);
	if(nodo_busqueda == NULL){
		return NULL;
	}
	return (char*)nodo_busqueda->valor_nodo_busqueda->valor;

}

bool hash_pertenece(const hash_t *hash, const char *clave){
	nodo_t* nodo_busqueda = hash_buscar(hash,clave);
	return nodo_busqueda != NULL;
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}

void *hash_borrar(hash_t *hash, const char *clave){
	size_t posicion = hashing(clave,hash->capacidad);
	lista_t* balde = hash->tabla_h[posicion];
	if (balde == NULL){
		return NULL;
	}
	lista_iter_t* iter = lista_iter_crear(balde);
	while (!lista_iter_al_final(iter)){
		campo_t* campo = (campo_t*)lista_iter_ver_actual(iter);
		if (!strcmp(clave,campo->clave)){
			void* aux = campo->clave;
			borrar_campo(campo);
			lista_iter_borrar(lista_iter_t *iter);
			return campo;
		}
		lista_iter_avanzar(iter);	
	}
	lista_iter_destruir(iter);
	return NULL;
}

void hash_destruir(hash_t *hash){
	lista_t** tabla_h = hash->tabla_h;
	for (int i = 0; i < hash->capacidad; ++i){
		if (tabla_h[i] != NULL){
			campo_t* campo = lista_borrar_primero(tabla_h[i]);
			destruir_dato(campo->dato);
			borrar_campo(campo);
	}
	free(tabla_h);
	free(hash);
}