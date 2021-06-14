#include "Layer.h"
#include "Internal_fun.h"
#include <stdio.h>
#include <stdlib.h>

void print_Layer(Layer* layer, int num, char* name){
  printf("Layer %i %s is %c with %li input and %li output\n",
    num, name,layer->type, layer->input_size, layer->output_size);
  __print_matrix("Input",   layer->input, 1, layer->input_size);
  __print_matrix("Output",  layer->output, 1, layer->output_size);
  if (layer->type == 'F')
  {
    __print_matrix("Weights", layer->weights, layer->input_size, layer->output_size);
    __print_matrix("Bias",    layer->bias, 1, layer->output_size);
  }
}

Layer* __Layer_def_FC(size_t input_size, size_t output_size)
{
  //def base
  Layer* layer = (Layer *) malloc(sizeof(Layer));
  layer->type = 'F';
  layer->input_size  = input_size ;
  layer->output_size = output_size;
  //reserve mat
  layer->input   = (double *) malloc(input_size * sizeof(double));
  layer->output  = (double *) malloc(output_size * sizeof(double));
  layer->weights = (double *) malloc(input_size * output_size *
				     sizeof(double));
  layer->bias    = (double *) malloc(output_size * sizeof(double));
  //def mat
  rand_matrice(input_size, 1, layer->input);
  rand_matrice(output_size, 1, layer->output);
  rand_matrice(input_size, output_size, layer->weights);
  rand_matrice(output_size, 1, layer->bias);
  return layer;
}

Layer* __Layer_def_A(size_t size)
{
  Layer* layer = (Layer *) malloc(sizeof(Layer));
  layer->type = 'A';
  layer->input_size  = size ;
  layer->output_size = size;
  //reserve mat
  layer->input  = (double *) malloc(size * sizeof(double));
  layer->output = (double *) malloc(size * sizeof(double));
  //def mat
  rand_matrice(size, 1, layer->input);
  rand_matrice(size, 1, layer->output);
  return layer;
}

Layer* __Layer_cpy(Layer* layer)
{
  Layer* child;
  if (layer->type == 'F')
  {
    child = __Layer_def_FC(layer->input_size,layer->output_size);
    cpy_mat(layer->weights, child->weights,
      child->input_size*child->output_size);
    cpy_mat(layer->bias, child->bias, child->output_size);
  }
  else
  {
    child = __Layer_def_A(layer->input_size);
  }
  return child;
}

void forward_propagation(Layer *layer)
{
    if(layer->type == 'F')
    {
        mul(layer->input, layer->weights, 1, layer->input_size, layer->output_size,layer->output);
        add(layer->output, layer->bias, 1, layer->output_size,
        layer->output);
    }
    else
    {
        for(size_t w = 0; w < layer->input_size; w++)
        {
            layer->output[w] = sigmoid(layer->input[w]);
        }
    }
}

void __free_layer(Layer *layer)
{
  free(layer->input);
  free(layer->output);
  if (layer->type == 'F')
  {
    free(layer->weights);
    free(layer->bias);
  }
  free(layer);
}

void __Layer_mutate(Layer *layer){
  if (layer->type == 'F')
  {
    mutate(layer->weights, layer->input_size*layer->output_size);
    mutate(layer->bias, layer->output_size);
  }
}

//cros function def
//void cross(double *a, double *b, double *c, double *d, size_t a_size, size_t b_size, size_t c_size, int num)
Layer** __Layer_cross(Layer *layer1, Layer *layer2, int num){
  Layer** childs = (Layer**) calloc(2,sizeof(Layer*));
  size_t input_size_mid  = (layer1->input_size+layer2->input_size)/2;
  size_t output_size_mid = (layer1->output_size+layer2->output_size)/2;
  childs[0] = __Layer_def_FC(input_size_mid, output_size_mid);
  childs[1] = __Layer_def_FC(input_size_mid, output_size_mid);
  if (layer1->input_size*layer1->output_size >= layer2->input_size*layer2->output_size)
  {
    cross(layer1->weights, layer2->weights, childs[0]->weights, childs[1]->weights,
      layer1->input_size*layer1->output_size,
      layer2->input_size*layer2->output_size,
      input_size_mid*output_size_mid, num);
  }
  else
  {
    cross(layer2->weights, layer1->weights, childs[0]->weights, childs[1]->weights,
      layer2->input_size*layer2->output_size,
      layer1->input_size*layer1->output_size,
      input_size_mid*output_size_mid, num);
  }
  if (layer1->output_size >= layer2->output_size)
  {
    cross(layer1->bias, layer2->bias, childs[0]->bias, childs[1]->bias,
      layer1->output_size, layer2->output_size,
      output_size_mid, num);
  }
  else
  {
    cross(layer2->bias, layer1->bias, childs[0]->bias, childs[1]->bias,
      layer2->output_size, layer1->output_size,
      output_size_mid, num);
  }
  return childs;
}
