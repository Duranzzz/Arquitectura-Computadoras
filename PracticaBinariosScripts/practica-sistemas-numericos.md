# Práctica: Sistemas Numéricos y Aritmética Binaria — Resuelta

**Materia:** Arquitectura de Computadoras — Ingeniería de Software
**Entorno:** Ubuntu Linux

---

## Nivel 1 — Conversión de bases (fundamentos)

### 1.1 Decimal → Binario

**Método:** se hacen divisiones sucesivas entre 2, anotando el residuo (0 o 1) de cada división, hasta que el cociente llega a 0. El número binario se arma leyendo esos residuos de abajo hacia arriba (el último residuo obtenido es el bit más significativo).

**45 → binario**
```
45 ÷ 2 = 22   residuo 1
22 ÷ 2 = 11   residuo 0
11 ÷ 2 =  5   residuo 1
 5 ÷ 2 =  2   residuo 1
 2 ÷ 2 =  1   residuo 0
 1 ÷ 2 =  0   residuo 1
```
Leyendo los residuos de abajo hacia arriba: **45 = 101101**

**128 → binario**
```
128 ÷ 2 = 64   residuo 0
 64 ÷ 2 = 32   residuo 0
 32 ÷ 2 = 16   residuo 0
 16 ÷ 2 =  8   residuo 0
  8 ÷ 2 =  4   residuo 0
  4 ÷ 2 =  2   residuo 0
  2 ÷ 2 =  1   residuo 0
  1 ÷ 2 =  0   residuo 1
```
Leyendo de abajo hacia arriba: **128 = 10000000**

**255 → binario**
```
255 ÷ 2 = 127   residuo 1
127 ÷ 2 =  63   residuo 1
 63 ÷ 2 =  31   residuo 1
 31 ÷ 2 =  15   residuo 1
 15 ÷ 2 =   7   residuo 1
  7 ÷ 2 =   3   residuo 1
  3 ÷ 2 =   1   residuo 1
  1 ÷ 2 =   0   residuo 1
```
Todas las divisiones dejan residuo 1; tiene sentido, porque 255 = 2⁸ − 1, el mayor número que cabe en 8 bits. Resultado: **255 = 11111111**

**1000 → binario**
```
1000 ÷ 2 = 500   residuo 0
 500 ÷ 2 = 250   residuo 0
 250 ÷ 2 = 125   residuo 0
 125 ÷ 2 =  62   residuo 1
  62 ÷ 2 =  31   residuo 0
  31 ÷ 2 =  15   residuo 1
  15 ÷ 2 =   7   residuo 1
   7 ÷ 2 =   3   residuo 1
   3 ÷ 2 =   1   residuo 1
   1 ÷ 2 =   0   residuo 1
```
Leyendo de abajo hacia arriba: **1000 = 1111101000**

### 1.2 Binario → Decimal

**Método:** cada bit se multiplica por la potencia de 2 correspondiente a su posición (la posición 0 es el bit más a la derecha) y se suman todos esos productos.

**1011**
```
1×2³ + 0×2² + 1×2¹ + 1×2⁰
= 8 + 0 + 2 + 1
= 11
```
**1011 = 11**

**10110110**
```
1×2⁷ + 0×2⁶ + 1×2⁵ + 1×2⁴ + 0×2³ + 1×2² + 1×2¹ + 0×2⁰
= 128 + 0 + 32 + 16 + 0 + 4 + 2 + 0
= 182
```
**10110110 = 182**

**11111111**
```
1×2⁷ + 1×2⁶ + 1×2⁵ + 1×2⁴ + 1×2³ + 1×2² + 1×2¹ + 1×2⁰
= 128 + 64 + 32 + 16 + 8 + 4 + 2 + 1
= 255
```
**11111111 = 255**

**100000000**
```
1×2⁸ + 0×2⁷ + 0×2⁶ + 0×2⁵ + 0×2⁴ + 0×2³ + 0×2² + 0×2¹ + 0×2⁰
= 256 + 0
= 256
```
**100000000 = 256**

