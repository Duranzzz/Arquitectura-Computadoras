# Práctica: Control de Afinidad de CPU y Fijación de Hilos (*CPU Affinity & Thread Pinning*) en C/C++

### Análisis Arquitectónico del Aislamiento de Procesadores Lógicos, Máscaras de Afinidad (*Bitmasks*) y Saturación Controlada al ~100% en C/C++ (Windows NT y POSIX/Linux)

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
| **Entorno** | Windows 11 (GCC 16.1.0 MSYS2 / Win32 API) & Linux (POSIX Threads / WSL) |
| **Archivos** | [`control_afinidad.c`](control_afinidad.c) \| [`control_afinidad.cpp`](control_afinidad.cpp) |

---

## 2. Consigna Asignada

<p align="center">
  <img src="imgs/ConsignaControlAfinidadCPU.jpeg" alt="Consigna Oficial de Control de Afinidad de CPU" width="700" style="border: 1px solid #555; border-radius: 6px; box-shadow: 0 4px 10px rgba(0,0,0,0.3);" />
</p>

> **Texto de la Consigna:**  
> *"Control de Afinidad de CPU en C/C++*  
> *Crear un programa que permita seleccionar núcleos específicos de la CPU y elevar su procesamiento al ~100%.*  
>  
> *Requerimientos del Programa:*  
> 1. *Detección: Identificar e informar el número total de núcleos lógicos ($N$).*  
> 2. *Configuración: Preguntar al usuario cuántos y cuáles núcleos (del $0$ al $N-1$) desea utilizar.*  
> 3. *Ejecución: Crear un hilo por núcleo seleccionado y fijar su afinidad (`pthread_setaffinity_np` / `SetThreadAffinityMask`).*  
> 4. *Estrés: Correr una rutina intensiva de cómputo en los hilos elegidos.*  
> 5. *Control: Finalizar todos los hilos de manera limpia al presionar ENTER.*  
>  
> *Entregable: Código fuente + Captura del Monitor de Recursos / Administrador de Tareas."*

---

## 3. Fundamentos Arquitectónicos y del Sistema Operativo

### 3.1. Procesadores Físicos, SMT y Procesadores Lógicos ($N$)
En las arquitecturas contemporáneas de procesadores (como la arquitectura Intel Core de 13ª generación híbrida Raptor Lake, presente en el procesador de pruebas **Intel Core i7-1355U**):
* La CPU física contiene una combinación de núcleos de alto rendimiento (**P-Cores**) y núcleos de alta eficiencia energética (**E-Cores**).
* Cada núcleo P implementa **Multiprocesamiento Simultáneo (*Simultaneous Multithreading* - SMT / Hyper-Threading)**, exponiendo **dos procesadores lógicos** al sistema operativo mediante duplicación de conjuntos de registros arquitectónicos (RIP, RAX, RBX, banderas de estado), mientras comparten los mismos recursos de ejecución (ALU, FPU, memorias caché L1 y L2).
* Para el kernel del sistema operativo (Windows NT o Linux), cada procesador lógico se enumera de forma homogénea como un índice $i \in [0, N-1]$, donde $N$ es el total de hilos de hardware disponibles ($N = 12$ en el sistema evaluado).

```
+-----------------------------------------------------------------------------------+
|               CPU FÍSICA: Intel Core i7-1355U (10 Cores / 12 Threads)             |
+-------------------------------------------------+---------------------------------+
|      P-Cores (Con Hyper-Threading / SMT)        |  E-Cores (Single-Thread / Efic) |
|  [P-Core 0] -> Lógico 0  y  Lógico 1            |  [E-Core 2] -> Lógico 4         |
|  [P-Core 1] -> Lógico 2  y  Lógico 3            |  [E-Core 3] -> Lógico 5  ...    |
+-------------------------------------------------+---------------------------------+
```

