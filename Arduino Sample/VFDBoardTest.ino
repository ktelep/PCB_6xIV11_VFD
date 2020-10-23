int data_pin = 4;
int latch_pin = 5;
int clock_pin = 6;

// Segs      abcdefgh-
int numbers[] = {
  B00000011, //0
  B10011111, //1
  B00100101, //2
  B00001101, //3
  B10011001, //4
  B01001001, //5
  B01000001, //6
  B00011111, //7
  B00000001, //8
  B00001001, //9
  B00010001, //a
  B11000001, //b
  B01100011, //c
  B10000101, //d
  B01100001, //e
  B01110001  //f
};

int loopy_three[] = {
  B00011111,
  B10001111,
  B11000111,
  B11100011,
  B01110011,
  B00111011
};

int loopy_two[] = {
  B00111111,
  B10011111,
  B11001111,
  B11100111,
  B11110011,
  B01111011
};

int loopy_one[] = {
  B01111111,
  B10111111,
  B11011111,
  B11101111,
  B11110111,
  B11111011
};

void clearDisplay() {
  digitalWrite(clock_pin, LOW);
  digitalWrite(latch_pin, LOW);
  for (int i = 0; i < 6; i++) {
    shiftOut(data_pin, clock_pin, LSBFIRST, B11111111);
  }
  digitalWrite(latch_pin, HIGH);
}

void testDisplay() {
  digitalWrite(clock_pin, LOW);
  digitalWrite(latch_pin, LOW);
  for (int i = 0; i < 6; i++) {
    shiftOut(data_pin, clock_pin, LSBFIRST, B00000000);
  }
  digitalWrite(latch_pin, HIGH);
}

void sendDisplay(int mess[]) {
  digitalWrite(clock_pin, LOW);
  digitalWrite(latch_pin, LOW);
  for (int i = 0; i < 6; i++) {
    shiftOut(data_pin, clock_pin, LSBFIRST, mess[i]);
  }
  digitalWrite(latch_pin, HIGH);
}

void circleAnim(int rotations) {
  int frames[16][6] = {0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF,
                       0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF,
                       0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF,
                       0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9F,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCF,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xEF,
                       0xFF, 0xFF, 0xFF, 0xEF, 0xEF, 0xFF,
                       0xFF, 0xFF, 0xEF, 0xEF, 0xFF, 0xFF,
                       0xFF, 0xEF, 0xEF, 0xFF, 0xFF, 0xFF,
                       0xEF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xF3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0x7B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                      };

  for (int i = 0; i < rotations; i++) {
    for (int curFrame = 0; curFrame < 16; curFrame++) {
      sendDisplay(frames[curFrame]);
      delay(50);
    }
  }
}

void snakeAnim(int rotations) {
  int pat[5] = {0xF3, 0x7B, 0x3F, 0x9F, 0xDF};
  int revpat[5] = {0x9F, 0xCF, 0xE7, 0xF3, 0xFB};
  int curFrame[6];
  bool nodelay = false;  // We use this to disable the delay at the end of the displays so we don't double show the same pattern

  for (int rot = 0; rot < rotations; rot++) {
    for (int actTube = 0; actTube < 6; actTube++) {
      for (int curPattern = 0; curPattern < 5; curPattern++) {
        if (curPattern > 0) {
          nodelay = false;
        }
        for (int i = 0; i < 6; i++) { // Blank the entire Frame
          curFrame[i] = 0xFF;
        }

        curFrame[actTube] = pat[curPattern];  // Set the current tube pattern
        if (curPattern == 4) {  // If we're on the last pattern, we set the NEXT tube to 0xF7
          if (actTube < 5)  {  // Unless it's the last tube
            curFrame[actTube + 1] = 0xF7;
          } else {
            nodelay = true;
            break;
          }
        }
        sendDisplay(curFrame);
        if (nodelay == false) {
          delay(50);
        }
      }

    }

    for (int actTube = 5; actTube >= 0; actTube--) {
      for (int curPattern = 0; curPattern < 5; curPattern++) {
        if (curPattern > 0) {
          nodelay = false;
        }
        for (int i = 0; i < 6; i++) { // Blank the entire Frame
          curFrame[i] = 0xFF;
        }

        curFrame[actTube] = revpat[curPattern];  // Set the current tube pattern
        if (curPattern == 4) {  // If we're on the last pattern, we set the NEXT tube to 0xBF
          if (actTube > 0)  {  // Unless it's the first tube
            curFrame[actTube - 1] = 0xBF;
          } else {
            nodelay = true;
            break;
          }
        }
        sendDisplay(curFrame);
        if (nodelay == false) {
          delay(50);
        }
      }
    }
  }
}

