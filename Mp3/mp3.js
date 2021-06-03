let serial; // the Serial object
let pHtmlMsg; // used for displaying messages via html (optional)
let serialOptions = { baudRate: 115200 };
let radius = 30;
let x1pos = Array(10);
let y1pos = Array(10);
let x2pos = Array(10);
let y2pos = Array(10);
let length1 = 0;
let length2 = 0;
let pastX = 0;
let pastY = 0;
let pastTime = 0;


function setup() {
  createCanvas(1280, 640);
   serial = new Serial();
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
     fill(100);
     let movingX = 0;
     let movingY = 0;
     console.log("past time was " + pastTime + " current time " + millis());
     
    if (mouseX > x2pos[0] && pastX !== mouseX) {
        movingX = -1;
    } else if (pastX !== mouseX) {
        movingX = 1;
    }
    if (mouseY > y2pos[0] && pastY !== mouseY) {
        movingY = -1;
    } else if (pastY !== mouseY) {
        movingY = 1;
    }
     pastTime = millis();
     console.log("past was " + pastX + " also " + pastY + " now " + mouseX + " and " + mouseY);
     pastX = mouseX;
     pastY = mouseY;
     serial.writeLine(movingX + "," + movingY);
     drawSnake(x1pos, y1pos, length1);
     drawSnake(x2pos, y2pos, length2);
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
     console.log("hello " + newData);
     let positions = newData.split(" ");
     let i = 0;
     while (positions[i] !== "|") {
         numbers = positions[i].split(",");
         x1pos[i] = 10 * parseInt(numbers[0]);
         y1pos[i] = 10 * parseInt(numbers[1]);
         length1++;
         i++;
      }
      for (let j = i; j < positions.length - 1; j++) {
          numbers = positions[j].split(",");
          x2pos[j] = 10 * parseInt(numbers[0]);
          y2pos[j] = 10 * parseInt(numbers[1]);
          length2++;
      }
    }

  function mouseClicked() {
     if (!serial.isOpen()) {
       serial.connectAndOpen(null, serialOptions);
    }
  }

  function drawSnake(arrayx, arrayy, length) {
      for (let i = 0; i < length; i++) {
        circle(arrayx[i], arrayy[i], 2 * radius);
      }
  }