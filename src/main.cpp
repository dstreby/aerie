#include <math.h>
#include <LiquidCrystal.h>
#include "DHT.h"

// Set up the DHT temp/humidity sensor
#define DHTPIN 2        // Pin D2
#define DHTTYPE DHT11   // DHT 11
#define READ_F true
DHT dht = DHT(DHTPIN, DHTTYPE);

// Initialize / define vars for the temperature readings
#define T_LENGTH 10 // Number of temperatures to use for averaging
#define T_DELAY 2000 // wait between tem readings (the sensor is slow)
unsigned long t_last_read = 0; // Keep track of the last time the temp was read
int t_index = -1; // Start the index at -1 so we can catch it the first time around and pad the array
float temp[T_LENGTH];	// We'll use an array of floats to generate the average temp
float temp_avg = 0;
int humid = 0;
int temp_set = 65;

// Setup for the relay
#define RELAYPIN 3    // Pin D3
#define RELAY_ON 1
#define RELAY_OFF 0
#define RELAY_DELAY 300000 // 300,000 ms (5 minute) delay
int relay_state = RELAY_OFF;
int relay_desired_state = RELAY_OFF;
unsigned long relay_last_change = 0;

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#define BACKLIGHT_PIN 10    // Backlight PWM pin
int backlight = 32;        // PWM value for backlight (0-255)

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
#define BTNDELAY  300

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
	// my buttons when read are centered at these valies: 0, 144, 329, 504, 741
	// we add approx 50 to those values and check to see if we are close
	if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
	if (adc_key_in < 50)   return btnRIGHT;
	if (adc_key_in < 195)  return btnUP;
	if (adc_key_in < 380)  return btnDOWN;
	if (adc_key_in < 555)  return btnLEFT;
	if (adc_key_in < 790)  return btnSELECT;

	return btnNONE;  // when all others fail, return this...
}

float get_temp()
{
	float temp_reading = dht.readTemperature(READ_F);
	t_last_read = millis();
	// Sanity check the returned value; fail to 0
	return isnan(temp_reading) ? 0 : temp_reading;
}

void calculate_temp()
{
	// Check our startup boundary condition
	if (t_index == -1) {
		float temp_current = get_temp();
		for(int i = 0; i < T_LENGTH; i++) {
			temp[i] = temp_current;
		}
		t_index = 0;
		temp_avg = temp_current;
	} else {
		// Check if we've waited long enough for the next reading
		if ((millis() - t_last_read) >= T_DELAY) {
			// Increment the array index; reset to 0 if we're over the end
			if (++t_index >= T_LENGTH) {
				t_index = 0;
			}

			float temp_current = get_temp();
			temp[t_index] = temp_current;

			// Get the average temp from the array (last T_LENGTH readings)
			float temp_sum = 0;
			for(int j = 0; j < T_LENGTH; j++) {
				temp_sum += temp[j];
			}
			temp_avg = temp_sum / T_LENGTH;
		}
	}

	// Read the humidity (no fancy processing, we don't really care about it much)
	humid = round(dht.readHumidity());
}

void printtemp()
{
	lcd.setCursor(6,0);
	lcd.print(round(temp_avg));
	lcd.print("F ");
	lcd.print(humid);
	lcd.print("%   ");
}

void printtemp_set()
{
	lcd.setCursor(6,1);            // move cursor to second line "1" and 6 spaces over
	lcd.print(temp_set);
	lcd.write('   ');              // Super lazy way to make sure the line is cleared after the temp_set
}

void setrelay()
{
	if (temp_set > temp_avg) {
		// Actual temp lower than desired temp, we want the heat on
		relay_desired_state = RELAY_ON;
	} else {
		// Actual temp equal to, or higher than desired temp, we want the heat off
		relay_desired_state = RELAY_OFF;
	}

	if (relay_desired_state != relay_state) {
		// Desired state difers from actual state; flip the actual state
		lcd.setCursor(10,1);
		if ((millis() - relay_last_change) >= RELAY_DELAY) {
			relay_state = relay_desired_state;
			digitalWrite(RELAYPIN, relay_state);
			relay_last_change = millis();
			// Display the relay state
			if (relay_state == RELAY_ON) {
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
	pinMode(RELAYPIN, OUTPUT);
	digitalWrite(RELAYPIN, LOW);		// init the relay to 'OFF'
	lcd.begin(16, 2);								// start the library
	dht.begin();
	init_display();
	pinMode(BACKLIGHT_PIN, OUTPUT);
	set_backlight();
	// Fake the first relay state change so we can change the relay state 
	// without waiting 5 minutes after startup
	relay_last_change = millis() - RELAY_DELAY;
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
				temp_set++;
				printtemp_set();
				delay(BTNDELAY);
				break;
			}
		case btnDOWN:
			{
				temp_set--;
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
