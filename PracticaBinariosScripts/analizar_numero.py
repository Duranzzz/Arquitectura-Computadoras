#!/usr/bin/env python3
import sys

def a_complemento_dos(valor, bits):
    minimo = -(2 ** (bits - 1))
    maximo = (2 ** (bits - 1)) - 1
    if valor < minimo or valor > maximo:
        return None
    if valor < 0:
        valor = (1 << bits) + valor
    return format(valor, f"0{bits}b")

def hay_overflow_8bits(valor):
    return valor < -128 or valor > 127

def analizar(n):
    print(f"\n=== Analisis de {n} ===")
    signo = "-" if n < 0 else ""
    print(f"Binario:      {signo}{bin(abs(n))[2:]}")
    print(f"Octal:        {signo}{oct(abs(n))[2:]}")
    print(f"Hexadecimal:  {signo}{hex(abs(n))[2:]}")
    for bits in (8, 16, 32):
        rep = a_complemento_dos(n, bits)
        print(f"CA2 {bits} bits:  " + (rep if rep else "no representable (fuera de rango)"))
    print("Overflow en 8 bits con signo:", "SI" if hay_overflow_8bits(n) else "NO")

def main():
    if len(sys.argv) > 1:
        valores = [int(x) for x in sys.argv[1:]]
    else:
        valores = [int(input("Introduce un numero entero decimal: "))]
    for v in valores:
        analizar(v)

if __name__ == "__main__":
    main()