#include <IRremote2.h>

#define pinIR 52
IRrecv irrecv(pinIR);
decode_results results;

#define MODE_BUTTON_PIN 53

const int MIN_PIN = 2;
const int MAX_PIN = 13;
const int PIN_COUNT = 12;

// These are indexes into the pin array and correspond to the output wire numbering
const int LEFT_STRIKE_A_PIN   = 0;
const int LEFT_STRIKE_B_PIN   = 1;
const int LEFT_COLUMN_PIN     = 2;
const int CENTER_STRIKE_A_PIN = 3;
const int CENTER_STRIKE_B_PIN = 4;
const int CENTER_STRIKE_C_PIN = 5;
const int CENTER_STRIKE_D_PIN = 6;
const int CENTER_COLUMN_PIN   = 7;
const int RIGHT_STRIKE_A_PIN  = 8;
const int RIGHT_STRIKE_B_PIN  = 9;
const int RIGHT_STRIKE_C_PIN  = 10;
const int RIGHT_COLUMN_PIN    = 11;

const int columnPins[] = { LEFT_COLUMN_PIN, CENTER_COLUMN_PIN, RIGHT_COLUMN_PIN };
const int ceilingPins[] = {
  LEFT_STRIKE_A_PIN, LEFT_STRIKE_B_PIN,
  CENTER_STRIKE_A_PIN, CENTER_STRIKE_B_PIN, CENTER_STRIKE_C_PIN, CENTER_STRIKE_D_PIN,
  RIGHT_STRIKE_A_PIN, RIGHT_STRIKE_B_PIN, RIGHT_STRIKE_C_PIN
};
const int ceilingPinCount = sizeof(ceilingPins) / sizeof(ceilingPins[0]);

