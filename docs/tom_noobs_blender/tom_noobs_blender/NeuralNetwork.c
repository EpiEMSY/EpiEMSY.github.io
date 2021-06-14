#include "NeuralNetwork.h"
#include "plain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <err.h>
#include <unistd.h>

const size_t __input_size  = 2;
const size_t __output_size = 1;
const size_t __layer_count = 5;

Network* Load_Network(size_t layer_size){
	FILE *fp = fopen("network.dat","r");
	if (fp == NULL)
	{
		return __Network_def_random(layer_size);
	}
	char * line = NULL;
	size_t len = 0;
  ssize_t read;
	Network* net = (Network*) malloc(sizeof(Network));
	size_t i = 0;
	size_t lay_size;
	size_t in_size = 0;
	size_t out_size = 0;
	char type;
	read = getline(&line, &len, fp);
	lay_size = (size_t)atol(line);
	net->layer_size = lay_size;
	net->layers = (Layer **) malloc(lay_size * sizeof(Layer*));
	while ((read = getline(&line, &len, fp)) != -1) {
		if (i%3 == 0)
		{
			/* reset + add in_size */
			in_size = (size_t)atol(line);
		}
		if (i%3 == 1)
		{
			/* out_size */
			out_size = (size_t)atol(line);
		}
		if (i%3 == 2)
		{
			/* type + create the lay with layer load and add it to net->layers[i/3]*/
			type = line[0];
			net->layers[i/3] = __layer_load(type, in_size, out_size, i/3);
		}
		i ++;
  }
  fclose(fp);
  if (line)
    free(line);
  return net;
}

Layer* __layer_load(char type, size_t input_size, size_t output_size, size_t i){
	Layer* lay;
	if (type == 'F')
	{
		lay = __Layer_def_FC(input_size, output_size);
		char name_weight[15];
		char name_bias[15];
		sprintf(name_weight, "weights%hu", (unsigned short)i);
		sprintf(name_bias, "bias%hu", (unsigned short)i);
		free(lay->weights); free(lay->bias);
		lay->weights = fileload_mat(input_size*output_size, name_weight);
		lay->bias    = fileload_mat(output_size, name_bias);
	}
	else{
		lay = __Layer_def_A(output_size);
	}
	return lay;
}
double* fileload_mat(size_t dim, const char* file_name){
	FILE* savefile = fopen(file_name, "rb");
	if (savefile == NULL)
		errx(3,"couldn't read the file");
	double* tab = (double*) malloc(dim * sizeof(double));
	if (fread(tab, sizeof(double), dim, savefile) < dim)
		errx(3,"hasn't read the good amount of data");
	if (fclose(savefile)) errx(3,"couldn't close the file");
	return tab;
}

int Save_Network(Network* net){
	FILE *f = fopen("network.dat", "w");
	fprintf(f, "%i\n", net->layer_size);
	char* string_temp;
	for(int i = 0; i < net->layer_size; i++)
	{
		string_temp = info_layer(net->layers[i], i);
		fprintf(f, "%s", string_temp);
		free(string_temp);
	}
	fclose(f);
	return 0;
}

char* __to_str_pointer(char* in){
	size_t l = strlen(in);
	char* out = (char *) malloc(l * sizeof(char));
	for (size_t i = 0; i < l; ++i)
	 {
	 	out[i] = in[i];
	 }
	 return out;
}

char* info_layer(Layer* lay, size_t num){
	char string[] = "";
	char line_input[15];
	char line_output[15];
	char type[5]; 
	sprintf(line_input, "%lu\n", lay->input_size);
	sprintf(line_output, "%lu\n", lay->output_size);
	strcat(string,line_input);
	strcat(string,line_output);
	if (lay->type == 'F')
	{
		char name_weight[15];
		char name_bias[15];
		sprintf(name_weight, "weights%lu", num);
		sprintf(name_bias, "bias%lu", num);
		filesave_mat(lay->weights, lay->input_size * lay->output_size, name_weight);
		filesave_mat(lay->bias, lay->output_size, name_bias);
		strcpy(type,"F\n");
	}
	else{
		strcpy(type,"A\n");
	}
	strcat(string,type);
	return __to_str_pointer(string);
}

int filesave_mat(double* mat, size_t dim, const char* file_name){
	FILE* savefile = fopen(file_name, "w");
	if (savefile == NULL)
		errx(3,"couldn't overwitre or write the file");
	if (fwrite(mat, sizeof(double), dim, savefile) < dim)
		errx(3,"save data in the file");
	if (fclose(savefile)) errx(3,"couldn't close the file");
	return 0;
}