### 1.3 Binario → Hexadecimal

**Método:** se agrupan los bits de 4 en 4 (nibbles) de derecha a izquierda, rellenando con ceros a la izquierda si hace falta, y cada grupo se traduce a un dígito hexadecimal según esta tabla de referencia:

| Bin | Hex | Bin | Hex |
|---|---|---|---|
| 0000 | 0 | 1000 | 8 |
| 0001 | 1 | 1001 | 9 |
| 0010 | 2 | 1010 | A |
| 0011 | 3 | 1011 | B |
| 0100 | 4 | 1100 | C |
| 0101 | 5 | 1101 | D |
| 0110 | 6 | 1110 | E |
| 0111 | 7 | 1111 | F |

**10101100**
Se agrupa en dos nibbles: `1010` `1100`
`1010` = A, `1100` = C
→ **10101100 = AC**

**111100001111**
Se agrupa en tres nibbles: `1111` `0000` `1111`
`1111` = F, `0000` = 0, `1111` = F
→ **111100001111 = F0F**

**1010101010101010**
Se agrupa en cuatro nibbles: `1010` `1010` `1010` `1010`
Los cuatro grupos son idénticos: `1010` = A
→ **1010101010101010 = AAAA**

### 1.4 Octal → Decimal

**Método:** cada dígito octal se multiplica por la potencia de 8 correspondiente a su posición y se suman los productos.

**17 (octal)**
```
1×8¹ + 7×8⁰
= 8 + 7
= 15
```
**17₈ = 15**

**254 (octal)**
```
2×8² + 5×8¹ + 4×8⁰
= 128 + 40 + 4
= 172
```
**254₈ = 172**

**777 (octal)**
```
7×8² + 7×8¹ + 7×8⁰
= 448 + 56 + 7
= 511
```
**777₈ = 511**

---

## Nivel 2 — Aritmética binaria básica

### 2.1 Sumas binarias (con acarreo)

**Método:** se suma columna por columna de derecha a izquierda; cuando la suma de una columna da 2 o más, se escribe el bit que sobra (0) y se "arrastra" un acarreo de 1 hacia la columna siguiente. Para que la fila de acarreos quede alineada con las columnas, se antepone un 0 a cada operando (no cambia su valor).

**1011 + 0110**
```
    0 1 1 1 0   <- acarreos
    0 1 0 1 1   (11)
  + 0 0 1 1 0   ( 6)
    ---------
    1 0 0 0 1   (17)
```
Columna por columna (derecha a izquierda): 1+0=1, sin acarreo. 1+1=10 → se escribe 0, acarreo 1. 0+1+1=10 → se escribe 0, acarreo 1. 1+0+1=10 → se escribe 0, acarreo 1. 0+0+1=1 → se escribe 1, sin acarreo de salida. **1011 + 0110 = 10001 (17)**

**11101 + 01011**
```
    0 1 1 1 1 1   <- acarreos
    0 1 1 1 0 1   (29)
  + 0 0 1 0 1 1   (11)
    -----------
    1 0 1 0 0 0   (40)
```
Aquí casi todas las columnas generan acarreo porque hay muchos "1" alineados. **11101 + 01011 = 101000 (40)**

### 2.2 Restas binarias (con préstamo / borrow)

**Método:** se resta columna por columna de derecha a izquierda; cuando el bit de arriba es menor que el de abajo (más el préstamo que venga arrastrado), se "pide prestado" 1 a la columna siguiente, lo que equivale a sumarle 2 a la columna actual y restarle 1 a la próxima columna.

