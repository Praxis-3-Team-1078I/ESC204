import numpy as np
import matplotlib.pyplot as plt

from env import Enviroment
from neural_network import Prediction_Network

class Well:
    def __init__(self, position):
        self.memory = 0

        self.position = (position[0]+1, position[1]+1)

    def data(self, real_quality, noisy_quality):
        output = real_quality[self.position[0], self.position[1]]

        inputs = (noisy_quality[self.position[0], self.position[1]],
                  noisy_quality[self.position[0]+1, self.position[1]],
                  noisy_quality[self.position[0]-1, self.position[1]],
                  noisy_quality[self.position[0], self.position[1]+1],
                  noisy_quality[self.position[0], self.position[1]-1],
                  self.memory)

        return output, inputs


class Network:
    """A network of wells"""
    def __init__(self, size):
        self.size = size

        self.grid = Enviroment(self.size)

        self.prediction_network = Prediction_Network(6, (10, 10), 1)

        self.wells = [Well((x, y)) for x in range(self.size[0]) for y in range(self.size[1])]

    def day(self):
        self.grid.step(10)
        self.real_quality = self.grid.quality()
        self.noisy_quality = self.real_quality+np.random.normal(scale=0.04, size=self.real_quality.shape)

    def train(self):
        inputs, outputs = [], []
        for well in self.wells:
            ins, outs = well.data(self.real_quality, self.noisy_quality)
            inputs.append(ins)
            outputs.append(outs)
            well.memory = self.prediction_network.evaluate(outs)

        inputs = np.array(inputs)
        outputs = np.array(outputs)
        outputs = np.expand_dims(outputs, axis=-1)

        self.prediction_network.train(inputs, outputs)

    def evaluate(self):
        predicted_quality = np.copy(self.noisy_quality)
        for well in self.wells:
            ins, outs = well.data(self.real_quality, self.noisy_quality)
            val = self.prediction_network.evaluate(ins)
            predicted_quality[well.position[0], well.position[1]]

        return predicted_quality[1:-2, 1:-2]

if __name__ == '__main__':
    net = Network((10, 10))
    for _ in range(10):
        net.day()
        net.train()
        net.evaluate()

    print(net.real_quality)
    print(net.noisy_quality)
    print(net.evaluate())