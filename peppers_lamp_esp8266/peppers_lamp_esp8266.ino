/** code to run lamp

pin connections

ws2811 data pin ---> pin 12 (D6)
push button topside ----> pin 14 (D5) ---> 10k ohm pulluup ---> 3.3V
push button bottomside ---> 220 ohm ---> GND
potentiometer middle ---> A0
potentiometer outer pins ---> GND, 3.3V

****/


#include <Adafruit_NeoPixel.h>
#include <Math.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN        12
#define NUMPIXELS 23

int button_pin = 14; //button pin

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


int hsv_colour;
bool multi_colour = false;
bool button_state;
bool prev_button_state = true;
unsigned long light_speed; //the speed at which the colours chagne for flash mode
unsigned long last_time;

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0

int sensorValue = 0;  // value read from the pot
int last_sv = 0;
int q=0;

void setup() {
  pixels.begin();
  pixels.setBrightness(225);
  // initialize serial communication at 115200
  Serial.begin(9600);
  pinMode(button_pin, INPUT_PULLUP);
  //digitalWrite(button_pin, HIGH);
  delay(10);
  button_state = digitalRead(button_pin);
  //Serial.println(button_state);
  prev_button_state = button_state;
  last_time = millis();
}

//

void loop() {
  // read the analog in value
  sensorValue = 0x0000 | analogRead(analogInPin);
  //Serial.println(sensorValue);
  button_state = digitalRead(button_pin);
  
  //this logic means that we need to press the button, then take our hand off
  //so if we hold down the button it won't keep changing.
  //we press the button to change from mulit_colour to change colour
  if(!button_state){
    //Serial.println("here");
    if(prev_button_state){
      prev_button_state = button_state;
      multi_colour = !multi_colour; //toggle state
      Serial.print("multi colour = ");
      Serial.println(multi_colour);
    }
    delay(500); //avoid bounce (hopefully)
  } else {
    prev_button_state = button_state;
  }

  light_speed = map(sensorValue, 0, 1024, 100, 4000);



  if(millis() - last_time>light_speed){
    last_time = millis();
    q++;
    if(q>22) q = 0; //reset q if it gets greater than 22. 

    if (light_speed > 3500){q=0;}//if we are above every 3s, then we don't change it but keep the lights static.
    if(multi_colour ) {
      for (uint16_t j = 0; j< pixels.numPixels(); j++){
        
        //for multi-colour, we keep changing the pixel colours
        pixels.setPixelColor(j, pixels.gamma32(pixels.ColorHSV((uint16_t ) ((int) ((double) hsv_colour + ((double) ((j+q)%pixels.numPixels()))*( 65535.0/23.0)) % 65535),255,255)));
      }
      pixels.show();
    }

  }

  //if we've changed the sensor value
  if(abs(sensorValue-last_sv)>20 && !multi_colour){
    //Serial.println("in if");
    hsv_colour = map(sensorValue, 0,1024,0,65535);
    for (uint16_t j = 0; j< pixels.numPixels(); j++){
      
      //for multi-colour, we keep changing the pixel colours
      if(multi_colour) pixels.setPixelColor(j, pixels.gamma32(pixels.ColorHSV((uint16_t ) ((int) ((double) hsv_colour + ((double) ((j+q)%pixels.numPixels()))*( 65535.0/23.0)) % 65535),255,255)));
      else pixels.setPixelColor(j, pixels.gamma32(pixels.ColorHSV((uint16_t )  hsv_colour ,255,255)));
    }
    pixels.show();
    last_sv = sensorValue;
    
  }
  // print the readings in the Serial Monitor
  
  
  delay(30);
}
