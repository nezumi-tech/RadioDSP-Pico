
/*************************************************************
   SimpleAudioFilter07.ino
   Audio Filter DSP for Shorthwave Receiver

   Filter Functions :
   0. Audio passthrough
   1. AM/SSB filter 6000 kHz
   2. SSB1 filter    3000 kHz
   3. CW  filter    700  Hz

   DNR Functions:
   0. Audio passthrough
   1. DNR Average denoise Level 1
   2. DNR Average denoise Level 2
   3. DNR Average denoise Level 3


   Created: Feb 2023
   Author: Giuseppe Callipo - IK8YFW
   https://github.com/ik8yfw

   License: Creative Common with attribution

   This program use filters built with the tFilter program
   http://t-filter.engineerjs.com/

   Last update: 11/02/2023
*/

#include "Arduino.h"
#include "audioIO.h"

#include "SSB1Filter.h" // 3000Hz
#include "SSB2Filter.h" // 2500Hz
#include "SSB3Filter.h" // 2000Hz
#include "SSB4Filter.h" // 1500Hz
#include "AM1Filter.h"
#include "CW1Filter.h" // 200Hz
#include "CW2Filter.h" // 400Hz
#include "CW3Filter.h" // 600Hz
#include "CW4Filter.h" // 800Hz
#include "AVGFilter.h"
#include "Dec8KFilter.h"

#include "pico/multicore.h"

#include <I2S.h>
#include <ADCInput.h>

// GPIO I2S pin numbers
// TO connect the MAX98357A power Amplifier
#define I2S_BCLK 16
#define I2S_WS 17
#define I2S_DOUT 18
const int sampleRate = 16000;

// Create the I2S port using a PIO state machine
I2S i2s(OUTPUT);

// Create inputt on Adc 2 - GPIO28
ADCInput adcIn(28);

// controll button to select filter
#define PIN_BUTTON_FLp 26
#define PIN_BUTTON_FLn 22

// controll button to select nr
#define PIN_BUTTON_NRp 21
#define PIN_BUTTON_NRn 20

// control to se the audio gain
#define PIN_BUTTON_AUDIO_GAIN 11

// Over range onboard led
#define LED_PIN 25
// Over range external led
//#define LED_PIN 27

// define the maximum safe signal in input
// the led blink at 90% of maximum input level
// this assures no distorsion or overload.
// the 12 bit range ADC is -2048 to +2047 (+- 1.5 V, 3V pp)
// The over range bling at about 2.7 V of input peak signal
// (3 V pp) this is a good safe guard for
// the Pico ADC.
#define OVER_RANGE 1840

// define min and max gain for output amplification
#define MIN_GAIN   1   // suitable for headphone
#define MAX_GAIN   8  // suitable for speaker

// globals
volatile uint8_t     decimator_ct = 0;
volatile uint8_t     decimator_factor = 1;
volatile int16_t     avg, sum, out_sample = 0;

SSB1Filter    flt0;   // AM/SSB filter
AM1Filter     flt;    // SSB filter
CW1Filter     flt1;   // CW  filter
AVGFilter     flt2;   // AVG  filter

SSB2Filter    flt3;   // AM/SSB filter
SSB3Filter    flt4;   // AM/SSB filter
SSB4Filter    flt5;   // AM/SSB filter

CW2Filter     flt6;   // CW  filter
CW3Filter     flt7;   // CW  filter
CW4Filter     flt8;   // CW  filter


Dec8KFilter   fltDec1;
Dec8KFilter   fltDec2;

int           passInput = 0;
uint8_t       filterMode = 0;
uint8_t       nrMode = 0;
int16_t       outSample = 0;
int16_t       outSample_8k = 0;
int8_t        gainAudio = MIN_GAIN;

// Check if need to boost the audio
// For safe reasons the value will be
// only at startup.
void initAudioGain(void) {

  // set the default audioGain for headphones
  gainAudio = MAX_GAIN;
  if (digitalRead(PIN_BUTTON_AUDIO_GAIN) == LOW) {
    // if the pin is connected to GND,
    // the audio gain will be set to 25, suitable
    // to drive loud a 4 to 8ohm 3W speaker
    // to allow this, the MAX amplifier need a Power supply
    // of 5V (1,5 A)
    gainAudio = MIN_GAIN;
  }

}

