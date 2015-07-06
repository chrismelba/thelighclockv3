#include <Wire.h> 
#include "RTClib.h" 
#include <avr/power.h>
#include <FastLED.h>
//#include <Time.h>


#define PIXEL_PIN 6  //the pin that attaches to the neopixel strip
#define N_PIX 120 //the number of pixels in the strip
#define colourbuttonPin 9     // the number of the button to change colour
#define modebuttonPin 10 // the number of the button to change mode


RTC_DS1307 rtc;

DateTime now;


CHSV Hour_Colour(40,255,255);
CHSV Minute_Colour(160,255,255);
CHSV Second_Colour(0, 0, 255);

// CRGB Hour_Colour= CRGB::Yellow;
// CRGB Minute_Colour= CRGB::Blue;
// CRGB Second_Colour= CRGB::White;
int Secondsonoff = 1;
int faceMode = 3;
int numfaces = 6;
int colourMode = 0; 
int numcolours = 4;
int Randommode = 0;
int randomonce = 0;
int nightmode = 1;
int debug = 0;
int testrun = 1;
int buttonState[2] = {HIGH, HIGH};
int lastbuttonstate[2] = {HIGH, HIGH};
int readbuttons = 1;

//Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
// NEO_KHZ800800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
// NEO_GRB Pixels are wired for GRB bitstream (most NeoPixel products)

CRGB leds[N_PIX];



