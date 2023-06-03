#ifndef CW2FILTER_H_
#define CW2FILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 8000 Hz

fixed point precision: 16 bits

* 0 Hz - 300 Hz
  gain = 0
  desired attenuation = -60 dB
  actual attenuation = n/a

* 500 Hz - 900 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = n/a

* 1100 Hz - 4000 Hz
  gain = 0
  desired attenuation = -60 dB
  actual attenuation = n/a

*/

#define CW2FILTER_TAP_NUM 71

typedef struct {
  int history[CW2FILTER_TAP_NUM];
  unsigned int last_index;
} CW2Filter;

void CW2Filter_init(CW2Filter* f);
void CW2Filter_put(CW2Filter* f, int input);
int CW2Filter_get(CW2Filter* f);

#endif
