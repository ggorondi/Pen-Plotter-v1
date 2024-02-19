#include <ezButton.h>
#include <AccelStepper.h>
#include <MultiStepper.h>


// Define functions
double hipotenusa_mot1(double x, double y);
double hipotenusa_mot2(double x, double y);


// Define pins for A4988 motor drivers
const int dirPin = 2; // Direction
const int stepPin = 3; // Step
const int dirPin2 = 4; // Direction
const int stepPin2 = 5; // Step

// Define pins for joystick
#define joyX_Pin A0
#define joyY_Pin A1
const int joySW_Pin = 6;
ezButton button(joySW_Pin);

// Motor steps per rotation
const int real_STEPS_PER_REV = 200; // my nema 17 has 200 steps per rev
const int STEPS_PER_REV = real_STEPS_PER_REV*16; // sixteenth steps

// Measurements of the canvas
#define TOTAL_WIDTH 143 // Horizontal distance between motors (cm)
#define START_Y 82.3 // Vertical distance from motors to pen (cm)
#define EACHSTEP 0.2
#define CM_TO_STEPS 1000
double startX=(TOTAL_WIDTH/2);
double startY=START_Y;
double currX=startX;
double currY=startY;
double startMot1=hipotenusa_mot1(startX,startY);
double startMot2=hipotenusa_mot2(startX,startY);
double curr_mot1=startMot1;
double curr_mot2=startMot2;

// Accelsteppers
AccelStepper motor1(AccelStepper::DRIVER, stepPin, dirPin); //AccelStepper::DRIVER==1 Is the syntax for using the A4988 driver
AccelStepper motor2(AccelStepper::DRIVER, stepPin2, dirPin2);
const int MAX_SPEED = 1000; // 1000 is the max which works well in theory. 700 with sixteenth steps works great.
#define FULL 1 // LLL
#define HALF 2 // HLL
#define QUARTER 4 // LHL
#define EIGHTH 8 // HHL
#define SIXTEENTH 16 // HHH
const double MICROSTEP = SIXTEENTH;

// MultiStepper object
MultiStepper motores;

// Define some variables
int varSize=100;
int rand1;
int rand2;
int xJoy = 0; // Stores X axis value
int yJoy = 0; // Stores Y axis value
int bJoy = 0; // Stores button value
int speedVal;
int sizeX;
int sizeY;

void setup() {
  // Setup the driver pins as Outputs
  pinMode(stepPin,OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin2,OUTPUT);
  pinMode(dirPin2, OUTPUT);

  Serial.begin(9600);
  button.setDebounceTime(50); // set debounce time to 50 milliseconds
  randomSeed(0);

  // Accelstepper setup
  motor1.setCurrentPosition(0);
  motor1.setMaxSpeed(MAX_SPEED);
  delay(500);
  motor2.setCurrentPosition(0);
  motor2.setMaxSpeed(MAX_SPEED);

  // Add the steppers to the MultiStepper
  motores.addStepper(motor1); //right
  motores.addStepper(motor2); //left
  
  //wait 2 sec
  delay(10000);
}


void loop() {
  canvas_sketch_lineas();
}


void read_joystick() {
  button.loop(); // MUST call the loop() function first
  // read Joystick
  yJoy = analogRead(joyX_Pin); //Vertical is Xpin
  xJoy = analogRead(joyY_Pin);
  bJoy = button.getState();
}

void print_joystick() {
  // print joystick data to Serial Monitor on Arduino IDE
  Serial.print("x = ");
  Serial.print(xJoy);
  Serial.print(", y = ");
  Serial.print(yJoy);
  Serial.print(" : button = ");
  Serial.println(bJoy);
}

// Function to move both motors to draw a straight line
void draw_line(int x, int y) {
  
  long steps1 = x;
  long steps2 = y;

  float speed1 = steps1 > 0 ? MAX_SPEED : -MAX_SPEED;
  float speed2 = steps2 > 0 ? MAX_SPEED : -MAX_SPEED;
  
  // Adjust the speeds based on the ratio between steps
  float ratio = (float)abs(steps1) / (float)abs(steps2);
  if (ratio > 1.0) {
    speed2 /= ratio;
  } else {
    speed1 *= ratio;
  }
  
  // Move both motors to the desired positions with adjusted speeds
  motor1.moveTo(steps1);
  //motor1.setSpeed(speed1);
  motor2.moveTo(steps2);
  //motor2.setSpeed(speed2);
  
  // Keep moving until both motors reach their target positions
  while (motor1.isRunning() || motor2.isRunning()) {
    // Serial.println(motor1.currentPosition());
    motor1.run();
    motor2.run();
  }
}

// Function to move both motors to desired positions
void move_motors(int target1, int target2) {
  
  long targetPositions[] = {target1, target2};
  motores.moveTo(targetPositions);

  while (motores.run()) {
    // You can add additional code here to run concurrently with the motor movements
  }
}

void reset_positions(){
  motor1.setCurrentPosition(0);
  motor2.setCurrentPosition(0);
}

void move_motors_with_joystick() {
  
  readJoystick();
  sizeX = map(xJoy, 0, 1023, -50, 50); 
  sizeY = map(yJoy, 0, 1023, 50, -50);
  printJoystick();
  if(!bJoy) {
    moveMotors(0,0);
    delay(100);
  }
  //move
  if((abs(sizeX)+abs(sizeY))>4){ // Noise gate
    moveMotors(sizeX+motor1.currentPosition(), sizeY+motor2.currentPosition());
  }
  else {delay(10);}
}