**1100 − 0101**
```
    0 1 1 1   <- prestamos
    1 1 0 0   (12)
  − 0 1 0 1   ( 5)
    -------
    0 1 1 1   (7)
```
bit0 (columna más a la derecha): 0−1 no alcanza → pide prestado, resultado 1, préstamo 1. bit1: 0−0−1(préstamo) no alcanza → pide prestado, resultado 1, préstamo 1. bit2: 1−1−1(préstamo) no alcanza → pide prestado, resultado 1, préstamo 1. bit3: 1−0−1(préstamo) = 0, ya no hace falta pedir prestado (tiene sentido: 12 ≥ 5, no debe quedar préstamo pendiente). **1100 − 0101 = 0111 (7)**

**10000 − 00111**
```
    0 1 1 1 1   <- prestamos
    1 0 0 0 0   (16)
  − 0 0 1 1 1   ( 7)
    ---------
    0 1 0 0 1   (9)
```
Mismo procedimiento columna por columna: se piden préstamos en las primeras cuatro columnas y la última (bit4) cierra sin préstamo pendiente, porque 16 ≥ 7. **10000 − 00111 = 01001 (9)**

### 2.3 Multiplicación binaria

**Método:** se recorre el multiplicador bit por bit (de derecha a izquierda); cada bit en 1 genera un producto parcial igual al multiplicando desplazado esa cantidad de posiciones a la izquierda, y cada bit en 0 genera un producto parcial de puros ceros. Al final se suman todos los productos parciales. Para que todas las filas queden alineadas, aquí se muestran ambos operandos y cada producto parcial extendidos a 8 bits (con ceros a la izquierda; el valor no cambia).

**1011 × 0101** (11 × 5)
```
  0 0 0 0 1 0 1 1   (multiplicando, 11)
x 0 0 0 0 0 1 0 1   (multiplicador,   5)
  ---------------
  0 0 0 0 1 0 1 1   <- bit0=1  (1011 desplazado 0 → 11)
  0 0 0 0 0 0 0 0   <- bit1=0  (0000 desplazado 1 →  0)
  0 0 1 0 1 1 0 0   <- bit2=1  (1011 desplazado 2 → 44)
  0 0 0 0 0 0 0 0   <- bit3=0  (0000 desplazado 3 →  0)
  ---------------
  0 0 1 1 0 1 1 1   (suma de los productos parciales)
```
Sumando los cuatro productos parciales (11 + 0 + 44 + 0 = 55): **1011 × 0101 = 110111 (55)** — coincide con 11 × 5 = 55 ✓.

---

## Nivel 3 — Complemento a dos

### 3.1 Decimal → Complemento a dos (8 bits)

**Método (números negativos):** se escribe la magnitud (valor absoluto) en binario puro de 8 bits, se invierten todos los bits (complemento a uno) y se le suma 1. **Método (números positivos):** se escribe el valor directamente en binario de 8 bits; el bit de signo queda en 0.

**−45**
```
Magnitud (45) en 8 bits:            00101101
Invertido (complemento a uno):      11010010
Sumando 1:                          11010010 + 1 = 11010011
```
**−45 = 11010011**

**−1**
```
Magnitud (1) en 8 bits:             00000001
Invertido (complemento a uno):      11111110
Sumando 1:                          11111110 + 1 = 11111111
```
**−1 = 11111111**

**−128**
Este es el caso límite: en complemento a dos de 8 bits el rango representable va de −128 a 127, así que +128 **no** cabe en 8 bits y el método de "sacar la magnitud, invertir y sumar 1" no se puede aplicar de forma directa (no hay una magnitud positiva de 8 bits de la cual partir). En su lugar se usa la definición de complemento a dos por "peso posicional", donde el bit más significativo vale −2⁷ = −128 y el resto de los bits valen igual que en binario normal:
```
10000000 = (1×−128) + (0×64) + (0×32) + (0×16) + (0×8) + (0×4) + (0×2) + (0×1) = −128
```
*(Comprobación: si a `10000000` se le aplica el método de invertir + sumar 1 de todas formas, se obtiene otra vez `10000000` — es el único patrón de 8 bits que "se niega a sí mismo", justamente porque su verdadero opuesto, +128, no cabe en el registro.)*

**−128 = 10000000**

