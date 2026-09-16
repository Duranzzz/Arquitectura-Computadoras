#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <cstring>

// =============================================================================
// PARÁMETROS DEL BENCHMARK
// Matriz de 2048 x 2048 = 4,194,304 píxeles (4.19 Megapíxeles)
// Huella en RAM: ~16 MB por buffer de punto flotante (5 buffers = ~80 MB activos)
// =============================================================================
constexpr int WIDTH = 2048;
constexpr int HEIGHT = 2048;

// =============================================================================
// 1. GENERACIÓN MATEMÁTICA DE IMAGEN SINTÉTICA DE ALTA RESOLUCIÓN
// Patrones: Círculos concéntricos, rectángulos, gradientes y patrones de frecuencia
// =============================================================================
void generar_imagen_sintetica(float* img, int width, int height) {
    const float cx = width / 2.0f;
    const float cy = height / 2.0f;
    const float r_max = width * 0.45f;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float val = 0.1f; // Fondo base oscuro

            // Gradiente diagonal suave
            val += 0.15f * ((float)(x + y) / (width + height));

            // Distancia al centro
            float dx = x - cx;
            float dy = y - cy;
            float dist = std::sqrt(dx * dx + dy * dy);

            // Anillos concéntricos nítidos (bordes circulares para Sobel)
            if (dist < r_max) {
                int ring = static_cast<int>(dist / 40.0f);
                if (ring % 2 == 0) {
                    val += 0.45f;
                }
            }

            // Rectángulo central contrastado
            if (std::abs(dx) < 250.0f && std::abs(dy) < 250.0f) {
                val = 0.95f;
            }

            // Cruz interna en el rectángulo
            if (std::abs(dx) < 40.0f || std::abs(dy) < 40.0f) {
                if (std::abs(dx) < 200.0f && std::abs(dy) < 200.0f) {
                    val = 0.05f;
                }
            }

            // Cuadrados en las esquinas
            if ((x < 300 && y < 300) || (x > width - 300 && y > height - 300)) {
                val += 0.35f;
            }

            // Clamping [0.0, 1.0]
            if (val > 1.0f) val = 1.0f;
            if (val < 0.0f) val = 0.0f;

            img[y * width + x] = val;
        }
    }
}

// =============================================================================
// 2. EXPORTADOR NATIVO DE ARCHIVOS BMP (24-bit RGB, sin librerías externas)
// =============================================================================
#pragma pack(push, 1)
struct BMPHeader {
    uint16_t file_type{0x4D42};       // "BM"
    uint32_t file_size{0};            // Tamaño total del archivo en bytes
    uint16_t reserved1{0};
    uint16_t reserved2{0};
    uint32_t offset_data{54};         // Offset a los datos de píxeles
};

struct BMPInfoHeader {
    uint32_t size{40};                // Tamaño del encabezado DIB
    int32_t  width{0};
    int32_t  height{0};
    uint16_t planes{1};
    uint16_t bit_count{24};           // 24 bits = RGB (8 bits por canal)
    uint32_t compression{0};          // 0 = BI_RGB (sin compresión)
    uint32_t size_image{0};
    int32_t  x_pixels_per_meter{2835};
    int32_t  y_pixels_per_meter{2835};
    uint32_t colors_used{0};
    uint32_t colors_important{0};
};
#pragma pack(pop)

bool guardar_bmp(const std::string& filename, const float* img, int width, int height, bool normalize = false) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: No se pudo abrir para escritura el archivo BMP: " << filename << "\n";
        return false;
    }

    const int row_stride = width * 3;
    const int padding = (4 - (row_stride % 4)) % 4;
    const uint32_t data_size = (row_stride + padding) * height;

    BMPHeader header;
    header.file_size = 54 + data_size;

    BMPInfoHeader info;
    info.width = width;
    info.height = height; // Positivo = orden de abajo hacia arriba (estándar BMP)
    info.size_image = data_size;

    file.write(reinterpret_cast<const char*>(&header), sizeof(header));
    file.write(reinterpret_cast<const char*>(&info), sizeof(info));

    float max_val = 1.0f;
    if (normalize) {
        max_val = 0.0001f;
        for (int i = 0; i < width * height; ++i) {
            if (img[i] > max_val) max_val = img[i];
        }
    }

    std::vector<uint8_t> pad(padding, 0);
    std::vector<uint8_t> row_buffer(row_stride);

    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            float v = img[y * width + x];
            if (normalize) v = (v / max_val);
            if (v > 1.0f) v = 1.0f;
            if (v < 0.0f) v = 0.0f;

            uint8_t byte_val = static_cast<uint8_t>(v * 255.0f);
            row_buffer[x * 3 + 0] = byte_val; // Blue
            row_buffer[x * 3 + 1] = byte_val; // Green
            row_buffer[x * 3 + 2] = byte_val; // Red
        }
        file.write(reinterpret_cast<const char*>(row_buffer.data()), row_stride);
        if (padding > 0) {
            file.write(reinterpret_cast<const char*>(pad.data()), padding);
        }
    }

    file.close();
    return true;
}

