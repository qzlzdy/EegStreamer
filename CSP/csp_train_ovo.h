#ifndef __CSP_TRAIN_OVO_H__
#define __CSP_TRAIN_OVO_H__

#include <array>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <math.h>
#include <vector>

#include "CCA/rt_nonfinite.h"
#include "CCA/rtwtypes.h"

// Function Declarations
namespace ehdu{

void csp_train_ovo(const std::array<double, 7200> &data_left_raw,
    const std::array<double, 7200> &data_right_raw,
    const std::array<double, 7200> &data_idle_raw,
    std::vector<double> &W1, std::vector<double> &sample1,
    std::vector<double> &W2, std::vector<double> &sample2,
    std::vector<double> &W3, std::vector<double> &sample3);
void csp_train_ovo_initialize();
void csp_train_ovo_terminate();

}

#endif

