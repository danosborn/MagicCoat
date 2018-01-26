
// A program for controlling the leds in a "magic" coat
// Feature inclued - light twinkle pattern

//#define DEBUG 1

#include "FastLED.h"
#include <avr/pgmspace.h>

// The coat has two zones
// The lower is composed of 23 strips, each 19 leds long = 437 Leds
// There's 5 in the right & left front, 13 across the back

#define L_LED_LENGTH 19
#define L_NUM_STRIPS 23

#define L_NUM_LEDS (L_LED_LENGTH*L_NUM_STRIPS)

//
// The upper has 6 across the front - 3 on each side.
// The left front is 13, 14, & 13 at the sleave.  The right is same
// The each arm has 2 @ 8 long
// The back has 7:  13, 14, 14, 14, 14, 14, 13
// Total upper is 208
#define U_NUM_LEDS_RAW ((13+14+13)+(8+8)+(13+14+14)+14+(14+14+13)+(8+8)+(13+14+13))

// For convience define a 2 dimensional map using the longest dimensions
#define U_LED_LENGTH 14
#define U_NUM_STRIPS 17
#define U_NUM_LEDS (U_LED_LENGTH*U_NUM_STRIPS)


// Define Fast LED controller parameters
// There's not enough memory for all ~650 LEDs, so it's necessary to control each strand individually

// Data pins
#define U_DATA_PIN 4
#define L_DATA_PIN 5
#define GBRIGHTNESS  128    // Default value, needed for showLeds() function

CLEDController *u_cntr;
CLEDController *l_cntr;

// Define the array of leds - User lower as it is biggest
CRGB leds[L_NUM_LEDS];




// Data Structures for Tinkling array;
struct ledstate {
  int position;
  int time;
};

#define NUM_LEDS_TWINKLE 3

ledstate upperTwinkleArray[NUM_LEDS_TWINKLE];
ledstate lowerTwinkleArray[NUM_LEDS_TWINKLE];

//Time
unsigned long previousTime = millis();









/*******************************************************************
   Define and Initialize the Sparkle background
*/

// Min values to trun on any LED is 2.
// Blue is a really dim at 2, use 3
#define LED_DIM_VALUE 4
#define LED_BRIGHT_VALUE 32

const byte lower_dim_background [L_NUM_LEDS] PROGMEM = {
  0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1,  // Row 1
  1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1,  // Row 2
  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,  // Row 3
  0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,  // Row 4
  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,  // Row 5
  1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,  // Row 6
  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1,  // Row 7
  0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,  // Row 8
  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1,  // Row 9
  1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,  // Row 10
  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,  // Row 11
  0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,  // Row 12
  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,  // Row 13
  0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,  // Row 14
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1,  // Row 15
  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,  // Row 16
  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,  // Row 17
  0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,  // Row 18
  1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1,  // Row 19
  0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,  // Row 20
  0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,  // Row 21
  0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,  // Row 22
  0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1   // Row 23  Always have an led on in last pos for twinkle
};


void Init_lower_background_array() {

  fill_solid(leds, L_NUM_LEDS, 0);   // Clear array

  CRGB *lptr = &leds[0];
  const byte *bptr = &lower_dim_background[0];

  for (int i = 0; i < L_NUM_LEDS; i++) {
    byte dim = pgm_read_byte_near(bptr);   //
    if (dim != 0) {
      lptr->b = LED_DIM_VALUE;
    }
    bptr++;
    lptr++;
  }
}


/****************************************************************************
   For the upper jacket, we need to map the led location with a 17x14
   Array for easier control
*/

const int LEDmap [] PROGMEM = {
  -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,  // left front: 13+14+13 = 40
  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
  -1, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
  -1, -1, 40, 41, 42, 43, 44, 45, 46, 47, -1, -1, -1, -1,  // left arm: 8+8 = 16, 56 total
  -1, -1, 48, 49, 50, 51, 52, 53, 54, 55, -1, -1, -1, -1,
  -1, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68,  // left back: 13+14+14 = 41, 97 total
  69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82,
  83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96,
  97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, // center back: 14 = 14, 111 total
  111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, // right back: 14+14+13 = 41, 152 total
  125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138,
  -1, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151,
  -1, -1, 152, 153, 154, 155, 156, 157, 158, 159, -1, -1, -1, -1, // right arm: 8+8 = 16, 168 total
  -1, -1, 160, 161, 162, 163, 164, 165, 166, 167, -1, -1, -1, -1,
  -1, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, // right front: 13+14+13 = 40, 208 total
  181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
  -1, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207
};


