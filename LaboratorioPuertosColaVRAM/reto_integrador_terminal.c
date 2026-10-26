/**
 * ============================================================================
 * UNIVERSIDAD CATÓLICA BOLIVIANA "SAN PABLO"
 * FACULTAD DE INGENIERÍA - DEPARTAMENTO DE INGENIERÍA DE SISTEMAS
 * SIS-131: ARQUITECTURA DE COMPUTADORAS
 * 
 * LABORATORIO: E/S POR PUERTO, COLA FIFO EN RAM Y CONTROLADOR VRAM
 * RETO INTEGRADOR: Terminal con E/S Aislada, Cola FIFO en RAM, VRAM y Tecla Backspace
 * 
 * Docente    : Ing. Paulo César Loayza Carrasco
 * Estudiante : Leonardo Durán Cuenca
 * Archivo    : reto_integrador_terminal.c
 * Compilación: gcc -std=c99 -Wall -Wextra -pedantic reto_integrador_terminal.c -o terminal
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define TAMANO_RAM          256

/* --- Mapa de Memoria Arquitectónico --- */
#define COLA_BASE_DIR       0x70    /* 0x70 - 0x77: Buffer circular FIFO en RAM (8 bytes) */
#define COLA_CAPACIDAD      8       /* Capacidad de la cola de teclado */
#define DIR_KB_HEAD         0x78    /* 0x78: Índice de lectura HEAD */
#define DIR_KB_TAIL         0x79    /* 0x79: Índice de escritura TAIL */
#define DIR_KB_COUNT        0x7A    /* 0x7A: Contador de elementos en cola */

#define VRAM_BASE_DIR       0xF0    /* 0xF0 - 0xFF: Memoria de Video (4x4 = 16 bytes) */
#define VRAM_LIMITE_DIR     0xFF    /* Fin del búfer de pantalla */
#define VRAM_FILAS          4
#define VRAM_COLUMNAS       4

/* --- Puertos de Entrada/Salida Aislada --- */
#define PUERTO_KB_STATUS    0x01    /* Bit 0: Tecla lista */
#define PUERTO_KB_DATA      0x02    /* Código ASCII de la tecla */
#define KB_STATUS_READY_BIT 0x01

/* --- Estructura Integral de la Computadora --- */
typedef struct {
    uint8_t  ram[TAMANO_RAM];
    uint16_t cursor_vram;    /* Dirección actual del cursor en VRAM (0xF0..0xFF) */
    
    /* Buses del Sistema */
    uint8_t bus_address;
    uint8_t bus_data;
    bool    bus_io_read;
    bool    bus_io_write;
    bool    bus_mem_read;
    bool    bus_mem_write;

    /* Hardware del Teclado */
    uint8_t kb_reg_status;
    uint8_t kb_reg_data;

    /* Latch del Monitor */
    char    monitor_latch[VRAM_FILAS][VRAM_COLUMNAS];
    uint32_t total_refrescos;
} SistemaComputadora;

static SistemaComputadora g_pc;

/**
 * Validador de colisión en el Bus de Control
 * MEM_READ + MEM_WRITE + IO_READ + IO_WRITE <= 1
 */
static bool validar_bus(void) {
    int activos = (g_pc.bus_mem_read ? 1 : 0) + 
                  (g_pc.bus_mem_write ? 1 : 0) + 
                  (g_pc.bus_io_read ? 1 : 0) + 
                  (g_pc.bus_io_write ? 1 : 0);
    return (activos <= 1);
}

/**
 * Tick del controlador de hardware del teclado ante señales del bus
 */
static void tick_controlador_teclado(void) {
    if (!validar_bus()) return;

    if (g_pc.bus_io_read) {
        if (g_pc.bus_address == PUERTO_KB_STATUS) {
            g_pc.bus_data = g_pc.kb_reg_status;
        } else if (g_pc.bus_address == PUERTO_KB_DATA) {
            g_pc.bus_data = g_pc.kb_reg_data;
            /* Handshake automático de hardware */
            g_pc.kb_reg_status = 0x00;
            g_pc.kb_reg_data   = 0x00;
        }
    }
}

