#include "CW4Filter.h"

static int filter_taps[CW4FILTER_TAP_NUM] = {
  36,
  30,
  -31,
  -184,
  -406,
  -612,
  -672,
  -485,
  -51,
  492,
  924,
  1051,
  827,
  397,
  25,
  -69,
  149,
  497,
  691,
  541,
  96,
  -377,
  -574,
  -380,
  26,
  275,
  69,
  -576,
  -1298,
  -1622,
  -1306,
  -572,
  -20,
  -239,
  -1346,
  -2778,
  -3517,
  -2677,
  -94,
  3435,
  6476,
  7673,
  6476,
  3435,
  -94,
  -2677,
  -3517,
  -2778,
  -1346,
  -239,
  -20,
  -572,
  -1306,
  -1622,
  -1298,
  -576,
  69,
  275,
  26,
  -380,
  -574,
  -377,
  96,
  541,
  691,
  497,
  149,
  -69,
  25,
  397,
  827,
  1051,
  924,
  492,
  -51,
  -485,
  -672,
  -612,
  -406,
  -184,
  -31,
  30,
  36
};

void CW4Filter_init(CW4Filter* f) {
  int i;
  for(i = 0; i < CW4FILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void CW4Filter_put(CW4Filter* f, int input) {
  f->history[f->last_index++] = input;
  if(f->last_index == CW4FILTER_TAP_NUM)
    f->last_index = 0;
}

int CW4Filter_get(CW4Filter* f) {
  long long acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < CW4FILTER_TAP_NUM; ++i) {
    index = index != 0 ? index-1 : CW4FILTER_TAP_NUM-1;
    acc += (long long)f->history[index] * filter_taps[i];
  };
  return acc >> 16;
}