const byte upper_dim_background_raw [U_NUM_LEDS_RAW] PROGMEM = {
  0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0,   // left front: 13+14+13 = 40
  1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
  1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0,
  1, 0, 0, 1, 0, 0, 0, 1,               // left arm: 8+8 = 16, 56 total
  0, 1, 0, 0, 1, 0, 1, 0,
  0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0,   // left back: 13+14+14 = 41, 97 total
  1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
  0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0,   // center back: 14 = 14, 111 total
  1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1,    // right back: 14+14+13 = 41, 97 total
  0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
  1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1,
  0, 1, 0, 0, 1, 0, 1, 0,               // right arm: 8+8 = 16, 56 total
  1, 0, 0, 1, 0, 0, 0, 1,
  0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1,   // right front: 13+14+13 = 40
  0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1          // Always have an led on in last pos for twinkle
};


const byte upper_dim_background [] PROGMEM = {
  0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0,   // left front: 13+14+13 = 40
  1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
  0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0,
  0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,   // left arm: 8+8 = 16, 56 total
  0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0,   // left back: 13+14+14 = 41, 97 total
  1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
  0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0,   // center back: 14 = 14, 111 total
  1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1,    // right back: 14+14+13 = 41, 97 total
  0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
  0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1,
  0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,   // right arm: 8+8 = 16, 56 total
  0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1,   // right front: 13+14+13 = 40
  0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0      // Always have an led on in last pos for twinkle
};

void Init_upper_background_array() {

  fill_solid(leds, U_NUM_LEDS_RAW, 0);   // Clear array

  CRGB *lptr = &leds[0];
  const byte *bptr = &upper_dim_background_raw[0];

  for (int i = 0; i < U_NUM_LEDS_RAW; i++) {
    byte dim = pgm_read_byte_near(bptr);   //
    if (dim != 0) {
      lptr->b = LED_DIM_VALUE;
    }
    bptr++;
    lptr++;
  }
}

/****************************************************************
   Twinkle some of the
*/
/***************************************************************

  /****************************************************************
  /  Make a circle/triangle burst pattern on the lower 1/2 of coat
  /
  /  Leds have 3.33cm (1.3in) vertical spacing.  The have ~5cm (at waist) spacing.
  /  Let's use 2 to 1, go up 2 leds to for 1 vertical row.
  /  Spread the burst across 3 rows.   We'll go out to 5 on the center row and 4 on the side.
  /  Want to bust out over 1/2 second and fade to zero at 2 seconds
  /***************************************************************/

#define BURST_BRIGHTNESS  32
//#define BURST_TIME 2000    // 1000 milliseconds
#define BURST_TIME 300
#define BURST_SPREAD 500    // 500 milliseconds
#define SPREAD_DISTANCE 4   // +/- 4 LEDs vertically
//#define SPREAD_INC (BURST_SPREAD/SPREAD_DISTANCE)
#define SPREAD_INC 75
#define BURST_WIDTH 350
#define VERT_SPREAD  50    // Additional delay on adjcent rows

struct burst_led {
  int position;
  int delay;
};



void burst_lower(int location) {

  Init_lower_background_array();

  for (int i = 0; i < 3; i++) {
    leds[location + i].g = BURST_BRIGHTNESS;
    leds[location + i].b = 0;
    leds[location - i].g = BURST_BRIGHTNESS;
    leds[location - i].b = 0;

    l_cntr->showLeds(GBRIGHTNESS);
    delay(SPREAD_INC);
  }
  delay(SPREAD_INC);
  Init_lower_background_array();
  l_cntr->showLeds(GBRIGHTNESS);
}

void burst_upper(int location) {

  Init_upper_background_array();

  for (int i = 0; i < 3; i++) {
    leds[location + i].g = BURST_BRIGHTNESS;
    leds[location + i].b = 0;
    leds[location - i].g = BURST_BRIGHTNESS;
    leds[location - i].b = 0;

    u_cntr->showLeds(GBRIGHTNESS);
    delay(SPREAD_INC);
  }
  delay(SPREAD_INC);
  Init_upper_background_array();
  u_cntr->showLeds(GBRIGHTNESS);
}

