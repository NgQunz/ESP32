#include <Adafruit_VL53L0X.h>
#include "BluetoothSerial.h"
#include "StepPin.h"
String device_name = "Micromouse";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

// ============================================================================= TOF SETUP  =======================================
// address we will assign for all 4 sensor
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31
#define LOX3_ADDRESS 0x32
int Front,Left, Right;

// set the pins to shutdown for all 4 sensors
#define SHT_LOX1 2  // right 
#define SHT_LOX2 19 // front
#define SHT_LOX3 25 //left


int corn_count = 0;
int pid_done = 1;
// objects for the vl53l0x
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox3 = Adafruit_VL53L0X();

// this holds the measurement
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;
VL53L0X_RangingMeasurementData_t measure3;

// ============================================================================ TOF FUNCTION ========================================

void setID() {
  // all reset
  digitalWrite(SHT_LOX1, LOW);    
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);    

  delay(10);
  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  digitalWrite(SHT_LOX3, HIGH);
 
  delay(10);

  // activating LOX1 and reseting LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);


  // initing LOX1
  if(!lox1.begin(LOX1_ADDRESS)) {
    Serial.println(F("Failed to boot first VL53L0X"));
    while(1);
  }
  delay(10);

  ///************************* sensor 2 activation 
  // activating LOX2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  //initing LOX2
  if(!lox2.begin(LOX2_ADDRESS)) {
    Serial.println(F("Failed to boot second VL53L0X"));
    while(1);
  }
  
   ///************************* Right activation  
  // activating LOX3
  digitalWrite(SHT_LOX3, HIGH);
  delay(10);

  //initing LOX3
  if(!lox3.begin(LOX3_ADDRESS)) {
    Serial.println(F("Failed to boot third VL53L0X"));
    while(1);
  }  
  
}

void ReadTof() {
  
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!
  lox3.rangingTest(&measure3, false); // pass in 'true' to get debug data printout!

  
  // print sensor one reading
  if(measure1.RangeStatus != 4) {     // if not out of range
    Front = measure1.RangeMilliMeter;    
    Serial.print("1: ");
    // Serial.print(Front);
  } 

  // print sensor two reading
  if(measure2.RangeStatus != 4) {
    Left = measure2.RangeMilliMeter;
    Serial.print(" 2: ");
    // Serial.print(Left);
  } 

   ///Robojax.com code see video https://youtu.be/0glBk917HPg
  // print sensor three reading
  if(measure3.RangeStatus != 4) {
    Serial.print(" 3: ");
    Right = measure3.RangeMilliMeter;
    // Serial.println(Right);
  } 
  

}
void SetupTof(){
  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);
  pinMode(SHT_LOX3, OUTPUT);

  Serial.println("Shutdown pins inited...");

  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);

  Serial.println("All four in reset mode...(pins are low)");
  
  Serial.println("Starting...");
  tone(14,300,100);
  setID();
  tone(14,1000,100);
  ReadTof();

}


// ============================================================================ Stepper setup ==========================================================================

int speed = 700;
int target = 0;

// D  = 67mm
// 1 step = 1.8 degree = 200 step
// 1 step == 1.052

void SetPinStep(){
  
    pinMode(LeftDir,OUTPUT);
    pinMode(LeftStep,OUTPUT);
    pinMode(RightDir,OUTPUT);
    pinMode(RightStep,OUTPUT);

}

