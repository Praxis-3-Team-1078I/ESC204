import numpy as np
import matplotlib.pyplot as plt
from perlin_numpy import generate_fractal_noise_2d

from PIL import Image

def scale(A, lower=0, upper=1):
    """Scale an np array between lower & upper"""
    return (A-np.min(A))/(np.max(A) - np.min(A))*(upper-lower)+lower

class Enviroment:
    """A grid enviroment with water quality between 0 & 1"""
    def __init__(self, size):
        # Size
        self.size = size

        # Position
        self.position = np.array([0, 0], dtype=np.float64)

        # Initalize grid
        self.quality_grid = self.cloud(512, 512, 3)

    def step(self, n=1, centering_factor=0.0001):
        for _ in range(n):
            self.position += np.random.normal(size=self.position.shape).astype(np.float64)
            self.position *= 1-centering_factor

    def quality(self):
        center = self.position.astype(np.int32)
        grid = self.quality_grid[center[0]-self.size[0]//2+255:center[0]+self.size[0]//2+257, center[1]-self.size[1]//2+255:center[1]+self.size[1]//2+257]
        return grid

    def cloud(self, x, y, octaves=1):
        cloud = scale(generate_fractal_noise_2d((x, y), (x//32, y//32), octaves))
        return cloud


if __name__ == '__main__':
    env = Enviroment((256, 256))

    rain = []
    for _ in range(14):
        env.step(100)
        rain.append(env.quality())

    real = sum(rain)/len(rain)
    noisy = real+np.random.normal(0, 1, size=real.shape)

    real = scale(real, upper=255)
    noisy = scale(noisy, upper=255)
    fixed = (real+noisy)/2

    print(np.mean(np.abs(real-noisy)/255))
    print(np.mean(np.abs(real-fixed)/255))

    real = Image.fromarray(real).convert("RGB")
    noisy = Image.fromarray(noisy).convert("RGB")
    fixed = Image.fromarray(fixed).convert("RGB")

    real.save("real.png")
    noisy.save("noisy.png")
    fixed.save("fixed.png")