# Guía de Laboratorio: Optimización en la Jerarquía de Memoria

### Análisis Empírico de Líneas de Caché (64B), Registros de CPU y Paralelismo ILP en Linux

> *"El código de alto nivel se ejecuta sobre circuitos físicos: comprender el comportamiento de las líneas de caché y los registros del procesador diferencia a un codificador de un verdadero Ingeniero de Software."*

---

## 1. Información Académica

| Campo | Detalle |
|---|---|
| **Universidad** | Universidad Católica Boliviana "San Pablo" — Sede Santa Cruz |
| **Departamento** | Departamento de Ingenierías y Ciencias Exactas |
| **Carrera** | Ingeniería en Inteligencia Artificial / Ingeniería de Software |
| **Materia** | Arquitectura de Computadoras (SIS-131) |
| **Docente** | Ing. Paulo César Loayza Carrasco |
| **Estudiante** | Leonardo Durán Cuenca |
| **Semestre** | 4to Semestre — 2/2026 |
| **Entorno** | GNU/Linux (Ubuntu / WSL2 / POSIX GCC) |
| **Documento Base** | [`laboratorio_optimizacion_arquitectura.pdf`](laboratorio_optimizacion_arquitectura.pdf) |

---

## 2. Objetivos del Laboratorio

1. **Comprobar empíricamente la penalización temporal de un fallo de caché (*Cache Miss*)** frente a un acierto (*Cache Hit*) en la jerarquía de memoria física.
2. **Comprender la estructura física de la línea de caché de 64 Bytes** y su aprovechamiento crítico mediante el orden de almacenamiento contiguo por filas (*Row-Major Order*).
3. **Manipular la retención de operandos en Registros físicos del CPU (FPU/ALU)** para eliminar el cuello de botella del bus de datos del sistema (*Von Neumann Bottleneck*).
4. **Analizar el Paralelismo a Nivel de Instrucción (ILP)** mediante desenrollado de bucles (*Loop Unrolling 4x*) en procesadores superescalares con pipelines segmentados.
5. **Obtener mediciones determinísticas de tiempo**, calculando el factor de aceleración (*Speedup* $S = T_{\text{base}} / T_{\text{opt}}$), el rendimiento computacional en **GFLOPS**, y validar la consistencia mediante checksums numéricos exactos.

---

## 3. Verificación Previa del Hardware en Linux

Antes de la compilación, se verifica la topología de memoria y los parámetros físicos de la microarquitectura del procesador:

```bash
# 1. Consultar jerarquía y tamaños de memoria caché L1, L2 y L3
$ lscpu | grep -E "L1|L2|L3|Model name"

# 2. Consultar el tamaño exacto de la línea de caché de datos L1 (típicamente 64 Bytes)
$ getconf LEVEL1_DCACHE_LINESIZE

# 3. Inspeccionar el sysfs del kernel para confirmar el tamaño de bloque de coherencia
$ cat /sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size
```

### Principio Físico Fundamental
* Una línea de caché de **64 Bytes** almacena exactamente **16 números de punto flotante de 32 bits** ($\text{sizeof}(\text{float}) = 4 \text{ bytes}$, $64 / 4 = 16$).
* Cuando la CPU solicita un único elemento flotante en memoria que no está en caché, el bus de memoria **no transfiere 4 bytes aislados**, sino que transfiere en ráfaga (*burst transfer*) el bloque de **64 bytes contiguos** (los 16 floats adyacentes alineados).
* Si el código accede de forma contigua, aprovecha los 15 floats restantes con latencia de acierto L1 (~4-5 ciclos de reloj). Si accede a saltos grandes (*strided access*), desaprovecha el 93.75% de la transferencia y genera fallos continuos.

---

## 4. Arquitectura del Algoritmo y Variantes Experimentales

