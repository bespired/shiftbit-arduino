/*
  Shift Register Example
 for two 74HC595 shift registers

 This sketch turns on each of the LEDs attached to two 74HC595 shift registers,
 in sequence from output 0 to output 15.

 Hardware:
 * 2 74HC595 shift register attached to pins 2, 3, and 4 of the Arduino,
 as detailed below.
 * LEDs attached to each of the outputs of the shift register

 Created 22 May 2009
 Modified 23 Mar 2010
 by Tom Igoe

 */

//Pin connected to latch pin (ST_CP) of 74HC595
const int latchPin = 10;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = 12;
////Pin connected to Data in (DS) of 74HC595
const int dataPin = 11;

char inputString[2];

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
  
 }


void setBytes( byte g, byte r, byte b ) 
{
  // | RGB RGB RG | B RGB RGB R | GB RGB RGB |
  // |  * | * |   *  | * | * |  *   | * | *  |
 
 
   byte b1 = 0;
   byte b2 = 0;
   byte b3 = 0;
   
   b3 |= gbit( b,1 ) << 0;
   b3 |= gbit( r,1 ) << 1;
   b3 |= gbit( g,1 ) << 2;
   b3 |= gbit( b,2 ) << 3;
   b3 |= gbit( r,2 ) << 4;
   b3 |= gbit( g,2 ) << 5;
   b3 |= gbit( b,3 ) << 6;
   b3 |= gbit( r,3 ) << 7;
 
   b2 |= gbit( g,3 ) << 0;
   b2 |= gbit( b,4 ) << 1;
   b2 |= gbit( r,4 ) << 2;
   b2 |= gbit( g,4 ) << 3;
   b2 |= gbit( b,5 ) << 4;
   b2 |= gbit( r,5 ) << 5;
   b2 |= gbit( g,5 ) << 6;
   b2 |= gbit( b,6 ) << 7;
  
   b1 |= gbit( r,6 ) << 0;
   b1 |= gbit( g,6 ) << 1;
   b1 |= gbit( b,7 ) << 2;
   b1 |= gbit( r,7 ) << 3;
   b1 |= gbit( g,7 ) << 4;
   b1 |= gbit( b,8 ) << 5;
   b1 |= gbit( r,8 ) << 6;
   b1 |= gbit( g,8 ) << 7;
  
  digitalWrite( latchPin , LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 255-b1 );
  shiftOut(dataPin, clockPin, MSBFIRST, 255-b2 );
  shiftOut(dataPin, clockPin, MSBFIRST, 255-b3 );
  digitalWrite(latchPin, HIGH);
  
}

byte gbit( byte b, int bitidx ){
  byte bitje=  ( b & ( 1 << bitidx-1 ) ) != 0 ? 1 : 0;
  return bitje;
}

void wave(){
  for( byte r = 0; r < 5; r++ ){
    int w = 100 + 50 * sin( r / 57 );
    setBytes( 0b01100000, 0b01000000, 0b11000000 ); delay(w);
    setBytes( 0b00110000, 0b00100000, 0b01100000 ); delay(w);
    setBytes( 0b00011000, 0b00010000, 0b00110000 ); delay(w);
    setBytes( 0b00001100, 0b00001000, 0b00011000 ); delay(w);
    setBytes( 0b00000110, 0b00000100, 0b00001100 ); delay(w);
    setBytes( 0b00000011, 0b00000010, 0b00000110 ); delay(w);
    setBytes( 0b00000110, 0b00000100, 0b00001100 ); delay(w);
    setBytes( 0b00001100, 0b00001000, 0b00011000 ); delay(w);
    setBytes( 0b00011000, 0b00010000, 0b00110000 ); delay(w);
    setBytes( 0b00110000, 0b00100000, 0b01100000 ); delay(w);
  }
}

void vlag(){
  
    setBytes( 0b00011111, 0b00011000, 0b11111000 ); delay(2000);
  
}

void vlaggy(){
  for( byte r = 0; r < 5; r++ ){
    setBytes( 0b00111100, 0b00110000, 0b11110000 ); delay(200);
    setBytes( 0b00011110, 0b00011000, 0b01111000 ); delay(200);
    setBytes( 0b00001111, 0b00001100, 0b00111100 ); delay(200);
  } 
}

void oranje(){
  
  byte w = 0b11100000;
 
   for( byte l=0; l < 6; l++){
     for( int r = 0; r < 75; r++ ){
      setBytes( w,0,0 ); delay(4);
      setBytes( w, w, 0 ); delay(1);
    }
    w = w >> 1;
  }
}


void roodwitblauw(){
  setBytes( 255, 0, 0 );
  delay(400);
  
  setBytes( 255, 255, 255 );
  delay(400);
  
  setBytes( 0, 0, 255 );
  delay(400);
}

void loop() {
  
  byte r = random( 5 );
  switch( r )
  {
    case 0:  
      wave();
    break;
    case 1:
      roodwitblauw();
    break;
    case 2:
      vlag();
    break;
    case 3:
      vlaggy();
    break;
    
    case 4:
      oranje();
    break;  
}
  

}
