#include "SSB2Filter.h"

static int filter_taps[SSB2FILTER_TAP_NUM] = {
  242,
  704,
  660,
  -42,
  -355,
  115,
  231,
  -203,
  -113,
  265,
  -36,
  -260,
  193,
  174,
  -322,
  -4,
  368,
  -218,
  -294,
  430,
  80,
  -552,
  243,
  504,
  -602,
  -234,
  879,
  -264,
  -934,
  938,
  614,
  -1680,
  277,
  2355,
  -2252,
  -2826,
  9997,
  19380,
  9997,
  -2826,
  -2252,
  2355,
  277,
  -1680,
  614,
  938,
  -934,
  -264,
  879,
  -234,
  -602,
  504,
  243,
  -552,
  80,
  430,
  -294,
  -218,
  368,
  -4,
  -322,
  174,
  193,
  -260,
  -36,
  265,
  -113,
  -203,
  231,
  115,
  -355,
  -42,
  660,
  704,
  242
};

void SSB2Filter_init(SSB2Filter* f) {
  int i;
  for(i = 0; i < SSB2FILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void SSB2Filter_put(SSB2Filter* f, int input) {
  f->history[f->last_index++] = input;
  if(f->last_index == SSB2FILTER_TAP_NUM)
    f->last_index = 0;
}

int SSB2Filter_get(SSB2Filter* f) {
  long long acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < SSB2FILTER_TAP_NUM; ++i) {
    index = index != 0 ? index-1 : SSB2FILTER_TAP_NUM-1;
    acc += (long long)f->history[index] * filter_taps[i];
  };
  return acc >> 16;
}