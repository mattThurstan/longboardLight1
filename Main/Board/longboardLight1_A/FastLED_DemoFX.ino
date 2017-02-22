// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  100//55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 150

uint8_t gHue = 0; // rotating "base color" used by many of the patterns
bool gReverseDirection = false;

void Fire2012()
{
// Array of temperature readings at each simulation cell
  byte heat[ledSegment[1].total];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < ledSegment[1].total; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / ledSegment[1].total) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= ledSegment[1].total - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160, 255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < ledSegment[1].total; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (ledSegment[1].total-1) - j;
      } else {
        pixelnumber = j;
      }
      _leds[ledSegment[1].first + pixelnumber] = color;
      _leds[ledSegment[2].first + pixelnumber] = color;
    }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( _leds, _ledNum, 10);
  int pos = random16(ledSegment[1].total);
  CHSV o = CHSV( gHue + random8(64), 50, 128);//200, 255);
  _leds[ledSegment[1].first + pos] += o; //CHSV( gHue + random8(64), 200, 255);
  _leds[ledSegment[2].first + pos] += o;
}
void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( _leds, _ledNum, 20);
  int pos = beatsin16( 13, 0, ledSegment[1].total );
  _leds[ledSegment[1].first + pos] += CHSV( gHue, 255, 192);
  _leds[ledSegment[2].first + pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 60;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < ledSegment[1].total; i++) { //9948
    uint8_t gh = gHue + (i * 2);
    uint8_t h = beat - gHue + (i * 10);
    _leds[ledSegment[1].first + i] = ColorFromPalette(palette, gh, h);
    _leds[ledSegment[2].first + i] = ColorFromPalette(palette, gh, h);
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(_leds, _ledNum, 20);
  byte dothue = 0;
  for( int i = 0; i < 4; i++) { //8
    int jug = beatsin16( i + 3, 0, ledSegment[1].total ); //7
    _leds[ledSegment[1].first + jug] |= CHSV(dothue, 200, 255);
    _leds[ledSegment[2].first + jug] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}



