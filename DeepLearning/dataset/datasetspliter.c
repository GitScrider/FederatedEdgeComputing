#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINES 1000     // máximo de linhas esperadas no CSV
#define MAX_LINE_LEN 512   // tamanho máximo de cada linha

// Função para embaralhar as linhas
void shuffle_lines(char *lines[], int count) {
    srand((unsigned int)time(NULL));
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char *temp = lines[i];
        lines[i] = lines[j];
        lines[j] = temp;
    }
}

void split_csv_dataset(const char *input_file,
                       const char *train_file,
                       const char *eval_file,
                       double eval_ratio) {
    FILE *in = fopen(input_file, "r");
    if (!in) {
        printf("❌ Erro ao abrir %s\n", input_file);
        return;
    }

    char *lines[MAX_LINES];
    int count = 0;
    char buffer[MAX_LINE_LEN];

    // Lê todas as linhas do arquivo CSV
    while (fgets(buffer, sizeof(buffer), in)) {
        buffer[strcspn(buffer, "\r\n")] = 0; // remove \n
        if (strlen(buffer) == 0)
            continue;
        lines[count] = strdup(buffer);
        count++;
        if (count >= MAX_LINES) break;
    }
    fclose(in);

    if (count == 0) {
        printf("⚠️ Nenhuma linha encontrada.\n");
        return;
    }

    // Embaralha as linhas
    shuffle_lines(lines, count);

    // Calcula tamanhos
    int eval_size = (int)(count * eval_ratio);
    int train_size = count - eval_size;

    FILE *train = fopen(train_file, "w");
    FILE *eval = fopen(eval_file, "w");

    if (!train || !eval) {
        printf("❌ Erro ao criar arquivos de saída.\n");
        if (train) fclose(train);
        if (eval) fclose(eval);
        return;
    }

    // Escreve as linhas embaralhadas
    for (int i = 0; i < count; i++) {
        FILE *target = (i < train_size) ? train : eval;
        fprintf(target, "%s\n", lines[i]);
        free(lines[i]);
    }

    fclose(train);
    fclose(eval);

    printf("✅ Dataset dividido com sucesso!\n");
    printf(" - %s: %d linhas (treino)\n", train_file, train_size);
    printf(" - %s: %d linhas (avaliação)\n", eval_file, eval_size);
}

int main() {
    split_csv_dataset("wine/wine.csv",
                      "wine/datasettraining.csv",
                      "wine/datasetevaluation.csv",
                      0.20); // 20% para avaliação
    return 0;
}
