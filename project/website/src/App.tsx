import React from "react";
import "./App.css";

function App() {
  let [curState, setCurState] = React.useState<string>("unknown");

  function getMessage(): string {
    if (curState === "safe") {
      return "Safe to drink.";
    } else if (curState === "contaminated") {
      return "Contamination detected! Do not drink.";
    } else {
      return "Unknown. Do not drink.";
    }
  }

  function fetchData() {
    fetch("/api")
      .then((res) => {
        if (res.ok) {
          return Promise.resolve(res);
        } else {
          return Promise.reject(new Error(res.statusText));
        }
      })
      .then((res) => res.json())
      .then((data) => {
        if (data.hasOwnProperty("waterState")) {
          setCurState(data.waterState);
        }
      })
      .catch((error) => console.log("Request failed", error));
  }

  setTimeout(() => {
    fetchData();
  }, 10000);

  return (
    <div className="App">
      <header className="App-header">
        <h1>Water Quality Info Portal</h1>
        <h2>Current water state: {getMessage()}</h2>
      </header>
    </div>
  );
}

export default App;
