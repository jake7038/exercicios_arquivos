#include <stdio.h>
#include <stdlib.h>
#include "CSVParser.h"
#include <string.h>  

#define READ_BUF_SIZE 8192

void callback(char** cols, int ncols, void* userData) {
    int* count = (int*) userData;
    if(strcmp(cols[1], "South America") == 0) {
        count[0] += atoi(cols[5]);
        count[1] += atoi(cols[8]);
    }
}
int main() {    
    int qt;
    int count[2] = {0, 0};
    unsigned int total = 0;
    char* buf = (char*) malloc(READ_BUF_SIZE);
    CSVParser csv;
    CSVParser_init(&csv);
    FILE *f = fopen("./owid-covid-data.csv","rb");
    if(f) {
        qt = fread(buf,1,READ_BUF_SIZE,f);
        while(qt > 0) {
            total += qt;
            CSVParser_processLines(&csv,buf,qt,callback,&count);
            qt = fread(buf,1,READ_BUF_SIZE,f);
        }
        fclose(f);
        CSVParser_processLines(&csv,"\n",1,callback,&count);
    }
    printf("Total de casos: %d\n", count[0]);
    printf("Total de mortes: %d\n", count[1]);
    free(buf);
    return 0;
}