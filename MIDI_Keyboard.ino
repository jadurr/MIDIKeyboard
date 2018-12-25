#include "MIDIUSB.h"
#define NUM_TOTAL_BUTTONS 12
#define NUM_BUTTONS 4
#define NUM_MUX_BUTTONS 8
#define NUM_ANALOG 0
#define NUM_MUX_ANALOG 8
#define MUX_DIGITAL_COM 5
#define MUX_ANALOG_COM A0
#define MIDI_CHANNEL 0
#define MUX_SELECT_0 2
#define MUX_SELECT_1 3
#define MUX_SELECT_2 4

const int selectPins[3] = {MUX_SELECT_0, MUX_SELECT_1, MUX_SELECT_2};
const int digitalMuxInput = MUX_DIGITAL_COM;
const int analogMuxInput = MUX_ANALOG_COM;
byte digitalInputs[NUM_BUTTONS] = {6, 7, 8, 9};
byte analogInputs[NUM_ANALOG];
uint8_t pressedButtons = 0x00;
uint8_t previousButtons = 0x00;
const byte notePitchesMux[NUM_MUX_BUTTONS] = {48, 49, 50, 51, 52, 53, 54, 55};
const byte notePitches[NUM_BUTTONS] = {56, 57, 58, 59};


void setup() {
  for(int i=0; i<3; i++){
    pinMode(selectPins[i],OUTPUT);
  }
  for(int i=0; i<4; i++){
    pinMode(digitalInputs[i], INPUT_PULLUP);
  }
  pinMode(digitalMuxInput, INPUT_PULLUP);
  pinMode(analogMuxInput, INPUT);
  Serial.begin(9600);
}

void loop() {
  readNote();
  playNote();
  readCC();
  playCC();

}

void readNote(){
  readButtons();
  //readMuxButtons();
}

void playNote(){
  playButtons();
  //playMuxButtons();
}

void readButtons(){
  for (int digitalchannel = 0; digitalchannel < 4; digitalchannel++)
  {
    int digitalValue = digitalRead(digitalInputs[digitalchannel]);
    int digitalread = HIGH;


    if (digitalRead(digitalInputs[digitalchannel]) == LOW)
    {
      bitWrite(pressedButtons, digitalchannel, 1);
      delay(50);
    }
    else{
      bitWrite(pressedButtons, digitalchannel, 0);

  }
 }
}


void playButtons(){
  for (int i = 0; i < 4; i++)
  {
    if (bitRead(pressedButtons, i) != bitRead(previousButtons, i))
    {
      if (bitRead(pressedButtons, i))
      {
        bitWrite(previousButtons, i , 1);
        noteOn(0, notePitches[i], 100);
        MidiUSB.flush();
      }
      else
      {
        bitWrite(previousButtons, i , 0);
        noteOff(0, notePitches[i], 0);
        MidiUSB.flush();
      }
    }
  }
}

void readMuxButtons(){
    for (byte i = 0; i < 8; i++)
  {
    selectMuxPin(i);
    if (digitalRead(5) == LOW)
    {
      bitWrite(pressedButtons, i, 1);
      delay(50);
    }
    else
      bitWrite(pressedButtons, i, 0);
  }
}

void playMuxButtons(){
    for (int i = 0; i < 8; i++)
  {
    selectMuxPin(i);
    if (bitRead(pressedButtons, i) != bitRead(previousButtons, i))
    {
      if (bitRead(pressedButtons, i))
      {
        bitWrite(previousButtons, i , 1);
        noteOn(0, notePitchesMux[i], 100);
        MidiUSB.flush();
      }
      else
      {
        bitWrite(previousButtons, i , 0);
        noteOff(0, notePitchesMux[i], 0);
        MidiUSB.flush();
      }
    }
  }
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void readCC(){
  //readAnalog();
 // readMuxAnalog();
}

void playCC(){
//  playAnalog();
//  playMuxAnalog();
}

void readAnalog(){

}

void readMuxAnalog(){

}

void playAnalog(){

}

void playMuxAnalog(){

}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void selectMuxPin(byte pin){
  for(int i=0; i<3; i++){
    if(pin & (1<<i))
      digitalWrite(selectPins[i], HIGH);
    else
      digitalWrite(selectPins[i], LOW);
  }
}