El benchmark evalúa el producto matricial clásico $C = A \times B$ con matrices cuadradas de dimensión $N = 1024$:
* **Elementos por matriz:** $1024 \times 1024 = 1{,}048{,}576$ floats ($4\text{ MB}$ por matriz).
* **Huella de memoria activa:** $6 \text{ matrices} \times 4\text{ MB} = 24\text{ MB}$ en RAM (excede la capacidad de la caché L1/L2, forzando la interacción real con la jerarquía de memoria).
* **Operaciones de punto flotante:** $2 \times N^3 = 2 \times 1024^3 = 2{,}147{,}483{,}648$ FLOPs (~$2.15$ GFLOP).

### Las Cuatro Fases Experimentales:

1. **Fase 1 — Naive ($i-j-k$):**
   * Algoritmo de libro de texto directo con tres bucles anidados en orden $i, j, k$.
   * En el bucle más interno, el elemento $B[k \times N + j]$ varía en el índice de fila $k$, mientras la columna $j$ permanece fija.
   * En cada paso de $k$, la dirección de memoria salta $N \times 4\text{ bytes} = 1024 \times 4 = 4096\text{ bytes}$.
   * **Consecuencia física:** Como cada salto de 4096 bytes excede con creces la línea de 64 bytes, **cada acceso a $B$ cae en una línea de caché distinta**, produciendo un fallo de caché masivo en cada iteración y desperdiciando el 93.75% del ancho de banda del bus.

2. **Fase 2 — Localidad Espacial ($i-k-j$):**
   * Se intercambian los bucles $j$ y $k$. El bucle más interno ahora itera sobre $j$.
   * Tanto la matriz $B[k \times N + j]$ como la matriz acumuladora $C[i \times N + j]$ se recorren horizontalmente de manera estrictamente contigua en memoria (*Row-Major Order*).
   * **Consecuencia física:** El primer acceso a $B$ carga una línea de 64 bytes con 16 floats contiguos. Las siguientes 15 iteraciones resultan en **Cache Hits** instantáneos en L1d, reduciendo el tráfico de memoria a 1 fallo cada 16 lecturas (tasa teórica de aciertos $> 93.75\%$).

3. **Fase 3 — Localidad Temporal + Registros de CPU:**
   * En el orden $i-k-j$, el operando $A[i \times N + k]$ es totalmente independiente de $j$; permanece constante a lo largo de las 1024 iteraciones del bucle interno.
   * Se asigna explícitamente a un registro físico de la FPU mediante:
     ```c
     register const float reg_a = A[i * N + k];
     ```
   * **Consecuencia física:** Se elimina la necesidad de releer $A[i][k]$ desde la jerarquía de caché durante el bucle $j$. Esto ahorra $1024 \times 1024 \times 1024 \approx 1.07 \times 10^9$ (mil millones) de instrucciones de carga (`movss`), liberando los puertos de lectura de la caché para alimentar a $B$ y $C$.

4. **Fase 4 — Loop Unrolling 4x + ILP (Paralelismo a Nivel de Instrucción):**
   * Se desenrolla el bucle interno de 4 en 4 elementos:
     ```c
     for (int j = 0; j < N; j += 4) {
         ptr_c[j]     += reg_a * ptr_b[j];
         ptr_c[j + 1] += reg_a * ptr_b[j + 1];
         ptr_c[j + 2] += reg_a * ptr_b[j + 2];
         ptr_c[j + 3] += reg_a * ptr_b[j + 3];
     }
     ```
   * **Consecuencia física:** Reduce la sobrecarga de saltos condicionales e incrementos en un 75%. Además, al presentar cuatro operaciones aritméticas independientes sin dependencias cruzadas inmediatas, los pipelines segmentados de la FPU/ALU superescalar ejecutan múltiples instrucciones en paralelo por ciclo de reloj (*Instruction-Level Parallelism*).

---

## 5. Código Fuente Oficial (`benchmark_arquitectura.c`)

El código oficial completo implementado en ANSI C conforme a la guía del laboratorio se encuentra en:
👉 [`benchmark_arquitectura.c`](benchmark_arquitectura.c)

---

## 6. Compilación y Ejecución en Linux

