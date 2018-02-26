#include <math.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include "main.h"
#include "debugutils.h"

// Initialize global structs
struct Temperature temperature = {-1, {0}, 0, 0, 65, 0};
struct Relay relay = {RELAY_OFF, RELAY_OFF, 0};

// Initialize global IO vars
DHT dht = DHT(DHTPIN, DHTTYPE);
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int backlight = 32;        // PWM value for backlight (0-255)
int lcd_key     = 0;
int adc_key_in  = 0;

void init_display()
{
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.setCursor(0,1);
  lcd.print("Set: ");
}

void set_backlight()
{
  if (backlight < 0) {
    backlight = 0;
  } else if (backlight > 255) {
    backlight = 255;
  }

  analogWrite(BACKLIGHT_PIN, backlight);

}

int read_LCD_buttons()
{
  adc_key_in = analogRead(0);      // read the value from the sensor 
  if (adc_key_in > THRESH_NONE)   return btnNONE; // Most likely result first
  if (adc_key_in < THRESH_RIGHT)  return btnRIGHT;
  if (adc_key_in < THRESH_UP)     return btnUP;
  if (adc_key_in < THRESH_DOWN)   return btnDOWN;
  if (adc_key_in < THRESH_LEFT)   return btnLEFT;
  if (adc_key_in < THRESH_SELECT) return btnSELECT;

  return btnNONE;  // when all others fail, return this...
}

float get_temp()
{
  float temp_reading = dht.readTemperature(READ_F);
  temperature.last_read = millis();
  // Sanity check the returned value; fail to 0
  return isnan(temp_reading) ? 0 : temp_reading;
}

void calculate_temp()
{
  // Check our startup boundary condition
  //TODO: Move this to setup()
  if (temperature.index == -1) {
    float temp_current = get_temp();
    for(int i = 0; i < T_LENGTH; i++) {
      temperature.reading[i] = temp_current;
    }
    temperature.index = 0;
    temperature.average = temp_current;
  } else {
    // Check if we've waited long enough for the next reading
    if ((millis() - temperature.last_read) >= T_DELAY) {
      // Increment the array index; reset to 0 if we're over the end
      if (++temperature.index >= T_LENGTH) {
        temperature.index = 0;
      }

      float temp_current = get_temp();
      temperature.reading[temperature.index] = temp_current;

      // Get the average temp from the array (last T_LENGTH readings)
      float temp_sum = 0;
      for(int j = 0; j < T_LENGTH; j++) {
        temp_sum += temperature.reading[j];
      }
      temperature.average = temp_sum / T_LENGTH;

      DEBUGPRINTLN( temp_current, DEC );
      DEBUGPRINTLN( temperature.average, DEC );
    }
  }

  // Read the humidity (no fancy processing, we don't really care about it much)
  temperature.humidity = round(dht.readHumidity());
}

void printtemp()
{
  lcd.setCursor(6,0);
  lcd.print(round(temperature.average));
  lcd.print("F ");
  lcd.print(temperature.humidity);
  lcd.print("%   ");
}

void printtemp_set()
{
  lcd.setCursor(6,1);            // move cursor to second line "1" and 6 spaces over
  lcd.print(temperature.set_point);
  lcd.write('   ');              // Super lazy way to make sure the line is cleared after the temp_set
}

void setrelay()
{
  if (temperature.set_point > temperature.average) {
    // Actual temp lower than desired temp, we want the heat on
    relay.desired_state = RELAY_ON;
  } else {
    // Actual temp equal to, or higher than desired temp, we want the heat off
    relay.desired_state = RELAY_OFF;
  }

  if (relay.desired_state != relay.actual_state) {
    // Desired state difers from actual state; flip the actual state
    lcd.setCursor(10,1);
    if ((millis() - relay.last_change) >= RELAY_DELAY) {
      relay.actual_state = relay.desired_state;
      digitalWrite(RELAYPIN, relay.actual_state);
      relay.last_change = millis();
      // Display the relay state
      if (relay.actual_state == RELAY_ON) {
        lcd.print("ON ");
      } else {
        lcd.print("OFF");
      }
    } else {
      // We want to change the state, but we're in a loackout period
      lcd.print("LCK");
    }
  }
}

void setup()
{
  Serial.begin( 9600 );
  DEBUGPRINTLN( "begin setup" );
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW);    // init the relay to 'OFF'
  lcd.begin(16, 2);               // start the library
  dht.begin();
  init_display();
  pinMode(BACKLIGHT_PIN, OUTPUT);
  set_backlight();
  // Fake the first relay state change so we can change the relay state 
  // without waiting 5 minutes after startup
  relay.last_change = millis() - RELAY_DELAY;
  lcd.setCursor(10,1);
  lcd.print("OFF");
}

void loop()
{
  calculate_temp();
  printtemp();
  printtemp_set();
  setrelay();

  lcd_key = read_LCD_buttons();  // read the buttons

  switch (lcd_key)               // depending on which button was pushed, we perform an action
  {
    case btnRIGHT:
      {
        backlight += 16;
        set_backlight();
        delay(BTNDELAY);
        break;
      }
    case btnLEFT:
      {
        backlight -= 16;
        set_backlight();
        delay(BTNDELAY);
        break;
      }
    case btnUP:
      {
        temperature.set_point++;
        printtemp_set();
        delay(BTNDELAY);
        break;
      }
    case btnDOWN:
      {
        temperature.set_point--;
        printtemp_set();
        delay(BTNDELAY);
        break;
      }
    case btnSELECT:
      {
        //lcd.print("SELECT");
        break;
      }
    case btnNONE:
      {
        //lcd.print("NONE  ");
        break;
      }
  }

}
