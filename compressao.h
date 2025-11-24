#ifndef COMPRESSAO_H // Se esse nome ainda não foi definido, entra aqui (proteção contra incluir 2x)
#define COMPRESSAO_H // Define o nome pra evitar que esse arquivo seja carregado de novo sem querer

#include <stddef.h> // Biblioteca que traz o tipo size_t (tamanho seguro)
#include <stdint.h> // Biblioteca dos tipos de tamanho exato, como uint8_t (1 byte certinho)

// Função pra COMPRIMIR usando RLE (Run-Length Encoding)
// in -> ponteiro para os dados originais (o sinal ou vetor de bytes)
// in_len -> tamanho desses dados originais
// out -> onde vamos guardar os dados comprimidos
// cap -> capacidade máxima do buffer de saída (pra não estourar memória)
// Retorno -> quantos bytes foram gerados na compressão
size_t rle_comprimir(const uint8_t *in, size_t in_len, uint8_t *out, size_t cap);

// Função pra DESCOMPRIMIR os dados comprimidos por RLE
// in -> o vetor já comprimido
// in_len -> tamanho desse vetor comprimido
// out -> buffer onde os dados vão ser colocados já descomprimidos
// cap -> capacidade máxima da saída
// Retorno -> quantos bytes foram escritos descomprimidos
size_t rle_descomprimir(const uint8_t *in, size_t in_len, uint8_t *out, size_t cap);

#endif // Fim da proteção contra inclusão múltipla