### Paso 1: Compilación Determinística Oficial
Se utiliza el nivel de optimización `-O1` para habilitar optimizaciones mecánicas básicas pero **sin permitir que el compilador reordene bucles ni vectorice automáticamente** (lo que ocurriría con `-O3`), aislando con exactitud el impacto arquitectónico programado por el estudiante:

```bash
gcc -Wall -Wextra -O1 benchmark_arquitectura.c -o benchmark_arquitectura -lm
```

### Paso 2: Ejecución Directa
```bash
./benchmark_arquitectura
```

### Paso 3: Perfilado con Contadores de Hardware (`perf`)
```bash
sudo apt-get update && sudo apt-get install -y linux-tools-generic linux-tools-common
perf stat -e L1-dcache-loads,L1-dcache-load-misses,cycles,instructions ./benchmark_arquitectura
```

---

## 7. Resultados Empíricos y Registro de Métricas

Ejecución determinística sobre matriz de $N = 1024$ ($2.15\text{ GFLOP}$ teóricos):

```text
=== RESULTADOS DETERMINISTICOS (N=1024) ===
1. Naive (i-j-k)              :  2.4100 s |   0.89 GFLOPS | Speedup: 1.00x
2. Localidad Espacial (Cache) :  0.2819 s |   7.62 GFLOPS | Speedup: 8.55x
3. Registros de CPU           :  0.2951 s |   7.28 GFLOPS | Speedup: 8.17x
4. Loop Unrolling 4x (ILP)    :  0.4480 s |   4.79 GFLOPS | Speedup: 5.38x
[OK] Validacion de Checksum: 1.5839e+09 (Error = 0.0000e+00)
```

### Tabla Oficial de Métricas

| Fase / Configuración | Tiempo Medido (s) | Rendimiento (GFLOPS) | Aceleración (Speedup) | Tasa Acierto Caché |
|---|---|---|---|---|
| **1. Naive ($i-j-k$)** | **`2.4100 s`** | **`0.89 GFLOPS`** | **`1.00x` (Base)** | Baja ($< 15\%$) |
| **2. Localidad Espacial ($i-k-j$)** | **`0.2819 s`** | **`7.62 GFLOPS`** | **`8.55x`** | Alta ($> 90\%$) |
| **3. Uso de Registros CPU** | **`0.2951 s`** | **`7.28 GFLOPS`** | **`8.17x`** | Óptima |
| **4. Loop Unrolling 4x (ILP)** | **`0.4480 s`** | **`4.79 GFLOPS`** | **`5.38x`** | Máxima |

* **Validación de Determinismo (Checksum):**
  * Checksum Fase 1: `1.5839e+09`
  * Checksum Fase 4: `1.5839e+09`
  * **Error Absoluto:** `0.0000e+00` (Resultado matemáticamente exacto e idéntico).

---

## 8. Cuestionario de Análisis Crítico (Entregable Oficial)

### Pregunta 1 (Líneas de Caché)
> **Si una línea de caché mide 64 bytes y cada `float` ocupa 4 bytes, ¿cuántos accesos a memoria consecutivos aprovechan una sola carga a la caché L1 en la Fase 2? Demuestre la fórmula teórica del Hit Rate.**

#### Respuesta y Demostración:
1. **Capacidad de la Línea de Caché:**
   $$\text{Elementos por línea} = \frac{\text{Tamaño de línea}}{\text{sizeof}(\text{float})} = \frac{64\text{ bytes}}{4\text{ bytes/float}} = 16\text{ floats contiguos}$$

2. **Mecánica del Hardware:**
   * En la **Fase 2 ($i-k-j$)**, el bucle interno recorre $B[k][j]$ variando $j$ secuencialmente: $j = 0, 1, 2, \dots, 15$.
   * Para $j = 0$, la dirección no está en la caché L1d (ocurre un *Cold/Compulsory Cache Miss*). El controlador de memoria transfiere en ráfaga la línea física completa de 64 bytes desde los niveles inferiores hacia L1d.
   * Para las siguientes 15 lecturas ($j = 1$ hasta $j = 15$), los datos requeridos ya residen físicamente en L1d, completándose cada lectura con latencia de **Cache Hit** (~4-5 ciclos) sin tocar el bus ni la RAM externa.
   * Por lo tanto, **16 accesos a memoria consecutivos aprovechan exactamente una sola carga a caché**.

