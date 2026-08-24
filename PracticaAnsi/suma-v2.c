#include <stdio.h>
#include <stdint.h>

typedef union {
    unsigned char byte;
    struct{
        unsigned char b0 : 1; /*Bit menos significativo (LSB)*/
        unsigned char b1 : 1;
        unsigned char b2 : 1;
        unsigned char b3 : 1;
        unsigned char b4 : 1;
        unsigned char b5 : 1;
        unsigned char b6 : 1;
        unsigned char b7 : 1; /*Bit más significativo (MSB)*/
    };
    
} Reg8Bits;

int main() {
    int8_t a = 120, b = 20;
    int8_t resultado = a + b;

    printf("%d + %d = %d\n", a, b, resultado);

    if (a>0 && b>0){
        if (a > INT8_MAX - b) {
            printf("Desborde\n");
        }
    }
    if (a<0 && b<0){
        if (a < INT8_MIN - b) {
            printf("Desborde\n");
        }
    }

    Reg8Bits miDato;
    miDato.byte = resultado;
    printf("Resultado en binario: %d%d%d%d%d%d%d%d\n", miDato.b7, miDato.b6, miDato.b5, miDato.b4, miDato.b3, miDato.b2, miDato.b1, miDato.b0);

    return 0;
}