**27** (positivo)
Por divisiones sucesivas (igual que en el ejercicio 1.1):
```
27 ÷ 2 = 13   residuo 1
13 ÷ 2 =  6   residuo 1
 6 ÷ 2 =  3   residuo 0
 3 ÷ 2 =  1   residuo 1
 1 ÷ 2 =  0   residuo 1
```
Leyendo de abajo hacia arriba: 27 = `11011` en binario puro. Como el número es positivo, esto ya es su representación en complemento a dos; solo falta rellenar a 8 bits con el bit de signo en 0:
**27 = 00011011**

### 3.2 `11010110` (CA2, 8 bits) → Decimal

El bit más significativo es 1, así que el número es negativo. Para hallar su magnitud se invierten los bits y se suma 1: `11010110` → invertido `00101001` → +1 → `00101010` = 42. Por lo tanto:

**`11010110` = −42**

### 3.3 45 − 60 usando complemento a dos (registros de 8 bits)

Restar es sumar el complemento a dos del sustraendo:

```
   45  →  00101101
  −60  →  11000100   (complemento a dos de 60)
  -------------------
          11110001
```

`00101101 + 11000100 = 11110001` (el acarreo de salida se descarta al trabajar en 8 bits).

**Verificación:** `11110001` en CA2 → bit de signo 1 (negativo) → invertir + 1: `00001110` + 1 = `00001111` = 15 → el valor es **−15**.

Efectivamente, 45 − 60 = −15 ✓.

---

## Nivel 4 — Desbordamiento (overflow) y banderas

### 4.1 ¿Hay overflow?

**01111111 + 00000001** (127 + 1, ambos con bit de signo 0 → positivos)

```
  01111111
+ 00000001
----------
  10000000
```
Resultado: `10000000` = −128 en CA2. Los dos operandos son positivos pero el resultado es negativo ⇒ **SÍ hay overflow** (no hay acarreo hacia fuera del registro: carry = 0, pero el signo del resultado es incorrecto).

**10000000 + 11111111** (−128 + (−1), ambos con bit de signo 1 → negativos)

```
  10000000
+ 11111111
----------
 (1)01111111   ← el 9° bit (acarreo de salida) se descarta
```
Resultado (8 bits): `01111111` = +127. Los dos operandos son negativos pero el resultado es positivo ⇒ **SÍ hay overflow** (aquí sí hubo acarreo de salida = 1, pero eso no es lo que indica el overflow; lo indica el cambio de signo).

### 4.2 Carry vs. Overflow

- **Carry (acarreo):** es el bit que "se sale" del registro por la izquierda al sumar. Es relevante para aritmética **sin signo**: indica que el resultado real no cupo en el ancho del registro.
- **Overflow (desbordamiento):** es relevante para aritmética **con signo** (complemento a dos). Ocurre cuando el resultado matemático correcto queda fuera del rango representable, y se detecta cuando **los dos operandos tienen el mismo signo pero el resultado tiene el signo contrario**. El overflow es independiente del carry: puede haber carry sin overflow, y overflow sin carry.

**Ejemplo de carry sin overflow (4 bits):**
```
  1111  (sin signo: 15 | con signo: −1)
+ 0001  (sin signo:  1 | con signo: +1)
------
 10000  → se descarta el 5° bit → 0000
```
Sin signo, hubo carry = 1 (15+1=16 no cabe en 4 bits). Pero interpretando los mismos bits como complemento a dos, es −1 + 1 = 0, que es el resultado correcto: **no hay overflow** (de hecho, cuando los operandos tienen signos distintos, nunca puede haber overflow).

**Ejemplo de overflow sin carry (4 bits):**
```
  0111  (+7)
+ 0001  (+1)
------
  1000  (−8 en CA2)
```
Aquí no hay acarreo de salida (carry = 0, la suma 7+1=8 "cabe" en 4 bits sin pasarse del bit 4), pero el resultado interpretado con signo es −8, lo cual es incorrecto (7+1 debería ser 8, fuera del rango de 4 bits con signo que es −8 a 7): **sí hay overflow**.

