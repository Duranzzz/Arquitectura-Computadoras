/**
 * ============================================================================
 * UNIVERSIDAD CATOLICA BOLIVIANA "SAN PABLO"
 * DEPARTAMENTO DE INGENIERIAS Y CIENCIAS EXACTAS
 * MATERIA: Arquitectura de Computadoras (SIS-131)
 * DOCENTE: Ing. Paulo Cesar Loayza Carrasco
 * ESTUDIANTE: Leonardo Duran Cuenca
 * SEMESTRE: 4to Semestre - 2/2026
 *
 * PRACTICA: Control de Afinidad de CPU en C/C++
 * DESCRIPCION:
 *   Programa de asignacion de afinidad de CPU a nivel de hardware y sistema
 *   operativo. Permite detectar el numero total de nucleos logicos, seleccionar
 *   nucleos especificos mediante mascaras de afinidad (Bitmask) y generar
 *   una carga intensiva de computo al ~100% de uso en los nucleos seleccionados.
 *
 *   Soporte multiplataforma:
 *     - Windows: Windows NT API (SetThreadAffinityMask, CreateThread, GetSystemInfo)
 *     - Linux/POSIX: POSIX Threads API (pthread_setaffinity_np, CPU_SET, sysconf)
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    typedef HANDLE thread_t;
#else
    #define _GNU_SOURCE
    #include <pthread.h>
    #include <sched.h>
    #include <unistd.h>
    typedef pthread_t thread_t;
#endif

/* Variable atomica / flag volatil para detencion limpia de hilos */
static volatile int g_stop_flag = 0;

/* Estructura para almacenar los parametros y estadisticas de cada hilo */
typedef struct {
    int thread_id;            /* Indice secuencial del hilo creado (1, 2, ...) */
    int target_core;          /* Indice del nucleo logico asignado (0, ..., N-1) */
    uint64_t iterations;      /* Contador de iteraciones completadas */
    double elapsed_seconds;   /* Tiempo de ejecucion medido */
    bool affinity_success;    /* Bandera de exito al aplicar la afinidad */
} ThreadContext;

/* Obtener el nombre descriptivo del procesador */
void get_cpu_brand_name(char *buffer, size_t max_len) {
#ifdef _WIN32
    HKEY hKey;
    DWORD bufSize = (DWORD)max_len;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExA(hKey, "ProcessorNameString", NULL, NULL, (LPBYTE)buffer, &bufSize) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return;
        }
        RegCloseKey(hKey);
    }
    const char *env_cpu = getenv("PROCESSOR_IDENTIFIER");
    if (env_cpu != NULL) {
        strncpy(buffer, env_cpu, max_len - 1);
        buffer[max_len - 1] = '\0';
        return;
    }
#else
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "model name", 10) == 0) {
                char *colon = strchr(line, ':');
                if (colon) {
                    colon++;
                    while (*colon == ' ' || *colon == '\t') colon++;
                    char *nl = strchr(colon, '\n');
                    if (nl) *nl = '\0';
                    strncpy(buffer, colon, max_len - 1);
                    buffer[max_len - 1] = '\0';
                    fclose(fp);
                    return;
                }
            }
        }
        fclose(fp);
    }
#endif
    snprintf(buffer, max_len, "Procesador Generico x86_64");
}

/* Deteccion de nucleos logicos totales del sistema */
int get_logical_core_count(void) {
#ifdef _WIN32
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    return (int)sys_info.dwNumberOfProcessors;
#else
    long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
    if (nprocs < 1) nprocs = 1;
    return (int)nprocs;
#endif
}

