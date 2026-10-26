/**
 * ============================================================================
 * UNIVERSIDAD CATÓLICA BOLIVIANA "SAN PABLO"
 * FACULTAD DE INGENIERÍA - DEPARTAMENTO DE INGENIERÍA DE SISTEMAS
 * SIS-131: ARQUITECTURA DE COMPUTADORAS
 * 
 * LABORATORIO: E/S POR PUERTO, COLA FIFO EN RAM Y CONTROLADOR VRAM
 * EXPERIMENTO 2: Cola Circular FIFO en Memoria RAM con Gestión de Desbordamiento
 * 
 * Docente    : Ing. Paulo César Loayza Carrasco
 * Estudiante : Leonardo Durán Cuenca
 * Archivo    : exp2_cola_teclado.c
 * Compilación: gcc -std=c99 -Wall -Wextra -pedantic exp2_cola_teclado.c -o exp2
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define TAMANO_RAM      256

/* --- Mapa de Memoria RAM para la Cola FIFO del Teclado --- */
#define COLA_BASE_DIR   0x70    /* 0x70 - 0x77: Buffer circular en RAM (8 bytes) */
#define COLA_CAPACIDAD  8       /* Capacidad máxima de la cola */

#define DIR_KB_HEAD     0x78    /* 0x78: Puntero de lectura (índice 0..7) */
#define DIR_KB_TAIL     0x79    /* 0x79: Puntero de escritura (índice 0..7) */
#define DIR_KB_COUNT    0x7A    /* 0x7A: Contador de elementos en cola (0..8) */

/* Memoria RAM global del sistema simulado */
static uint8_t g_ram[TAMANO_RAM];

/**
 * Inicializa la memoria RAM y los punteros de la cola en cero
 */
void cola_teclado_inicializar(void) {
    memset(g_ram, 0, sizeof(g_ram));
    g_ram[DIR_KB_HEAD]  = 0;
    g_ram[DIR_KB_TAIL]  = 0;
    g_ram[DIR_KB_COUNT] = 0;
}

/**
 * Encola un carácter ASCII en el buffer circular residente en RAM
 * Retorna true en éxito, o false en caso de BUFFER OVERFLOW.
 */
bool cola_teclado_encolar(uint8_t ascii) {
    uint8_t tail  = g_ram[DIR_KB_TAIL];
    uint8_t count = g_ram[DIR_KB_COUNT];

    if (count >= COLA_CAPACIDAD) {
        /* BUFFER OVERFLOW: La capacidad física de 8 bytes está saturada */
        return false;
    }

    /* Escribe estrictamente en la celda de memoria calculada */
    g_ram[COLA_BASE_DIR + tail] = ascii;

    /* Aritmética modular para avance circular del puntero TAIL */
    g_ram[DIR_KB_TAIL]  = (tail + 1) % COLA_CAPACIDAD;
    g_ram[DIR_KB_COUNT] = count + 1;

    return true;
}

/**
 * Desencola un carácter ASCII del buffer circular residente en RAM
 * Retorna true en éxito, o false si la cola está vacía.
 */
bool cola_teclado_desencolar(uint8_t *salida_ascii) {
    uint8_t head  = g_ram[DIR_KB_HEAD];
    uint8_t count = g_ram[DIR_KB_COUNT];

    if (count == 0) {
        /* COLA VACÍA: No existen bytes pendientes de lectura */
        return false;
    }

    /* Lee el byte apuntado por HEAD en la memoria RAM */
    *salida_ascii = g_ram[COLA_BASE_DIR + head];

    /* Opcional para depuración: limpiar celda leída con 0 */
    /* g_ram[COLA_BASE_DIR + head] = 0x00; */

    /* Aritmética modular para avance circular del puntero HEAD */
    g_ram[DIR_KB_HEAD]  = (head + 1) % COLA_CAPACIDAD;
    g_ram[DIR_KB_COUNT] = count - 1;

    return true;
}

/**
 * Muestra una visualización arquitectónica detallada de las celdas de RAM
 * correspondientes a la cola (0x70..0x77) y sus metadatos (0x78..0x7A)
 */
