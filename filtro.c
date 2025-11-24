#include "filtro.h"     
// Inclui o cabeçalho onde estão definidas a struct do filtro e as funções públicas
#include <stdlib.h>     
// Biblioteca que traz malloc, calloc, free (alocação dinâmica)
#include <string.h>     
// Biblioteca que traz memcpy, memmove (cópia de memória)
#include <stdio.h>      
// Usada aqui só pro perror(), que mostra mensagens de erro no terminal

// --------------------------------------------------------------
// Função que cria um filtro FIR (média móvel, suavização, etc.)
// --------------------------------------------------------------
filtro_t filtro_criar(const double *coef, size_t taps){
    filtro_t f;               
    // "f" vai ser nosso filtro final

    f.taps = taps;           
    // Guarda quantos coeficientes o filtro tem

    f.coef = malloc(taps * sizeof(double));   
    // Aloca memória pros coeficientes do filtro

    if(!f.coef){             
        perror("malloc coef");    // Se falhar, mostra erro
        exit(1);                  // Fecha o programa (não tem como continuar sem coeficientes)
    }

    memcpy(f.coef, coef, taps * sizeof(double));  
    // Copia os coeficientes enviados pelo usuário para dentro do filtro

    size_t st = (taps > 1) ? (taps - 1) : 1;  
    // Quantidade de memória para o “estado”, que são as últimas amostras
    // Ex: filtro de 3 → precisa de 2 amostras anteriores

    f.estado = calloc(st, sizeof(double));  
    // Aloca o estado e já inicializa com zero (por isso calloc)

    if(!f.estado){
        perror("calloc estado");  // Se der erro
        exit(1);                  // Para tudo
    }

    return f;   // Retorna o filtro montadinho
}

// --------------------------------------------------------------
// Função que libera toda a memória usada pelo filtro
// --------------------------------------------------------------
void filtro_liberar(filtro_t *f){
    if(!f) return;       // Se passar ponteiro nulo, já sai

    free(f->coef);       // Libera coeficientes
    free(f->estado);     // Libera memória do estado

    // Zera ponteiros pra evitar lixo
    f->coef = NULL;
    f->estado = NULL;
    f->taps = 0;
}

// --------------------------------------------------------------
// Função que realmente faz a filtragem FIR
// - in  = entrada (sinal original)
// - out = saída (sinal filtrado)
// --------------------------------------------------------------
void filtro_processar(filtro_t *f, const double *in, double *out, size_t n){

    size_t T = f->taps;                       
    // Quantos coeficientes o filtro tem

    size_t st = (T > 1) ? (T - 1) : 1;        
    // Tamanho do estado (quantas amostras anteriores precisamos guardar)

    
    // Para cada amostra da entrada…
    for(size_t i = 0; i < n; i++){
        double acc = 0;                      
        // Acumulador da soma do filtro (resultado final da convolução ponto-a-ponto)

        
        // Loop pelos coeficientes do filtro (convolução)
        for(size_t t = 0; t < T; t++){
            long idx = (long)i - (long)t;    
            // Índice da amostra correspondente na entrada

            double x;

            if(idx >= 0)
                x = in[idx];                 
                // Se o índice for válido → usa valor da entrada
            else
                x = f->estado[st + idx];     
                // Senão → pega valor da memória “estado” (amostras anteriores)

            acc += f->coef[t] * x;           
            // Soma coeficiente * valor da entrada/estado
        }

        out[i] = acc;                        
        // Envia o resultado final pro vetor de saída
    }


    // ----------------------------------------------------------
    // Atualiza o estado para a próxima chamada da função
    // (importante para sinais grandes / processamento contínuo)
    // ----------------------------------------------------------
    if(T > 1){

        if(n >= st){
            // Se o sinal tem mais amostras que o tamanho do estado:
            memcpy(f->estado, in + (n - st), st * sizeof(double));
            // Copia as últimas amostras da entrada pro estado
        }
        else{
            // Se a entrada é menor que o estado:
            size_t shift = st - n;

            memmove(f->estado, f->estado + n, shift * sizeof(double));
            // Puxa parte do estado pra frente

            memcpy(f->estado + shift, in, n * sizeof(double));
            // Completa o resto com dados da entrada
        }
    }
}
