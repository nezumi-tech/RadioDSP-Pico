#ifndef SSB4FILTER_H_
#define SSB4FILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 8000 Hz

fixed point precision: 16 bits

* 0 Hz - 1300 Hz
  gain = 1
  desired ripple = 2 dB
  actual ripple = n/a

* 1500 Hz - 4000 Hz
  gain = 0
  desired attenuation = -60 dB
  actual attenuation = n/a

*/

#define SSB4FILTER_TAP_NUM 77

typedef struct {
  int history[SSB4FILTER_TAP_NUM];
  unsigned int last_index;
} SSB4Filter;

void SSB4Filter_init(SSB4Filter* f);
void SSB4Filter_put(SSB4Filter* f, int input);
int SSB4Filter_get(SSB4Filter* f);

#endif