/**
 * Instrucción CPU IN: Lee un dato desde un puerto aislado
 */
static uint8_t cpu_in(uint8_t puerto) {
    g_pc.bus_address  = puerto;
    g_pc.bus_io_read  = true;
    g_pc.bus_mem_read = false;

    tick_controlador_teclado();

    uint8_t dato = g_pc.bus_data;

    g_pc.bus_io_read  = false;
    g_pc.bus_address  = 0x00;
    g_pc.bus_data     = 0x00;
    return dato;
}

/**
 * Encola un carácter en la cola circular en RAM (0x70..0x7A)
 */
bool cola_teclado_encolar(uint8_t ascii) {
    uint8_t tail  = g_pc.ram[DIR_KB_TAIL];
    uint8_t count = g_pc.ram[DIR_KB_COUNT];

    if (count >= COLA_CAPACIDAD) {
        return false; /* Buffer Overflow */
    }

    g_pc.ram[COLA_BASE_DIR + tail] = ascii;
    g_pc.ram[DIR_KB_TAIL]  = (tail + 1) % COLA_CAPACIDAD;
    g_pc.ram[DIR_KB_COUNT] = count + 1;
    return true;
}

/**
 * Desencola un carácter de la cola circular en RAM (0x70..0x7A)
 */
bool cola_teclado_desencolar(uint8_t *salida_ascii) {
    uint8_t head  = g_pc.ram[DIR_KB_HEAD];
    uint8_t count = g_pc.ram[DIR_KB_COUNT];

    if (count == 0) {
        return false; /* Cola vacía */
    }

    *salida_ascii = g_pc.ram[COLA_BASE_DIR + head];
    g_pc.ram[DIR_KB_HEAD]  = (head + 1) % COLA_CAPACIDAD;
    g_pc.ram[DIR_KB_COUNT] = count - 1;
    return true;
}

/**
 * Desplazamiento Vertical de VRAM (Scroll Up)
 */
void vram_scroll_arriba(void) {
    for (int f = 0; f < VRAM_FILAS - 1; f++) {
        for (int c = 0; c < VRAM_COLUMNAS; c++) {
            g_pc.ram[VRAM_BASE_DIR + (f * VRAM_COLUMNAS) + c] = 
                g_pc.ram[VRAM_BASE_DIR + ((f + 1) * VRAM_COLUMNAS) + c];
        }
    }
    for (int c = 0; c < VRAM_COLUMNAS; c++) {
        g_pc.ram[VRAM_BASE_DIR + ((VRAM_FILAS - 1) * VRAM_COLUMNAS) + c] = 0x20;
    }
}

/**
 * Actualiza el Latch del monitor y dibuja la interfaz en pantalla
 */
void monitor_actualizar_pantalla(void) {
    for (int f = 0; f < VRAM_FILAS; f++) {
        for (int c = 0; c < VRAM_COLUMNAS; c++) {
            g_pc.monitor_latch[f][c] = (char)g_pc.ram[VRAM_BASE_DIR + (f * VRAM_COLUMNAS) + c];
        }
    }
    g_pc.total_refrescos++;

    printf("\n  +----------- VRAM MONITOR 4x4 -----------+\n");
    for (int f = 0; f < VRAM_FILAS; f++) {
        printf("  | Fila %d: [ ", f);
        for (int c = 0; c < VRAM_COLUMNAS; c++) {
            uint8_t dir = VRAM_BASE_DIR + (f * VRAM_COLUMNAS) + c;
            char ch = g_pc.monitor_latch[f][c];
            if (dir == g_pc.cursor_vram) {
                printf(">%c< ", (ch == ' ') ? '_' : ch);
            } else {
                printf(" %c  ", ch);
            }
        }
        printf("] |\n");
    }
    printf("  +----------------------------------------+\n");
    printf("  | Cursor VRAM: 0x%02X | Cola: %u/%u        |\n", 
           g_pc.cursor_vram, g_pc.ram[DIR_KB_COUNT], COLA_CAPACIDAD);
    printf("  +----------------------------------------+\n");
}

