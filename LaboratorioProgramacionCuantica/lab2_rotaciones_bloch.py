"""
=============================================================================
  Práctica 2: Rotaciones en la Esfera de Bloch y Variación Continua
             de Probabilidades
=============================================================================
  Objetivo: Explorar la naturaleza continua del cúbit frente al bit clásico.
  Modulando el ángulo de rotación continuo θ alrededor del eje Y mediante
  la compuerta paramétrica Ry(theta), comprobar experimentalmente que:
    P(|0⟩) = cos²(θ/2)
    P(|1⟩) = sin²(θ/2)

  Resultado esperado:
    Polo Norte (θ=0):    P(|0⟩)=100%, P(|1⟩)=0%
    Ángulo 45° (θ=π/4):  P(|0⟩)≈85.4%, P(|1⟩)≈14.6%
    Ecuador (θ=π/2):     P(|0⟩)≈50%, P(|1⟩)≈50%
    Polo Sur (θ=π):      P(|0⟩)=0%, P(|1⟩)=100%
=============================================================================
"""

import math
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator


def simular_angulo(theta: float):
    """Simula un circuito de 1 cúbit con rotación Ry(theta)."""
    # 1. Circuito de 1 cúbit y 1 bit clásico
    qc = QuantumCircuit(1, 1)
    # 2. Rotación en el eje Y por ángulo continuo theta
    qc.ry(theta, 0)
    # 3. Medición en la base computacional Z
    qc.measure(0, 0)
    # 4. Simulación de 10,000 mediciones
    backend = AerSimulator()
    counts = backend.run(qc, shots=10000).result().get_counts()
    return counts


# Evaluación en 4 posiciones geométricas clave de la Esfera de Bloch:
angulos = [
    (0.0, "Polo Norte (theta = 0)"),
    (math.pi / 4, "Ángulo 45° (theta = pi/4)"),
    (math.pi / 2, "Ecuador (theta = pi/2, Estado |+>)"),
    (math.pi, "Polo Sur (theta = pi, Estado |1>)")
]

print("ROTACIONES EN LA ESFERA DE BLOCH — Verificación de P(|0⟩) = cos²(θ/2)")
print("=" * 75)
for theta, desc in angulos:
    c = simular_angulo(theta)
    p0 = (c.get('0', 0) / 10000) * 100
    p1 = (c.get('1', 0) / 10000) * 100
    t0 = (math.cos(theta / 2) ** 2) * 100
    t1 = (math.sin(theta / 2) ** 2) * 100
    print(f"{desc:40s} -> P(|0⟩): {p0:5.1f}% | P(|1⟩): {p1:5.1f}%  "
          f"[Teórico: {t0:5.1f}% | {t1:5.1f}%]")
