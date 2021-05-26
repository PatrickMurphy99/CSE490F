let shapeFraction = 0; // tracks the new shape fraction off serial
let serial; // the Serial object
let pHtmlMsg; // used for displaying messages via html (optional)
let centerPointX; // center of circle X coordinate
let centerPointY; // center of circle Y coordinate
let radius = 20; // radius of the circle
let serialOptions = { baudRate: 115200 };


function setup() {
  createCanvas(800, 800);
   serial = new Serial();
   centerPointX = 300;
   centerPointY = 300;
   serial.on(SerialEvents.CONNECTION_OPENED, onSerialConnectionOpened);
   serial.on(SerialEvents.CONNECTION_CLOSED, onSerialConnectionClosed);
   serial.on(SerialEvents.DATA_RECEIVED, onSerialDataReceived);
   serial.on(SerialEvents.ERROR_OCCURRED, onSerialErrorOccurred);

   // If we have previously approved ports, attempt to connect with them
   //serial.autoConnectAndOpenPreviouslyApprovedPort(serialOptions);

   // Add in a lil <p> element to provide messages. This is optional
   pHtmlMsg = createP("Click anywhere on this page to open the serial connection dialog");
   }
  
   function draw() {
     background(200);
     if (centerPointX - radius <= 0) {
       centerPointX += 2 * radius;
     } else if (centerPointX + radius >= 800) {
       centerPointX -= 2 * radius;
     }
     if (centerPointY - radius <= 0) {
       centerPointY += 2 * radius;
     } else if (centerPointY + radius >= 800) {
       centerPointY -= 2 * radius;
     }
     fill(100);
     circle(centerPointX, centerPointY, 2 * radius);
     
   }
   function onSerialErrorOccurred(eventSender, error) {
     console.log("onSerialErrorOccurred", error);
     pHtmlMsg.html(error);
   }
  
   function onSerialConnectionOpened(eventSender) {
     console.log("onSerialConnectionOpened");
     pHtmlMsg.html("Serial connection opened successfully");
   }
  
   function onSerialConnectionClosed(eventSender) {
     console.log("onSerialConnectionClosed");
     pHtmlMsg.html("onSerialConnectionClosed");
    }
  
   function onSerialDataReceived(eventSender, newData) {
     console.log("onSerialDataReceived");
     pHtmlMsg.html("onSerialDataReceived: ");
     xNum = newData.substring(0, newData.indexOf(' '));
     yNum = newData.substring(newData.indexOf(' ') + 1, newData.length);
     console.log("new data is " + newData + "x is " + xNum + " y is " + yNum);
     centerPointX += parseInt(xNum);
     centerPointY += parseInt(yNum);
     console.log("hello");
     console.log(centerPointX + " " + centerPointY);
   }

  function mouseClicked() {
     if (!serial.isOpen()) {
       serial.connectAndOpen(null, serialOptions);
     }
   }

