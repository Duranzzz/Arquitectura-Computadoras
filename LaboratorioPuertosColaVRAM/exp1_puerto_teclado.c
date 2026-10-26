/**
 * ============================================================================
 * UNIVERSIDAD CATÓLICA BOLIVIANA "SAN PABLO"
 * FACULTAD DE INGENIERÍA - DEPARTAMENTO DE INGENIERÍA DE SISTEMAS
 * SIS-131: ARQUITECTURA DE COMPUTADORAS
 * 
 * LABORATORIO: E/S POR PUERTO, COLA FIFO EN RAM Y CONTROLADOR VRAM
 * EXPERIMENTO 1: Lectura Directa desde Puerto de Entrada y Handshake de Hardware
 * 
 * Docente    : Ing. Paulo César Loayza Carrasco
 * Estudiante : Leonardo Durán Cuenca
 * Archivo    : exp1_puerto_teclado.c
 * Compilación: gcc -std=c99 -Wall -Wextra -pedantic exp1_puerto_teclado.c -o exp1
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* --- Definición del Mapa de Puertos de E/S Aislada --- */
#define PUERTO_KB_STATUS  0x01  /* Puerto 0x01: Registro de Estado (Bit 0 = Tecla Lista) */
#define PUERTO_KB_DATA    0x02  /* Puerto 0x02: Registro de Datos (Código ASCII) */

#define KB_STATUS_READY_BIT 0x01

/* --- Estructura del Sistema de Buses Compartidos --- */
typedef struct {
    uint8_t address_bus; /* Bus de Direcciones (8 bits) */
    uint8_t data_bus;    /* Bus de Datos (8 bits bidireccional) */
    bool    mem_read;    /* Señal de Control: Lectura de Memoria RAM */
    bool    mem_write;   /* Señal de Control: Escritura en Memoria RAM */
    bool    io_read;     /* Señal de Control: Lectura de Puerto E/S (IOR) */
    bool    io_write;    /* Señal de Control: Escritura en Puerto E/S (IOW) */
} SystemBuses;

/* --- Estructura del Periférico Hardware: Controlador de Teclado --- */
typedef struct {
    uint8_t reg_status;  /* Registro Interno de Estado (0x01) */
    uint8_t reg_data;    /* Registro Interno de Retención de Datos (0x02) */
    uint32_t total_keys_typed;
    uint32_t total_keys_read;
} TecladoHardware;

/* Instancias globales de simulación */
static SystemBuses      g_bus;
static TecladoHardware  g_teclado;

/**
 * Validador de Exclusión Mutua de Buses de Control
 * Regla de Oro: MEM_READ + MEM_WRITE + IO_READ + IO_WRITE <= 1
 */
static bool validar_exclusion_mutua_buses(const SystemBuses *bus) {
    int control_signals_active = 0;
    if (bus->mem_read)  control_signals_active++;
    if (bus->mem_write) control_signals_active++;
    if (bus->io_read)   control_signals_active++;
    if (bus->io_write)  control_signals_active++;

    if (control_signals_active > 1) {
        fprintf(stderr, "\n[FATAL BUS COLLISION] Cortocircuito en Bus de Control! Señales activas: %d\n", 
                control_signals_active);
        fprintf(stderr, "  MEM_READ=%d, MEM_WRITE=%d, IO_READ=%d, IO_WRITE=%d\n",
                bus->mem_read, bus->mem_write, bus->io_read, bus->io_write);
        return false;
    }
    return true;
}

/**
 * Controlador de Hardware de Teclado (Simulación en ciclo de reloj)
 * Expone datos en el bus externo cuando detecta IO_READ con su dirección.
 * Ejecuta el Handshake automático al leer el puerto de datos (0x02).
 */
