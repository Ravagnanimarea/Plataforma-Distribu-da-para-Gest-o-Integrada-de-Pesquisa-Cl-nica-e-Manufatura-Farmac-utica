#ifndef VETOR_H // Proteção pra evitar incluir esse arquivo mais de uma vez no mesmo projeto
#define VETOR_H // Marca que o arquivo já foi carregado

#include <stddef.h> // Biblioteca que traz o tipo size_t (usado para tamanhos e índices seguros)


// Aqui definimos uma "struct", que é tipo um mini-objeto que guarda vários dados juntos
typedef struct {
    size_t tamanho; // Quantos elementos o vetor tem (ex: 5, 10, 100...)
    double *dados; // Ponteiro para onde os valores do vetor realmente ficam guardados na memória
} vetor_t; // Nome do tipo que estamos criando: "vetor_t"


// Função que cria um vetor novo na memória
// tamanho -> quantos elementos queremos
// Retorna um vetor_t já criado e com memória alocada
vetor_t vetor_criar(size_t tamanho);


// Função que libera um vetor criado anteriormente
// Muito importante pra evitar vazamento de memória
void vetor_liberar(vetor_t *v);


// Função que calcula o produto escalar entre dois vetores
// a -> primeiro vetor
// b -> segundo vetor
// Retorna um número (double) que é o resultado da multiplicação elemento a elemento somada
double vetor_produto(const vetor_t *a, const vetor_t *b);


#endif // Fecha a proteção contra inclusão múltipla