//    Go straight mm for both wheels
void go_mm( int mm){                       
  int step = mm/1.052*16;
//    speed -=100;
  // set direction forward
  digitalWrite(LeftDir,LeftFord);
  digitalWrite(RightDir,RightFord);


  for (int i = 0; i < step ; i ++){ 
    digitalWrite(RightStep,HIGH);
    digitalWrite(LeftStep,HIGH); 
 
    digitalWrite(LeftStep,LOW); 
    digitalWrite(RightStep,LOW); 
    delayMicroseconds(speed); 
    // if ( speed > 200)
    //   speed -= 3 ;
  }
}
void back_mm(int mm){
    int step = mm/1.052*16;

  // set direction backward
  digitalWrite(LeftDir,LeftBack);
  digitalWrite(RightDir,RightBack);
  for (int i = 0; i < step ; i ++){ 
    digitalWrite(RightStep,HIGH);
    digitalWrite(LeftStep,HIGH); 
 
    digitalWrite(LeftStep,LOW); 
    digitalWrite(RightStep,LOW); 

    delayMicroseconds(speed); 
  }

}
void TurnRight(){
  digitalWrite(LeftDir,LeftFord);
  digitalWrite(RightDir,RightBack);
  for (int i = 0 ;  i < 143*8; i++){
    digitalWrite(LeftStep,HIGH);
    digitalWrite(LeftStep,LOW); 
    digitalWrite(RightStep,HIGH);
    digitalWrite(RightStep,LOW); 
    delayMicroseconds(500);     
  }
}

void TurnLeft(){
    digitalWrite(LeftDir,LeftBack);
  digitalWrite(RightDir,RightFord);
  for (int i = 0 ;  i < 143*8; i++){

    digitalWrite(LeftStep,HIGH);
    digitalWrite(LeftStep,LOW); 
    digitalWrite(RightStep,HIGH);
    digitalWrite(RightStep,LOW); 
    delayMicroseconds(500);     
  }
}

void KeepDistance(int target){
  int step = Front - target;
  step = step/1.052*16;
  if ( Front > target){
    digitalWrite(LeftDir,LeftFord);
    digitalWrite(RightDir,RightFord);
  }
  else
  {
    digitalWrite(LeftDir,LeftBack);
    digitalWrite(RightDir,RightBack);
  }
  for (int i = 0; i < step ; i ++){ 
    digitalWrite(RightStep,HIGH);
    digitalWrite(LeftStep,HIGH); 
 
    digitalWrite(LeftStep,LOW); 
    digitalWrite(RightStep,LOW); 
    delayMicroseconds(speed); 
    // if ( speed > 200)
    //   speed -= 3 ;
  }
}

void FixError(int value){
    int step = (value - target)/1.052*16;
//    speed -=100;
  // set direction forward
  if ( step > 0 ){
    digitalWrite(LeftDir,LeftBack);
  }
  else {

    step = -step;
    digitalWrite(LeftDir,LeftFord);

  }

  Serial.println(step);
  for (int i = 0; i < step ; i ++){ 
    // digitalWrite(RightStep,HIGH);
    digitalWrite(LeftStep,HIGH); 
 
    digitalWrite(LeftStep,LOW); 
    // digitalWrite(RightStep,LOW); 
    delayMicroseconds(speed); 
    // if ( speed > 200)
    //   speed -= 3 ;
  }
}
// ========================================================================== End Step setup ==========================================================================
void BltPrintTof(){
  if (SerialBT.available()) {
    SerialBT.print("Left: %d");
    SerialBT.print(Left);
    SerialBT.print("front: ");
    SerialBT.print(Front);
    SerialBT.print("Right: ");
    SerialBT.println(Right);

  }

}


// ========================================================================== Main function ==========================================================================

void LeftHand(){
  if (Left > 100) {
      TurnLeft();
      tone(14,500,100);
  }
  ReadTof();
  delay(500);
  while (Front < 60) {
      TurnRight();
      ReadTof();
      delay(500);
  }
  go_mm(180);
  ReadTof();
  FixError(Left); ///// fix position function  
  delay(500);
}

void TestBat(){
  int time = millis();
  speed = 300;
  while ( (millis() - time) < 600000){
    go_mm(180);
  }
}

