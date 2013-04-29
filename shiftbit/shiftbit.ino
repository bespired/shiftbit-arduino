#include <avr/io.h>
#include <avr/interrupt.h>
 
// Check if your pins have the same connection:
// Pin connected to latch pin (ST_CP) of 74HC595
const int latchPin = 10;
// Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = 12;
// Pin connected to Data in (DS) of 74HC595
const int dataPin  = 11;

// Indexed colors for the rgb LEDS
// LEDS are blinking very fast to dim brightness and mix to colors.
//                    frame1, frame2, frame3, frame4, frame5, extra byte. 
//                    R G B   R G B   R G B   R G B   R G B   +
const byte sets[] = { 0,0,0 , 0,0,0 , 0,0,0 , 0,0,0 , 0,0,0 , 0, // rgb's for off
                      0,0,0 , 1,1,1 , 0,0,0 , 1,0,1 , 0,0,0 , 1, // rgb's for white , not on much and a little less green.
                      1,0,0 , 0,0,0 , 1,0,0 , 0,0,0 , 1,0,0 , 2, // rgb's for red
                      0,1,0 , 0,0,0 , 0,1,0 , 0,0,0 , 0,1,0 , 3, // rgb's for green
                      0,0,1 , 0,0,1 , 0,0,0 , 0,0,1 , 0,0,1 , 4, // rgb's for blue
                      1,0,0 , 1,0,0 , 0,0,0 , 1,0,0 , 1,1,0 , 5, // rgb's for orange
                      0,0,0 , 1,1,0 , 0,0,0 , 1,1,0 , 0,0,0 , 6, // rgb's for geel
                      0,0,0 , 1,0,1 , 0,0,0 , 1,0,1 , 0,0,0 , 7, // rgb's for paars
                      0,0,0 , 0,1,1 , 0,0,0 , 0,1,1 , 0,0,0 , 8, // rgb's for cyaan
};
// sets[] has 16 values in a row so we can do << 4 and not * 15,... thats all. 
// we can also make a dimmed red by 1,0,0 ,0,0,0 ,0,0,0 ,0,0,0 ,0,0,0 = 1 red, 4 off.
// addition could be to add a function that creates this set from 15bit RGB color ints.

volatile byte colframe= 0;   // 5 'frames' of 
volatile byte chip[15]= {};  // This setup has 3 595 chips and 5 'frames' to mix colors.

int  anim  = 0;
int  frames= 46;
byte LEDS[]= { 30, 0, 0, 0, 0, 0, 0, 0, 0,
               30, 5, 5, 5, 5, 5, 5, 5, 5,
               30, 2, 2, 2, 2, 2, 2, 2, 2,
               30, 1, 1, 1, 1, 1, 1, 1, 1,
               30, 4, 4, 4, 4, 4, 4, 4, 4,
               30, 0, 0, 0, 0, 0, 0, 0, 0,
               90, 5, 2, 1, 4, 5, 2, 1, 4,
               
               20, 5, 5, 1, 4, 5, 5, 1, 4,
               20, 5, 5, 5, 4, 5, 5, 5, 4,
               
               60, 5, 5, 5, 5, 5, 5, 5, 5,
               10, 5, 5, 5, 5, 5, 5, 5, 2,
               12, 5, 5, 5, 5, 5, 5, 2, 2,
               14, 5, 5, 5, 5, 5, 2, 2, 1,
               16, 5, 5, 5, 5, 2, 2, 1, 1,
               18, 5, 5, 5, 2, 2, 1, 1, 4,
               20, 5, 5, 2, 2, 1, 1, 4, 4,
               90, 5, 2, 2, 1, 1, 4, 4, 5,
               
               15, 2, 2, 1, 1, 4, 4, 5, 5,
               14, 2, 1, 1, 4, 4, 5, 5, 5,
               13, 1, 1, 4, 4, 5, 5, 5, 5,
               12, 1, 4, 4, 5, 5, 5, 5, 5,
               11, 4, 4, 5, 5, 5, 5, 5, 5,
               10, 4, 5, 5, 5, 5, 5, 5, 5,
               60, 5, 5, 5, 5, 5, 5, 5, 5,
               
               14, 0, 0, 5, 5, 5, 5, 5, 5,
               14, 0, 0, 0, 0, 0, 5, 5, 5,
               
               14, 0, 0, 0, 0, 0, 0, 0, 5,
               14, 0, 0, 0, 0, 0, 0, 5, 0,
               14, 0, 0, 0, 0, 0, 5, 0, 2,
               14, 0, 0, 0, 0, 5, 0, 2, 1,
               14, 0, 0, 0, 5, 0, 2, 1, 4,
               14, 0, 0, 5, 0, 2, 1, 4, 0,
               14, 0, 5, 0, 2, 1, 4, 0, 0,
               24, 5, 0, 2, 1, 4, 0, 0, 0,
               14, 5, 0, 0, 2, 1, 4, 0, 0, 
               24, 5, 5, 0, 0, 2, 1, 4, 0,
               14, 0, 5, 5, 2, 1, 4, 0, 0,
               14, 5, 5, 2, 1, 4, 0, 0, 0,
               24, 5, 2, 1, 4, 0, 0, 0, 0,
               14, 5, 5, 2, 1, 4, 0, 0, 0,
               24, 0, 5, 5, 2, 1, 4, 0, 0,
               14, 5, 5, 2, 1, 4, 0, 0, 0,
               14, 5, 2, 1, 4, 0, 0, 0, 0,
               14, 2, 1, 4, 0, 0, 0, 0, 0,
               14, 1, 4, 0, 0, 0, 0, 0, 0,
               14, 4, 0, 0, 0, 0, 0, 0, 0,
               40, 0, 0, 0, 0, 0, 0, 0, 0,
               10, 0, 0, 0, 0, 0, 0, 0, 0,
               10, 0, 0, 0, 0, 0, 0, 0, 0,
               
};

