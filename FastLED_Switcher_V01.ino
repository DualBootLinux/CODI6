/// @file    FastLED_Switcher_V01.ino
/// @brief   Switches Between LED Modes & Colors
/// @example FastLED_Switcher_V01.ino

//This line attaches the "FastLED" library to code
#include <FastLED.h>

// How many LEDS in LOGO strip?
#define NUM_LEDS_LOGO 4
// What PIN is LOGO plugged into?
#define LED_PIN_LOGO 3

// How many LEDS in FRONT strip?
#define NUM_LEDS_FRONT 16
// What PIN is FRONT plugged into?
#define LED_PIN_FRONT 5

// How many LEDS in INSIDE strip?
#define NUM_LEDS_INSIDE 87
// What PIN is INSIDE plugged into?
#define LED_PIN_INSIDE 6

// How many LEDS in FANS strip?
#define NUM_LEDS_FANS 8  //8 Per FAN copied on 3 FANS (8x3=24 LEDS)
// What PIN is FANS plugged into?
#define LED_PIN_FANS 11

//Specifically used by Fire Color_Mode
#define COOLING 55
#define SPARKING 120
#define NUM_SPARKS 3

//Varuable used for Hue is limited to 0-255 color wheel
uint8_t hue;

//Later being used for overtemp warning, uses procedure WarningLightMode() to visually tell user the computer is overheating
int temp;

uint8_t COLOR;  //COLOR//MIDDLE KNOB//GREEN WIRE//

int color_mode;  //COLOR MODE//RIGHT KNOB//BLUE WIRE//

int delay_timer;  //DELAY TIMER//LEFT KNOB//YELLOW WIRE//

int x;
//a "uint8_t" is a number between o and 255 (the color wheel)


// Define the array of leds
CRGB logo3[NUM_LEDS_LOGO];
CRGB leds5[NUM_LEDS_FRONT];
CRGB leds6[NUM_LEDS_INSIDE];
CRGB fans11[NUM_LEDS_FANS];

void setup() {
  //Defines arrays in FastLED
  FastLED.addLeds<WS2812B, LED_PIN_LOGO, GRB>(logo3, NUM_LEDS_LOGO);      // GRB ordering is assumed
  FastLED.addLeds<WS2812B, LED_PIN_FRONT, GRB>(leds5, NUM_LEDS_FRONT);    // GRB ordering is assumed
  FastLED.addLeds<WS2812B, LED_PIN_INSIDE, GRB>(leds6, NUM_LEDS_INSIDE);  // GRB ordering is assumed
  FastLED.addLeds<WS2812B, LED_PIN_FANS, GRB>(fans11, NUM_LEDS_FANS);     // GRB ordering is assumed

  //ALL LEDS: Sets overall brightness to 85% out of %100 for LED longevity
  FastLED.setBrightness(85);
  //ALL LEDS: Sets color correction depending on LED strip brand/type/colortempeture
  FastLED.setCorrection(TypicalPixelString);

  //Sets up Serial Monitor
  Serial.begin(9600);
}

//VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV START OF LOOP VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV//
void loop() {

  temp = 70;
  x = 80;

  //oox//<---6 --->1//oox//
  //COLOR MODE//RIGHT KNOB//BLUE WIRE//
  //{7/1023}+1 = {1 -> 7} MODES//0.006842 FOR 7 MODES//REDO MATH FOR MORE MODES//
  color_mode = ((analogRead(A0) * 0.006842) + 1);  //((analogRead(A0) * {NUMBER OF MODES/1023})+1)
  //    <--//--7-----6-----5-----4-----3-----2-----1--//-->
  //    <--//color_mode7-----------------colormode_1--//-->

  //oxo//<---210 --->0//oxo//
  //DELAY TIMER//LEFT KNOB//YELLOW WIRE//
  //{1023 / 100} = {0 - 10} DELAY SPEEDS VARY DEPENDING ON MODE...
  delay_timer = (analogRead(A1) / 100);  //DIVIDING BY 100 GIVES A GOOD 0 THRU 10 SCALE {1023/100}]
  //    <--//--10----9----8----7----6----5----4----3----2----1----0--//-->
  //    <--//-1023------------------------------------------------0--//-->

  //xoo//<---100 --->0//xoo//
  //COLOR//MIDDLE KNOB//GREEN WIRE//
  //{255/POT LIMIT} = {0 -> 240} COLORS//0.2346 FOR RED THRU PINK//
  COLOR = (analogRead(A2) * 0.2346);  //0.249 FOR ENTIRE COLOR SCALE//RED THRU RED//
  //    <--//PINK---PURPLE---BLUE---CYAN---GREEN---YELLOW---RED//-->
  //    <--//240-----210-----160----127-----95------45------0--//-->

  if (temp > x) {
    WarningLightMode();
    FastLED.show();

  } else if (temp < x) {

    //KNOB POSITION 1//RAINBOW MODE1
    if (color_mode == 1) {
      if (COLOR <= 85) {
        RainbowMode();
      }
      if (COLOR > 85 && COLOR <= 170) {
        RainbowMode2();
      }
      if (COLOR > 170 && COLOR <= 255) {
        RainbowMode3();
      }
      FastLED.show();

      //KNOB POSITION 2//RAINBOW MODE2
    } else if (color_mode == 2) {
      MeteorRainMode();
      FastLED.show();

      //KNOB POSITION 3//DANCING MODE
    } else if (color_mode == 3) {
      DancingMode();
      FastLED.show();

      //KNOB POSITION 4//RUNNING LIGHT MODE
    } else if (color_mode == 4) {
      ChaseMode();

      //KNOB POSITION 5//BREATHING MODE
    } else if (color_mode == 5) {
      BreathingMode();
      FastLED.show();

      //KNOB POSITION 6//TWINKLE MODE
    } else if (color_mode == 6) {
      TwinkleMode();
      FastLED.show();

      //KNOB POSITION 7//ALL ON WHITE MODE
    } else if (color_mode == 7) {
      AllOnWhiteMode();
      FastLED.show();
    }
  }
  //REGARDLESS OF LOOP TIMING THE FOLLOWING SERIAL PRINT HAPPENS EVERY 10 SECONDS
  EVERY_N_MILLISECONDS(10000) {
    SerialPrintDetails();
  }
}
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^END OF LOOP^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//

//VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV START OF PROCEDURES VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV//
///POSSIBLE MODES///
//(In Order as they appear from Top-to-bottom)
//FireworksMode()
//FireEffectMode()
//MeteorRainMode()
//RainbowMode()
//RainbowMode2()
//RainbowMode3()
//FillLedsBlack()
//ChaseMode()
//SolidColorMode()
//AllOnWhiteMode()
//BreathingMode()
//DancingMode()
//TwinkleMode()
//WarningLightMode()

//SERIAL PRINT DATA PRINTS THE "Delay_Timer" "COLOR" & "color_mode" VALUES IN SERIAL MONITOR 9600//
void SerialPrintDetails() {
  Serial.println("");
  Serial.println("NEWEST V V V V V NEWEST");
  Serial.print("delay_timer: ");
  Serial.println(delay_timer);
  Serial.print("COLOR: ");
  Serial.println(COLOR);
  Serial.print("color_mode: ");
  Serial.println(color_mode);
  Serial.println("NEWEST ^ ^ ^ ^ ^ NEWEST");
  Serial.println("");
}

void FireworksMode() {
  static uint8_t fireworksTimer = 0;
  static bool fireworksActive = false;

  // Check if it's time to trigger a firework
  if (fireworksTimer <= 0) {
    fireworksActive = true;
    fireworksTimer = random(500, 3000);  // Adjust timing between fireworks
  }

  if (fireworksActive) {
    // Randomly light up LEDs in different colors
    for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
      if (random(10) < 2) {                      // Adjust this value for intensity
        leds6[i] = CHSV(random(256), 255, 255);  // Random color
      }
    }

    for (int i = 0; i < NUM_LEDS_FRONT; i++) {
      if (random(10) < 2) {                      // Adjust this value for intensity
        leds5[i] = CHSV(random(256), 255, 255);  // Random color
      }
    }

    // Show the LEDs
    FastLED.show();

    // Deactivate fireworks after a short duration
    delay(100);  // Adjust this value for fireworks duration
    for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
      leds6[i] = CRGB::Black;  // Turn off LEDs
    }
    for (int i = 0; i < NUM_LEDS_FRONT; i++) {
      leds5[i] = CRGB::Black;  // Turn off LEDs
    }
    FastLED.show();
    fireworksActive = false;
  }

  // Decrement the fireworks timer
  if (fireworksTimer > 0) {
    fireworksTimer--;
  }

  // Show the LEDs
  FastLED.show();
  delay(delay_timer);  // Adjust delay for display speed
}


