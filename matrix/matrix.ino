
#include "HID-Project.h"

byte rows[] = {2,3,4,5};
byte cols[] = {6,7,8,9};

#define STATUS_OFF 0
#define STATUS_PRESSED 1
#define STATUS_HELD 2
#define STATUS_RELEASED 3

byte status[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
byte fsm[4][2] = {{STATUS_OFF, STATUS_PRESSED}, {STATUS_OFF, STATUS_HELD},{STATUS_RELEASED, STATUS_HELD},{STATUS_OFF, STATUS_PRESSED}};

int counter[4][4] = {{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1}};

void setup() {
  Serial.begin(115200);
  Consumer.begin();
 
    for(int x=0; x<4; x++) {
        Serial.print(rows[x]); Serial.println(" as input");
        pinMode(rows[x], INPUT);
    }
 
    for (int x=0; x<4; x++) {
        Serial.print(cols[x]); Serial.println(" as input-pullup");
        pinMode(cols[x], INPUT_PULLUP);
    }
}

void loop() {
  for (int colIndex=0; colIndex < 4; colIndex++) {
        // col: set to output to low
        byte curCol = cols[colIndex];
        pinMode(curCol, OUTPUT);
        digitalWrite(curCol, LOW);
 
        // row: interate through the rows
        for (int rowIndex=0; rowIndex < 4; rowIndex++) {
            byte rowCol = rows[rowIndex];
            pinMode(rowCol, INPUT_PULLUP);
            byte pressed = digitalRead(rowCol) == LOW ? 1 : 0;
            byte curStat = status[colIndex][rowIndex];
            status[colIndex][rowIndex] = fsm[curStat][pressed];
            pinMode(rowCol, INPUT);
        }
        // disable the column
        pinMode(curCol, INPUT);
    }

    for(byte col = 0; col < 4; col++) {
      for(byte row = 0; row < 4; row++) {
        if (status[col][row] == STATUS_HELD) {
          counter[col][row]++;
          counter[col][row]%=4;
        }
        if (status[col][row] == STATUS_RELEASED || status[col][row] == STATUS_OFF) {
          counter[col][row] = -1;
        }
        //Serial.print(status[col][row]);
      }
    }
    /*
    Serial.print(" -> ");
    for(byte col = 0; col < 4; col++) {
      for(byte row = 0; row < 4; row++) {
        Serial.print(counter[col][row] + 1);
      }
    }
    Serial.println();
    */

    if(counter[0][0] == 0){
      Consumer.write(MEDIA_VOLUME_DOWN);
    }
    if(counter[0][2] == 0){
      Consumer.write(MEDIA_VOLUME_UP);
    }
    if(status[0][1] == STATUS_PRESSED){
      Consumer.write(MEDIA_VOLUME_MUTE);
    }
    if(status[1][0] == STATUS_PRESSED){
      Consumer.write(MEDIA_PREVIOUS);
    }
    if(status[1][1] == STATUS_PRESSED){
      Consumer.write(MEDIA_PLAY_PAUSE);
    }
    if(status[1][2] == STATUS_PRESSED){
      Consumer.write(MEDIA_NEXT);
    }

    delay(50);
}
