import React from "react";
import "./App.css";

function App() {
  function Buttons() {
    let [curSelected, setCurSelected] = React.useState<String>();

    React.useEffect(() => {
      if (typeof curSelected === "undefined") {
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
            if (data.hasOwnProperty("curDir")) {
              setCurSelected(data.curDir);
            }
          })
          .catch((error) => console.log("Request failed", error));
      }
    });

    function handleClick(dir: string) {
      if (dir === curSelected) {
        setCurSelected("none");
      } else {
        setCurSelected(dir);
      }

      fetch("/api", {
        method: "PUT",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({ curDir: dir === curSelected ? "none" : dir }),
      });
    }

    return (
      <div>
        {curSelected && (
          <>
            <button
              className={curSelected === "N" ? "selected" : "notSelected"}
              onClick={(_: React.MouseEvent<HTMLButtonElement>) =>
                handleClick("N")
              }
            >
              North
            </button>
            <button
              className={curSelected === "E" ? "selected" : "notSelected"}
              onClick={(_: React.MouseEvent<HTMLButtonElement>) =>
                handleClick("E")
              }
            >
              East
            </button>
            <button
              className={curSelected === "S" ? "selected" : "notSelected"}
              onClick={(_: React.MouseEvent<HTMLButtonElement>) =>
                handleClick("S")
              }
            >
              South
            </button>
            <button
              className={curSelected === "W" ? "selected" : "notSelected"}
              onClick={(_: React.MouseEvent<HTMLButtonElement>) =>
                handleClick("W")
              }
            >
              West
            </button>
          </>
        )}
      </div>
    );
  }

  return (
    <div className="App">
      <header className="App-header">
        <h1>Microcontroller Control Portal</h1>
        <h2>Control Directions:</h2>
        <Buttons />
        <h2>Sensor Data:</h2>
      </header>
    </div>
  );
}

export default App;
