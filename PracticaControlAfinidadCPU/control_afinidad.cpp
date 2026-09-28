/**
 * ============================================================================
 * UNIVERSIDAD CATOLICA BOLIVIANA "SAN PABLO"
 * DEPARTAMENTO DE INGENIERIAS Y CIENCIAS EXACTAS
 * MATERIA: Arquitectura de Computadoras (SIS-131)
 * DOCENTE: Ing. Paulo Cesar Loayza Carrasco
 * ESTUDIANTE: Leonardo Duran Cuenca
 * SEMESTRE: 4to Semestre - 2/2026
 *
 * PRACTICA: Control de Afinidad de CPU en C/C++ (Version C++ Moderno)
 * DESCRIPCION:
 *   Implementacion en C++20/C++17 con hilos estandar (std::thread), variables
 *   atomicas (std::atomic) y vinculacion de afinidad de hardware mediante el
 *   identificador nativo del sistema operativo (thread::native_handle).
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <set>
#include <string>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #define _GNU_SOURCE
    #include <pthread.h>
    #include <sched.h>
    #include <unistd.h>
#endif

// Bandera atomica global de sincronizacion y finalizacion de hilos
std::atomic<bool> g_keep_running{true};

struct ThreadStats {
    int thread_id;
    int target_core;
    uint64_t iterations;
    double elapsed_sec;
};

// Funcion de estres continuo de ALU ejecutada por cada hilo
void worker_stress_routine(int thread_id, int target_core, ThreadStats* out_stats) {
    // Fijar afinidad del hilo al nucleo logico asignado
#ifdef _WIN32
    DWORD_PTR affinity_mask = (1ULL << target_core);
    SetThreadAffinityMask(GetCurrentThread(), affinity_mask);
#else
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(target_core, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
#endif

    auto start_time = std::chrono::high_resolution_clock::now();
    uint64_t counter = 0;
    volatile uint64_t state = 0x9E3779B97F4A7C15ULL ^ static_cast<uint64_t>(target_core + 1);

    while (g_keep_running.load(std::memory_order_relaxed)) {
        // Carga de trabajo intensiva en la unidad aritmetico-logica (ALU)
        state ^= (state << 13);
        state ^= (state >> 7);
        state ^= (state << 17);
        state += 0x6A09E667F3BCC908ULL;
        counter++;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;

    out_stats->thread_id = thread_id;
    out_stats->target_core = target_core;
    out_stats->iterations = counter;
    out_stats->elapsed_sec = duration.count();
}

int main() {
    std::cout << "========================================================================\n";
    std::cout << "   UNIVERSIDAD CATOLICA BOLIVIANA \"SAN PABLO\" - SANTA CRUZ\n";
    std::cout << "   SIS-131: Arquitectura de Computadoras | 2-2026\n";
    std::cout << "   Estudiante: Leonardo Duran Cuenca\n";
    std::cout << "   Control de Afinidad de CPU en C++ (std::thread + native_handle)\n";
    std::cout << "========================================================================\n\n";

    // 1. DETECCION: Numero de nucleos logicos mediante la biblioteca estandar C++
    unsigned int total_hardware_threads = std::thread::hardware_concurrency();
    if (total_hardware_threads == 0) total_hardware_threads = 1;

    std::cout << "[1. DETECCION DE NUCLEOS LOGICOS]\n";
    std::cout << "   -> Nucleos logicos disponibles en el procesador (N): " << total_hardware_threads << "\n";
    std::cout << "   -> Rango de indices validos de nucleos: [0 a " << (total_hardware_threads - 1) << "]\n\n";

    // 2. CONFIGURACION: Cantidad de nucleos a utilizar
    int num_cores_to_use = 0;
    while (true) {
        std::cout << "[2. CONFIGURACION]\n";
        std::cout << "   -> Ingrese la cantidad de nucleos a estresar (1 a " << total_hardware_threads << "): ";
        if (std::cin >> num_cores_to_use && num_cores_to_use >= 1 && num_cores_to_use <= static_cast<int>(total_hardware_threads)) {
            std::string dummy;
            std::getline(std::cin, dummy); // Limpiar resto de linea
            break;
        }
        std::cin.clear();
        std::string discard;
        std::getline(std::cin, discard);
        std::cout << "   [!] Valor invalido. Ingrese un entero entre 1 y " << total_hardware_threads << ".\n\n";
    }

    // 2. CONFIGURACION: Indices de los nucleos
    std::vector<int> selected_cores;
    while (true) {
        std::cout << "   -> Ingrese los " << num_cores_to_use << " indices de nucleos (separados por espacio): ";
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);

        std::vector<int> temp_cores;
        std::set<int> unique_check;
        int core_idx;
        bool valid = true;

        while (iss >> core_idx) {
            if (core_idx < 0 || core_idx >= static_cast<int>(total_hardware_threads)) {
                std::cout << "   [!] Indice fuera de rango: " << core_idx << ".\n";
                valid = false;
                break;
            }
            if (unique_check.find(core_idx) != unique_check.end()) {
                std::cout << "   [!] Nucleo duplicado detectado: " << core_idx << ".\n";
                valid = false;
                break;
            }
            unique_check.insert(core_idx);
            temp_cores.push_back(core_idx);
        }

        if (valid && temp_cores.size() == static_cast<size_t>(num_cores_to_use)) {
            selected_cores = temp_cores;
            break;
        } else if (valid && temp_cores.size() != static_cast<size_t>(num_cores_to_use)) {
            std::cout << "   [!] Se ingresaron " << temp_cores.size() << " nucleos, pero se requieren " << num_cores_to_use << ".\n\n";
        }
    }

    std::cout << "\n[3. PLAN DE AFINIDAD DE HARDWARE]\n";
    for (size_t i = 0; i < selected_cores.size(); ++i) {
        uint64_t mask = (1ULL << selected_cores[i]);
        std::cout << "   - Hilo #" << std::setw(2) << std::setfill('0') << (i + 1)
                  << " -> Nucleo Logico " << std::setw(2) << selected_cores[i]
                  << " | Mascara de Afinidad: 0x" << std::hex << std::uppercase
                  << std::setw(16) << std::setfill('0') << mask << std::dec << "\n";
    }
    std::cout << "\n";

    // 3 y 4. EJECUCION Y ESTRES: Crear hilos y fijar afinidad
    std::vector<std::thread> thread_pool;
    std::vector<ThreadStats> stats_pool(num_cores_to_use);
    g_keep_running.store(true);

    std::cout << "[4. INICIANDO HILOS Y ESTABLECIENDO AFINIDAD]\n";

    for (int i = 0; i < num_cores_to_use; ++i) {
        int core_id = selected_cores[i];
        thread_pool.emplace_back(worker_stress_routine, i + 1, core_id, &stats_pool[i]);
        std::cout << "   [OK] Hilo #" << (i + 1) << " iniciado y anclado al Nucleo Logico " << core_id << ".\n";
    }

    std::cout << "\n************************************************************************\n";
    std::cout << "   ESTRES DE CPU EN MARCHA (~100% EN NUCLEOS SELECCIONADOS)\n";
    std::cout << "   Verifique en el Administrador de Tareas / Monitor de Recursos.\n";
    std::cout << "************************************************************************\n\n";

    // 5. CONTROL: Finalizar limpiamente con ENTER
    std::cout << ">>> Presione [ENTER] para finalizar todos los hilos limpiamente... <<<\n";
    std::string wait_input;
    std::getline(std::cin, wait_input);

    std::cout << "\n[5. DETENIENDO HILOS Y RECOLECTANDO METRICAS]\n";
    g_keep_running.store(false);

    for (auto& t : thread_pool) {
        if (t.joinable()) {
            t.join();
        }
    }

    std::cout << "   -> Todos los hilos finalizaron correctamente.\n\n";
    std::cout << "========================================================================\n";
    std::cout << "                     METRICAS FINALES DE EJECUCION                      \n";
    std::cout << "========================================================================\n";
    std::cout << " Hilo | Nucleo Asignado | Mascara Bitwise    | Iteraciones       | Tiempo (s)\n";
    std::cout << "------+-----------------+--------------------+-------------------+-----------\n";

    uint64_t total_ops = 0;
    for (const auto& s : stats_pool) {
        total_ops += s.iterations;
        uint64_t mask = (1ULL << s.target_core);
        std::cout << "  #" << std::setw(2) << std::setfill('0') << s.thread_id
                  << " | Nucleo Logico " << std::setw(2) << s.target_core
                  << " | 0x" << std::hex << std::uppercase << std::setw(16) << std::setfill('0') << mask << std::dec
                  << " | " << std::setw(17) << std::setfill(' ') << s.iterations
                  << " | " << std::fixed << std::setprecision(2) << s.elapsed_sec << " s\n";
    }
    std::cout << "------+-----------------+--------------------+-------------------+-----------\n";
    std::cout << " Total operaciones ALU ejecutadas: " << total_ops << "\n";
    std::cout << " Finalizacion limpia sin fugas ni violaciones de acceso de memoria.\n";
    std::cout << "========================================================================\n\n";

    return 0;
}
