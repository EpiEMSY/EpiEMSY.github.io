#include "Layer.h"

#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

typedef struct Network Network;
struct Network{
	int layer_size;
	Layer** layers;
	double note;
};

//SAVES FUCTIONS
void rewrite(int fd, const void *buf, size_t count);
int filesave_mat(double* mat, size_t dim, const char* file_name);
char* info_layer(Layer* lay, size_t num);
int Save_Network(Network* net);

//LOAD FUNCITONS
double* fileload_mat(size_t dim, const char* file_name);
Layer* __layer_load(char type, size_t input_size, size_t output_size, size_t i);
Network* Load_Network();

//This function return a random neural network
Network* __Network_def_random(size_t layer_size);

Network** __Network_def_gen_1(size_t numb_network, size_t layer_size);

void __Network_free(Network* net);

void __Gen_free(size_t numb_network, Network** net);

//This function add a layer to the neural network.
//void __Network_add_Layer(Layer *layer);

//This function do forward propagation in a full network
void __Network_propagation(Network* net, double* input);

// Sort a list of Network*
void XOR_Gen_sort(Network** nets, size_t size);

void __Gen_popagation(Network** nets, double* input, size_t size);

//TRAIN FUNCTIONS

void choose_move(Network* net, double* moves, size_t c);

void GAME_TRAIN(size_t net_s, size_t gen);
Network* Load_Network(size_t layer_size);

#endif
