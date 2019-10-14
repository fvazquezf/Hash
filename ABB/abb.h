#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/
 
typedef struct abb abb_t;

typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);

typedef struct abb_iter abb_iter_t;

/* ******************************************************************
 *                PRIMITIVAS DE ABB
 * *****************************************************************/
// Dada una funcion para destruir datos y una para comparar claves, crea un abb
// Pre: Una funcion de destruir datos y una de comparar claves
// Post: devuelve un nuevo abb, en caso de error devuelve null
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

// Dada una clave y un valor, se guarda el registro en el abb. si la clave ya existe
// reemplaza el valor.
// Pre: el abb esta creado
// Post: se guardo el registro en el abb y devuelve true, false en caso de error
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

// Dada una clave, se borra el registro y se devuelve el valor asociado
// Pre: el abb esta creado
// Post: se eliminó el registro del abb y se devuelve el valor asociado
// Si la clave no existe devuelve NULL
void *abb_borrar(abb_t *arbol, const char *clave);

// Dada una clave, se obtiene el valor asociado al registro
// Pre: el abb esta creado
// Post: se devolvio el valor asociado a la clave, NULL caso contrario
void *abb_obtener(const abb_t *arbol, const char *clave);

// Dada una clave, devuelve true si se encuentra en el abb, false caso contrario
// Pre: el abb esta creado
bool abb_pertenece(const abb_t *arbol, const char *clave);

// Devuelve la cantidad de elementos en el abb
// Pre: el abb esta creado
size_t abb_cantidad(abb_t *arbol);

// Se destruye el abb y todos sus registros
// Pre: el abb esta creado
// Post: se destruyo el abb y sus registros
void abb_destruir(abb_t *arbol);

/* ******************************************************************
 *                    PRIMITIVAS DE ITERADOR INTERNO
 * *****************************************************************/
 
// Recorre los elementos de; abb. Si recibe la funcion visitar por parametro
// la aplica para los valores recorridos. Extra es un parametro de visitar que se
// pasa a la funcion para ser usada dentro del iterador.
// Pre: el abb fue creado. visitar es una funcion valida y extra un valor
// correspondiente a la funcion visitar.
// Post: se aplico visitar a todos los valores previstos.
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);

/* ******************************************************************
 *                    PRIMITIVAS DE ITERADOR EXTERNO
 * *****************************************************************/
// Dado un abb, se crea un iterador del mismo
// Pre: el abb esta creado
// Post: devuelve un iterador de abb
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Se avanza al siguiente registro del abb en el iterador, si esta al final devuelve false
// Pre: el iterador de abb esta creado
// Post: se avanzo al siguiente registro en el iterador, o devuelve false en caso de estar al final
bool abb_iter_in_avanzar(abb_iter_t *iter);

// Devuelve la clave del abb en la posicion actual del iterador
// Pre: el iterador de abb esta creado
// Post: devuelve la clave del abb en la posicion actual
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

// Devuelve true si el iterador se encuentra al final, false caso contrario
// Pre: el iterador de abb esta creado
bool abb_iter_in_al_final(const abb_iter_t *iter);

// Se destruye el iterador de abb
// Pre: el iterador de abb esta creado
// Post: el iterador de abb se destruyo
void abb_iter_in_destruir(abb_iter_t* iter);
