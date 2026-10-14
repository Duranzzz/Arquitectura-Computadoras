#!/bin/bash
# ==============================================================================
# Script de Compilacion y Ejecucion del Benchmark de Buses de Comunicacion e IPC
# Arquitectura de Computadoras (SIS-131) - UCB San Pablo
# ==============================================================================

set -e

echo "======================================================================"
echo " COMPILANDO PROGRAMAS IPC (Prod/Cons NO OPT & OPT) con -O3"
echo "======================================================================"

gcc -O3 Prod_NO.c -o Prod_NO
gcc -O3 Cons_NO.c -o Cons_NO
gcc -O3 Prod_OPT.c -o Prod_OPT -lrt -lpthread
gcc -O3 Cons_OPT.c -o Cons_OPT -lrt -lpthread

echo "[OK] Todos los binarios fueron compilados exitosamente."
echo ""

echo "======================================================================"
echo " PRUEBA 1: VERSION NO OPTIMIZADA (Named Pipes / FIFO)"
echo " Overhead de 20,000,000 syscalls y copias User <-> Kernel Space"
echo "======================================================================"

# Limpiar FIFO previo si existiera
rm -f /tmp/bus_fifo_no_opt

# Ejecutar consumidor en segundo plano y productor en primer plano
./Cons_NO &
CONS_PID=$!
sleep 0.2
./Prod_NO
wait $CONS_PID

echo ""
echo "======================================================================"
echo " PRUEBA 2: VERSION OPTIMIZADA (POSIX Shared Memory + Semaphores + Batching)"
echo " Zero-Copy en RAM mapeada con mmap() y reduccion del 99.8% de syscalls"
echo "======================================================================"

# Ejecutar productor y consumidor optimizados
./Prod_OPT &
PROD_PID=$!
sleep 0.1
./Cons_OPT
wait $PROD_PID

echo ""
echo "======================================================================"
echo " BENCHMARK COMPLETADO EXITOSAMENTE"
echo "======================================================================"
