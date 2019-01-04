#include "MIDIUSB.h"

#define SELECT_0 2
#define SELECT_1 3
#define SELECT_2 4
#define MUX_COM_DIGITAL 5
#define MUX_COM_ANALOG A0
#define NUM_MUX_DIGITAL 8
#define NUM_DIGITAL 4
#define NUM_MUX_ANALOG 8
#define NUM_ANALOG 0
#define TOTAL_NUM_BUTTONS 12
#define TOTAL_NUM_ANALOG 8

int selectPins[] = {SELECT_0, SELECT_1, SELECT_2};

const int digitalChannelPin[NUM_DIGITAL] = {6, 7, 8, 9};
byte notePitches[TOTAL_NUM_BUTTONS] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59};
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
}

void loop() {
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
          noteOn(0, notePitches[i], 100);
          MidiUSB.flush();
        }
        else
        {
          noteOff(0, notePitches[i], 0);
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
          noteOn(0, notePitches[8 + i], 100);
          MidiUSB.flush();
        }
        else
        {
          noteOff(0, notePitches[8 + i], 0);
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
    Serial.print(muxTimer[i]);
    Serial.println();
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
//pin 10 and pin 14 for octave up and down
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
