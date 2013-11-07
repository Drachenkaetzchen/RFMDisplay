


int oePin = 6;
int mrPin = 8;
// Verified
int dataPin1 = 9;
int dataPin2 = 10;
int dataPin3 = 11;
int dataPin4 = 12;
int clockPin = 13;
int latchPin = 7;

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin1, OUTPUT);
  pinMode(dataPin2, OUTPUT);
  pinMode(dataPin3, OUTPUT);
  pinMode(dataPin4, OUTPUT);
  
    pinMode(oePin, OUTPUT);
    digitalWrite(oePin, LOW);
    
        pinMode(mrPin, OUTPUT);
    digitalWrite(mrPin, HIGH);
}

void loop() {
  // count from 0 to 255 and display the number 
  // on the LEDs
  for (int numberToDisplay = 0; numberToDisplay < 256; numberToDisplay++) {
    // take the latchPin low so 
    // the LEDs don't change while you're sending in bits:
    digitalWrite(latchPin, LOW);
    // shift out the bits:
    shiftOut(dataPin1, clockPin, MSBFIRST, numberToDisplay);  

    //take the latch pin high so the LEDs will light up:
    digitalWrite(latchPin, HIGH);
    // pause before next value:
    delay(500);
  }
}
