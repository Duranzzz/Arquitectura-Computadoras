/**
 * ============================================================================
 * UNIVERSIDAD CATÓLICA BOLIVIANA "SAN PABLO"
 * FACULTAD DE INGENIERÍA - DEPARTAMENTO DE INGENIERÍA DE SISTEMAS
 * SIS-131: ARQUITECTURA DE COMPUTADORAS
 * 
 * LABORATORIO: ENTRADAS, SALIDAS Y MEMORIA DE VIDEO (VRAM)
 * PROGRAMA 3 : ESCRITURA DIRECTA EN MEMORIA DEL MONITOR (escribir_monitor_linux.c)
 * 
 * Docente    : Ing. Paulo César Loayza Carrasco
 * Estudiante : Leonardo Durán Cuenca
 * Compilación: gcc -Wall -Wextra -pedantic escribir_monitor_linux.c -o monitor_linux
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#if defined(__unix__) || defined(__APPLE__) || defined(__linux__)
#include <unistd.h>
#include <sys/mman.h>
#endif

#if defined(__linux__)
#include <sys/ioctl.h>
#include <linux/fb.h>
#endif

#define SHM_FILAS 10
#define SHM_COLS  40

typedef struct {
    uint8_t ascii;
    uint8_t attribute;
} __attribute__((packed)) CeldaVram;

void opcion_linux_framebuffer(void) {
    printf("\n====================================================================\n");
    printf("  OPCIÓN 1: LINUX DIRECT FRAMEBUFFER (/dev/fb0 con mmap)             \n");
    printf("====================================================================\n");
#if defined(__linux__)
    int fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror(">> Error al abrir /dev/fb0 (Ejecuta con sudo o en consola TTY)");
        return;
    }
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
    ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);

    long screensize = vinfo.yres_virtual * finfo.line_length;
    uint8_t *vram = (uint8_t *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (vram == MAP_FAILED) { 
        perror(">> Error mmap"); 
        close(fbfd); 
        return; 
    }

    int start_x = vinfo.xres / 2 - 50;
    int start_y = vinfo.yres / 2 - 50;
    int bpp = vinfo.bits_per_pixel / 8;

    for (int y = start_y; y < start_y + 100; y++) {
        for (int x = start_x; x < start_x + 100; x++) {
            long loc = (x + vinfo.xoffset) * bpp + (y + vinfo.yoffset) * finfo.line_length;
            if (bpp == 4) { 
                vram[loc]   = 255; /* Azul */
                vram[loc+1] = 255; /* Verde */
                vram[loc+2] = 255; /* Rojo */
                vram[loc+3] = 0;   /* Alpha */
            }
        }
    }
    printf(">> Cuadrado blanco dibujado en VRAM física del Framebuffer.\n");
    munmap(vram, screensize); 
    close(fbfd);
#else
    printf(">> Funcionalidad exclusiva del kernel Linux con dispositivo (/dev/fb0).\n");
    printf(">> Arquitectura: Mapea píxeles RGB directo en la GPU mediante mmap().\n");
#endif
}

void opcion_linux_vcsa(void) {
    printf("\n====================================================================\n");
    printf("  OPCIÓN 2: MEMORIA DE VIDEO DE CONSOLA VIRTUAL (/dev/vcsa)          \n");
    printf("====================================================================\n");
#if defined(__linux__)
    int fd = open("/dev/vcsa", O_RDWR);
    if (fd == -1) { 
        perror(">> Error al abrir /dev/vcsa (Requiere sudo)"); 
        return; 
    }
    lseek(fd, 4, SEEK_SET);
    const char *mensaje = "HOLA DESDE MEMORIA /dev/vcsa";
    for (int i = 0; mensaje[i] != '\0'; i++) {
        uint8_t celda[2] = {(uint8_t)mensaje[i], 0x1E};
        write(fd, celda, 2);
    }
    printf(">> Texto escrito en la memoria de la consola virtual.\n");
    close(fd);
#else
    printf(">> Funcionalidad exclusiva de consolas TTY en Linux (/dev/vcsa).\n");
    printf(">> Arquitectura: Escribe pares [ASCII, Color VGA] sin pasar por stdout.\n");
#endif
}

void opcion_linux_dev_mem(void) {
    printf("\n====================================================================\n");
    printf("  OPCIÓN 3: MEMORIA FÍSICA BRUTA (/dev/mem con mmap y ASM)           \n");
    printf("====================================================================\n");
    printf("Código en C + Ensamblador x86 embebido:\n");
    printf("  int mem_fd = open(\"/dev/mem\", O_RDWR | O_SYNC);\n");
    printf("  uint8_t *vram = (uint8_t *)mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0xB8000);\n");
    printf("  char caracter = 'X';\n");
    printf("  uint8_t color = 0x4F; /* Fondo Rojo (4) + Texto Blanco Brillante (F) */\n");
    printf("  __asm__ __volatile__ (\n");
    printf("      \"movb %%0, (%%1)    \\n\\t\"  /* Escribe ASCII en Bus de Datos hacia RAM */\n");
    printf("      \"movb %%2, 1(%%1)   \\n\\t\"  /* Escribe Atributo de Color en bus */\n");
    printf("      : : \"r\"(caracter), \"r\"(vram), \"r\"(color) : \"memory\"\n");
    printf("  );\n");
    printf(">> [ANÁLISIS]: Acceso directo al bus de memoria física en dirección base 0xB8000.\n");
}