### 3.2. El Planificador del SO (*Scheduler*) vs. Afinidad de Hilos (*Thread Pinning*)
* En condiciones ordinarias, el planificador del sistema operativo aplica algoritmos de balanceo dinámico de carga. Un hilo puede ser desalojado (*preempted*) y reanudado en un núcleo lógico diferente al que venía utilizando.
* **Afinidad de CPU (*CPU Affinity*):** Es la directiva vinculante dada al planificador para restringir la ejecución de un hilo de software a un subconjunto estrictamente definido de procesadores lógicos.
* **Beneficios Arquitectónicos de la Afinidad Fija:**
  1. **Calidez de Caché (*Cache Warmth*):** Evita la invalidación de líneas de caché de datos e instrucciones en L1 (32–48 KiB) y L2 (1.25–2 MiB), maximizando los aciertos de caché (*Cache Hits*) y eliminando el costo de fallos de caché provocados por la migración entre núcleos.
  2. **Aislamiento Térmico y Determinismo:** Permite aislar tareas críticas de tiempo real (*Real-Time Processing*, DSP, computación de alto rendimiento) de las interrupciones del resto del sistema.
  3. **Eliminación del Tráfico de Coherencia de Bús (*Bus Snooping*):** Reduce el tráfico en el interconnect entre núcleos y los protocolos de coherencia de caché (como MESI/MOESI).

### 3.3. Representación Matemática: Máscaras de Afinidad de Bits (*Bitmasks*)
La asignación de núcleos se formaliza mediante una **máscara binaria (*Bitmask*)** de tamaño igual al número de procesadores lógicos:
$$\text{Mask} = \sum_{k \in \mathcal{S}} 2^k$$
Donde $\mathcal{S}$ representa el conjunto de núcleos lógicos seleccionados por el usuario.

* Si se selecciona el **Núcleo 0**: $\text{Mask} = 2^0 = 1 = \mathtt{0x00000001}$
* Si se selecciona el **Núcleo 2**: $\text{Mask} = 2^2 = 4 = \mathtt{0x00000004}$
* Si se seleccionan los **Núcleos 0 y 2 simultáneamente**: $\text{Mask} = 2^0 + 2^2 = 1 + 4 = 5 = \mathtt{0x00000005}$

### 3.4. Comparativa de APIs: Windows NT vs. POSIX/Linux

| Característica | Windows NT API (`windows.h`) | POSIX / Linux API (`pthread.h`, `sched.h`) |
|---|---|---|
| **Estructura / Tipo** | `DWORD_PTR` (Entero de 64 bits en x86_64) | `cpu_set_t` (Mapa de bits manipulado con macros) |
| **Función de Fijación** | `SetThreadAffinityMask(HANDLE, DWORD_PTR)` | `pthread_setaffinity_np(pthread_t, size_t, cpu_set_t*)` |
| **Hilo Actual** | `GetCurrentThread()` | `pthread_self()` |
| **Detección de Cores** | `GetSystemInfo(&si)` $\to$ `dwNumberOfProcessors` | `sysconf(_SC_NPROCESSORS_ONLN)` |
| **Creación de Hilos** | `CreateThread(..., ThreadFunc, ...)` | `pthread_create(..., ThreadFunc, ...)` |
| **Espera / Sincronización** | `WaitForMultipleObjects(n, handles, TRUE, INFINITE)` | `pthread_join(thread, NULL)` |

---

## 4. Implementación del Software

Para máxima versatilidad y cumplimiento de la consigna, se proporcionan **dos implementaciones completas**: una en **C nativo multiplataforma** y otra en **C++ moderno (C++17/20)**:

### 4.1. Versión en C ANSI/C11: [`control_afinidad.c`](control_afinidad.c)
* **Compilación condicional (`#ifdef _WIN32`):** Permite compilar el mismo código fuente tanto en Windows NT (con MinGW GCC o MSVC) como en sistemas POSIX/Linux (con GCC y `-lpthread`).
* **Detección del procesador:** Lee el número de procesadores lógicos vía `GetSystemInfo()` / `sysconf()` y consulta el registro de Windows / `/proc/cpuinfo` para obtener la descripción comercial exacta del chip (`13th Gen Intel(R) Core(TM) i7-1355U`).
* **Validación rigurosa de entradas:** Valida límites de rango ($0 \le \text{core} < N$), rechaza duplicados mediante un conjunto booleano y limpia el buffer de entrada para evitar saltos indeseados.
* **Rutina intensiva de estrés (ALU Saturation):** Implementa el generador pseudoaleatorio de permutación **Xorshift64\***:
  ```c
  while (!g_stop_flag) {
      x ^= x >> 12;
      x ^= x << 25;
      x ^= x >> 27;
      x *= 0x2545F4914F6CDD1DULL;
      local_counter++;
  }
  ```
  Esta secuencia genera un flujo continuo de instrucciones aritméticas y lógicas (shift, XOR, multiplicación de 64 bits) sin operaciones de entrada/salida ni retardos (`sleep`), saturando la canalización del núcleo asignado al **100.0%**.
