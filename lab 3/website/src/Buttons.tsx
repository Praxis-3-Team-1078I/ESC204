import React from "react";
import "./Buttons.css";

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
      {typeof curSelected !== "undefined" && (
        <>
          <button
            className={curSelected === "N" ? "selected" : "notSelected"}
            onClick={() => handleClick("N")}
          >
            Forward
          </button>
          <button
            className={curSelected === "E" ? "selected" : "notSelected"}
            onClick={() => handleClick("E")}
          >
            Right
          </button>
          <button
            className={curSelected === "S" ? "selected" : "notSelected"}
            onClick={() => handleClick("S")}
          >
            Backward
          </button>
          <button
            className={curSelected === "W" ? "selected" : "notSelected"}
            onClick={() => handleClick("W")}
          >
            Left
          </button>
        </>
      )}
    </div>
  );
}

export default Buttons;