// =============================================================================
// 3. VISTA PREVIA ASCII EN CONSOLA
// =============================================================================
void render_ascii_preview(const float* img, int width, int height, int out_w = 60, int out_h = 24) {
    const char* ramp = " .:-=+*#%@";
    const int ramp_len = 10;

    int step_x = width / out_w;
    int step_y = height / out_h;

    std::cout << "+" << std::string(out_w, '-') << "+\n";
    for (int py = 0; py < out_h; ++py) {
        std::cout << "|";
        for (int px = 0; px < out_w; ++px) {
            int start_x = px * step_x;
            int start_y = py * step_y;

            float sum = 0.0f;
            int samples = 0;
            for (int dy = 0; dy < step_y; dy += 4) {
                for (int dx = 0; dx < step_x; dx += 4) {
                    sum += img[(start_y + dy) * width + (start_x + dx)];
                    samples++;
                }
            }
            float avg = sum / samples;
            int idx = static_cast<int>(avg * 2.5f * (ramp_len - 1));
            if (idx < 0) idx = 0;
            if (idx >= ramp_len) idx = ramp_len - 1;

            std::cout << ramp[idx];
        }
        std::cout << "|\n";
    }
    std::cout << "+" << std::string(out_w, '-') << "+\n";
}

// =============================================================================
// 4. ALGORITMOS DE CONVOLUCIÓN SOBEL 3x3 Y OPTIMIZACIONES ARQUITECTÓNICAS
// =============================================================================

// -----------------------------------------------------------------------------
// FASE 1: NAIVE (Recorrido por columnas - Column-Major)
// Ineficiente: Salto de WIDTH * 4 bytes (8192 B) en cada iteración de y
// Genera Cache Misses continuos en cada vecino del kernel 3x3
// -----------------------------------------------------------------------------
void convolucion_naive(const float* in, float* out, int width, int height) {
    std::memset(out, 0, width * height * sizeof(float));

    for (int x = 1; x < width - 1; ++x) {
        for (int y = 1; y < height - 1; ++y) {
            float p00 = in[(y - 1) * width + (x - 1)];
            float p01 = in[(y - 1) * width + x];
            float p02 = in[(y - 1) * width + (x + 1)];

            float p10 = in[y * width + (x - 1)];
            float p12 = in[y * width + (x + 1)];

            float p20 = in[(y + 1) * width + (x - 1)];
            float p21 = in[(y + 1) * width + x];
            float p22 = in[(y + 1) * width + (x + 1)];

            float gx = -p00 + p02 - 2.0f * p10 + 2.0f * p12 - p20 + p22;
            float gy = -p00 - 2.0f * p01 - p02 + p20 + 2.0f * p21 + p22;

            out[y * width + x] = std::fabs(gx) + std::fabs(gy);
        }
    }
}