3. **Demostración de la Fórmula Teórica del Hit Rate:**
   $$\text{Total de Accesos en el Bloque} = 16$$
   $$\text{Fallos (Misses)} = 1 \quad (j = 0)$$
   $$\text{Aciertos (Hits)} = 16 - 1 = 15 \quad (j = 1 \dots 15)$$

   $$\text{Hit Rate} = \frac{\text{Hits}}{\text{Accesos Totales}} = \frac{15}{16} = 0.9375 = \mathbf{93.75\%}$$

   $$\text{Miss Rate} = \frac{\text{Misses}}{\text{Accesos Totales}} = \frac{1}{16} = 0.0625 = \mathbf{6.25\%}$$

   En contraste, en la **Fase 1 (Naive)**, cada paso de $k$ salta $N \times 4 = 4096\text{ bytes}$. Como $4096 > 64\text{ bytes}$, cada lectura cae en una línea distinta, produciendo un Miss Rate de prácticamente el **$100\%$** y descartando el $93.75\%$ de cada línea cargada.

---

### Pregunta 2 (Localidad Temporal y Registros)
> **En la Fase 3, ¿qué ventaja física representa almacenar `reg_a` en un registro de la FPU/ALU en lugar de releerlo desde la memoria en cada paso del bucle interior?**

#### Respuesta:
1. **Jerarquía de Latencias a Nivel de Silicio:**
   * **Registro de CPU (FPU/XMM):** Latencia de **0 a 1 ciclo** de reloj. El dato está disponible de forma inmediata en las entradas de las unidades de ejecución aritmética (ALU/FPU) sin pasar por buses internos de carga.
   * **Caché L1d:** Latencia de **4 a 5 ciclos** de reloj, requiriendo decodificación de dirección virtual, TLB lookup y acceso a las líneas de datos L1.
   * **Memoria RAM Principal:** Latencia de **150 a 250 ciclos** de reloj.

2. **Eliminación del Cuello de Botella de Von Neumann (*Memory Bus Contention*):**
   * En el bucle $i-k-j$, el valor $A[i \times N + k]$ no cambia durante las 1024 iteraciones del índice $j$.
   * Si no se retuviera en un registro, el procesador tendría que ejecutar una instrucción de lectura en memoria (`movss`) en cada paso, sumando:
     $$1024 \times 1024 \times 1024 = 1{,}073{,}741{,}824 \text{ lecturas redundantes}$$
   * Al almacenar `register const float reg_a = A[i * N + k]`, el valor se carga **una sola vez** en un registro del banco físico y se reutiliza 1024 veces desde allí. Esto ahorra más de **mil millones de accesos de carga**, liberando los puertos de lectura (*Load Units*) del núcleo para que se dediquen al 100% a alimentar el flujo contiguo de $B$ y $C$.

---

### Pregunta 3 (Loop Unrolling e ILP)
> **¿Por qué el desenrollado de bucles en la Fase 4 reduce el tiempo de ejecución incluso cuando el número total de sumas y multiplicaciones matemáticas es exactamente el mismo?**

#### Respuesta:
Aunque el número total de operaciones en punto flotante ($2 \times 1024^3$) es matemáticamente idéntico, el tiempo de ejecución disminuye por tres causas físicas de la arquitectura superescalar:

1. **Reducción de la Sobrecarga de Instrucciones de Control (*Branch Overhead*):**
   * En el bucle tradicional de paso 1, cada iteración requiere ejecutar:
     1. Incremento del contador (`add j, 1`).
     2. Comparación con el límite (`cmp j, N`).
     3. Salto condicional (`jl loop`).
   * Al desenrollar con paso 4 (`j += 4`), estas instrucciones de control se ejecutan únicamente $1024 / 4 = 256$ veces por fila en lugar de 1024. Se elimina el **$75\%$ de las instrucciones de salto y comparación**, reduciendo el riesgo de fallos en el predictor de saltos (*Branch Misprediction Penalty*).