// indexes 0 = off, 1 = white, 2 = red, 

void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  
  digitalWrite( latchPin , LOW);
  shiftOut( dataPin, clockPin, MSBFIRST, 255 );
  shiftOut( dataPin, clockPin, MSBFIRST, 255 );
  shiftOut( dataPin, clockPin, MSBFIRST, 255 );
  digitalWrite(latchPin, HIGH);
  
  
 
 
    cli();          // disable global interrupts
    TCCR1A = 0;     // set entire TCCR1A register to 0
    TCCR1B = 0;     // same for TCCR1B
 
    // set compare match register to desired timer count:
    OCR1A = 15624;
    
    // turn on CTC mode:
    TCCR1B |= (1 << WGM12);
    // Set CS10 bit for no prescaler:
    TCCR1B |= (1 << CS10);
    // enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);
    sei();   // enable global interrupts
    
 }

ISR( TIMER1_COMPA_vect )
{

  colframe++;
  if ( colframe > 4 ) colframe = 0;
  
  digitalWrite( latchPin , LOW);
  shiftOut( dataPin, clockPin, MSBFIRST, chip[ 10+colframe ] );
  shiftOut( dataPin, clockPin, MSBFIRST, chip[  5+colframe ] );
  shiftOut( dataPin, clockPin, MSBFIRST, chip[    colframe ] );
  digitalWrite(latchPin, HIGH);
 
}

void setColors( byte led1, byte led2, byte led3, byte led4, byte led5, byte led6, byte led7, byte led8 )
{
  byte idx = 0;
  
  for ( byte c1 = 0; c1 < 5; c1++ )
  {
    byte c2 = c1 + 5;  // fill array with values that can be blitterd to the shift chips
    byte c3 = c2 + 5;  // chip 3 has array index 10-15, chip 2 has 5-9 and chip 1 has 0-4 for 5 'frames' of color mixing.
    byte mp = c1 * 3;
    idx = (led1 << 4) + mp; chip[ c1 ]  = sets[ idx + 2 ]     ; chip[ c1 ] |= sets[ idx + 1 ] << 1; chip[ c1 ] |= sets[ idx + 0 ] << 2;
    idx = (led2 << 4) + mp; chip[ c1 ] |= sets[ idx + 2 ] << 3; chip[ c1 ] |= sets[ idx + 1 ] << 4; chip[ c1 ] |= sets[ idx + 0 ] << 5;
    idx = (led3 << 4) + mp; chip[ c1 ] |= sets[ idx + 2 ] << 6; chip[ c1 ] |= sets[ idx + 1 ] << 7; chip[ c2 ]  = sets[ idx + 0 ] << 0;
    idx = (led4 << 4) + mp; chip[ c2 ] |= sets[ idx + 2 ] << 1; chip[ c2 ] |= sets[ idx + 1 ] << 2; chip[ c2 ] |= sets[ idx + 0 ] << 3;
    idx = (led5 << 4) + mp; chip[ c2 ] |= sets[ idx + 2 ] << 4; chip[ c2 ] |= sets[ idx + 1 ] << 5; chip[ c2 ] |= sets[ idx + 0 ] << 6;
    idx = (led6 << 4) + mp; chip[ c2 ] |= sets[ idx + 2 ] << 7; chip[ c3 ] =  sets[ idx + 1 ] << 0; chip[ c3 ] |= sets[ idx + 0 ] << 1;
    idx = (led7 << 4) + mp; chip[ c3 ] |= sets[ idx + 2 ] << 2; chip[ c3 ] |= sets[ idx + 1 ] << 3; chip[ c3 ] |= sets[ idx + 0 ] << 4;
    idx = (led8 << 4) + mp; chip[ c3 ] |= sets[ idx + 2 ] << 5; chip[ c3 ] |= sets[ idx + 1 ] << 6; chip[ c3 ] |= sets[ idx + 0 ] << 7;
    
    chip[ c1 ]  = 255- chip[ c1 ]; // LEDS sink to Vcc
    chip[ c2 ]  = 255- chip[ c2 ]; //
    chip[ c3 ]  = 255- chip[ c3 ]; // 
    
  }
  
}


void loop() {
  
  setColors( LEDS[anim+1], LEDS[anim+2], LEDS[anim+3], LEDS[anim+4], LEDS[anim+5], LEDS[anim+6], LEDS[anim+7], LEDS[anim+8] );
  delay( LEDS[anim] * 10 );                 // delay until next animation-frame, ISR will blitter the colorframes.
  anim+=9; if ( anim > frames*9 ) anim = 0; // ( frames as is in animation-frames. )


}
