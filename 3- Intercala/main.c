#include <stdio.h>
#include <stdlib.h>

#define K 8             
#define MAX 1000         

int comparar(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void dividirArquivo(const char *entrada) {
    FILE *arq = fopen(entrada, "r");
    if (!arq) {
        printf("Erro ao abrir arquivo de entrada\n");
        return;
    }

    FILE *blocos[K];
    char nome[20];

    for (int i = 0; i < K; i++) {
        sprintf(nome, "bloco%d.txt", i);
        blocos[i] = fopen(nome, "w");
    }

    int num, i = 0;
    while (fscanf(arq, "%d", &num) != EOF) {
        fprintf(blocos[i % K], "%d\n", num);
        i++;
    }

    fclose(arq);
    for (int i = 0; i < K; i++) fclose(blocos[i]);
}

void ordenarBlocos() {
    char nome[20];
    int vetor[MAX];
    int n;

    for (int i = 0; i < K; i++) {
        sprintf(nome, "bloco%d.txt", i);

        FILE *arq = fopen(nome, "r");
        if (!arq) continue;

        n = 0;
        while (fscanf(arq, "%d", &vetor[n]) != EOF) {
            n++;
        }
        fclose(arq);

        qsort(vetor, n, sizeof(int), comparar);

        arq = fopen(nome, "w");
        for (int j = 0; j < n; j++) {
            fprintf(arq, "%d\n", vetor[j]);
        }
        fclose(arq);
    }
}

void mergeDoisArquivos(const char *arq1, const char *arq2, const char *saida) {
    FILE *f1 = fopen(arq1, "r");
    FILE *f2 = fopen(arq2, "r");
    FILE *out = fopen(saida, "w");

    int a, b;
    int temA = fscanf(f1, "%d", &a);
    int temB = fscanf(f2, "%d", &b);

    while (temA != EOF && temB != EOF) {
        if (a < b) {
            fprintf(out, "%d\n", a);
            temA = fscanf(f1, "%d", &a);
        } else {
            fprintf(out, "%d\n", b);
            temB = fscanf(f2, "%d", &b);
        }
    }

    while (temA != EOF) {
        fprintf(out, "%d\n", a);
        temA = fscanf(f1, "%d", &a);
    }

    while (temB != EOF) {
        fprintf(out, "%d\n", b);
        temB = fscanf(f2, "%d", &b);
    }

    fclose(f1);
    fclose(f2);
    fclose(out);
}

void intercalarBlocos() {

    mergeDoisArquivos("bloco0.txt", "bloco1.txt", "temp1.txt");
    mergeDoisArquivos("bloco2.txt", "bloco3.txt", "temp2.txt");

    mergeDoisArquivos("temp1.txt", "temp2.txt", "ordenado.txt");
}

int main() {
    dividirArquivo("entrada.txt");
    ordenarBlocos();
    intercalarBlocos();

    printf("Arquivo ordenado gerado: ordenado.txt\n");
    return 0;
}