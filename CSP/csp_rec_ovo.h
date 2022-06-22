#ifndef __CSP_REC_OVO_H__
#define __CSP_REC_OVO_H__

// Include files
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "CCA/rt_nonfinite.h"
#include "CCA/rtwtypes.h"

// Function Declarations
extern double csp_rec_ovo(const double data[12600], std::vector<double> W1, std::vector<double> sample1, std::vector<double> W2, std::vector<double> sample2, std::vector<double> W3, std::vector<double> sample3);
extern void csp_rec_ovo_initialize();
extern void csp_rec_ovo_terminate();

#endif

