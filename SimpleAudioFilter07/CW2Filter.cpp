#include "CW2Filter.h"

static int filter_taps[CW2FILTER_TAP_NUM] = {
  -41,
  -62,
  -74,
  -48,
  24,
  131,
  235,
  283,
  235,
  85,
  -125,
  -317,
  -411,
  -368,
  -213,
  -35,
  62,
  20,
  -127,
  -253,
  -193,
  159,
  763,
  1404,
  1755,
  1516,
  573,
  -896,
  -2437,
  -3466,
  -3497,
  -2364,
  -328,
  1984,
  3799,
  4484,
  3799,
  1984,
  -328,
  -2364,
  -3497,
  -3466,
  -2437,
  -896,
  573,
  1516,
  1755,
  1404,
  763,
  159,
  -193,
  -253,
  -127,
  20,
  62,
  -35,
  -213,
  -368,
  -411,
  -317,
  -125,
  85,
  235,
  283,
  235,
  131,
  24,
  -48,
  -74,
  -62,
  -41
};

void CW2Filter_init(CW2Filter* f) {
  int i;
  for(i = 0; i < CW2FILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void CW2Filter_put(CW2Filter* f, int input) {
  f->history[f->last_index++] = input;
  if(f->last_index == CW2FILTER_TAP_NUM)
    f->last_index = 0;
}

int CW2Filter_get(CW2Filter* f) {
  long long acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < CW2FILTER_TAP_NUM; ++i) {
    index = index != 0 ? index-1 : CW2FILTER_TAP_NUM-1;
    acc += (long long)f->history[index] * filter_taps[i];
  };
  return acc >> 16;
}
