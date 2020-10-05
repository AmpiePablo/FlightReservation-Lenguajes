#ifndef CRYPT_H
#define CRYPT_H

void Encrypt(char*);
char EncryptChar(char);

/*
 * EncryptChar
 *
 * Entradas:
 *		Una cadena de caracteres
 *
 * Restricciones:
 *		Debe tener caracteres ascii digitables
 *		por un usuario por medio de la
 *		consola.
 *
 * Objetivo:
 *		Hacer un cifrado de bloques en cadena
 *		de la cadena de caracteres que entró
 *		por parámetro.
 *		
 */
void Encrypt(char *str) {
	
	str[0] = EncryptChar(str[0]);
	
	for (int i = 1; str[i] != '\0'; i++)
		str[i] = EncryptChar(str[i] ^ str[i-1]);
}

/*
 * EncryptChar
 *
 * Entradas:
 *		Un caracter
 *
 * Salidas:
 *		Otro caracter
 *
 * Restricciones:
 *		No debe ser un caracter de escape
 *
 * Objetivo:
 *		Hacer una encriptación de un caracter
 *		por medio de una "función redonda"
 *		arbitraria.
 *		
 */
char EncryptChar(char c) {
	c -= 32;
	c = (c + 67) % 94;
	c += 32;
	return c;
}

#endif