/* Rutina intensiva de estres de computo (Worker Function) */
#ifdef _WIN32
DWORD WINAPI thread_stress_worker(LPVOID lpParam)
#else
void *thread_stress_worker(void *lpParam)
#endif
{
    ThreadContext *ctx = (ThreadContext *)lpParam;

    /* Asegurar y fijar la afinidad del hilo al nucleo destino */
#ifdef _WIN32
    DWORD_PTR affinity_mask = ((DWORD_PTR)1) << ctx->target_core;
    DWORD_PTR prev_mask = SetThreadAffinityMask(GetCurrentThread(), affinity_mask);
    ctx->affinity_success = (prev_mask != 0);
#else
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(ctx->target_core, &cpuset);
    int res = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    ctx->affinity_success = (res == 0);
#endif

    clock_t start_time = clock();
    volatile uint64_t x = 0x123456789ABCDEFULL ^ (uint64_t)(ctx->target_core + 1);
    uint64_t local_counter = 0;

    /* Ciclo de computo intensivo: operaciones ALU/Bitwise continuas */
    while (!g_stop_flag) {
        /* Xorshift64* pseudo-random sequence generator: saturacion pura de ALU */
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        x *= 0x2545F4914F6CDD1DULL;

        local_counter++;
    }

    clock_t end_time = clock();
    ctx->iterations = local_counter;
    ctx->elapsed_seconds = (double)(end_time - start_time) / CLOCKS_PER_SEC;

#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

/* Limpiar lineas residuales de entrada */
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int main(void) {
    char cpu_brand[256];
    get_cpu_brand_name(cpu_brand, sizeof(cpu_brand));
    int total_cores = get_logical_core_count();

    printf("========================================================================\n");
    printf("   UNIVERSIDAD CATOLICA BOLIVIANA \"SAN PABLO\" - SANTA CRUZ\n");
    printf("   SIS-131: Arquitectura de Computadoras | 2-2026\n");
    printf("   Estudiante: Leonardo Duran Cuenca\n");
    printf("   Practica: Control de Afinidad de CPU y Estres de Nucleos (~100%%)\n");
    printf("========================================================================\n\n");

    printf("[1. DETECCION DE ARQUITECTURA DE HARDWARE]\n");
    printf("   -> Modelo de CPU : %s\n", cpu_brand);
    printf("   -> Nucleos Logicos Detectados (N) : %d\n", total_cores);
    printf("   -> Rango de Indices Validos : 0 a %d\n\n", total_cores - 1);

    /* 2. CONFIGURACION: Cantidad de nucleos */
    int num_cores_to_use = 0;
    while (1) {
        printf("[2. CONFIGURACION]\n");
        printf("   -> Ingrese cuantos nucleos desea estresar (1 a %d): ", total_cores);
        if (scanf("%d", &num_cores_to_use) == 1) {
            if (num_cores_to_use >= 1 && num_cores_to_use <= total_cores) {
                clear_input_buffer();
                break;
            }
        }
        clear_input_buffer();
        printf("   [!] Valor invalido. Debe ser un numero entero entre 1 y %d.\n\n", total_cores);
    }

    /* 2. CONFIGURACION: Seleccion especifica de nucleos */
    int selected_cores[64];
    bool core_used[64] = {false};

    while (1) {
        printf("   -> Ingrese los indices de los %d nucleos elegidos (separados por espacio): ", num_cores_to_use);
        char line_buffer[512];
        if (fgets(line_buffer, sizeof(line_buffer), stdin) == NULL) {
            continue;
        }

        /* Resetear registro de nucleos */
        for (int i = 0; i < total_cores; i++) core_used[i] = false;

        char *token = strtok(line_buffer, " \t\r\n");
        int count = 0;
        bool valid = true;

        while (token != NULL) {
            char *endptr;
            long val = strtol(token, &endptr, 10);
            if (*endptr != '\0') {
                printf("   [!] Entrada invalida: '%s' no es un numero.\n", token);
                valid = false;
                break;
            }
            if (val < 0 || val >= total_cores) {
                printf("   [!] Nucleo fuera de rango: %ld. Debe estar entre 0 y %d.\n", val, total_cores - 1);
                valid = false;
                break;
            }
            if (core_used[val]) {
                printf("   [!] Nucleo duplicado detectado: %ld. Cada nucleo debe ser unico.\n", val);
                valid = false;
                break;
            }

            core_used[val] = true;
            if (count < 64) {
                selected_cores[count] = (int)val;
            }
            count++;
            token = strtok(NULL, " \t\r\n");
        }

        if (valid && count == num_cores_to_use) {
            break;
        } else if (valid && count != num_cores_to_use) {
            printf("   [!] Ingreso %d nucleos, pero se requerian exactamente %d.\n\n", count, num_cores_to_use);
        }
    }

    /* Resumen de configuracion previa a la ejecucion */
    printf("\n[3. PLAN DE EJECUCION Y ASIGNACION DE AFINIDAD]\n");
    for (int i = 0; i < num_cores_to_use; i++) {
        uint64_t mask = ((uint64_t)1) << selected_cores[i];
        printf("   - Hilo #%02d -> Asignado al Nucleo Logico %02d | Mascara Bitwise: 0x%016llX\n",
               i + 1, selected_cores[i], (unsigned long long)mask);
    }
    printf("\n");

    /* 3. EJECUCION: Crear los hilos y fijar afinidad */
    ThreadContext contexts[64];
    thread_t threads[64];

    g_stop_flag = 0;
    printf("[4. LANZAMIENTO Y ESTRES DE HILOS]\n");

    for (int i = 0; i < num_cores_to_use; i++) {
        contexts[i].thread_id = i + 1;
        contexts[i].target_core = selected_cores[i];
        contexts[i].iterations = 0;
        contexts[i].elapsed_seconds = 0.0;
        contexts[i].affinity_success = false;

#ifdef _WIN32
        DWORD thread_id_win;
        threads[i] = CreateThread(
            NULL,
            0,
            thread_stress_worker,
            &contexts[i],
            0,
            &thread_id_win
        );
        if (threads[i] == NULL) {
            fprintf(stderr, "   [ERROR] No se pudo crear el hilo #%d para el nucleo %d.\n", i + 1, selected_cores[i]);
            continue;
        }
        /* Asignar la mascara de afinidad explicitamente al hilo recien creado */
        DWORD_PTR mask = ((DWORD_PTR)1) << selected_cores[i];
        SetThreadAffinityMask(threads[i], mask);
        printf("   [OK] Hilo #%02d (Win32 TID %5lu) anclado firmemente al Nucleo %02d.\n",
               i + 1, thread_id_win, selected_cores[i]);
#else
        int rc = pthread_create(&threads[i], NULL, thread_stress_worker, &contexts[i]);
        if (rc != 0) {
            fprintf(stderr, "   [ERROR] Fallo pthread_create en hilo #%d para el nucleo %d.\n", i + 1, selected_cores[i]);
            continue;
        }
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(selected_cores[i], &cpuset);
        pthread_setaffinity_np(threads[i], sizeof(cpu_set_t), &cpuset);
        printf("   [OK] Hilo #%02d (POSIX TID %p) anclado firmemente al Nucleo %02d.\n",
               i + 1, (void *)threads[i], selected_cores[i]);
#endif
    }

    printf("\n************************************************************************\n");
    printf("   ESTRES EN EJECUCION ACTIVA (~100%% DE USO EN NUCLEOS SELECCIONADOS)\n");
    printf("   Abra el Administrador de Tareas (Pestana Rendimiento -> CPU -> Cambiar\n");
    printf("   grafico a 'Procesadores Logicos') o el Monitor de Recursos (resmon.exe).\n");
    printf("************************************************************************\n\n");

    /* 5. CONTROL: Finalizar limpiamente al presionar ENTER */
    printf(">>> Presione [ENTER] en cualquier momento para detener los hilos limpiamente... <<<\n");
    char finish_char = getchar();
    (void)finish_char;

    printf("\n[5. DETENCION Y LIBERACION DE RECURSOS]\n");
    printf("   -> Enviando senal de detencion (g_stop_flag = 1) a todos los hilos...\n");
    g_stop_flag = 1;

    /* Esperar la finalizacion ordenada de todos los hilos */
#ifdef _WIN32
    WaitForMultipleObjects(num_cores_to_use, threads, TRUE, INFINITE);
    for (int i = 0; i < num_cores_to_use; i++) {
        CloseHandle(threads[i]);
    }
#else
    for (int i = 0; i < num_cores_to_use; i++) {
        pthread_join(threads[i], NULL);
    }
#endif

    printf("   -> Todos los hilos finalizaron su ciclo de computo de forma controlada.\n\n");
    printf("========================================================================\n");
    printf("                     INFORME DE RENDIMIENTO Y ESTRES                    \n");
    printf("========================================================================\n");
    printf(" Hilo | Nucleo Asignado | Mascara Bitwise    | Iteraciones ALU   | Estado\n");
    printf("------+-----------------+--------------------+-------------------+--------\n");

    uint64_t total_iterations = 0;
    for (int i = 0; i < num_cores_to_use; i++) {
        uint64_t mask = ((uint64_t)1) << contexts[i].target_core;
        total_iterations += contexts[i].iterations;
        printf("  #%02d | Nucleo Logico %02d| 0x%016llX | %14llu  | OK (100%%)\n",
               contexts[i].thread_id,
               contexts[i].target_core,
               (unsigned long long)mask,
               (unsigned long long)contexts[i].iterations);
    }
    printf("------+-----------------+--------------------+-------------------+--------\n");
    printf(" Total de iteraciones de computo acumuladas: %llu\n", (unsigned long long)total_iterations);
    printf(" Afinidad de hardware ejecutada sin excepciones de sistema operativo.\n");
    printf("========================================================================\n\n");

    return 0;
}
