#include <FastLED.h>

#define BRIGHTNESS_PIN A0
#define SPEED_PIN A1
#define MODE_BUTTON_PIN 3
#define DIRECTION_PIN 5

#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#define MAX_SPEED 31

const TProgmemRGBPalette16 LocalHeatColors_p FL_PROGMEM =
{
  0x000000, 0x220000, 0x330000, 0x440000,
  0x660000, 0x880000, 0xAA0000, 0xBB0000,
  0xDD0000, 0xEE1100, 0xFF1100, 0xFF2200,
  0xFF3300, 0xFF3300, 0xFF4400, 0xFFBBAA
};

class LavaTube {
  public:
    enum Mode {
      Sinusoids_1,
      Sinusoids_2,
      Sinusoids_3,
      Sinusoids_4,
      Sinusoids_5,
      StaticPalette,
      MovingPalette,
      ModeEnd,
    };

    LavaTube(CRGB leds[], int count, int iteration)
      : _leds(leds)
      , _count(count)
      , _brightness(75)
      , _mode(Sinusoids_1)
      , _iteration(iteration)
      , _speed(1)
      , _direction(true)
    {
    }

    void initialize() {
      FastLED.setBrightness(_brightness);
    }

    int brightness() {
      return _brightness;
    }

    void setBrightness(int brightness) {
      _brightness = brightness;
      FastLED.setBrightness(brightness);
    }

    int speed() {
      return _speed;
    }

    void setSpeed(int speed) {
      _speed = speed;
    }

    byte mode() {
      return _mode;
    }

    void setMode(byte mode) {
      if (mode < ModeEnd) {
        _mode = mode;
      }
      memset(_leds, 0, _count * sizeof(CRGB));
      memset(_leds, 0xFF, _mode * sizeof(CRGB));
    }

    bool direction() {
      return _direction;
    }

    void setDirection(bool direction) {
      _direction = direction;
    }


  private:
    long waveAtPoint(uint16_t a, uint16_t b, int point) {
      const long maxValue = 500000;
      return min(maxValue, max(0, a * sin16(b * point))) >> 3;
    }

    long sawtoothAtPoint(uint16_t a, uint16_t b, int point) {
      const long maxValue = 500000;
      //      return min(500000, max(0, a * sin16(b * point))) >> 3;
      int derivative = cos16(b * point);
      if ((_direction && derivative >= 0) || (!_direction && derivative <= 0)) {
        return min(maxValue, max(0, a * sin16(b * point))) >> 3;
      } else {
        return min(maxValue, max(0, a * (sin16(b * point) + (_direction ? 1 : -1) * 8 * derivative))) >> 3;
      }
    }

  public:
    void sineWaves1() {
      for (int i = 0; i < _count; i++) {
        long temp = 0;

        int point = (_iteration >> 3) + i;
        temp += sawtoothAtPoint(6, 801, point);
        temp += sawtoothAtPoint(5, 151, point);
        temp += sawtoothAtPoint(3, 10, point);
        temp += sawtoothAtPoint(2, 201, point);
        temp += sawtoothAtPoint(3, 401, point);
        temp += sawtoothAtPoint(5, 501, point);

        byte colorIndex = scale16(min(65535, temp), 240);
        _leds[i] = ColorFromPalette(LocalHeatColors_p, colorIndex);
      }
    }

    void sineWaves2() {
      for (int i = 0; i < _count; i++) {
        long temp = 0;

        int point = (_iteration >> 3) + i;
        temp += sawtoothAtPoint(6, 801, point);
        temp += sawtoothAtPoint(5, 151, point);
        temp += sawtoothAtPoint(2, 10, point);

        byte colorIndex = scale16(min(65535, temp), 240);
        _leds[i] = ColorFromPalette(LocalHeatColors_p, colorIndex);
      }
    }

    void sineWaves3() {
      for (int i = 0; i < _count; i++) {
        long temp = 0;

        int point = (_iteration >> 3) + i;
        temp += sawtoothAtPoint(2, 201, point);
        temp += sawtoothAtPoint(3, 401, point);
        temp += sawtoothAtPoint(5, 501, point);

        byte colorIndex = scale16(min(65535, temp), 240);
        _leds[i] = ColorFromPalette(LocalHeatColors_p, colorIndex);
      }
    }

    void sineWaves4() {
      for (int i = 0; i < _count; i++) {
        long temp = 0;

        int point = (_iteration >> 2) + i / 2;
        temp += sawtoothAtPoint(6, 801, point);
        temp += sawtoothAtPoint(5, 151, point);
        temp += sawtoothAtPoint(3, 10, point);
        temp += sawtoothAtPoint(2, 201, point);
        temp += sawtoothAtPoint(3, 401, point);
        temp += sawtoothAtPoint(5, 501, point);

        byte colorIndex = scale16(min(65535, temp), 240);
        _leds[i] = ColorFromPalette(LocalHeatColors_p, colorIndex);
      }
    }

    void sineWaves5() {
      EVERY_N_MILLISECONDS( 100 ) {
        fadeUsingColor( _leds, _count, CRGB(210, 125, 0));
      }

      int pos = (_iteration / 2) % _count;
      CRGB heatColor = CRGB(255, 187, 170);
      _leds[pos] += heatColor;
    }