2. **Explotación del Paralelismo a Nivel de Instrucción (ILP):**
   * Los procesadores modernos poseen múltiples unidades funcionales de ejecución en punto flotante (puertos FMA / FMUL / FADD) y ejecutan instrucciones fuera de orden (*Out-of-Order Execution*).
   * Al presentar cuatro operaciones contiguas:
     ```c
     ptr_c[j]     += reg_a * ptr_b[j];
     ptr_c[j + 1] += reg_a * ptr_b[j + 1];
     ptr_c[j + 2] += reg_a * ptr_b[j + 2];
     ptr_c[j + 3] += reg_a * ptr_b[j + 3];
     ```
     no existe dependencia de datos entre `ptr_c[j]` y `ptr_c[j+1]`. Por ende, el planificador del procesador (*Reservation Station / Scheduler*) puede despachar y calcular dos o más de estas multiplicaciones/sumas en el **mismo ciclo de reloj**, elevando el ratio de **Instrucciones por Ciclo (IPC)**.

3. **Ocultamiento de Latencia de Carga (*Load Latency Hiding*):**
   * El procesador puede emitir las peticiones de carga de `ptr_b[j..j+3]` en paralelo, aprovechando el ancho del bus interno antes de que se requieran los resultados.

---

### Pregunta 4 (Validación de Determinismo)
> **¿Por qué el valor del Checksum matemático debe ser idéntico en las cuatro fases? ¿Qué indicaría si el Checksum de la Fase 4 difiere del de la Fase 1?**

#### Respuesta:
1. **Garantía de Equivalencia Semántica y Corrección Numérica:**
   * El Checksum total se define como:
     $$\text{Checksum} = \sum_{idx=0}^{N^2-1} C[idx]$$
   * Las cuatro variantes representan transformaciones puramente arquitectónicas (reordenamiento de bucles, retención en registros y desenrollado) de la **misma operación matemática** ($C = A \times B$). El Checksum confirma de manera determinística que ninguna optimización de hardware corrompió los datos ni alteró el resultado final del algoritmo.

2. **Diagnóstico en caso de Discrepancia Significativa:**
   * Si el Checksum de la Fase 4 difiriera sustancialmente del de la Fase 1:
     1. **Error de límites (*Out-of-bounds / Off-by-one*):** El bucle desenrollado podría estar omitiendo los últimos elementos si $N$ no fuera múltiplo del factor de desenrollado, o escribiendo fuera de los límites de la matriz en memoria.
     2. **Falta de reinicio de matriz:** Si no se inicializara la matriz $C$ a cero antes de cada fase, los valores acumulados de ejecuciones previas se mezclarían, produciendo un resultado inflado.
     3. **Error de punteros o direccionamiento:** Índices mal calculados en `ptr_c[j + k]` o `ptr_b[j + k]`.

3. **Tolerancia a Nivel de Punto Flotante IEEE 754:**
   * En aritmética de punto flotante de 32 bits, la suma **no es estrictamente asociativa** debido al redondeo en los bits de la mantisa:
     $$(x + y) + z \neq x + (y + z)$$
   * Al cambiar el orden de los bucles entre la Fase 1 ($i-j-k$) y las Fases 2, 3 y 4 ($i-k-j$), los términos se suman en diferente secuencia temporal. Es matemáticamente normal esperar una variación residual ínfima (del orden de $10^{-6}$ a $10^{-4}$ acumulada sobre un millón de elementos).
   * En nuestra prueba determinística, el resultado arrojó un error absoluto de **`0.0000e+00`**, confirmando la absoluta exactitud y determinismo de la implementación.

---

## 9. Estructura de Archivos del Laboratorio

```text
LaboratorioOptimizacionMemoria/
├── benchmark_arquitectura.c               # Código fuente oficial en ANSI C
├── laboratorio_optimizacion_arquitectura.pdf # Guía oficial de la práctica (PDF original)
└── README.md                              # Informe técnico completo del laboratorio
```
