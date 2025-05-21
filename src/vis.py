import numpy as np
import matplotlib.pyplot as plt

def load_matrix(filename):
    return np.genfromtxt(filename, delimiter=',')

hologram = load_matrix('hologram.csv')
reconstructed = load_matrix('reconstructed.csv')

plt.figure(figsize=(10, 5))
plt.subplot(121)
plt.imshow(hologram, cmap='gray')
plt.title('Голограмма')

plt.subplot(122)
reconstructed_normalized = reconstructed / reconstructed.max()
print(reconstructed_normalized)
plt.imshow(reconstructed, cmap='gray', vmax=reconstructed.max()/1e7)  # Подберите vmaxplt.title('Восстановленное изображение')
plt.show()