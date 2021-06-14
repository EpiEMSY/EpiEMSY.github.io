#include "Internal_fun.h"
#ifndef LAYER_H
#define LAYER_H

typedef struct Layer Layer;
struct Layer{
	char   type;    //could be F for FC or A for Activation
	size_t input_size;
	size_t output_size;
	double *input;   //RECEVE A 1D BOARD
	double *output;  //RECEVE A 1D BOARD
	//USED IN FCLAYERS
	double *weights; //RECEVE A 2D BOARD
	double *bias;    //RECEVE A 1D BOARD
};

void print_Layer(Layer* layer, int num, char* name);

Layer* __Layer_def_FC(size_t input_size, size_t output_size);

Layer* __Layer_def_A(size_t size);

Layer* __Layer_cpy(Layer* layer);

void __free_layer(Layer *layer);

void forward_propagation(Layer *layer);

void __Layer_mutate(Layer *layer);

Layer** __Layer_cross(Layer *layer1, Layer *layer2, int num);

#endif
