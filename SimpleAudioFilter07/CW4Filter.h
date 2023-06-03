#ifndef CW4FILTER_H_
#define CW4FILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 8000 Hz

fixed point precision: 16 bits

* 0 Hz - 100 Hz
  gain = 0
  desired attenuation = -60 dB
  actual attenuation = n/a

* 300 Hz - 1100 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = n/a

* 1250 Hz - 4000 Hz
  gain = 0
  desired attenuation = -60 dB
  actual attenuation = n/a

*/

#define CW4FILTER_TAP_NUM 83

typedef struct {
  int history[CW4FILTER_TAP_NUM];
  unsigned int last_index;
} CW4Filter;

void CW4Filter_init(CW4Filter* f);
void CW4Filter_put(CW4Filter* f, int input);
int CW4Filter_get(CW4Filter* f);

#endif
