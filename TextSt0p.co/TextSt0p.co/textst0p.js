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
			
function isAccess() {
    if (vars.docked) {
        document.getElementById("access").innerHTML = '<p>Access Granted.</p>';
        document.getElementById("access").style.backgroundColor = "green";
    } else {
        document.getElementById("access").innerHTML = '<p>Access Denied.</p>';
        document.getElementById("access").style.backgroundColor = "red";
    }
}
			
function send2ard() {	
	async function sendData(value) {
        const port = await navigator.serial.requestPort();
        await port.open({ baudRate: 9600 });
        
        const writer = port.writable.getWriter();
        const data = new Uint8Array([value]); // Convert your value to a byte array
        await writer.write(data);
        writer.releaseLock();
    }
      
    sendData(vars.butPressed); // Example value
}