/**
 * @file saturacion_swap.c
 * @brief Programa en C para saturar la memoria RAM disponible y forzar el uso de Swap en Linux.
 * 
 * Arquitectura de Computadoras (SIS-131)
 * Universidad Catolica Boliviana "San Pablo" - Sede Santa Cruz
 * Docente: Ing. Paulo Cesar Loayza Carrasco
 * Estudiante: Leonardo Duran Cuenca
 * 
 * Principio Arquitectonico:
 * En Linux, el kernel implementa "Demand Paging" y "Memory Overcommit".
 * Un simple malloc() solo reserva espacio en el mapa de memoria virtual;
 * no consume memoria fisica real hasta que las paginas son escritas (Page Fault).
 * Este programa reserva bloques continuos y escribe en cada pagina (4096 bytes)
 * para forzar al subsistema de gestion de memoria del kernel a desalojar paginas
 * a la particion/archivo de intercambio (SWAP) cuando la RAM se agota.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

#define CHUNK_SIZE_MB 50                     // Tamano del bloque a asignar en cada paso (50 MB)
#define CHUNK_SIZE_BYTES (CHUNK_SIZE_MB * 1024 * 1024)
#define PAGE_SIZE_BYTES 4096                 // Tamano de pagina estandar en x86-64 / Linux

// Estructura de lista enlazada para retener los punteros asignados
typedef struct MemoryNode {
    char *ptr;
    struct MemoryNode *next;
} MemoryNode;

static MemoryNode *head = NULL;
static size_t total_mb = 0;
static volatile bool keep_running = true;

// Manejador de senal para capturar Ctrl+C (SIGINT) de forma ordenada
void handle_sigint(int sig) {
    (void)sig;
    keep_running = false;
    printf("\n\n[INFO] Interrupcion recibida (SIGINT). Deteniendo asignacion de memoria...\n");
}

// Liberar toda la memoria reservada antes de salir
void free_all_memory(void) {
    printf("[INFO] Liberando bloques de memoria para devolver recursos al SO...\n");
    MemoryNode *current = head;
    while (current != NULL) {
        MemoryNode *temp = current;
        current = current->next;
        free(temp->ptr);
        free(temp);
    }
    head = NULL;
    printf("[INFO] Memoria liberada exitosamente. Total devuelto: %zu MB (%.2f GB).\n",
           total_mb, (double)total_mb / 1024.0);
}

int main(int argc, char *argv[]) {
    // Configurar retardo entre asignaciones (por defecto 100 ms para dar tiempo a monitorear)
    int sleep_ms = 100;
    if (argc > 1) {
        sleep_ms = atoi(argv[1]);
        if (sleep_ms < 0) sleep_ms = 0;
    }

    // Registrar manejador de Ctrl+C
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);

    printf("======================================================================\n");
    printf("   PRUEBA DE SATURACION DE MEMORIA RAM Y ACTIVACION DE SWAP (LINUX)  \n");
    printf("   Arquitectura de Computadoras - UCB San Pablo                       \n");
    printf("======================================================================\n");
    printf(" Tamano de bloque por iteracion: %d MB\n", CHUNK_SIZE_MB);
    printf(" Retardo entre bloques:          %d ms\n", sleep_ms);
    printf(" Tamano de pagina de memoria:    %d bytes\n", PAGE_SIZE_BYTES);
    printf(" Para monitorear en otra terminal, ejecuta cualquiera de los siguientes:\n");
    printf("   1) watch -n 1 free -h\n");
    printf("   2) vmstat 1\n");
    printf("   3) htop\n");
    printf(" Presiona Ctrl+C en cualquier momento para detener la prueba de forma segura.\n");
    printf("======================================================================\n\n");

    printf("Iniciando asignacion continua de memoria...\n");

    int iteration = 0;
    while (keep_running) {
        iteration++;

        // 1. Asignar el bloque de memoria virtual
        char *block = (char *)malloc(CHUNK_SIZE_BYTES);
        if (block == NULL) {
            printf("\n[ALERTA] malloc() devolvio NULL en la iteracion %d. Total asignado: %zu MB.\n",
                   iteration, total_mb);
            printf("[ALERTA] El sistema ha denegado mas memoria. Presiona Enter para salir...");
            getchar();
            break;
        }

        // 2. FORZAR LA ASIGNACION FISICA (Page Faults / Dirty Pages)
        // En Linux, si no escribes en la memoria, no consume RAM fisica.
        // Escribimos en cada limite de pagina (cada 4096 bytes) para activar las tablas de paginas.
        for (size_t offset = 0; offset < CHUNK_SIZE_BYTES; offset += PAGE_SIZE_BYTES) {
            block[offset] = (char)(iteration & 0xFF);
        }
        // Escribir tambien el ultimo byte del bloque
        block[CHUNK_SIZE_BYTES - 1] = 0xAA;

        // 3. Guardar el nodo en la lista enlazada para retener la referencia
        MemoryNode *node = (MemoryNode *)malloc(sizeof(MemoryNode));
        if (node == NULL) {
            free(block);
            printf("\n[ALERTA] No se pudo crear el nodo de control en la iteracion %d.\n", iteration);
            break;
        }
        node->ptr = block;
        node->next = head;
        head = node;

        total_mb += CHUNK_SIZE_MB;

        // Mostrar progreso en terminal
        printf(" -> Paso %4d: Asignados +%d MB | Total retenido: %6zu MB (~%.2f GB)\n",
               iteration, CHUNK_SIZE_MB, total_mb, (double)total_mb / 1024.0);
        fflush(stdout);

        // Pausa para dar tiempo al monitor de memoria
        if (sleep_ms > 0) {
            usleep(sleep_ms * 1000);
        }
    }

    printf("\n======================================================================\n");
    printf(" RESUMEN DE EJECUCION\n");
    printf(" Total de pasos completados:    %d\n", iteration);
    printf(" Total de memoria fisica usada: %zu MB (~%.2f GB)\n", total_mb, (double)total_mb / 1024.0);
    printf("======================================================================\n");

    printf("\nPresiona Enter para liberar la memoria y finalizar el programa...");
    fflush(stdout);
    getchar();

    free_all_memory();

    printf("[FIN] Programa concluido normalmente.\n");
    return 0;
}