void demo() {
  // Start with everything off
  for (int ctr = 0; ctr < PIN_COUNT; ++ctr) {
    analogWrite(MIN_PIN + ctr, 0);
  }

  // Fade up the column
  for (int ctr = 0; ctr < 25; ++ctr) {
    analogWrite(LEFT_COLUMN_PIN + MIN_PIN, ctr * 10);
    delay(50);
  }
  // Twinkle the strikes above it
  for (int ctr = 0; ctr < 50; ++ctr) {
    if (ctr % 2) {
      analogWrite(LEFT_STRIKE_A_PIN + MIN_PIN, 100);
    } else {
      analogWrite(LEFT_STRIKE_B_PIN + MIN_PIN, 100);
    }
    delay(50);
    analogWrite(LEFT_STRIKE_A_PIN + MIN_PIN, 0);
    analogWrite(LEFT_STRIKE_B_PIN + MIN_PIN, 0);
  }
  // Turn off the column
  analogWrite(LEFT_COLUMN_PIN + MIN_PIN, 0);

  // Fade up the column
  for (int ctr = 0; ctr < 25; ++ctr) {
    analogWrite(CENTER_COLUMN_PIN + MIN_PIN, ctr * 10);
    delay(50);
  }
  // Twinkle the strikes above it
  for (int ctr = 0; ctr < 50; ++ctr) {
    switch (ctr % 4) {
      case 0:
        analogWrite(CENTER_STRIKE_A_PIN + MIN_PIN, 100);
        break;
      case 1:
        analogWrite(CENTER_STRIKE_B_PIN + MIN_PIN, 100);
        break;
      case 2:
        analogWrite(CENTER_STRIKE_C_PIN + MIN_PIN, 100);
        break;
      case 3:
        analogWrite(CENTER_STRIKE_D_PIN + MIN_PIN, 100);
        break;
    }
    delay(40);
    analogWrite(CENTER_STRIKE_A_PIN + MIN_PIN, 0);
    analogWrite(CENTER_STRIKE_B_PIN + MIN_PIN, 0);
    analogWrite(CENTER_STRIKE_C_PIN + MIN_PIN, 0);
    analogWrite(CENTER_STRIKE_D_PIN + MIN_PIN, 0);
  }
  // Turn off the column
  analogWrite(CENTER_COLUMN_PIN + MIN_PIN, 0);

  // Fade up the column
  for (int ctr = 0; ctr < 25; ++ctr) {
    analogWrite(RIGHT_COLUMN_PIN + MIN_PIN, ctr * 10);
    delay(50);
  }
  // Twinkle the strikes above it
  for (int ctr = 0; ctr < 50; ++ctr) {
    switch (ctr % 3) {
      case 0:
        analogWrite(RIGHT_STRIKE_A_PIN + MIN_PIN, 100);
        break;
      case 1:
        analogWrite(RIGHT_STRIKE_B_PIN + MIN_PIN, 100);
        break;
      case 2:
        analogWrite(RIGHT_STRIKE_C_PIN + MIN_PIN, 100);
        break;
    }
    delay(45);
    analogWrite(RIGHT_STRIKE_A_PIN + MIN_PIN, 0);
    analogWrite(RIGHT_STRIKE_B_PIN + MIN_PIN, 0);
    analogWrite(RIGHT_STRIKE_C_PIN + MIN_PIN, 0);
  }
  // Turn off the column
  analogWrite(RIGHT_COLUMN_PIN + MIN_PIN, 0);

  for (int ctr = 0; ctr < 50; ++ctr) {
    int pin = MIN_PIN + random(PIN_COUNT);
    analogWrite(pin, 150 + random(100));
    delay(50 + random(50));
    analogWrite(pin, 0);
    delay(25 + random(75));
  }
  delay(350);

  // Big strike on all three columns
  for (int ctr = 0; ctr < 5; ++ctr) {
    analogWrite(LEFT_COLUMN_PIN + MIN_PIN, 255);
    analogWrite(CENTER_COLUMN_PIN + MIN_PIN, 255);
    analogWrite(RIGHT_COLUMN_PIN + MIN_PIN, 255);
    delay(20 * ctr);
    analogWrite(LEFT_COLUMN_PIN + MIN_PIN, 0);
    analogWrite(CENTER_COLUMN_PIN + MIN_PIN, 0);
    analogWrite(RIGHT_COLUMN_PIN + MIN_PIN, 0);
    delay(50);
  }

  for (int brightness = 250; brightness >= 0; brightness -= max(1, brightness / 10)) {
    analogWrite(LEFT_COLUMN_PIN + MIN_PIN, brightness);
    analogWrite(CENTER_COLUMN_PIN + MIN_PIN, brightness);
    analogWrite(RIGHT_COLUMN_PIN + MIN_PIN, brightness);

    if (random(10) < 7) {
      int randomCeilingPin = ceilingPins[random(ceilingPinCount)];
      analogWrite(randomCeilingPin + MIN_PIN, 25 + random(50));
    } else {
      for (int ctr = 0; ctr < ceilingPinCount; ++ctr) {
        analogWrite(ceilingPins[ctr] + MIN_PIN, 0);
      }
    }

    delay(150);
  }
  for (int ctr = 0; ctr < ceilingPinCount; ++ctr) {
    analogWrite(ceilingPins[ctr] + MIN_PIN, 0);
  }
  
  delay(1000);
}

enum RemoteKeys {
  Zero,
  One,
  Two,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Nine,
  PowerToggle,
  VolUp,
  VolDown,
  FuncStop,
  PrevTrack,
  NextTrack,
  PlayPause,
  Down,
  Up,
  Eq,
  StRept
};

class LightningPanel {
  public:
    LightningPanel(int columnPin, int* strikePins, unsigned strikeCount)
      : _columnPin(columnPin)
      , _strikePins(strikePins)
      , _strikeCount(strikeCount)
    { }

    int randomStrikePin() {
      return _strikePins[random(_strikeCount)];
    }

    int randomStrikePinNot(int equalTo) {
      int choice;
      do {
        choice = randomStrikePin();
      } while (choice == equalTo);
      return choice;
    }

    int _columnPin;
    int* _strikePins;
    unsigned _strikeCount;
};

int leftStrikePins[] = { LEFT_STRIKE_A_PIN, LEFT_STRIKE_B_PIN };
LightningPanel leftPanel = LightningPanel(LEFT_COLUMN_PIN, leftStrikePins, sizeof(leftStrikePins) / sizeof(leftStrikePins[0]));

int centerStrikePins[] = { CENTER_STRIKE_A_PIN, CENTER_STRIKE_B_PIN, CENTER_STRIKE_C_PIN, CENTER_STRIKE_D_PIN };
LightningPanel centerPanel = LightningPanel(CENTER_COLUMN_PIN, centerStrikePins, sizeof(centerStrikePins) / sizeof(centerStrikePins[0]));

