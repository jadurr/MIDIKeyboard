#include "MIDIUSB.h"

const int selectPins[3] = {2, 3, 4};
const int digitalMuxInput = 5;
const int analogMuxInput = A0;
const uint8_t digitalInputs[4] = {6, 7, 8, 9};
uint8_t pressedButtons = 0x00;
uint8_t previousButtons = 0x00;
const byte notePitchesMux[8] = {48, 49, 50, 51, 52, 53, 54, 55};
const byte notePitches[4] = {56, 57, 58, 59};

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

void selectMuxPin(byte pin){
  for(int i=0; i<3; i++){
    if(pin & (1<<i))
      digitalWrite(selectPins[i], HIGH);
    else
      digitalWrite(selectPins[i], LOW);
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

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void readNote(){
  //readButtons();
  readMuxButtons();
}

void playNote(){
 // playButtons();
  playMuxButtons();
}

void readButtons(){
  for (int i = 0; i < 4; i++)
  {
    if (digitalRead(digitalInputs[i]) == LOW)
    {
      bitWrite(pressedButtons, i, 1);
      delay(50);
    }
    else
      bitWrite(pressedButtons, i, 0);
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
        noteOn(0, notePitches[i], 64);
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
    for (int i = 0; i < 8; i++)
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
    if (bitRead(pressedButtons, i) != bitRead(previousButtons, i))
    {
      if (bitRead(pressedButtons, i))
      {
        bitWrite(previousButtons, i , 1);
        noteOn(0, notePitchesMux[i], 64);
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

void readCC(){
  //readAnalog();
 // readMuxAnalog();
}

void playCC(){
//  playAnalog();
//  playMuxAnalog();
}
