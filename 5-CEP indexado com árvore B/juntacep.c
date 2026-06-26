#include "ArvoreB.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct _Endereco Endereco;

struct _Endereco
{
    char logradouro[72];
    char bairro[72];
    char cidade[72];
    char uf[72];
    char sigla[2];
    char cep[8];
    char lixo[2];
};

int main() {

    FILE *a1 = fopen("cep80.dat", "rb");
    FILE *saida = fopen("intersecao.dat", "wb");
    ArvoreB *indice = ArvoreB_Abre("cep60.idx");

    if (a1 == NULL || saida == NULL || indice == NULL) {
        printf("Erro ao abrir os arquivos.\n");

        if (a1) fclose(a1);
        if (saida) fclose(saida);
        if (indice) ArvoreB_Fecha(indice);

        return 1;
    }

    Endereco endereco;
    long posicao;

    while (fread(&endereco, sizeof(Endereco), 1, a1) == 1) {

        posicao = ArvoreB_Busca(indice, endereco.cep);

        if (posicao != -1) {
            fwrite(&endereco, sizeof(Endereco), 1, saida);
        }
    }

    fclose(a1);
    fclose(saida);
    ArvoreB_Fecha(indice);

    printf("Juncao finalizada com sucesso!\n");

    return 0;
}