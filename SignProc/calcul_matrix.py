import numpy as np

phi = np.array([
    [1, 0],
    [1, 0.5],
    [1, 1],
    [1, 1.5]
])

phi_legendre = np.array([
    [1, 0, 0],
    [1, 0.5, -0.125],
    [1, 1, 1],
    [1, 1.5, 2.875]
])

x = np.array([0, 0.8, 1.4, 1.2])

# --- Calcul de la solution par moindres carrés ---
# sol = (phi^T * phi)^(-1) * phi^T * x
theta = np.linalg.inv(phi.T @ phi) @ phi.T @ x
e = x - phi @ theta
norm_e_squared = (np.linalg.norm(e)**2)/4
print("Solution [b, m] =", theta)
print("error e squared = ", norm_e_squared)


# --- Calcul de la solution par moindres carrés avec polynome de Legendre---

theta_legendre = np.linalg.inv(phi_legendre.T @ phi_legendre) @ phi_legendre.T @ x
e_legendre = x - phi_legendre @ theta_legendre

print("Solution [Theta0, Theta1, Theta2] =", theta_legendre)
norm_e_squared_legendre = (np.linalg.norm(e_legendre)**2)/4
print("error e_legendre_squared = ", norm_e_squared_legendre)

