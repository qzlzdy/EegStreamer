#ifndef KCCA_H
#define KCCA_H

#include <QString>
#include <armadillo>
using namespace arma ;

// Function Declarations
//为21*600的二维矩阵，其顺序为先列后行，就是
extern void me_KCCA(const double data[12600], double left_fre, double right_fre,
                   double *v, double *idx);

mat kernel(mat X1, mat X2, QString ktype, double kpar);

void kernel_icd(mat X, QString ktype, double kpar, unsigned long long m,
                mat& G, Row<uword>& subset);

void kcca(mat X1, mat X2, QString kernel, double kernelpar, double reg,
          uword numeig, QString decomp, unsigned long long lrank,
          colvec& y1, colvec& y2, colvec& beta);

mat refsig(double f, double S, uword T, uword N);



#endif // KCCA_H
