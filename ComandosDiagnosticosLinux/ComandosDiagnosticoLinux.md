# Comandos de Diagnóstico en Linux

Este documento explica, campo por campo, la información que muestran los comandos de diagnóstico de hardware en Linux, agrupados en las tres categorías de la presentación: **Procesador**, **Buses y Almacenamiento**, y **Memoria y Sistema**.

---

## 1. Procesador — `lscpu`

El comando `lscpu` lee la información de la CPU desde `/proc/cpuinfo` y `sysfs`, y la presenta de forma resumida y legible. Salida capturada:

```
Architecture:            x86_64
CPU op-mode(s):          32-bit, 64-bit
Address sizes:           39 bits physical, 48 bits virtual
Byte Order:              Little Endian
CPU(s):                  2
On-line CPU(s) list:     0,1
Vendor ID:                GenuineIntel
Model name:              13th Gen Intel(R) Core(TM) i7-1355U
CPU family:              6
Model:                   186
Thread(s) per core:      1
Core(s) per socket:      2
Socket(s):               1
Stepping:                3
BogoMIPS:                5222.40
```

| Campo | Significado |
|---|---|
| **Architecture** | Arquitectura del set de instrucciones de la CPU. `x86_64` indica un procesador de 64 bits compatible con la familia x86. |
| **CPU op-mode(s)** | Modos de operación que soporta el procesador. `32-bit, 64-bit` significa que puede ejecutar tanto programas de 32 como de 64 bits. |
| **Address sizes** | Ancho, en bits, de las direcciones de memoria física y virtual que puede manejar la CPU. `39 bits physical` = hasta 512 GB de RAM direccionable; `48 bits virtual` = hasta 256 TB de espacio de direcciones virtuales por proceso. |
| **Byte Order** | Orden en que se almacenan los bytes en memoria. `Little Endian` significa que el byte menos significativo se guarda primero (estándar en x86/x86_64). |
| **CPU(s)** | Número total de CPUs lógicas visibles para el sistema operativo (resultado de socket × núcleos por socket × hilos por núcleo). En este caso, 2. |
| **On-line CPU(s) list** | Lista de los identificadores de las CPUs que están actualmente activas/en uso (`0,1`). Es posible desactivar CPUs lógicas manualmente en Linux, y aquí se vería reflejado. |
| **Vendor ID** | Fabricante del procesador, tal como lo reporta la instrucción `CPUID`. `GenuineIntel` identifica a Intel (AMD usaría `AuthenticAMD`). |
| **Model name** | Nombre comercial completo del procesador. `13th Gen Intel(R) Core(TM) i7-1355U` es un i7 de 13ª generación, de bajo consumo (sufijo "U"). |
| **CPU family** | Número de familia del procesador según la clasificación interna de Intel/AMD, usado por el kernel para identificar generaciones de arquitectura. |
| **Model** | Número de modelo específico dentro de la familia, junto con la familia identifica de forma única el diseño del silicio. |
| **Thread(s) per core** | Hilos de ejecución por núcleo físico. `1` indica que **Hyper-Threading/SMT está deshabilitado o no disponible** en esta vista (normalmente sería 2 en un i7 con Hyper-Threading activo). |
| **Core(s) per socket** | Núcleos físicos por cada zócalo/CPU física. Aquí hay 2 núcleos por socket. |
| **Socket(s)** | Número de procesadores físicos (zócalos) instalados en la placa. `1` = un solo chip físico. |
| **Stepping** | Revisión de fabricación (versión menor) del silicio dentro del mismo modelo; corrige errores o mejora el proceso de fabricación. |
| **BogoMIPS** | Medida aproximada ("bogus MIPS") de la velocidad de la CPU calculada por el kernel al arrancar. No es una medida real de rendimiento, solo se usa para calibrar bucles de espera internos del kernel. |

> **Nota:** como esta captura fue tomada dentro de una máquina virtual (se ve más abajo `Hypervisor vendor: KVM`), los valores de `CPU(s)`, `Core(s) per socket` y `Thread(s) per core` reflejan los recursos **asignados a la VM**, no necesariamente el hardware físico completo del i7-1355U (que en la realidad tiene más núcleos e Hyper-Threading).

### Flags

