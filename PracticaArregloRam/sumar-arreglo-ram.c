#include <stdio.h>

#define TAMANO 5

int main(void) {
    // Arreglo de enteros almacenado en memoria RAM
    int arreglo[TAMANO] = {15, 25, 35, 45, 55};
    int suma = 0;

    // Recorrido e iteración secuencial del arreglo en RAM
    for (int i = 0; i < TAMANO; i++) {
        suma += arreglo[i];
    }

    // Salida informativa de la ejecución
    printf("=== Suma de un Arreglo Almacenado en RAM ===\n\n");
    printf("Elementos en memoria RAM:\n");
    for (int i = 0; i < TAMANO; i++) {
        printf("  arreglo[%d] (Direccion: %p) = %d\n", i, (void*)&arreglo[i], arreglo[i]);
    }
    printf("\nResultado total de la suma: %d\n", suma);

    return 0;
}
