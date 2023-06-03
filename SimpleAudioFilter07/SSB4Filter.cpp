#include "SSB4Filter.h"

static int filter_taps[SSB4FILTER_TAP_NUM] = {
  -35,
  -174,
  -349,
  -484,
  -442,
  -201,
  109,
  274,
  174,
  -90,
  -263,
  -167,
  115,
  298,
  174,
  -157,
  -358,
  -188,
  218,
  441,
  204,
  -302,
  -552,
  -220,
  425,
  708,
  235,
  -613,
  -944,
  -247,
  937,
  1359,
  257,
  -1634,
  -2354,
  -262,
  4368,
  9157,
  11188,
  9157,
  4368,
  -262,
  -2354,
  -1634,
  257,
  1359,
  937,
  -247,
  -944,
  -613,
  235,
  708,
  425,
  -220,
  -552,
  -302,
  204,
  441,
  218,
  -188,
  -358,
  -157,
  174,
  298,
  115,
  -167,
  -263,
  -90,
  174,
  274,
  109,
  -201,
  -442,
  -484,
  -349,
  -174,
  -35
};

void SSB4Filter_init(SSB4Filter* f) {
  int i;
  for(i = 0; i < SSB4FILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void SSB4Filter_put(SSB4Filter* f, int input) {
  f->history[f->last_index++] = input;
  if(f->last_index == SSB4FILTER_TAP_NUM)
    f->last_index = 0;
}

int SSB4Filter_get(SSB4Filter* f) {
  long long acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < SSB4FILTER_TAP_NUM; ++i) {
    index = index != 0 ? index-1 : SSB4FILTER_TAP_NUM-1;
    acc += (long long)f->history[index] * filter_taps[i];
  };
  return acc >> 16;
}
