#ifndef SSB3FILTER_H_
#define SSB3FILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 8000 Hz

fixed point precision: 16 bits

* 0 Hz - 1800 Hz
  gain = 1
  desired ripple = 2 dB
  actual ripple = n/a

* 2000 Hz - 4000 Hz
  gain = 0
  desired attenuation = -60 dB
  actual attenuation = n/a

*/

#define SSB3FILTER_TAP_NUM 77

typedef struct {
  int history[SSB3FILTER_TAP_NUM];
  unsigned int last_index;
} SSB3Filter;

void SSB3Filter_init(SSB3Filter* f);
void SSB3Filter_put(SSB3Filter* f, int input);
int SSB3Filter_get(SSB3Filter* f);

#endif
