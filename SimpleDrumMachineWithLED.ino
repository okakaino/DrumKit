//*******************************************************************************************************************
// simple drum trigger sketch
// by ted gao (diiingdong@gmail.com)
// non-blocking idea inspired by Luca Soltoggio (https://arduinoelectronics.wordpress.com/2015/05/12/non-blocking-breathing-led/)
// last edited: jan 20, 2016 GMT+8
//*******************************************************************************************************************

//////////////////////////////////////////////////////////////////////////////////////////
//  mapping table, reference available at http://www.midi.org/techspecs/gm1sound.php    //
//  INSTRUMENT            KEY   PIN #                                                   //
//  ACOUSTIC_BASS_DRUM    35      A0                                                    //
//  ACOUSTIC_SNARE_DRUM   38      A1                                                    //
//  HI_MID-TOM            48      A2                                                    //
//  LOW_MID_TOM           47      A3                                                    //
//  HIGH_FLOOR_TOM        43      A4                                                    //
//  OPEN_HI_HAT           46      A5                                                    //
//  CRASH_CYMBAL_1        49      A6                                                    // 
//  RIDE_CYMBAL_1         51      A7                                                    //
//////////////////////////////////////////////////////////////////////////////////////////
#include <LiquidCrystal.h>
#include <VirtualWire.h>

const byte INSTRUMENT_KEYS[8] = {35, 38, 48, 47, 43, 46, 49, 51};
const byte INSTRUMENT_PINS[8] = {A0, A1, A2, A3, A4, A5, A6, A7};

// max number of instruments supported, 6 for Arduino Uno, 8 for Nano
const byte NUMBER_OF_INSTRUMENTS = 8;

const byte latch_pin = 8;
const byte clock_pin = 9;
const byte data_pin = 10;
const byte output_enable_pin = 6;
const byte transiting_led_pin = 13;

const int led_duration = 1000;

// note on message on channel 10 (channel reserved for percussion instruments, 0-indexed)
const byte MIDI_NOTE_ON = 153;
const byte PIEZO_THRESHOLD = 30;

byte leds = 0b00000000;

char *rf_data = "1";

boolean should_send_midi_message[8] = {true, true, true, true, true, true, true, true};
int peak_midi_velocity[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int midi_velocity[8] = {0, 0, 0, 0, 0, 0, 0, 0};

unsigned long led_start_time[8] = {0, 0, 0, 0, 0, 0, 0, 0};

unsigned long lcd_start_time = millis();

boolean lcd_on = false;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
  Serial.begin(115200);

  pinMode(latch_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);
  pinMode(output_enable_pin, OUTPUT);
  pinMode(transiting_led_pin, OUTPUT);

  analogWrite(output_enable_pin, 255 - 200); // set bright for LED, 0 for brightest, 255 for off
  
  vw_set_ptt_inverted(true);
  vw_set_tx_pin(7);
  vw_setup(4000);
  
  lcd.begin(16, 2);
  reset_lcd();
}

void loop()
{
  for(byte i = 0; i < NUMBER_OF_INSTRUMENTS; i++)
  {
    get_velocity(i);

    if(millis() - led_start_time[i] > led_duration)
    {
      leds = leds & ~(1 << i);
    }

    if(millis() - lcd_start_time > led_duration && lcd_on)
    {
      reset_lcd();
    }
  }
  
  update_shift_register();
}

void get_velocity(byte pin_to_read)
{
  int hit_pressure = analogRead(INSTRUMENT_PINS[pin_to_read]);

  if(hit_pressure >= PIEZO_THRESHOLD)
  {
    if(hit_pressure > peak_midi_velocity[pin_to_read])
    {
      peak_midi_velocity[pin_to_read] = hit_pressure;
    }
    else
    {
      if(should_send_midi_message[pin_to_read])
      {
        midi_velocity[pin_to_read] = (peak_midi_velocity[pin_to_read] + 1) / 8 -1;      // mapping 0-1023 to 0-127;
        midi_message(MIDI_NOTE_ON, INSTRUMENT_KEYS[pin_to_read], midi_velocity[pin_to_read]);

        update_lcd(INSTRUMENT_KEYS[pin_to_read], midi_velocity[pin_to_read]);

        send_through_rf();

        led_start_time[pin_to_read] = millis();
        leds = leds | (1 << pin_to_read);
    
        should_send_midi_message[pin_to_read] = false;
      }
    }
  }
  else
  {
    peak_midi_velocity[pin_to_read] = 0;
    should_send_midi_message[pin_to_read] = true;
  }
}

// Transmit MIDI Message
void midi_message(byte command, byte pitch, byte velocity) 
{
  Serial.write(command);
  Serial.write(pitch);
  Serial.write(velocity);
}

void update_shift_register()
{
  digitalWrite(latch_pin, LOW);
  shiftOut(data_pin, clock_pin, LSBFIRST, leds);
  digitalWrite(latch_pin, HIGH);
}

void send_through_rf()
{
  digitalWrite(transiting_led_pin, true);
  vw_send((uint8_t *)rf_data, strlen(rf_data));
  vw_wait_tx();
  digitalWrite(transiting_led_pin, false);
}

void reset_lcd()
{
  lcd.clear();
  lcd.print("Current Note:");
  lcd.setCursor(0, 1);
  lcd.print("Velocity:");

  lcd_on = false;
}

void update_lcd(byte note, int velocity)
{
  lcd_on = true;
  lcd_start_time = millis();
  
  lcd.setCursor(13, 0);
  lcd.print(note);
  
  lcd.setCursor(9, 1);
  lcd.print(velocity);
}