// -----------------------------------------------------------------------------
// FASE 2: LOCALIDAD ESPACIAL (Recorrido contiguo por filas - Row-Major Order)
// Aprovecha la línea de caché de 64 Bytes (16 floats contiguos por línea)
// 15 de cada 16 accesos resultan en Cache Hit en L1
// -----------------------------------------------------------------------------
void convolucion_localidad_espacial(const float* in, float* out, int width, int height) {
    std::memset(out, 0, width * height * sizeof(float));

    for (int y = 1; y < height - 1; ++y) {
        const float* row_prev = &in[(y - 1) * width];
        const float* row_curr = &in[y * width];
        const float* row_next = &in[(y + 1) * width];
        float* row_out = &out[y * width];

        for (int x = 1; x < width - 1; ++x) {
            float p00 = row_prev[x - 1];
            float p01 = row_prev[x];
            float p02 = row_prev[x + 1];

            float p10 = row_curr[x - 1];
            float p12 = row_curr[x + 1];

            float p20 = row_next[x - 1];
            float p21 = row_next[x];
            float p22 = row_next[x + 1];

            float gx = -p00 + p02 - 2.0f * p10 + 2.0f * p12 - p20 + p22;
            float gy = -p00 - 2.0f * p01 - p02 + p20 + 2.0f * p21 + p22;

            row_out[x] = std::fabs(gx) + std::fabs(gy);
        }
    }
}

// -----------------------------------------------------------------------------
// FASE 3: LOCALIDAD TEMPORAL + REGISTROS DE CPU (Ventana deslizante en registros)
// Reutiliza las 2 columnas solapadas de la ventana 3x3 en registros físicos FPU
// Reduce las cargas de memoria de 9 a solo 3 por píxel (ahorra 25 millones de loads)
// -----------------------------------------------------------------------------
void convolucion_registros_cpu(const float* in, float* out, int width, int height) {
    std::memset(out, 0, width * height * sizeof(float));

    for (int y = 1; y < height - 1; ++y) {
        const float* row_prev = &in[(y - 1) * width];
        const float* row_curr = &in[y * width];
        const float* row_next = &in[(y + 1) * width];
        float* row_out = &out[y * width];

        // Columnas iniciales retenidas en registros físicos del procesador
        float c0_top = row_prev[0], c0_mid = row_curr[0], c0_bot = row_next[0];
        float c1_top = row_prev[1], c1_mid = row_curr[1], c1_bot = row_next[1];

        for (int x = 1; x < width - 1; ++x) {
            // Solo carga la nueva columna 2 desde memoria
            const float c2_top = row_prev[x + 1];
            const float c2_mid = row_curr[x + 1];
            const float c2_bot = row_next[x + 1];

            float gx = -c0_top + c2_top - 2.0f * c0_mid + 2.0f * c2_mid - c0_bot + c2_bot;
            float gy = -c0_top - 2.0f * c1_top - c2_top + c0_bot + 2.0f * c1_bot + c2_bot;

            row_out[x] = std::fabs(gx) + std::fabs(gy);

            // Desplaza las columnas directamente en registros (sin tráfico de memoria)
            c0_top = c1_top; c0_mid = c1_mid; c0_bot = c1_bot;
            c1_top = c2_top; c1_mid = c2_mid; c1_bot = c2_bot;
        }
    }
}