int rightStrikePins[] = { RIGHT_STRIKE_A_PIN, RIGHT_STRIKE_B_PIN, RIGHT_STRIKE_C_PIN };
LightningPanel rightPanel = LightningPanel(RIGHT_COLUMN_PIN, rightStrikePins, sizeof(rightStrikePins) / sizeof(rightStrikePins[0]));

LightningPanel* globalPanels[] = { &leftPanel, &centerPanel, &rightPanel };

class LightningController {
  public:
    LightningController()
      : next(this)
      , _startPin(MIN_PIN)
      , _endPin(MAX_PIN)
      , _panels(globalPanels)
      , _panelCount(sizeof(globalPanels) / sizeof(globalPanels[0]))
    {
      for (int ctr = 0; ctr < numPins(); ++ctr) {
        _pinBrightness[ctr] = 0;
      }
    }

    virtual ~LightningController() { }

    virtual void reset() {
      setAllPins(0);
    }

    virtual void update() { }

    virtual void handleRemoteKey(int key) { }

    void setAllPins(int brightness) {
      for (int pin = _startPin; pin <= _endPin; ++pin) {
        _pinBrightness[indexOfPin(pin)] = brightness;
        analogWrite(pin, brightness);
      }
    }

    int indexOfPin(int pin) {
      return pin - _startPin;
    }

    void setPin(int pin, int brightness) {
      _pinBrightness[indexOfPin(pin)] = brightness;
      analogWrite(pin, brightness);
    }

    int pinAt(int index) {
      return _startPin + index;
    }

    int randomPin() {
      return random(_endPin - _startPin + 1) + _startPin;
    }

    LightningPanel* randomPanel() {
      return _panels[random(_panelCount)];
    }

    int randomPinNot(int equalTo) {
      int choice;
      do {
        choice = randomPin();
      } while (choice == equalTo);
      return choice;
    }

    int numPins() {
      return _endPin - _startPin + 1;
    }

    LightningController *next;

  protected:
    int _startPin;
    int _endPin;
    byte _pinBrightness[PIN_COUNT];
    LightningPanel** _panels;
    int _panelCount;
};

class AnimationFrame {
  public:
    AnimationFrame() : _duration(0) {
      for (int ctr = 0; ctr < PIN_COUNT; ++ctr) {
        _pattern[ctr] = 0;
      }
    }

    // Each byte in the array indicates the brightness for the corresponding pin
    byte _pattern[PIN_COUNT];
    byte _duration;
};

class Strike {
  public:
    Strike()
      : _panel(0)
      , _currentFrame(-1)
      , _currentFrameStartTm(0)
      , _maxBrightness(255)
    { }

    virtual ~Strike() { }

    void setMaxBrightness(byte max) {
      _maxBrightness = max;
    }

    // Updates the pin(s) this Strike is controlling for the current plan
    void update() {
      if (finished()) return;

      AnimationFrame& current = _frames[_currentFrame];

      if (millis() >= _currentFrameStartTm + current._duration) {
        if (_currentFrame < MaxFrames - 1 && _frames[_currentFrame + 1]._duration > 0) {
          _currentFrame++;
          _currentFrameStartTm = millis();
        } else {
          _currentFrame = -1;
          _currentFrameStartTm = 0;
          return;
        }
      }

      // Set pins to the pattern's state
      for (int ctr = 0; ctr < PIN_COUNT; ++ctr) {
        analogWrite(MIN_PIN + ctr, current._pattern[ctr]);
      }
    }

    // Returns true if the Strike is finished animating its current plan
    bool finished() {
      return _currentFrame < 0 || _currentFrameStartTm <= 0;
    }

    void setPanel(LightningPanel* panel) {
      _panel = panel;

      _currentFrame = 0;
      _currentFrameStartTm = millis();
      for (int i = 0; i < MaxFrames; ++i) {
        for (int j = 0; j < PIN_COUNT; ++j) {
          _frames[i]._pattern[j] = 0;
        }
        _frames[i]._duration = 0;
      }

      reset();
    }

    // Subclasses should override this method and update their animation
    virtual void reset() { }

  protected:
    uint32_t timeInCurrentFrame() {
      return millis() - _currentFrameStartTm;
    }

    static const int MaxFrames = 60;

    LightningPanel* _panel;
    AnimationFrame _frames[MaxFrames];
    int _currentFrame;
    uint32_t _currentFrameStartTm;
    byte _maxBrightness;
};

class BigStrike: public Strike {
  public:
    BigStrike() { }

