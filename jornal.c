#include "../include/jornal.h"
// Puxa o arquivo .h onde está declarada a função jornal_escrever()
#include <stdio.h>
// Biblioteca padrão para lidar com arquivos (fopen, fwrite, fclose, etc.)
#include <stdlib.h>
// Biblioteca geral, usada para coisas como sair do programa (se precisasse)
#include <string.h>
// Usada para funções como snprintf() e funções de string/memória
#include <stdint.h>
// Traz tipos como uint8_t (um byte certinho)

#include <io.h>     /* _commit, _fileno */
// Biblioteca específica do Windows/MinGW
// _commit() = versão Windows do "fsync" (força gravação no disco)
// _fileno() = pega o file descriptor de um FILE*

#include <errno.h>
// Biblioteca que ajuda a lidar com erros (não usada diretamente aqui)

// --------------------------------------------------------------
// Função que grava dados em arquivo de forma SEGURA (journaling)
// --------------------------------------------------------------
// path  = nome do arquivo final (ex: "registro.bin")
// data  = ponteiro para os dados que vamos gravar
// len   = quantidade de bytes que vamos escrever
// Retorno:
//   0  = tudo certo
//  -1  = erro em alguma etapa
int jornal_escrever(const char *path, const uint8_t *data, size_t len) {

    char tmp[1024];
    /* Cria um nome temporário simples baseado no arquivo final, exemplo:
    path = "registro.bin"
    tmp  = "registro.bin.tmp"
    A ideia é escrever tudo no TMP, e só no fim trocar
    pelo arquivo verdadeiro. Isso evita arquivos corrompidos. */
    if (snprintf(tmp, sizeof(tmp), "%s.tmp", path) >= (int)sizeof(tmp)) {
        // snprintf retornando além do tamanho significa erro
        return -1;
    }

    // Abre o arquivo temporário para escrita binária
    FILE *f = fopen(tmp, "wb");
    if (!f) return -1;   // Se não conseguiu abrir → erro

    // Escreve os dados no arquivo .tmp
    size_t written = fwrite(data, 1, len, f);
    if (written != len) {
        // Se escreveu menos do que deveria → deu ruim
        fclose(f);
        remove(tmp);     // Deleta o arquivo temporário
        return -1;
    }

    /* fflush: força o buffer da stdio a realmente escrever no arquivo.
       Sem isso, os dados podem ficar "guardados" na RAM. */
    if (fflush(f) != 0) {
        fclose(f);
        remove(tmp);
        return -1;
    }

    /* _commit força o Windows a gravar no DISCO de verdade,
    não só no buffer do sistema.
    É a versão Win do fsync() do Linux. */
    if (_commit(_fileno(f)) != 0) {
        fclose(f);
        remove(tmp);
        return -1;
    }

    // Fecha o arquivo final
    if (fclose(f) != 0) {
        remove(tmp);
        return -1;
    }

    /* Agora que o arquivo temporário foi totalmente gravado,
    fazemos o rename para o nome oficial.
    No Windows NTFS, rename() é atômico — ou seja, troca 100% de uma vez.
    Isso garante que você nunca ficará com arquivo MEIO gravado. */
    if (rename(tmp, path) != 0) {
        // Se até o rename falhar, ainda tentamos remover o temporário
        remove(tmp);
        return -1;
    }

    return 0;   // Sucesso total :)
}