void opcion_linux_shm_simulada(void) {
    printf("\n====================================================================\n");
    printf("  OPCIÓN 4: VRAM MAPEADA EN MEMORIA CON PROYECCIÓN CRT              \n");
    printf("====================================================================\n");

    CeldaVram vram[SHM_FILAS * SHM_COLS];
    memset(vram, 0, sizeof(vram));
    CeldaVram *pvram = vram;

    const char *linea1 = ">>> VRAM MAPEADA EN MEMORIA RAM <<<";
    const char *linea2 = "Formato VGA: [Byte ASCII] + [Color]";
    const char *linea3 = "Escrito directamente con punteros C";

    for (int i = 0; linea1[i] != '\0'; i++) {
        pvram[2 * SHM_COLS + 3 + i].ascii = (uint8_t)linea1[i];
        pvram[2 * SHM_COLS + 3 + i].attribute = 0x1E; /* Fondo Azul + Texto Amarillo */
    }
    for (int i = 0; linea2[i] != '\0'; i++) {
        pvram[4 * SHM_COLS + 2 + i].ascii = (uint8_t)linea2[i];
        pvram[4 * SHM_COLS + 2 + i].attribute = 0x2A; /* Fondo Verde + Texto Verde Claro */
    }
    for (int i = 0; linea3[i] != '\0'; i++) {
        pvram[6 * SHM_COLS + 2 + i].ascii = (uint8_t)linea3[i];
        pvram[6 * SHM_COLS + 2 + i].attribute = 0x4F; /* Fondo Rojo + Texto Blanco */
    }

    printf("  +--- MONITOR DE VIDEO: LECTURA EN VIVO DE LA VRAM ---+\n");
    for (int f = 0; f < SHM_FILAS; f++) {
        printf("  |");
        for (int c = 0; c < SHM_COLS; c++) {
            CeldaVram cel = pvram[f * SHM_COLS + c];
            char ch = (cel.ascii >= 32 && cel.ascii <= 126) ? (char)cel.ascii : ' ';
            int fg = 30 + (cel.attribute & 0x07);
            int bg = 40 + ((cel.attribute >> 4) & 0x07);
            int bold = (cel.attribute & 0x08) ? 1 : 0;
            if (cel.attribute == 0) {
                printf(" ");
            } else {
                printf("\033[%d;%d;%dm%c\033[0m", bold, fg, bg, ch);
            }
        }
        printf("|\n");
    }
    printf("  +----------------------------------------------------+\n");
}

int main(int argc, char *argv[]) {
    /* Modo demostración / no interactivo */
    if (argc > 1 && strcmp(argv[1], "--demo") == 0) {
        printf("====================================================================\n");
        printf("  ESCRITURA DIRECTA EN MEMORIA DEL MONITOR (MODO DEMO INTEGRAL)     \n");
        printf("====================================================================\n");
        opcion_linux_framebuffer();
        opcion_linux_vcsa();
        opcion_linux_dev_mem();
        opcion_linux_shm_simulada();
        return 0;
    }

    int opcion;
    printf("====================================================================\n");
    printf("  ESCRITURA DIRECTA EN MEMORIA DEL MONITOR EN LINUX                 \n");
    printf("====================================================================\n");
    printf("Seleccione la opción de memoria de video en Linux:\n");
    printf("  [1] LINUX DIRECT FRAMEBUFFER (/dev/fb0 con mmap) -> Píxeles gráficos\n");
    printf("  [2] CONSOLA VIRTUAL DE LINUX (/dev/vcsa) -> Modo texto [ASCII, Color]\n");
    printf("  [3] MEMORIA FÍSICA BRUTA (/dev/mem con mmap y ASM) -> Bus físico\n");
    printf("  [4] VRAM DIDÁCTICA EN MEMORIA (mmap) -> Monitor CRT interactivo\n");
    printf("Ingrese opción (1-4): ");

    if (scanf("%d", &opcion) != 1) return 1;

    switch (opcion) {
        case 1: opcion_linux_framebuffer(); break;
        case 2: opcion_linux_vcsa(); break;
        case 3: opcion_linux_dev_mem(); break;
        case 4: opcion_linux_shm_simulada(); break;
        default: printf("Opción no válida.\n"); return 1;
    }
    return 0;
}
