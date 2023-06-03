#include "SSB3Filter.h"

static int filter_taps[SSB3FILTER_TAP_NUM] = {
  111,
  242,
  162,
  -234,
  -615,
  -539,
  -97,
  127,
  -108,
  -320,
  -90,
  225,
  79,
  -279,
  -186,
  248,
  250,
  -239,
  -351,
  188,
  447,
  -119,
  -553,
  10,
  656,
  140,
  -759,
  -350,
  853,
  642,
  -936,
  -1069,
  1004,
  1776,
  -1055,
  -3286,
  1087,
  10366,
  15286,
  10366,
  1087,
  -3286,
  -1055,
  1776,
  1004,
  -1069,
  -936,
  642,
  853,
  -350,
  -759,
  140,
  656,
  10,
  -553,
  -119,
  447,
  188,
  -351,
  -239,
  250,
  248,
  -186,
  -279,
  79,
  225,
  -90,
  -320,
  -108,
  127,
  -97,
  -539,
  -615,
  -234,
  162,
  242,
  111
};

void SSB3Filter_init(SSB3Filter* f) {
  int i;
  for(i = 0; i < SSB3FILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void SSB3Filter_put(SSB3Filter* f, int input) {
  f->history[f->last_index++] = input;
  if(f->last_index == SSB3FILTER_TAP_NUM)
    f->last_index = 0;
}

int SSB3Filter_get(SSB3Filter* f) {
  long long acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < SSB3FILTER_TAP_NUM; ++i) {
    index = index != 0 ? index-1 : SSB3FILTER_TAP_NUM-1;
    acc += (long long)f->history[index] * filter_taps[i];
  };
  return acc >> 16;
}
