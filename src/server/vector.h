#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    int *data;
	int length;
	int size;
} vectar;


vectar* create_vectar();
vectar* create_vectar_with_size(int size);
void pop(vectar *vec);
vectar* push(vectar *vec, int data);

#endif
