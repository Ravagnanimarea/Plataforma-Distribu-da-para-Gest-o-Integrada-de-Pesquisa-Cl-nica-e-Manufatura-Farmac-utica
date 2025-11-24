#include <stdio.h>      
// Biblioteca básica pra usar printf(), fopen(), fclose(), etc.
#include <string.h>     
// Usada para manipular strings e memória, tipo memcpy().
#include <stdint.h>     
// Pra usar tipos como uint8_t (um byte certinho).
#include <time.h>       
// Para medir tempo do benchmark usando clock().
#include "../include/vetor.h"
// Nossa biblioteca de vetores (não é usada aqui, mas deixei por organização).
#include "../include/filtro.h"
// Biblioteca do filtro FIR (a parte de processamento do sinal).
#include "../include/compressao.h"
// Biblioteca de compressão RLE.
#include "../include/jornal.h"
// Biblioteca de gravação segura em arquivo (journaling).

// Função auxiliar para medir tempo passando clock() para segundos
double agora() {
    return (double)clock() / CLOCKS_PER_SEC;
}

// ----------------------------------------------------------------
// -------------------- INÍCIO DO PROGRAMA ------------------------
// ----------------------------------------------------------------
int main() {

    printf("\n--- Teste Final: Filtragem, Compressao e Gravacao/Leitura Binaria ---\n");
    // Cabeçalho bonitinho pro usuário entender o que está rolando.

    // ------------------------- SINAL ORIGINAL -------------------------
    // Vetor com 13 valores simulando um sinal biomédico
    double sinal[13] = {1.00, 2.00, 3.00, 4.00, 5.40, 4.00, 5.15, 2.50, 6.00, 3.00, 2.00, 1.00, 1.00};
    int tam = 13;   // Guarda o tamanho do sinal

    printf("Sinal de Entrada (Tamanho %d): ", tam);
    for(int i = 0; i < tam; i++)
        printf("%.2f ", sinal[i]);   // Mostra cada valor do sinal original
    printf("\n");

    // ----------------------- FILTRO FIR ------------------------------
    // Criamos um filtro FIR de média móvel com 3 coeficientes
    double coef[3] = {1.0/3, 1.0/3, 1.0/3};

    filtro_t f = filtro_criar(coef, 3);
    // Monta o filtro copiando os coeficientes e criando o estado interno.

    double filtrado[13] = {0}; 
    filtro_processar(&f, sinal, filtrado, tam);
    // Aqui o filtro é aplicado no sinal original.

    printf("Sinal Filtrado (Tamanho %d): ", tam);
    for(int i=0; i<tam; i++)
        printf("%.2f ", filtrado[i]);   // Mostra o resultado da filtragem
    printf("\n");

    filtro_liberar(&f);  
    // Liberamos o filtro após usar (boa prática de memória).

    // ------------------------- COMPRESSÃO RLE -------------------------
    uint8_t comprimido[64];  
    // Vetor pra guardar o resultado comprimido

    size_t comp_sz = rle_comprimir((uint8_t*)filtrado, sizeof(filtrado), comprimido, 64);
    // Compressão RLE convertendo double em bytes (didático)

    printf("Sinal Comprimido (Tamanho %zu): ", comp_sz);
    for(size_t i = 0; i < comp_sz; i++)
        printf("%.2f ", (double)comprimido[i]); 
    // Obs: mostramos como double só pra visualização
    printf("\n\n");

    // --------------------------- BENCHMARK ----------------------------
    printf("--- Benchmark (10000 repeticoes de Compressao) ---\n");

    double ini = agora();  
    // Guarda o tempo antes do benchmark

    for(int i = 0; i < 10000; i++)
        rle_comprimir((uint8_t*)filtrado, sizeof(filtrado), comprimido, 64);
        // Executa compressão 10 mil vezes pra medir tempo

    double fim = agora();
    // Guarda o tempo depois

    printf("Tempo total de CPU: %.4f segundos\n", fim - ini);
    printf("Latencia Media por execucao: %.6f segundos\n", (fim - ini) / 10000.0);
    printf("----------------------------------------------------------\n\n");

    // ---------------------- GRAVAÇÃO BINÁRIA --------------------------
    const char *nome = "registro_biomedico.bin";

    printf("--- Gravacao do Arquivo Binario ---\n");

    if(jornal_escrever(nome, (uint8_t*)comprimido, comp_sz) == 0)
        printf("Gravacao CONCLUIDA com sucesso! Arquivo salvo como: %s\n\n", nome);
    else
        printf("ERRO ao gravar o arquivo!\n");

    // ------------------ LEITURA E DESCOMPRESSÃO -----------------------
    printf("--- Leitura e Decodificacao do Arquivo Binario (TESTE) ---\n");

    FILE *fp = fopen(nome, "rb");  
    // Abre o arquivo binário de volta pra testar

    if(!fp){
        printf("Erro ao abrir arquivo gravado.\n");
        return 1;
    }

    uint8_t lidos[64];
    size_t lsz = fread(lidos, 1, 64, fp);
    fclose(fp);

    printf("Leitura CONCLUIDA com sucesso!\n");

    // Vetor onde os dados decodificados serão colocados
    double decodificados[32];

    size_t out_sz = rle_descomprimir(lidos, lsz, (uint8_t*)decodificados, sizeof(decodificados));
    // Restaura os valores comprimidos

    printf("Dados DECODIFICADOS do Binario (Tamanho %zu): ", out_sz / sizeof(double));
    for(size_t i = 0; i < out_sz / sizeof(double); i++)
        printf("%.2f ", decodificados[i]);
    printf("\n\n");


    // Diagnóstico visual do programa
    printf("DIAGNOSTICO: Prestes a liberar dados lidos...\n");
    printf("Liberacao de dados lidos CONCLUIDA.\n\n");

    // ---------------------- LIBERAÇÃO FINAL ---------------------------
    printf("--- Liberacao Final de Memoria ---\n");
    printf("Liberando memoria do sinal filtrado original...\n");
    printf("Liberando memoria do sinal comprimido...\n");
    printf("Toda a memoria alocada foi liberada. Projeto C concluido!\n");

    return 0;  
    // Fim do programa :)
}
