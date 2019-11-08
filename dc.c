#define _POSIX_C_SOURCE 200809L // strdup()
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "pila.h"
#include "strutil.h"

#define FIN_CADENA '\0'
#define ESPACIO ' '
#define MENSAJE_ERROR "ERROR"
#define OPERACIONES_SIMPLES "+-*/^?"
#define OPERADOR_TERNARIO "?"
#define RAIZ "sqrt"
#define LOGARITMO_N "log\n"
#define LOGARITMO "log"
#define LARGO_AUX 100


int _potencia(int base, int exp){
	if(exp == 0){
		return 1;
	}
	if(exp%2 == 0){
		return _potencia(base*base, exp/2);
	} else {
		return base*_potencia(base*base, (exp-1)/2);
	}
}


int potencia(int base,int exp){
	if (base == 0){
		return 0;
	}
	if (exp == 0){
		return 1;
	}
	if (base < 0){
		if (exp%2 == 0){
			if (exp > 0){
				return _potencia(-base,exp); // exp es par > 0
			}
			return 1 / _potencia(-base,-exp);//exp es par < 0
		} else {
			if (exp > 0){
				return -(_potencia(-base,exp)); // base >0 es impar
			}
			return -1 / _potencia(-base,exp); // base < 0 es impar
		}	
	}
	if (exp < 0){
		return 1 / _potencia(base,-exp);
	}

	return _potencia(base,exp);
}

int _logaritmo(int base, int cont, int argumento){
	if(potencia(base, cont) < argumento){
		return _logaritmo(base, cont+1, argumento);
	}
	else{
		return cont;
	}
}

int logaritmo(int base,int argumento){
	if (argumento == 1){
		return 0;
	}
	if (argumento == base){
		return 1;
	}
	return _logaritmo(base,0,argumento);
}


int _raiz(int n, int ini, int fin){

	int medio = (ini+fin)/2;
	if(medio*medio > n){
		return _raiz(n, ini, medio-1);
	}
	if(medio*medio <= n && (medio+1)*(medio+1) > n){
		return medio;
	}
	return _raiz(n, medio+1, fin);
}

int raiz(int n){
	if (n==0){
		return 0;
	}
	if (n==1){
		return 1;
	}
	return _raiz(n,0,n-1);
}

int ternario(int primero,int segundo, int tercero){
	if(tercero == 0){
		return primero;
	}
	return segundo;
}

void fin_dc(pila_t* pila){
	char* operacion;
	while(!pila_esta_vacia(pila)){
		operacion = pila_desapilar(pila);
		free(operacion);
	}	
	pila_destruir(pila);
}

void imprime_mensaje(char* mensaje){
	fputs(mensaje,stdout);
}

bool es_operador_valido(char* entrada){
	if (*entrada == FIN_CADENA) return false;
	if(strcmp(entrada,RAIZ) == 0 || strcmp(entrada,LOGARITMO) == 0 || strcmp(entrada,LOGARITMO_N) == 0  ){
		return true;
	}
	char* operaciones = OPERACIONES_SIMPLES;
	char* comparacion = strchr(operaciones,*entrada);
	if (comparacion) return true;
	return false;
}

bool es_entero_valido(char entrada[]){
	int i = 0;
	size_t largo = strlen(entrada);
	if(entrada[0]=='-'){
		if(largo == 1){
			return false;
		}
		i++;
	}
	for(; i < largo; i++){
		if(!isdigit(entrada[i])){
			//printf("%s no es numero\n",entrada);
			return false;
		}
	}
	return true;
}
void no_es_entrada_valida(pila_t* pila){
	fin_dc(pila);
	char* mensaje = MENSAJE_ERROR;
	imprime_mensaje(mensaje);
}
void sacar_barra_n(char* linea[]){
	size_t indice = 0;
	while(linea[indice]){
		size_t s = strlen(linea[indice]);
		if (s && (linea[indice][s-1] == '\n')){
			linea[indice][--s] = '\0';
		}
		indice++;
	}

}

