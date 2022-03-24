import Buttons from "./Buttons";
import Slider from "./Slider";
import Sensors from "./Sensors";
import "./App.css";

function App() {
  return (
    <div className="App">
      <header className="App-header">
        <h1>Microcontroller Control Portal</h1>
        <h2>Control Directions:</h2>
        <Buttons />
        <Slider />
        <h2>Sensor Data:</h2>
        <Sensors />
      </header>
    </div>
  );
}

export default App;