void tick_controlador_teclado(SystemBuses *bus, TecladoHardware *kb) {
    if (!validar_exclusion_mutua_buses(bus)) {
        return;
    }

    if (bus->io_read) {
        if (bus->address_bus == PUERTO_KB_STATUS) {
            /* Entrega el estado del periférico al bus de datos */
            bus->data_bus = kb->reg_status;
        } 
        else if (bus->address_bus == PUERTO_KB_DATA) {
            /* Entrega el byte ASCII del teclado al bus de datos */
            bus->data_bus = kb->reg_data;
            /* Handshake automático: Al ser leído el registro de datos,
               el periférico limpia su bandera de estado (reg_status = 0) */
            kb->reg_status = 0x00;
            kb->reg_data   = 0x00;
            kb->total_keys_read++;
        }
    } else {
        /* Líneas en Alta Impedancia (Hi-Z) cuando no hay lectura activa */
        /* En simulación de C, el bus no es alterado por el dispositivo */
    }
}

/**
 * Simulación de Instrucción CPU IN: IN [Puerto] -> Acumulador/Registro
 * Maneja las fases T de ciclo de bus y la aduana MAR/MDR
 */
uint8_t cpu_instruccion_in(uint8_t puerto) {
    /* Fase Setup: MAR expone dirección al Bus de Direcciones */
    g_bus.address_bus = puerto;
    g_bus.io_read     = true;
    g_bus.mem_read    = false;
    g_bus.mem_write   = false;
    g_bus.io_write    = false;

    /* Pulso de Reloj: El periférico seleccionado responde en el bus */
    tick_controlador_teclado(&g_bus, &g_teclado);

    /* Fase Latch / Captura: MDR absorbe el dato expuesto en el bus */
    uint8_t dato_leido = g_bus.data_bus;

    /* Fin del ciclo de bus: Se desactivan las señales de control */
    g_bus.io_read     = false;
    g_bus.address_bus = 0x00;
    g_bus.data_bus    = 0x00;

    return dato_leido;
}

/**
 * Simulación de pulsación física en el teclado (evento de entrada)
 */
void hardware_presionar_tecla(char c) {
    g_teclado.reg_data   = (uint8_t)c;
    g_teclado.reg_status = KB_STATUS_READY_BIT; /* Bit 0 en Alto (1 = tecla disponible) */
    g_teclado.total_keys_typed++;
    printf("  [HW EVENT] Pulsación de tecla: '%c' (0x%02X) -> reg_status=0x%02X, reg_data=0x%02X\n",
           c, (uint8_t)c, g_teclado.reg_status, g_teclado.reg_data);
}

/**
 * Imprime el estado actual del bus y registros
 */
static void imprimir_estado(const char *etiqueta) {
    printf("\n--- %s ---\n", etiqueta);
    printf("  BUS: Addr=0x%02X | Data=0x%02X | CTRL: MR=%d MW=%d IOR=%d IOW=%d\n",
           g_bus.address_bus, g_bus.data_bus,
           g_bus.mem_read, g_bus.mem_write, g_bus.io_read, g_bus.io_write);
    printf("  HARDWARE KB: Status=0x%02X (Ready=%d) | Data=0x%02X ('%c')\n",
           g_teclado.reg_status, (g_teclado.reg_status & KB_STATUS_READY_BIT) ? 1 : 0,
           g_teclado.reg_data, (g_teclado.reg_data >= 32 && g_teclado.reg_data <= 126) ? g_teclado.reg_data : '.');
}