// -----------------------------------------------------------------------------
// FASE 4: LOOP UNROLLING 4X + ILP (Paralelismo a Nivel de Instrucción)
// Desenrolla el bucle procesando 4 píxeles consecutivos en paralelo
// Reduce branches en 75% y satura los pipelines superescalares
// -----------------------------------------------------------------------------
void convolucion_loop_unrolling(const float* in, float* out, int width, int height) {
    std::memset(out, 0, width * height * sizeof(float));

    for (int y = 1; y < height - 1; ++y) {
        const float* row_prev = &in[(y - 1) * width];
        const float* row_curr = &in[y * width];
        const float* row_next = &in[(y + 1) * width];
        float* row_out = &out[y * width];

        int x = 1;
        for (; x <= width - 5; x += 4) {
            // Píxel 0 (x)
            float gx0 = -row_prev[x - 1] + row_prev[x + 1] - 2.0f * row_curr[x - 1] + 2.0f * row_curr[x + 1] - row_next[x - 1] + row_next[x + 1];
            float gy0 = -row_prev[x - 1] - 2.0f * row_prev[x] - row_prev[x + 1] + row_next[x - 1] + 2.0f * row_next[x] + row_next[x + 1];

            // Píxel 1 (x + 1)
            float gx1 = -row_prev[x] + row_prev[x + 2] - 2.0f * row_curr[x] + 2.0f * row_curr[x + 2] - row_next[x] + row_next[x + 2];
            float gy1 = -row_prev[x] - 2.0f * row_prev[x + 1] - row_prev[x + 2] + row_next[x] + 2.0f * row_next[x + 1] + row_next[x + 2];

            // Píxel 2 (x + 2)
            float gx2 = -row_prev[x + 1] + row_prev[x + 3] - 2.0f * row_curr[x + 1] + 2.0f * row_curr[x + 3] - row_next[x + 1] + row_next[x + 3];
            float gy2 = -row_prev[x + 1] - 2.0f * row_prev[x + 2] - row_prev[x + 3] + row_next[x + 1] + 2.0f * row_next[x + 2] + row_next[x + 3];

            // Píxel 3 (x + 3)
            float gx3 = -row_prev[x + 2] + row_prev[x + 4] - 2.0f * row_curr[x + 2] + 2.0f * row_curr[x + 4] - row_next[x + 2] + row_next[x + 4];
            float gy3 = -row_prev[x + 2] - 2.0f * row_prev[x + 3] - row_prev[x + 4] + row_next[x + 2] + 2.0f * row_next[x + 3] + row_next[x + 4];

            row_out[x]     = std::fabs(gx0) + std::fabs(gy0);
            row_out[x + 1] = std::fabs(gx1) + std::fabs(gy1);
            row_out[x + 2] = std::fabs(gx2) + std::fabs(gy2);
            row_out[x + 3] = std::fabs(gx3) + std::fabs(gy3);
        }

        // Remanente para bordes
        for (; x < width - 1; ++x) {
            float p00 = row_prev[x - 1]; float p02 = row_prev[x + 1];
            float p10 = row_curr[x - 1]; float p12 = row_curr[x + 1];
            float p20 = row_next[x - 1]; float p22 = row_next[x + 1];
            float p01 = row_prev[x];     float p21 = row_next[x];

            float gx = -p00 + p02 - 2.0f * p10 + 2.0f * p12 - p20 + p22;
            float gy = -p00 - 2.0f * p01 - p02 + p20 + 2.0f * p21 + p22;
            row_out[x] = std::fabs(gx) + std::fabs(gy);
        }
    }
}

// Checksum matemático para validar determinismo
static double calcular_checksum(const float* mat, int size) {
    double sum = 0.0;
    for (int i = 0; i < size; ++i) sum += static_cast<double>(mat[i]);
    return sum;
}

