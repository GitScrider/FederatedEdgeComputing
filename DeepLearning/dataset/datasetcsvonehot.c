#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256
#define NUM_FEATURES 10   // ID + 9 atributos (sem TYPE)
#define NUM_CLASSES 7     // Tipos 1 a 6

void convert_glass_data_to_csv(const char *input_file, const char *output_file) {
    FILE *in = fopen(input_file, "r");
    FILE *out = fopen(output_file, "w");

    if (!in || !out) {
        printf("❌ Erro ao abrir arquivo.\n");
        if (in) fclose(in);
        if (out) fclose(out);
        return;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), in)) {
        // Remove o \n no final da linha
        line[strcspn(line, "\r\n")] = 0;

        // Ignora linhas vazias
        if (strlen(line) == 0)
            continue;

        // Quebra a linha pelos delimitadores ","
        char *token = strtok(line, ",");
        double values[NUM_FEATURES + 1]; // +1 para TYPE
        int count = 0;

        while (token && count < NUM_FEATURES + 1) {
            values[count++] = atof(token);
            token = strtok(NULL, ",");
        }

        if (count < NUM_FEATURES + 1)
            continue; // pula linha incompleta

        int id = (int)values[0];
        int type = (int)values[NUM_FEATURES]; // última coluna é TYPE

        // Escreve as features (sem o campo TYPE original)
        fprintf(out, "%d", id);
        for (int i = 1; i < NUM_FEATURES; i++)
            fprintf(out, ",%.5f", values[i]);

        // Faz o one-hot encoding das classes (1 a 6)
        for (int c = 1; c <= NUM_CLASSES; c++)
            fprintf(out, ",%d", (c == type) ? 1 : 0);

        fprintf(out, "\n");
    }

    fclose(in);
    fclose(out);
    printf("✅ Conversão concluída!\n");
    printf("Arquivo de saída: %s\n", output_file);
}



int main() {
    convert_glass_data_to_csv("glass/glass.data", "glass/glass.csv");
    return 0;
}