void setup() {
  pinMode(14,OUTPUT);
  Serial.begin(115200);
  SetPinStep();
  SetupTof();
  SerialBT.begin(device_name);  
  while ( touchRead(T4) > 70){}
  tone(14,1000,100);
  ReadTof();
  target = Left;
}
//===========================================define running maze=================================
#include <cmath>
#include <iostream>
#include <string>

#define loopCost 20
#define travelCost 5
#define branchReward 5
void setWall(int dir, char relativeDir); // sets wall
void think();                            // movement logic (greed)
void move(char relativeDir);             // to move f, l, r
int maze[33][33];
int score[16][16];      // scores every square, initialised to 0
int history[16][16][3]; // stores last 3 steps of each point
int X=1, Y=1;
int preX=0, preY=0;
int x, y, facing;  // coordinates & direction faced
int moveCount = 1; // total number of moves made till now
bool inDeadEnd;    // checks if we are inside a deadend


/* sets the wall for the cell and adjacent cell */
void setWall(int dir, char relativeDir) {
    if (relativeDir == 'f')
        ; // do nothing
    else if (relativeDir == 'l')
        dir--; // rotate anticlockwise
    else if (relativeDir == 'r')
        dir++; // rotate clockwise

    dir = (dir + 4) % 4; // -1 --> 3 and 4 --> 0

    char absoluteDir = (dir == 0) ? 'n' : (dir == 1) ? 'e'
                                      : (dir == 2)   ? 's'
                                                     : 'w';
    // API::setWall(x, y, absoluteDir);
    // API::setColor(x,y,'R');
}


/* the main logic used for making decisions at squares */
void think() {
    bool wallFront, wallLeft,wallRight;
    ReadTof();
    if(Right<70){
      wallRight = true;
    }
    else {
      wallRight = false;
    }
    if(Front<70){
      wallFront = true;
    }
    else {
      wallFront= false;
    }
    if(Left<70){
      wallLeft = true;
    }
    else {
      wallLeft = false;
    }
    if (wallFront) setWall(facing, 'f');
    if (wallLeft) setWall(facing, 'l');
    if (wallRight) setWall(facing, 'r');

    int wallCount = wallFront + wallRight + wallLeft;

    history[x][y][0] = history[x][y][1]; // discard first value
    history[x][y][1] = history[x][y][2]; // and add current move
    history[x][y][2] = moveCount;        // to end of history

    // if the last 3 times we reached a square were at regular intervals, we are looping
    if (history[x][y][1] - history[x][y][0] == history[x][y][2] - history[x][y][1]) {
        // std::cerr << "looping" << std::endl;
        score[x][y] -= loopCost;
        // API::setText(x, y, std::to_string(score[x][y]));
    }

    if (wallCount == 3) { // dead end
        // std::cerr << "Help :(" << std::endl;
        inDeadEnd = true; // initiate protocol
        move('b');
    } else if (wallCount == 2) { // only one way to go
        if (!wallFront) {
            move('f');
        } else if (!wallLeft) {
            move('l');
        } else {
            move('r');
        }
    } else if (wallCount < 2) {
        inDeadEnd = false; // no longer in dead end if we are thinking

        int dir, u, v;
        int scoreF = -16383, scoreR = -16383, scoreL = -16383;
        if (!wallFront) {
            dir = facing; // dir is straight ahead
            u = (dir == 1) ? x + 1 : (dir == 3) ? x - 1
                                                : x; // take a step
            v = (dir == 0) ? y + 1 : (dir == 2) ? y - 1
                                                : y; // forwards
            scoreF = score[u][v];
        }
        if (!wallRight) {
            dir = (facing + 1) % 4; // dir is right
            u = (dir == 1) ? x + 1 : (dir == 3) ? x - 1
                                                : x; // take a step
            v = (dir == 0) ? y + 1 : (dir == 2) ? y - 1
                                                : y; // rightwards
            scoreR = score[u][v];
        }
        if (!wallLeft) {
            dir = (facing + 3) % 4; // dir is left
            u = (dir == 1) ? x + 1 : (dir == 3) ? x - 1
                                                : x; // take a step
            v = (dir == 0) ? y + 1 : (dir == 2) ? y - 1
                                                : y; // leftwards
            scoreL = score[u][v];
        }

        if (history[x][y][0] == 0 && history[x][y][1] == 0) {
            score[x][y] += branchReward * (2 - wallCount);
            // API::setText(x, y, std::to_string(score[x][y]));
        }

        if (scoreF > -8191 && scoreF >= scoreR && scoreF >= scoreL) {
            // std::cerr << "go ahead" << std::endl;
            move('f');
        } else if (scoreR > -8191 && scoreR >= scoreL) {
            // std::cerr << "take a right" << std::endl;
            move('r');
        } else if (scoreL > -8191) {
            // std::cerr << "take a left" << std::endl;
            move('l');
        }

        else if (scoreF > -8191) { // just move somewhere if good moves not allowed
            // std::cerr << "go ahead" << std::endl;
            move('f');
        } else if (scoreL > -8191) {
            // std::cerr << "take a left" << std::endl;
            move('l');
        } else if (scoreR > -8191) {
            // std::cerr << "take a right" << std::endl;
            move('r');
        } else {
            inDeadEnd = true; // no other ways left to go
            move('b');        // only way left to go
        }
    }
    // std::cerr << x << " "<< y << '\n';
}


