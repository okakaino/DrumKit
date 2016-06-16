#include <VirtualWire.h>

byte switch_pin = 6;

bool current_state = false;

const byte transimit_pin = 3;
const byte transimitting_indicator_led = 13;

const byte INSTRUMENT_KEY = 84;
const byte MIDI_NOTE_ON = 0x99;     // note on in channel 10
const byte MIDI_NOTE_OFF = 0x89;    // note off in channel 10
const byte NOTE_VELOCITY = 127;     // velocity = 127, maximum

void setup()
{
  pinMode(switch_pin, INPUT);
  pinMode(transimitting_indicator_led, OUTPUT);
  
  vw_set_ptt_inverted(true);
  vw_set_tx_pin(transimit_pin);
  vw_setup(4000);
}

void loop()
{
  bool digital_reading = digitalRead(switch_pin);
  
  if(current_state != digital_reading)
  {
    current_state = digital_reading;

    char msg[3];
    
    msg[1] = INSTRUMENT_KEY;
    msg[2] = NOTE_VELOCITY;
    
    if(current_state)
    {
      msg[0] = MIDI_NOTE_ON;
    }
    else
    {
      msg[0] = MIDI_NOTE_OFF;
    }
    
    digitalWrite(transimitting_indicator_led, true);
    vw_send((uint8_t *)msg, sizeof(msg));
    vw_wait_tx();
    digitalWrite(transimitting_indicator_led, false);
  }
}
