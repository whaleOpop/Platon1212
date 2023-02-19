#include <DynamixelWorkbench.h>
#define PortName ""
int BaudRate = 1000000;
int DXL_ID = 1;
int DXR_ID = 2;
DynamixelWorkbench DXL;
DynamixelWorkbench DXR;
int32_t spdL = 200;
int32_t spdR = 200;
int32_t sspdL = spdL;
int32_t sspdR = spdR;

int buzzer_pin = 31;
int fso = 15; // ФСО
int light = 14; // СВЕТ
unsigned long timer;
int tmp = 0;
uint32_t tempL; // Температура левого двигателя
uint32_t tempR; // Температура правого двигателя
bool fso_flag = false;
bool light_flag = false;

bool Map = false;
unsigned long Millis;
int timerStep = 0;
struct Struct{
  int LeftSpeed;
  int RightSpeed;
  int Time;
};
int listCnt = 0;
Struct list[30];

void startAlarm() {
  tone(buzzer_pin, 554, 200);
  delay(200);
  tone(buzzer_pin, 330, 200);
  delay(200);
  tone(buzzer_pin, 440, 200);
  delay(200);
}
bool protection = false;
void protectAlarm() {
  
   
  timer=millis()+10000;
  if (timer - tmp > 10000){
    DXL.readRegister(DXL_ID, uint16_t(43), uint16_t(1), &tempL);
    DXR.readRegister(DXR_ID, uint16_t(43), uint16_t(1), &tempR);
    
    Serial1.print ("Voltage = ");
    Serial1.print(getPowerInVoltage());
    Serial.print ("Voltage = ");
    Serial.print(getPowerInVoltage());
    Serial.print (" | ");
    Serial1.print (" | ");
    Serial1.print ("Temperature = ");
    Serial1.println(tempL >= tempR ? tempL : tempR);
    Serial.print ("Temperature = ");
    Serial.println(tempL >= tempR ? tempL : tempR);
    
    tmp = millis()+10000;
      if (getPowerInVoltage() <= 10.6){
        protection = true;
        Serial1.println("CRITICAL VOLTAGE!!!");
        Serial.println("CRITICAL VOLTAGE!!!");
        digitalWrite(23, LOW);
        tone(buzzer_pin, 554, 200);
        delay(200);
        tone(buzzer_pin, 330, 200);
        delay(200);  
    }else{digitalWrite(23, HIGH);}
  }
}



void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.setTimeout(5);
  Serial1.setTimeout(5);
  DXL.init(PortName, BaudRate);
  DXR.init(PortName, BaudRate);
  DXL.ping(DXL_ID);
  DXR.ping(DXR_ID);
  DXL.wheelMode(DXL_ID, 0);
  DXR.wheelMode(DXR_ID, 0);
  DXL.goalVelocity(DXL_ID, (int32_t) 0);
  DXR.goalVelocity(DXR_ID, (int32_t) 0);

  pinMode(fso, OUTPUT);
  pinMode(light, OUTPUT);

  startAlarm();
  
}

void loop() {
  protectAlarm();
   
  char buf[20] = {'1','1','1','1','1','1','1','1','1','1','1','1','1','1'};
  if (Serial1.available()){
    //int none = Serial1.readBytesUntil('', buf, 14);
    Serial1.readBytesUntil('\n', buf, 20);
    //buf[none] = NULL;
    Serial.println(buf); 
   
   }
   
    if (String(buf) == "forwardStart11"){  //forwardStart
        sspdL = spdL;
        sspdR = -spdR;
        Millis = millis();
        DXL.goalVelocity(DXL_ID, spdL);
        DXR.goalVelocity(DXR_ID, -spdR);}
    if (String(buf) == "backwardStart1"){ //backwardStart
        sspdL = -spdL;
        sspdR = spdR;
        Millis = millis();
        DXL.goalVelocity(DXL_ID, -spdL);
        DXR.goalVelocity(DXR_ID, spdR);}
    if (String(buf) == "leftStart11111"){ //leftStart
        sspdL = -spdL;
        sspdR = -spdR;
        Millis = millis();
        DXL.goalVelocity(DXL_ID, -spdL);
        DXR.goalVelocity(DXR_ID, -spdR);}
    if (String(buf) == "rightStart1111"){ //rightStart
        sspdL = spdL;
        sspdR = spdR;
        Millis = millis();
        DXL.goalVelocity(DXL_ID, spdL);
        DXR.goalVelocity(DXR_ID, spdR);}   
     
    if ((String(buf) == "lookLeftStart1")&&(spdL <= 800)){ //Up
        spdL+=100;
        spdR+=100;
        Serial1.print ("Speed = ");
        Serial1.println(spdL);
        Serial.print ("Speed = ");
        Serial.println(spdL);}
    if ((String(buf) == "lookRightStart")&&(spdL >=200)){ //Down
        spdL-=100;
        spdR-=100;
        Serial1.print ("Speed = ");
        Serial1.println(spdL);
        Serial.print ("Speed = ");
        Serial.println(spdL);}

    if (String(buf) == "aStart11111111"){
        if (fso_flag == false){
          digitalWrite(15, HIGH);
          fso_flag = true;
        }
        else if (fso_flag == true){
          digitalWrite(15, LOW);
          fso_flag = false;
        }
     }
    if (String(buf) == "bStart11111111"){
        if (light_flag == false){
          digitalWrite(14, HIGH);
          light_flag = true;
        }
        else if (light_flag == true){
          digitalWrite(14, LOW);
          light_flag = false;
        }
     }
     
    //look(Left/Right)(Start/Stop)
      
    if ((String(buf) == "forwardStop111")||(String(buf) == "backwardStop11")||(String(buf) == "leftStop111111")||(String(buf) == "rightStop11111")){
        Serial.println("STOP");
        DXL.goalVelocity(DXL_ID, 0);
        DXR.goalVelocity(DXR_ID, 0); 
        timerStep = millis() - Millis;
        //Serial1.print("Time =  "); выберите serial
        //Serial1.println(timerStep);

        if ((Map == true)&&(listCnt < 30)){
          list[listCnt].LeftSpeed = -sspdL;
          list[listCnt].RightSpeed = -sspdR;
          list[listCnt].Time = timerStep;
          listCnt++;}
    }
   


    if (String(buf) == "cStart11111111"){
      Map = !Map;
      tone(buzzer_pin, 330, 200);
      delay(200);
      tone(buzzer_pin, 554, 200);
      delay(200);
      if (Map == false){
        tone(buzzer_pin, 440, 200);
        delay(200);
        int listCnt = 0;
      }
    }
    
    if ((String(buf) == "dStart11111111")&&(Map)&&(listCnt >= 1)){
      Map = false;
      tone(buzzer_pin, 330, 200);
      delay(200);
      tone(buzzer_pin, 554, 200);
      delay(200);
      tone(buzzer_pin, 440, 200);
      delay(200);

      for(int i = 29;i>-1;i--){
        if (list[i].LeftSpeed != 0){
          /*Serial1.print(list[i].LeftSpeed);
          Serial1.print("  ");
          Serial1.print(list[i].RightSpeed);
          Serial1.print("  ");
          Serial1.println(list[i].Time);*/
          Millis = millis();
          while(millis() - Millis < list[i].Time){
            DXL.goalVelocity(DXL_ID, list[i].LeftSpeed);
            DXR.goalVelocity(DXR_ID, list[i].RightSpeed);   
          }
          
          
        }
      }
      Struct list[30];
      listCnt = 0;
      DXL.goalVelocity(DXL_ID, 0);
      DXR.goalVelocity(DXR_ID, 0);
    }
}