/* moves jerry in the direction of relativeDir */
/* relativeDir is f/l/r/b for front/left/right/back */
void move(char relativeDir) {
    score[x][y] -= travelCost;
    // API::setText(x, y, std::to_string(score[x][y]));
    moveCount++;

    if (inDeadEnd) {
        score[x][y] = -16383; // mark as dead end
        // API::setText(x, y, "X");
    }

    if (relativeDir == 'b') { // turn around
        TurnRight();
        TurnRight();
        facing = (facing + 2) % 4;
    } else if (relativeDir == 'r') { // turn right
        TurnRight();
        facing = (facing + 1) % 4;
    } else if (relativeDir == 'l') { // turn left
        TurnLeft();
        facing = (facing + 3) % 4;
    }
	// API::setColor(x,y,'w');
    go_mm(180); // go ahead,
    x = (facing == 1) ? x + 1 : (facing == 3) ? x - 1
                                              : x; // adjust the
    y = (facing == 0) ? y + 1 : (facing == 2) ? y - 1
                                              : y; // coordinates
}


/* this is the main method */
int runMaze() {
	maze[1][1]=1;
    // std::cerr << "Running.!!!" << std::endl;
    // API::setColor(0, 0, 'R');
    // API::setText(0, 0, "start");

    for (int i = 0; i < 16; i++) { // initialise score array and display it
        for (int j = 0; j < 16; j++) {
            score[i][j] = 128 - (abs(2 * i - 15) + abs(2 * j - 15)) / 2;
            // API::setText(i, j, std::to_string(score[i][j]));
        }
    }

	while (!((x == 7 || x == 8) && (y == 7 || y == 8))) {
	    think();
	    if(x!= preX){
	    	if(x- preX ==1){
	    		X+=2;
	    		maze[X-1][Y]=1;
			}
			else if( preX - x == 1){
				X-=2;
				maze[X+1][Y] =1 ;
			}
		}
		if(y != preY){
			if( y - preY ==1){
				Y+=2;
				maze[X][Y-1]=1;
				}
			else if ( preY - y == 1){
				Y-=2;
				maze[X][Y+1]=1;
			}
		}
		maze[X][Y]=1;
		// for(int i=0 ; i<33 ; i++){
		// 	for(int j=0 ; j<33 ; j++){
		// 		std:: cerr << maze[i][j]<<" ";
		// 	}
		// 	std:: cerr << '\n';
		// }
		preX = x;
		preY = y;
	}
}
void loop() { 
  runMaze();
}
 
  
