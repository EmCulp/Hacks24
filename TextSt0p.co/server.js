const express = require('express');
const { SerialPort } = require('serialport'); // Use destructuring to get SerialPort

const app = express();
const port = 3000;

// Replace with your Arduino's COM port
const arduinoPort = 'COM5'; // Change this to your Arduino's port

const serialPort = new SerialPort({
  path: arduinoPort,
  baudRate: 9600
});

serialPort.on('open', () => {
  console.log('Serial Port Opened');
});

serialPort.on('data', (data) => {
  console.log('Data:', data.toString());
  // You can emit this data to your webpage via Socket.IO or serve it through an endpoint
});

app.get('/', (req, res) => {
  res.sendFile(__dirname + '/textst0p.html');
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});
