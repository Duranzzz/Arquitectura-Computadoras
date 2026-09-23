#!/usr/bin/env python3
"""
saturacion_swap.py
Programa en Python para saturar la memoria RAM disponible y forzar el uso de Swap en Linux.

Arquitectura de Computadoras (SIS-131)
Universidad Catolica Boliviana "San Pablo" - Sede Santa Cruz
Docente: Ing. Paulo Cesar Loayza Carrasco
Estudiante: Leonardo Duran Cuenca

Principio Arquitectonico:
Al utilizar bytearray, Python reserva bloques de memoria contigua e inicializa cada byte,
lo que provoca Page Faults forzando la asignacion de marcos fisicos en la memoria RAM.
Cuando la RAM disponible se agota, el Administrador de Memoria Virtual del kernel Linux
traslada paginas a la particion Swap (Paging / Swapping).
"""

import sys
import time
import signal

CHUNK_SIZE_MB = 50
CHUNK_SIZE_BYTES = CHUNK_SIZE_MB * 1024 * 1024

memory_holder = []
keep_running = True

def handle_sigint(signum, frame):
    global keep_running
    keep_running = False
    print("\n\n[INFO] Interrupcion recibida (Ctrl+C). Deteniendo asignacion de memoria...")

def main():
    signal.signal(signal.SIGINT, handle_sigint)
    signal.signal(signal.SIGTERM, handle_sigint)

    sleep_sec = 0.1
    if len(sys.argv) > 1:
        try:
            sleep_sec = float(sys.argv[1]) / 1000.0
        except ValueError:
            pass

    print("======================================================================")
    print("   PRUEBA DE SATURACION DE MEMORIA RAM Y ACTIVACION DE SWAP (PYTHON)  ")
    print("   Arquitectura de Computadoras - UCB San Pablo                       ")
    print("======================================================================")
    print(f" Tamano de bloque por paso: {CHUNK_SIZE_MB} MB")
    print(f" Retardo entre bloques:     {sleep_sec * 1000:.0f} ms")
    print(" Monitoreo recomendado en otra terminal:")
    print("   watch -n 1 free -h")
    print("   vmstat 1")
    print(" Presiona Ctrl+C en cualquier momento para detener y liberar la memoria.")
    print("======================================================================\n")

    iteration = 0
    total_mb = 0

    try:
        while keep_running:
            iteration += 1
            # bytearray inicializa la memoria a 0, forzando la reserva fisica de marcos de RAM
            block = bytearray(CHUNK_SIZE_BYTES)
            memory_holder.append(block)
            total_mb += CHUNK_SIZE_MB

            print(f" -> Paso {iteration:4d}: Asignados +{CHUNK_SIZE_MB} MB | Total retenido: {total_mb:6d} MB (~{total_mb / 1024.0:.2f} GB)")
            sys.stdout.flush()

            if sleep_sec > 0:
                time.sleep(sleep_sec)

    except MemoryError:
        print(f"\n[ALERTA] MemoryError alcanzado en el paso {iteration}. Memoria total: {total_mb} MB.")
    except Exception as e:
        print(f"\n[ERROR] Excepcion inesperada: {e}")

    print("\n======================================================================")
    print(" RESUMEN DE EJECUCION")
    print(f" Pasos completados:            {iteration}")
    print(f" Total de memoria fisica usada: {total_mb} MB (~{total_mb / 1024.0:.2f} GB)")
    print("======================================================================")

    try:
        input("\nPresiona Enter para liberar la memoria y finalizar...")
    except (KeyboardInterrupt, EOFError):
        pass

    print("[INFO] Liberando memoria...")
    memory_holder.clear()
    print("[FIN] Memoria liberada. Proceso terminado.")

if __name__ == "__main__":
    main()
