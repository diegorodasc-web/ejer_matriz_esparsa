#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define M 100
#define N 100
#define P 100

typedef struct {
    int fila;
    int col;
    double valor;
} ElementoEsparso;

int comparar(const void *a, const void *b) {
    ElementoEsparso *x = (ElementoEsparso *)a;
    ElementoEsparso *y = (ElementoEsparso *)b;
    if (x->fila != y->fila) return x->fila - y->fila;
    return x->col - y->col;
}

int main() {
    srand(time(NULL));

    int cant_a = (int)(M * N * 0.05);
    FILE *f = fopen("matriz_a.txt", "w");
    fprintf(f, "%d %d %d\n", M, N, cant_a);
    for (int k = 0; k < cant_a; k++) {
        fprintf(f, "%d %d %.2f\n", rand() % M, rand() % N, (rand() % 10) + 1.0);
    }
    fclose(f);

    int cant_b = (int)(N * P * 0.05);
    f = fopen("matriz_b.txt", "w");
    fprintf(f, "%d %d %d\n", N, P, cant_b);
    for (int k = 0; k < cant_b; k++) {
        fprintf(f, "%d %d %.2f\n", rand() % N, rand() % P, (rand() % 10) + 1.0);
    }
    fclose(f);

    FILE *txt = fopen("matriz_a.txt", "r");
    FILE *bin = fopen("matriz_a.bin", "wb");
    int f_dim, c_dim, total;
    fscanf(txt, "%d %d %d", &f_dim, &c_dim, &total);
    ElementoEsparso e;
    while (fscanf(txt, "%d %d %lf", &e.fila, &e.col, &e.valor) == 3) {
        fwrite(&e, sizeof(ElementoEsparso), 1, bin);
    }
    fclose(txt); fclose(bin);

    txt = fopen("matriz_b.txt", "r");
    bin = fopen("matriz_b.bin", "wb");
    fscanf(txt, "%d %d %d", &f_dim, &c_dim, &total);
    while (fscanf(txt, "%d %d %lf", &e.fila, &e.col, &e.valor) == 3) {
        fwrite(&e, sizeof(ElementoEsparso), 1, bin);
    }
    fclose(txt); fclose(bin);

    FILE *fa = fopen("matriz_a.bin", "rb");
    FILE *fb = fopen("matriz_b.bin", "rb");
    FILE *facum = fopen("acumulado.bin", "wb+");
    ElementoEsparso ea, eb, er;

    while (fread(&ea, sizeof(ElementoEsparso), 1, fa) == 1) {
        rewind(fb);
        while (fread(&eb, sizeof(ElementoEsparso), 1, fb) == 1) {
            if (ea.col == eb.fila) {
                er.fila = ea.fila;
                er.col = eb.col;
                er.valor = ea.valor * eb.valor;
                fwrite(&er, sizeof(ElementoEsparso), 1, facum);
            }
        }
    }
    fclose(fa); fclose(fb);

    fseek(facum, 0, SEEK_END);
    long bytes = ftell(facum);
    int num_parciales = bytes / sizeof(ElementoEsparso);
    
    rewind(facum);
    ElementoEsparso *lista = malloc(bytes);
    fread(lista, sizeof(ElementoEsparso), num_parciales, facum);
    fclose(facum);

    qsort(lista, num_parciales, sizeof(ElementoEsparso), comparar);

    FILE *fres = fopen("resultado.bin", "wb");
    int i = 0;
    while (i < num_parciales) {
        ElementoEsparso actual = lista[i];
        int j = i + 1;
        while (j < num_parciales && lista[j].fila == actual.fila && lista[j].col == actual.col) {
            actual.valor += lista[j].valor;
            j++;
        }
        fwrite(&actual, sizeof(ElementoEsparso), 1, fres);
        i = j;
    }
    free(lista);
    fclose(fres);

    // --- PASO 4: Escritura del Resultado Final en Texto ---[cite: 29]
    FILE *f_res_bin = fopen("resultado.bin", "rb");
    FILE *f_fin = fopen("matriz_axb.txt", "w");
    
    fseek(f_res_bin, 0, SEEK_END);
    long total_final = ftell(f_res_bin) / sizeof(ElementoEsparso);
    rewind(f_res_bin);

    fprintf(f_fin, "%d %d %ld\n", M, P, total_final);
    while (fread(&e, sizeof(ElementoEsparso), 1, f_res_bin) == 1) {
        fprintf(f_fin, "%d %d %.2f\n", e.fila, e.col, e.valor);
    }
    fclose(f_res_bin);
    fclose(f_fin);

    printf("Proceso completado con exito. Archivo final: matriz_axb.txt\n");
    return 0;
}