void rewrite(int fd, const void *buf, size_t count)
{
    size_t total = 0;
    int a = 0;
    while (total < count)
    {
        if ((a = write(fd, buf, count)) == -1)
            errx(1,"problem in rewrite");
        buf += a;
        total += a;
    }
}

//Layer* __Layer_cpy(Layer* layer);
Network* __Network_cpy(Network* net)
{
	Network* child = (Network *) malloc(sizeof(Network));
	child->layer_size = net->layer_size;
	child->layers = (Layer**) malloc(net->layer_size * sizeof(Layer *));
	if (child->layers == NULL) errx(3,"calloc shutdown");
	for (int i = 0; i < net->layer_size; ++i)
	{
		child->layers[i] = __Layer_cpy(net->layers[i]);
	}
	return child;
}

Network* __Network_def_random(size_t layer_size){
	if (layer_size < 1)
	{
		return NULL;
	}
	Network* net = (Network *) malloc(sizeof(Network));
	//Each layer has an activation layer
	net->layer_size = layer_size*2;
	net->layers = (Layer**) malloc (layer_size * 2 * sizeof(Layer *));
	size_t last_size = __input_size; //The input size
	size_t act_size;
	for (size_t i = 0; i < layer_size; ++i)
	{
		if (i == layer_size-1) act_size = __output_size;
		else act_size = random_d(3);
		net->layers[i*2]   = __Layer_def_FC(last_size,act_size);
		net->layers[i*2+1] = __Layer_def_A(act_size);
		last_size = act_size;
	}
	return net;
}

void __Network_mutate(Network* net){
	for (int i = 0; i < net->layer_size; ++i)
	{
		__Layer_mutate(net->layers[i]);
	}
}

// void __Layer_cross(Layer *layer1, Layer *layer2, Layer *child1, Layer *child2, int num)
// Layer* __Layer_def_A(size_t size)
void __Network_cross(Network* net1, Network* net2, 
	Network** nets, size_t child, int num){
	size_t output_size_mid ;
	Layer** childs;
  nets[child] = (Network *) malloc(sizeof(Network));
  nets[child+1] = (Network *) malloc(sizeof(Network));
  nets[child]->layer_size = net1->layer_size;
  nets[child+1]->layer_size = net1->layer_size;
  nets[child]->layers = (Layer**) malloc(net1->layer_size * sizeof(Layer *));
  nets[child+1]->layers = (Layer**) malloc(net1->layer_size * sizeof(Layer *));
	for (int i = 0; i < net1->layer_size; ++i)
	{
		if (net1->layers[i]->type == 'F')
		{
			childs = __Layer_cross(net1->layers[i], net2->layers[i], num);
			nets[child]->layers[i] = childs[0];
			nets[child+1]->layers[i] = childs[1];
			free(childs);
		}
		else{
  		output_size_mid = (net1->layers[i]->output_size+net2->layers[i]->output_size)/2;
  		nets[child]->layers[i] = __Layer_def_A(output_size_mid);
  		nets[child+1]->layers[i] = __Layer_def_A(output_size_mid);
		}
	}
}

Network** __Network_def_gen_1(size_t numb_network, size_t layer_size)
{
	Network** net_list = (Network **) calloc(numb_network,sizeof(Network*));
	for (size_t i = 0; i < numb_network; ++i)
	{
		net_list[i] = __Network_def_random(layer_size);
	}
	return net_list;
}

Network** __Gen_cpy(Network** nets, size_t size)
{
	Network** child = (Network **) malloc(sizeof(Network*) * size);
	for (size_t i = 0; i < size; ++i)
	{
		child[i] = __Network_cpy(nets[i]);
	}
	return child;
}

void __Gen_popagation(Network** nets, double* input, size_t size)
{
	for (size_t i = 0; i < size; ++i)
	{
		__Network_propagation(nets[i], input);
	}
}

void __Network_propagation(Network* net, double* input)
{
	cpy_mat(input,net->layers[0]->input,net->layers[0]->input_size);
	for (int i = 0; i < net->layer_size-1; ++i)
	{
		forward_propagation(net->layers[i]);
		cpy(net->layers[i]->output,net->layers[i+1]->input, 1, net->layers[i]->output_size);
	}
	forward_propagation(net->layers[net->layer_size-1]);
}

void __Network_free(Network* net)
{
	for (int i = 0; i < net->layer_size; ++i)
		__free_layer(net->layers[i]);
	free(net->layers);
	free(net);
}

void __Gen_free(size_t numb_network, Network** net)
{
	for (size_t i = 0; i < numb_network; ++i)
	{
		__Network_free(net[i]);
	}
	free(net);
}

void __print_network(Network* net){
  for (int i = 0; i < net->layer_size; ++i)
  {
  	print_Layer(net->layers[i], i, "from net");
  	printf("\n\n\n");
  }
}

