import numpy as input_shape
from keras.layers import Dense, Input
from keras.models import Sequential

class Prediction_Network:
    def __init__(self, input_shape, layers=None, output_dim=1):
        self.model = Sequential()

        # Add input layer
        self.model.add(Input(shape=input_shape))

        # Add hidden layers
        if layers is not None:
            for n in layers:
                self.model.add(Dense(n, activation="relu"))

        # Add output layer
        self.model.add(Dense(output_dim, activation="softmax" if output_dim > 1 else "sigmoid"))

        # Complile
        self.model.compile(loss="binary_crossentropy", optimizer="adam", metrics=["accuracy"])

        self.model.summary()

    def train(self, inputs, outputs):
        print(inputs.shape)
        print(outputs.shape)
        self.model.fit(inputs, outputs, batch_size=1, epochs=5)

    def test(self, inputs, outputs):
        score = self.model.evaluate(inputs, outputs, verbose=0)
        print('Test loss:', score[0])
        print('Test accuracy:', score[1])

    def evaluate(inputs):
        return self.model.evaluate(inputs)