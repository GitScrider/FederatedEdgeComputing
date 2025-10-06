#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1000
#define MAX_COLS 100
#define MAX_LINE_LEN 1024

// Ajuste para o número de colunas de one-hot no seu CSV
#define NUM_CLASSES 3  

int main() {
    FILE *in = fopen("wine/datasetevaluation.csv", "r");
    FILE *out = fopen("wine/datasetevaluationnormalized.csv", "w");
    if (!in || !out) {
        printf("Erro ao abrir arquivo.\n");
        return 1;
    }

    char line[MAX_LINE_LEN];
    float data[MAX_LINES][MAX_COLS];
    int n_lines = 0;
    int n_cols = 0;

    // Leitura do CSV
    while (fgets(line, sizeof(line), in)) {
        line[strcspn(line, "\r\n")] = 0; // remove newline
        char *token = strtok(line, ",");
        int col = 0;
        while (token != NULL) {
            data[n_lines][col++] = atof(token);
            token = strtok(NULL, ",");
        }
        if (n_lines == 0) n_cols = col; // define número de colunas
        n_lines++;
        if (n_lines >= MAX_LINES) break;
    }

    // Normalização min-max entre 0 e 1 (pula primeira coluna e one-hot)
    for (int col = 1; col < n_cols - NUM_CLASSES; col++) {
        float min = data[0][col];
        float max = data[0][col];
        for (int row = 0; row < n_lines; row++) {
            if (data[row][col] < min) min = data[row][col];
            if (data[row][col] > max) max = data[row][col];
        }
        float range = max - min;
        if (range == 0) range = 1; // evita divisão por zero
        for (int row = 0; row < n_lines; row++) {
            data[row][col] = (data[row][col] - min) / range;
        }
    }

    // Escrita do CSV normalizado
    for (int row = 0; row < n_lines; row++) {
        fprintf(out, "%.0f", data[row][0]); // primeira coluna (index) sem normalização
        for (int col = 1; col < n_cols; col++) {
            fprintf(out, ",%.6f", data[row][col]);
        }
        fprintf(out, "\n");
    }

    fclose(in);
    fclose(out);

    printf("Arquivo datasetnormalized.csv gerado com %d linhas e %d colunas.\n", n_lines, n_cols);
    return 0;
}