void __XOR_Network_evaluate(Network* net){
	double e1[2] = {0,0};
	double e2[2] = {1,0};
	double e3[2] = {0,1};
	double e4[2] = {1,1};
	double r1[1] = {0};
	double r2[1] = {1};
	double _obtained = 0;
	__Network_propagation(net,e1);
	_obtained = (double)absolute(r1[0]-net->layers[net->layer_size-1]->output[0]);
	__Network_propagation(net,e2);
	_obtained += (double)absolute(r2[0]-net->layers[net->layer_size-1]->output[0]);
	__Network_propagation(net,e3);
	_obtained += (double)absolute(r2[0]-net->layers[net->layer_size-1]->output[0]);
	__Network_propagation(net,e4);
	_obtained += (double)absolute(r1[0]-net->layers[net->layer_size-1]->output[0]);
	net->note = _obtained/4;
}

void XOR_Gen_evaluate(Network** nets, size_t size){
	for (size_t i = 0; i < size; ++i)
	{
		__XOR_Network_evaluate(nets[i]);
	}
}

Network** Gen_evoluate(Network** nets, size_t size){
	Network** next_gen = (Network **) calloc(size, sizeof(Network*));
	size_t best_end = size/10;
	size_t worst_start = (size * 9)/10;
	//CPY BEST
	for (size_t i = 0; i < best_end; ++i)
	{
		next_gen[i] = __Network_cpy(nets[i]);
	}
	//NEW WORST
	for (size_t i = worst_start; i < size; ++i)
	{
		next_gen[i] = __Network_def_random(nets[0]->layer_size);
	}

	//size_t first = 0;

	size_t k = best_end-1;
	//size_t j = 0;
	int _random_num;
	//int _random_num_t;
	while(k <=worst_start)
	{
		for (size_t i = 0; i < size && k <=worst_start; ++i)
		{
			if (i < best_end)
				_random_num = random_d(8);
			else
				_random_num = random_d(10);
			if (_random_num <5)
			{
				//MUTATE
				if (next_gen[k] != NULL)
				{
					__Network_free(next_gen[k]);
				}
				next_gen[k] = __Network_cpy(nets[i]);
				__Network_mutate(next_gen[k]);
				k += 1;
			}
			/*if (_random_num <6 && k < worst_start-1)
			{
				printf("%s%li\n\n\n", "Netcross in ",k);
				if (first == 0)
				{
					first = k;
				}
				//CROSS
				_random_num_t = random_d(100);
				while(_random_num_t != 1 || j == i){
					_random_num_t = random_d(100);
					j += 1;
					if(j>= size){
						j = 0;
					}
				}
				printf("%s%li\n\n\n", "Netcross choose ",j);
				__Network_cross(nets[i], nets[j], next_gen, k, 1);
				//print_Layer(next_gen[k]->layers[0],0,"None");
				printf("%s%li\n\n\n", "Netcross out ",k);
				k+=2;
			}*/
		}
	}
	//printf("%li\n", first);
	//print_Layer(next_gen[first]->layers[0],0,"None");
	__Gen_free(size, nets);
	//printf("%s\n", "after free");
	//print_Layer(next_gen[first]->layers[0],0,"None");
	return next_gen;
}

void quicksort(Network** nets, size_t first, size_t last)
{
	size_t i, j, pivot;
	Network* temp;

	if (first < last)
	{
		pivot = first;
		i = first;
		j = last;

		while (i < j)
		{
			while (nets[i]->note <= nets[pivot]->note && i < last)
				i++;
			while (nets[j]->note > nets[pivot]->note)
				j--;
			if (i < j)
			{
				temp = nets[i];
				nets[i] = nets[j];
				nets[j] = temp;
			}
		}

		temp = nets[pivot];
		nets[pivot] = nets[j];
		nets[j] = temp;
		quicksort(nets, first, j-1);
		quicksort(nets, j+1, last);
	}
}

void XOR_Gen_sort(Network** nets, size_t size)
{
	double min;
	size_t k_min;
	Network* temp_net;
	for (size_t i = 0; i < size-1; ++i)
	{
		min = nets[i]->note;
		k_min = i;
		for (size_t j = i+1; j < size-1; ++j)
		{
			if (nets[j]->note < min)
			{
				min = nets[j]->note;
				k_min = j;
			}
		}
		temp_net = nets[i];
		nets[i] = nets[k_min];
		nets[k_min] = temp_net;
	}
}