void FireEffectMode() {
  static byte heat_logo[NUM_LEDS_LOGO];
  static byte heat_front[NUM_LEDS_FRONT];
  static byte heat_inside[NUM_LEDS_INSIDE];
  static byte heat_fans[NUM_LEDS_FANS];

  // Step 1. Cooling down every cell a little

  for (int i = 0; i < NUM_LEDS_LOGO; i++) {
    heat_logo[i] = qsub8(heat_logo[i], random8(0, ((COOLING * 10) / NUM_LEDS_LOGO) + 2));
  }

  for (int i = 0; i < NUM_LEDS_FRONT; i++) {
    heat_front[i] = qsub8(heat_front[i], random8(0, ((COOLING * 10) / NUM_LEDS_FRONT) + 2));
  }

  for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
    heat_inside[i] = qsub8(heat_inside[i], random8(0, ((COOLING * 10) / NUM_LEDS_INSIDE) + 2));
  }

  for (int i = 0; i < NUM_LEDS_FANS; i++) {
    heat_fans[i] = qsub8(heat_fans[i], random8(0, ((COOLING * 10) / NUM_LEDS_FANS) + 2));
  }

  // Step 2. Sparking
  for (int i = 0; i < ((COLOR / 60) + 1); i++) {

    int sparkPositionLogo = random8(NUM_LEDS_LOGO);
    heat_logo[sparkPositionLogo] = qadd8(heat_logo[sparkPositionLogo], random8(SPARKING));

    int sparkPositionFront = random8(NUM_LEDS_FRONT);
    heat_front[sparkPositionFront] = qadd8(heat_front[sparkPositionFront], random8(SPARKING));

    int sparkPositionInside = random8(NUM_LEDS_INSIDE);
    heat_inside[sparkPositionInside] = qadd8(heat_inside[sparkPositionInside], random8(SPARKING));

    int sparkPositionFans = random8(NUM_LEDS_FANS);
    heat_fans[sparkPositionFans] = qadd8(heat_fans[sparkPositionFans], random8(SPARKING));
  }

  // Step 3. Map heat to LED colors
  for (int i = 0; i < NUM_LEDS_LOGO; i++) {
    CRGB color = HeatColor(heat_logo[i]);
    logo3[i] = color;
  }

  for (int i = 0; i < NUM_LEDS_FRONT; i++) {
    CRGB color = HeatColor(heat_front[i]);
    leds5[i] = color;
  }

  for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
    CRGB color = HeatColor(heat_inside[i]);
    leds6[i] = color;
  }

  for (int i = 0; i < NUM_LEDS_FANS; i++) {
    CRGB color = HeatColor(heat_fans[i]);
    fans11[i] = color;
  }

  // Show the LEDs
  FastLED.show();
  delay(delay_timer);  // Adjust delay for fire effect speed
}

//FLASHING SETS OF WHITE LIGHT BETWEEN FRONT AND INSIDE LEDS//COLOR IS DISABLED//DELAY TIMER CAN BE SET TO DIFFERENT SPEEDS//
void MeteorRainMode() {
  // Define the meteor trail decay rate
  const byte meteorTrailDecay = 64;

  for (int i = 0; i < NUM_LEDS_LOGO; i++) {
    logo3[i] = CHSV(100, 0, 255);
    //CHSV(hue, saturation, value)
  }

  for (int i = 0; i < NUM_LEDS_FANS; i++) {
    fans11[i] = CHSV(100, 0, 255);
  }

  // Update the LED colors with meteor trail decay
  for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
    leds6[i].fadeToBlackBy(meteorTrailDecay);
  }

  for (int i = 0; i < NUM_LEDS_FRONT; i++) {
    leds5[i].fadeToBlackBy(meteorTrailDecay);
  }

  // Generate a new meteor at random position
  if (random(10) < 3) {             // Adjust this value for meteor frequency
    int meteorSize = random(1, 5);  // Adjust this range for meteor size
    int meteorStart = random(NUM_LEDS_INSIDE + NUM_LEDS_FRONT - meteorSize);

    for (int i = 0; i < meteorSize; i++) {
      int ledIndex = meteorStart + i;
      if (ledIndex < NUM_LEDS_INSIDE) {
        leds6[ledIndex] = CRGB::White;  // You can change the meteor color here
      } else {
        leds5[ledIndex - NUM_LEDS_INSIDE] = CRGB::White;  // You can change the meteor color here
      }
    }
  }

  // Show the LEDs
  FastLED.show();
  delay(delay_timer);  // Adjust delay for meteor rain speed
}

