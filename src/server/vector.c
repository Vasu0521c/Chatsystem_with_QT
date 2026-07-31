#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vector.h"

vectar* create_vectar() {
	
	vectar *new_vectar = malloc(sizeof(vectar));
	memset(new_vectar, 0, sizeof(vectar));
	return new_vectar;
}

vectar* create_vectar_with_size(int size) {

	vectar *new_vectar = malloc(sizeof(vectar));
	new_vectar -> data = malloc((2 * size) * sizeof(int));
	memset(new_vectar -> data, 0, (2 * size * (sizeof(int))));
	new_vectar -> size = size * 2;
	return new_vectar;
}

void clear(vectar *vec) {
	
	vectar *clear = vec;
	if(vec -> length == 0) {
		return;
	}
	int i = 0;
	while(i <= (vec -> length)) {
		clear -> data = NULL;
		clear++;
		i++;
	}
	vec -> length = 0;
	return;
}

void pop(vectar *vec) {

	if(vec -> length == 0) {
		vec -> size = 0;
		return;
	}
	vec -> length--;
	int *hell     = malloc(sizeof(int) * vec -> size + 1);
	memset(hell, 0, (vec -> size + 1) * (sizeof(int)));
	memcpy(hell, vec -> data + 1, sizeof(int) * (vec -> length));
	free(vec -> data);
	vec -> data = hell;
    return;
}

vectar* push(vectar *vec, int data) {

	if(vec -> size == 0) {
		vec -> data   = malloc(sizeof(int));
		vec -> data[vec -> length] = data;
		vec -> size   = 1;
		vec -> length = 1;
		return vec;
	}
	if(vec -> length >= vec -> size) {
		vectar *temp   = create_vectar_with_size(vec -> size);
		memcpy(temp -> data, vec -> data, (vec -> size) * sizeof(int));
        temp -> length = vec -> length;
		free(vec);
		vec = temp;
	}
    vec -> data[vec -> length] = data;
	vec -> length = vec -> length + 1;
	return vec;
}
