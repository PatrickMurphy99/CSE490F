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
let pastTime = 0;
let inputData = Array(10);
let video;
let poseNet;
let currentPos;
let poseNetOptions;
function setup() {
  createCanvas(1280, 640);
   serial = new Serial();
   pauseTime = millis();
   serial.on(SerialEvents.CONNECTION_OPENED, onSerialConnectionOpened);
   serial.on(SerialEvents.CONNECTION_CLOSED, onSerialConnectionClosed);
   serial.on(SerialEvents.DATA_RECEIVED, onSerialDataReceived);
   serial.on(SerialEvents.ERROR_OCCURRED, onSerialErrorOccurred);
   video = createCapture(VIDEO);
   video.hide();
   poseNetOptions = { detectionType: "single", minConfidence: 0.8};
   poseNet = ml5.poseNet(video, poseNetOptions, onPoseNetModelReady);
   poseNet.on('pose', onPoseDetected);

   // If we have previously approved ports, attempt to connect with them
   //serial.autoConnectAndOpenPreviouslyApprovedPort(serialOptions);

   // Add in a lil <p> element to provide messages. This is optional
   pHtmlMsg = createP("Click anywhere on this page to open the serial connection dialog");
}
   function onPoseNetModelReady() {
     console.log("ok the poseNet model is ready....");
   }

   function onPoseDetected(poses) {
    currentPos = poses;
   }
  
   function draw() {
    background(200);
    fill(100);
    if (inputData[0] === "INTRO") {
       prompt0 = "current snake size " + inputData[2];
       prompt1 = "increase snake size";
       prompt2 = "decrease snake size";
       prompt3 = "finalize snake size";
       if (inputData[1] === "2") {
         prompt0 = "current snake radius " + inputData[2];
         prompt1 = "increase snake radius";
         prompt2 = "decrease snake radius";
         prompt3 = "finalize snake radius";
       }
       inputButtons(prompt0, prompt1, prompt2, prompt3, 0);
     } else if (inputData[0] === "LOADING") {
       textSize(32);
       text(inputData[1], 400, 50);
       drawSnake(x1pos, y1pos, length1, "red");
       drawSnake(x2pos, y2pos, length2, "blue");
     } else if (inputData[0] === "GAMEOVER") {
       textSize(32);
       text('Player ' + inputData[1] + ' won', 0, 50);
       text('Player 1 total ' + inputData[2], 0, 100);
       text('Player 2 total ' + inputData[3], 0, 150);
       prompt0 = "Play again or change the rules? ";
       if (inputData[4] === "0") {
         prompt0 += "change the rules";
       } else {
         prompt0 += "play again";
       }
       inputButtons(prompt0, "Change answer", "Change answer", "finalize answer", 200);
     } else if (inputData[0] === "PAUSED") {
       textSize(32);
       prompt0 = "The game is currently paused, should we reset the game? ";
       if (inputData[1] === "1") {
         prompt0 += "no";
       } else {
         prompt0 += "yes";
       }
       inputButtons(prompt0, "Change answer", "Change answer", "finalize answer", 0);
     } else if (inputData[0] === "PLAYING" && currentPos && currentPos[0].pose.rightWrist.y > 150) {
     inputButtons("", "DO NOT PRESS RN", "DO NOT PRESS RN", "DO NOT PRESS", 200);
     let movingX = 0;
     let movingY = 0;
     if (millis() - pastTime > 1) {
         if (mouseX - x2pos[0] > radius) {
             movingX = 1;
         } else if (mouseX - x2pos[0] < -1 * radius) {
             movingX = -1;
         }
         if (mouseY - y2pos[0] > radius) {
             movingY = 1;
         } else if (mouseY - y2pos[0] < -1 * radius) {
             movingY = -1;
         }
         pastTime = millis();
         let pressed = 0;
         if (mouseIsPressed) {
           pressed = 1;
         }
         serial.writeLine(movingX +',' + movingY +' ' + pressed);
     }
     drawSnake(x1pos, y1pos, length1, "red");
     drawSnake(x2pos, y2pos, length2, "blue");
    } else if (inputData[0] === "PLAYING") {
      serial.writeLine(-100 + ',' + -100 + ' ' + -100);
    }
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
     pHtmlMsg.html("onSerialDataReceived: ");
     inputData = newData.split(" ");
     if (inputData[0] === "PLAYING" || inputData[0] === "LOADING") {
          positionFinder(inputData);
      } 
    }
  
  function positionFinder(positions) {
    let i = 1;
    let k = 0;
    length1 = 0;
    length2 = 0;
    if (inputData[0] === "LOADING") {
      i = 2;
      k = 1;
    }
    while (positions[i] !== "|") {
        numbers = positions[i].split(",");
        x1pos[i - 1 - k] = 10 * parseInt(numbers[0]);
        y1pos[i - 1 - k] = 10 * parseInt(numbers[1]);
        length1++;
        i++;
     }
     for (let j = i + 1; j < positions.length - 1; j++) {
         numbers = positions[j].split(",");
         x2pos[j - i - 1] = 10 * parseInt(numbers[0]);
         y2pos[j - i - 1] = 10 * parseInt(numbers[1]);
         length2++;
     }
  }
  
  function mouseClicked() {
     if (!serial.isOpen()) {
       serial.connectAndOpen(null, serialOptions);
    }
  }

  function drawSnake(arrayx, arrayy, length, color) {
    fill('gold');
    circle(arrayx[0], arrayy[0], 2 * radius);
    fill(color);
    for (let i = 1; i < length; i++) {
        circle(arrayx[i], arrayy[i], 2 * radius);
    }
  }

  function inputButtons(prompt0, prompt1, prompt2, prompt3, indent) {
    textSize(32);
    text(prompt0, 0, 50 + indent);   
    button1 = createButton(prompt1);
    button1.position(100, 700);
    button1.size(200, 100);
    button1.mousePressed( () => serial.writeLine(1));
    button2 = createButton(prompt2);
    button2.position(400, 700);
    button2.size(200, 100);
    button2.mousePressed( () => serial.writeLine(-1));
    button3 = createButton(prompt3);
    button3.position(700, 700);
    button3.size(200, 100);
    button3.mousePressed( () => serial.writeLine('done'));
    if (currentPos && currentPos[0].pose.rightWrist.y < 150) {
      serial.writeLine('done');
    } 
  }