import matplotlib.pyplot as plt
import numpy as np

t = np.array([0, 0.5, 1, 1.5])
x = np.array([0, 0.8, 1.4, 1.2])

# Paramètres de la droite moindre carré : y = a*x + b
theta1 = 0.85
theta0 = 0.294

# Paramètres de la droite moindre carré avec Legendre : y = a*x + b
theta0_l = -0.031
theta1_l = 1.774
theta2_l = -0.479


# --- Création de la droite ---
x_approx = theta1 * t + theta0
x_approx_legendre = theta2_l * t**2 + theta1_l * t + theta0_l

# --- Tracé ---
plt.figure(figsize=(8, 5))
plt.scatter(t, x, label="valeures mesurées", marker="o")
plt.scatter(t, x_approx, label="approximation moindre carré", marker="x", color="red")
plt.plot(t, x_approx, color="red")
plt.scatter(t, x_approx_legendre, label="approximation moindre carré avec Legendre", marker="x", color="blue")
plt.plot(t, x_approx_legendre, color="blue")

# Mise en forme
plt.xlabel("t")
plt.ylabel("x")
#plt.title("Scatterplot avec droite y = ax + b")
plt.legend()
plt.grid(True)
plt.show()