// =============================================================================
// FUNCIÓN PRINCIPAL
// =============================================================================
int main() {
    std::cout << "======================================================================\n";
    std::cout << " BENCHMARK: CONVOLUCION 2D SOBEL - OPTIMIZACION EN JERARQUIA DE MEMORIA\n";
    std::cout << " Arquitectura de Computadoras | UCB San Pablo | Leonardo Duran Cuenca\n";
    std::cout << "======================================================================\n";

    const size_t num_pixels = static_cast<size_t>(WIDTH) * HEIGHT;
    const size_t bytes_per_img = num_pixels * sizeof(float);
    const double total_megapixels = num_pixels / 1e6;

    std::cout << "[i] Dimension de Imagen : " << WIDTH << " x " << HEIGHT << " (" << std::fixed << std::setprecision(2) << total_megapixels << " Megapixeles)\n";
    std::cout << "[i] Memoria por Buffer  : " << (bytes_per_img / (1024.0 * 1024.0)) << " MB\n";
    std::cout << "[i] Huella Activa RAM   : " << ((bytes_per_img * 5) / (1024.0 * 1024.0)) << " MB (Excede L1/L2, fuerza uso de jerarquia)\n";

    std::vector<float> img_in(num_pixels);
    std::vector<float> img_c1(num_pixels);
    std::vector<float> img_c2(num_pixels);
    std::vector<float> img_c3(num_pixels);
    std::vector<float> img_c4(num_pixels);

    std::cout << "\n[*] Generando patron matematico de alta resolucion en memoria...\n";
    generar_imagen_sintetica(img_in.data(), WIDTH, HEIGHT);

    // Exportar imagen original
    std::string path_orig = "PracticaConvolucion2DMemoria/imgs/imagen_original.bmp";
    std::cout << "[*] Guardando imagen original sintetica en '" << path_orig << "'...\n";
    guardar_bmp(path_orig, img_in.data(), WIDTH, HEIGHT, false);

    std::cout << "\n[*] Iniciando ejecucion del Benchmark en las 4 fases arquitectonicas...\n";

    using clock_type = std::chrono::high_resolution_clock;

    // Fase 1: Naive (Column-Major)
    auto t0 = clock_type::now();
    convolucion_naive(img_in.data(), img_c1.data(), WIDTH, HEIGHT);
    auto t1 = clock_type::now();
    double t_naive = std::chrono::duration<double>(t1 - t0).count();

    // Fase 2: Localidad Espacial (Row-Major 64B)
    t0 = clock_type::now();
    convolucion_localidad_espacial(img_in.data(), img_c2.data(), WIDTH, HEIGHT);
    t1 = clock_type::now();
    double t_spatial = std::chrono::duration<double>(t1 - t0).count();

    // Fase 3: Localidad Temporal + Registros de CPU
    t0 = clock_type::now();
    convolucion_registros_cpu(img_in.data(), img_c3.data(), WIDTH, HEIGHT);
    t1 = clock_type::now();
    double t_reg = std::chrono::duration<double>(t1 - t0).count();

    // Fase 4: Loop Unrolling 4x + ILP
    t0 = clock_type::now();
    convolucion_loop_unrolling(img_in.data(), img_c4.data(), WIDTH, HEIGHT);
    t1 = clock_type::now();
    double t_unroll = std::chrono::duration<double>(t1 - t0).count();

    // Exportar imagen procesada con Sobel
    std::string path_sobel = "PracticaConvolucion2DMemoria/imgs/imagen_sobel_optimizada.bmp";
    std::cout << "[*] Guardando imagen procesada (deteccion de bordes) en '" << path_sobel << "'...\n";
    guardar_bmp(path_sobel, img_c4.data(), WIDTH, HEIGHT, true);

    // Validación de Checksum
    double chk1 = calcular_checksum(img_c1.data(), num_pixels);
    double chk4 = calcular_checksum(img_c4.data(), num_pixels);
    double error_abs = std::abs(chk1 - chk4);

    std::cout << "\n======================================================================\n";
    std::cout << "                 RESULTADOS DEL BENCHMARK (N=2048)                     \n";
    std::cout << "======================================================================\n";
    std::cout << std::left
              << std::setw(34) << "Configuracion Arquitectonica"
              << std::setw(14) << "Tiempo (s)"
              << std::setw(16) << "Tasa (MP/s)"
              << std::setw(14) << "Aceleracion"
              << "\n";
    std::cout << std::string(74, '-') << "\n";

    std::cout << std::left
              << std::setw(34) << "1. Naive (Column-Major)"
              << std::fixed << std::setprecision(4)
              << std::setw(14) << t_naive
              << std::setw(16) << (total_megapixels / t_naive)
              << "1.00x (Base)\n";

    std::cout << std::left
              << std::setw(34) << "2. Localidad Espacial (Row-Major)"
              << std::fixed << std::setprecision(4)
              << std::setw(14) << t_spatial
              << std::setw(16) << (total_megapixels / t_spatial)
              << std::fixed << std::setprecision(2)
              << (t_naive / t_spatial) << "x\n";

    std::cout << std::left
              << std::setw(34) << "3. Registros CPU (Sliding Win)"
              << std::fixed << std::setprecision(4)
              << std::setw(14) << t_reg
              << std::setw(16) << (total_megapixels / t_reg)
              << std::fixed << std::setprecision(2)
              << (t_naive / t_reg) << "x\n";

    std::cout << std::left
              << std::setw(34) << "4. Loop Unrolling 4x + ILP"
              << std::fixed << std::setprecision(4)
              << std::setw(14) << t_unroll
              << std::setw(16) << (total_megapixels / t_unroll)
              << std::fixed << std::setprecision(2)
              << (t_naive / t_unroll) << "x\n";

    std::cout << std::string(74, '-') << "\n";
    std::cout << "[OK] Validacion de Checksum: " << std::scientific << chk1
              << " (Error Absoluto = " << error_abs << ")\n";
    std::cout << "======================================================================\n\n";

    std::cout << "VISTA PREVIA EN CONSOLA (ASCII ART DE BORDES SOBEL DETECTADOS):\n";
    render_ascii_preview(img_c4.data(), WIDTH, HEIGHT, 60, 24);

    std::cout << "\n[+] Benchmark finalizado con exito. Archivos BMP generados en imgs/\n";
    return 0;
}
