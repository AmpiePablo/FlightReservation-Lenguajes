#ifndef GET_INPUT_H
#define GET_INPUT_H

/*
 * GetInput
 *
 * Entradas:
 *		Una secuencia de caracteres digitada por el usuario
 *		por medio de la consola.
 *
 * Salidas:
 *		La secuencia de caracteres digitada por el usuario
 *		en forma de un char* delimitado por '\0'
 *		(retorno de función)
 *
 * Objetivo:
 *		Almacenar todos los caracteres que el usuario
 *		quiera digitar por consola, sin limitación de
 *		cantidad de caracteres ni caracteres de espacio,
 *		y luego delimitarlo con '\0'
 */
char* GetInput() {
	
	unsigned int size = INIT_SIZE;
	
	char *input = (char*) malloc (size * sizeof(char));
	
	unsigned int length = 0;
	
	char c;
	
	while ( ( c = getchar() ) != '\n' ) {
		
		if (length == size) {
			
			size += size/2;
			input = (char*) realloc (input, size * sizeof(char));
		}
		input[length++] = c;
	}
	input = (char*) realloc (input, (length + 1) * sizeof(char));
	input[length] = '\0';
	
	return input;
}

#endif
