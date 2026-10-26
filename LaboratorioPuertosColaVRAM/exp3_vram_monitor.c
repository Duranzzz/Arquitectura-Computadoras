/**
 * ============================================================================
 * UNIVERSIDAD CATÓLICA BOLIVIANA "SAN PABLO"
 * FACULTAD DE INGENIERÍA - DEPARTAMENTO DE INGENIERÍA DE SISTEMAS
 * SIS-131: ARQUITECTURA DE COMPUTADORAS
 * 
 * LABORATORIO: E/S POR PUERTO, COLA FIFO EN RAM Y CONTROLADOR VRAM
 * EXPERIMENTO 3: Memoria de Video VRAM (0xF0..0xFF), Latch y Desplazamiento Vertical
 * 
 * Docente    : Ing. Paulo César Loayza Carrasco
 * Estudiante : Leonardo Durán Cuenca
 * Archivo    : exp3_vram_monitor.c
 * Compilación: gcc -std=c99 -Wall -Wextra -pedantic exp3_vram_monitor.c -o exp3
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define TAMANO_RAM      256

/* --- Mapa de Memoria de Video VRAM --- */
#define VRAM_BASE_DIR   0xF0    /* 0xF0 - 0xFF: Matriz de 16 caracteres (4x4) */
#define VRAM_FILAS      4
#define VRAM_COLUMNAS   4
#define VRAM_TOTAL_BYTES (VRAM_FILAS * VRAM_COLUMNAS)

/* Memoria RAM global compartida */
static uint8_t g_ram[TAMANO_RAM];

/* Latch de Hardware del Monitor (Aisla la lectura del haz de electrones/pantalla) */
typedef struct {
    char matriz_latch[VRAM_FILAS][VRAM_COLUMNAS];
    uint32_t ciclos_refresco;
} MonitorHardware;

static MonitorHardware g_monitor;

/**
 * Inicializa la VRAM con espacios en blanco (0x20)
 */
void vram_inicializar(void) {
    for (int i = 0; i < VRAM_TOTAL_BYTES; i++) {
        g_ram[VRAM_BASE_DIR + i] = 0x20; /* Espacio en blanco ASCII */
    }
    memset(&g_monitor, 0, sizeof(MonitorHardware));
    for (int f = 0; f < VRAM_FILAS; f++) {
        for (int c = 0; c < VRAM_COLUMNAS; c++) {
            g_monitor.matriz_latch[f][c] = ' ';
        }
    }
}

/**
 * Escribe un carácter en coordenadas (Fila, Columna) de la VRAM
 * Mapeo físico: Dirección = 0xF0 + (Fila * 4) + Columna
 */
bool vram_escribir(uint8_t fila, uint8_t col, char c) {
    if (fila >= VRAM_FILAS || col >= VRAM_COLUMNAS) {
        return false; /* Coordenada fuera de límites */
    }
    uint8_t dir = VRAM_BASE_DIR + (fila * VRAM_COLUMNAS) + col;
    g_ram[dir] = (uint8_t)c;
    return true;
}

/**
 * Lee un carácter directamente de una posición de la VRAM
 */
char vram_leer(uint8_t fila, uint8_t col) {
    if (fila >= VRAM_FILAS || col >= VRAM_COLUMNAS) {
        return '\0';
    }
    uint8_t dir = VRAM_BASE_DIR + (fila * VRAM_COLUMNAS) + col;
    return (char)g_ram[dir];
}

/**
 * Desplazamiento Vertical hacia arriba (Scroll Up)
 * Copia filas (f+1) hacia fila (f), y llena la última fila con espacios (0x20)
 */
void vram_scroll_arriba(void) {
    for (int f = 0; f < VRAM_FILAS - 1; f++) {
        for (int c = 0; c < VRAM_COLUMNAS; c++) {
            g_ram[VRAM_BASE_DIR + (f * VRAM_COLUMNAS) + c] = 
                g_ram[VRAM_BASE_DIR + ((f + 1) * VRAM_COLUMNAS) + c];
        }
    }
    /* Limpia la última fila con espacios */
    for (int c = 0; c < VRAM_COLUMNAS; c++) {
        g_ram[VRAM_BASE_DIR + ((VRAM_FILAS - 1) * VRAM_COLUMNAS) + c] = 0x20;
    }
}

/**
 * Simulación del Latch de Pantalla:
 * Copia el contenido de la VRAM en el buffer de retención del monitor
 * para evitar parpadeos (tearing / flickering) en el renderizado
 */
void monitor_capturar_latch(void) {
    for (int f = 0; f < VRAM_FILAS; f++) {
        for (int c = 0; c < VRAM_COLUMNAS; c++) {
            uint8_t dir = VRAM_BASE_DIR + (f * VRAM_COLUMNAS) + c;
            g_monitor.matriz_latch[f][c] = (char)g_ram[dir];
        }
    }
    g_monitor.ciclos_refresco++;
}

