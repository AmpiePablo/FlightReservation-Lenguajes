#ifndef GET_INPUT_H
#define GET_INPUT_H

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