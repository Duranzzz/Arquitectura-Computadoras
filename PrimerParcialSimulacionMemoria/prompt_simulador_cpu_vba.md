# Prompt para construir el Simulador de CPU (Von Neumann) en Excel con VBA

## Contexto
Tengo un archivo `1788990394006_SimuladorMemoriaCPU.xlsx` en mi carpeta de proyectos. Es un **boceto visual** (celdas coloreadas simulando un panel de control) de un simulador educativo de arquitectura de computadores, modelo de 3 direcciones (Eduardo Alcalde / Miguel García López). Contiene dos hojas:

- **Simulador_CPU**: registros de la Unidad de Control (C5:C11), Unidad Aritmético-Lógica (G5:G11), Interfaz de Memoria (K5:K11), tabla de Memoria Central (B14:F20) y un "panel de macros" dibujado con celdas de color (H13:K18) que todavía NO están conectadas a ningún código.
- **Tabla_de_Traza**: encabezados (A3:K3) para registrar el historial de ejecución paso a paso, con una sola fila de ejemplo (T0).

Necesito que conviertas este boceto en un **simulador funcional real con macros VBA**, con botones de verdad, animación visual del flujo de datos (buses/flechas), y un ciclo de instrucción completo (fetch + decodificación + ejecución).

## 1. Conversión de formato — PASO OBLIGATORIO PRIMERO
El archivo es `.xlsx`, que **no admite macros VBA**. Antes de cualquier otra cosa:
- Guarda una copia como `SimuladorMemoriaCPU.xlsm` (Libro de Excel habilitado para macros).
- Todo el trabajo posterior se hace sobre el `.xlsm`, no sobre el `.xlsx` original (consérvalo como respaldo).

**Si tu herramienta no puede generar o inyectar un proyecto VBA real dentro de un `.xlsm`** (por ejemplo, si solo puedes editar celdas con `openpyxl`/`pandas` y no puedes escribir el binario `vbaProject.bin`), dime esto explícitamente y en vez de fallar en silencio, hazme lo siguiente:
1. Deja el archivo `.xlsm` con toda la estructura de celdas, nombres de rango, botones de formulario (sin macro asignada aún) y formato listos.
2. Entrégame el código VBA completo como un archivo de texto separado (`.bas` o `.txt`), organizado en módulos.
3. Dame instrucciones exactas paso a paso de cómo importarlo yo mismo en Excel (Alt+F11 → clic derecho en el proyecto → Importar archivo → seleccionar el `.bas`; luego asignar cada macro a su botón con clic derecho → Asignar macro).

## 2. Rangos con nombre (crear todos estos antes de escribir cualquier macro)
En la hoja Simulador_CPU:
| Nombre | Celda | Significado |
|---|---|---|
| `CP` | C5 | Contador de Programa |
| `RI` | C6 | Registro de Instrucción (texto completo, ej. "SUMAR 033 992 993") |
| `CO` | C7 | Código de Operación decodificado |
| `OP1` | C8 | Dirección Operando 1 |
| `OP2` | C9 | Dirección Operando 2 |
| `OP3` | C10 | Dirección Resultado |
| `Fase` | C11 | Estado actual de la máquina (motor del state machine) |
| `REN1` | G5 | Registro Entrada 1 |
| `REN2` | G6 | Registro Entrada 2 |
| `COP` | G7 | Operación activa en la UAL |
| `Ac` | G8 | Acumulador |
| `FlagZ` | G9 | Zero flag |
| `FlagC` | G10 | Carry flag |
| `FlagS` | G11 | Sign flag |
| `RDM` | K5 | Registro Dirección de Memoria |
| `RIM` | K6 | Registro Información de Memoria |
| `SelectorMem` | K7 | Selector de memoria (ACTIVO/INACTIVO) |
| `BusDirecciones` | K8 | Texto descriptivo del bus activo |
| `BusDatos` | K9 | Texto descriptivo del bus activo |
| `BusControl` | K10 | LECTURA/ESCRITURA |
| `Reloj` | K11 | Paso Tn actual |
| `TablaMemoria` | B15:C20 (ampliar según necesidad) | Dirección→Contenido |
| `Consola` | H18 | Mensaje de estado para el usuario |

En la hoja Tabla_de_Traza:
| Nombre | Rango |
|---|---|
| `EncabezadosTraza` | A3:K3 |
| `InicioTraza` | A4 (primera fila de datos, para saber dónde empezar a insertar) |

## 3. Conjunto de instrucciones a soportar
Implementa un decodificador que reconozca al menos estos opcodes (formato 3 direcciones: `OPCODE OP1 OP2 OP3`, salvo excepciones anotadas):
- `SUMAR OP1 OP2 OP3` → Mem[OP3] = Mem[OP1] + Mem[OP2]
- `RESTAR OP1 OP2 OP3` → Mem[OP3] = Mem[OP1] - Mem[OP2]
- `CARGAR OP1` (1 dirección) → Ac = Mem[OP1]
- `ALMACENAR OP1` (1 dirección) → Mem[OP1] = Ac
- `SALTAR OP1` (1 dirección) → CP = OP1 (incondicional)
- `SALTAR_SI_CERO OP1` (1 dirección) → si FlagZ=1, CP = OP1
- `HLT` (0 direcciones) → detiene la ejecución, Fase = "FIN"

Si alguna de estas no aplica a lo que se enseña en mi curso, o si falta alguna, avísame antes de programar todo el set — puedo ajustarlo.

