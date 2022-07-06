#ifndef __CSP_REC_OVO_H__
#define __CSP_REC_OVO_H__

// Include files
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "CCA/rt_nonfinite.h"
#include "CCA/rtwtypes.h"

namespace ehdu{

// Function Declarations
double csp_rec_ovo(const std::array<double, 480> &data,
                   std::vector<double> &W1, std::vector<double> &sample1,
                   std::vector<double> &W2, std::vector<double> &sample2,
                   std::vector<double> &W3, std::vector<double> &sample3);
void csp_rec_ovo_initialize();
void csp_rec_ovo_terminate();

}

#endif