void setup() {

  // initialize the pushbutton pin as an input:
  pinMode(modebuttonPin, INPUT_PULLUP); 
  pinMode(colourbuttonPin, INPUT_PULLUP); 


  Serial.begin(115200);
  Wire.begin();
  rtc.begin();

  FastLED.addLeds<NEOPIXEL, PIXEL_PIN>(leds, N_PIX);
  


    //lower brightness for low voltage applications, like when powering off computer USB to test code. ENSURE YOU REMOVE THIS LINE PRIOR TO FINAL BUILD
    
   //FastLED.setBrightness(15);   

    if (! rtc.isrunning()) {
    	Serial.println("RTC is NOT running!");
      // following line sets the RTC to the date & time this sketch was compiled
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // This line sets the RTC with an explicit date & time, for example to set
      // January 21, 2014 at 3am you would call:
      // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  now = rtc.now();

}

void loop() {
      int secs;

  uint16_t hour_pos;
  uint16_t min_pos;
  int invertpos;

    // I wanted a debug mode to run the face faster to test out colour combinations
switch (testrun) {
    case 0:
        // no testing
        hour_pos = (now.hour() % 12) * N_PIX / 12 + now.minute()/6;
        min_pos = now.minute() * N_PIX / 60;
        if(Secondsonoff == 1){
          invertpos= now.second() * N_PIX / 60;
          leds[invertpos] = -leds[invertpos];
        }
      break;
    case 1:
      //set the face to tick ever second rather than every minute 
      hour_pos = (now.minute() % 12) * N_PIX / 12 + now.second()/6;
      min_pos = now.second() * N_PIX / 60;
      invertpos = (now.hour() % 12) * N_PIX / 12 + now.minute()/6;
      leds[invertpos] = -leds[invertpos];     
      // do something
      break;
    case 2: 
      //set the face to the classic 10 past 10 for photos
      hour_pos = 10*N_PIX/12;
      min_pos = 10* N_PIX /60;
  
}

  //show the LEDs here otherwise the second call arguments above will be erased by the face calls below
  FastLED.show();
//randomises the face and colour on the hour
if(Randommode == 1){
  if(min_pos+now.second() == 0){
    if(randomonce == 0){
      colourMode = random8(numcolours);
      faceMode = random8(numfaces);
      colourchange(colourMode);
      randomonce = 1;   
    }
  }
  else
  {
    randomonce = 0;
}

//changes to face 5 (minimal face) if nightmode is enabled and the time is between 11pm and 7am
Serial.println(now.hour());
  if(nightmode == 1){
    if(now.hour()>=23||now.hour()<7){
      faceMode = 5; 
      Secondsonoff = 0;               
      } else {
        Secondsonoff = 1;
      }
  }    

}
// call the face that has it's mode currently active
  switch (faceMode) {
    case 0:
    fadeface(hour_pos, min_pos);
    break;
    case 1:
    chsvface(hour_pos, min_pos);
    break;
    case 2:
    fadefacedark(hour_pos, min_pos);
    break;
    case 3:
    epiphanyface(hour_pos, min_pos);
    break;
    case 4:
    brianface(hour_pos, min_pos);
    break;
    case 5:
    rainbowface(hour_pos, min_pos);
    break;
    case 6:
    simpleface(hour_pos,min_pos);
    break;
    case 7:
    demofacemins(hour_pos,min_pos);
    break;
    case 8:
    demofacehours(hour_pos,min_pos);
  }





  //FastLED.delay(10);
  checkbuttons();
  //update the internal time.
 	now = rtc.now();
  //now.adjustTime(1);

}

void checkbuttons() {



  buttonState[0] = digitalRead(modebuttonPin);
  buttonState[1] = digitalRead(colourbuttonPin);

//check if either button has been released
  if(buttonState[0] != lastbuttonstate[0] || buttonState[1] != lastbuttonstate[1]){
    if(lastbuttonstate[0]==LOW && lastbuttonstate[1]==LOW){
      testrun++;
      testrun%=3;
      lastbuttonstate[0]=HIGH;
      lastbuttonstate[1]=HIGH;        
    }
  }
    //check if just button 0 was released
  if(buttonState[0] != lastbuttonstate[0]){
      if(lastbuttonstate[0]==LOW){
      faceMode++;
      //sets the face and allows minute and hour demo modes to be selectable. 
      faceMode%=(3+numfaces);     
    }
  }

  if(buttonState[1] != lastbuttonstate[1]){
    if(lastbuttonstate[1]==LOW){
      colourMode++;
      colourMode%=numcolours;
      colourchange(colourMode);
    }
  }

  for(int i=0; i<2; i++){
    lastbuttonstate[i]= buttonState[i];        
  }

      
}




void fadeface(uint16_t hour_pos, uint16_t min_pos){
  //this face aims to have each hand desaturate quickly to white and blend them by biasing the hue towards whichever is more saturated
	fract8 fadepoint;

	CHSV hourpix;
	CHSV minpix;
	CHSV blendedpix;
	float fadestepsize = N_PIX/255;
	minpix = Minute_Colour;
	hourpix = Hour_Colour;


	for(int j=0; j<N_PIX; j++){

		//calculate what the minute pixel should look like
		//
		fadepoint = 255-min(255,((min_pos+j)%N_PIX)*3);
		minpix.s = fadepoint;
		//same thing for the hour pixel
		fadepoint = 255-min(255, ((hour_pos+j)%N_PIX)*3);
		hourpix.s = fadepoint;

		fadepoint = ((float)hourpix.s)/((float)minpix.s+(float)hourpix.s)*255;

		blendedpix = blend(minpix, hourpix, fadepoint);

		leds[N_PIX-j-1] = blendedpix;
		//Serial.println(addedpix[j]);

	}
}


void fadefacedark(uint16_t hour_pos, uint16_t min_pos){
  //similar to fadeface, but this will fade to dark rather than desaturate to white.
  fract8 fadepoint;
  fract8 midpoint = 128;
  CHSV hourpix;
  CHSV minpix;
  CHSV blendedpix;
  float fadestepsize = N_PIX/255;
  minpix = Minute_Colour;
  hourpix = Hour_Colour;


  for(int j=0; j<N_PIX; j++){

    //calculate what the minute pixel should look like
    //
    fadepoint = 255-min(255,((min_pos+j)%N_PIX)*3);
    minpix.v = fadepoint;
    //same thing for the hour pixel
    fadepoint = 255-min(255, ((hour_pos+j)%N_PIX)*3);
    hourpix.v = fadepoint;

    fadepoint = ((float)hourpix.v)/((float)minpix.v+(float)hourpix.v)*255;

    blendedpix = blend(minpix, hourpix, fadepoint);

    leds[N_PIX-j-1] = blendedpix;
    //Serial.println(addedpix[j]);

  }
}

void chsvface(uint16_t hour_pos, uint16_t min_pos) {
	
  //this face will find an interpolated mid-point between the hue of the two colours, keeping the entire face saturated and colourful. 
  fract8 fadepoint;
	//CHSV blendedpix;
	int minstr;
	int hourstr;
  int handone;
  int handtwo;
  int sweepone;
  CRGB temppix;
  hour_pos = N_PIX-hour_pos-1;
  min_pos = N_PIX-min_pos-1;

	for(int j=0; j<N_PIX; j++){

	 	hourstr = (N_PIX+j-hour_pos)%N_PIX;
	 	minstr = (N_PIX+j-min_pos)%N_PIX;
    fadepoint= 255-255*hourstr/(hourstr+minstr);
    //blendedpix = 

	  leds[N_PIX-1-j] = blend(Minute_Colour, Hour_Colour, fadepoint);
	            
    }



}

void blockface(uint16_t hour_pos, uint16_t min_pos) {
//this face colours the clock in 2 sections, the c1->c2 divide represents the minute hand and the c2->c1 divide represents the hour hand.
      CHSV c1;
      CHSV c2;
      int firsthand = min(hour_pos, min_pos);
      int secondhand = max(hour_pos, min_pos);
    //check which hand is first, so we know what colour the 0 pixel is

    if(hour_pos>min_pos){
      for(int i=0; i<3; i++){
        c2 = Hour_Colour;
        c1 = Minute_Colour;    
      }
      

    }
    else
    {
      for(int i=0; i<3; i++){
        c1 = Hour_Colour;
        c2 = Minute_Colour;    
      }
    }

    //loop the first colour until we hit the first hand
    // for(int i=0; i<firsthand; i++){
    //   leds[i]=c1;
    // }
    //loop the second colour to the second hand
    for(int i=firsthand; i<secondhand; i++){
      leds[i]=c2;      
    }
    //and the last hand
    for(int i=secondhand; i<N_PIX+firsthand; i++){
      leds[i%120]=c1;
    }
}

void epiphanyface(uint16_t hour_pos, uint16_t min_pos) {
//this face colours the clock in 2 sections, the c1->c2 divide represents the minute hand and the c2->c1 divide represents the hour hand.
      CHSV c1;
      CHSV c1blend;
      CHSV c2;
      CHSV c2blend;
      int gap;
      int blendpoint = 120;
      int firsthand = min(hour_pos, min_pos);
      int secondhand = max(hour_pos, min_pos);
    //check which hand is first, so we know what colour the 0 pixel is

    if(hour_pos>min_pos){       
        c2 = Hour_Colour;
        c1 = Minute_Colour;         
    }
    else
    {
        c1 = Hour_Colour;
        c2 = Minute_Colour;         
    }

    c1blend = blend(c1, c2, blendpoint);
    c2blend = blend(c2, c1, blendpoint);

    gap = secondhand - firsthand;

    //create the blend between first and 2nd hand
    for(int i=firsthand; i<secondhand; i++){
      leds[i]=blend(c2blend, c2, ((float)i-(float)firsthand)/(float)gap*255);      
    }
    gap = 120 - gap;
    //and the last hand
    for(int i=secondhand; i<N_PIX+firsthand; i++){
      leds[i%120]=blend(c1blend, c1, ((float)i-(float)secondhand)/(float)gap*255);
    }
    leds[hour_pos]=Hour_Colour;
    leds[min_pos]=Minute_Colour;
}

void brianface(uint16_t hour_pos, uint16_t min_pos) {
//this face starts each "hand" at the 12 o'clock position and overlays the shorter over the longer. The pixels between the second hand and midday going clockwise are blank.
      CHSV c1;
      CHSV c2;
      int firsthand = min(hour_pos, min_pos);
      int secondhand = max(hour_pos, min_pos);
    //check which hand is first, so we know what colour the 0 pixel is

    if(hour_pos>min_pos){
      for(int i=0; i<3; i++){
        c2 = Hour_Colour;
        c1 = Minute_Colour;    
      }
      

    }
    else
    {
      for(int i=0; i<3; i++){
        c1 = Hour_Colour;
        c2 = Minute_Colour;    
      }
    }

    //loop the first colour until we hit the first hand
    for(int i=0; i<firsthand; i++){
      leds[i]=c1;
    }
    //loop the second colour to the second hand
    for(int i=firsthand; i<secondhand; i++){
      leds[i]=c2;      
    }
    //and the last hand we just turn all the LEDs off.
    for(int i=secondhand; i<N_PIX; i++){
      leds[i]=0x000000;
    }
}

void simpleface(uint16_t hour_pos, uint16_t min_pos) {
//a face that just lights up a single pixel for the hour and minute hands.
for(int i=0; i<N_PIX; i++){
  leds[i]=0x000000;
}

leds[hour_pos]= Hour_Colour;
leds[min_pos]= Minute_Colour;

}

void demofacemins(uint16_t hour_pos, uint16_t min_pos){
//this face is to demonstrate how to read the time, it shows only the minutes
  fract8 fadepoint;
  fract8 midpoint = 128;
  //CHSV hourpix;
  CHSV minpix;
  //CHSV blendedpix;
  //float fadestepsize = N_PIX/255;
  minpix = Minute_Colour;
  //hourpix = Hour_Colour;


  for(int j=0; j<N_PIX; j++){

    //calculate what the minute pixel should look like
    //
    fadepoint = 255-min(255,((min_pos+j)%N_PIX)*3);
    minpix.v = fadepoint;
    //same thing for the hour pixel
    //fadepoint = 255-min(255, ((hour_pos+j)%N_PIX)*3);
    //hourpix.v = fadepoint;

    //fadepoint = ((float)hourpix.v)/((float)minpix.v+(float)hourpix.v)*255;

    //blendedpix = blend(minpix, hourpix, fadepoint);

    leds[N_PIX-j-1] = minpix;
    //Serial.println(addedpix[j]);

  }
}
void rainbowface(uint16_t hour_pos, uint16_t min_pos) {
//this face starts each "hand" at the 12 o'clock position and overlays the shorter over the longer. The pixels between the second hand and midday going clockwise are blank.
  CHSV rainbowcolour = CHSV(0,255,255);
  rainbowcolour.h += 50 * (millis()%1000)/200;


    for(int i=0; i<min_pos; i++){
      rainbowcolour.h += 255/(float)min_pos;
      rainbowcolour.h %=255;
      leds[i]=rainbowcolour;
    }

  for(int i=min_pos; i<N_PIX; i++){
    leds[i]=0x000000;
      
  }

  leds[hour_pos] = - leds[hour_pos];
    
}





void demofacehours(uint16_t hour_pos, uint16_t min_pos){
//this face is to demonstrate how to read the time, it shows only the hours
  fract8 fadepoint;
  fract8 midpoint = 128;
  CHSV hourpix;
  //CHSV minpix;
  //CHSV blendedpix;
  //float fadestepsize = N_PIX/255;
  //minpix = Minute_Colour;
  hourpix = Hour_Colour;


  for(int j=0; j<N_PIX; j++){

    //calculate what the minute pixel should look like
    //
    //fadepoint = 255-min(255,((min_pos+j)%N_PIX)*3);
    //minpix.v = fadepoint;
    //same thing for the hour pixel
    fadepoint = 255-min(255, ((hour_pos+j)%N_PIX)*3);
    hourpix.v = fadepoint;

    //fadepoint = ((float)hourpix.v)/((float)minpix.v+(float)hourpix.v)*255;

    //blendedpix = blend(minpix, hourpix, fadepoint);

    leds[N_PIX-j-1] = hourpix
    ;
    //Serial.println(addedpix[j]);

  }
}
void colourchange(int colourset){
//uses a button (typically) to update the colour of the "hands"
  switch (colourset) {
      case 0:
        Hour_Colour= CHSV(50,255,255);
        Minute_Colour= CHSV(160,255,255);
        Second_Colour= CHSV(0, 0, 255);
        break;
      case 1:
        Hour_Colour= CHSV(0,255,255);
        Minute_Colour= CHSV(64,255,255);
        Second_Colour= CHSV(0, 0, 255);
        break;
      case 2:
        Hour_Colour= CHSV(0,255,255);
        Minute_Colour= CHSV(160,255,255);
        Second_Colour= CHSV(0, 0, 255);
        break;
      case 3:
        Hour_Colour= CHSV(128,0,255);
        Minute_Colour= CHSV(160,255,255);
        Second_Colour= CHSV(0, 0, 255);
        break;
      
  }


}