/**
 * Renderiza la pantalla física a partir del Latch del monitor
 */
void monitor_renderizar(const char *titulo) {
    monitor_capturar_latch();

    printf("\n+================================+\n");
    printf("| PANTALLA CRT 4x4: %-12s |\n", titulo);
    printf("+--------------------------------+\n");
    printf("|       C0   C1   C2   C3        |\n");
    for (int f = 0; f < VRAM_FILAS; f++) {
        printf("| F%d: [  %c    %c    %c    %c  ]       |\n", 
               f,
               g_monitor.matriz_latch[f][0],
               g_monitor.matriz_latch[f][1],
               g_monitor.matriz_latch[f][2],
               g_monitor.matriz_latch[f][3]);
    }
    printf("+--------------------------------+\n");
    printf("| Latch Sync Ciclos: %-10u  |\n", g_monitor.ciclos_refresco);
    printf("+================================+\n");
}

/**
 * Muestra el volcado hexadecimal de los 16 bytes de VRAM (0xF0..0xFF)
 */
void imprimir_volcado_vram(void) {
    printf("\n>>> Volcado Hexadecimal de VRAM (0xF0 .. 0xFF) <<<\n");
    printf(" Dir  | +0(C0) +1(C1) +2(C2) +3(C3) | Caracteres\n");
    printf("------+-----------------------------+------------\n");
    for (int f = 0; f < VRAM_FILAS; f++) {
        uint8_t base = VRAM_BASE_DIR + (f * VRAM_COLUMNAS);
        printf(" 0x%02X |  0x%02X   0x%02X   0x%02X   0x%02X  |  %c %c %c %c\n",
               base,
               g_ram[base + 0], g_ram[base + 1], g_ram[base + 2], g_ram[base + 3],
               g_ram[base + 0], g_ram[base + 1], g_ram[base + 2], g_ram[base + 3]);
    }
    printf("------+-----------------------------+------------\n");
}

int main(void) {
    printf("====================================================================\n");
    printf("  EXPERIMENTO 3: CONTROLADOR DE VIDEO VRAM 4x4 Y LATCH DE PANTALLA\n");
    printf("  Arquitectura de Computadoras - UCB San Pablo\n");
    printf("====================================================================\n");

    vram_inicializar();
    monitor_renderizar("VRAM Limpia");
    imprimir_volcado_vram();

    /* Paso 1: Escribir texto línea por línea */
    printf("\n[TEST 1] Escritura estructurada en matriz de VRAM (0xF0..0xFF):\n");
    const char *lineas[4] = { "HOLA", "MUND", "UCB ", "2026" };
    for (int f = 0; f < 4; f++) {
        for (int c = 0; c < 4; c++) {
            vram_escribir(f, c, lineas[f][c]);
        }
    }
    monitor_renderizar("Texto Cargado");
    imprimir_volcado_vram();

    /* Paso 2: Validación de protección de límites */
    printf("\n[TEST 2] Comprobacion de Limites Fuera de Rango (Bound Checks):\n");
    bool r1 = vram_escribir(4, 0, 'X'); /* Fila inválida */
    bool r2 = vram_escribir(2, 5, 'Y'); /* Columna inválida */
    printf("  Escritura en fila 4, col 0 : %s\n", r1 ? "ACEPTADA" : "RECHAZADA (Correcto)");
    printf("  Escritura en fila 2, col 5 : %s\n", r2 ? "ACEPTADA" : "RECHAZADA (Correcto)");

    /* Paso 3: Desplazamiento Vertical hacia arriba (Scroll Up) */
    printf("\n[TEST 3] Ejecutando Desplazamiento Vertical (Scroll Up):\n");
    printf("  Las filas 1, 2, 3 se moveran a las posiciones 0, 1, 2.\n");
    printf("  La fila 3 se reiniciara con espacios en blanco.\n");
    vram_scroll_arriba();
    monitor_renderizar("Post-Scroll");
    imprimir_volcado_vram();

    /* Paso 4: Escribir nueva línea en la fila inferior liberada */
    printf("\n[TEST 4] Escribiendo nueva linea 'TEST' en la fila 3:\n");
    const char *nueva_linea = "TEST";
    for (int c = 0; c < 4; c++) {
        vram_escribir(3, c, nueva_linea[c]);
    }
    monitor_renderizar("Nueva Linea");
    imprimir_volcado_vram();

    printf("\n====================================================================\n");
    printf("  EXPERIMENTO 3 COMPLETADO EXITOSAMENTE\n");
    printf("====================================================================\n");

    return 0;
}
