#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO_DADOS  "cep.dat"
#define ARQUIVO_INDICE "cep_index.dat"

typedef struct
{
    char logradouro[72];
    char bairro[72];
    char cidade[72];
    char uf[72];
    char sigla[2];
    char cep[8];
    char lixo[2]; /* espaço final + quebra de linha */
} Endereco;

typedef struct
{
    char  cep[8];    
    long  posicao;
} IndiceItem;


//indice
int comparar_indice(const void *a, const void *b)
{
    const IndiceItem *ia = (const IndiceItem *)a;
    const IndiceItem *ib = (const IndiceItem *)b;
    return strncmp(ia->cep, ib->cep, 8);
}

int construir_indice(void)
{
    FILE     *fDados  = NULL;
    FILE     *fIndice = NULL;
    Endereco  e;
    IndiceItem *indice = NULL;
    long      totalRegistros = 0;
    long      tamBytes;
    long      i;

    fDados = fopen(ARQUIVO_DADOS, "rb");
    if (fDados == NULL)
    {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", ARQUIVO_DADOS);
        return 0;
    }

    fseek(fDados, 0, SEEK_END);
    tamBytes       = ftell(fDados);
    totalRegistros = tamBytes / (long)sizeof(Endereco);
    rewind(fDados);

    printf("Arquivo: %s\n", ARQUIVO_DADOS);
    printf("Tamanho do registro : %lu bytes\n", (unsigned long)sizeof(Endereco));
    printf("Total de registros  : %ld\n\n", totalRegistros);

    if (totalRegistros == 0)
    {
        fprintf(stderr, "Erro: arquivo de dados vazio.\n");
        fclose(fDados);
        return 0;
    }

    indice = (IndiceItem *)malloc(totalRegistros * sizeof(IndiceItem));
    if (indice == NULL)
    {
        fprintf(stderr, "Erro: sem memoria para o indice.\n");
        fclose(fDados);
        return 0;
    }

    printf("Lendo registros e construindo indice...\n");
    for (i = 0; i < totalRegistros; i++)
    {
        if (fread(&e, sizeof(Endereco), 1, fDados) != 1)
        {
            fprintf(stderr, "Aviso: leitura interrompida no registro %ld.\n", i);
            totalRegistros = i;
            break;
        }
        memcpy(indice[i].cep, e.cep, 8);
        indice[i].posicao = i;          /* posição = nº do registro */
    }
    fclose(fDados);

    printf("Ordenando indice (%ld entradas)...\n", totalRegistros);
    qsort(indice, (size_t)totalRegistros, sizeof(IndiceItem), comparar_indice);

    fIndice = fopen(ARQUIVO_INDICE, "wb");
    if (fIndice == NULL)
    {
        fprintf(stderr, "Erro: nao foi possivel criar '%s'\n", ARQUIVO_INDICE);
        free(indice);
        return 0;
    }

    if (fwrite(indice, sizeof(IndiceItem), (size_t)totalRegistros, fIndice)
            != (size_t)totalRegistros)
    {
        fprintf(stderr, "Erro: falha ao escrever indice.\n");
        fclose(fIndice);
        free(indice);
        return 0;
    }

    fclose(fIndice);
    free(indice);

    printf("Indice salvo em '%s' com sucesso!\n", ARQUIVO_INDICE);
    printf("Tamanho da entrada de indice: %lu bytes\n",
    (unsigned long)sizeof(IndiceItem));
    return 1;
}


int buscar_cep(const char *cepBusca)
{
    FILE       *fIndice = NULL;
    FILE       *fDados  = NULL;
    IndiceItem  item;
    Endereco    e;
    long        totalIndice;
    long        tamBytes;
    long        inicio, fim, meio;
    int         cmp;
    int         iteracoes = 0;
    int         encontrado = 0;

    fIndice = fopen(ARQUIVO_INDICE, "rb");
    if (fIndice == NULL)
    {
        fprintf(stderr,
            "Erro: indice '%s' nao encontrado.\n"
            "Execute primeiro: %s --build\n",
            ARQUIVO_INDICE, "cep_index");
        return 0;
    }

    fseek(fIndice, 0, SEEK_END);
    tamBytes    = ftell(fIndice);
    totalIndice = tamBytes / (long)sizeof(IndiceItem);

    inicio = 0;
    fim    = totalIndice - 1;

    while (inicio <= fim)
    {
        meio = (inicio + fim) / 2;
        fseek(fIndice, meio * (long)sizeof(IndiceItem), SEEK_SET);
        fread(&item, sizeof(IndiceItem), 1, fIndice);
        iteracoes++;

        cmp = strncmp(cepBusca, item.cep, 8);

        if (cmp == 0)
        {
            encontrado = 1;
            break;
        }
        else if (cmp < 0)
            fim = meio - 1;
        else
            inicio = meio + 1;
    }

    fclose(fIndice);

    if (!encontrado)
    {
        printf("CEP '%.*s' nao encontrado.\n", 8, cepBusca);
        printf("Iteracoes na busca binaria: %d\n", iteracoes);
        return 0;
    }

    fDados = fopen(ARQUIVO_DADOS, "rb");
    if (fDados == NULL)
    {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", ARQUIVO_DADOS);
        return 0;
    }

    fseek(fDados, item.posicao * (long)sizeof(Endereco), SEEK_SET);

    if (fread(&e, sizeof(Endereco), 1, fDados) != 1)
    {
        fprintf(stderr, "Erro: falha ao ler registro na posicao %ld.\n",
                item.posicao);
        fclose(fDados);
        return 0;
    }
    fclose(fDados);

    printf("=== Endereco encontrado ===\n");
    printf("Logradouro : %.72s\n", e.logradouro);
    printf("Bairro     : %.72s\n", e.bairro);
    printf("Cidade     : %.72s\n", e.cidade);
    printf("UF         : %.72s\n", e.uf);
    printf("Sigla      : %.2s\n",  e.sigla);
    printf("CEP        : %.8s\n",  e.cep);
    printf("===========================\n");
    printf("Posicao no arquivo : %ld\n", item.posicao);
    printf("Iteracoes na busca : %d\n",  iteracoes);

    return 1;
}


static void help(const char *prog)
{
    fprintf(stderr,
        "Uso:\n"
        "  use a flag --build           para Construir o indice a partir de '%s'\n"
        "  use a flag --search <CEP>    para Buscar um CEP (8 digitos, sem traco)\n",
        ARQUIVO_DADOS);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        help(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--build") == 0)
    {
        return construir_indice() ? 0 : 1;
    }
    else if (strcmp(argv[1], "--search") == 0)
    {
        if (argc < 3)
        {
            fprintf(stderr, "Erro: informe o CEP para busca.\n");
            return 1;
        }
        return buscar_cep(argv[2]) ? 0 : 1;
    }
}