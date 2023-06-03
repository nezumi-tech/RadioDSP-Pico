#ifndef CW3FILTER_H_
#define CW3FILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 8000 Hz

fixed point precision: 16 bits

* 0 Hz - 200 Hz
  gain = 0
  desired attenuation = -60 dB
  actual attenuation = n/a

* 400 Hz - 1000 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = n/a

* 1200 Hz - 4000 Hz
  gain = 0
  desired attenuation = -60 dB
  actual attenuation = n/a

*/

#define CW3FILTER_TAP_NUM 71

typedef struct {
  int history[CW3FILTER_TAP_NUM];
  unsigned int last_index;
} CW3Filter;

void CW3Filter_init(CW3Filter* f);
void CW3Filter_put(CW3Filter* f, int input);
int CW3Filter_get(CW3Filter* f);

#endif