    void reset() {
      if (!_panel) return;

      int strikePin = _panel->randomStrikePin();
      int columnPin = _panel->_columnPin;

      int frames = 3 + random(10);
      for (int i = 0; i < frames; i += 2) {
        if (random(100) < 85) {
          // Pretty bright
          _frames[i]._pattern[strikePin] = _maxBrightness * 3 / 5 + random(_maxBrightness * 2 / 5);
          if (random(100) < 75) { // 75% of the time include the column
            _frames[i]._pattern[columnPin] = _maxBrightness * 3 / 5 + random(_maxBrightness * 2 / 5);
          }
          _frames[i]._duration = 8 + random(8);
          if (i + 1 < frames) {
            _frames[i + 1]._pattern[strikePin] = 0;
            _frames[i + 1]._pattern[columnPin] = 0;
            _frames[i + 1]._duration = 10 + random(10);
          }
        } else {
          // Full brightness
          _frames[i]._pattern[strikePin] = _maxBrightness;
          _frames[i]._pattern[columnPin] = _maxBrightness;
          _frames[i]._duration = 100;
          if (i + 1 < frames) {
            _frames[i + 1]._pattern[strikePin] = 0;
            _frames[i + 1]._pattern[columnPin] = 0;
            _frames[i + 1]._duration = 25;
          }
        }
      }
    }
};

class SmallStrike: public Strike {
  public:
    SmallStrike() { }

    void reset() {
      if (!_panel) return;

      int strikePin = _panel->randomStrikePin();
      int columnPin = _panel->_columnPin;

      int frames = 3 + random(8);
      for (int i = 0; i < frames; i += 2) {
        _frames[i]._pattern[strikePin] = _maxBrightness / 10 + random(_maxBrightness / 3);
        if (random(100) < 25) { // 25% of the time include the column
          _frames[i]._pattern[columnPin] = _maxBrightness / 10 + random(_maxBrightness / 3);
        }
        _frames[i]._duration = 8 + random(12);
        if (i + 1 < frames) {
          _frames[i + 1]._pattern[strikePin] = 0;
          _frames[i + 1]._pattern[columnPin] = 0;
          _frames[i]._duration = 10 + random(15);
        }
      }
    }
};

class MultiStrike: public Strike {
  public:
    MultiStrike() { }

    AnimationFrame makeFrame(int pinA, int pinB, int columnPin, byte brightness, byte duration) {
      AnimationFrame frame;
      frame._duration = duration;

      if (random(10) < 3) {
        if (random(2)) {
          frame._pattern[pinA] = brightness;
        } else {
          frame._pattern[pinB] = brightness;
        }
      } else {
        frame._pattern[pinA] = brightness;
        frame._pattern[pinB] = brightness;
      }

      if (random(100) < 85) {
        frame._pattern[columnPin] = brightness;
      }

      return frame;
    }

    void reset() {
      if (!_panel) return;

      int pinA = _panel->randomStrikePin();
      int pinB = _panel->randomStrikePinNot(pinA);
      int columnPin = _panel->_columnPin;

      int frames = 6 + random(10);
      for (int i = 0; i < frames; i += 2) {
        if (random(100) < 90) {

          _frames[i] = makeFrame(pinA, pinB, columnPin, _maxBrightness / 2 + random(_maxBrightness / 2), 4 + random(12));
          if (i + 1 < frames) {
            _frames[i + 1] = makeFrame(pinA, pinB, columnPin, 0, 10 + random(10));
          }

        } else {

          _frames[i] = makeFrame(pinA, pinB, columnPin, _maxBrightness * 4 / 5 + random(_maxBrightness / 5), 100);
          if (i + 1 < frames) {
            _frames[i + 1] = makeFrame(pinA, pinB, columnPin, 0, 15);
          }

        }
      }
    }
};

