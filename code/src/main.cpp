#include <Arduino.h>

constexpr static uint8_t _LedPin = 0;
constexpr static uint8_t _LightSensor = 3;
constexpr static uint8_t _Buzzer = 1;

constexpr static int32_t _ThresholdVoltage = 1024 / 50 * 10;
constexpr static uint64_t _TimerLengthMs = 30 * 60 * 1000ULL;

static int32_t _InitialBrightness;
static uint64_t trigger_timeout_start_ms;
static bool trigger_timeout_started;
static uint64_t timerBeginn;

enum
{
  timer_not_set,
  timer_running,
  timer_ringing,
} _State = timer_not_set;

template <typename T>
T no_wrap_sub(const T a, const T b)
{
  if (a >= b)
    return a - b;
  else
    return 0;
}

void measure_initial_brightness()
{
  _InitialBrightness = analogRead(_LightSensor);
}

void init_values()
{
  trigger_timeout_start_ms = 0;
  trigger_timeout_started = false;
  timerBeginn = 0;
}

bool light_triggered()
{
  const int32_t val = analogRead(_LightSensor);

  if (val <= no_wrap_sub(_InitialBrightness, _ThresholdVoltage))
  {
    if (!trigger_timeout_started)
    {
      trigger_timeout_started = true;
      trigger_timeout_start_ms = millis();
    }
    else
    {
      if (trigger_timeout_start_ms + 1000 < millis())
      {
        trigger_timeout_started = false;
        trigger_timeout_start_ms = 0;
        return true;
      }
    }
  }
  else
  {
    trigger_timeout_started = false;
  }

  return false;
}

bool dark_triggered()
{
  const int32_t val = analogRead(_LightSensor);

  if (val >= _InitialBrightness)
  {
    if (!trigger_timeout_started)
    {
      trigger_timeout_started = true;
      trigger_timeout_start_ms = millis();
    }
    else
    {
      if (trigger_timeout_start_ms + 1000 < millis())
      {
        trigger_timeout_started = false;
        trigger_timeout_start_ms = 0;
        return true;
      }
    }
  }
  else
  {
    trigger_timeout_started = false;
  }

  return false;
}

bool timer_finished()
{  
  if ((uint64_t)(timerBeginn + _TimerLengthMs) < millis())
    return true;

  return false;
}

void blink_timer()
{
  digitalWrite(_LedPin, !(millis() & 3072));
}

void ring_timer()
{
  digitalWrite(_LedPin, 1);

  if (!!(millis() & 3072))
    tone(_Buzzer, 1000 + (millis() & 1023));
  else
    noTone(_Buzzer);
}

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))

void setup() 
{
  pinMode(_LedPin, OUTPUT);
  
  const uint16_t notes[] = { 2093, 0, 2093, 0, 2093, 0, 1568, 0, 1760, 0, 1760, 0, 1568, 1568, 0, 0, 1319, 0, 1319, 0, 1175, 0, 1175, 0, 100, 100, 100, 100, 100, 0 };

  for (size_t i = 0; i < ARRAYSIZE(notes); i++)
  {
    if (notes[i] == 0)
      noTone(_Buzzer);
    else
      tone(_Buzzer, notes[i] / 2);

    delay(200);
  }

  digitalWrite(_LedPin, 1); // to compensate for the brightness of the led.
  delay(100);
  measure_initial_brightness();
  init_values();
  digitalWrite(_LedPin, 0);
}

void loop() 
{ 
  switch(_State)
  {
    case timer_not_set:
    {
      if (light_triggered())
      {
        timerBeginn = millis();
        _State = timer_running;
      }

      break;
    }
    case timer_running:
    {
      blink_timer();

      if (timer_finished())
      {
        digitalWrite(_LedPin, 0);
        _State = timer_ringing;
      }

      break;
    }
    case timer_ringing:
    {
      ring_timer();

      if (dark_triggered())
      {
        noTone(_Buzzer);
        digitalWrite(_LedPin, 0);

        _State = timer_not_set;
        init_values();
      }

      break;
    }
    default:
    {
      digitalWrite(_LedPin, 0);
      noTone(_Buzzer);
      break;
    }
  }
}