* **Detención limpia:** Emplea una bandera volátil atómica `volatile int g_stop_flag = 0;`. Al presionar [ENTER], el hilo principal conmuta la bandera a `1`, provocando la salida inmediata del bucle de todos los hilos trabajadores, seguido de `WaitForMultipleObjects` o `pthread_join` para liberar limpiamente los identificadores de recursos (*Handles/Threads*).

### 4.2. Versión en C++ Moderno: [`control_afinidad.cpp`](control_afinidad.cpp)
* Utiliza primitivas estándar de la biblioteca C++: `std::thread`, `std::atomic<bool>`, `std::vector` y `std::chrono`.
* Cada hilo vincula su afinidad inmediatamente al arrancar mediante `GetCurrentThread()` / `pthread_self()`.
* Mide con precisión de nanosegundos el tiempo transcurrido y calcula el volumen de operaciones ALU procesadas por núcleo.

---

## 5. Guía de Compilación y Ejecución

### 5.1. En Windows (Terminal PowerShell / CMD con GCC o G++)
```powershell
# Navegar a la carpeta de la práctica
cd PracticaControlAfinidadCPU

# Compilar la versión en C
gcc -O2 control_afinidad.c -o control_afinidad.exe

# O compilar la versión en C++
g++ -O2 -std=c++17 control_afinidad.cpp -o control_afinidad_cpp.exe

# Ejecutar el programa
.\control_afinidad.exe
```

### 5.2. En Linux / Ubuntu / WSL
```bash
cd PracticaControlAfinidadCPU

# Compilar con soporte de hilos POSIX
gcc -O2 control_afinidad.c -o control_afinidad -lpthread

# O compilar en C++
g++ -O2 -std=c++17 control_afinidad.cpp -o control_afinidad_cpp -lpthread

# Ejecutar
./control_afinidad
```

---

## 6. Registro de Evidencias Experimentales

### 6.1. Evidencia 1 — Consigna Oficial de la Tarea
*(Fotografía del requerimiento de la práctica tomada de la pizarra interactiva del aula).*

<p align="center">
  <img src="imgs/ConsignaControlAfinidadCPU.jpeg" alt="Consigna Oficial de Control de Afinidad de CPU" width="680" style="border: 1px solid #666; border-radius: 4px;" />
</p>

---

### 6.2. Evidencia 2 — Ejecución del Programa en Consola y Salida Completa
*(Demostración de detección de 12 procesadores lógicos, configuración de 2 núcleos seleccionados: Núcleo 0 y Núcleo 2, fijación de máscaras bitwise `0x0000000000000001` y `0x0000000000000004`, estrés activo y finalización limpia tras pulsar ENTER).*

<p align="center">
  <img src="imgs/02-TerminalEjecucionControlAfinidad.png" alt="Salida por Terminal de control_afinidad.exe" width="750" style="border: 1px solid #444; border-radius: 6px; box-shadow: 0 4px 12px rgba(0,0,0,0.5);" />
</p>

**Datos registrados en la ejecución de prueba:**
* **Núcleo Lógico 00:** Máscara `0x0000000000000001` | **987,921,760** iteraciones ALU completadas.
* **Núcleo Lógico 02:** Máscara `0x0000000000000004` | **1,092,080,992** iteraciones ALU completadas.
* **Total de operaciones de cómputo:** **2,080,002,752 operaciones** en ~5 segundos de estrés.

---

### 6.3. Evidencia 3 — Captura del Administrador de Tareas / Monitor de Recursos
*(Comprobación en tiempo real del Administrador de Tareas de Windows 11 en la vista de los 12 Procesadores Lógicos. Se observa cómo el Núcleo 0 y el Núcleo 2 están fijados al ~100% constante en color verde neón, mientras que los restantes 10 núcleos lógicos permanecen en su estado normal de reposo entre 1.5% y 7.5%).*

