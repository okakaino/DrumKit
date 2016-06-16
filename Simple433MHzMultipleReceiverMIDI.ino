#include <VirtualWire.h>

void setup()
{
  Serial.begin(31250);
  
  pinMode(13, OUTPUT);
  
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_rx_pin(2);
  vw_setup(4000);  // Bits per sec

  vw_rx_start();       // Start the receiver PLL running
}

void loop()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    digitalWrite(13, 1);

//    Serial.print("midi_note: ");
//    Serial.print(buf[1]);
//    Serial.print(", midi_velocity: ");
//    Serial.println(buf[2]);

    midi_message(buf[0], buf[1], buf[2]);

    digitalWrite(13, 0);
  }
}

// Transmit MIDI Message
void midi_message(byte command, byte pitch, byte velocity) 
{
  Serial.write(command);
  Serial.write(pitch);
  Serial.write(velocity);
}