```
Flags: fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36
clflush mmx fxsr sse sse2 ss ht syscall nx rdtscp lm constant_tsc rep_good nopl
xtopology nonstop_tsc cpuid tsc_known_freq pni pclmulqdq ssse3 fma cx16 pcid
sse4_1 sse4_2 movbe popcnt aes xsave avx f16c rdrand hypervisor lahf_lm abm
3dnowprefetch invpcid_single ibrs_enhanced fsgsbase bmi1 avx2 bmi2 invpcid
rdseed adx clflushopt sha_ni arat md_clear flush_l1d arch_capabilities
```

Son las **banderas de características (features)** que la CPU soporta a nivel de instrucciones. Algunas de las más relevantes:

- **fpu**: unidad de punto flotante integrada.
- **tsc**: Time Stamp Counter, contador de ciclos de reloj de alta precisión.
- **pae**: Physical Address Extension, permite direccionar más de 4 GB de RAM en modo 32 bits.
- **mmx / sse / sse2 / sse4_1 / sse4_2 / avx / avx2**: conjuntos de instrucciones para cálculo vectorial/multimedia, muy usados en compresión de video, IA y cálculo científico.
- **aes**: aceleración por hardware para cifrado/descifrado AES.
- **lm**: Long Mode, es decir, soporte real para ejecutar código de 64 bits.
- **nx**: bit "No-Execute", protección de seguridad que evita ejecutar código en zonas de memoria marcadas solo como datos.
- **hypervisor**: indica que el sistema está corriendo **dentro de una máquina virtual**, no en hardware físico directo.
- **rdrand / rdseed**: generadores de números aleatorios por hardware.
- **ibrs_enhanced**: mitigación por hardware contra ataques de ejecución especulativa tipo Spectre.

### Virtualization features

| Campo | Significado |
|---|---|
| **Hypervisor vendor** | Software de virtualización que está gestionando esta máquina virtual. `KVM` (Kernel-based Virtual Machine, el hipervisor nativo de Linux). |
| **Virtualization type** | Tipo de virtualización usada. `full` significa virtualización completa (el sistema invitado no sabe, salvo por estas señales, que está virtualizado). |

### Caches (sum of all)

| Campo | Significado |
|---|---|
| **L1d** | Caché de nivel 1 para **datos**. Es la más rápida y pequeña, dedicada por núcleo. `96 KiB (2 instances)` = 48 KiB por núcleo × 2 núcleos. |
| **L1i** | Caché de nivel 1 para **instrucciones**, separada de la de datos. |
| **L2** | Caché de nivel 2, más grande y algo más lenta que L1, también suele ser por núcleo o compartida entre pares de núcleos. |
| **L3** | Caché de nivel 3 (o "Last Level Cache"), la más grande y compartida entre todos los núcleos del socket. Actúa como colchón entre la CPU y la RAM. |

### NUMA

| Campo | Significado |
|---|---|
| **NUMA node(s)** | Número de nodos NUMA (Non-Uniform Memory Access). En sistemas multi-socket, cada CPU física suele tener su propio banco de memoria "cercano". Aquí solo hay 1 nodo, típico de sistemas de un solo socket o VMs pequeñas. |
| **NUMA node0 CPU(s)** | Qué CPUs lógicas pertenecen a ese nodo NUMA (`0,1`, es decir, todas). |

### Vulnerabilities

Lista de vulnerabilidades de seguridad conocidas a nivel de hardware (relacionadas con ejecución especulativa) y el estado de mitigación de cada una:

| Campo | Significado |
|---|---|
| **Itlb multihit** | Vulnerabilidad relacionada con el Instruction TLB en entornos virtualizados. `Not affected`: este procesador no es vulnerable. |
| **L1tf** | *L1 Terminal Fault*, fuga de datos a través de la caché L1. `Not affected`. |
| **Mds** | *Microarchitectural Data Sampling*, fuga de datos entre procesos vía buffers internos de la CPU. `Not affected`. |
| **Meltdown** | Vulnerabilidad clásica de 2018 que permite leer memoria del kernel desde espacio de usuario. `Not affected`. |
| **Spec store bypass** | Vulnerabilidad de ejecución especulativa sobre almacenamiento en memoria. `Vulnerable`: **no** está mitigada en este sistema. |
| **Spectre v1** | Fuga de datos por predicción de saltos condicionales. Estado: `Mitigation; usercopy/swapgs barriers and __user pointer sanitization` — mitigada mediante barreras de software. |
| **Spectre v2** | Fuga de datos por predicción de rama indirecta. Estado: `Mitigation; Enhanced IBRS, RSB filling` — mitigada por hardware (IBRS) y software. |
| **Srbds** | *Special Register Buffer Data Sampling*, fuga de datos del generador de números aleatorios interno. `Not affected`. |
| **Tsx async abort** | Vulnerabilidad relacionada con Intel TSX (transacciones de memoria). `Not affected`. |

