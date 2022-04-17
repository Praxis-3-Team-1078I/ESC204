import numpy as np
import matplotlib.pyplot as plt
from perlin_numpy import generate_fractal_noise_2d

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
    env = Enviroment((20, 20))
    cloud1 = env.quality()
    env.step(100)
    cloud2 = env.quality()
    env.step(100)
    cloud3 = env.quality()
    env.step(100)
    cloud4 = env.quality()

    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2)
    ax1.imshow(cloud1, cmap="gray", interpolation='lanczos')
    ax2.imshow(cloud2, cmap="gray", interpolation='lanczos')
    ax3.imshow(cloud3, cmap="gray", interpolation='lanczos')
    ax4.imshow(cloud4, cmap="gray", interpolation='lanczos')
    plt.show()