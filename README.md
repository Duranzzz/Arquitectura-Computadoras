# Arquitectura de Computadoras

Repositorio académico de la materia **Arquitectura de Computadoras** correspondiente a la carrera de Ingeniería en Inteligencia Artificial en la **Universidad Católica Boliviana "San Pablo"**, sede Santa Cruz de la Sierra.

Aquí se centralizan las prácticas, programas, scripts, reportes e investigaciones desarrolladas durante el cursado de la materia. La gestión y seguimiento de las actividades se organiza mediante un tablero Kanban en GitHub Projects.

---

## Tabla de Contenidos

- [Información Académica](#información-académica)
- [Descripción de la Materia](#descripción-de-la-materia)
- [Organización del Trabajo — Kanban](#organización-del-trabajo--kanban)
- [Formato de Tareas](#formato-de-tareas)

---

## Información Académica

| Campo              | Detalle                                                    |
|--------------------|------------------------------------------------------------|
| **Universidad**    | Universidad Católica Boliviana "San Pablo" — Sede Santa Cruz |
| **Carrera**        | Ingeniería en Inteligencia Artificial                      |
| **Materia**        | Arquitectura de Computadoras                               |
| **Semestre**       | 4to Semestre — 2do Semestre 2026                           |
| **Docente**        | Ing. Paulo César Loayza Carrasco                           |
| **Estudiante**     | Leonardo Durán Cuenca                                      |

---

## Descripción de la Materia

Arquitectura de Computadoras es una materia con enfoque técnico orientado al hardware y al bajo nivel, abarcando desde los fundamentos de representación numérica hasta la organización interna de un sistema computacional y la interacción directa con el procesador. Los temas principales cubiertos a lo largo del semestre incluyen:

- Sistemas numéricos: binario, octal, hexadecimal y sus conversiones
- Aritmética binaria y representación en complemento a dos
- Formato de punto flotante estándar (IEEE 754)
- Arquitectura del procesador: registros, ALU, unidad de control y banderas de estado (Flags)
- Programación a bajo nivel e integración de lenguaje ensamblador (Inline Assembly / x86) en C/C++
- Jerarquía de memoria: caché, memoria RAM y direccionamiento
- Conjunto de instrucciones (ISA) y flujo de ejecución
- Pipeline, paralelismo y sistemas de Entrada/Salida (I/O)
- Comandos y herramientas de diagnóstico de hardware en sistemas tipo UNIX/Linux

Las actividades prácticas se desarrollan en un entorno **Ubuntu Linux** (mediante máquinas virtuales), utilizando herramientas como GCC/G++, Python, scripts de terminal y editores de código.

---

## Organización del Trabajo — Kanban

El seguimiento de todas las actividades asignadas se realiza a través de un **tablero Kanban en GitHub Projects**, vinculado directamente a este repositorio. Las tareas se gestionan como issues que transitan por las siguientes columnas:

| Columna         | Descripción                                                  |
|-----------------|--------------------------------------------------------------|
| **To Do**       | Tareas pendientes, recién asignadas o por iniciar            |
| **In Progress** | Tareas actualmente en desarrollo                             |
| **Done**        | Tareas completadas y entregadas                              |

Cada tarjeta en el tablero corresponde a una actividad asignada por el docente: desde configuración de entornos de trabajo y diagnósticos del sistema hasta prácticas de programación a bajo nivel.

> Los issues del tablero Kanban están redactados en **inglés**, siguiendo una estructura estandarizada que se describe en la siguiente sección.

---

## Formato de Tareas

Cada issue en el tablero Kanban sigue el formato estandarizado oficial en inglés definido en [`TaskTemplate.md`](TaskTemplate.md):

```markdown
## Title
<!-- Infinitive verb + short, clear phrase (3rd person) -->
<!-- Everything has to be in English -->
<!-- Kanban Project Fields: Assignee = Duranzzz | Start Date = Current Date (YYYY-MM-DD) | Target Date = Due Date (Defaults to Current Date if not specified) -->

## Description
**Assigned by:** Paulo César Loayza Carrasco  
**What to do:** [Action to be performed]  
**Objective:** [Purpose and academic benefit]  

## Responsible
Leonardo Durán Cuenca

## Due Date
YYYY-MM-DD

## Link
https://github.com/Duranzzz/Arquitectura-Computadoras

## Attachments
<p align="center">

</p>

## Program Output
<p align="center">

</p>
```

### Reglas para la Redacción y Metadatos en GitHub Projects

* **Asignación (`Assignee`):** Todo issue se asigna directamente al usuario de GitHub [`@Duranzzz`](https://github.com/Duranzzz).
* **Fecha de Inicio (`Start date`):** Se establece automáticamente con la fecha de creación de la tarea (`YYYY-MM-DD`, día actual).
* **Fecha Objetivo / Límite (`Target date` / `Due Date`):** Representa el plazo de entrega de la práctica (`YYYY-MM-DD`). En caso de no especificarse una fecha límite, se toma por defecto la fecha actual.
* **Título (`Title`):** Verbo en infinitivo en tercera persona (ej: *Install*, *Implement*, *Execute*, *Analyze*, *Build*), redactado en inglés de forma concisa y técnica.
* **Descripción (`Description`):**
  * `Assigned by:` Siempre el docente responsable (**Paulo César Loayza Carrasco**).
  * `What to do:` Detalle técnico paso a paso de lo que se implementa, compila o ejecuta.
  * `Objective:` Justificación conceptual, aprendizaje esperado y relevancia en la arquitectura del computador.
* **Responsable (`Responsible`):** Leonardo Durán Cuenca.
* **Enlace (`Link`):** URL al repositorio o ruta de la carpeta específica dentro de la rama `main`.
* **Evidencias (`Attachments` y `Program Output`):** Secciones con etiquetas HTML `<p align="center">` para albergar imágenes centradas del procedimiento y capturas de la salida por terminal/consola.

---

*Repositorio mantenido por Leonardo Durán Cuenca — 2026*

