#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 1024
#define EXPECTED_TOKENS 14   // 1 classe + 13 features

/* remove espaços no início/fim */
char *trim_inplace(char *s) {
    char *end;
    while (*s && isspace((unsigned char)*s)) s++;
    if (*s == 0) return s;
    end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return s;
}

int main(void) {
    FILE *in = fopen("wine/wine.data", "r");
    FILE *out = fopen("wine/wine.csv", "w");
    if (!in || !out) {
        fprintf(stderr, "Erro ao abrir arquivo(s).\n");
        if (in) fclose(in);
        if (out) fclose(out);
        return 1;
    }

    char line[MAX_LINE_LEN];
    int total = 0;
    int skipped = 0;

    // Cabeçalho
    fprintf(out, "index,");
    for (int i = 1; i <= 13; ++i) {
        fprintf(out, "feature%d,", i);
    }
    fprintf(out, "class1,class2,class3\n");

    while (fgets(line, sizeof(line), in)) {
        // Remove quebra de linha
        line[strcspn(line, "\r\n")] = '\0';

        // Ignora linhas vazias
        char *p = line;
        while (*p && isspace((unsigned char)*p)) p++;
        if (*p == '\0') continue;

        // Tokeniza por vírgula
        char *tokens[20];
        int nt = 0;
        char *tok = strtok(line, ",");
        while (tok && nt < 20) {
            tokens[nt++] = trim_inplace(tok);
            tok = strtok(NULL, ",");
        }

        // Verifica se tem número esperado de colunas
        if (nt < EXPECTED_TOKENS) {
            fprintf(stderr, "Linha ignorada (colunas=%d < %d): \"%s\"\n", nt, EXPECTED_TOKENS, p);
            skipped++;
            continue;
        }

        // Classe
        int classe = atoi(tokens[0]);
        if (classe < 1 || classe > 3) {
            fprintf(stderr, "Linha ignorada (classe inválida=%d): \"%s\"\n", classe, p);
            skipped++;
            continue;
        }

        // Índice começa em 1
        fprintf(out, "%d,", total + 1);

        // Features (tokens[1..13])
        fprintf(out, "%s", tokens[1]);
        for (int i = 2; i <= 13; ++i) {
            fprintf(out, ",%s", tokens[i]);
        }

        // One-hot
        if (classe == 1)      fprintf(out, ",1,0,0\n");
        else if (classe == 2) fprintf(out, ",0,1,0\n");
        else                  fprintf(out, ",0,0,1\n");

        total++;
    }

    fclose(in);
    fclose(out);

    printf("Concluído: %d linhas processadas, %d ignoradas.\n", total, skipped);
    printf("Arquivo gerado: wine.csv\n");
    return 0;
}
