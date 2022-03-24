import React from "react";
import "./Sensors.css";

type AccelData = {
  x: number;
  y: number;
  z: number;
};

type GyroData = {
  x: number;
  y: number;
  z: number;
};

type SensorData = {
  accelData: AccelData;
  gyroData: GyroData;
};

function Sensors() {
  let [curData, setCurData] = React.useState<SensorData>();

  function fetchData() {
    fetch("/data")
      .then((res) => {
        if (res.ok) {
          return Promise.resolve(res);
        } else {
          return Promise.reject(new Error(res.statusText));
        }
      })
      .then((res) => res.json())
      .then((data) => {
        let tmp: SensorData = {
          accelData: { x: 0, y: 0, z: 0 },
          gyroData: { x: 0, y: 0, z: 0 },
        };
        if (data.hasOwnProperty("accel")) {
          tmp.accelData.x = data.accel.x;
          tmp.accelData.y = data.accel.y;
          tmp.accelData.z = data.accel.z;
        }
        if (data.hasOwnProperty("gyro")) {
          tmp.gyroData.x = data.gyro.x;
          tmp.gyroData.y = data.gyro.y;
          tmp.gyroData.z = data.gyro.z;
        }
        setCurData(tmp);
      })
      .catch((error) => console.log("Request failed", error));
  }

  React.useEffect(() => {
    if (typeof curData === "undefined") {
      fetchData();
    }
  });

  setTimeout(() => {
    fetchData();
  }, 100);

  return (
    <>
      {typeof curData !== "undefined" && (
        <>
          <p>Accelerometer:</p>
          <p>{`x: ${curData.accelData.x.toFixed(
            2
          )} y: ${curData.accelData.y.toFixed(
            2
          )} z: ${curData.accelData.z.toFixed(2)}`}</p>
          <p>Gyroscope:</p>
          <p>{`x: ${curData.gyroData.x.toFixed(
            2
          )} y: ${curData.gyroData.y.toFixed(
            2
          )} z: ${curData.gyroData.z.toFixed(2)}`}</p>
        </>
      )}
    </>
  );
}

export default Sensors;