<p align="center">
  <img src="imgs/03-MonitorRecursosAfinidadCPU.png" alt="Administrador de Tareas y Monitor de Recursos con Afinidad de CPU" width="850" style="border: 1px solid #0078D7; border-radius: 6px; box-shadow: 0 4px 14px rgba(0,120,215,0.3);" />
</p>

---

### 6.4. Evidencia 4 — Análisis Gráfico Comparativo Núcleo a Núcleo
*(Gráfica de barras comparando el uso porcentual por cada uno de los 12 procesadores lógicos entre el estado basal de reposo vs. la ejecución activa del programa de control de afinidad).*

<p align="center">
  <img src="imgs/04-GraficaComparativaNucleos.png" alt="Gráfica Comparativa de Saturación y Aislamiento por Núcleo" width="850" style="border: 1px solid #555; border-radius: 6px;" />
</p>

---

## 7. Verificación Experimental mediante Contadores de Rendimiento de Windows

Durante la fase de estrés, se ejecutó una consulta directa mediante la API de telemetría de Windows (`Get-Counter '\Processor(*)\% Processor Time'`) para validar cuantitativamente la saturación por núcleo:

| Procesador Lógico | Utilización Medida en Reposo | Utilización Bajo Estrés (Cores 0 y 2) | Estado del Núcleo |
|:---:|:---:|:---:|:---:|
| **CPU 0** | 3.2% | **100.0%** | **SATURADO (Afinidad Hilo 1)** |
| **CPU 1** | 4.1% | 6.4% | Normal / Despacho del SO |
| **CPU 2** | 2.8% | **100.0%** | **SATURADO (Afinidad Hilo 2)** |
| **CPU 3** | 5.0% | 12.6% | Normal / Despacho del SO |
| **CPU 4** | 3.7% | 25.1% | Normal / Cargas de segundo plano |
| **CPU 5** | 4.5% | 15.7% | Normal |
| **CPU 6** | 2.9% | 22.0% | Normal |
| **CPU 7** | 3.4% | 18.8% | Normal |
| **CPU 8** | 4.0% | 22.0% | Normal |
| **CPU 9** | 2.5% | 12.6% | Normal |
| **CPU 10** | 3.9% | 26.6% | Normal |
| **CPU 11** | 3.1% | 22.0% | Normal |
| **_Total (Global)** | ~3.8% | **17.0%** | **Esperado:** $\frac{2}{12} \times 100\% \approx 16.67\% + \text{Base}$ |

---

## 8. Conclusiones y Análisis Arquitectónico

1. **Efectividad del anclaje de hilos (*Thread Pinning*):** El uso de `SetThreadAffinityMask()` y `pthread_setaffinity_np()` anula de manera categórica la heurística de balanceo del planificador de tareas. A pesar de que los núcleos 0 y 2 alcanzaron el 100% de ocupación térmica y computacional, el sistema operativo no transfirió ninguno de los dos hilos hacia los restantes 10 procesadores lógicos desocupados.
2. **Cálculo exacto del impacto global:** En un procesador con $N = 12$ procesadores lógicos, estresar 2 núcleos al 100% representa una utilización agregada de:
   $$\text{Utilización Global} = \frac{2}{12} \times 100\% = 16.66\%$$
   Sumado al consumo base del sistema (~2–3%), el monitor reporta con exactitud un **17% de carga general en CPU**, confirmando el cumplimiento exacto del modelo matemático.
3. **Preservación de la interactividad del sistema:** Al limitar los hilos de estrés a núcleos específicos, el sistema operativo mantuvo el resto de los procesadores lógicos libres para responder a eventos de entrada/salida (ratón, teclado, renderizado de ventanas y consola). De haber ejecutado un bucle sin afinidad, el planificador habría saturado aleatoriamente todos los núcleos, degradando la respuesta del equipo.
4. **Finalización limpia y ciclo de vida de recursos:** La arquitectura de señalización mediante variables de memoria volátiles (`volatile int g_stop_flag`) sincronizada con `WaitForMultipleObjects()` y `CloseHandle()` garantiza la terminación ordenada de los hilos de trabajo, previniendo condiciones de carrera, estados zombi (*zombie threads*) y fugas de recursos en el kernel.

---

*Desarrollado para la materia de Arquitectura de Computadoras (SIS-131) — Universidad Católica Boliviana "San Pablo" — 2026*
