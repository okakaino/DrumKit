bool switches_on[3] = {false, false, false};

byte switches[3] = {2, 4, 7};

const byte leds[3] = {9, 10, 11};

const byte INSTRUMENT_KEYS[3] = {48, 60, 72};
const byte MIDI_NOTE_ON = 0x99;     // note on in channel 10
const byte MIDI_NOTE_OFF = 0x89;    // note off in channel 10
const byte NOTE_VELOCITY = 127;     // velocity = 127, maximum

void setup()
{
  Serial.begin(31250);
  
  pinMode(switches[0], INPUT);
  pinMode(switches[1], INPUT);
  pinMode(switches[2], INPUT);
  pinMode(leds[0], OUTPUT);
  pinMode(leds[1], OUTPUT);
  pinMode(leds[2], OUTPUT);

  digitalWrite(leds[0], false);
  digitalWrite(leds[1], false);
  digitalWrite(leds[2], false);
}

void loop()
{
  for(byte i = 0; i <= 2; i++)
  {
    bool temp_reading = digitalRead(switches[i]);
    
    if(switches_on[i] != temp_reading)
    {
      switches_on[i] = temp_reading;

      if(switches_on[i])
      {
        midi_message(MIDI_NOTE_ON, INSTRUMENT_KEYS[i], NOTE_VELOCITY);
      }
      else
      {
        midi_message(MIDI_NOTE_OFF, INSTRUMENT_KEYS[i], NOTE_VELOCITY);
      }

      digitalWrite(leds[i], switches_on[i]);
    }
  }
}

// Transmit MIDI Message
void midi_message(byte command, byte pitch, byte velocity) 
{
  Serial.write(command);
  Serial.write(pitch);
  Serial.write(velocity);
}