void bigeightAnim(int rotations) {
  int frames[28][6] = {0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF,
                       0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF,
                       0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF,
                       0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBD,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFD,
                       0xFF, 0xFF, 0xFF, 0xFD, 0xFD, 0xFF,
                       0xFF, 0xFF, 0xFD, 0xFD, 0xFF, 0xFF,
                       0xFF, 0xFD, 0xFD, 0xFF, 0xFF, 0xFF,
                       0xFD, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xF5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xEF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xFF, 0xEF, 0xEF, 0xFF, 0xFF, 0xFF,
                       0xFF, 0xFF, 0xEF, 0xEF, 0xFF, 0xFF,
                       0xFF, 0xFF, 0xFF, 0xEF, 0xEF, 0xFF,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xEF,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCF,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDD,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFD,
                       0xFF, 0xFF, 0xFF, 0xFD, 0xFD, 0xFF,
                       0xFF, 0xFF, 0xFD, 0xFD, 0xFF, 0xFF,
                       0xFF, 0xFD, 0xFD, 0xFF, 0xFF, 0xFF,
                       0xFD, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0x7B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
                      };

  for (int i = 0; i < rotations; i++) {
    for (int curFrame = 0; curFrame < 28; curFrame++) {
      sendDisplay(frames[curFrame]);
      delay(50);
    }
  }

}

void wipeAnim(int rotations) {

  int curFrame[6];

  for (int rot = 0; rot < rotations; rot++) {
    int actTube = 0;

    while (actTube < 6) {

      int pat[3] = {0xF3, 0x61, 0x00};

      for (int actPat = 0; actPat < 3; actPat++) {
        curFrame[actTube] = pat[actPat];
        for (int i = actTube + 1; i < 6; i++) {
          curFrame[i] = 0xFF;
        }
        sendDisplay(curFrame);
        delay(50);

      }
      actTube++;
    }

    actTube = 0;

    while (actTube < 6) {
      int pat[3] = {0x0D, 0x9F, 0xFF};

      for (int actPat = 0; actPat < 3; actPat++) {
        curFrame[actTube] = pat[actPat];
        for (int i = actTube + 1; i < 6; i++) {
          curFrame[i] = 0x00;
        }
        sendDisplay(curFrame);
        delay(50);

      }
      actTube++;
    }
  }
}

void vertwipeAnim(int rotations) {
    int pat[] = {0x7F, 0x3B, 0x39, 0x11, 0x00, 0x81, 0xC5, 0xC7, 0xEF, 0xFF};

    int curFrame[6];
    
    for (int curPat=0; curPat<10; curPat++) {
       for(int i=0; i<6; i++) {
           curFrame[i] = pat[curPat];    
       }
       sendDisplay(curFrame);
       delay(50);
    }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  Serial.begin(9600);

  delay(250);
}

void loop() {
  testDisplay();
  delay(500);
  clearDisplay();
  delay(500);
  
  for (int i=0; i<2; i++) {
      vertwipeAnim(1);
      wipeAnim(1);
  }
  
  bigeightAnim(2);
  snakeAnim(2);
  circleAnim(2);
}
