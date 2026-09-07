# Investigación: Visual Basic y Simulación de Arquitectura de Computadoras en Excel VBA

**Universidad Católica Boliviana "San Pablo" — Sede Santa Cruz**  
**Carrera:** Ingeniería en Inteligencia Artificial  
**Materia:** Arquitectura de Computadoras  
**Semestre:** 4to Semestre — 2026  
**Docente:** Ing. Paulo César Loayza Carrasco  
**Estudiante:** Leonardo Durán Cuenca  
**Fecha:** 7 de septiembre de 2026  

---

## Tabla de Contenidos
1. [Introducción y Contexto Histórico](#1-introducción-y-contexto-histórico)
   - 1.1 De Dartmouth BASIC a Microsoft BASIC
   - 1.2 El Proyecto Ruby de Alan Cooper y el acuerdo con Bill Gates
   - 1.3 Evolución de versiones clásicas: VB 1.0 a VB 6.0
   - 1.4 La bifurcación: Visual Basic .NET y la preservación de VBA
2. [Arquitectura Interna y Modelo de Ejecución de Visual Basic](#2-arquitectura-interna-y-modelo-de-ejecución-de-visual-basic)
   - 2.1 P-Code (Pseudo-Código) vs. Código Nativo x86
   - 2.2 La Máquina Virtual y el Runtime de Visual Basic (`MSVBVM60.DLL` / Motor VBA)
   - 2.3 Modelo de Memoria y Tipos de Datos a Bajo Nivel
   - 2.4 Arquitectura Orientada a Eventos y el Bucle de Mensajes de Windows
   - 2.5 Component Object Model (COM) e Interoperabilidad
3. [Fundamentos Teóricos: Arquitectura de Von Neumann y Ciclo de Instrucción](#3-fundamentos-teóricos-arquitectura-de-von-neumann-y-ciclo-de-instrucción)
   - 3.1 Estructura Clásica de Von Neumann
   - 3.2 Banco de Registros Fundamentales (PC, MAR, MBR/MDR, IR, AC, Flags)
   - 3.3 El Ciclo de Instrucción: Fetch, Decode, Execute
   - 3.4 Nivel de Transferencia de Registros (RTL) y Microórdenes (Señales de Control)
4. [Simulación de la Arquitectura de un Computador con Visual Basic / VBA en Excel](#4-simulación-de-la-arquitectura-de-un-computador-con-visual-basic--vba-en-excel)
   - 4.1 Excel como Entorno de Modelado de Hardware: La Cuadrícula como RAM
   - 4.2 Diseño de la Interfaz Visual: Panel de Registros y Mapa de Memoria
   - 4.3 Modelado de Registros y Buses en VBA
   - 4.4 Implementación del Ciclo de Búsqueda (Fetch Cycle)
   - 4.5 Decodificación y Conjunto de Instrucciones Simulado (ISA)
   - 4.6 Ejecución de Microórdenes y Señales de la Unidad de Control
   - 4.7 Código VBA Completo y Funcional: Simulador de CPU Tipo SAP-1
5. [Ventajas Pedagógicas y Limitaciones Técnicas de la Simulación en Excel/VBA](#5-ventajas-pedagógicas-y-limitaciones-técnicas-de-la-simulación-en-excelvba)
6. [Conclusiones](#6-conclusiones)
7. [Bibliografía y Referencias](#7-bibliografía-y-referencias)

---

## 1. Introducción y Contexto Histórico

### 1.1 De Dartmouth BASIC a Microsoft BASIC
El lenguaje **BASIC** (*Beginner's All-purpose Symbolic Instruction Code*) nació en 1964 en Dartmouth College, creado por John G. Kemeny y Thomas E. Kurtz con el objetivo de permitir que estudiantes de disciplinas no científicas pudieran interactuar con computadoras de tiempo compartido sin necesidad de programar en ensamblador o Fortran.

En 1975, Bill Gates y Paul Allen fundaron **Micro-Soft** desarrollando un intérprete de BASIC para el Altair 8800 (basado en el procesador Intel 8080). Microsoft BASIC se convirtió en el estándar de facto de la era de las microcomputadoras de 8 bits (Apple II, Commodore 64, MSX) y más tarde en la base del sistema MS-DOS con GW-BASIC y QBasic.

### 1.2 El Proyecto Ruby de Alan Cooper y el acuerdo con Bill Gates
A finales de la década de 1980, programar interfaces gráficas de usuario (GUI) para Microsoft Windows requería escribir código sumamente verboso y complejo en lenguaje C, interactuando directamente con el SDK de Win16/Win32, manipulando bucles de mensajes, manejadores de ventanas (*HWND*) y contextos de dispositivo (*HDC*).

En 1988, **Alan Cooper** (conocido como el "Padre de Visual Basic") concibió un prototipo llamado **Tripod**, posteriormente rebautizado como **Ruby**. Ruby no era un lenguaje de programación completo, sino un constructor visual de interfaces gráficas basado en componentes arrastrables (*drag-and-drop*).

Bill Gates vio una demostración de Ruby y reconoció inmediatamente su potencial. Microsoft compró el software a Cooper y tomó una decisión estratégica fundamental: combinar el entorno de diseño visual de Ruby con el motor del lenguaje QuickBASIC. Este proyecto interno llevó por nombre clave **Thunder** y fue presentado oficialmente en mayo de 1991 como **Visual Basic 1.0 para Windows**.

### 1.3 Evolución de versiones clásicas: VB 1.0 a VB 6.0
* **Visual Basic 1.0 (1991):** Introdujo la programación rápida de aplicaciones (RAD) para Windows 3.0 de 16 bits. Los controles se extendían mediante librerías DLL dinámicas denominadas VBX (*Visual Basic Extensions*).
* **Visual Basic 2.0 y 3.0 (1992-1993):** Incorporó el motor de bases de datos Microsoft Jet (Access) y soporte ampliado para OLE (*Object Linking and Embedding*).
* **Visual Basic 4.0 (1995):** Versión de transición que permitió compilar tanto para Windows de 16 bits como de 32 bits (Windows 95 / NT). Reemplazó los controles VBX de 16 bits por controles OCX basados en la arquitectura COM de 32 bits.
* **Visual Basic 5.0 (1997):** Hito arquitectónico. Introdujo por primera vez un compilador capaz de generar **código máquina nativo x86** optimizado, además de permitir la creación de componentes ActiveX propios.
* **Visual Basic 6.0 (1998):** La cúspide de la era clásica. Añadió integración con páginas web dinámicas (DHTML), soporte empresarial masivo mediante COM+ y MTS, y una estabilidad legendaria. Se convirtió en el lenguaje de desarrollo corporativo más utilizado en el mundo a finales de los años 90.

### 1.4 La bifurcación: Visual Basic .NET y la preservación de VBA
En 2002, Microsoft lanzó la plataforma **.NET**, reescribiendo Visual Basic desde sus cimientos como **Visual Basic .NET (VB.NET)**. Aunque conservó la sintaxis superficial de BASIC, arquitectónicamente fue una ruptura radical: se eliminó el modelo COM directo y el conteo de referencias para adoptar el *Common Language Runtime* (CLR), compilación intermedia (MSIL) y recolección de basura generacional.

Sin embargo, el motor clásico de Visual Basic no desapareció: sobrevivió y sigue plenamente vigente a través de **VBA (Visual Basic for Applications)**, el motor de macros integrado en la suite de productividad **Microsoft Office** (Excel, Word, Access). VBA continúa operando sobre la arquitectura COM clásica de 32 y 64 bits, convirtiendo a Microsoft Excel en una plataforma de cálculo, modelado y simulación accesible en millones de estaciones de trabajo alrededor del planeta.

---

## 2. Arquitectura Interna y Modelo de Ejecución de Visual Basic

Desde el punto de vista de la Arquitectura de Computadoras, Visual Basic clásico (VB6 y VBA) se ubica en un punto fascinante entre los entornos interpretados y los compiladores a bajo nivel.

### 2.1 P-Code (Pseudo-Código) vs. Código Nativo x86
Visual Basic ofrece históricamente dos modalidades de traducción y ejecución de código:

```
[ Código Fuente VB/VBA ]
           │
           ▼ Compilador Frontend
┌──────────────────────────────────────┐
│        Opciones de Compilación       │
├──────────────────┬───────────────────┤
│    Modo P-Code   │ Modo Código Nativo│
│ (Pseudo-Código)  │   (Machine Code)  │
└────────┬─────────┘─────────┬─────────┘
         │                   │
         ▼                   ▼ Backend C2 (MSVC)
┌─────────────────┐ ┌──────────────────┐
│ Bytecode P-Code │ │ Instrucciones x86│
│ (Compacto, VM)  │ │ (Optimizadas)    │
└────────┬────────┘ └────────┬─────────┘
         │                   │
         ▼                   ▼
┌──────────────────────────────────────┐
│   MSVBVM60.DLL / Motor VBA Core      │
│ (Gestión de Memoria, COM, I/O, OS)   │
└──────────────────────────────────────┘
```

1. **P-Code (Pseudo-Código):**
   * El compilador traduce el código fuente a una secuencia compacta de instrucciones intermedias (bytecode) destinadas a una máquina virtual de pila.
   * **Ventaja:** Genera archivos ejecutables binarios extremadamente pequeños, ideales para la era de almacenamiento reducido (disquetes y redes lentas).
   * **Desventaja:** Cada opcode de P-Code debe ser despachado e interpretado en tiempo de ejecución por el motor de la máquina virtual, introduciendo sobrecarga respecto a la ejecución directa en el procesador.
   * **Nota relevante:** El motor de macros de **Excel VBA opera internamente mediante P-Code compilado** almacenado en los flujos binarios del libro (*VBA streams* en formato CFBF/OpenXML).

2. **Código Nativo x86 (Introducido en VB5/VB6):**
   * El frontend del compilador genera una representación intermedia que es entregada al optimizador de backend **C2.EXE** (el mismo motor de optimización utilizado por Microsoft Visual C++).
   * Genera instrucciones máquina x86 reales (`mov`, `add`, `cmp`, `call`, `jmp`).
   * Aunque el código es nativo, **sigue dependiendo críticamente del runtime** para operaciones de cadenas, asignación de memoria y llamadas al sistema operativo.

### 2.2 La Máquina Virtual y el Runtime de Visual Basic (`MSVBVM60.DLL` / Motor VBA)
A diferencia de los binarios puramente estáticos generados en C o ensamblador, todo programa compilado en Visual Basic 6 requiere la librería dinámica `MSVBVM60.DLL` (*Microsoft Visual Basic Virtual Machine 6.0*), y en el caso de Excel, la librería correspondiente al motor VBA (`VBE7.DLL`).

Las responsabilidades arquitectónicas del Runtime incluyen:
* **Administración del Heap y Memoria Dinámica:** Creación y destrucción de objetos COM y arreglos dinámicos (`SafeArray`).
* **Operaciones Matemáticas Seguras:** Inserción de rutinas de verificación de desbordamiento aritmético (*Integer Overflow Check*) y comprobación de límites de arreglos (*Array Bounds Check*).
* **Manejo Estructurado y No Estructurado de Excepciones:** Soporte para la pila de errores y la directiva `On Error GoTo`.
* **Conversión y Coerción de Tipos en Tiempo de Ejecución:** Conversión implícita de tipos a través del evaluador de `Variant`.

### 2.3 Modelo de Memoria y Tipos de Datos a Bajo Nivel

En la arquitectura de 32 bits (x86) de Visual Basic 6 y VBA:

| Tipo de Dato | Tamaño en Memoria | Representación a Bajo Nivel |
| :--- | :--- | :--- |
| **`Byte`** | 1 byte (8 bits) | Entero sin signo (`0` a `255`). |
| **`Integer`** | 2 bytes (16 bits) | Entero con signo en Complemento a Dos (`-32,768` a `32,767`). |
| **`Long`** | 4 bytes (32 bits) | Entero con signo en Complemento a Dos (`-2,147,483,648` a `2,147,483,647`). |
| **`Single`** | 4 bytes (32 bits) | Punto flotante estándar IEEE 754 (precisión simple). |
| **`Double`** | 8 bytes (64 bits) | Punto flotante estándar IEEE 754 (precisión doble). |
| **`Currency`** | 8 bytes (64 bits) | Entero en complemento a dos escalado implícitamente por 10,000 (evita errores de redondeo de coma flotante). |
| **`String` (`BSTR`)** | 4 bytes puntero + Heap | Puntero a cadena COM con prefijo de longitud de 32 bits (en bytes) y terminador nulo `\0`. |
| **`Variant`** | 16 bytes | Estructura que almacena un discriminador de tipo (`VARTYPE` de 2 bytes) + 6 bytes de alineación + 8 bytes para el dato/puntero. |

#### Las Cadenas `BSTR` en Memoria:
Una cadena en Visual Basic no es un arreglo de caracteres simple terminado en nulo (`char*` estilo C). Es un **BSTR** de COM:
```
[ Long: Longitud en bytes (4 bytes) ] [ Caracteres UTF-16 (2 bytes c/u) ... ] [ Nulo: 0x0000 (2 bytes) ]
                                      ▲
                                      └── El puntero BSTR apunta aquí
```
Esto permite conocer la longitud de la cadena en tiempo constante $O(1)$ sin necesidad de recorrer la memoria buscando un carácter nulo.

#### Punteros Nativos en Visual Basic (`VarPtr`, `StrPtr`, `ObjPtr`):
Aunque Visual Basic oculta los punteros directos al programador para garantizar seguridad de memoria, el lenguaje proporciona funciones de bajo nivel que exponen las direcciones de memoria física/virtual del proceso:
* **`VarPtr(variable)`:** Devuelve la dirección de memoria de una variable de cualquier tipo (equivalente al operador `&` en C).
* **`StrPtr(cadena)`:** Devuelve la dirección de memoria del primer carácter del buffer Unicode del `BSTR`.
* **`ObjPtr(objeto)`:** Devuelve la dirección de la interfaz `IUnknown` del objeto COM en memoria.

### 2.4 Arquitectura Orientada a Eventos y el Bucle de Mensajes de Windows
Una aplicación tradicional de línea de comandos en C sigue una ejecución procedural secuencial guiada por el flujo de control del hilo principal. Por el contrario, Visual Basic se construyó alrededor de la **arquitectura orientada a eventos** de la interfaz gráfica de Windows:

1. El sistema operativo Windows captura interrupciones de hardware (teclado, mouse, temporizador).
2. Los controladores del sistema operativo traducen los eventos de hardware a **Mensajes de Windows** (`WM_LBUTTONDOWN`, `WM_KEYDOWN`, `WM_PAINT`, `WM_TIMER`) y los colocan en la cola de mensajes del hilo de la aplicación.
3. El bucle de mensajes central (*Message Loop*) extrae el mensaje con `GetMessage()` y lo despacha con `DispatchMessage()` a la función de ventana correspondiente (*Window Procedure* o `WndProc`).
4. El Runtime de Visual Basic intercepta este mensaje y lo mapea automáticamente al manejador de eventos del formulario o control:
   ```vb
   Private Sub CommandButton1_Click()
       ' Se ejecuta en respuesta a WM_LBUTTONUP sobre el botón
   End Sub
   ```

### 2.5 Component Object Model (COM) e Interoperabilidad
Toda la extensibilidad de Visual Basic reposa en **COM (Component Object Model)**:
* Cada objeto expone una tabla de punteros a funciones virtuales denominada **vtable** en memoria.
* Todas las interfaces heredan de **`IUnknown`**, que implementa conteo de referencias atómico (`AddRef`, `Release`) y consulta de interfaces dinámicas (`QueryInterface`).
* Soporta tanto **Early Binding** (acceso de alto rendimiento directamente a través del desplazamiento en la vtable, resuelto en tiempo de compilación) como **Late Binding** (acceso dinámico a través del método `IDispatch::Invoke`, resuelto en tiempo de ejecución buscando el nombre del método en una tabla hash).

---

## 3. Fundamentos Teóricos: Arquitectura de Von Neumann y Ciclo de Instrucción

Para comprender cómo Visual Basic y Excel pueden simular el funcionamiento interno de un computador, es indispensable revisar los componentes esenciales de la arquitectura clásica de computadoras.

### 3.1 Estructura Clásica de Von Neumann
Propuesta por John von Neumann en 1945, se basa en el concepto del **programa almacenado**, donde tanto las instrucciones del programa como los datos comparten el mismo espacio de memoria física y son transportados a través de los mismos buses del sistema.

```
┌────────────────────────────────────────────────────────────────────────┐
│                        CPU (Unidad Central de Proceso)                 │
│                                                                        │
│  ┌────────────────────────┐            ┌────────────────────────────┐  │
│  │   Unidad de Control    │            │   Unidad Aritmético-Lógica │  │
│  │         (UC)           │            │            (ALU)           │  │
│  │                        │            │                            │  │
│  │ ┌────────────────────┐ │            │ ┌────────────────────────┐ │  │
│  │ │ Registro de        │ │            │ │      Acumulador        │ │  │
│  │ │ Instrucción (IR)   │ │            │ │         (ACC)          │ │  │
│  │ └────────────────────┘ │            │ └────────────────────────┘ │  │
│  │ ┌────────────────────┐ │            │ ┌────────────────────────┐ │  │
│  │ │ Decodificador      │ │            │ │   Registro de Estado   │ │  │
│  │ │ de Instrucción     │ │            │ │      (FLAGS: Z, C)     │ │  │
│  │ └────────────────────┘ │            │ └────────────────────────┘ │  │
│  └───────────┬────────────┘            └─────────────┬──────────────┘  │
│              │                                       │                 │
│  ┌───────────┴───────────────────────────────────────┴──────────────┐  │
│  │                        Banco de Registros                        │  │
│  │  PC (Contador de Programa)    MAR (Reg. Direcciones de Memoria)  │  │
│  │  MBR / MDR (Reg. Datos)       TMP (Registros de Trabajo Temporal)│  │
│  └──────────────────────────────────────────────────────────────────┘  │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │ Buses del Sistema
               ┌────────────────────┼────────────────────┐
               │ Bus de Direcciones │ Bus de Control     │ Bus de Datos
               ▼                    ▼                    ▼
┌────────────────────────────────────────────────────────────────────────┐
│                        Memoria Principal (RAM)                         │
│   [ Dirección 00 ]  [ Instrucción / Dato ]                             │
│   [ Dirección 01 ]  [ Instrucción / Dato ]                             │
│   ...                                                                  │
│   [ Dirección FF ]  [ Instrucción / Dato ]                             │
└────────────────────────────────────────────────────────────────────────┘
```

### 3.2 Banco de Registros Fundamentales

1. **`PC` (Program Counter / Contador de Programa):**  
   Almacena la dirección de memoria de la **próxima instrucción** que debe ser leída y ejecutada por la CPU. Se incrementa automáticamente en cada ciclo de búsqueda.
2. **`MAR` (Memory Address Register / Registro de Dirección de Memoria):**  
   Almacena la dirección de memoria que la CPU desea acceder en una operación de lectura o escritura a través del Bus de Direcciones.
3. **`MBR` o `MDR` (Memory Buffer/Data Register / Registro de Datos de Memoria):**  
   Almacena el dato leído desde la memoria RAM o el dato que está a punto de ser grabado en ella a través del Bus de Datos.
4. **`IR` (Instruction Register / Registro de Instrucción):**  
   Almacena el código binario de la instrucción que se acaba de leer de la memoria para que la Unidad de Control pueda decodificarlo.
5. **`AC` o `ACC` (Accumulator / Acumulador):**  
   Registro de trabajo principal de la ALU que alberga los operandos y recibe los resultados de las operaciones aritméticas y lógicas.
6. **`FLAGS` (Registro de Estado):**  
   Conjunto de biestables (flip-flops) que registran las condiciones del resultado de la última operación en la ALU:
   * **`Z` (Zero Flag):** Vale 1 si el resultado de la operación fue 0.
   * **`C` (Carry Flag):** Vale 1 si hubo acarreo en el bit más significativo.
   * **`S` (Sign Flag):** Vale 1 si el resultado es negativo.

### 3.3 El Ciclo de Instrucción: Fetch, Decode, Execute
Cada instrucción ejecutada por el procesador atraviesa tres fases invariables:

1. **Fase de Búsqueda (Fetch):**  
   La CPU transfiere la dirección contenida en el `PC` al `MAR`, envía una señal de lectura al bus de control, la RAM deposita el contenido en el `MBR`, se copia la instrucción del `MBR` al `IR` y se incrementa el `PC` en 1 para apuntar a la instrucción siguiente.
2. **Fase de Decodificación (Decode):**  
   La Unidad de Control examina los bits del `IR` para separar el **Código de Operación (Opcode)** de los **Operandos / Direcciones**. El decodificador activa las líneas de control requeridas para la instrucción identificada.
3. **Fase de Ejecución (Execute):**  
   Se ejecutan las microoperaciones específicas dictadas por la instrucción (ej. leer un operando de memoria, realizar una suma en la ALU, alterar el contenido del Acumulador o modificar el flujo del programa mediante un salto condicional).

### 3.4 Nivel de Transferencia de Registros (RTL) y Microórdenes
A nivel de hardware, una sola instrucción máquina (como `ADD [50]`) no se ejecuta en un solo instante, sino que se divide en una secuencia de **pasos de tiempo** (*T-States* o ciclos de reloj: $T_0, T_1, T_2, \dots$), donde la Unidad de Control emite **microórdenes** (señales eléctricas binarias) que abren o cierran compuertas lógicas hacia el bus interno de la CPU.

Ejemplo en notación RTL (*Register Transfer Language*):
* **$T_0$:** $MAR \leftarrow PC$ *(Microórdenes activas: $PC_{out}, MAR_{in}$)*
* **$T_1$:** $MBR \leftarrow Memoria[MAR], \quad PC \leftarrow PC + 1$ *(Microórdenes activas: $RAM_{read}, MBR_{in}, PC_{inc}$)*
* **$T_2$:** $IR \leftarrow MBR$ *(Microórdenes activas: $MBR_{out}, IR_{in}$)*
* **$T_3$:** $MAR \leftarrow IR_{operando}$ *(Microórdenes activas: $IR_{out}, MAR_{in}$)*
* **$T_4$:** $MBR \leftarrow Memoria[MAR]$ *(Microórdenes activas: $RAM_{read}, MBR_{in}$)*
* **$T_5$:** $AC \leftarrow AC + MBR$ *(Microórdenes activas: $MBR_{out}, ALU_{add}, AC_{in}$)*

---

## 4. Simulación de la Arquitectura de un Computador con Visual Basic / VBA en Excel

La combinación de **Microsoft Excel** como interfaz gráfica visual y **VBA (Visual Basic for Applications)** como motor lógico proporciona una plataforma ideal para construir simuladores de procesadores didácticos (como la arquitectura SAP-1 de Albert Paul Malvino o procesadores Von Neumann de 8/16 bits).

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          PANTALLA DE EXCEL (FRONTEND)                       │
│                                                                             │
│  [ PANEL DE CONTROL ]            [ BANCO DE REGISTROS ]                     │
│  ┌────────────────────────┐      ┌─────────────────────────────┐            │
│  │ [Botón: Ciclo Reloj]   │      │ PC:  [ 05 ]   MAR: [ 05 ]   │            │
│  │ [Botón: Ejecutar]      │      │ MBR: [ 19 ]   IR:  [ 19 ]   │            │
│  │ [Botón: Reset]         │      │ ACC: [ 42 ]   ZF:  [ 0  ]   │            │
│  └────────────────────────┘      └─────────────────────────────┘            │
│                                                                             │
│  [ CUADRÍCULA DE EXCEL: MEMORIA RAM ]                                       │
│  ┌──────────┬──────────────┬───────────────┬──────────────┬──────────────┐  │
│  │ Direcc.  │ Hex / Bin    │ Operación     │ Operando     │ Comentario   │  │
│  ├──────────┼──────────────┼───────────────┼──────────────┼──────────────┤  │
│  │ 00       │ 1A           │ LDA           │ 10           │ Cargar A     │  │
│  │ 01       │ 2B           │ ADD           │ 11           │ Sumar B      │  │
│  │ 02 (PC)  │ 3C ◄ Resalta │ STA           │ 12           │ Guardar Res  │  │
│  │ ...      │ ...          │ ...           │ ...          │ ...          │  │
│  └──────────┴──────────────┴───────────────┴──────────────┴──────────────┘  │
└──────────────────────────────────────┬──────────────────────────────────────┘
                                       │ Llamadas a macros de VBA
                                       ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                           MOTOR DE VBA (BACKEND)                            │
│                                                                             │
│  Sub CicloReloj() ──► FaseFetch() ──► FaseDecodeExecute() ──► ActualizarUI()│
│                                                                             │
│  • Microórdenes binarias: PC_out, MAR_in, RAM_read, ALU_add, etc.           │
│  • Manipulación de estado de registros y variables globales.                │
│  • Coloreado dinámico de celdas en tiempo real (Visualización de Buses).    │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 4.1 Excel como Entorno de Modelado de Hardware: La Cuadrícula como RAM
En un computador físico, la memoria RAM es un arreglo lineal unidimensional de celdas direccionables con transistores y condensadores. En Microsoft Excel:
* **Cada fila de una tabla representa una dirección de memoria física.**
* Una columna alberga la **Dirección** (en decimal o hexadecimal, ej. `0x00` a `0x0F`).
* Otra columna alberga el **Contenido Binario / Hexadecimal** (el byte o palabra almacenada).
* Columnas auxiliares pueden albergar el **Mnemónico ASM** y los **Comentarios** para facilitar el seguimiento visual por parte del alumno.
* El acceso a la RAM se implementa con métodos nativos de VBA extremadamente directos:
  ```vb
  ' Lectura de la celda de memoria indicada por el MAR
  ValorLeido = Hoja1.Cells(MAR + OFFSET_FILA, COLUMNA_DATO).Value
  
  ' Escritura en memoria
  Hoja1.Cells(MAR + OFFSET_FILA, COLUMNA_DATO).Value = MBR
  ```

### 4.2 Diseño de la Interfaz Visual: Panel de Registros y Mapa de Memoria
Excel permite diseñar sin necesidad de frameworks de interfaz gráfica un panel completo de CPU:
1. **Sección de Registros:** Celdas formateadas con bordes y fuentes de ancho fijo (*Courier New* o *Consolas*) que muestran los valores actuales de `PC`, `MAR`, `MBR`, `IR`, `ACC`, `ZF` y `CF`.
2. **Resaltado Dinámico de Buses y Punteros:** Utilizando la propiedad `Interior.Color` de VBA, se puede colorear de amarillo o verde la fila de la memoria que está siendo apuntada por el `PC` o leída por el `MAR`, ofreciendo una visualización inmediata del flujo de datos en el bus.

### 4.3 Modelado de Registros y Buses en VBA
En un módulo estándar de VBA (`ModuloCPU.bas`), los registros de la CPU se definen como variables globales de tipos con tamaño ajustado a la arquitectura modelada (por ejemplo, una CPU de 8 bits con direcciones de 4 bits, como SAP-1):

```vb
' ==========================================================
' MODELO DE REGISTROS DE LA CPU (8 BITS)
' ==========================================================
Public PC  As Byte  ' Contador de Programa (4 bits: 0 a 15)
Public MAR As Byte  ' Registro de Direcciones de Memoria (4 bits)
Public MBR As Byte  ' Registro de Datos de Memoria (8 bits: 0 a 255)
Public IR  As Byte  ' Registro de Instrucción (8 bits: 4 bits Opcode + 4 bits Operando)
Public ACC As Byte  ' Acumulador (8 bits)
Public ZF  As Boolean ' Zero Flag (Bandera de Cero)
Public CF  As Boolean ' Carry Flag (Bandera de Acarreo)
Public FaseActual As String ' Estado del ciclo: Fetch, Decode, Execute
Public EstadoCPU  As Boolean ' True = En ejecución, False = Detenido (HALT)
```

### 4.4 Implementación del Ciclo de Búsqueda (Fetch Cycle)
El ciclo de búsqueda es universal e independiente de la instrucción a ejecutar. En VBA se modela paso a paso:

```vb
Public Sub FaseFetch()
    ' Paso T0: Transferir PC al MAR (MAR <- PC)
    MAR = PC
    VisualizarBusDirecciones MAR
    
    ' Paso T1: Leer memoria en la dirección MAR hacia MBR, e incrementar PC (PC <- PC + 1)
    MBR = LeerMemoriaRAM(MAR)
    PC = (PC + 1) Mod 16 ' Ajustado al tamaño de memoria (4 bits = 16 palabras)
    
    ' Paso T2: Transferir instrucción de MBR al IR (IR <- MBR)
    IR = MBR
    
    FaseActual = "DECODE"
    ActualizarInterfazGrafica
End Sub
```

### 4.5 Decodificación y Conjunto de Instrucciones Simulado (ISA)
Supongamos un procesador simple con palabras de 8 bits: los **4 bits superiores** representan el Código de Operación (*Opcode*) y los **4 bits inferiores** representan la Dirección del Operando (*Address*):

```
┌──────────────────────────────────────┬──────────────────────────────────────┐
│        4 Bits Superiores: OPCODE     │       4 Bits Inferiores: OPERANDO    │
│  [ Bit 7 ] [ Bit 6 ] [ Bit 5 ] [ Bit 4] │  [ Bit 3 ] [ Bit 2 ] [ Bit 1 ] [ Bit 0] │
└──────────────────────────────────────┴──────────────────────────────────────┘
```

Conjunto de instrucciones didáctico de ejemplo:
* `0000 (0x0)` — **NOP**: No operation (sin operación).
* `0001 (0x1)` — **LDA**: Carga el contenido de la memoria especificada en el Acumulador (`ACC <- RAM[Operando]`).
* `0010 (0x2)` — **ADD**: Suma el contenido de la memoria al Acumulador (`ACC <- ACC + RAM[Operando]`).
* `0011 (0x3)` — **SUB**: Resta el contenido de la memoria al Acumulador (`ACC <- ACC - RAM[Operando]`).
* `0100 (0x4)` — **STA**: Almacena el valor del Acumulador en la memoria (`RAM[Operando] <- ACC`).
* `0101 (0x5)` — **JMP**: Salto incondicional (`PC <- Operando`).
* `0110 (0x6)` — **JZ**: Salto condicional si Zero Flag está activa (`If ZF = 1 Then PC <- Operando`).
* `1111 (0xF)` — **HLT**: Detener la CPU (*Halt*).

En VBA, la separación de campos binarios se realiza de manera eficiente utilizando división entera (`\`) y el operador módulo (`Mod`):

```vb
Public Sub FaseDecodeExecute()
    Dim Opcode   As Byte
    Dim Operando As Byte
    
    ' Desempaquetado de campos binarios en un byte
    Opcode = IR \ 16       ' Desplaza 4 bits a la derecha (bits 7..4)
    Operando = IR Mod 16   ' Conserva los 4 bits inferiores (bits 3..0)
    
    Select Case Opcode
        Case &H0 ' NOP
            ' No hace nada
            
        Case &H1 ' LDA (Load Accumulator)
            MAR = Operando
            MBR = LeerMemoriaRAM(MAR)
            ACC = MBR
            ActualizarFlags ACC
            
        Case &H2 ' ADD (Sumar al Acumulador)
            MAR = Operando
            MBR = LeerMemoriaRAM(MAR)
            Dim Suma As Integer
            Suma = CInt(ACC) + CInt(MBR)
            CF = (Suma > 255) ' Detección de Carry Flag
            ACC = Suma Mod 256
            ActualizarFlags ACC
            
        Case &H3 ' SUB (Restar del Acumulador)
            MAR = Operando
            MBR = LeerMemoriaRAM(MAR)
            Dim Resta As Integer
            Resta = CInt(ACC) - CInt(MBR)
            CF = (Resta < 0) ' Detección de Borrow
            ACC = (Resta + 256) Mod 256
            ActualizarFlags ACC
            
        Case &H4 ' STA (Store Accumulator)
            MAR = Operando
            MBR = ACC
            EscribirMemoriaRAM MAR, MBR
            
        Case &H5 ' JMP (Salto Incondicional)
            PC = Operando
            
        Case &H6 ' JZ (Salto si Cero)
            If ZF = True Then
                PC = Operando
            End If
            
        Case &HF ' HLT (Halt)
            EstadoCPU = False
            MsgBox "CPU Detenida: Instrucción HLT ejecutada exitosamente.", vbInformation, "Simulador CPU"
            
        Case Else
            EstadoCPU = False
            MsgBox "Error de Arquitectura: Opcode desconocido (" & Hex(Opcode) & ")", vbCritical, "Fallo de CPU"
    End Select
    
    FaseActual = "FETCH"
    ActualizarInterfazGrafica
End Sub
```

### 4.6 Ejecución de Microórdenes y Señales de la Unidad de Control
Para llevar la simulación a un nivel aún más cercano al hardware real de compuertas lógicas y buses de tres estados (*tri-state buffers*), VBA puede modelar la **palabra de control (Control Word)**.

Una palabra de control típica de 12 bits para una máquina como SAP-1 se define mediante banderas binarias booleanas:
```vb
Public Type MicroOrdenes
    Cp As Boolean  ' Habilita incremento de Contador de Programa (PC_inc)
    Ep As Boolean  ' Habilita salida del PC hacia el Bus (PC_out)
    Lm As Boolean  ' Carga dirección en MAR (MAR_in)
    Ce As Boolean  ' Habilita lectura de RAM (RAM_read)
    Li As Boolean  ' Carga dato en IR (IR_in)
    Ei As Boolean  ' Habilita salida del operando del IR hacia el Bus (IR_out)
    La As Boolean  ' Carga dato en Acumulador (ACC_in)
    Ea As Boolean  ' Habilita salida del Acumulador hacia el Bus (ACC_out)
    Su As Boolean  ' Selecciona Resta en lugar de Suma en ALU (ALU_sub)
    Eu As Boolean  ' Habilita salida de la ALU hacia el Bus (ALU_out)
    Lb As Boolean  ' Carga registro B (B_in)
    Lo As Boolean  ' Carga registro de salida (OUT_in)
End Type
```
En cada ciclo de reloj $T_n$, la Unidad de Control calcula y publica la palabra de control. Una celda o fila en Excel puede mostrar en tiempo real los valores binarios `1` o `0` de cada señal, iluminando de color rojo o verde qué compuertas del circuito integrado están activadas en cada milisegundo de ejecución.

### 4.7 Código VBA Completo y Funcional: Simulador de CPU Tipo SAP-1

A continuación se presenta un módulo VBA autocontenido que puede ser insertado directamente en cualquier libro de Microsoft Excel para convertirlo en un simulador funcional de una CPU de 8 bits con memoria de 16 palabras:

```vb
' ===========================================================================
' SIMULADOR DE ARQUITECTURA DE COMPUTADORAS EN EXCEL VBA
' Modelo: CPU didáctica tipo Von Neumann / SAP-1 de 8 bits
' ===========================================================================
Option Explicit

' Declaración de Registros de Hardware
Public PC  As Byte
Public MAR As Byte
Public MBR As Byte
Public IR  As Byte
Public ACC As Byte
Public ZF  As Boolean
Public CF  As Boolean
Public EstadoCPU As Boolean

' Configuración de la Hoja de Excel (Coordenadas de la Interfaz)
Private Const FILA_INICIO_RAM As Integer = 5
Private Const COL_DIRECCION   As Integer = 2  ' Columna B: Dirección (0 a 15)
Private Const COL_DATO_HEX    As Integer = 3  ' Columna C: Dato/Instrucción Hex
Private Const COL_MNEMONICO   As Integer = 4  ' Columna D: Mnemónico ASM

' Coordenadas de celdas donde se muestran los Registros en Excel
Private Const CELDA_PC  As String = "G5"
Private Const CELDA_MAR As String = "G6"
Private Const CELDA_MBR As String = "G7"
Private Const CELDA_IR  As String = "G8"
Private Const CELDA_ACC As String = "G9"
Private Const CELDA_ZF  As String = "G10"
Private Const CELDA_CF  As String = "G11"

' ---------------------------------------------------------------------------
' 1. INICIALIZACIÓN DEL SISTEMA Y RESET DE REGISTROS
' ---------------------------------------------------------------------------
Public Sub ResetCPU()
    PC = 0
    MAR = 0
    MBR = 0
    IR = 0
    ACC = 0
    ZF = False
    CF = False
    EstadoCPU = True
    
    ' Limpiar resaltado previo en la hoja de Excel
    Hoja1.Range("B5:E20").Interior.Color = xlNone
    ActualizarInterfaz
    MsgBox "CPU reiniciada: Registros en 0, PC apuntando a dirección 0x00.", vbInformation, "Simulador CPU"
End Sub

' ---------------------------------------------------------------------------
' 2. EJECUCIÓN DE UN CICLO DE RELOJ COMPLETO (FETCH - DECODE - EXECUTE)
' ---------------------------------------------------------------------------
Public Sub PasoCicloReloj()
    If Not EstadoCPU Then
        MsgBox "La CPU está detenida (HALT). Presione 'Reset' para reiniciar.", vbExclamation, "CPU Detenida"
        Exit Sub
    End If
    
    ' Limpiar color de la línea de memoria anterior
    Hoja1.Range("B5:E20").Interior.Color = xlNone
    
    ' ================= FASE 1: FETCH (Búsqueda) =================
    MAR = PC
    
    ' Resaltar visualmente en Excel la celda de memoria que se está leyendo
    Hoja1.Range(Hoja1.Cells(FILA_INICIO_RAM + MAR, COL_DIRECCION), _
                Hoja1.Cells(FILA_INICIO_RAM + MAR, COL_MNEMONICO)).Interior.Color = RGB(255, 255, 150)
    
    ' Lectura de Memoria RAM (MBR <- RAM[MAR])
    Dim StrDato As String
    StrDato = Trim(CStr(Hoja1.Cells(FILA_INICIO_RAM + MAR, COL_DATO_HEX).Value))
    If StrDato = "" Then StrDato = "00"
    MBR = CByte("&H" & StrDato)
    
    ' Incremento del PC (PC <- PC + 1)
    PC = (PC + 1) Mod 16
    
    ' Carga en el Registro de Instrucción (IR <- MBR)
    IR = MBR
    
    ' ================= FASE 2 y 3: DECODE & EXECUTE ==============
    Dim Opcode   As Byte
    Dim Operando As Byte
    
    Opcode = IR \ 16      ' 4 bits superiores
    Operando = IR Mod 16  ' 4 bits inferiores
    
    Select Case Opcode
        Case &H0 ' NOP: Sin operación
            ' No altera estado
            
        Case &H1 ' LDA [dir]: Acumulador <- RAM[dir]
            MAR = Operando
            MBR = CByte("&H" & Hoja1.Cells(FILA_INICIO_RAM + MAR, COL_DATO_HEX).Value)
            ACC = MBR
            ActualizarFlags ACC
            
        Case &H2 ' ADD [dir]: Acumulador <- Acumulador + RAM[dir]
            MAR = Operando
            MBR = CByte("&H" & Hoja1.Cells(FILA_INICIO_RAM + MAR, COL_DATO_HEX).Value)
            Dim ResAdd As Integer
            ResAdd = CInt(ACC) + CInt(MBR)
            CF = (ResAdd > 255)
            ACC = ResAdd Mod 256
            ActualizarFlags ACC
            
        Case &H3 ' SUB [dir]: Acumulador <- Acumulador - RAM[dir]
            MAR = Operando
            MBR = CByte("&H" & Hoja1.Cells(FILA_INICIO_RAM + MAR, COL_DATO_HEX).Value)
            Dim ResSub As Integer
            ResSub = CInt(ACC) - CInt(MBR)
            CF = (ResSub < 0)
            ACC = (ResSub + 256) Mod 256
            ActualizarFlags ACC
            
        Case &H4 ' STA [dir]: RAM[dir] <- Acumulador
            MAR = Operando
            MBR = ACC
            Hoja1.Cells(FILA_INICIO_RAM + MAR, COL_DATO_HEX).Value = Right("0" & Hex(ACC), 2)
            
        Case &H5 ' JMP [dir]: PC <- Operando
            PC = Operando
            
        Case &H6 ' JZ [dir]: Salto condicional si Zero Flag = True
            If ZF Then PC = Operando
            
        Case &HF ' HLT: Detener procesador
            EstadoCPU = False
            ActualizarInterfaz
            MsgBox "Instrucción HLT: Fin del programa simulado.", vbInformation, "CPU Detenida"
            Exit Sub
            
        Case Else
            EstadoCPU = False
            ActualizarInterfaz
            MsgBox "Código de Operación inválido: 0x" & Hex(Opcode), vbCritical, "Error de CPU"
            Exit Sub
    End Select
    
    ActualizarInterfaz
End Sub

' ---------------------------------------------------------------------------
' 3. ACTUALIZACIÓN DE BANDERAS DE ESTADO (FLAGS)
' ---------------------------------------------------------------------------
Private Sub ActualizarFlags(ByVal Valor As Byte)
    ZF = (Valor = 0)
End Sub

' ---------------------------------------------------------------------------
' 4. SINCRONIZACIÓN DE REGISTROS CON LAS CELDAS DE EXCEL
' ---------------------------------------------------------------------------
Private Sub ActualizarInterfaz()
    Hoja1.Range(CELDA_PC).Value  = Right("0" & Hex(PC), 2)
    Hoja1.Range(CELDA_MAR).Value = Right("0" & Hex(MAR), 2)
    Hoja1.Range(CELDA_MBR).Value = Right("0" & Hex(MBR), 2)
    Hoja1.Range(CELDA_IR).Value  = Right("0" & Hex(IR), 2)
    Hoja1.Range(CELDA_ACC).Value = Right("0" & Hex(ACC), 2)
    Hoja1.Range(CELDA_ZF).Value  = IIf(ZF, 1, 0)
    Hoja1.Range(CELDA_CF).Value  = IIf(CF, 1, 0)
End Sub

' ---------------------------------------------------------------------------
' 5. EJECUCIÓN CONTINUA HASTA ENCONTRAR HLT
' ---------------------------------------------------------------------------
Public Sub EjecutarProgramaCompleto()
    ResetCPU
    Do While EstadoCPU
        PasoCicloReloj
        DoEvents ' Permite que Excel refresque visualmente la pantalla
    Loop
End Sub
```

---

## 5. Ventajas Pedagógicas y Limitaciones Técnicas de la Simulación en Excel/VBA

### Ventajas Pedagógicas:
1. **Transparencia Absoluta de la Memoria:**  
   A diferencia de un computador real donde la memoria RAM es invisible e inaccesible salvo mediante depuradores de bajo nivel (*GDB* o *OllyDbg*), en Excel la RAM está desplegada en celdas visibles con filas, columnas y colores.
2. **Control Temporal Total (Paso a Paso):**  
   Los estudiantes pueden pulsar un botón para ejecutar un pulso de reloj único, observando exactamente en qué momento el `PC` transfiere su valor al `MAR`, cuándo el dato viaja al `MBR` y cómo se activa la bandera de cero `ZF`.
3. **Sin Barreras de Instalación:**  
   No requiere compilar con GCC, configurar máquinas virtuales Linux ni instalar herramientas complejas de simulación de circuitos (como Logisim, Proteus o ModelSim). Funciona nativamente en cualquier estación de trabajo con Office.
4. **Programación Intuitiva de Microórdenes:**  
   El uso de VBA permite alterar con facilidad el microcódigo o agregar nuevas instrucciones (por ejemplo, `XOR`, `INC`, `DEC`) simplemente modificando el bloque `Select Case Opcode`.

### Limitaciones Técnicas:
1. **Velocidad de Reloj Simulada:**  
   Un procesador moderno funciona a frecuencias de gigahercios ($3.0 \times 10^9$ ciclos por segundo). Un simulador en Excel VBA, debido a la sobrecarga gráfica de refresco de pantalla de OLE/Excel y el despacho de P-Code, opera a frecuencias de unos pocos cientos o miles de ciclos por segundo (kilohercios).
2. **Capacidad de Memoria Práctica:**  
   Modelar gigabytes de RAM no es factible visualmente en una cuadrícula. Está diseñado óptimamente para espacios de memoria didácticos de 16 a 4096 palabras (4 a 12 bits de direccionamiento).
3. **Ausencia de Concurrencia a Nivel de Silicio:**  
   En un circuito integrado físico, muchas microórdenes ocurren en paralelo exacto a nivel de nanosegundos (ej. el incremento del PC y la decodificación de la instrucción). En VBA, todo se modela como código secuencial en un solo hilo.

---

## 6. Conclusiones

1. **Visual Basic como Lenguaje de Ingeniería:**  
   Aunque históricamente categorizado como una herramienta de desarrollo comercial rápido (RAD), el motor interno de Visual Basic clásico y VBA posee un control riguroso sobre estructuras de datos, direccionamiento de punteros indirectos (`VarPtr`, `BSTR`) e interoperabilidad con componentes COM y la API del sistema operativo.
2. **Potencia Didáctica de Excel y VBA:**  
   El uso de Excel como representación espacial de la memoria RAM y de VBA como Unidad de Control y ALU constituye uno de los métodos pedagógicos más efectivos para desmitificar el funcionamiento de un computador digital. Permite transformar conceptos abstractos como buses, señales de control, decodificación de opcodes y el ciclo de instrucción en fenómenos visuales comprensibles y manipulables en tiempo real.
3. **Comprensión Integral del Ciclo Von Neumann:**  
   La implementación paso a paso de las fases de Fetch, Decode y Execute y la descomposición en microoperaciones permite asimilar profundamente cómo el hardware ejecuta instrucciones binarias simples de manera coordinada para sustentar toda la pirámide del software moderno.

---

## 7. Bibliografía y Referencias Técnicas

1. **Stallings, W. (2018).** *Organización y Arquitectura de Computadores* (10ª ed.). Pearson Educación.
2. **Tanenbaum, A. S., & Austin, T. (2013).** *Structured Computer Organization* (6th ed.). Pearson.
3. **Malvino, A. P., & Brown, J. A. (1993).** *Digital Computer Electronics* (3rd ed., SAP-1 Architecture). McGraw-Hill.
4. **Cooper, A. (1995).** *About Face: The Essentials of User Interface Design*. IDG Books.
5. **Microsoft Corporation.** *Visual Basic 6.0 Language Reference & Runtime Architecture Documentation*. MSDN Archive.
6. **Patt, Y. N., & Patel, S. J. (2004).** *Introduction to Computing Systems: From Bits and Gates to C and Beyond* (LC-3 Architecture). McGraw-Hill.