/*
This code was to crate a burst across strips, but was abandon in favor of a simple up and down a single strip
 
const burst_led burst_pattern[] PROGMEM = {
  { 0, 0},
  { 1, SPREAD_INC},
  {  1, SPREAD_INC},
  { 2, 2 * SPREAD_INC},
  { -2, 2 * SPREAD_INC},
  {  0, -1 },
  { L_LED_LENGTH, 2 * SPREAD_INC + VERT_SPREAD},
  { -L_LED_LENGTH, 2 * SPREAD_INC + VERT_SPREAD},
  { 3, 3 * SPREAD_INC},
  { -3, 3 * SPREAD_INC},
  { L_LED_LENGTH + 1, 3 * SPREAD_INC + VERT_SPREAD},
  { L_LED_LENGTH - 1, 3 * SPREAD_INC + VERT_SPREAD},
  { -L_LED_LENGTH + 1 , 3 * SPREAD_INC + VERT_SPREAD},
  { -L_LED_LENGTH - 1 , 3 * SPREAD_INC + VERT_SPREAD},
  { 4, 4 * SPREAD_INC},
  { -4, 4 * SPREAD_INC},
  { L_LED_LENGTH + 2, 4 * SPREAD_INC + VERT_SPREAD},
  { L_LED_LENGTH - 2, 4 * SPREAD_INC + VERT_SPREAD},
  { -L_LED_LENGTH + 2, 4 * SPREAD_INC + VERT_SPREAD},
  { -L_LED_LENGTH - 2, 4 * SPREAD_INC + VERT_SPREAD},
  { L_LED_LENGTH * 2, 4 * SPREAD_INC + 2 * VERT_SPREAD},
  { -L_LED_LENGTH * 2, 4 * SPREAD_INC + 2 * VERT_SPREAD},
  { L_LED_LENGTH + 3, 5 * SPREAD_INC + VERT_SPREAD},
  { L_LED_LENGTH - 3, 5 * SPREAD_INC + VERT_SPREAD},
  { -L_LED_LENGTH + 3, 5 * SPREAD_INC + VERT_SPREAD},
  { -L_LED_LENGTH - 3, 5 * SPREAD_INC + VERT_SPREAD},
  { 0, -1}                // Use a delay of -1 as an end marker
};

void burst_lower(int location) {

  unsigned long startTime = millis();
  unsigned int time;
  boolean change = false;

  Init_lower_background_array();

  do {                              // loop over the total fade time
    time = millis() - startTime;

    const burst_led *ptr = &burst_pattern[0];
    int delay;
    int position;

    delay = pgm_read_word_near(&(*ptr).delay);

    while (delay >= 0) {       // loop over all burst pattern, -1 marks the end
      if (time >= delay) {     // Only update LEDs that have passed the delay time

        byte new_value = (time >= delay + BURST_WIDTH) ? 0 : BURST_BRIGHTNESS;
        int pos = location + pgm_read_word_near( &(*ptr).position);

        if (new_value != leds[pos].g) { // New value?
          leds[pos].g = new_value;
          leds[pos].b = 0;              // Make sure background blue is off
          change = true;
        }
      }
      ptr++;
      delay = pgm_read_word_near(&(*ptr).delay);
    }
    if (change == true) {
      l_cntr->showLeds(GBRIGHTNESS);
      change = false;
    }
  } while (time < BURST_TIME);
}
*/

 
#define MIN_BURST_TIME 1000
#define MAX_BURST_TIME 3000

int lower_burst_delay;
int upper_burst_delay;

void burst (unsigned int delay)
{

  lower_burst_delay -= delay;
  if ( lower_burst_delay <= 0) {
    int position = random16(1, L_NUM_STRIPS - 1) * L_LED_LENGTH + random16(3, L_LED_LENGTH - 3);
    burst_lower(position);
    lower_burst_delay = random16(MIN_BURST_TIME, MAX_BURST_TIME);
  }

 
    upper_burst_delay -= delay;
    if( upper_burst_delay <= 0){
      int position = random16(0, U_NUM_STRIPS-1)*U_LED_LENGTH + random16(3,6);
      position = pgm_read_word_near(LEDmap + position);
      burst_upper(position);
      upper_burst_delay = random16(MIN_BURST_TIME, MAX_BURST_TIME);
    }

}


/***********************************************************************************
   Rain (ripple) down the coat, turning on all the background LEDs in the processes
*/

#define RAIN_VALUE 16
#define RAIN_DELAY 50   // Delay in msec between rows

void RainAllStrands()    // Initial the magic field on the coat from top to bottom
{
  RainUpperStrands();
  RainLowerStrands();
}


