/**************************************************************
color-recognition.ino
BlackBug Engineering
14.05.2017
https://github.com/dbprof/color-recognition
***************************************************************/

///////////////////////////////ДАТЧИК_ПРИСУТСТВИЯ///////////////////////////////
#define SENSOR_PIN 10

///////////////////////////////ДАТЧИК_ЦВЕТА///////////////////////////////
//https://github.com/sparkfun/APDS-9960_RGB_and_Gesture_Sensor
#include <Wire.h>
#include <SparkFun_APDS9960.h>
SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint16_t ambient_light = 0;
uint16_t red_light = 0;
uint16_t green_light = 0;
uint16_t blue_light = 0;

///////////////////////////////СЕРВОПРИВОД///////////////////////////////
// добавляем библиотеку для работы с сервоприводами
#include <Servo.h> 
// для дальнейшей работы назовем 9 пин как servoPin
#define VERT_SERVO_PIN 9
Servo VertServo;

///////////////////////////////ДВИГАТЕЛЬ///////////////////////////////
#define STEP_PIN 7
#define DIR_PIN 6
#define ENABLE_PIN 8
int iStepSpeed = 1000;
int iCurStepPos = 0;

// Для того чтобы избежать шума драйвера мотора используем функции подключения и отключения
bool isMotorAttached = false;
void attachMotor() {
  if (!isMotorAttached) {
    digitalWrite(ENABLE_PIN, LOW);
    delay(500);
    isMotorAttached = true;
  }
}
void detachMotor() {
  if (isMotorAttached) {
    digitalWrite(ENABLE_PIN, HIGH);
    delay(500);
    isMotorAttached = false;
  }
}

void setStepPos (int iPos)
{
  // Текущая позиция = 0, от нее отсчитываются шаги в + по час. стрелке
  //                                                в - против час. стрелки
  if (iPos == iCurStepPos) { return; }
  int iDiff = iPos - iCurStepPos;
  if (iDiff > 0)
  {
    //attachMotor();
    while(iCurStepPos < iPos)
    {
      digitalWrite(DIR_PIN,LOW);
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(iStepSpeed);          
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(iStepSpeed);
      iCurStepPos++;
    }
    //detachMotor();
  }
  else
  {
    //attachMotor();
    while(iCurStepPos > iPos)
    {
      digitalWrite(DIR_PIN,HIGH);
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(iStepSpeed);          
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(iStepSpeed);
      iCurStepPos--;
    }
    //detachMotor();
  }
}

///////////////////////////////ДАТЧИК_ЦВЕТА///////////////////////////////
int getHueFromRGB (int iR,int iG,int iB)
{
  int iMIN = min(min(iR,iG),iB);
  int iMAX = max(max(iR,iG),iB);
  int iHue;

  if (iMIN == iMAX)
  {
    iHue = 0;
  }
  else if (iMAX == iR && iG >= iB)
  {
    iHue = (60*(float)((float)(iG-iB)/(float)(iMAX-iMIN)))+0;
  }
  else if (iMAX == iR && iG < iB)
  {
    iHue = (60*(float)((float)(iG-iB)/(float)(iMAX-iMIN)))+360;
  }
  else if (iMAX == iG)
  {
    iHue = (60*(float)((float)(iB-iR)/(float)(iMAX-iMIN)))+120;
  }
  else if (iMAX == iB)
  {
    iHue = (60*(float)((float)(iR-iG)/(float)(iMAX-iMIN)))+240;
  }
  else
  {
    iHue = 0;
  }
  /*
  while (iHue >= 360)
  {
    iHue = iHue - 360;
  }
  */
  return iHue;
}

int getColorNumberFromRGB (int iR,int iG,int iB)
{
  // 1 - Черный                 
  // 2 - Белый                
  // 3 - Красный              
  // 4 - Зеленый
  // 5 - Синий
  // 6 - Голубой
  // 7 - Фиолетовый/Пурпурный
  // 8 - Желтый
  int iColorNumber;
  int iHue = getHueFromRGB(iR, iG, iB);
  if      (iHue >= 330 && iHue <= 360 || iHue >= 0 && iHue < 5)  
                                      { iColorNumber = 3; }
  else if (iHue >= 5 && iHue < 75)    { iColorNumber = 8; }
  else if (iHue >= 75 && iHue < 150)  { iColorNumber = 4; }
  else if (iHue >= 150 && iHue < 210) { iColorNumber = 6; }
  else if (iHue >= 210 && iHue < 270) { iColorNumber = 5; }
  else if (iHue >= 270 && iHue < 330) { iColorNumber = 7; }
  else                                { iColorNumber = 0; }
  return iColorNumber;
}