/**
 * Simulación de pulsación de tecla física por el usuario:
 * Carga el registro del periférico y dispara la rutina ISR de la CPU
 * que lee mediante IN y deposita el carácter en la cola FIFO de RAM.
 */
void simular_evento_tecla(char c) {
    g_pc.kb_reg_data   = (uint8_t)c;
    g_pc.kb_reg_status = KB_STATUS_READY_BIT;

    /* Rutina de Servicio de Interrupción (ISR) del Teclado */
    uint8_t st = cpu_in(PUERTO_KB_STATUS);
    if (st & KB_STATUS_READY_BIT) {
        uint8_t ascii = cpu_in(PUERTO_KB_DATA);
        bool ok = cola_teclado_encolar(ascii);
        if (!ok) {
            printf("  [ISR ALERTA] Tecla '%c' descartada por BUFFER OVERFLOW!\n", c);
        }
    }
}

/**
 * Procesador de la terminal integradora:
 * Desencola caracteres de la FIFO y actualiza el búfer VRAM.
 * Implementa borrado interactivo con Backspace (0x08 y 0x7F) y Scroll automático.
 */
bool terminal_procesar_siguiente_tecla(void) {
    uint8_t tecla;
    if (!cola_teclado_desencolar(&tecla)) {
        return false; /* No hay teclas pendientes */
    }

    if (tecla == 0x08 || tecla == 0x7F) {
        /* BACKSPACE: Borrado visual del carácter anterior */
        if (g_pc.cursor_vram > VRAM_BASE_DIR) {
            g_pc.cursor_vram--;
            g_pc.ram[g_pc.cursor_vram] = 0x20; /* Reemplaza por espacio */
            printf("  [TERMINAL] <BACKSPACE> ejecutado -> Cursor retrocede a 0x%02X\n", 
                   g_pc.cursor_vram);
        } else {
            printf("  [TERMINAL] <BACKSPACE> ignorado: Cursor al inicio de VRAM (0x%02X)\n", 
                   g_pc.cursor_vram);
        }
    } 
    else if (tecla >= 32 && tecla <= 126) {
        /* CARÁCTER IMPRIMIBLE */
        if (g_pc.cursor_vram <= VRAM_LIMITE_DIR) {
            g_pc.ram[g_pc.cursor_vram] = tecla;
            g_pc.cursor_vram++;
            printf("  [TERMINAL] Carácter '%c' (0x%02X) escrito en VRAM -> Cursor avanza a 0x%02X\n",
                   (char)tecla, tecla, g_pc.cursor_vram);
        } else {
            /* Pantalla llena: Ejecutar Scroll automático para abrir espacio */
            printf("  [TERMINAL] VRAM llena! Desplazando hacia arriba (Scroll Up)...\n");
            vram_scroll_arriba();
            g_pc.cursor_vram = VRAM_BASE_DIR + (3 * VRAM_COLUMNAS); /* Inicio última fila */
            g_pc.ram[g_pc.cursor_vram] = tecla;
            g_pc.cursor_vram++;
            printf("  [TERMINAL] Carácter '%c' (0x%02X) escrito en nueva línea (0x%02X)\n",
                   (char)tecla, tecla, g_pc.cursor_vram);
        }
    }

    monitor_actualizar_pantalla();
    return true;
}

/**
 * Inicialización completa del sistema
 */
