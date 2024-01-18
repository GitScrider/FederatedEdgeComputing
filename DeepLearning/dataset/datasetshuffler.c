#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

void readCSV(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL) {
        float vetor[7];
        int count = 0;

        // Utilizando strtok para dividir a linha em tokens
        char *token = strtok(line, ",");
        
        // Pular o primeiro token que contém o número da linha
        token = strtok(NULL, ",");
        
        while (token != NULL && count < 7) {
            vetor[count++] = atof(token);
            token = strtok(NULL, ",");
        }

        // Imprimindo os elementos do vetor
        for (int i = 0; i < 7; i++) {
            printf("%.2f ", vetor[i]);
        }
        printf("\n");
    }

    fclose(file);
}

int main() {
    const char *filename = "dataset.csv";
    readCSV(filename);

    return 0;
}