---

## 2. Buses y Almacenamiento — `lspci` y `lsblk`

### `lspci`

Muestra todos los dispositivos conectados a los **buses PCI/PCIe** del sistema (tarjetas gráficas, controladoras de red, USB, SATA, chipset, etc.). Cada línea sigue el formato:

```
[bus]:[dispositivo].[función]  Clase de dispositivo: Fabricante Modelo (rev revisión)
```

Ejemplos de la captura y su explicación:

| Línea | Explicación |
|---|---|
| `00:00.0 Host bridge: Intel Corporation 440FX...` | El **puente principal** que conecta la CPU con el resto del sistema (memoria y bus PCI). En este caso emula el chipset clásico Intel 440FX, típico de máquinas virtuales (QEMU/VirtualBox). |
| `00:01.0 ISA bridge: Intel Corporation 82371SB PIIX3...` | Puente que da compatibilidad con el antiguo bus **ISA**, heredado por razones de compatibilidad. |
| `00:01.1 IDE interface: Intel Corporation 82371AB/EB/MB PIIX4 IDE` | Controladora de discos con interfaz **IDE/PATA** emulada. |
| `00:02.0 VGA compatible controller: VMware SVGA II Adapter` | La **tarjeta gráfica virtual**, en este caso emulada por el hipervisor (aunque diga "VMware", muchos hipervisores emulan este chip por compatibilidad). |
| `00:03.0 Ethernet controller: Intel Corporation 82540EM Gigabit...` | Tarjeta de **red Ethernet** virtual (emula el chip real Intel 82540EM). |
| `00:04.0 System peripheral: InnoTek Systemberatung GmbH VirtualBox Guest Service` | Confirma que la VM corre bajo **VirtualBox** ("InnoTek" es la empresa original creadora de VirtualBox); es el servicio de integración huésped. |
| `00:05.0 Multimedia audio controller: Intel Corporation 82801AA AC'97...` | Tarjeta de **sonido** virtual, con chip de audio AC'97 emulado. |
| `00:06.0 USB controller: Apple Inc. KeyLargo/Intrepid USB` | Controladora **USB** (aquí aparece con identificador de Apple por cómo el hipervisor reporta el dispositivo). |
| `00:07.0 Bridge: Intel Corporation 82371AB/EB/MB PIIX4 ACPI` | Puente relacionado con la gestión de energía **ACPI**. |
| `00:0b.0 USB controller: ... USB2 EHCI Controller` | Segunda controladora **USB 2.0** (estándar EHCI). |
| `00:0d.0 SATA controller: ... [AHCI mode]` | Controladora de discos con interfaz **SATA** en modo AHCI (el modo estándar y más moderno de acceso a discos). |

En resumen, `lspci` sirve para saber **qué chips y controladoras** tiene el equipo (o la VM) para gráficos, red, audio, USB y almacenamiento, junto con su fabricante y revisión de hardware.

### `lsblk`

Muestra los **dispositivos de bloque** (discos, particiones, unidades ópticas) en forma de árbol.

```
NAME    MAJ:MIN RM  SIZE RO TYPE MOUNTPOINTS
loop0     7:0    0    4K  1 loop /snap/bare/5
...
sda       8:0    0  500G  0 disk
├─sda1    8:1    0  220.6G 0 part /
├─sda2    8:2    0   286M  0 part /boot
├─sda3    8:3    0    95M  0 part
├─sda4    8:4    0   8.9G  0 part [SWAP]
└─sda5    8:5    0  270.1G 0 part /home
sr0      11:0    1  1024M  0 rom
```

| Columna | Significado |
|---|---|
| **NAME** | Nombre del dispositivo (`sda` = primer disco SATA/SCSI; `sda1`, `sda2`... = sus particiones; `loopN` = dispositivos de bucle; `sr0` = unidad óptica). |
| **MAJ:MIN** | Números **mayor:menor** (major:minor) del dispositivo en el kernel; identifican de forma única el driver y la instancia del dispositivo. |
| **RM** | *Removable*. `1` = dispositivo extraíble (como `sr0`, el lector de CD/DVD); `0` = fijo. |
| **SIZE** | Tamaño total del dispositivo o partición. |
| **RO** | *Read Only*. `1` = solo lectura (los `loopN`, imágenes de paquetes snap, son de solo lectura); `0` = lectura/escritura. |
| **TYPE** | Tipo de dispositivo: `disk` (disco completo), `part` (partición), `loop` (dispositivo de bucle, usado por ejemplo por Snap para montar paquetes), `rom` (unidad óptica). |
| **MOUNTPOINTS** | Punto(s) de montaje actual del sistema de archivos, si está montado. |