void dc(char* linea[]){
	pila_t* pila = pila_crear();
	char aux[LARGO_AUX];
	int resultado;
	char* entrada;
	char* apilado_num;
	char* ultimo;
	char* penultimo;
	int ultimo_num;
	int penultimo_num;
	char* res_char = NULL;
	for (int i = 0; linea[i]; ++i){
		entrada = linea[i];
		//printf("la entrada es :'%s'\n",entrada );
		bool es_operador = false;
		bool es_numero = false;
		if (es_operador_valido(entrada)){
			//printf("%s no es operador\n",entrada );
			es_operador = true;
		} 
		if (es_entero_valido(entrada)){
			//printf("%s no es entero\n",entrada );
			es_numero = true;
		}
		if (!es_numero && !es_operador){
			//printf("ERROR POR : %s no es op ni num\n",entrada);
			no_es_entrada_valida(pila);
			return;
		}
		if (es_numero){
			apilado_num = strdup(entrada);
			pila_apilar(pila,apilado_num);
			continue;
		}
		if (pila_esta_vacia(pila)){
			//printf("ERROR POR : %s no tiene numero para operar\n",entrada);
			no_es_entrada_valida(pila);
			return;
		}
		ultimo = pila_desapilar(pila);
		if (!es_entero_valido(ultimo)){
			//printf("ERROR POR : %s es num\n",ultimo);
			free(ultimo);
			no_es_entrada_valida(pila);
			return;
		}
		ultimo_num = atoi(ultimo);
		if (strcmp(entrada,RAIZ)==0){
			if (ultimo_num<0){
				//printf("ERROR POR :raiz negativa\n");
				free(ultimo);
				no_es_entrada_valida(pila);
				return;
			}
			resultado = raiz(ultimo_num);
			//printf("raiz de %d es igual %d\n",ultimo_num,resultado);
			//lo puedo reusar
			sprintf(aux,"%d",resultado); //Uso aux de pivot
			res_char =  strdup(aux);
			pila_apilar(pila,(void*)res_char);
			free(ultimo);
			continue;
		}
		
		if (pila_esta_vacia(pila)){
			free(ultimo);
			no_es_entrada_valida(pila);
			return;
		}
		
		penultimo = pila_desapilar(pila);
		
		if (!es_entero_valido(ultimo)){
			free(ultimo);
			free(penultimo);
			//printf("ERROR POR :penultimo no es numero\n");
			no_es_entrada_valida(pila);
			return;
		}

		penultimo_num = atoi(penultimo); //base

		if(strcmp(entrada,LOGARITMO)==0 || strcmp(entrada,LOGARITMO_N)==0 ){
			// penultimo base, ultimo argumento
			if ((penultimo_num==1 || penultimo_num <=0) || ultimo_num<=0){
				free(ultimo);
				free(penultimo);
				//printf("ERROR POR : logaritmo invalido\n");
				no_es_entrada_valida(pila);
				return;
			}
			resultado = logaritmo(penultimo_num,ultimo_num);
			//printf("logaritmo de %d en base %d es igual %d\n",ultimo_num,penultimo_num,resultado);
			//lo puedo reusar
			sprintf(aux,"%d",resultado);
			res_char = strdup(aux);
			pila_apilar(pila,(void*)res_char);
			free(ultimo);
			free(penultimo);
			continue;
		}

		if (strcmp(entrada,OPERADOR_TERNARIO)==0){
			char* antepenultimo = pila_desapilar(pila);
			if (!es_entero_valido(antepenultimo)){
				free(ultimo);
				free(penultimo);
				free(antepenultimo);
				//printf("ERROR POR :ante penultimo no es numero\n");
				no_es_entrada_valida(pila);
				return;
			}
			int antepenultimo_num = atoi(antepenultimo);
			resultado = ternario(ultimo_num,penultimo_num,antepenultimo_num);
			//printf("operador ternario %d? %d : %d -> %d\n",ultimo_num,penultimo_num,antepenultimo_num,resultado);
			sprintf(aux,"%d",resultado);
			res_char = strdup(aux);
			pila_apilar(pila,(void*)res_char);
			free(ultimo);
			free(penultimo);
			free(antepenultimo);
			continue;		
		}
		switch (*entrada){
			case('+'):
				resultado = (ultimo_num) + (penultimo_num);
				//printf("la suma de %d y %d es igual %d\n",ultimo_num,penultimo_num,resultado);
				break;
			case('-'):
				resultado = (ultimo_num) - (penultimo_num);
				//printf("la resta de %d y %d es igual %d\n",ultimo_num,penultimo_num,resultado);
				break;
			case('*'):
				resultado = (ultimo_num) * (penultimo_num);
				//printf("la multi de %d y %d es igual %d\n",ultimo_num,penultimo_num,resultado);
				break;
			case('/'):
				if (penultimo_num == 0){
					free(ultimo);
					free(penultimo);
					no_es_entrada_valida(pila);
					return;
				}
				resultado = (ultimo_num) / (penultimo_num);
				//printf("la div de %d y %d es igual %d\n",ultimo_num,penultimo_num,resultado);
				break;
			case('^'):
				//ultimo es base, penultimo exponente
				if (ultimo_num == 0 && penultimo_num == 0){
					free(ultimo);
					free(penultimo);
					no_es_entrada_valida(pila);
					return;
				}
				resultado = potencia(ultimo_num,penultimo_num);

				//printf("la potencia de %d y %d es igual %d\n",ultimo_num,penultimo_num,resultado);
				break;
		}
		sprintf(aux,"%d",resultado);
		res_char = strdup(aux);
		pila_apilar(pila,(void*)res_char);
		free(ultimo);
		free(penultimo);	
	}
	if (res_char) imprime_mensaje(res_char);
	fin_dc(pila);
}

int main(int argc, char *argv[]){
	char* linea = NULL;
	char** actual;
	size_t capacidad = 0;
	//int i = 1;
	while (getline(&linea,&capacidad,stdin)!=-1){
		actual = split(linea,ESPACIO);
		sacar_barra_n(actual);
		//printf("Caso %d:",i);
		//i++;
		dc(actual);
		//printf("\n");
		if (actual) free_strv(actual);
	}
	if (linea) free(linea);
	return 0;
}