String getColorNameFromRGB (int iR,int iG,int iB)
{
  String strColorName;
  int iHue = getHueFromRGB(iR, iG, iB);
  if      (iHue >= 330 && iHue <= 360 || iHue >= 0 && iHue < 5)  
                                      { strColorName = "Red"; }
  else if (iHue >= 5 && iHue < 75)    { strColorName = "Yellow"; }
  else if (iHue >= 75 && iHue < 150)  { strColorName = "Green"; }
  else if (iHue >= 150 && iHue < 210) { strColorName = "Cyan"; }
  else if (iHue >= 210 && iHue < 270) { strColorName = "Blue"; }
  else if (iHue >= 270 && iHue < 330) { strColorName = "Violet"; }
  else                                { strColorName = "Not Defined"; }
  return strColorName;
}

///////////////////////////////СЕРВОПРИВОД///////////////////////////////
// Для того чтобы избежать шума сервов используем функции подключения и отключения
bool isServoAttached = false;
void attachServo() {
  if (!isServoAttached) {
    VertServo.attach(VERT_SERVO_PIN);
    isServoAttached = true;
  }
}
void detachServo() {
  if (isServoAttached) {
    VertServo.detach();
    isServoAttached = false;
  }
}
void closeGate() {
  attachServo();
  VertServo.write(120);
  delay(200);
  detachServo();
}
void openGate() {
  attachServo();
  VertServo.write(60);
  delay(200);
  detachServo();
}
void passOne() {
  openGate();
  closeGate();
}

void setup()
{
  ///////////////////////////////ДАТЧИК_ЦВЕТА///////////////////////////////
  // Initialize Serial port
  Serial.begin(9600);
  apds.init();
  apds.enableLightSensor(false);
  delay(500);
  
  ///////////////////////////////СЕРВОПРИВОД///////////////////////////////
  // Устанавливаем качалку сервопривода в положение - закрыто
  closeGate();

  ///////////////////////////////ДВИГАТЕЛЬ///////////////////////////////
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(DIR_PIN, LOW);
  digitalWrite(STEP_PIN, LOW);
}

void loop()
{
  if (!digitalRead(SENSOR_PIN)) 
  {
    Serial.println("No object");
    delay(1000);
    return;
  }
  ///////////////////////////////ДАТЧИК_ЦВЕТА///////////////////////////////
  // Read the light levels (ambient, red, green, blue)
  if (  !apds.readAmbientLight(ambient_light) ||
        !apds.readRedLight(red_light) ||
        !apds.readGreenLight(green_light) ||
        !apds.readBlueLight(blue_light) ) {
    return;
  }
  
  Serial.print("Ambient: ");
  Serial.print(ambient_light);
  Serial.print(" Red: ");
  Serial.print(red_light);
  Serial.print(" Green: ");
  Serial.print(green_light);
  Serial.print(" Blue: ");
  Serial.print(blue_light);
  Serial.print(" Hue: ");
  Serial.print(getHueFromRGB(red_light,green_light,blue_light));
  Serial.print(" ColorNumber: ");
  Serial.print(getColorNumberFromRGB(red_light,green_light,blue_light));
  Serial.print(" - ColorName: ");
  Serial.println(getColorNameFromRGB(red_light,green_light,blue_light));
  

  int iColorNumber = getColorNumberFromRGB(red_light,green_light,blue_light);

  // 7 - Если цвет Фиолетовый, то ничего не делаем 
  if (iColorNumber == 7)
  {
    delay(1000);
    return;
  }
  // 8 - Если цвет Желтый, то повернуть в положение -50
  else if (iColorNumber == 8)
  {
    setStepPos(-50);
    delay(1000);
    passOne();
    delay(3000);
  }
  // 4 - Если цвет Зеленый, то повернуть в положение -50
  else if (iColorNumber == 4)
  {
    setStepPos(-50);
    delay(1000);
    passOne();
    delay(3000);
  }
  // 3 - Если цвет Красный, то повернуть в положение 0
  else if (iColorNumber == 3)
  {
    setStepPos(0);
    delay(1000);
    passOne();
    delay(3000);
  }       
  // 6 - Если цвет Голубой, то повернуть в положение 50
  else if (iColorNumber == 6)
  {
    setStepPos(50);
    delay(1000);
    passOne();
    delay(3000);
  }
  // 5 - Если цвет Синий, то повернуть в положение 50
  else if (iColorNumber == 5)
  {
    setStepPos(50);
    delay(1000);
    passOne();
    delay(3000);
  }
}