Explicación de las particiones de `sda` (disco de 500 GB):

- **sda1 (220.6G) → `/`**: partición raíz, donde vive el sistema operativo.
- **sda2 (286M) → `/boot`**: contiene el kernel y los archivos necesarios para arrancar el sistema.
- **sda3 (95M)**: partición sin punto de montaje visible aquí (probablemente relacionada con el gestor de arranque, tipo EFI/boot auxiliar).
- **sda4 (8.9G) → [SWAP]**: espacio de intercambio (memoria virtual en disco).
- **sda5 (270.1G) → `/home`**: partición donde se guardan los archivos y configuraciones de los usuarios.

Los `loopN` que aparecen (`/snap/...`) son imágenes de solo lectura montadas por el sistema de paquetes **Snap** de Ubuntu (cada aplicación snap se monta como su propio "disco" virtual).

---

## 3. Memoria y Sistema — `lsmem` y `sudo lshw`

### `lsmem`

Muestra los **rangos de memoria física (RAM)** reconocidos por el kernel y su estado.

```
RANGE                                  SIZE   STATE   REMOVABLE  BLOCK
0x0000000000000000-0x00000000dfffffff  3.5G   online  yes        0-27
0x0000000100000000-0x000000011fffffff  512M   online  yes        32-35

Memory block size:       128M
Total online memory:        4G
Total offline memory:       0B
```

| Campo | Significado |
|---|---|
| **RANGE** | Rango de direcciones físicas de memoria (en hexadecimal) que cubre ese bloque. |
| **SIZE** | Tamaño de ese rango de memoria. |
| **STATE** | Estado del bloque: `online` = actualmente disponible y usable por el sistema; `offline` = presente físicamente pero no en uso (memoria "hot-plug" desactivada). |
| **REMOVABLE** | Indica si ese bloque de memoria se puede quitar en caliente (hot-unplug) sin apagar el sistema. `yes` en ambos casos porque es una VM que soporta memoria dinámica. |
| **BLOCK** | Número(s) de bloque de memoria dentro del sistema, según el tamaño de bloque definido (`Memory block size`). |
| **Memory block size** | Tamaño en que el kernel divide la RAM para gestionarla en bloques (aquí, 128 MB por bloque). |
| **Total online memory** | Suma total de memoria actualmente activa y disponible. `4G`. |
| **Total offline memory** | Memoria presente pero no habilitada actualmente. `0B`, es decir, toda la RAM asignada está en uso. |

> Nota: hay dos rangos porque existe un "hueco" de direcciones reservado por el hardware/BIOS entre ambos (típico entre ~3.5 GB y 4 GB, reservado para direccionamiento de dispositivos PCI/BIOS), por lo que la memoria continúa en un segundo rango a partir de `0x100000000` (4 GB).

### `sudo lshw`

`lshw` (*Hardware Lister*) es el comando de diagnóstico **más completo** de los cuatro: genera un inventario de **todo** el hardware en forma de **árbol jerárquico**, donde cada nodo "cuelga" del bus o componente al que está conectado físicamente. Se ejecuta con `sudo` porque necesita permisos de administrador para leer información sensible (tablas DMI/SMBIOS de la BIOS, números de serie, registros de dispositivos).

Cada nodo del árbol repite un mismo conjunto de campos (no se repite la explicación en cada rama para no alargar el documento):

| Campo | Significado |
|---|---|
| **description / product / vendor** | Qué es el dispositivo, su modelo comercial y su fabricante. |
| **physical id / bus info** | Identificador dentro del árbol y dirección exacta en el bus (ej. `pci@0000:00:02.0`, `scsi@2:0.0.0`). |
| **logical name** | Nombre que le asigna el kernel (`/dev/sda`, `enp0s3`, `/dev/input/event0`, etc.). |
| **version / serial** | Revisión de hardware/firmware y número de serie único. |
| **size / capacity** | Tamaño o velocidad actual / máximo soportado por el dispositivo. |
| **capabilities** | Características o estándares que soporta (ej. `usb-2.00`, `ext4`, `bus_master`). |
| **configuration** | Parámetros con los que está funcionando ahora mismo (driver, IP, modo, punto de montaje, etc.). |
| **resources** | Recursos del sistema que usa: IRQ, direcciones de memoria o puertos de E/S. |