void imprimir_volcado_ram_cola(const char *motivo) {
    uint8_t head  = g_ram[DIR_KB_HEAD];
    uint8_t tail  = g_ram[DIR_KB_TAIL];
    uint8_t count = g_ram[DIR_KB_COUNT];

    printf("\n>>> Estado RAM [%s] <<<\n", motivo);
    printf("  +------+------+------+------+------+------+------+------+\n");
    printf("  | 0x70 | 0x71 | 0x72 | 0x73 | 0x74 | 0x75 | 0x76 | 0x77 |\n");
    printf("  +------+------+------+------+------+------+------+------+\n  |");

    for (int i = 0; i < COLA_CAPACIDAD; i++) {
        uint8_t val = g_ram[COLA_BASE_DIR + i];
        char c = (val >= 32 && val <= 126) ? (char)val : ' ';
        printf(" '%c'  |", c);
    }
    printf("\n  |");

    for (int i = 0; i < COLA_CAPACIDAD; i++) {
        printf(" 0x%02X |", g_ram[COLA_BASE_DIR + i]);
    }
    printf("\n  +------+------+------+------+------+------+------+------+\n  |");

    for (int i = 0; i < COLA_CAPACIDAD; i++) {
        char puntero[7] = "      ";
        if (i == head && i == tail) {
            strcpy(puntero, " H,T  ");
        } else if (i == head) {
            strcpy(puntero, " HEAD ");
        } else if (i == tail) {
            strcpy(puntero, " TAIL ");
        }
        printf("%s|", puntero);
    }
    printf("\n  +------+------+------+------+------+------+------+------+\n");
    printf("  METADATOS EN RAM: KB_HEAD(0x78)=%u | KB_TAIL(0x79)=%u | KB_COUNT(0x7A)=%u/%u\n",
           head, tail, count, COLA_CAPACIDAD);
}

int main(void) {
    printf("====================================================================\n");
    printf("  EXPERIMENTO 2: COLA CIRCULAR FIFO EN MEMORIA RAM (0x70..0x7A)\n");
    printf("  Arquitectura de Computadoras - UCB San Pablo\n");
    printf("====================================================================\n");

    cola_teclado_inicializar();
    imprimir_volcado_ram_cola("Inicializacion del Sistema");

    /* Paso 1: Intentar desencolar en cola vacía */
    printf("\n[TEST 1] Desencolado en Cola Vacia:\n");
    uint8_t leido;
    if (!cola_teclado_desencolar(&leido)) {
        printf("  [OK] cola_teclado_desencolar retorno FALSE (Cola vacia detectada correctamente)\n");
    } else {
        printf("  [ERROR] Se leyo un byte en cola vacia!\n");
    }

    /* Paso 2: Encolar secuencia inicial ("HOLA") */
    printf("\n[TEST 2] Encolando 'H', 'O', 'L', 'A' (4 bytes):\n");
    const char *palabra1 = "HOLA";
    for (size_t i = 0; i < strlen(palabra1); i++) {
        bool ok = cola_teclado_encolar((uint8_t)palabra1[i]);
        printf("  Encolar '%c': %s\n", palabra1[i], ok ? "EXITO" : "FALLO");
    }
    imprimir_volcado_ram_cola("Tras Encolar 'HOLA'");

    /* Paso 3: Desencolar 2 bytes ('H' y 'O') */
    printf("\n[TEST 3] Desencolando 2 bytes del frente:\n");
    for (int i = 0; i < 2; i++) {
        if (cola_teclado_desencolar(&leido)) {
            printf("  Desencolado: '%c' (0x%02X)\n", (char)leido, leido);
        }
    }
    imprimir_volcado_ram_cola("Tras Desencolar 2 bytes");

    /* Paso 4: Encolar hasta provocar desbordamiento (Wrap-around circular y Overflow) */
    printf("\n[TEST 4] Demostracion de Wrap-around y Prueba de Desbordamiento (Overflow):\n");
    const char *palabra2 = "1234567"; /* 7 caracteres; habiendo 2 en cola, llenará los 6 restantes y fallará el 7mo */
    for (size_t i = 0; i < strlen(palabra2); i++) {
        bool ok = cola_teclado_encolar((uint8_t)palabra2[i]);
        printf("  Encolar '%c' -> %s (Count=%u)\n", 
               palabra2[i], 
               ok ? "EXITO" : "[RECHAZADO - BUFFER OVERFLOW]", 
               g_ram[DIR_KB_COUNT]);
    }
    imprimir_volcado_ram_cola("Cola Saturada a Capacidad Maxima (8/8)");

    /* Paso 5: Desencolar todos los elementos restantes verificando orden FIFO */
    printf("\n[TEST 5] Vaciado Completo de Cola FIFO (Verificacion de Integridad):\n");
    printf("  Secuencia extraida: ");
    while (cola_teclado_desencolar(&leido)) {
        printf("%c", (char)leido);
    }
    printf("\n");
    imprimir_volcado_ram_cola("Cola Completamente Vaciada");

    printf("\n====================================================================\n");
    printf("  EXPERIMENTO 2 COMPLETADO EXITOSAMENTE\n");
    printf("====================================================================\n");

    return 0;
}
