#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* La lista está planteada como una lista de punteros genéricos. */

typedef struct lista lista_t;

/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista
// Post: devuelve una nueva lista vacía.
lista_t *lista_crear(void);

// Devuelve true si la lista esta vacía, false caso contrario
// Pre: la lista esta creada
bool lista_esta_vacia(const lista_t *lista);

// Inserta un elemento al principio de la lista, devuelve false en caso de error
// Pre: la lista esta creada
// Post: se inserto un elemento al principio de la lista  
bool lista_insertar_primero(lista_t *lista, void *dato);

// Inserta un elemento al final de la lista, devuelve false en caso de error
// Pre: la lista esta creada
// Post: se insertó un elemento al final de la lista
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Elimina el primer elemento de la lista y devuelve su valor. 
// En caso de que la lista este vacía, se devuelve NULL
// Pre: la lista esta creada
// Post: se devolvio el valor del primer elemento de la lista, en caso de que no este vacía.
void *lista_borrar_primero(lista_t *lista);

// Devuelve el valor del primer elemento de la lista.
// En caso de que la lista este vacía, se devuelve NULL
// Pre: la lista esta creada
// Post: se devolvió el primer valor de la lista, en caso de que no esté vacía
void *lista_ver_primero(const lista_t *lista);

// Devuelve el valor del ultimo elemento de la lista.
// En caso de que la lista este vacía, se devuelve NULL
// Pre: la lista esta creada
// Post: se devolvió el ultimo valor de la lista, en caso de que no esté vacía
void *lista_ver_ultimo(const lista_t* lista);

// Devuelve el largo de la lista
// Pre: la lista esta creada
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct lista_iter lista_iter_t;


/* ******************************************************************
 *           PRIMITIVAS DE ITERADOR EXTERNO DE LA LISTA
 * *****************************************************************/

// Crea un iterador de lista
// Pre: la lista está creada
// Post: devuelve un nuevo iterador de lista
lista_iter_t *lista_iter_crear(lista_t *lista);

// Avanza en una posicion la lista
// Pre: el iterador de lista esta creado
// Post: el iterador de lista avanza en una posición y devuelve
// true si es posible avanzar, false en caso contrario
bool lista_iter_avanzar(lista_iter_t *iter);

// Devuelve el valor de elemento en el que se encuentra el iterador
// Pre: el iterador de la lista esta creado
// Post: se devolvio el valor actual del elemento de la lista, si
// es que no se encuentra al final de la lista
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Devuelve si la lista se encuentra al final, false caso contrario
// Pre: el iterador de lista esta creado
bool lista_iter_al_final(const lista_iter_t *iter);

// Destruye el iterador de lista
// Pre: el iterador de lista esta creado
// Post: se destruyo el iterador de lista
void lista_iter_destruir(lista_iter_t *iter);

// Se inserta el valor en la posicion actual del iterador
// Pre: el iterador de lista esta creado
// Post: Se insertó el valor en la posicion actual del iterador y
// se devuelve true, en caso de que no este al final
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Se borra el elemento de la posicion actual del iterador
// Pre: el iterador de lista esta creado
// Post: se eliminó el valor en la posición actual del iterador y
// se devuelve true, en caso de que no este al final
void *lista_iter_borrar(lista_iter_t *iter);


/* ******************************************************************
 *           PRIMITIVAS DE ITERADOR INTERNO DE LA LISTA
 * *****************************************************************/

// Itera la lista de manera interna
// Pre: la lista esta creada
// Post: se itera la lista y se le aplica la funcion, si es posible
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);


/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

// Realiza pruebas sobre la implementación del alumno.
//
// Las pruebas deben emplazarse en el archivo ‘pruebas_alumno.c’, y
// solamente pueden emplear la interfaz pública tal y como aparece en cola.h
// (esto es, las pruebas no pueden acceder a los miembros del struct cola).
//
// Para la implementación de las pruebas se debe emplear la función
// print_test(), como se ha visto en TPs anteriores.

void pruebas_lista_alumno(void);

#endif // LISTA_H
