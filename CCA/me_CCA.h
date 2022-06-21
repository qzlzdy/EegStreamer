#ifndef __ME_CCA_H__
#define __ME_CCA_H__

// Include files
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "CCA/rt_nonfinite.h"
#include "CCA/rtwtypes.h"
#include "CCA/me_CCA_types.h"

// Function Declarations
//为21*600的二维矩阵，其顺序为先列后行，就是
extern void me_CCA(const double data[12600], double left_fre, double right_fre,
                   double *v, double *idx);

#endif