Network** XOR_Gen_Resolve(Network** nets, size_t size, size_t gen)
{
	for (size_t i = 0; i < gen; ++i)
	{
		XOR_Gen_evaluate(nets, size);
		//printf("%s\n", "exit evaluate");
		XOR_Gen_sort(nets, size);
		printf("%4li : %lf\n", i, nets[0]->note);
		nets = Gen_evoluate(nets, size);
	}
	XOR_Gen_evaluate(nets, size);
	printf("%4li : %lf\n", gen, nets[0]->note);
	return nets;
}

void choose_move(Network* net, double* moves, size_t c){
	size_t best_p = 0;
	double best = -1;
	for (size_t i = 0; i < c; ++i)
	{
		moves[0+i*8] = moves[0+i*8]/10;
		moves[1+i*8] = moves[1+i*8]/10;
		moves[2+i*8] = moves[2+i*8]/10;
		moves[3+i*8] = moves[3+i*8]/10;
		__Network_propagation(net,(moves+i*8));
		if (net->layers[net->layer_size]->output[0] < best)
		{
			best = net->layers[net->layer_size]->output[0];
			best_p = i;
		}
	}
	c = c-1;
	moves[0+c*8] = moves[0+best_p*8]*10;
  moves[1+c*8] = moves[1+best_p*8]*10;
	moves[2+c*8] = moves[2+best_p*8]*10;
	moves[3+c*8] = moves[3+best_p*8]*10;
}

void __duplicate(int mat1[8][8],int mat2[8][8]){
	for (int i = 0; i < 8; ++i)
		for (int j = 0; j < 8; ++j)
			mat2[i][j] = mat1[i][j];
}

void GAME_sort(Network** nets, size_t size)
{
	double max;
	size_t k_max;
	Network* temp_net;
	for (size_t i = 0; i < size-1; ++i)
	{
		max = nets[i]->note;
		k_max = i;
		for (size_t j = i+1; j < size-1; ++j)
		{
			if (nets[j]->note > max)
			{
				max = nets[j]->note;
				k_max = j;
			}
		}
		temp_net = nets[i];
		nets[i] = nets[k_max];
		nets[k_max] = temp_net;
	}
}

void generate_grid(int matrix[HEIGHT][WIDTH])
{
  srand(time(0));
  for(size_t i = 0; i < HEIGHT; i++)
  {
    for(size_t j = 0; j < WIDTH; j++)
    {
      matrix[i][j] = rand() % 4;
    }
  }
  while(checkgrid(matrix, START, 1) == 1)
    continue; 
}

void GAME_evaluate(Network** nets, size_t c){
	int MAP_def[8][8];
	generate_grid(MAP_def);
	int MAP_tmp[8][8];
	for (size_t i = 0; i < c; ++i)
	{
		__duplicate(MAP_def, MAP_tmp);
		nets[i]->note = (double) trainnn(MAP_tmp,nets[i]);
	}
}

Network** GAME_Gen_Resolve(Network** nets, size_t size, size_t gen)
{
	for (size_t i = 0; i < gen; ++i)
	{
		GAME_evaluate(nets, size);
		//printf("%s\n", "exit evaluate");
		GAME_sort(nets, size);
		printf("%4li : %lf\n", i, nets[0]->note);
		nets = Gen_evoluate(nets, size);
	}
	GAME_evaluate(nets, size);
	printf("%4li : %lf\n", gen, nets[0]->note);
	return nets;
}

void GAME_TRAIN(size_t net_s, size_t gen){
	Network** nets = __Network_def_gen_1(net_s,__layer_count);
	__Network_free(nets[0]);
	nets[0] = Load_Network(__layer_count);
  nets = XOR_Gen_Resolve(nets, net_s, gen);
  Save_Network(nets[0]);
  __Gen_free(net_s,nets);
}

void __TEST_SAVE(){
  Network* net = __Network_def_random(3);
  Save_Network(net);
  __Network_free(net);
}

void __TEST_LOAD(){
  Network* net = Load_Network(__layer_count);
  __print_matrix("weight_1",net->layers[0]->weights,
  	net->layers[0]->input_size,
  	net->layers[0]->output_size);
  __print_matrix("bias_1",net->layers[0]->bias,1,net->layers[0]->output_size);
  __print_matrix("weight_last",net->layers[net->layer_size-2]->weights,
  	net->layers[net->layer_size-2]->input_size,
  	net->layers[net->layer_size-2]->output_size);
  __print_matrix("bias_last",net->layers[net->layer_size-2]->bias,1,
  	net->layers[net->layer_size-2]->output_size);
  __Network_free(net);
}

void __TEST_XOR(){
	printf("%s\n", "Launch test for XOR");
  Network** nets = __Network_def_gen_1(500,2);
  nets = XOR_Gen_Resolve(nets, 500, 1);
  __Gen_free(500,nets);
}

