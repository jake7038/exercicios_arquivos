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
    srand(time(NULL));
    int saida = 0;
    FILE *f = fopen("cep.dat", "rb");
    FILE *g = fopen("cep80.dat", "w");
    Endereco endereco;
    fseek(f, 0, SEEK_END);
    long tamanhoByte = ftell(f);
    rewind(f);
    long tamanhoregistro = tamanhoByte / sizeof(Endereco);

    for(int i =0; i < tamanhoregistro; i++){
        float random_num = 1.0 * rand() / (RAND_MAX+1);
        fread(&endereco, sizeof(Endereco), 1, f);  
        if(random_num < 0.8){
            fwrite(&endereco, sizeof(Endereco), 1, g);
            saida++;
        }
    }
    printf("entradas totais: %d\n", tamanhoregistro);
    printf("numero de arquivos escolhidos: %d\n", saida);
    return 0;
}