//CIRCLING RAINBOW MODE CAN CHANGE SPEED BUT NOT COLOR//
void RainbowMode() {

  for (int i = 0; i < NUM_LEDS_LOGO; i++) {
    logo3[i] = CHSV(hue + (i * 10), 255, 127);
    //CHSV(hue, saturation, value)
  }
  for (int i = 0; i < NUM_LEDS_FRONT; i++) {
    leds5[i] = CHSV(hue + (i * 10), 255, 127);
  }
  for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
    leds6[i] = CHSV(hue + (i * 10), 255, 127);
  }
  for (int i = 0; i < NUM_LEDS_FANS; i++) {
    fans11[i] = CHSV(hue + (i * 10), 255, 127);
  }

  delay(delay_timer);
  hue++;
}

void RainbowMode2() {
  static uint8_t hue = 0;

  // First slide the led in one direction
  for (int i = 0; i < NUM_LEDS_LOGO; i++) {
    // Set the i'th led to red
    logo3[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    for (int i = 0; i < NUM_LEDS_LOGO; i++) { logo3[i].nscale8(250); }
    // Wait a little bit before we loop around and do it again
    delay(delay_timer);
  }

  for (int i = 0; i < NUM_LEDS_FRONT; i++) {
    // Set the i'th led to red
    leds5[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    for (int i = 0; i < NUM_LEDS_FRONT; i++) { leds5[i].nscale8(250); }
    // Wait a little bit before we loop around and do it again
    delay(delay_timer);
  }

  for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
    // Set the i'th led to red
    leds6[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    for (int i = 0; i < NUM_LEDS_INSIDE; i++) { leds6[i].nscale8(250); }
    // Wait a little bit before we loop around and do it again
    delay(delay_timer);
  }

  for (int i = 0; i < NUM_LEDS_FANS; i++) {
    // Set the i'th led to red
    fans11[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    for (int i = 0; i < NUM_LEDS_FANS; i++) { fans11[i].nscale8(250); }
    // Wait a little bit before we loop around and do it again
    delay(delay_timer);
  }

  // Now go in the other direction.
  for (int i = (NUM_LEDS_LOGO)-1; i >= 0; i--) {
    // Set the i'th led to red
    logo3[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    for (int i = 0; i < NUM_LEDS_LOGO; i++) { logo3[i].nscale8(250); }
    // Wait a little bit before we loop around and do it again
    delay(delay_timer);
  }

  for (int i = (NUM_LEDS_FRONT)-1; i >= 0; i--) {
    // Set the i'th led to red
    leds5[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    for (int i = 0; i < NUM_LEDS_FRONT; i++) { leds5[i].nscale8(250); }
    // Wait a little bit before we loop around and do it again
    delay(delay_timer);
  }

  for (int i = (NUM_LEDS_INSIDE)-1; i >= 0; i--) {
    // Set the i'th led to red
    leds6[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    for (int i = 0; i < NUM_LEDS_INSIDE; i++) { leds6[i].nscale8(250); }
    // Wait a little bit before we loop around and do it again
    delay(delay_timer);
  }

  for (int i = (NUM_LEDS_FANS)-1; i >= 0; i--) {
    // Set the i'th led to red
    fans11[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    for (int i = 0; i < NUM_LEDS_FANS; i++) { fans11[i].nscale8(250); }
    // Wait a little bit before we loop around and do it again
    delay(delay_timer);
  }
}

//CIRCLING RAINBOW MODE CAN CHANGE SPEED BUT NOT COLOR//
void RainbowMode3() {
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1;  // Move the rainbow along the LED strip

  // Fill the LED strips with a color chase effect
  for (int i = 0; i < NUM_LEDS_LOGO; i++) {
    int hueValue = map((startIndex + i) % 255, 0, 255, 0, 255);
    logo3[i] = CHSV(hueValue, 255, 255);
  }

  for (int i = 0; i < NUM_LEDS_FRONT; i++) {
    int hueValue = map((startIndex + i) % 255, 0, 255, 0, 255);
    leds5[i] = CHSV(hueValue, 255, 255);
  }

  for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
    int hueValue = map((startIndex + i) % 255, 0, 255, 0, 255);
    leds6[i] = CHSV(hueValue, 255, 255);
  }

  for (int i = 0; i < NUM_LEDS_FANS; i++) {
    int hueValue = map((startIndex + i) % 255, 0, 255, 0, 255);
    fans11[i] = CHSV(hueValue, 255, 255);
  }

  // Show the LEDs
  FastLED.show();
  delay(delay_timer);  // Adjust delay for chase speed
}

//FUCTION TO FILL ALL LED STRINGS WITH BLACK/OFF LEDS//USED BY OTHER FUCTIONS//
void FillLedsBlack() {
  // Turn OFF ALL LEDS
  fill_solid(logo3, NUM_LEDS_LOGO, CRGB::Black);
  fill_solid(leds5, NUM_LEDS_FRONT, CRGB::Black);
  fill_solid(leds6, NUM_LEDS_INSIDE, CRGB::Black);
  fill_solid(fans11, NUM_LEDS_FANS, CRGB::Black);
  FastLED.show();
}

//CHASE MODE SENDS A SINGLE LED AROUND THE ENTIRE SYSTEM OF THE CASE//DELAY TIMER SLOWS THIS DOWN AND COLOR IS ENABLED//
void ChaseMode() {
  // Set all LEDs to black
  fill_solid(logo3, NUM_LEDS_LOGO, CRGB::Black);
  fill_solid(leds5, NUM_LEDS_FRONT, CRGB::Black);
  fill_solid(leds6, NUM_LEDS_INSIDE, CRGB::Black);
  fill_solid(fans11, NUM_LEDS_FANS, CRGB::Black);

  // Create a moving chase effect
  for (int i = 0; i < NUM_LEDS_LOGO; i++) {
    logo3[i] = CHSV(COLOR, 255, 255);
    FastLED.show();
    delay(delay_timer);  // Adjust delay for chase speed
    logo3[i] = CRGB::Black;
  }
  for (int i = 0; i < NUM_LEDS_FRONT; i++) {
    leds5[i] = CHSV(COLOR, 255, 255);
    FastLED.show();
    delay(delay_timer);  // Adjust delay for chase speed
    leds5[i] = CRGB::Black;
  }
  for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
    leds6[i] = CHSV(COLOR, 255, 255);
    FastLED.show();
    delay(delay_timer);  // Adjust delay for chase speed
    leds6[i] = CRGB::Black;
  }
  for (int i = 0; i < NUM_LEDS_FANS; i++) {
    fans11[i] = CHSV(COLOR, 255, 255);
    FastLED.show();
    delay(delay_timer);  // Adjust delay for chase speed
    fans11[i] = CRGB::Black;
  }
}

//PRODUCES A SOLID COLOR ON ALL LEDS ON ALL STRINGS THAT CAN BE QUICKLY CHANGED BY COLOR DIAL//DELAY DIAL DOES NOTHING//
void SolidColorMode() {
  // Clear all LEDs
  fill_solid(logo3, NUM_LEDS_LOGO, CRGB::Black);
  fill_solid(leds5, NUM_LEDS_FRONT, CRGB::Black);
  fill_solid(leds6, NUM_LEDS_INSIDE, CRGB::Black);
  fill_solid(fans11, NUM_LEDS_FANS, CRGB::Black);

  // Calculate the gradient color
  CRGB gradientColor = CHSV(COLOR, 255, 255);

  // Apply the gradient color to each LED
  for (int i = 0; i < NUM_LEDS_LOGO; i++) {
    logo3[i] = gradientColor;
  }
  for (int i = 0; i < NUM_LEDS_FRONT; i++) {
    leds5[i] = gradientColor;
  }
  for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
    leds6[i] = gradientColor;
  }
  for (int i = 0; i < NUM_LEDS_FANS; i++) {
    fans11[i] = gradientColor;
  }

  // Show the LEDs
  FastLED.show();
}

//TURNS ALL LEDS TO WHITE AND TO 90% BRIGHTNESS//
void AllOnWhiteMode() {

  for (int i = 0; i < NUM_LEDS_LOGO; i++) {
    logo3[i] = CHSV(100, 0, 255);
    //CHSV(hue, saturation, value)
  }

  for (int i = 0; i < NUM_LEDS_FRONT; i++) {
    leds5[i] = CHSV(100, 0, 255);
  }

  for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
    leds6[i] = CHSV(100, 0, 255);
  }

  for (int i = 0; i < NUM_LEDS_FANS; i++) {
    fans11[i] = CHSV(100, 0, 255);
  }
}

//BREATHIG LEDS CAN CHANGE COLOR AND SPEED OF BREATHING//
void BreathingMode() {

  static uint8_t breathValue = 0;
  static bool breathDirection = true;

  for (int i = 0; i < NUM_LEDS_LOGO; i++) {
    logo3[i] = CHSV(COLOR, 255, breathValue);
    //CHSV(hue, saturation, value)
  }

  for (int i = 0; i < NUM_LEDS_FRONT; i++) {
    leds5[i] = CHSV(COLOR, 255, breathValue);
  }

  for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
    leds6[i] = CHSV(COLOR, 255, breathValue);
  }

  for (int i = 0; i < NUM_LEDS_FANS; i++) {
    fans11[i] = CHSV(COLOR, 255, breathValue);
  }

  if (breathDirection) {
    breathValue++;
    if (breathValue >= 255) {
      breathDirection = false;
    }
  } else {
    breathValue--;
    if (breathValue <= 0) {
      breathDirection = true;
    }
  }

  delay(delay_timer / 2);
}

//DANCING MODE SENDS A LIT LED DOWN EACH LED STRING AND BACK, CAN CHANGE COLOR AND SPEED//
void DancingMode() {

  uint8_t sinBeat = beatsin8((delay_timer * 10) + 2, 0, NUM_LEDS_LOGO - 1, 0, 0);
  uint8_t sinBeat2 = beatsin8((delay_timer * 10) + 2, 0, NUM_LEDS_FRONT - 1, 0, 0);
  uint8_t sinBeat3 = beatsin8((delay_timer * 10) + 2, 0, NUM_LEDS_INSIDE - 1, 0, 0);
  uint8_t sinBeat4 = beatsin8((delay_timer * 10) + 2, 0, NUM_LEDS_FANS - 1, 0, 0);

  logo3[sinBeat] = CHSV(COLOR, 255, 255);
  leds5[sinBeat2] = CHSV(COLOR, 255, 255);
  leds6[sinBeat3] = CHSV(COLOR, 255, 255);
  fans11[sinBeat4] = CHSV(COLOR, 255, 255);

  fadeToBlackBy(logo3, NUM_LEDS_LOGO, 10);
  fadeToBlackBy(leds5, NUM_LEDS_FRONT, 10);
  fadeToBlackBy(leds6, NUM_LEDS_INSIDE, 10);
  fadeToBlackBy(fans11, NUM_LEDS_FANS, 10);
}

//TWINKLE MODE SENDS A STRING OF LEDS OF THE CHOOSEN COLOR DOWN EACH STRING ENDLESSLY, SPEED CAN ALSO BE CHANGED//
void TwinkleMode() {

  logo3[0] = CHSV(COLOR, 255, random8(50, 255));
  leds5[0] = CHSV(COLOR, 255, random8(50, 255));
  leds6[0] = CHSV(COLOR, 255, random8(50, 255));
  fans11[0] = CHSV(COLOR, 255, random8(50, 255));


  for (int i = NUM_LEDS_LOGO - 1; i > 0; i--) {
    logo3[i] = logo3[i - 1];
  }

  for (int i = NUM_LEDS_FRONT - 1; i > 0; i--) {
    leds5[i] = leds5[i - 1];
  }

  for (int i = NUM_LEDS_INSIDE - 1; i > 0; i--) {
    leds6[i] = leds6[i - 1];
  }

  for (int i = NUM_LEDS_FANS - 1; i > 0; i--) {
    fans11[i] = fans11[i - 1];
  }

  delay(delay_timer * 5);
}

//WARNING LIGHT//THIS IS A FLASHING RED LIGHT THAT WARNS USER THE COMPUTER IS OVER 80F or 26.667C//
void WarningLightMode() {
  static bool warningOn = false;

  // Toggle the warning light on and off
  if (millis() % 1000 < 500) {  // Adjust the timing for the warning light
    warningOn = true;
  } else {
    warningOn = false;
  }

  // Set the LEDs to red if warning light is on, otherwise turn them off
  CRGB warningColor = CRGB::Red;
  for (int i = 0; i < NUM_LEDS_LOGO; i++) {
    logo3[i] = warningOn ? warningColor : CRGB::Black;
  }
  for (int i = 0; i < NUM_LEDS_FRONT; i++) {
    leds5[i] = warningOn ? warningColor : CRGB::Black;
  }
  for (int i = 0; i < NUM_LEDS_INSIDE; i++) {
    leds6[i] = warningOn ? warningColor : CRGB::Black;
  }
  for (int i = 0; i < NUM_LEDS_FANS; i++) {
    fans11[i] = warningOn ? warningColor : CRGB::Black;
  }

  delay(10);  // Adjust delay for warning light speed
}