// Rain down the upper leds
void  RainUpperStrands() {
  fill_solid(leds, U_NUM_LEDS_RAW, 0x0);

  for (int i = 0; i < U_LED_LENGTH; i++) {

    const int *ptr = &LEDmap[i];

    for (int j = 0; j < U_NUM_STRIPS; j++) {
      int index = pgm_read_word_near(ptr);
      if (index >= 0)
        leds[index].b = RAIN_VALUE;
      ptr += U_LED_LENGTH;   // Go to next strip
    }

    u_cntr->showLeds(GBRIGHTNESS);  // FastLED.show();
    delay(RAIN_DELAY);

    // Set the background as we rain down

    ptr = &LEDmap[i];
    const byte *bptr = &upper_dim_background[i];
    for (int j = 0; j < U_NUM_STRIPS; j++) {
      int index = pgm_read_word_near(ptr);
      if (index >= 0) {
        byte dim = pgm_read_byte_near(bptr);
        leds[index].b = (dim != 0) ? LED_DIM_VALUE : 0;
      }
      ptr += U_LED_LENGTH;   // Go to next strip
      bptr += U_LED_LENGTH;
    }

  }
  u_cntr->showLeds(GBRIGHTNESS);   // FastLED.show();           // Turn off last LED
  delay(RAIN_DELAY);
}

// Rain down the lower leds
void RainLowerStrands() {
  fill_solid(leds, L_NUM_LEDS, 0x0);

  for (int i = 0; i < L_LED_LENGTH; i++)
  {
    CRGB *ptr = &leds[i];
    for (int j = 0; j < L_NUM_STRIPS; j++) {
      ptr->b = RAIN_VALUE;
      ptr += L_LED_LENGTH;     // Go to next strip
    }

    l_cntr->showLeds(GBRIGHTNESS);   // FastLED.show();
    delay(RAIN_DELAY);

    // Set the background as we rain down

    ptr = &leds[i];
    const byte *bptr = &lower_dim_background[i];
    for (int j = 0; j < L_NUM_STRIPS; j++) {
      byte dim = pgm_read_byte_near(bptr);

      ptr->b = (dim != 0) ? LED_DIM_VALUE : 0;
      ptr += L_LED_LENGTH;     // Go to next strip
      bptr += L_LED_LENGTH;

    }
  }
  l_cntr->showLeds(GBRIGHTNESS);   // FastLED.show();          // Turn off last LED
  delay(RAIN_DELAY);
}

/***********************************************************************************
   Fire up down the coat, turning off  LEDs in the processes
*/

void FireAllStrands()    // Initial the magic field on the coat from top to bottom
{
  FireLowerStrands();
  FireUpperStrands();
}

// Fire up the lower leds
void FireLowerStrands() {

  Init_lower_background_array();
  for (int i = L_LED_LENGTH - 1; i >= 0; i--)
  {
    CRGB *ptr = &leds[i];
    for (int j = 0; j < L_NUM_STRIPS; j++) {
      ptr->b = 0;
      ptr->r = RAIN_VALUE;
      ptr += L_LED_LENGTH;     // Go to next strip
    }

    l_cntr->showLeds(GBRIGHTNESS);   // FastLED.show();
    delay(RAIN_DELAY);

    // Clear the background as we fire up
    ptr = &leds[i];
    for (int j = 0; j < L_NUM_STRIPS; j++) {
      ptr->r = 0;
      ptr += L_LED_LENGTH;     // Go to next strip
    }
  }
  l_cntr->showLeds(GBRIGHTNESS);   // FastLED.show();          // Turn off last LED
  delay(RAIN_DELAY);
}


// Fire up the upper leds
void  FireUpperStrands() {
  Init_upper_background_array();

  for (int i = U_LED_LENGTH - 1; i >= 0; i--) {

    const int *ptr = &LEDmap[i];

    for (int j = 0; j < U_NUM_STRIPS; j++) {
      int index = pgm_read_word_near(ptr);
      if (index >= 0){
        leds[index].b = 0;
        leds[index].r = RAIN_VALUE;
      }
      ptr += U_LED_LENGTH;   // Go to next strip
    }

    u_cntr->showLeds(GBRIGHTNESS);  // FastLED.show();
    delay(RAIN_DELAY);

    // Clear the background as we fire up

      ptr = &LEDmap[i];
      for (int j = 0; j < U_NUM_STRIPS; j++) {
      int index = pgm_read_word_near(ptr);
      if (index >= 0) {
        
        leds[index].r = 0;
      }
      ptr += U_LED_LENGTH;   // Go to next strip
    }

  }
  u_cntr->showLeds(GBRIGHTNESS);   // FastLED.show();           // Turn off last LED
  delay(RAIN_DELAY);
}
/***********************************************************************************
   Twinkle some of the background LEDs
   Pick an led and brighten it for 0.5 to 1.5 seconds
*/