    void pump() {
      if (_mode == Mode::Sinusoids_1) {
        sineWaves1();
      } else if (_mode == Mode::Sinusoids_2) {
        sineWaves2();
      } else if (_mode == Mode::Sinusoids_3) {
        sineWaves3();
      } else if (_mode == Mode::Sinusoids_4) {
        sineWaves4();
      } else if (_mode == Mode::Sinusoids_5) {
        sineWaves5();
      } else if (_mode == Mode::StaticPalette) {
        int start = _speed * 8;
        for (int i = 0; i < _count; ++i) {
          byte colorIndex = scale8(start + i, 240);
          _leds[i] = ColorFromPalette(LocalHeatColors_p, colorIndex);
        }
      } else if (_mode == Mode::MovingPalette) {
        for (int i = 0; i < _count; i++) {
          byte tempByte;
          byte colorindex;
          const int iterationDivisor = 4;
          tempByte = int(i - (_iteration / iterationDivisor)) % 256;
          if (!_direction) {
            colorindex = scale8(tempByte, 240);
          } else {
            colorindex = 240 - scale8(tempByte, 240);
          }
          _leds[i] = ColorFromPalette(LocalHeatColors_p, colorindex);
        }
      }
      _iteration += _speed * (_direction ? -1 : 1);
    }

    void printColor(uint32_t color) {
      if (color < 0x10) {
        Serial.print("00000");
      } else if (color < 0x100) {
        Serial.print("0000");
      } else if (color < 0x1000) {
        Serial.print("000");
      } else if (color < 0x10000) {
        Serial.print("00");
      } else if (color < 0x100000) {
        Serial.print("0");
      }
      Serial.print(color, HEX);
    }

  private:
    CRGB* _leds;
    int _count;
    byte _brightness;
    byte _mode;
    int _iteration;
    int _speed;
    bool _direction;
};

class TubeSet {
  public:
    TubeSet(LavaTube** tubes, int count)
      : _tubes(tubes)
      , _count(count)
    { }

    void initialize() {
      for (int i = 0; i < _count; ++i) {
        _tubes[i]->initialize();
      }
    }

    byte brightness() {
      return _tubes[0]->brightness();
    }

    void setBrightness(byte brightness) {
      for (int i = 0; i < _count; ++i) {
        _tubes[i]->setBrightness(brightness);
      }
    }

    int speed() {
      return _tubes[0]->speed();
    }

    void setSpeed(int speed) {
      for (int i = 0; i < _count; ++i) {
        _tubes[i]->setSpeed(speed);
      }
    }

    bool direction() {
      return _tubes[0]->direction();
    }

    void setDirection(bool direction) {
      for (int i = 0; i < _count; ++i) {
        _tubes[i]->setDirection(direction);
      }
    }

    int mode() {
      return _tubes[0]->mode();
    }

    void setMode(int mode) {
      for (int i = 0; i < _count; ++i) {
        _tubes[i]->setMode(mode);
      }
      FastLED.show();
    }

    void pump() {
      for (int i = 0; i < _count; ++i) {
        _tubes[i]->pump();
      }
      FastLED.show();
    }

  private:
    LavaTube** _tubes;
    int _count;
};

// Strip 0
#define STRIP_0_LEDS 300
CRGB leds_0[STRIP_0_LEDS];
LavaTube tube_0(leds_0, STRIP_0_LEDS, 0.0);

// Strip 1
#define STRIP_1_LEDS 300
CRGB leds_1[STRIP_1_LEDS];
LavaTube tube_1(leds_1, STRIP_1_LEDS, 1000.0);

// Strip 2
#define STRIP_2_LEDS 300
CRGB leds_2[STRIP_2_LEDS];
LavaTube tube_2(leds_2, STRIP_2_LEDS, 10000.0);

LavaTube* tubesArr[] = { &tube_0, &tube_1, &tube_2 };
TubeSet tubes(tubesArr, sizeof(tubesArr) / sizeof(tubesArr[0]));

volatile byte mode = 0;

void modeSelector_ISR() {
  // Keep track of last mode change time to debounce the button
  static long lastModeChangeTm = 0;
  if (digitalRead(MODE_BUTTON_PIN) == LOW && millis() - lastModeChangeTm > 250) {
    mode = (mode + 1) % LavaTube::Mode::ModeEnd;
    lastModeChangeTm = millis();
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(SPEED_PIN, INPUT);
  pinMode(BRIGHTNESS_PIN, INPUT);

  pinMode(DIRECTION_PIN, INPUT_PULLUP);

  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MODE_BUTTON_PIN), modeSelector_ISR, CHANGE);

  FastLED.addLeds<CHIPSET, 6, COLOR_ORDER>(leds_0, STRIP_0_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 10, COLOR_ORDER>(leds_1, STRIP_1_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 11, COLOR_ORDER>(leds_2, STRIP_2_LEDS).setCorrection(TypicalLEDStrip);

  tubes.initialize();
  mode = tubes.mode();
}

void loop() {
  int brightness = analogRead(BRIGHTNESS_PIN) / 32;
  if ((brightness * 8) != tubes.brightness()) {
    tubes.setBrightness(brightness * 8);
  }

  int speed = analogRead(SPEED_PIN) / 32;
  if (speed != tubes.speed()) {
    tubes.setSpeed(speed);
  }

  bool direction = digitalRead(DIRECTION_PIN) == HIGH;
  if (direction != tubes.direction()) {
    tubes.setDirection(direction);
  }

  if (mode != tubes.mode()) {
    tubes.setMode(mode);
    ::delay(500);
  }

  // Serial.println("looping mode = " + String(mode) + " brightness = " + String(brightness) + " speed = " + String(speed) + " direction = " + String(direction));

  tubes.pump();

  //  FastLED.delay();
}