## 4. Máquina de estados del ciclo de instrucción (una macro = un microorden)
Implementa `Sub PasoCicloReloj()` como un `Select Case` sobre `Fase`, con esta secuencia:
1. `INICIO` → `RDM = CP` → Fase = `FETCH_RIM`
2. `FETCH_RIM` → `RIM = BuscarEnMemoria(RDM)` → Fase = `FETCH_RI`
3. `FETCH_RI` → `RI = RIM` → Fase = `DECODIFICAR`
4. `DECODIFICAR` → separar `RI` en `CO`, `OP1`, `OP2`, `OP3` (usar `Split(RI, " ")`) → Fase = `INC_CP`
5. `INC_CP` → `CP = CP + 1` → Fase = `EJECUTAR`
6. `EJECUTAR` → `Select Case CO` con la lógica de cada instrucción del punto 3 → si CO = "HLT", Fase = `FIN`; si no, Fase = `INICIO`
7. `FIN` → mostrar `MsgBox` de programa terminado y deshabilitar más pasos hasta `ResetCPU`

Cada llamada a `PasoCicloReloj` debe terminar llamando a `RegistrarPasoEnTraza` (agrega una fila nueva a Tabla_de_Traza con: Paso Tn, Fase, notación RTL de la microorden ejecutada, y el valor actual de CP/RDM/RIM/RI/REN1/REN2/Ac/Mem[OP3]).

## 5. Macros requeridas (módulos VBA)
- `Sub PasoCicloReloj()` — descrita arriba.
- `Sub EjecutarInstruccionCompleta()` — llama a `PasoCicloReloj` en un `Do While Fase <> "FIN"` hasta terminar una instrucción completa (o el programa completo, aclárame cuál prefieres — sugiero: una instrucción completa por clic, para no perder el detalle educativo).
- `Sub ResetCPU()` — regresa todos los registros a su estado inicial (CP=006, RDM=000, RIM=0, etc.), limpia Tabla_de_Traza dejando solo el encabezado y la fila T0, y borra los resaltados de color de las celdas.
- `Function BuscarEnMemoria(direccion As String) As String` — busca en `TablaMemoria` y regresa el contenido; si no existe, regresa `"ERROR: Dirección no encontrada"` y lo muestra en `Consola`.
- `Sub EscribirEnMemoria(direccion As String, valor As String)` — igual que la anterior pero para escritura (ALMACENAR).
- `Sub DecodificarInstruccion()` — separa el texto de `RI` en sus componentes.
- `Sub RegistrarPasoEnTraza()` — agrega una fila al final de Tabla_de_Traza.
- `Sub ResaltarCambio(rango As Range)` — pinta temporalmente de amarillo (`RGB(255, 235, 59)`) la celda que acaba de cambiar, espera ~0.6-1 seg con `Application.Wait`, y regresa a blanco. Debe llamarse desde cada paso del `Select Case` sobre la celda que se acaba de modificar (ej. después de `RDM = CP`, resaltar el rango `RDM`).

## 6. Botones (controles reales, no celdas pintadas)
Inserta **controles de formulario** (Insertar → Formularios → Botón, NO ActiveX) posicionados exactamente sobre estas celdas ya diseñadas, y asígnales su macro:
- Sobre H14 ("▶ PasoCicloReloj") → asignar `PasoCicloReloj`
- Sobre H15 ("⏩ EjecutarInstruccionCompleta") → asignar `EjecutarInstruccionCompleta`
- Sobre J14 ("🔄 ResetCPU") → asignar `ResetCPU`

Mantén el texto y color que ya tienen esas celdas; el botón debe cubrir la celda completa para que se vea integrado al panel.

## 7. Animación del flujo de datos (buses/flechas)
Excel no anima de forma nativa, así que simula el efecto con **Formas (Insertar → Shapes → Flecha)** dibujadas sobre el diagrama, conectando visualmente:
- CP → RDM
- RDM → Memoria Central
- Memoria Central → RIM
- RIM → RI
- OP1/OP2 (memoria) → REN1/REN2
- REN1+REN2 → C.OP → Ac

Nombra cada flecha de forma identificable (`FlechaCP_RDM`, `FlechaRDM_Mem`, etc.) para poder referenciarlas desde VBA. En cada paso del `Select Case` de `PasoCicloReloj`, activa la flecha correspondiente cambiando su color/grosor (`.Line.ForeColor.RGB` y `.Line.Weight`) por ~1 segundo y luego regrésala a su estado neutro — esto simula el "viaje" del dato por el bus sin necesitar animaciones reales.

## 8. Protección de la hoja
Protege la hoja Simulador_CPU dejando **solo editable** el rango de memoria de datos/programa (`B15:C20` o el rango que definas como `TablaMemoria`), para que el usuario pueda cargar su propio programa sin romper el resto del panel.

## 9. Qué necesito que me confirmes o me preguntes antes de generar todo
- Si tu entorno puede o no escribir macros VBA reales dentro del binario del `.xlsm` (ver sección 1).
- Si el conjunto de instrucciones de la sección 3 coincide con lo que necesito, o si debo ajustarlo.
- Si `EjecutarInstruccionCompleta` debe ejecutar una sola instrucción o el programa completo hasta HLT.

Entrégame al final: el archivo `.xlsm` (o el `.xlsx` + el código `.bas` + instrucciones manuales, según lo que puedas hacer), y un resumen breve de qué macros quedaron conectadas a qué botones.
