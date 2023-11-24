#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 1024

// Função para embaralhar um array (excluindo a primeira linha)
void shuffleArray(char* lines[], int size) {
    for (int i = size - 1; i > 1; i--) { // Começar do índice 2 para manter a primeira linha e a primeira coluna fixas
        int j = rand() % (i - 1) + 2;
        if (i != j) {
            char* temp = lines[i];
            lines[i] = lines[j];
            lines[j] = temp;
        }
    }
}

int main() {
    // Abrir o arquivo CSV para leitura
    FILE* file = fopen("iris.csv", "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo CSV.\n");
        return 1;
    }

    // Ler as linhas do arquivo e armazená-las em um array
    char* lines[1000];  // Suponha um limite máximo de 1000 linhas
    int lineCount = 0;

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        lines[lineCount] = strdup(line);  // Copiar a linha para o array
        lineCount++;
    }

    fclose(file);

    // Embaralhar as linhas (mantendo a primeira linha e a primeira coluna)
    srand((unsigned)time(NULL));
    shuffleArray(lines, lineCount);

    // Escrever as linhas embaralhadas de volta para o arquivo
    file = fopen("dataset.csv", "w");
    if (file == NULL) {
        printf("Erro ao criar o arquivo CSV embaralhado.\n");
        return 1;
    }

    for (int i = 0; i < lineCount; i++) {
        fputs(lines[i], file);
        free(lines[i]);  // Liberar a memória alocada
    }

    fclose(file);

    printf("Embaralhamento concluído e salvo como 'dataset.csv'.\n");

    return 0;
}