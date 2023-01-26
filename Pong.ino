#include "LedControl.h"
#include "binary.h"
#include <IRremote.h>
#include "string.h"
#include "SoftwareSerial.h"


int pin_recv = 5;

//IRrecv irrecv(4);
//IRrecv irrecv(5);
decode_results results;
LedControl lc = LedControl(11,13,10,2);

unsigned long delaytime = 1000;

int matrix = 0;
int player2 = 7;
int player1 = 224;
int row = 0;
int ballPosition[2] = {5,7};
int ballDirection[2] = {-1,1};
unsigned long ballAll = 100; 
unsigned long lastBall;

int lastStatePlayer1 = 1;
int lastStatePlayer2 = 1;

bool isReseting;

void setup() {
  // put your setup code here, to run once:
  lc.shutdown(0, false);
  lc.shutdown(1, false);
  lc.setIntensity(1,8);
  lc.setIntensity(0,8); 
  lc.clearDisplay(1);
  //lc.setDigit(0, 7, (byte)7, false);
  //lc.setDigit(1, 8, (byte)8, false);
  Serial.begin(9600);
  IrReceiver.begin(pin_recv);
  delay(500);
  
  lc.setRow(1,0,player1);
  lc.setRow(0,0,player2);
  lc.setLed(matrix, ballPosition[0], ballPosition[1], true);
  delay(500);
  

  pinMode(4, INPUT);
  pinMode(5, INPUT);
}

long readIR(){
  if(IrReceiver.decode()){
    
    // Serial.println("Received something...");
    //IrReceiver.printIRResultShort(&Serial);
    //Serial.println();
    //Serial.println(IrReceiver.decodedIRData.decodedRawData);
    IrReceiver.resume();
    return IrReceiver.decodedIRData.decodedRawData;
  }
  return 0;
}

void setPlayerPos(){
  long buttonValue = readIR();
  switch(buttonValue){
    case 3108437760 :
      if (pin_recv == 4){
        if (player1 > 7){
          player1 /= 2;
        }
        lc.setRow(1, 0, player1);
      }else if (pin_recv == 5){
        if (player2 > 7){
          player2 /= 2;
        }
        lc.setRow(0, 0, player2);
      }
      break;
    case 3927310080 :
      if (pin_recv == 4){
        if (player1 < 224){
          player1 *= 2;
        }
        lc.setRow(1, 0, player1);     
      }else if (pin_recv == 5){
        if (player2 < 224){
          player2 *= 2;
        }
        lc.setRow(0, 0, player2);
      }
      break;
  }
}

int power(int num, int powerNum){
  if (powerNum <= 0){
    return 1;
  }
  return power(num, powerNum - 1) * num;  
}



void moveBall(){
    lc.setLed(matrix, ballPosition[0], ballPosition[1], false);
    ballPosition[0] -= ballDirection[0];
    ballPosition[1] -= ballDirection[1];
    lc.setLed(matrix, ballPosition[0], ballPosition[1], true);
}

void setDirection(){
  if(ballPosition[0] == 1){
    /*if(ballAll > 50){
      ballAll /= 2;
    }*/
    
    if (matrix == 1){ 
      if(player1 == 7 * power(2, 7 - ballPosition[1])){
        ballDirection[0] = -1;
        ballDirection[1] = -1;
      }else if(player1 == 7 * power(2, 6 - ballPosition[1])){
        ballDirection[0] = -1;
        ballDirection[1] = 0;
      }else if(player1 == 7 * power(2, 5 - ballPosition[1])){
        ballDirection[0] = -1;
        ballDirection[1] = 1;
      }else{
        ballDirection[1] = 0;
      }
    }
    
    if (matrix == 0){
      if(player2 == 7 * power(2, 7 - ballPosition[1])){
        ballDirection[0] = -1;
        ballDirection[1] = -1;
      }else if(player2 == 7 * power(2, 6 - ballPosition[1])){
        ballDirection[0] = -1;
        ballDirection[1] = 0;
      }else if(player2 == 7 * power(2, 5 - ballPosition[1])){
        ballDirection[0] = -1;
        ballDirection[1] = 1;
      }else{
        ballDirection[1] = 0;
      }
    }


  }else if(ballPosition[0] == 7){
    if (ballDirection[0] == -1 /*&& matrix == 1*/){
      lc.setLed(matrix, ballPosition[0], ballPosition[1], false);
      if (matrix == 1){
        matrix = 0;
        ballDirection[0] = 1;
      }else if (matrix == 0){
        matrix = 1;
        ballDirection[0] = 1;
      }
      ballDirection[1] = -ballDirection[1];
      ballPosition[1] = 7 - ballPosition[1];
      ballPosition[0] = 8; 
    }
  }

  if ((ballPosition[1] == 7 && ballDirection[1] == -1) || (ballPosition[1] == 0 && ballDirection[1] == 1)){
    ballDirection[1] = -ballDirection[1];
  }

  if (ballPosition[0] < 0){
    resetBall();
  }

}

void resetBall(){
  ballPosition[0] = 7;
  ballPosition[1] = 4;
  ballDirection[0] = 1;
  ballDirection[1] = 0;
  lc.setLed(matrix, ballPosition[0], ballPosition[1], true);
  ballAll = 2000;
  isReseting = true;
}

void changeReceiver(){
  int value1;
  int value2;

  value1 = digitalRead( 4 );
  value2 = digitalRead( 5 );  

  
  if(value1 != lastStatePlayer1){
    if (value1 == 0){
      pin_recv = 4;
      IrReceiver.begin(pin_recv);
      lastStatePlayer1 = value1;
    }else {
      lastStatePlayer1 = 1;
    }
    
  }

  if(value2 != lastStatePlayer2){
    if (value2 == 0){
      pin_recv = 5;
      IrReceiver.begin(pin_recv);
      lastStatePlayer2 = value2;
    }else {
      lastStatePlayer2 = 1;
    }
    
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  changeReceiver(); 
  
  unsigned long topLoop = millis();

  if (topLoop - lastBall >= ballAll) {
    lastBall = topLoop;
    setDirection();
    if(ballAll!=2000){
      moveBall();
    }else if(isReseting){
      isReseting = false;
    }else{
      ballAll = 100;
    }
    
    
  }
  setPlayerPos();
  
  

}