// continuous loop running for audio processing
void audioIO_loop(void)
{
  int16_t newSample = 0;

  // For debug only
#ifdef DEBUG_SERIAL
  int  nn = micros();
#endif


#ifdef DEBUG_SERIAL
  //char buffer[40];
  //sprintf(buffer, "Value 0 %d ", cap_buf[0]);
  //Serial.println(buffer);
#endif

  while (adcIn.available() > 0) {

    newSample = adcIn.read();
    newSample = newSample - ADC_BIAS - ADC_BIAS_SHIFT;

    /* Blink the builtin LED if the input signal go over range */
    if (newSample > OVER_RANGE) {
      gpio_put(LED_PIN, 1);
    } else {
      gpio_put(LED_PIN, 0);
    }
    newSample = newSample * gainAudio;

    // Pre Filter for Decimator Anti Aliasing
    // a decimator factor by 2 need a 8kHz pre filter
    if (decimator_factor == 2) {
      Dec8KFilter_put(&fltDec1, newSample * 2);
      newSample = Dec8KFilter_get(&fltDec1);
    }

    // apply the NR.

    AVGFilter_put(&flt2, newSample);
    newSample = AVGFilter_get(&flt2);

    // Main Filter Banks
    /// AUDIO PROCESSING WITH FILTERING AND DECIMATIN BLOCK
    decimator_ct ++;
    if ( decimator_ct >= decimator_factor ) {
      decimator_ct = 0;

      // passthrough - no decimation (fs=16 ksps)
      if (filterMode == 0 && decimator_factor == 1) {
        outSample = newSample;
      }

      // Filter for AM mode (fs=16 ksps)
      if (filterMode == 1 && decimator_factor == 1) {

        AM1Filter_put(&flt, newSample * 2);
        outSample = AM1Filter_get(&flt);
      }

      // Filter for SSB (fs=8 ksps)
      if (filterMode == 2 && decimator_factor == 2) {

        SSB1Filter_put(&flt0, newSample * 2);
        outSample_8k = SSB1Filter_get(&flt0);
      }

      // Filter for SSB (fs=8 ksps)
      if (filterMode == 3 && decimator_factor == 2) {

        SSB2Filter_put(&flt3, newSample * 2);
        outSample_8k = SSB2Filter_get(&flt3);
      }

      // Filter for SSB (fs=8 ksps)
      if (filterMode == 4 && decimator_factor == 2) {

        SSB3Filter_put(&flt4, newSample * 2);
        outSample_8k = SSB3Filter_get(&flt4);
      }

      // Filter for SSB (fs=8 ksps)
      if (filterMode == 5 && decimator_factor == 2) {

        SSB4Filter_put(&flt5, newSample * 2);
        outSample_8k = SSB4Filter_get(&flt5);
      }

      // Filter for CW (fs=8 ksps)
      if (filterMode == 6 && decimator_factor == 2) {

        CW1Filter_put(&flt1, newSample * 2);
        outSample_8k = CW1Filter_get(&flt1);
      }

      // Filter for CW (fs=8 ksps)
      if (filterMode == 7 && decimator_factor == 2) {

        CW2Filter_put(&flt6, newSample * 2);
        outSample_8k = CW2Filter_get(&flt6);
      }

      // Filter for CW (fs=8 ksps)
      if (filterMode == 8 && decimator_factor == 2) {

        CW3Filter_put(&flt7, newSample * 2);
        outSample_8k = CW3Filter_get(&flt7);
      }

      // Filter for CW (fs=8 ksps)
      if (filterMode == 9 && decimator_factor == 2) {

        CW4Filter_put(&flt8, newSample * 2);
        outSample_8k = CW4Filter_get(&flt8);
      }

    };

    // Post Filter for Interpolation Anti Aliasing
    // a decimator factor by 2 need a 8kHz pre filter
    if (decimator_factor == 2) {
      Dec8KFilter_put(&fltDec2, outSample_8k * 2);
      outSample = Dec8KFilter_get(&fltDec2);
    }

    // write the same sample twice, once for left and once for the right channel
    i2s.write(outSample);
    i2s.write(outSample);

  };

}