double hipotenusa_mot1(double x, double y) { //left
  return sqrt(x*x+y*y);
}

double hipotenusa_mot2(double x, double y) { //right
  return sqrt((TOTAL_WIDTH-x)*(TOTAL_WIDTH-x)+y*y); 
}

double cm_to_steps(double cm) {
  return cm*50*MICROSTEP;
}
double steps_to_cm(double steps) {
  return steps*0.02/MICROSTEP;
}

void move_xy_abs_from_start(double x,double y) {
  //entran coordenadas en cm adonde se quiere mover, tomando (0,0) como el punto donde empieza el lapiz.
  double xx= startX + x; // Transforms into coordinate from upper left corner (if x=0, xx=startX)(positive values are above 0, adding goes right)
  double yy= startY - y; // Transforms into coordinate from upper left corner (if y=0, yy=startY)(positive value is up) (Example: x=10,y=10 does diagonal up-right from start point)
  move_xy_abs_from_upleft_corner(xx,yy);
  return;
}

void move_xy_abs_from_start_en_trozos(double x,double y) {
  // Same but splits movement into smaller steps
  double xx;
  double yy;
  for(double i=1;i<51;i++){
    xx= startX + x*i/50; 
    yy= startY - y*i/50;
    move_xy_abs_from_upleft_corner(xx,yy);
  }
  return;
}

void move_xy_abs_from_upleft_corner(double xx,double yy) {
  // Takes the coordinates in cm where you want to move the pen, where (0,0) is the left motor
  double largo_mot1=hipotenusa_mot1(xx,yy); //Transforms into motor coordinate (in cm)
  double largo_mot2=hipotenusa_mot2(xx,yy); 
  double mot1 = -(startMot1 -largo_mot1); // Makes it relative to starting pos
  double mot2 = startMot2 - largo_mot2;
  moveMotors(cm_to_steps(mot1), cm_to_steps(mot2));
  return;
}

void draw_rect_and_return_to_zero(double x, double y, double width, double height) {
  move_xy_abs_from_start(x,y);
  move_xy_abs_from_start(x,y+height);
  move_xy_abs_from_start(x+width,y+height);
  move_xy_abs_from_start(x+width,y);
  move_xy_abs_from_start(x,y);
  move_xy_abs_from_start(0,0);
  return;
}

void draw_rect_and_dont_return_to_zero(double x, double y, double width, double height) {
  move_xy_abs_from_start(x,y);
  move_xy_abs_from_start(x,y+height);
  move_xy_abs_from_start(x+width,y+height);
  move_xy_abs_from_start(x+width,y);
  move_xy_abs_from_start(x,y);
  return;
}

void grid_of_rects_for_calibration(double canvasWidth, double canvasHeight) {
  for(double i=0;i<10;i++){
    for(double j=0;j<10;j++){
      //
      read_joystick();
      if(!bJoy) { move_motors(0,0); delay(10000);}
      //
      draw_rect_and_dont_return_to_zero(i*canvasWidth/10-canvasWidth/2, j*canvasHeight/10-canvasHeight/2, canvasWidth/11, canvasHeight/11);
    }
  }
  move_xy_abs_from_start(0,0);
  return;
}

void square_for_calibration(double canvasWidth, double canvasHeight) {
  //
  read_joystick();
  if(!bJoy) { move_motors(0,0); delay(10000);}
  //
  draw_rect_and_return_to_zero(-canvasWidth/2, -canvasHeight/2, canvasWidth, canvasHeight); //ahora restar es ir hacia izq y abajo
  return;
}

double formula_x(double width,double height,double i,double totaliter) {
  return width/2.0/2.0 * sin(i/9.507)+width/10*cos(i/19.01);
}

double formula_y(double width,double height,double i,double totaliter) {
  return height/2.0/2.0 * cos(i/19.1)+height/10*cos(i/19.01)+height/10*cos(i/19.0);
}

double sigmoid(double x) {
  return 1.0/(1.0 + exp(-x));
}

void canvas_sketch_lineas(){
  // Draws straight lines from point to point, depending on the points assigned by formula_x and formula_y
  double X;
  double Y;
  double width=21; // Canvas width aprox en cm
  double height=30;// Canvas height aprox en cm
  //(A3 aprox 30,45 , A4 aprox 21,30 )

  double totaliter=30000;
  X=formula_x(width,height,0,totaliter);
  Y=formula_y(width,height,0,totaliter);
  move_xy_abs_from_start(X,Y);
  delay(10000);
  for(double i=1;i<totaliter;i++) {
    //
    read_joystick();
    if(!bJoy) { move_motors(0,0); delay(10000);}
    //
    X=formula_x(width,height,i,totaliter);
    Y=formula_y(width,height,i,totaliter);
    //context.rotate(i*0.01);
    Serial.print("X: ");  Serial.print(X, 2);  Serial.print(" Y: "); Serial.print(Y, 2);  Serial.print(" i: "); Serial.println(i, 2);
    move_xy_abs_from_start(X,Y);
  }
  while(true){
    delay(1000);
    read_joystick();
    if(!bJoy) {move_motors(0,0); break;}
  };
  while(true){/*stop after finished*/};
}