class RollingStrike: public Strike {
    void reset() {

      int sweeps = 10 + 2 * random(15); // Even number assumed below
      int frameCtr = 0;
      for (; frameCtr < sweeps; frameCtr += 2) {
        LightningPanel* panel = globalPanels[random((sizeof(globalPanels) / sizeof(globalPanels[0])))];
        int strikePin = panel->randomStrikePin();
        _frames[frameCtr]._pattern[strikePin] = _maxBrightness / 15 + random(_maxBrightness / 2);
        if (random(10) > 7) { // 20% chance
          _frames[frameCtr]._pattern[panel->randomStrikePinNot(strikePin)] = _maxBrightness / 4 + random(_maxBrightness / 2);
        }
        _frames[frameCtr]._duration = 12 + random(12);

        // Turn back off
        _frames[frameCtr + 1]._duration = 150 + random(100);
      }

      _frames[frameCtr++]._duration = 150 + random(100);

      LightningPanel* tdPanel = globalPanels[random((sizeof(globalPanels) / sizeof(globalPanels[0])))];
      int touchdown = 10 + random(6);
      for (int tdCtr = 0; tdCtr < touchdown; tdCtr += 2) {
        int strikePin = tdPanel->randomStrikePin();
        _frames[sweeps + tdCtr]._pattern[strikePin] = _maxBrightness * 4 / 5 + random(_maxBrightness / 5);
        if (random(10) < 8) { // 80% chance
          _frames[sweeps + tdCtr]._pattern[tdPanel->randomStrikePinNot(strikePin)] = _maxBrightness * 3 / 5 + random(_maxBrightness / 5);          
        }
        _frames[sweeps + tdCtr]._pattern[tdPanel->_columnPin] = _maxBrightness * 4 / 5 + random(_maxBrightness / 5);
        _frames[sweeps + tdCtr]._duration = 10 + random(40);

        _frames[sweeps + tdCtr + 1]._duration = 15 + random(25);
      }
    }
};

class RandomStrikesController: public LightningController {
    const int DefaultDelay = 2500;
    const int DefaultBrightness = 150;

  public:
    RandomStrikesController()
      : _delay(DefaultDelay)
      , _maxBrightness(DefaultBrightness)
      , _nextStrikeTm(0)
      , _currentStrike(0)
    {
    }

    void handleRemoteKey(int key) {
      if (key == RemoteKeys::Up) {
        _maxBrightness = min(255, _maxBrightness + 10);
      } else if (key == RemoteKeys::Down) {
        _maxBrightness = max(15, _maxBrightness - 10);
      } else if (key == RemoteKeys::PrevTrack) {
        _delay = min(5000, _delay + 100);
      } else if (key == RemoteKeys::NextTrack) {
        _delay = max(0, _delay - 100);
      }
    }

    void update() {
      if (!_currentStrike && millis() < _nextStrikeTm) {
        return;
      }

      if (_currentStrike) {
        _currentStrike->setMaxBrightness(_maxBrightness);

        // If there's a current strike either update it or, if it just finished,
        // clear it out and determine how long until the next strike.

        if (!_currentStrike->finished()) {
          _currentStrike->update();
        } else if (_nextStrikeTm <= millis()) {
          // Make sure everything's dark
          for (int pin = _startPin; pin <= _endPin; ++pin) {
            analogWrite(pin, 0);
          }
          _currentStrike = 0;
          _nextStrikeTm = millis() + 500 + random(_delay);
        }
        return;
      }

      // Create a new strike
      createRandomStrike();
    }

    void createRandomStrike() {
      // Strike on one panel/column
      LightningPanel* panel = randomPanel();

      int typeSelector = random(100);
      if (typeSelector < 30) {
        // 30 % chance of big strike
        _currentStrike = &_bigStrike;
      } else if (typeSelector < 80) {
        // 50 % chance of small strike
        _currentStrike = &_smallStrike;
      } else if (typeSelector < 97) {
        // 17 % chance of multi strike
        _currentStrike = &_multiStrike;
      } else {
        // 3 % chance of rolling strike
        _currentStrike = &_rollingStrike;
      }
      _currentStrike->setPanel(panel);
    }


  private:
    int _delay;
    byte _maxBrightness;
    uint32_t _nextStrikeTm;
    Strike *_currentStrike;
    Strike _noopStrike;
    BigStrike _bigStrike;
    SmallStrike _smallStrike;
    MultiStrike _multiStrike;
    RollingStrike _rollingStrike;
};

class RandomFadeController: public LightningController {
    const int DefaultDelay = 20;
    const int DefaultBrightness = 150;

  public:
    RandomFadeController()
      : _delay(DefaultDelay)
      , _maxBrightness(DefaultBrightness)
      , _waxingPin(_startPin)
      , _waxingPinBrightness(0)
      , _waningPin(_endPin)
      , _waningPinBrightness(0)
      , _nextUpdateTm(0)
    { }