#define MIN_TWINKLE_TIME  500
#define MAX_TWINKEL_TIME 1500

void twinkle(unsigned int delay)
{
  bool change = false;
  // Update twinkle for upper leds
  for (int i = 0; i < NUM_LEDS_TWINKLE; i++) {
    upperTwinkleArray[i].time -= delay;
    if ( upperTwinkleArray[i].time <= 0) {
      upperTwinkleArray[i].position = pickLed(upper_dim_background, U_NUM_LEDS);
      upperTwinkleArray[i].time = random16(MIN_TWINKLE_TIME, MAX_TWINKEL_TIME);
      change = true;

#ifdef DEBUG
      Serial.println("Upper Twinkle");
      Serial.println(upperTwinkleArray[i].position);
      Serial.println(upperTwinkleArray[i].time);
      Serial.println(" ");
#endif
    }
  }

  // update upper coat

  if (change == true) {
    change = false;
    Init_upper_background_array();
    for (int i = 0; i < NUM_LEDS_TWINKLE; i++) {
      int index = pgm_read_word_near(LEDmap + upperTwinkleArray[i].position);
      leds[index].b = LED_BRIGHT_VALUE;
    }
    u_cntr->showLeds(GBRIGHTNESS);   // FastLED.show();
  }

  // Update twinkle for lower leds
  for (int i = 0; i < NUM_LEDS_TWINKLE; i++) {
    lowerTwinkleArray[i].time -= delay;
    if ( lowerTwinkleArray[i].time <= 0) {
      //      lowerTwinkleArray[i].position = pickLed(lower_dim_background, L_NUM_LEDS);
      lowerTwinkleArray[i].time = random16(MIN_TWINKLE_TIME, MAX_TWINKEL_TIME);
      change = true;

#ifdef DEBUG
      Serial.println("Lower Twinkle");
      Serial.println(lowerTwinkleArray[i].position);
      Serial.println(lowerTwinkleArray[i].time);
      Serial.println(" ");
#endif
    }
  }
  if (change == true) {
    change = false;
    Init_lower_background_array();
    for (int i = 0; i < NUM_LEDS_TWINKLE; i++) {
      leds[lowerTwinkleArray[i].position].b = LED_BRIGHT_VALUE;
    }
    l_cntr->showLeds(GBRIGHTNESS);   // FastLED.show();
  }
}


int pickLed(const byte background[], int size) {
  int pos = random16(0, size - 1);
  byte dim;

  do {
    dim = pgm_read_byte_near(background + pos);
    if (dim == 0)
      pos++;
  }  while (dim == 0);

  return pos;
}


void setup() {

  u_cntr = &FastLED.addLeds<WS2812B, U_DATA_PIN, GRB>(leds, U_NUM_LEDS_RAW);
  l_cntr = &FastLED.addLeds<WS2812B, L_DATA_PIN, GRB>(leds, L_NUM_LEDS);

  // Initialize the Twinkle Array
  for (int i = 0; i < NUM_LEDS_TWINKLE; i++) {
    upperTwinkleArray[i].time = 0;
    lowerTwinkleArray[i].time = 0;
  }


  lower_burst_delay = random16(MIN_BURST_TIME, MAX_BURST_TIME);
  upper_burst_delay = random16(MIN_BURST_TIME, MAX_BURST_TIME);
  
#ifdef DEBUG
  Serial.begin(9600);
#endif
}


void loop() {

#ifdef DEBUG
  Serial.println("Starting");
#endif

  RainAllStrands();


  int seconds = 0;
  int mills = 0;
  while (seconds < 150) {


    unsigned long currentTime = millis();
    unsigned int milliseconds = currentTime - previousTime;
    mills += milliseconds;
    previousTime = currentTime;


    twinkle(milliseconds);
    burst(milliseconds);

    if ( mills >= 1000) { // 1000 milliseconds has passed
      seconds++;
      mills -= 1000;
#ifdef DEBUG
      Serial.println("1 sec passed");
#endif
    }
  } // end of seconds loop - 2.5 minutes
  
  FireAllStrands();
}



