#ifndef __CSP_TRAIN_OVO_H__
#define __CSP_TRAIN_OVO_H__

#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "CCA/rt_nonfinite.h"
#include "CCA/rtwtypes.h"

// Function Declarations
extern void csp_train_ovo(const double data_left_raw[189000], const double
  data_right_raw[189000], const double data_idle_raw[189000], std::vector<double> &W1, std::vector<double> &sample1, std::vector<double> &W2, std::vector<double> &sample2
, std::vector<double> &W3, std::vector<double> &sample3);
extern void csp_train_ovo_initialize();
extern void csp_train_ovo_terminate();

#endif

