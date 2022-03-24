import React from "react";
import "./Slider.css";

function Slider() {
  let [curSpeed, setCurSpeed] = React.useState<number>();

  React.useEffect(() => {
    if (typeof curSpeed === "undefined") {
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
          if (data.hasOwnProperty("curSpd")) {
            setCurSpeed(data.curSpd);
          }
        })
        .catch((error) => console.log("Request failed", error));
    }
  });

  function updateSpeed(event: React.ChangeEvent<HTMLInputElement>) {
    let spd = parseFloat(event.target.value);
    setCurSpeed(spd);

    fetch("/api", {
      method: "PUT",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({ curSpd: spd }),
    });
  }

  return (
    <>
      {typeof curSpeed !== "undefined" && (
        <>
          <p>Speed:</p>
          <input
            type="range"
            className="slider"
            max={100}
            min={0}
            step={1}
            onChange={updateSpeed}
            value={curSpeed}
          />
        </>
      )}
    </>
  );
}

export default Slider;
