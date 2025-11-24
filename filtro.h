#ifndef FILTRO_H // Se esse nome ainda não foi definido, entra aqui (proteção contra incluir 2x)
#define FILTRO_H // Define o identificador pra evitar inclusão repetida

#include <stddef.h> // Biblioteca que traz o tipo size_t (muito usado para tamanhos)


// Aqui criamos uma estrutura (struct) que representa um filtro FIR simples
typedef struct {
    size_t taps; // Quantos coeficientes o filtro tem (exemplo: 3 no caso de média móvel)
    double *coef; // Ponteiro para os coeficientes do filtro (h[0], h[1], h[2]...)
    double *estado; // Memória interna do filtro (últimas amostras pra manter continuidade)
} filtro_t;


// Função que CRIA o filtro
// coef -> lista de coeficientes
// taps -> quantos coeficientes existem
// Retorna uma struct filtro_t toda montada e pronta pra usar
filtro_t filtro_criar(const double *coef, size_t taps);


// Função que LIBERA tudo que foi alocado no filtro
// f -> ponteiro para o filtro que queremos liberar
void filtro_liberar(filtro_t *f);


// Função que PROCESSA (filtra) um vetor de entrada
// f -> o filtro criado anteriormente
// in -> dados de entrada (exemplo: seu sinal biomédico)
// out -> onde o resultado do filtro será colocado
// n -> quantidade de amostras
void filtro_processar(filtro_t *f, const double *in, double *out, size_t n);


#endif // Fecha a proteção contra múltipla inclusão