A continuación, lo que muestra **esta captura en particular**, rama por rama:

**Raíz del sistema (`osboxes`) → `*-core`**

- El equipo es una **máquina virtual VirtualBox** (`product: VirtualBox`, `vendor: innotek GmbH`), de 64 bits, con un UUID propio de la VM.
- `*-core` (placa base): motherboard virtual de Oracle/VirtualBox.
- `*-firmware`: la **BIOS** emulada (VirtualBox, fecha 12/01/2006, 128 KiB), con capacidades como `bootselect` (elegir dispositivo de arranque) y `acpi`.
- `*-memory`: confirma los **4 GiB** de RAM asignados a la VM (coincide con el `Total online memory` de `lsmem`).
- `*-cpu`: repite lo visto en `lscpu` (i7-1355U, Intel, 64 bits) pero agrega `configuration: microcode=4294967295`, la versión del microcódigo cargado en la CPU (parches de bajo nivel del fabricante).

**`*-pci` (Host bridge, chipset 440FX) y sus ramas**

- `*-isa`: puente ISA con dos dispositivos **PnP** (Plug and Play) hijos: el controlador del **teclado** (`i8042 kbd`) y del **puerto auxiliar/PS2** (`i8042 aux`).
- `*-ide` → `*-cdrom`: interfaz IDE que sirve la unidad óptica virtual (`/dev/sr0`, `/dev/cdrom`), reportada como lector de DVD sin disco insertado (`status=nodisc`).
- `*-display`: la tarjeta gráfica (VMware SVGA II), igual que en `lspci`, pero con detalle extra: resolución configurada (`2048,2048`), driver (`vmwgfx`) y los rangos de memoria que usa.
- `*-network`: la tarjeta de red Intel emulada, con su **dirección MAC** (`08:00:27:5d:86:11`), nombre de interfaz (`enp0s3`), IP asignada (`10.0.2.15`, típica de NAT en VirtualBox) y velocidad de enlace (`1Gbit/s`).
- `*-generic`: la integración de mouse de VirtualBox (`vboxguest`), que expone el puntero del host directamente dentro del invitado.
- `*-multimedia`: la tarjeta de sonido AC'97, con sus dispositivos ALSA (`/dev/snd/...`).
- `*-usb:0` y `*-usb:1`: **dos controladoras USB distintas** — una OHCI (USB 1.1, 12 Mbit/s) con un dispositivo hijo *VirtualBox USB Tablet* (así es como VirtualBox emula el mouse/tableta), y otra EHCI (USB 2.0, 480 Mbit/s) sin dispositivos conectados.
- `*-bridge`: puente ACPI/SMBus (gestión de energía).
- `*-sata` → `*-disk` → `*-volume:0` a `*-volume:4`: la controladora SATA en modo AHCI, con el disco virtual `VBOX HARDDISK` de 500 GB (`/dev/sda`, particionado en GPT), y sus 5 particiones detalladas: `/` (220 GiB, ext4), `/boot` (286 MiB, ext4), una partición de arranque BIOS/EFI (94 MiB, sin sistema de archivos), el swap (≈9 GiB) y `/home` (270 GiB, ext4) — coincide exactamente con lo visto en `lsblk`, pero aquí además se ve el **UUID/serial de cada partición** y sus fechas de creación y último montaje.

**`*-input:0` a `*-input:4`**

Los dispositivos de entrada del sistema: botón de encendido (*Power Button*), botón de suspensión (*Sleep Button*), el teclado (*AT Translated Set 2 keyboard*), el *Video Bus* (canal usado para eventos de brillo/pantalla) y el mouse PS/2 (*ImExPS/2 Generic Explorer Mouse*).

> En este equipo no aparece una sección `*-power` (batería), porque al ser una máquina virtual sin batería física, `lshw` simplemente omite esa rama.

En conjunto, mientras `lscpu`, `lspci`, `lsblk` y `lsmem` son comandos **especializados** (cada uno en su propia pieza de hardware), `lshw` actúa como una **radiografía completa** del equipo: combina en un solo árbol toda la información que los demás entregan por separado, más algunos datos extra (microcódigo, UUIDs de partición, fechas de montaje). Variantes útiles: `sudo lshw -short` (tabla resumida) y `sudo lshw -class <clase>` (filtrar por una sola categoría, ej. `disk`, `network`).