### 4.3 Programa en C — overflow con `int8_t`

> El programa está en ```overflow.c```

---

## Nivel 5 — Punto flotante (IEEE 754)

### 5.1 10.25 en IEEE 754 de precisión simple (32 bits)

10.25 en binario es `1010.01`, que en notación científica binaria normalizada es `1.01001 × 2³`.

- **Signo:** 0 (positivo)
- **Exponente:** 3 + 127 (sesgo) = 130 → `10000010`
- **Mantisa:** `01001000000000000000000` (los 23 bits después del "1." implícito)

**Representación completa (32 bits):**
```
0 10000010 01001000000000000000000
```

### 5.2 `1 10000010 01100000000000000000000` → Decimal

- Signo = 1 → número **negativo**
- Exponente = `10000010` = 130 → exponente real = 130 − 127 = 3
- Mantisa = `01100000000000000000000` → valor = 1 + 0·2⁻¹ + 1·2⁻² + 1·2⁻³ = 1 + 0.25 + 0.125 = 1.375

Valor = −(1.375 × 2³) = −(1.375 × 8) = **−11.0**

### 5.3 Pérdida de precisión en punto flotante

El punto flotante representa los números reales con una cantidad **finita** de bits (23 bits de mantisa en precisión simple), mientras que muchos números decimales tienen una representación binaria **infinita y periódica** — igual que 1/3 no se puede escribir de forma exacta con un número finito de decimales. Al redondear esos bits infinitos a los que caben en el formato, se introduce un pequeño error de redondeo.

**Ejemplo clásico:** al sumar `0.1 + 0.2` en punto flotante, el resultado no es exactamente `0.3`, sino:

```
0.30000000000000004
```

Esto ocurre porque ni 0.1 ni 0.2 tienen una representación binaria finita exacta (son fracciones periódicas en base 2, igual que 1/3 lo es en base 10); cada uno ya se almacena con un pequeño error de redondeo, y al sumarlos esos errores se combinan y se hacen visibles en los últimos dígitos.

---

## Nivel 6 — Integración: script de verificación

### 6.1 Script en Python

> El programa está en ```analizar_numero.py```

### 6.2 Reflexión: overflow silencioso en un sensor de 8 bits

Si un sensor de hardware embebido reporta la temperatura como un entero de 8 bits con signo (rango −128 °C a 127 °C) y la temperatura real supera ese límite, el hardware no "avisa" del error: simplemente aplica la aritmética de complemento a dos y el bit sobrante se pierde, tal como se ve en el caso de 128 en las pruebas del script (que se representa igual que un número fuera de rango y se marca como overflow). Por ejemplo, si la temperatura real fuera 130 °C, el sistema podría leer el valor equivalente a −126 °C en el registro de 8 bits, porque 130 se desborda del rango [−128, 127] y "da la vuelta" hacia el extremo negativo — el mismo fenómeno que se vio con `120 + 20` en el programa de C, que dio −116 en lugar de 140.

Esto es peligroso porque el sistema no distingue entre "el sensor midió efectivamente un valor negativo" y "el sensor midió un valor positivo tan grande que se desbordó". En un sistema embebido crítico (por ejemplo, el control térmico de un motor, un reactor, o un sistema de frenado), un salto repentino de una lectura alta y creciente (127 °C) a una lectura negativa (−126 °C) podría interpretarse como que la temperatura *cayó* drásticamente, cuando en realidad *subió* peligrosamente. Si el software de control reacciona a esa lectura errónea —por ejemplo, desactivando una alarma de sobrecalentamiento o incluso activando un sistema de calefacción pensando que hace demasiado frío— las consecuencias podrían ser graves. Por eso, en sistemas reales es fundamental usar un tipo de dato con rango suficiente para los valores esperados, o bien validar/saturar (*clamping*) los valores en los límites en vez de dejar que el overflow ocurra silenciosamente.
