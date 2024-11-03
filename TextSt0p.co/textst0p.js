let vars = {};

vars.docked = Boolean(0);
vars.butPressed = Boolean(0);

function start() {
    document.getElementById("access").innerHTML = '<p>Access Denied.</p>';
    document.getElementById("access").style.backgroundColor = "red";
    isAccess();
}

function button() {
    vars.docked = !vars.docked;
    isAccess();
    if (vars.docked) {
        vars.butPressed = !vars.butPressed;
	    send2ard();
    }
}

/*function toggleLED(state) {
    fetch(/led/${state})
    .then(response => response.text())
    .then(data => alert(data))
    .catch(error => console.error('Error:', error));
}*/
			
function isAccess() {
    if (vars.docked) {
        document.getElementById("access").innerHTML = '<p>Access Granted.</p>';
        document.getElementById("access").style.backgroundColor = "green";
    } else {
        document.getElementById("access").innerHTML = '<p>Access Denied.</p>';
        document.getElementById("access").style.backgroundColor = "red";
    }
}
			
function readArd() {	
    const SerialPort = require('serialport');
    const Readline = require('@serialport/parser-readline');

    // Replace with your Arduino's serial port
    const port = new SerialPort({ path: '/dev/ttyUSB0', baudRate: 9600 });
    const parser = port.pipe(new Readline({ delimiter: '\r\n' }));

    parser.on('data', (line) => {
        console.log(`Received: ${line}`);
    });

    port.on('error', (err) => {
        console.error(`Error: ${err.message}`);
    });
}