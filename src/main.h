// Set up the DHT temp/humidity sensor
#ifndef  DHTTYPE
#define  DHTTYPE DHT22                          /* Default to DHT22 */
#endif                                          /* ----- #ifndef DHTTYPE  ----- */
#define DHTPIN 2        // Pin D2
#define READ_F true

// Initialize / define vars for the temperature readings
#define T_LENGTH 10 // Number of temperatures to use for averaging
#define T_DELAY 2000 // wait between tem readings (the sensor is slow)
struct Temperature {
	int index;
	float reading[T_LENGTH];
	float average;
	unsigned long last_read;
	int set_point;
	int humidity;
};

// Setup for the relay
#define RELAYPIN 3    // Pin D3
#define RELAY_ON 1
#define RELAY_OFF 0
#define RELAY_DELAY 300000 // 300,000 ms (5 minute) delay
struct Relay {
	int actual_state;
	int desired_state;
	unsigned long last_change;
};

// select the pins used on the LCD panel
#define BACKLIGHT_PIN 10    // Backlight PWM pin

// define some values used by the panel and buttons
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
#define BTNDELAY  300
