#ifndef HASH_H
#define HASH_H

#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct hash hash_t;
typedef struct hash_iter hash_iter_t;
typedef void (*hash_destruir_dato_t)(void *);


/* ******************************************************************
 *                PRIMITIVAS DE HASH
 * *****************************************************************/

// Dada una funcion para destruir campos, crea una tabla de hash
// Pre: Una funcion de destruir campos de una tabla de hash
// Post: devuelve una nueva tabla de hash, en caso de error de destruye
hash_t *hash_crear(hash_destruir_dato_t destruir_dato);

// Dada una clave y un valor, se guarda el registro en el hash. si la clave ya existe
// reemplaza el valor.
// Pre: la tabla de hash esta creada
// Post: se guardo el registro en el hash y devuelve true, false en caso de error
bool hash_guardar(hash_t *hash, const char *clave, void *dato);

// Dada una clave, se borra el registro y se devuelve el valor asociado
// Pre: la tabla de hash esta creada
// Post: se eliminó el registro del hash y se devuelve el valor asociado
// Si la clave no existe devuelve NULL
// En caso de error, devuelve NULL
void *hash_borrar(hash_t *hash, const char *clave);

// Dada una clave, se obtiene el valor asociado al registro
// Pre: la tabla de hash esta creada
// Post: se devolvio el valor asociado a la clave, NULL caso contrario
void *hash_obtener(const hash_t *hash, const char *clave);

// Dada una clave, devuelve true si se encuentra en el hash, false caso contrario
// Pre: la tabla de hash esta creada
bool hash_pertenece(const hash_t *hash, const char *clave);

// Devuelve la cantidad de elementos en el hash
// Pre: la tabla de hash esta creada
size_t hash_cantidad(const hash_t *hash);

// Se destruye el hash y todos sus registros
// Pre: la tabla de hash esta creada
// Post: se destruyo el hash y sus registros
void hash_destruir(hash_t *hash);


/* ******************************************************************
 *                    PRIMITIVAS DE ITERADOR HASH
 * *****************************************************************/

// Dada una tabla de hash, se crea un iterador del mismo
// Pre: el hash esta creado
// Post: devuelve un iterador de hash
hash_iter_t *hash_iter_crear(const hash_t *hash);

// Se avanza al siguiente registro del hash en el iterador
// Pre: el iterador de hash esta creado
// Post: se avanzo al siguiente registro en el iterador
bool hash_iter_avanzar(hash_iter_t *iter);

// Devuelve el registro del hash en la posicion actual del iterador
// Pre: el iterador de hash esta creado
// Post: devuelve el registro del hash en la posicion actual
const char *hash_iter_ver_actual(const hash_iter_t *iter);

// Devuelve true si el iterador se encuentra al final, false caso contrario
// Pre: el iterador de hash esta creado
bool hash_iter_al_final(const hash_iter_t *iter);

// Se destruye el iterador de hash
// Pre: el iterador de hash esta creado
// Post: el iterador de hash se destruyo
void hash_iter_destruir(hash_iter_t* iter);

#endif // HASH_H

