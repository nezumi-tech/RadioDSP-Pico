#include "CW3Filter.h"

static int filter_taps[CW3FILTER_TAP_NUM] = {
  39,
  56,
  63,
  42,
  0,
  -41,
  -54,
  -29,
  3,
  -15,
  -132,
  -345,
  -565,
  -647,
  -456,
  31,
  680,
  1231,
  1421,
  1132,
  486,
  -195,
  -554,
  -421,
  58,
  461,
  317,
  -593,
  -2034,
  -3351,
  -3756,
  -2746,
  -414,
  2499,
  4901,
  5828,
  4901,
  2499,
  -414,
  -2746,
  -3756,
  -3351,
  -2034,
  -593,
  317,
  461,
  58,
  -421,
  -554,
  -195,
  486,
  1132,
  1421,
  1231,
  680,
  31,
  -456,
  -647,
  -565,
  -345,
  -132,
  -15,
  3,
  -29,
  -54,
  -41,
  0,
  42,
  63,
  56,
  39
};

void CW3Filter_init(CW3Filter* f) {
  int i;
  for(i = 0; i < CW3FILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void CW3Filter_put(CW3Filter* f, int input) {
  f->history[f->last_index++] = input;
  if(f->last_index == CW3FILTER_TAP_NUM)
    f->last_index = 0;
}

int CW3Filter_get(CW3Filter* f) {
  long long acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < CW3FILTER_TAP_NUM; ++i) {
    index = index != 0 ? index-1 : CW3FILTER_TAP_NUM-1;
    acc += (long long)f->history[index] * filter_taps[i];
  };
  return acc >> 16;
}