    void reset() {
      LightningController::reset();
      _delay = DefaultDelay;
      _maxBrightness = DefaultBrightness;
    }

    void handleRemoteKey(int key) {
      if (key == RemoteKeys::Up) {
        _maxBrightness = min(255, _maxBrightness + 10);
      } else if (key == RemoteKeys::Down) {
        _maxBrightness = max(15, _maxBrightness - 10);
      } else if (key == RemoteKeys::PrevTrack) {
        _delay = min(50, _delay + 1);
      } else if (key == RemoteKeys::NextTrack) {
        _delay = max(1, _delay - 1);
      }
    }

    void update() {
      if (millis() < _nextUpdateTm) {
        return;
      }

      if (_waxingPinBrightness >= _maxBrightness) {
        analogWrite(_waningPin, 0);
        _waningPin = _waxingPin;
        _waningPinBrightness = _waxingPinBrightness;

        _waxingPin = randomPinNot(_waningPin);
        _waxingPinBrightness = 0;
      }

      _waningPinBrightness = max(0, _waningPinBrightness - 1);
      setPin(_waningPin, _waningPinBrightness);
      _waxingPinBrightness = min(_maxBrightness, _waxingPinBrightness + 1);
      setPin(_waxingPin, _waxingPinBrightness);

      _nextUpdateTm = millis() + _delay;
    }

  private:
    int _delay;
    int _maxBrightness;
    int _waxingPin;
    int _waxingPinBrightness;
    int _waningPin;
    int _waningPinBrightness;
    uint32_t _nextUpdateTm;
};

class SteadyController: public LightningController {
  public:
    SteadyController()
      : _brightness(100)
    { }

    void update() {
      // Nothing to do
    }

    void reset() {
      setAllPins(0);
      setPin(pinAt(0), _brightness);
      setPin(pinAt(1), _brightness);
    }

    void handleRemoteKey(int key) {
      // Remote operates on 1-based indexes
      if (key >= 1 && key <= numPins()) {
        togglePinAt(key - 1);
      } else if (key == RemoteKeys::Zero) {
        if (!allStripsOn()) {
          setAllPins(_brightness);
        } else {
          setAllPins(0);
        }
      } else if (key == RemoteKeys::Up) {
        _brightness = min(255, _brightness + 20);
        for (int i = 0; i < numPins(); ++i) {
          if (_pinBrightness[i] > 0) {
            setPin(pinAt(i), _brightness);
          }
        }
      } else if (key == RemoteKeys::Down) {
        _brightness = max(5, _brightness - 20);
        for (int i = 0; i < numPins(); ++i) {
          if (_pinBrightness[i] > 0) {
            setPin(pinAt(i), _brightness);
          }
        }
      }
    }

    bool allStripsOn() {
      for (int i = 0; i < numPins(); ++i) {
        if (_pinBrightness[i] == 0)
          return false;
      }
      return true;
    }

    void togglePinAt(int index) {
      if (_pinBrightness[index] > 0) {
        setPin(pinAt(index), 0);
      } else {
        setPin(pinAt(index), _brightness);
      }
    }

  private:
    int _startPin;
    int _endPin;
    int _brightness;
};

class SequencedController: public LightningController {
    const int DefaultDelay = 500;
    const int DefaultBrightness = 150;

  public:
    SequencedController()
      : _delay(DefaultDelay)
      , _brightness(DefaultBrightness)
      , _currentPin(_endPin)
      , _nextUpdateTm(0)
    { }

    void reset() {
      LightningController::reset();
      _delay = DefaultDelay;
      _brightness = DefaultBrightness;
    }

    void handleRemoteKey(int key) {
      if (key == RemoteKeys::Up) {
        _brightness = min(255, _brightness + 10);
      } else if (key == RemoteKeys::Down) {
        _brightness = max(15, _brightness - 10);
      } else if (key == RemoteKeys::PrevTrack) {
        _delay = min(5000, _delay + 10);
      } else if (key == RemoteKeys::NextTrack) {
        _delay = max(100, _delay - 10);
      }
    }

    void update() {
      if (millis() < _nextUpdateTm) {
        return;
      }

      setPin(_currentPin, 0);
      _currentPin++;
      if (_currentPin > _endPin) {
        _currentPin = _startPin;
      }
      setPin(_currentPin, _brightness);

      _nextUpdateTm = millis() + (_currentPin == _startPin ? 5 : 1) * _delay;
    }

