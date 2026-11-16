"""
=============================================================================
  Práctica 1: 'Hola Mundo Cuántico' — Creación del Estado de Bell (|Φ⁺⟩)
=============================================================================
  Objetivo: Construir el circuito cuántico más fundamental: la creación de
  un par de cúbits máximamente entrelazados (Estado de Bell |Φ⁺⟩).
  Demostrar experimentalmente la superposición cuántica, el entrelazamiento
  y la correlación del 100% en las mediciones.

  Resultado esperado:
    - Estado |00⟩: ~50%
    - Estado |11⟩: ~50%
    - Estados |01⟩ y |10⟩: 0% (ausencia total)
=============================================================================
"""

from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator

# 1. Definición del circuito: 2 cúbits cuánticos y 2 bits clásicos
qc = QuantumCircuit(2, 2)

# 2. Creación de superposición equiprobable en el cúbit 0
#    Aplica Hadamard: |0⟩ → (|0⟩ + |1⟩)/√2
qc.h(0)

# 3. Creación de entrelazamiento máximo mediante CNOT
#    Estado compuesto: (|00⟩ + |11⟩)/√2 = |Φ⁺⟩
qc.cx(0, 1)

# 4. Medición de los cúbits en los registros clásicos
qc.measure([0, 1], [0, 1])

# 5. Visualización del circuito en consola
print("CIRCUITO CUÁNTICO:")
print(qc.draw(output="text"))

# 6. Instanciación del simulador y ejecución de 10,000 disparos (shots)
backend = AerSimulator()
shots = 10000
job = backend.run(qc, shots=shots)
counts = job.result().get_counts(qc)

# 7. Presentación de los resultados estadísticos
print(f"\nRESULTADOS DE LA MEDICIÓN ({shots} disparos):")
for bitstring, count in sorted(counts.items()):
    porcentaje = (count / shots) * 100
    print(f"Estado |{bitstring}⟩: {count} ocurrencias ({porcentaje:.2f}%)")
