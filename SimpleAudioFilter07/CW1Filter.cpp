#include "CW1Filter.h"

static int filter_taps[CW1FILTER_TAP_NUM] = {
  -12,
  -82,
  -60,
  -56,
  -18,
  43,
  112,
  164,
  172,
  119,
  7,
  -139,
  -276,
  -350,
  -321,
  -176,
  58,
  319,
  522,
  588,
  472,
  185,
  -204,
  -580,
  -817,
  -823,
  -570,
  -111,
  426,
  877,
  1092,
  984,
  567,
  -49,
  -682,
  -1137,
  -1265,
  -1015,
  -452,
  258,
  902,
  1282,
  1282,
  902,
  258,
  -452,
  -1015,
  -1265,
  -1137,
  -682,
  -49,
  567,
  984,
  1092,
  877,
  426,
  -111,
  -570,
  -823,
  -817,
  -580,
  -204,
  185,
  472,
  588,
  522,
  319,
  58,
  -176,
  -321,
  -350,
  -276,
  -139,
  7,
  119,
  172,
  164,
  112,
  43,
  -18,
  -56,
  -60,
  -82,
  -12
};

void CW1Filter_init(CW1Filter* f) {
  int i;
  for (i = 0; i < CW1FILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void CW1Filter_put(CW1Filter* f, int input) {
  f->history[f->last_index++] = input;
  if (f->last_index == CW1FILTER_TAP_NUM)
    f->last_index = 0;
}

int CW1Filter_get(CW1Filter* f) {
  long long acc = 0;
  int index = f->last_index, i;
  for (i = 0; i < CW1FILTER_TAP_NUM; ++i) {
    index = index != 0 ? index - 1 : CW1FILTER_TAP_NUM - 1;
    acc += (long long)f->history[index] * filter_taps[i];
  };
  return acc >> 16;
}
