#include <math.h>
#include <LiquidCrystal.h>
#include "DHT.h"

// Set up the DHT temp/humidity sensor
#define DHTPIN 2        // Pin D2
#define DHTTYPE DHT11   // DHT 11
#define RELAYPIN 3    // Pin D3
#define READ_F true
DHT dht = DHT(DHTPIN, DHTTYPE);
int temp = 0;
int humid = 0;
int temp_set = 65;
int relay_state = 0;

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

// read the buttons
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

void printtemp_set()
{
 lcd.setCursor(6,1);            // move cursor to second line "1" and 6 spaces over
 lcd.print(temp_set);
 lcd.write('   ');              // Super lazy way to make sure the line is cleared after the temp_set
}

void readtemp()
{
  temp = round(dht.readTemperature(READ_F));
  humid = round(dht.readHumidity());
}

void printtemp()
{
  lcd.setCursor(6,0);
  lcd.print(temp);
  lcd.print("F ");
  lcd.print(humid);
  lcd.print("%   ");
}

void setrelay()
{
  lcd.setCursor(10,1);
  if (temp_set > temp) {
    relay_state = 1;
    digitalWrite(RELAYPIN, HIGH);
    lcd.print("ON ");
  } else {
    relay_state = 0;
    digitalWrite(RELAYPIN, LOW);
    lcd.print("OFF");
  }
}

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

void setup()
{
 pinMode(RELAYPIN, OUTPUT);
 digitalWrite(RELAYPIN, LOW);
 lcd.begin(16, 2);              // start the library
 dht.begin();
 init_display();
 pinMode(BACKLIGHT_PIN, OUTPUT);
 set_backlight();
}
 
void loop()
{
 readtemp();
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
     //lcd.print("UP    ");
     temp_set++;
     printtemp_set();
     delay(BTNDELAY);
     break;
     }
   case btnDOWN:
     {
     //lcd.print("DOWN  ");
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
    /*
   case btnNONE:
     {
     lcd.print("NONE  ");
     break;
     }
     */
 }

}
