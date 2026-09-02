#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>

constexpr int TAMANO_PRINCIPAL = 1000;
constexpr int TAMANO_TRANSFERENCIA = 100;
constexpr int INDICE_INICIO = 500;
constexpr int INDICE_FIN = 599;
const char* ARCHIVO_DATOS = "datos_secundaria.bin";

// Generar archivo en memoria secundaria si no existe
void generarArchivoSecundaria(const char* ruta) {
    std::ofstream archivo(ruta, std::ios::binary);
    if (!archivo) {
        std::cerr << "Error: No se pudo crear el archivo en memoria secundaria.\n";
        return;
    }

    int buffer[TAMANO_PRINCIPAL];
    for (int i = 0; i < TAMANO_PRINCIPAL; ++i) {
        buffer[i] = (i + 1) * 10; // Ejemplo: 10, 20, 30, ..., 10000
    }

    archivo.write(reinterpret_cast<const char*>(buffer), sizeof(buffer));
    archivo.close();
}

int main() {
    int arreglo_principal[TAMANO_PRINCIPAL];
    int arreglo_secundario[TAMANO_TRANSFERENCIA];

    std::cout << "===================================================================\n";
    std::cout << "  ARQUITECTURA DE COMPUTADORAS - GESTION DE JERARQUIA DE MEMORIA   \n";
    std::cout << "===================================================================\n\n";

    // 1. Crear y leer arreglo de 1000 enteros desde memoria secundaria a memoria RAM
    generarArchivoSecundaria(ARCHIVO_DATOS);

    std::ifstream archivo(ARCHIVO_DATOS, std::ios::binary);
    if (!archivo) {
        std::cerr << "Error al abrir el archivo de memoria secundaria.\n";
        return 1;
    }

    archivo.read(reinterpret_cast<char*>(arreglo_principal), sizeof(arreglo_principal));
    archivo.close();

    std::cout << "[Paso 1] Arreglo de " << TAMANO_PRINCIPAL << " enteros cargado desde Memoria Secundaria a RAM.\n\n";

    // 2. Mostrar primeros 5 y ultimos 5 elementos antes del traslado
    std::cout << "--- Muestra del Arreglo Inicial en RAM (Antes del traslado) ---\n";
    std::cout << "Primeros 5 elementos:\n";
    for (int i = 0; i < 5; ++i) {
        std::cout << "  arreglo_principal[" << std::setw(3) << i << "] = "
                  << std::setw(6) << arreglo_principal[i]
                  << " (Direccion: " << &arreglo_principal[i] << ")\n";
    }

    std::cout << "\nUltimos 5 elementos:\n";
    for (int i = TAMANO_PRINCIPAL - 5; i < TAMANO_PRINCIPAL; ++i) {
        std::cout << "  arreglo_principal[" << std::setw(3) << i << "] = "
                  << std::setw(6) << arreglo_principal[i]
                  << " (Direccion: " << &arreglo_principal[i] << ")\n";
    }
    std::cout << "\n";

    // 3. Transferir 100 elementos (indices 500 al 599) al segundo arreglo mediante std::memcpy
    std::cout << "[Paso 2] Transfiriendo bloque de memoria (indices " << INDICE_INICIO << " a " << INDICE_FIN << ") con std::memcpy()...\n";
    std::memcpy(arreglo_secundario, &arreglo_principal[INDICE_INICIO], TAMANO_TRANSFERENCIA * sizeof(int));

    // 4. Limpiar los valores transferidos en el arreglo original estableciendolos en cero
    std::cout << "[Paso 3] Limpiando bloque en arreglo original con std::memset()...\n\n";
    std::memset(&arreglo_principal[INDICE_INICIO], 0, TAMANO_TRANSFERENCIA * sizeof(int));

    // 5. Mostrar los 100 elementos transferidos al segundo arreglo
    std::cout << "--- Contenido del Segundo Arreglo (" << TAMANO_TRANSFERENCIA << " elementos transferidos) ---\n";
    for (int i = 0; i < TAMANO_TRANSFERENCIA; ++i) {
        std::cout << "[" << std::setw(3) << i << "]: " << std::setw(6) << arreglo_secundario[i] << "  ";
        if ((i + 1) % 5 == 0) {
            std::cout << "\n";
        }
    }
    std::cout << "\n";

    // 6. Validacion de que los indices 500 al 599 del primer arreglo contienen ceros
    std::cout << "--- Validacion de Limpieza en Arreglo Original (indices " << INDICE_INICIO << " a " << INDICE_FIN << ") ---\n";
    bool todo_cero = true;
    for (int i = INDICE_INICIO; i <= INDICE_FIN; ++i) {
        if (arreglo_principal[i] != 0) {
            todo_cero = false;
            std::cout << "  [ERROR] arreglo_principal[" << i << "] = " << arreglo_principal[i] << " (se esperaba 0)\n";
            break;
        }
    }

    if (todo_cero) {
        std::cout << "  [CORRECTO] Todos los 100 elementos (indices " << INDICE_INICIO << " a " << INDICE_FIN << ") en el arreglo principal contienen '0'.\n";
        std::cout << "  [CORRECTO] Transferencia y limpieza de memoria verificadas exitosamente.\n";
    } else {
        std::cout << "  [FALLO] La validacion encontro inconsistencias en la memoria.\n";
    }

    std::cout << "\n===================================================================\n";
    return 0;
}
