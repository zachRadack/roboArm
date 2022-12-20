// normal base



#include <Servo.h>

//Servo
// Left Arm can also be called "Main Joint"
Servo leftArmServo;
// Right Arm can be also called "Minor Joint"
Servo rightArmServo;
Servo baseServo;

Servo gripperServo;

const int leftArmServoInput = 11;
const int rightArmServoInput = 6;
const int baseServoInput = 13;
const int gripperServoInput = 10;
const int yError = 1; 
const int xError = 1; 

const int yError1 = 0; 
const int xError1 = 1; 

int rotationPos = 90; 
int liftPos = 90; 


float ArmLength = 81;


// Joystick
int VRx = A5;
int VRy = A4;
int SW = 8;
int VRx1 = A3;
int VRy1 = A2;
int SW1 = 9;


int xPosition = 0;
int yPosition = 0;
int xPosition1 = 0;
int yPosition1 = 0;
int SW_state = 0;
int SW_state1 = 0;
int mapX = 0;
int mapY = 0;
int mapX1 = 0;
int mapY1 = 0;




// Global variables for storing the position to move to
int moveToTheta     = 90;
int moveToR         = 88;
int moveToZ         = 22;
int moveToGripper   = 40;




void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  leftArmServo.attach(leftArmServoInput);
  rightArmServo.attach(rightArmServoInput);
  baseServo.attach(baseServoInput);
  gripperServo.attach(gripperServoInput);
  Serial.println("starting");
  

  // starting arm position
  //delay(500);
  moveRZ();
  //leftArmServo.write(48.0);
  //delay(500);
  //rightArmServo.write(67.0);
  //delay(500);
  moveTheta();
  //baseServo.write(90.0);
  //delay(500);
  moveGripperTheta();
  call_movement();
  //delay(50);
  

  // joystick initiation
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP); 
  pinMode(VRx1, INPUT);
  pinMode(VRy1, INPUT);
  pinMode(SW1, INPUT_PULLUP); 



}

void loop() {
  // put your main code here, to run repeatedly:
  //liftArm(1.0);

//for(moveToZ=-5;moveToZ<=16;moveToZ++){
  //if (true){

  if (doSerialConsole()){
    MoveEntire();
    }
} 


void MoveEntire(){

  moveRZ();
  moveTheta();
  moveGripperTheta();
  //delay(50);
}




//ARDUNIO COMMAND MOVEMENT CODE
// FEELING CUTE, MIGHT DELETE LATE

// Move the rotational (theta, polar coordinates) axis of the MeArm
void moveTheta() {
  baseServo.write(moveToTheta);
}

// Open the gripper a given width/distance
void moveGripperTheta() {
  gripperServo.write(moveToGripper);
}

// Move the arm along the r axis (polar coordinates), or in height (z)

void moveRZ() {
  float c = sideC();

  float K = angleK(c);
  float B = angleB(c);
  float C= angleC(B);

  float X = (90 - (K+B));

  float Y = (90-X);
  double W= 180-C-Y;


  //Serial.print("trig c: ");
  //Serial.println(c);
  //Serial.print("trig K: ");
  //Serial.println(K);`
  //Serial.print("trig C: ");
  //Serial.println(C);
  //Serial.print("ANGLE C: ");
  //Serial.println(C);
  //Serial.print("Y: ");
  //Serial.println(Y);

  //X = X+10;
  //W= W+40;

  int newX = 50+X;
  int newW = 100-W;


  // if angles decide death is preferable to cooperating, then this checks them back to reality
  if ((newX <=10)||(newW<=10)){
    WIPEOUT();
  }

  

  // if the pos of where the gripper to be is below  or above a resonable certain point, this stops it 
  if (moveToR>160){
    moveToR = 160;
  } else if (moveToR<30){
    moveToR = 30;
  }
  

  if (moveToZ>65){
    moveToZ = 65;
  } else if (moveToZ<-40){
    moveToZ = -40;
  }
  
  
  

  Serial.print(" | MainJoint: ");
  Serial.print(newX);
  Serial.print(" | MinorJoint: ");
  Serial.print(newW);
  Serial.print(" | R: ");
  Serial.print(moveToR);
  Serial.print(" | Z: ");
  Serial.print(moveToZ);

  rightArmServo.write(newX);
  leftArmServo.write(newW);
}

float sideC(){
  float c = sqrt(sq(moveToR)+sq(moveToZ));
  return c;
}

float angleK(float c){
  float K = asin(moveToZ/c)*(180/PI);
  
  return K;
}

float angleB(float c){
  float B= acos((sq(ArmLength)+sq(c)-sq(ArmLength))/(2*ArmLength*c))*(180/PI);
  

  return B;

}

float angleC(float B){
  float C= 180-(2*B);
  

  return C;

}