void sistema_inicializar(void) {
    memset(&g_pc, 0, sizeof(SistemaComputadora));
    
    /* Limpieza de VRAM con espacios 0x20 */
    for (int i = 0; i < VRAM_FILAS * VRAM_COLUMNAS; i++) {
        g_pc.ram[VRAM_BASE_DIR + i] = 0x20;
    }
    g_pc.cursor_vram = VRAM_BASE_DIR;

    /* Inicializar Latch */
    for (int f = 0; f < VRAM_FILAS; f++) {
        for (int c = 0; c < VRAM_COLUMNAS; c++) {
            g_pc.monitor_latch[f][c] = ' ';
        }
    }
}

int main(void) {
    printf("====================================================================\n");
    printf("  RETO INTEGRADOR: CONTROLADOR DE TERMINAL Y MONITOR VRAM EN ANSI C\n");
    printf("  Arquitectura de Computadoras - UCB San Pablo\n");
    printf("====================================================================\n");

    sistema_inicializar();
    printf("\n[PASO 0] Estado Inicial de la Terminal:\n");
    monitor_actualizar_pantalla();

    /* Paso 1: Escritura normal de caracteres */
    printf("\n[PASO 1] Escribiendo 'H', 'O', 'L', 'A':\n");
    const char *sec1 = "HOLA";
    for (size_t i = 0; i < strlen(sec1); i++) {
        simular_evento_tecla(sec1[i]);
    }
    while (terminal_procesar_siguiente_tecla()) { }

    /* Paso 2: Corrección con Backspace (Borrar 'A' y 'L', luego escribir 'T', 'E') */
    printf("\n[PASO 2] Prueba de Backspace: Enviar 2 Backspaces (0x08) y teclear 'T', 'E':\n");
    simular_evento_tecla(0x08); /* Borra 'A' */
    simular_evento_tecla(0x08); /* Borra 'L' */
    simular_evento_tecla('T');  /* Escribe 'T' */
    simular_evento_tecla('E');  /* Escribe 'E' */
    while (terminal_procesar_siguiente_tecla()) { }

    /* Paso 3: Llenar la pantalla y disparar el Scroll Vertical */
    printf("\n[PASO 3] Llenado continuo de VRAM para provocar Scroll Automatico:\n");
    const char *sec2 = "123456789ABCDEF";
    for (size_t i = 0; i < strlen(sec2); i++) {
        simular_evento_tecla(sec2[i]);
        terminal_procesar_siguiente_tecla();
    }

    /* Paso 4: Backspace en pantalla con scroll */
    printf("\n[PASO 4] Backspace en nueva linea post-scroll:\n");
    simular_evento_tecla(0x08);
    simular_evento_tecla('Z');
    while (terminal_procesar_siguiente_tecla()) { }

    /* Paso 5: Volcado final de memoria RAM (0x70..0x7A y 0xF0..0xFF) */
    printf("\n[PASO 5] Volcado Arquitectonico de RAM del Sistema:\n");
    printf("  --- Cola Teclado (0x70..0x77) + Punteros (0x78..0x7A) ---\n  ");
    for (int i = 0x70; i <= 0x7A; i++) {
        printf("[0x%02X]=0x%02X ", i, g_pc.ram[i]);
    }
    printf("\n\n  --- VRAM Pantalla (0xF0..0xFF) ---\n");
    for (int f = 0; f < 4; f++) {
        printf("  Fila %d (0x%02X): ", f, VRAM_BASE_DIR + (f * 4));
        for (int c = 0; c < 4; c++) {
            uint8_t d = VRAM_BASE_DIR + (f * 4) + c;
            printf("['%c' 0x%02X] ", (g_pc.ram[d] >= 32 && g_pc.ram[d] <= 126) ? g_pc.ram[d] : ' ', g_pc.ram[d]);
        }
        printf("\n");
    }

    printf("\n====================================================================\n");
    printf("  RETO INTEGRADOR COMPLETADO CON EXITO!\n");
    printf("====================================================================\n");

    return 0;
}
