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

void setup() {
  // put your setup code here, to run once:
  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  delay(250);
}


void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 16; i++) {
    digitalWrite(clock_pin, LOW);
    digitalWrite(latch_pin, LOW);
    shiftOut(data_pin, clock_pin, LSBFIRST, numbers[i]);
    digitalWrite(latch_pin, HIGH);
    delay(500);
  }
  
}