bool doSerialConsole() {
  // Display serial console 
  char inputStr[80];
  int index = 0;
  
  while (1) {
    int done = 0;
    // Step 1: Display serial console message
    Serial.println("");
    Serial.println("Enter coordinates: theta,r,z,gripper (comma delimited, no spaces)");
    Serial.println("Example: 10,20,30,40");

    // Step 2: Clear string
    for (int i=0; i<80; i++) {
      inputStr[i] = 0;
    }
    index = 0;

    // Step 3: Read serial data
    while (done == 0) {    
      joystick_loop();
      //Serial.print("yaaa");
      // Step 3A: Read serial data and add to input string, if available
      while(Serial.available() > 0) {
        char inputChar = Serial.read();        
        
        if (inputChar == '\n') {          
          // Newline character -- user has pressed enter
          done = 1;                
        } else {
          // Regular character -- add to string
          if (index < 79) {
            inputStr[index] = inputChar;                  
            index += 1;
          }
        }      
        
      }      
    }

    // Step 4: Debug output: Let the user know what they've input
    Serial.print ("Recieved input: ");
    Serial.println(inputStr);
    // if ((inputStr == "joy")|| (inputStr == "joystick"))){
    //   joystick_Mode = true;
    // }
    // Step 5: Check if string is valid
    if (sscanf(inputStr, "%d,%d,%d,%d", &moveToTheta, &moveToR, &moveToZ, &moveToGripper) == 4) {  
      Serial.println("Valid input!");  
      // Valid string
      return true;
    } else {
      // Invalid string -- restart
      Serial.println ("Invalid input -- try again.  example: 10,20,30,40  or joystick");     
      return false; 
    }      
  }

}

////////////////////////////////////////////////////////////////////////////////////////////////
//Joystick
void joystick_loop(){
    mapX = 0;
    mapY = 0;
    mapX1 = 0;
    mapY1 = 0;



  //JOYSTICK
  // gets the input output of the arms
  arms();

  //moves the base and gripper

  basegrip();

  call_movement();

  //delay(50);


}



void arms(){
  // this controls main and side joint
  xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  SW_state = digitalRead(SW);
  mapX = map(xPosition, 0, 1023, -10, 10)+xError;
  mapY = map(yPosition, 0, 1023, -10, 10)+yError;


  if(mapX >= -1 && mapX <= 1){
    mapX = 0;
  }
  if(mapY >= -1 && mapY <= 1){
    mapY = 0;
  }
  
  Serial.print("mapX: ");
  Serial.print(mapX);
  Serial.print(" | mapY: ");
  Serial.print(mapY);
  Serial.print(" | SW: ");
  Serial.print(SW_state);

  
}

void basegrip(){
  xPosition1 = analogRead(VRx1);
  yPosition1 = analogRead(VRy1);
  SW_state1 = digitalRead(SW1);
  mapX1 = map(xPosition1, 0, 1023, -10, 10)+xError1;
  mapY1 = (map(yPosition1, 0, 1023, -10, 10)+yError1)*3;

 if(mapX1 >= -2 && mapX1 <= 2){
    mapX1 = 0;
  }


  if (moveToTheta+mapX1>130){
      mapX1 = 0;
      moveToTheta = 130;
  }
  if (moveToTheta+mapX1<45){
      mapX1 = 0;
      moveToTheta = 45;
  }
  // Gripper override
  if(mapY1 >= -3 && mapY1 <= 3){
    mapY1 = 0;
  }

  if (moveToGripper+mapY1>60){
      mapY1 = 0;
      moveToGripper = 60;
  }
  if (moveToGripper+mapY1<10){
      mapY1 = 0;
      moveToGripper = 11;
  }
}





void call_movement(){ 



  if (abs(mapX)>=abs(mapY)){

    mapY= 0;
  }else{
    mapX= 0;
  }

  moveToR = moveToR+mapX;
  moveToZ = moveToZ+mapY;  
  
  

  moveRZ();
  
  if (abs(mapX1)>abs(mapY1)){

    mapY1= 0;
  }else{
    mapX1= 0;
  }
  moveToTheta = moveToTheta+mapX1;
  moveToGripper = moveToGripper+mapY1;
  
  moveTheta();
  moveGripperTheta();

  Serial.print("| Base: ");
  Serial.print(moveToTheta);
  Serial.print(" | Gripper: ");
  Serial.print(moveToGripper);
  Serial.print("| mapX1: ");
  Serial.print(mapX1);
  Serial.print(" | mapY1: ");
  Serial.print(mapY1);
  Serial.print(" | SW1: ");
  Serial.print(SW_state1);
  
  Serial.println(" ");

  if(SW_state ==0){

    WIPEOUT();
  }
}


void WIPEOUT(){

  Serial.println("WIPE");
    moveToR = 80;
    moveToZ = 40;
    moveToTheta = 90;
    moveToGripper = 40;

}