  private:
    int _delay;
    int _brightness;
    int _currentPin;
    uint32_t _nextUpdateTm;
};

RandomStrikesController strikes;
RandomFadeController fader;
SteadyController steady;
SequencedController sequenced;

LightningController *modeController = &strikes;

volatile bool wantsModeChange = false;

void modeChange_ISR() {
  // Keep track of last mode change time to debounce the button
  static long lastModeChangeTm = 0;
  if (digitalRead(MODE_BUTTON_PIN) == LOW && millis() - lastModeChangeTm > 250) {
    wantsModeChange = true;
    lastModeChangeTm = millis();
  }
}

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);

  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MODE_BUTTON_PIN), modeChange_ISR, CHANGE);

  for (int pin = MIN_PIN; pin <= MAX_PIN; ++pin) {
    pinMode(pin, OUTPUT);
    analogWrite(pin, 0);
  }
  delay(2000);

  strikes.next = &sequenced;
  sequenced.next = &fader;
  fader.next = &steady;
  steady.next = &strikes;
}

uint32_t last_decoded_command = 0;
uint32_t last_decoded_tm = 0;

void decode_ir_command() {
  if (irrecv.decode(&results)) {
    if (results.decode_type == 1) {
      uint32_t command = results.value;
      if (command == 0xFFFFFFFF) {
        if (!last_decoded_command || millis() - last_decoded_tm < 150) {
          // Ignore the repeat for now
          irrecv.resume();
          return;
        }

        last_decoded_tm = millis();
        command = last_decoded_command;
        Serial.print("Repeating command ");
        Serial.println(command, HEX);

      } else {
        Serial.print("Decoded command ");
        Serial.println(command, HEX);

        last_decoded_command = command;
        last_decoded_tm = millis();
      }

      switch (command) {
        case 0xFFA25D: // Elegoo Power toggle
        case 0xF740BF: // IRC240-S Off
          demo();
          last_decoded_command = 0; // Prevent repeat
          break;

        case 0xFFE21D: // Elegoo Func/Stop
        case 0xF7C03F: // IRC240-S On
          modeController = modeController->next;
          modeController->reset();
          last_decoded_command = 0; // Prevent repeat
          break;


        case 0xFF22DD: // Elegoo Prev Track
        case 0xF7609F: // IRC240-S Clear
          modeController->handleRemoteKey(RemoteKeys::PrevTrack); break;
        
        case 0xFFC23D: // Elegoo Next Track
        case 0xF7E01F: // IRC240-S smooth
        modeController->handleRemoteKey(RemoteKeys::NextTrack); break;
        

        // These are all Elegoo first, IRC240-S second
        case 0xFF6897:
        case 0xF720DF:
          modeController->handleRemoteKey(0); break;
        case 0xFF30CF:
        case 0xF7A857:
          modeController->handleRemoteKey(1); break;
        case 0xFF18E7:
        case 0xF78877:
          modeController->handleRemoteKey(2); break;
        case 0xFF7A85:
        case 0xF7B04F:
          modeController->handleRemoteKey(3); break;
        case 0xFF10EF:
        case 0xF7906F:
          modeController->handleRemoteKey(4); break;
        case 0xFF38C7:
        case 0xF7A05F:
          modeController->handleRemoteKey(5); break;
        case 0xFF5AA5:
        case 0xF728D7:
          modeController->handleRemoteKey(6); break;
        case 0xFF42BD:
        case 0xF708F7:
          modeController->handleRemoteKey(7); break;
        case 0xFF4AB5:
        case 0xF730CF:
          modeController->handleRemoteKey(8); break;
        case 0xFF52AD:
        case 0xF710EF:
          modeController->handleRemoteKey(9); break;

        case 0xFF906F: // Elegoo
        case 0xF700FF: // IRC240-S
          modeController->handleRemoteKey(Up); break;

        case 0xFFE01F: // Elegoo
        case 0xF7807F: // IRC240-S
          modeController->handleRemoteKey(Down); break;

        case 0xFFFFFF: break; // repeat

        default:
          Serial.print("Ignoring unrecognized code: ");
          Serial.println(command, HEX);
          break;
      }
    }
    irrecv.resume();
  }
}

void loop() {
  if (wantsModeChange) {
    modeController = modeController->next;
    modeController->reset();
    wantsModeChange = false;
  }

  decode_ir_command();
  modeController->update();
}