int main(void) {
    printf("====================================================================\n");
    printf("  EXPERIMENTO 1: E/S AISLADA Y HANDSHAKE AUTOMATICO EN ANSI C\n");
    printf("  Arquitectura de Computadoras - UCB San Pablo\n");
    printf("====================================================================\n");

    memset(&g_bus, 0, sizeof(SystemBuses));
    memset(&g_teclado, 0, sizeof(TecladoHardware));

    imprimir_estado("Estado Inicial del Sistema");

    /* Paso 1: Polling de estado con puerto vacío */
    printf("\n[TEST 1] Sondeo (Polling) de Estado con Registro Vacio:\n");
    uint8_t st = cpu_instruccion_in(PUERTO_KB_STATUS);
    printf("  CPU ejecuta IN 0x%02X -> Recibido Status = 0x%02X (Tecla lista: %s)\n",
           PUERTO_KB_STATUS, st, (st & KB_STATUS_READY_BIT) ? "SI" : "NO");

    /* Paso 2: Presionar una tecla en el hardware */
    printf("\n[TEST 2] Generacion de Evento de Teclado:\n");
    hardware_presionar_tecla('A');
    imprimir_estado("Despues de presionar 'A'");

    /* Paso 3: Polling detecta la tecla lista */
    printf("\n[TEST 3] Sondeo CPU detecta tecla lista:\n");
    st = cpu_instruccion_in(PUERTO_KB_STATUS);
    printf("  CPU ejecuta IN 0x%02X -> Recibido Status = 0x%02X (Tecla lista: %s)\n",
           PUERTO_KB_STATUS, st, (st & KB_STATUS_READY_BIT) ? "SI" : "NO");

    /* Paso 4: CPU lee el dato y se verifica el Handshake automático */
    printf("\n[TEST 4] Lectura de Dato por Puerto 0x02 y Handshake:\n");
    uint8_t data = cpu_instruccion_in(PUERTO_KB_DATA);
    printf("  CPU ejecuta IN 0x%02X -> Recibido ASCII = '%c' (0x%02X)\n",
           PUERTO_KB_DATA, (char)data, data);
    imprimir_estado("Inmediatamente despues de leer 0x02 (Handshake activo)");

    /* Paso 5: Verificar que status volvio a 0 */
    st = cpu_instruccion_in(PUERTO_KB_STATUS);
    printf("\n[TEST 5] Verificacion de Limpieza de Flag tras Handshake:\n");
    printf("  CPU ejecuta IN 0x%02X -> Status = 0x%02X (Tecla lista: %s - Handshake EXITOSO!)\n",
           PUERTO_KB_STATUS, st, (st & KB_STATUS_READY_BIT) ? "SI" : "NO");

    /* Paso 6: Secuencia continua de caracteres con Polling */
    printf("\n[TEST 6] Rafaga de Caracteres con Polling CPU ('U', 'C', 'B'):\n");
    const char secuencia[] = "UCB";
    for (size_t i = 0; i < strlen(secuencia); i++) {
        hardware_presionar_tecla(secuencia[i]);
        /* CPU polling */
        while ((cpu_instruccion_in(PUERTO_KB_STATUS) & KB_STATUS_READY_BIT) == 0) {
            /* Espera activa */
        }
        uint8_t car = cpu_instruccion_in(PUERTO_KB_DATA);
        printf("  CPU consumio: '%c' (0x%02X) | Status post-lectura: 0x%02X\n",
               car, car, g_teclado.reg_status);
    }

    /* Paso 7: Validacion de exclusion mutua (Prueba de Colisión de Bus) */
    printf("\n[TEST 7] Comprobacion de Seguridad: Regla de Exclusion Mutua de Bus:\n");
    printf("  Formula: MEM_READ + MEM_WRITE + IO_READ + IO_WRITE <= 1\n");
    printf("  Inyectando colision artificial (IO_READ=1 y MEM_READ=1 simultaneos)...\n");
    SystemBuses bus_fallo;
    memset(&bus_fallo, 0, sizeof(SystemBuses));
    bus_fallo.io_read = true;
    bus_fallo.mem_read = true; /* COLISIÓN */
    bool valida = validar_exclusion_mutua_buses(&bus_fallo);
    printf("  Resultado de validacion: %s (Deteccion preventiva correcta)\n", 
           valida ? "PERMITIDO" : "BLOQUEADO CON EXCEPCION");

    printf("\n====================================================================\n");
    printf("  EXPERIMENTO 1 COMPLETADO EXITOSAMENTE\n");
    printf("====================================================================\n");

    return 0;
}
