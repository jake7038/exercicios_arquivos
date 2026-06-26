#include "ArvoreB.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


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


int main(){
    ArvoreB *arvore = ArvoreB_Abre("cep80.idx");

FILE *f = fopen("cep80.dat", "rb");

if (f == NULL) {
    printf("Erro ao abrir cep80.dat\n");
    return 1;
}

Endereco endereco;
long posicao = 0;

while (fread(&endereco, sizeof(Endereco), 1, f) == 1) {
    ArvoreB_Insere(arvore, endereco.cep, posicao);
    posicao++;
}

fclose(f);

ArvoreB_Fecha(arvore);
return 0;
}

