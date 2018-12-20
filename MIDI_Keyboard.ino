#include "MIDIUSB.h"

const int selectPins[3] = {2, 3, 4};
const int digitalMuxInput = 5;
const int analogMuxInput = A0;
const int digitalInputs[4] = {6, 7, 8, 9};

void setup() {
  for(int i=0; i<3; i++){
    pinMode(selectPins[i],OUTPUT);
  }
  for(int i=0; i<4; i++){
    pinMode(digitalInputs[i], INPUT_PULLUP);
  }
  pinMode(digitalMuxInput, INPUT_PULLUP);
  pinMode(analogMuxInput, INPUT);
}

void loop() {

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
