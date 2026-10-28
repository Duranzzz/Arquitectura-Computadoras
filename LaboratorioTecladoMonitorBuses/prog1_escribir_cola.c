/**
 * ============================================================================
 * UNIVERSIDAD CATÓLICA BOLIVIANA "SAN PABLO"
 * FACULTAD DE INGENIERÍA - DEPARTAMENTO DE INGENIERÍA DE SISTEMAS
 * SIS-131: ARQUITECTURA DE COMPUTADORAS
 * 
 * LABORATORIO: ENTRADAS, SALIDAS Y MEMORIA DE VIDEO (VRAM)
 * PROGRAMA 1 : ESCRITOR EN COLA / MEMORIA DE TECLADO (prog1_escribir_cola.c)
 * 
 * Docente    : Ing. Paulo César Loayza Carrasco
 * Estudiante : Leonardo Durán Cuenca
 * Compilación: gcc -Wall -Wextra -pedantic prog1_escribir_cola.c -o prog1
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#if defined(__unix__) || defined(__APPLE__)
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#define BDA_FILE "/tmp/bios_keyboard_bda.bin"
#else
#define BDA_FILE "bios_keyboard_bda.bin"
#endif

#pragma pack(push, 1)
typedef struct {
    uint16_t head;          /* Offset 0x001A: Puntero de lectura */
    uint16_t tail;          /* Offset 0x001C: Puntero de escritura */
    uint16_t buffer[16];    /* Offset 0x001E a 0x003D: 16 palabras (Scan-code en MSB, ASCII en LSB) */
    uint8_t  port_60_data;  /* Registro de dato del Puerto 0x60 */
    uint8_t  port_64_status;/* Registro de estado del Puerto 0x64 (Bit 0: Tecla lista) */
} BiosKeyboardMemory;
#pragma pack(pop)

void cargar_bda(BiosKeyboardMemory *bda) {
    FILE *f = fopen(BDA_FILE, "rb");
    if (!f) {
        bda->head = 0x001E;
        bda->tail = 0x001E;
        memset(bda->buffer, 0, sizeof(bda->buffer));
        bda->port_60_data = 0;
        bda->port_64_status = 0;
    } else {
        if (fread(bda, sizeof(BiosKeyboardMemory), 1, f) != 1) {
            bda->head = 0x001E;
            bda->tail = 0x001E;
        }
        fclose(f);
    }
}

void guardar_bda(const BiosKeyboardMemory *bda) {
    FILE *f = fopen(BDA_FILE, "wb");
    if (f) {
        fwrite(bda, sizeof(BiosKeyboardMemory), 1, f);
        fclose(f);
    }
}

void metodo_inyectado(const char *texto) {
    printf("\n--- [OPCIÓN 1: INYECTANDO EN COLA DE ENTRADA TTY] ---\n");
#if defined(__unix__) || defined(__APPLE__)
    int fd = open("/dev/tty", O_RDWR);
    if (fd < 0) fd = STDIN_FILENO;
    int i = 0;
    while (texto[i] != '\0') {
        if (ioctl(fd, TIOCSTI, &texto[i]) < 0) {
            perror("Error en ioctl TIOCSTI");
            if (fd != STDIN_FILENO) close(fd);
            return;
        }
        i++;
    }
    if (fd != STDIN_FILENO) close(fd);
    printf(">> ¡Éxito! Se inyectaron %d caracteres en la cola de entrada del terminal.\n", i);
#else
    printf("TIOCSTI solo disponible en entornos POSIX / Unix nativos.\n");
    printf("Simulando inyeccion en driver de consola del sistema para: \"%s\"\n", texto);
#endif
}

void metodo_asm_int16(char ascii, uint8_t scan_code) {
    printf("\n--- [OPCIÓN 2: EN ASM CON INT 16h (SERVICIO 05h)] ---\n");
    printf("Código Ensamblador equivalente en arquitectura x86:\n");
    printf("  mov ah, 05h       ; AH = 05h (Insertar pulsación en buffer)\n");
    printf("  mov ch, 0x%02X      ; CH = Scan-code (hardware)\n", scan_code);
    printf("  mov cl, '%c' (0x%02X); CL = Código ASCII\n", ascii, (uint8_t)ascii);
    printf("  int 16h           ; Llamada al vector de interrupción BIOS\n");

    BiosKeyboardMemory bda;
    cargar_bda(&bda);

    uint16_t next_tail = (bda.tail >= 0x003C) ? 0x001E : bda.tail + 2;
    if (next_tail == bda.head) {
        printf(">> [BIOS INT 16h] AL = 1: BUFFER OVERFLOW. Buffer lleno.\n");
        return;
    }

    int idx = (bda.tail - 0x001E) / 2;
    bda.buffer[idx] = ((uint16_t)scan_code << 8) | (uint8_t)ascii;
    bda.tail = next_tail;
    bda.port_60_data = scan_code;
    bda.port_64_status |= 0x01;
    guardar_bda(&bda);

    printf(">> [BIOS INT 16h] Tecla '%c' insertada con éxito. AL = 0.\n", ascii);
    printf(">> [ESTADO BDA] TAIL = 0x%04X | HEAD = 0x%04X | Port 64h = 0x%02X\n",
           bda.tail, bda.head, bda.port_64_status);
}

