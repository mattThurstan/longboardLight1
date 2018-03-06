/*----------------------------(main lights) sub-mode (init)----------------------------*/
typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct {
  Pattern pattern;
  String name;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

#include "subMode_pattern.h"

// List of patterns to cycle through.  Each is defined as a separate function below.
PatternAndNameList patterns = {
  { blank,                "Blank" },
  { glow,                 "Glow" },
  { gradient,             "Gradient (end to end)" },
  { loopTrackLights,      "Track (basic)" },  /* always 3 cos of sensors loopOrientation() */
  { mattKspTest,          "mattKsp Test" },
  { showSolidColor,       "Solid Color" },
  { gradientCol,          "Gradient (end to end) w Colours" },
  { loopTrackLightsCol,   "Track (basic) w Colour" },
  { rainbow,              "Rainbow" },
  { rainbowWithGlitter,   "Rainbow glitter" },
  { confetti,             "Confetti" },
  { sinelon,              "Sinelon" },
  { bpm,                  "BPM" },
  { juggle,               "Juggle" },
  { rainbowCol,           "Rainbow w Colour" },
  { rainbowWithGlitterCol,   "Rainbow glitter w Colour" },
  { confettiCol,          "Confetti w Colour" },
  { sinelonCol,           "Sinelon w Colour" },
  { cylon,                "Cylon" },
  { cylonCol,             "Cylon w Colour" }
};

//const uint8_t patternCount = ARRAY_SIZE(patterns);
const uint8_t _mainLightsSubModeTotal = ARRAY_SIZE(patterns);  //const byte

