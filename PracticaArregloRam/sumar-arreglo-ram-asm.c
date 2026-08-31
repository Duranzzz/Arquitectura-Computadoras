#include <stdio.h>

#define TAMANO 5

int main(void) {
    // Arreglo de enteros almacenado en memoria RAM
    int arreglo[TAMANO] = {15, 25, 35, 45, 55};
    int suma = 0;

    // Suma del arreglo almacenado en RAM utilizando ensamblador en línea (inline Assembly)
    __asm__ (
        "movl $0, %%eax\n\t"              // %%eax = acumulador de la suma (suma = 0)
        "movl $0, %%ecx\n\t"              // %%ecx = índice del bucle (i = 0)
        "bucle_suma:\n\t"                 // Etiqueta de inicio del bucle
        "cmpl %2, %%ecx\n\t"              // Compara índice 'i' (%ecx) con TAMANO (%2)
        "jge fin_suma\n\t"                // Si i >= TAMANO, sale del bucle
        // Lectura directa desde memoria RAM: base + (índice * 4 bytes por entero)
        "addl (%1, %%rcx, 4), %%eax\n\t"  // eax += arreglo[i]
        "incl %%ecx\n\t"                  // i++ (incrementa el índice)
        "jmp bucle_suma\n\t"              // Salta al inicio del ciclo
        "fin_suma:\n\t"
        "movl %%eax, %0\n\t"              // Almacena el resultado acumulado en la variable C 'suma'
        : "=r" (suma)                     // Operando de salida: %0
        : "r" (arreglo), "r" (TAMANO)     // Operandos de entrada: %1 = puntero base en RAM, %2 = tamaño
        : "%rax", "%rcx", "memory"        // Registros modificados y barrera de memoria
    );

    // Salida informativa de la ejecución
    printf("=== Suma de un Arreglo Almacenado en RAM (Inline Assembly) ===\n\n");
    printf("Elementos en memoria RAM:\n");
    for (int i = 0; i < TAMANO; i++) {
        printf("  arreglo[%d] (Direccion: %p) = %d\n", i, (void*)&arreglo[i], arreglo[i]);
    }
    printf("\nResultado total de la suma calculada con ASM: %d\n", suma);

    return 0;
}
