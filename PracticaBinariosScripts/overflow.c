#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Union para inspeccionar el mismo registro de 8 bits como signed y unsigned
typedef union {
    int8_t con_signo;
    uint8_t sin_signo;
} Registro8;

void imprimir_binario(uint8_t val) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (val >> i) & 1);
    }
}

int main(int argc, char *argv[]) {
    int val_a, val_b;

    if (argc > 2) {
        val_a = atoi(argv[1]);
        val_b = atoi(argv[2]);
    } else {
        printf("Ingrese el primer numero (a): ");
        if (scanf("%d", &val_a) != 1) return 1;
        printf("Ingrese el segundo numero (b): ");
        if (scanf("%d", &val_b) != 1) return 1;
    }

    Registro8 a, b, res;

    a.con_signo = (int8_t)val_a;
    b.con_signo = (int8_t)val_b;
    res.con_signo = a.con_signo + b.con_signo;

    printf("\n=== Analisis de Operacion (8 bits) ===\n");

    printf("a:   Signed: %4d | Unsigned: %3u | Binario: ", a.con_signo, a.sin_signo);
    imprimir_binario(a.sin_signo);
    printf("\n");

    printf("b:   Signed: %4d | Unsigned: %3u | Binario: ", b.con_signo, b.sin_signo);
    imprimir_binario(b.sin_signo);
    printf("\n");

    printf("-----------------------------------------\n");

    printf("Suma Matematica (int) : %d\n", val_a + val_b);
    printf("Resultado (8 bits)    : Signed: %4d | Unsigned: %3u | Binario: ", res.con_signo, res.sin_signo);
    imprimir_binario(res.sin_signo);
    printf("\n\n");

    // Deteccion de overflow con signo (Signed Overflow)
    int overflow_signed = ((a.con_signo > 0 && b.con_signo > 0 && res.con_signo < 0) ||
                           (a.con_signo < 0 && b.con_signo < 0 && res.con_signo >= 0));

    // Deteccion de acarreo sin signo (Unsigned Carry)
    int carry_unsigned = ((unsigned int)a.sin_signo + (unsigned int)b.sin_signo) > 255;

    printf("Banderas de Estado:\n");
    printf("  - Signed Overflow (OF): %s\n", overflow_signed ? "SI (desbordamiento con signo)" : "NO");
    printf("  - Unsigned Carry   (CF): %s\n", carry_unsigned ? "SI (acarreo sin signo)" : "NO");

    return 0;
}