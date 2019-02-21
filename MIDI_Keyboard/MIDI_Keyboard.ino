#include "MIDIUSB.h"

#define SELECT_0 2 //Multiplexer Pin A
#define SELECT_1 3 //Multiplexer Pin B
#define SELECT_2 4 //Multiplexer Pin C
#define MUX_COM_DIGITAL 5 //Multiplexer Digital Input Pin
#define MUX_COM_ANALOG A0 //Multiplexer Analog Input Pin
#define NUM_MUX_DIGITAL 8
#define NUM_DIGITAL 4 //Amount of Digital Inputs not on the multiplexer
#define NUM_MUX_ANALOG 8
#define NUM_ANALOG 0 //Amount of Analog Inputs not on the multiplexer
#define TOTAL_NUM_BUTTONS 12 //Total number of buttons
#define TOTAL_NUM_ANALOG 8 //Total number of analog inputs
#define OCTAVE_UP 14    //Octave up button
#define OCTAVE_DOWN 10    //Octave down button

int selectPins[] = {SELECT_0, SELECT_1, SELECT_2};
int octave = 4;
const int digitalChannelPin[NUM_DIGITAL] = {6, 7, 8, 9};
byte notePitches[][TOTAL_NUM_BUTTONS] = {
{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
{12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
{24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35},
{36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47},
{48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59},
{60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71},
{72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83},
{84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95},
{96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107},
{108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119},
{120, 121, 122, 123, 124, 125, 126, 127, 127, 127, 127, 127},
};
int muxBLState[NUM_MUX_DIGITAL] = {0};
int muxBTState[NUM_MUX_DIGITAL] = {0};
unsigned long debounceMux[NUM_MUX_DIGITAL] = {0};
unsigned long debounceDelay = 5;
int BLState[NUM_DIGITAL] = {0};
int BTState[NUM_DIGITAL] = {0};
unsigned long debounceButton[NUM_DIGITAL] = {0};

int muxPLState[NUM_MUX_ANALOG] = {0};
int muxPTState[NUM_MUX_ANALOG] = {0};
int midiPLState[NUM_MUX_ANALOG] = {0};
int midiPTState[NUM_MUX_ANALOG] = {0};
int muxPVar = 0;
int TIMEOUT = 300;
int varThreshold = 10;
boolean muxPotMoving = true;
unsigned long muxPTime[NUM_MUX_ANALOG] = {0};
unsigned long muxTimer[NUM_MUX_ANALOG] = {0};
int octaveUpState = 0;         // current state of the button
int octaveDownState = 0;         // current state of the button
int originalOctaveState = 0;     // previous state of the button

void setup() {
  Serial.begin(9600);

  for(int i = 0; i < NUM_DIGITAL; i++){
    pinMode(digitalChannelPin[i], INPUT_PULLUP);
  }
  pinMode(MUX_COM_DIGITAL, INPUT_PULLUP);
  pinMode(MUX_COM_ANALOG, INPUT);
  pinMode(SELECT_0, OUTPUT);
  pinMode(SELECT_1, OUTPUT);
  pinMode(SELECT_2, OUTPUT);
  pinMode(OCTAVE_UP, INPUT_PULLUP);
  pinMode(OCTAVE_DOWN, INPUT_PULLUP);
}

void loop() {
  chooseOctave();
  playNotes();
  playCC();
}

void playNotes(){
  for (int i = 0; i < NUM_MUX_DIGITAL; i++){
    selectMuxPin(i);
    muxBLState[i] = digitalRead(MUX_COM_DIGITAL);
    if((millis() - debounceMux[i]) > debounceDelay){
      if(muxBTState[i] != muxBLState[i]){
        debounceMux[i] = millis();
        if(muxBLState[i] == LOW){
          noteOn(0, notePitches[octave][i], 100);
          MidiUSB.flush();
        }
        else
        {
          noteOff(0, notePitches[octave][i], 0);
          MidiUSB.flush();
        }
        muxBTState[i] = muxBLState[i];
      }
    }
  }
  for (int i = 0; i < NUM_DIGITAL; i++){
    BLState[i] = digitalRead(digitalChannelPin[i]);
    if ((millis() - debounceButton[i]) > debounceDelay){
      if (BTState[i] != BLState[i]){
        debounceButton[i] = millis();
        if(BLState[i] == LOW){
          noteOn(0, notePitches[octave][8 + i], 100);
          MidiUSB.flush();
        }
        else
        {
          noteOff(0, notePitches[octave][8 + i], 0);
          MidiUSB.flush();
        }
        BTState[i] = BLState[i];
      }
    }
  }
}

void playCC(){
  for (int i = 0; i < NUM_MUX_ANALOG; i++){
    selectMuxPin(i);
    muxPLState[i] = analogRead(MUX_COM_ANALOG);
    midiPLState[i] = map(muxPLState[i], 0, 1023, 0, 127);
    muxPVar = abs(muxPLState - muxPTState);
    if(muxPVar > varThreshold){
      muxPTime[i] = millis();
    }
    muxTimer[i] = millis() - muxPTime[i];
    if(muxTimer[i] < TIMEOUT){
      muxPotMoving = true;
    }
    else {
      muxPotMoving = false;
    }
    if(muxPotMoving == true){
      if(midiPTState[i] != midiPLState[i]){
        controlChange(0, i, midiPLState[i]);
        MidiUSB.flush();
        muxPTState[i] = muxPLState[i];
        midiPTState[i] = midiPLState[i];
      }
    }
  }
  for (int i = 0; i < NUM_ANALOG; i++){

  }
}

void selectMuxPin(byte pin){
  for(int i=0; i<3; i++){
    if(pin & (1<<i))
      digitalWrite(selectPins[i], HIGH);
    else
      digitalWrite(selectPins[i], LOW);
  }
}
void chooseOctave(){
     octaveUpState = digitalRead(OCTAVE_UP);
   if (octaveUpState != originalOctaveState) {
     if (octaveUpState == LOW)
     {
      octave++;
     }
   }
   originalOctaveState = octaveUpState;
   octaveDownState = digitalRead(OCTAVE_DOWN);
   if (octaveDownState != originalOctaveState) {
     if (octaveDownState == LOW)
     {
      octave--;
     }
   }
   originalOctaveState = octaveDownState;
   delay(300);
}
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
