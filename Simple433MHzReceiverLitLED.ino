#include <VirtualWire.h>

unsigned long led_start_time = millis();
boolean led_on = false;
const int led_duration = 1000;

void setup()
{
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_rx_pin(12);
  vw_setup(4000);  // Bits per sec
  
  pinMode(13, OUTPUT);

  vw_rx_start();       // Start the receiver PLL running
}

void loop()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    led_start_time = millis();
    led_on = true;
    
    analogWrite(5, 200);
    analogWrite(6, 200);
  }

  if(millis() - led_start_time > led_duration && led_on)
  {
    led_on = false;
    
    analogWrite(5, 0);
    analogWrite(6, 0);
  }
}