void metodo_directo_memoria(char ascii, uint8_t scan_code) {
    printf("\n--- [OPCIÓN 3: DIRECTO A MEMORIA RAM (BDA 0x0040:0x001A)] ---\n");
    BiosKeyboardMemory bda;
    cargar_bda(&bda);

    uint16_t next_tail = (bda.tail >= 0x003C) ? 0x001E : bda.tail + 2;
    if (next_tail == bda.head) {
        printf(">> [ERROR RAM] Buffer circular lleno en memoria física.\n");
        return;
    }

    int idx = (bda.tail - 0x001E) / 2;
    bda.buffer[idx] = ((uint16_t)scan_code << 8) | (uint8_t)ascii;
    bda.tail = next_tail;
    bda.port_60_data = scan_code;
    bda.port_64_status |= 0x01;
    guardar_bda(&bda);

    printf(">> [RAM BDA] Escribiendo palabra 0x%04X en offset 0x%04X\n", bda.buffer[idx], bda.tail);
    printf(">> [RAM BDA] Nuevo TAIL = 0x%04X | HEAD = 0x%04X\n", bda.tail, bda.head);
}

void volcado_completo_bda(void) {
    BiosKeyboardMemory bda;
    cargar_bda(&bda);
    printf("\n=== VOLCADO DE MEMORIA BDA (0x0040:0x001A - 0x003D) ===\n");
    printf("  HEAD (0x001A): 0x%04X | TAIL (0x001C): 0x%04X\n", bda.head, bda.tail);
    printf("  PUERTO 0x60 (Data): 0x%02X | PUERTO 0x64 (Status): 0x%02X\n",
           bda.port_60_data, bda.port_64_status);
    printf("  BUFFER CIRCULAR (16 palabras):\n  ");
    for (int i = 0; i < 16; i++) {
        uint16_t offset = 0x001E + (i * 2);
        char mark = ' ';
        if (offset == bda.head && offset == bda.tail) mark = '*';
        else if (offset == bda.head) mark = 'H';
        else if (offset == bda.tail) mark = 'T';

        uint8_t asc = (uint8_t)(bda.buffer[i] & 0xFF);
        uint8_t sc  = (uint8_t)(bda.buffer[i] >> 8);
        printf("[%02X:%c '%c' 0x%02X%02X] ", i, mark, (asc >= 32 && asc <= 126) ? asc : '.', sc, asc);
        if ((i + 1) % 4 == 0) printf("\n  ");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    /* Modo demostración / no interactivo */
    if (argc > 1 && strcmp(argv[1], "--demo") == 0) {
        printf("====================================================================\n");
        printf("  PROGRAMA 1: ESCRITOR EN COLA / MEMORIA DE TECLADO (MODO DEMO)     \n");
        printf("====================================================================\n");
        metodo_inyectado("ARQUITECTURA_UCB\n");
        metodo_asm_int16('A', 0x1E);
        metodo_asm_int16('B', 0x30);
        metodo_directo_memoria('C', 0x2E);
        volcado_completo_bda();
        return 0;
    }

    int opcion;
    char texto[100];
    char c;

    printf("====================================================================\n");
    printf("  PROGRAMA 1: ESCRITOR EN COLA / MEMORIA DE TECLADO                 \n");
    printf("====================================================================\n");
    printf("Seleccione el método de escritura:\n");
    printf("  [1] INYECTADO (En la cola de entrada del SO via ioctl TIOCSTI)\n");
    printf("  [2] EN ASM CON INT 16h (Servicio 05h de la BIOS x86)\n");
    printf("  [3] DIRECTO A MEMORIA (Segmento 0x0040:0x001A de la BIOS en RAM)\n");
    printf("  [4] VOLCADO DE ESTRUCTURA BDA COMPLETA\n");
    printf("Ingrese opción (1-4): ");

    if (scanf("%d", &opcion) != 1) return 1;
    while ((c = getchar()) != '\n' && c != EOF);

    switch (opcion) {
        case 1:
            printf("\nIngrese texto o palabra a inyectar: ");
            if (fgets(texto, sizeof(texto), stdin)) {
                if (texto[strlen(texto) - 1] != '\n') strcat(texto, "\n");
                metodo_inyectado(texto);
            }
            break;
        case 2:
            printf("\nIngrese un carácter para enviar por INT 16h: ");
            c = getchar();
            if (c != '\n' && c != EOF) {
                uint8_t sc = (c >= 'A' && c <= 'Z') ? (uint8_t)(0x1E + (c - 'A')) : 0x1E;
                metodo_asm_int16(c, sc);
            }
            break;
        case 3:
            printf("\nIngrese un carácter para escribir directo a RAM: ");
            c = getchar();
            if (c != '\n' && c != EOF) {
                metodo_directo_memoria(c, 0x1E);
            }
            break;
        case 4:
            volcado_completo_bda();
            break;
        default:
            printf("Opción no válida.\n");
            return 1;
    }
    return 0;
}