int8_t valFLp, old_valFLp = HIGH;
int8_t valFLn, old_valFLn = HIGH;
int8_t valNRp, old_valNRp = HIGH;
int8_t valNRn, old_valNRn = HIGH;
// check commands on core 1
void core1_commands_check() {

  // run on loop
  while (1) {

    // debounche
    valFLp = digitalRead(PIN_BUTTON_FLp);
    if (valFLp != old_valFLp) {
      old_valFLp = valFLp;

      if (valFLp == LOW) {

        // Roll the filter selection
        if (filterMode == 9)
          filterMode = 0;
        else
          filterMode++;

        // Active the filter stage
        if (filterMode == 0)
          decimator_factor = 1;
        else if (filterMode == 1)
          decimator_factor = 1;
        else if (filterMode == 2)
          decimator_factor = 2;
        else if (filterMode == 3)
          decimator_factor = 2;
        else if (filterMode == 4)
          decimator_factor = 2;
        else if (filterMode == 5)
          decimator_factor = 2;
        else if (filterMode == 6)
          decimator_factor = 2;
        else if (filterMode == 7)
          decimator_factor = 2;
        else if (filterMode == 8)
          decimator_factor = 2;
        else if (filterMode == 9)
          decimator_factor = 2;

        for (int i = 0; i < filterMode; i++) {
          gpio_put(LED_PIN, 1);
          delay(50);
          gpio_put(LED_PIN, 0);
          delay(50);
        }
      }
    }

    valFLn = digitalRead(PIN_BUTTON_FLn);
    if (valFLn != old_valFLn) {
      old_valFLn = valFLn;

      if (valFLn == LOW) {

        // Roll the filter selection
        if (filterMode == 0)
          filterMode = 9;
        else
          filterMode--;

        // Active the filter stage
        if (filterMode == 0)
          decimator_factor = 1;
        else if (filterMode == 1)
          decimator_factor = 1;
        else if (filterMode == 2)
          decimator_factor = 2;
        else if (filterMode == 3)
          decimator_factor = 2;
        else if (filterMode == 4)
          decimator_factor = 2;
        else if (filterMode == 5)
          decimator_factor = 2;
        else if (filterMode == 6)
          decimator_factor = 2;
        else if (filterMode == 7)
          decimator_factor = 2;
        else if (filterMode == 8)
          decimator_factor = 2;
        else if (filterMode == 9)
          decimator_factor = 2;

        for (int i = 0; i < filterMode; i++) {
          gpio_put(LED_PIN, 1);
          delay(50);
          gpio_put(LED_PIN, 0);
          delay(50);
        }
      }
    }

    // debounche
    valNRp = digitalRead(PIN_BUTTON_NRp);
    if (valNRp != old_valNRp) {
      old_valNRp = valNRp;

      if (valNRp == LOW) {

        // Roll the filter selection
        if (nrMode == 3)
          nrMode = 0;
        else
          nrMode++;

        // Noise Reduction stage
        if (nrMode == 0) {
          AVGFilter_init(&flt2, 2);
        } else if (nrMode == 1) {
          AVGFilter_init(&flt2, 5);
        } else if (nrMode == 2) {
          AVGFilter_init(&flt2, 10);
        } else if (nrMode == 3) {
          AVGFilter_init(&flt2, 15);
        }

      }
    }

    valNRn = digitalRead(PIN_BUTTON_NRn);
    if (valNRn != old_valNRn) {
      old_valNRn = valNRn;

      if (valNRn == LOW) {

        // Roll the filter selection
        if (nrMode == 0)
          nrMode = 3;
        else
          nrMode--;

        // Noise Reduction stage
        if (nrMode == 0) {
          AVGFilter_init(&flt2, 2);
        } else if (nrMode == 1) {
          AVGFilter_init(&flt2, 5);
        } else if (nrMode == 2) {
          AVGFilter_init(&flt2, 10);
        } else if (nrMode == 3) {
          AVGFilter_init(&flt2, 15);
        }

      }
    }

    sleep_ms(200);
  }
}


// general setup
void audioIO_setup() {

  Dec8KFilter_init(&fltDec1);
  Dec8KFilter_init(&fltDec2);
  SSB1Filter_init(&flt0);
  AM1Filter_init(&flt);
  CW1Filter_init(&flt1);
  AVGFilter_init(&flt2, 2);

  SSB2Filter_init(&flt3);
  SSB3Filter_init(&flt4);
  SSB4Filter_init(&flt5);

  CW2Filter_init(&flt6);
  CW3Filter_init(&flt7);
  CW4Filter_init(&flt8);


  gpio_init_mask(1 << LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  gpio_put(LED_PIN, 0);

  i2s.setBCLK(I2S_BCLK);
  i2s.setDATA(I2S_DOUT);
  i2s.setBitsPerSample(16);

  // start I2S at the sample rate with 16-bits per sample
  if (!i2s.begin(sampleRate)) {

#ifdef DEBUG_SERIAL
    Serial.println("Failed to initialize I2S!");
#endif
    while (1); // do nothing
  }

  adcIn.setBuffers(4, 16);
  adcIn.onReceive(audioIO_loop);

  if (!adcIn.begin(sampleRate)) {
    //Serial.println("Failed to initialize ADCInput!");
    while (1); // do nothing
  }

  // start controller commands :
  pinMode(PIN_BUTTON_FLp, INPUT_PULLUP);
  pinMode(PIN_BUTTON_FLn, INPUT_PULLUP);
  pinMode(PIN_BUTTON_NRp, INPUT_PULLUP);
  pinMode(PIN_BUTTON_NRn, INPUT_PULLUP);
  pinMode(PIN_BUTTON_AUDIO_GAIN, INPUT_PULLUP);

  // set the maximum audio gain
  initAudioGain();

  // pushbutton to select the filter on core 1
  multicore_launch_core1(core1_commands_check);
  sleep_ms(1400);

}
