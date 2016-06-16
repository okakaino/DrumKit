#include <VirtualWire.h>

bool switches_on[3] = {false, false, false};

byte switches[3] = {2, 4, 7};

const byte leds[3] = {3, 5, 6};

bool current_state = false;

const byte transimit_pin = 8;
const byte transimitting_indicator_led = 13;

const byte INSTRUMENT_KEYS[3] = {48, 60, 72};;
const byte MIDI_NOTE_ON = 0x99;     // note on in channel 10
const byte MIDI_NOTE_OFF = 0x89;    // note off in channel 10
const byte NOTE_VELOCITY = 127;     // velocity = 127, maximum

void setup()
{
  pinMode(switches[0], INPUT);
  pinMode(switches[1], INPUT);
  pinMode(switches[2], INPUT);
  pinMode(leds[0], OUTPUT);
  pinMode(leds[1], OUTPUT);
  pinMode(leds[2], OUTPUT);
  pinMode(transimitting_indicator_led, OUTPUT);

  digitalWrite(leds[0], false);
  digitalWrite(leds[1], false);
  digitalWrite(leds[2], false);
  
  vw_set_ptt_inverted(true);
  vw_set_tx_pin(transimit_pin);
  vw_setup(4000);
}

void loop()
{
  for(byte i = 0; i <= 2; i++)
  {
    bool temp_reading = digitalRead(switches[i]);
    
    if(switches_on[i] != temp_reading)
    {
      switches_on[i] = temp_reading;

      char msg[3];
    
      msg[1] = INSTRUMENT_KEYS[i];
      msg[2] = NOTE_VELOCITY;
      
      if(switches_on[i])
      {
        msg[0] = MIDI_NOTE_ON;
      }
      else
      {
        msg[0] = MIDI_NOTE_OFF;
      }

      digitalWrite(leds[i], switches_on[i]);
      
      digitalWrite(transimitting_indicator_led, true);
      vw_send((uint8_t *)msg, sizeof(msg));
      vw_wait_tx();
      digitalWrite(transimitting_indicator_led, false);
    }
  }
}
