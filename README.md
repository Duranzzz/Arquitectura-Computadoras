# Arquitectura de Computadoras

[![Language: C/C++](https://img.shields.io/badge/Language-C%20%2F%20C%2B%2B%20%2F%20ASM-00599C.svg?logo=c&logoColor=white)](https://github.com/Duranzzz/Arquitectura-Computadoras)
[![Environment: Linux & Windows](https://img.shields.io/badge/Platform-Ubuntu%20Linux%20%7C%20MSYS2-E95420.svg?logo=ubuntu&logoColor=white)](https://github.com/Duranzzz/Arquitectura-Computadoras)
[![Kanban: Project 5](https://img.shields.io/badge/Kanban-GitHub%20Projects%20%235-brightgreen.svg?logo=github&logoColor=white)](https://github.com/users/Duranzzz/projects/5)
[![Status: Active](https://img.shields.io/badge/Semester-2--2026-blue.svg)](https://github.com/Duranzzz/Arquitectura-Computadoras)

Repositorio académico oficial de la materia **SIS-131: Arquitectura de Computadoras** en la **Universidad Católica Boliviana "San Pablo"**. 

Este espacio centraliza el código fuente, bancos de prueba, simuladores de hardware, reportes técnicos, diagnósticos de bajo nivel y documentación metodológica desarrollados a lo largo del semestre. La gestión del ciclo de vida del proyecto y el avance de las actividades se coordina mediante un **tablero Kanban interactivo en GitHub Projects**.

---

## Tabla de Contenidos

1. [Información Académica](#1-información-académica)
2. [Descripción y Enfoque de la Materia](#2-descripción-y-enfoque-de-la-materia)
3. [Índice General de Prácticas y Laboratorios](#3-índice-general-de-prácticas-y-laboratorios)
4. [Módulos Recientes Destacados](#4-módulos-recientes-destacados)
5. [Organización del Trabajo — Tablero Kanban](#5-organización-del-trabajo--tablero-kanban)
6. [Convenciones Oficiales y Formato de Tareas](#6-convenciones-oficiales-y-formato-de-tareas)
7. [Instrucciones de Compilación y Ejecución Global](#7-instrucciones-de-compilación-y-ejecución-global)

---

## 1. Información Académica

| Campo | Detalle Institucional |
| :--- | :--- |
| **Universidad** | Universidad Católica Boliviana "San Pablo" |
| **Facultad** | Facultad de Ingeniería |
| **Departamento** | Departamento de Ingeniería de Sistemas / Inteligencia Artificial |
| **Materia** | SIS-131: Arquitectura de Computadoras |
| **Semestre** | 4to Semestre — Semestre Académico 2-2026 |
| **Docente** | Ing. Paulo César Loayza Carrasco |
| **Estudiante** | Leonardo Durán Cuenca |
| **Usuario GitHub** | [`@Duranzzz`](https://github.com/Duranzzz) |
| **Repositorio** | [Duranzzz/Arquitectura-Computadoras](https://github.com/Duranzzz/Arquitectura-Computadoras) |
| **Tablero Kanban** | [Proyecto #5 — Arquitectura de Computadoras](https://github.com/users/Duranzzz/projects/5) |

---

## 2. Descripción y Enfoque de la Materia

**Arquitectura de Computadoras** proporciona una comprensión rigurosa y matemática del funcionamiento físico y lógico del computador, abarcando desde la capa de compuertas y transistores hasta la interacción entre el microprocesador, la memoria principal y el subsistema de Entrada/Salida (E/S).

### Núcleos Temáticos Desarrollados:
* **Modelo Von Neumann y Datapath:** Estructura de la CPU, registros generales (`AX, CX, R1..R3`), registro de instrucción (`IR`), contador de programa (`PC`), ALU y reloj bifásico de micro-pasos temporales ($T_0$ a $T_6$).
* **Frontera de Silicio y Aduanas de Memoria:** Aislamiento de buses internos y externos mediante los registros **MAR** (*Memory Address Register*) y **MDR** (*Memory Data Register*).
* **Disciplina de Buses y Exclusión Mutua:** Arbitraje de bus compartido (Bus de Direcciones, Bus de Datos y Bus de Control) y validación de la Regla de Oro contra colisiones lógicas:
  $$\text{MEM\_READ} + \text{MEM\_WRITE} + \text{IO\_READ} + \text{IO\_WRITE} \le 1$$
* **Espacio de E/S Aislada (*Isolated I/O*):** Controladores periféricos por puertos (`0x60/0x64`, `0x01/0x02`), instrucciones `IN`/`OUT` y protocolos de handshake automático en hardware.
* **Búferes Circulares en Memoria RAM:** Implementación de colas elásticas FIFO en memoria física (como el área BDA en `0x0040:0x001A` o búferes dedicados), punteros `HEAD` y `TAIL`, y detección preventiva de *Buffer Overflow*.
* **Controladores de Video y VRAM:** Mapeo de memoria de pantalla en modo texto (VGA `0xB8000` y matrices didácticas con Latch CRT), renderizado de celdas empaquetadas `[ASCII, Color]`, desplazamiento vertical (*Scroll Up*) y acceso directo por framebuffer (`/dev/fb0`, `/dev/vcsa`, `/dev/mem`).
* **Jerarquía y Optimización de Memoria:** Cuantificación de latencia en niveles de caché L1/L2/L3, penalización de fallos de caché (*cache misses*), algoritmos de convolución 2D optimizados y análisis de intercambio a disco (*Swap space*).
* **Buses de Comunicación e IPC de Alto Rendimiento:** Comparación entre conductos conmutados por el kernel (*Named Pipes* con millones de syscalls) y memoria compartida POSIX (*Shared Memory* Zero-Copy con semáforos y procesamiento por lotes).
* **Paralelismo y Afinidad de CPU:** Control de afinidad de núcleos (*CPU Affinity*), anclaje de hilos (*Thread Pinning*) y pruebas de estrés bajo contención multi-core.

---

## 3. Índice General de Prácticas y Laboratorios

El repositorio se encuentra estructurado en carpetas independientes para cada módulo de laboratorio o práctica asignada:

| Directorio | Tarea / Módulo | Descripción Técnica | Lenguajes / Herramientas | Estado |
| :--- | :--- | :--- | :--- | :---: |
| [`LaboratorioTecladoMonitorBuses/`](LaboratorioTecladoMonitorBuses/) | **Lab: E/S, BDA y VRAM** (Issue #35) | Inyección en cola TTY, interrupción BIOS `INT 16h`, BDA en RAM (`0x0040:0x001A`), lectura por puerto `0x60/0x64`, y memoria VRAM (`/dev/fb0`, `/dev/vcsa`, `0xB8000`). | ANSI C99, ASM inline, POSIX | **Done** |
| [`LaboratorioPuertosColaVRAM/`](LaboratorioPuertosColaVRAM/) | **Lab: Buses, FIFO y VRAM 4x4** (Issue #34) | Controlador de teclado por puertos aislados (`0x01/0x02`), cola circular FIFO en RAM (`0x70..0x7A`), VRAM con Latch CRT y terminal con borrado Backspace. | ANSI C99, GCC, Simulación Bus | **Done** |
| [`PracticaOptimizacionBusesIPC/`](PracticaOptimizacionBusesIPC/) | **Práctica: Buses IPC y Memoria** (Issue #33) | Benchmark de 10,000,000 paquetes: Pipes FIFO (20M syscalls) vs Memoria Compartida POSIX Zero-Copy + Semáforos + Batching (40k señales). | C99, POSIX shm, Pthreads | **Done** |
| [`PracticaControlAfinidadCPU/`](PracticaControlAfinidadCPU/) | **Práctica: Afinidad de CPU** (Issue #32) | Control de afinidad y *thread pinning* con `sched_setaffinity()` y `pthread_setaffinity_np()`, estrés de contención multi-core. | C / C++17, Pthreads | **Done** |
| [`PracticaMemoriaRamSwap/`](PracticaMemoriaRamSwap/) | **Práctica: RAM y Swap en Linux** (Issue #31) | Asignación de bloques contiguos de memoria para agotar la RAM física y forzar la paginación a Swap en Linux. | C, Linux Memory Subsystem | **Done** |
| [`PracticaConvolucion2DMemoria/`](PracticaConvolucion2DMemoria/) | **Práctica: Convolución 2D Caché** (Issue #30) | Benchmark de convolución matricial 2D comparando acceso ingenuo por columnas vs acceso contiguo por filas alineado a líneas de caché. | C++17, Cache Locality | **Done** |
| [`LaboratorioOptimizacionMemoria/`](LaboratorioOptimizacionMemoria/) | **Lab: Jerarquía de Memoria e ILP** (Issue #29) | Optimización de bucles con desenrollado (*Loop Unrolling*), uso de registros de CPU y localidad temporal/espacial. | ANSI C99, GCC O0-O3 | **Done** |
| [`PracticaDiagnosticoMemoriaCPU/`](PracticaDiagnosticoMemoriaCPU/) | **Práctica: Diagnóstico de Sistema** (Issue #28) | Diagnóstico e inspección de métricas de CPU y memoria en Linux (`lscpu`, `/proc/cpuinfo`, `/proc/meminfo`, `free`, `vmstat`). | Bash, Linux CLI | **Done** |
| [`InvestigacionVisualBasic/`](InvestigacionVisualBasic/) | **Investigación: Simulador VBA** (Issues #26, #27) | Diseño arquitectónico y modelo de simulación de computadora Von Neumann en Excel VBA. | Excel VBA, Macro Architecture | **Done** |
| [`PracticaGestionMemoria/`](PracticaGestionMemoria/) | **Práctica: Jerarquía de Memoria** (Issue #20) | Lectura de niveles de memoria y transferencia de datos por bloques de tamaño variable. | C++14, Memory Hierarchy | **Done** |
| [`PracticaArregloRam/`](PracticaArregloRam/) | **Práctica: Suma de Arreglos en RAM** (Issues #18, #19) | Implementación de suma de arreglos residentes en memoria RAM en C nativo e Inline Assembly x86. | C, Inline Assembly (x86) | **Done** |
| [`PracticaAnsi/`](PracticaAnsi/) | **Práctica: Aritmética y Banderas** (Issues #6, #8, #9, #10..#14) | Operaciones aritméticas binarias, bitfields y detección de banderas del procesador (Carry, Overflow, Zero) en ASM y ANSI C. | ANSI C, ASM x86 | **Done** |
| [`ComandosDiagnosticosLinux/`](ComandosDiagnosticosLinux/) | **Práctica: Diagnóstico de Hardware** (Issues #23, #24, #25) | Inspección de procesadores, buses PCI/USB y dispositivos de almacenamiento con herramientas nativas de Linux. | Bash, `lspci`, `lsusb`, `lshw` | **Done** |
| [`PracticaBinariosScripts/`](PracticaBinariosScripts/) | **Práctica: Entorno Linux** (Issues #4, #5) | Instalación, configuración y scripts base del entorno de trabajo en máquina virtual Ubuntu. | Ubuntu Linux, Shell scripting | **Done** |

---

## 4. Módulos Recientes Destacados

### 4.1 Laboratorio de Entradas, Salidas por Puertos, BDA y VRAM (`LaboratorioTecladoMonitorBuses`)
* **Manual Técnico de 11 Páginas:** Modela el subsistema de entrada/salida y video del PC clásico x86.
* **Escritor BDA (`prog1_escribir_cola.c`):** Inserta pulsaciones mediante inyección TTY (`ioctl(TIOCSTI)`), servicio de interrupción BIOS `INT 16h (AH = 05h)` con detección de desbordamiento, y escritura directa a memoria RAM en el segmento BDA (`0x0040:0x001A`).
* **Lector Multi-Método (`prog2_leer_teclado.c`):** Demuestra lectura en niveles de abstracción crecientes: `scanf()`, `getc()`, lectura directa por puerto aislado `0x60` con handshake en el puerto `0x64`, lectura directa de BDA en RAM y llamada BIOS `INT 16h (AH = 00h)` extrayendo `AL=ASCII` y `AH=Scan-code`.
* **Monitor VRAM en Linux (`escribir_monitor_linux.c`):** Manipulación de Framebuffer `/dev/fb0` (píxeles RGB), consola virtual `/dev/vcsa`, memoria física en `0xB8000` con ensamblador embebido `movb %0, (%1)`, y proyección CRT didáctica con colores ANSI reales.

### 4.2 Laboratorio de Puertos, Cola Circular y Monitor VRAM 4x4 (`LaboratorioPuertosColaVRAM`)
* **Simulación Von Neumann:** Implementa buses externos independientes (Address, Data, Control) y aduanas MAR/MDR.
* **Handshake Automático de Hardware:** La lectura del puerto de datos `0x02` limpia de inmediato el bit 0 del registro de estado en el puerto `0x01`.
* **Buffer Circular en Memoria RAM (`0x70..0x7A`):** Gestión rigurosa de 8 celdas en RAM con punteros `KB_HEAD` (`0x78`), `KB_TAIL` (`0x79`) y `KB_COUNT` (`0x7A`), asegurando aritmética circular modular y rechazo por *Buffer Overflow*.
* **Controlador VRAM y Reto Integrador:** Pantalla de 4x4 caracteres (`0xF0..0xFF`) con Latch de refresco para evitar parpadeos visuales (*tearing*), desplazamiento vertical hacia arriba (*Scroll Up*) y terminal con borrado interactivo mediante tecla **Backspace** (`0x08` / `0x7F`).

### 4.3 Práctica de Optimización de Buses e IPC en Linux (`PracticaOptimizacionBusesIPC`)
* **Carga de Estrés:** Transmisión masiva de 10,000,000 paquetes de 64 bytes (640 MB de carga útil neta).
* **Comparativa Arquitectónica:**
  * *No Optimizado (Named Pipes):* 20,000,000 cambios de contexto Ring 3 $\leftrightarrow$ Ring 0 mediante `write()` y `read()` individuales, sufriendo el cuello de botella del kernel y doble copia en memoria.
  * *Optimizado (POSIX Shared Memory):* Transferencia **Zero-Copy** en memoria física compartida (`mmap`), sincronización con semáforos POSIX y procesamiento por lotes (*Batching* de 500 paquetes), reduciendo las llamadas al sistema en un **99.8%** y multiplicando el rendimiento por más de 10 veces.

---

## 5. Organización del Trabajo — Tablero Kanban

El seguimiento pedagógico y técnico de todas las actividades se gestiona a través del tablero oficial de **GitHub Projects (Proyecto #5)**:

👉 **[Acceder al Tablero Kanban del Proyecto](https://github.com/users/Duranzzz/projects/5)**

### Ciclo de Estados de las Tarjetas:
* **Backlog:** Tareas y requerimientos propuestos pendientes de inicio.
* **In Progress:** Tareas en fase activa de desarrollo, codificación y pruebas.
* **Done:** Prácticas terminadas con código compilado, validación de salida, capturas de evidencia, reporte técnico en `README.md` y commit sincronizado en `main`.

### Regla de Posicionamiento Descendente:
Para garantizar una navegación ágil y cronológica, las tarjetas de la columna **Done** se posicionan con el número de tarea más reciente en el **tope superior (posición #1)**, descendiendo progresivamente hacia las primeras tareas del semestre:
$$\text{Issue \#35} \to \text{Issue \#34} \to \text{Issue \#33} \to \dots \to \text{Issue \#4}$$

---

## 6. Convenciones Oficiales y Formato de Tareas

Cada issue creada en el repositorio cumple de forma obligatoria con la plantilla estándar oficial [`TaskTemplate.md`](TaskTemplate.md):

```markdown
## Title
<!-- Infinitive verb + short, clear phrase (3rd person) in English -->

## Description
**Assigned by:** Paulo César Loayza Carrasco  
**What to do:** [Detailed step-by-step technical implementation]  
**Objective:** [Academic purpose and architectural insights]  

## Responsible
Leonardo Durán Cuenca

## Due Date
YYYY-MM-DD

## Link
https://github.com/Duranzzz/Arquitectura-Computadoras/tree/main/[nombre carpeta]

## Attachments
<p align="center">
  <img src="https://github.com/Duranzzz/Arquitectura-Computadoras/raw/main/[nombre carpeta]/imgs/[imagen].png" alt="Diagrama" width="850">
</p>

## Program Output
<p align="center">
  <img src="https://github.com/Duranzzz/Arquitectura-Computadoras/raw/main/[nombre carpeta]/imgs/[captura].png" alt="Terminal" width="850">
</p>
```

### Reglas Críticas del Proyecto:
1. **Idioma Estricto:** Todo el contenido de issues y tarjetas en GitHub se redacta en **inglés**.
2. **Formato Obligatorio del Campo `Link`:** Siempre apunta directamente a la carpeta de la práctica en la rama `main`:
   ```text
   https://github.com/Duranzzz/Arquitectura-Computadoras/tree/main/[nombre carpeta]
   ```
3. **Enlaces de Imágenes (`/raw/main/`):** Todas las imágenes incrustadas en GitHub Issues emplean exclusivamente la URL directa `/raw/main/` (**nunca `/blob/main/`**), asegurando la renderización inmediata sin páginas HTML intermedias.
4. **Metadatos Fijos:**
   * **Assigned by:** `Paulo César Loayza Carrasco`
   * **Responsible:** `Leonardo Durán Cuenca`
   * **Assignee:** `Duranzzz`
   * **Priority:** `P1`
   * **Status:** `Done`

---

## 7. Instrucciones de Compilación y Ejecución Global

Los programas del repositorio están desarrollados bajo los estándares **ANSI C (C89/C99)** y **C++ (C++14/C++17)**, compatibles con compiladores GCC/Clang tanto en entornos **Linux** como en **Windows (MSYS2 UCRT64 / MinGW)**.

### Compilación General con GCC:
```bash
# Compilación con advertencias estrictas recomendada en la materia:
gcc -std=c99 -Wall -Wextra -pedantic archivo.c -o ejecutable

# Para programas que emplean memoria compartida o hilos POSIX en Linux:
gcc -O3 -Wall -Wextra -pedantic archivo.c -o ejecutable -lrt -lpthread

# Para módulos en C++:
g++ -std=c++17 -Wall -Wextra -pedantic archivo.cpp -o ejecutable -lpthread
```

---

*Repositorio académico administrado por Leonardo Durán Cuenca — 2026*  
*Docente Revisor: Ing. Paulo César Loayza Carrasco — Universidad Católica Boliviana "San Pablo"*
