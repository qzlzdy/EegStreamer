
// Include files
#include "me_CCA.h"
#include "rt_nonfinite.h"

// Function Declarations
static void b_diag(const double v[441], double d[21]);
static void b_eml_div(const creal_T x[21], const creal_T y[21], creal_T z[21]);
static void b_eml_matlab_zlascl(double cfrom, double cto, creal_T A[21]);
static boolean_T b_isfinite(double x);
static void b_mldivide(const double A[64], double B[168]);
static int b_mod(int x);
static void b_sqrt(creal_T *x);
static void c_sqrt(double x[441]);
static void cca(const double X[12600], const double Y[4800], creal_T Wx[441],
creal_T Wy[168], double r[21]);
static void cov(const double x[17400], double xy[841]);
static void diag(const creal_T v[21], creal_T d[441]);
static void eig(const double A[441], creal_T V[441], creal_T D[441]);
static creal_T eml_div(const creal_T x, double y);
static void eml_matlab_zggbak(creal_T V[441], int ilo, int ihi, const int
rscale[21]);
static void eml_matlab_zggbal(creal_T A[441], int *ilo, int *ihi, int rscale[21]);
static void eml_matlab_zggev(creal_T A[441], double *info, creal_T alpha1[21],
creal_T beta1[21], creal_T V[441]);
static void eml_matlab_zgghrd(int ilo, int ihi, creal_T A[441], creal_T Z[441]);
static void eml_matlab_zhgeqz(creal_T A[441], int ilo, int ihi, creal_T Z[441],
double *info, creal_T alpha1[21], creal_T beta1[21]);
static double eml_matlab_zlangeM(const creal_T x[441]);
static double eml_matlab_zlanhs(const creal_T A[441], int ilo, int ihi);
static void eml_matlab_zlartg(const creal_T f, const creal_T g, double *cs,
                              creal_T *sn, creal_T *r);
static void eml_matlab_zlascl(double cfrom, double cto, creal_T A[441]);
static void eml_matlab_ztgevc(const creal_T A[441], creal_T V[441]);
static void eml_sort(const double x[21], double y[21], int idx[21]);
static double eml_xnrm2(const creal_T x[441], int ix0);
static void eml_xswap(double x[441], int ix0, int iy0);
static void fliplr(creal_T x[441]);
static void flipud(double x[21]);
static void inv(const double x[64], double y[64]);
static void invNxN(const double x[64], double y[64]);
static void mldivide(const double A[441], double B[168]);
static void refsig(double f, double S, double T, double N, double y[4800]);

// Function Definitions

//
// Arguments    : const double v[441]
//                double d[21]
// Return Type  : void
//
static void b_diag(const double v[441], double d[21])
{
    int j;
    for (j = 0; j < 21; j++) {
        d[j] = v[j * 22];
    }
}

//
// Arguments    : const creal_T x[21]
//                const creal_T y[21]
//                creal_T z[21]
// Return Type  : void
//
static void b_eml_div(const creal_T x[21], const creal_T y[21], creal_T z[21])
{
    int i;
    double brm;
    double bim;
    double d;
    for (i = 0; i < 21; i++) {
        if (y[i].im == 0.0) {
            if (x[i].im == 0.0) {
                z[i].re = x[i].re / y[i].re;
                z[i].im = 0.0;
            } else if (x[i].re == 0.0) {
                z[i].re = 0.0;
                z[i].im = x[i].im / y[i].re;
            } else {
                z[i].re = x[i].re / y[i].re;
                z[i].im = x[i].im / y[i].re;
            }
        } else if (y[i].re == 0.0) {
            if (x[i].re == 0.0) {
                z[i].re = x[i].im / y[i].im;
                z[i].im = 0.0;
            } else if (x[i].im == 0.0) {
                z[i].re = 0.0;
                z[i].im = -(x[i].re / y[i].im);
            } else {
                z[i].re = x[i].im / y[i].im;
                z[i].im = -(x[i].re / y[i].im);
            }
        } else {
            brm = fabs(y[i].re);
            bim = fabs(y[i].im);
            if (brm > bim) {
                bim = y[i].im / y[i].re;
                d = y[i].re + bim * y[i].im;
                z[i].re = (x[i].re + bim * x[i].im) / d;
                z[i].im = (x[i].im - bim * x[i].re) / d;
            } else if (bim == brm) {
                if (y[i].re > 0.0) {
                    bim = 0.5;
                } else {
                    bim = -0.5;
                }

                if (y[i].im > 0.0) {
                    d = 0.5;
                } else {
                    d = -0.5;
                }

                z[i].re = (x[i].re * bim + x[i].im * d) / brm;
                z[i].im = (x[i].im * bim - x[i].re * d) / brm;
            } else {
                bim = y[i].re / y[i].im;
                d = y[i].im + bim * y[i].re;
                z[i].re = (bim * x[i].re + x[i].im) / d;
                z[i].im = (bim * x[i].im - x[i].re) / d;
            }
        }
    }
}

//
// Arguments    : double cfrom
//                double cto
//                creal_T A[21]
// Return Type  : void
//
static void b_eml_matlab_zlascl(double cfrom, double cto, creal_T A[21])
{
    double cfromc;
    double ctoc;
    boolean_T notdone;
    double cfrom1;
    double cto1;
    double mul;
    int i8;
    cfromc = cfrom;
    ctoc = cto;
    notdone = true;
    while (notdone) {
        cfrom1 = cfromc * 2.0041683600089728E-292;
        cto1 = ctoc / 4.9896007738368E+291;
        if ((fabs(cfrom1) > fabs(ctoc)) && (ctoc != 0.0)) {
            mul = 2.0041683600089728E-292;
            cfromc = cfrom1;
        } else if (fabs(cto1) > fabs(cfromc)) {
            mul = 4.9896007738368E+291;
            ctoc = cto1;
        } else {
            mul = ctoc / cfromc;
            notdone = false;
        }

        for (i8 = 0; i8 < 21; i8++) {
            A[i8].re *= mul;
            A[i8].im *= mul;
        }
    }
}

//
// Arguments    : double x
// Return Type  : boolean_T
//
static boolean_T b_isfinite(double x)
{
    return (!rtIsInf(x)) && (!rtIsNaN(x));
}

//
// Arguments    : const double A[64]
//                double B[168]
// Return Type  : void
//
static void b_mldivide(const double A[64], double B[168])
{
    double b_A[64];
    signed char ipiv[8];
    int i5;
    int j;
    int c;
    int jBcol;
    int ix;
    double temp;
    int k;
    double s;
    int i;
    int kAcol;
    memcpy(&b_A[0], &A[0], sizeof(double) << 6);
    for (i5 = 0; i5 < 8; i5++) {
        ipiv[i5] = (signed char)(1 + i5);
    }

    for (j = 0; j < 7; j++) {
        c = j * 9;
        jBcol = 0;
        ix = c;
        temp = fabs(b_A[c]);
        for (k = 2; k <= 8 - j; k++) {
            ix++;
            s = fabs(b_A[ix]);
            if (s > temp) {
                jBcol = k - 1;
                temp = s;
            }
        }

        if (b_A[c + jBcol] != 0.0) {
            if (jBcol != 0) {
                ipiv[j] = (signed char)((j + jBcol) + 1);
                ix = j;
                jBcol += j;
                for (k = 0; k < 8; k++) {
                    temp = b_A[ix];
                    b_A[ix] = b_A[jBcol];
                    b_A[jBcol] = temp;
                    ix += 8;
                    jBcol += 8;
                }
            }

            i5 = (c - j) + 8;
            for (i = c + 1; i + 1 <= i5; i++) {
                b_A[i] /= b_A[c];
            }
        }

        jBcol = c;
        kAcol = c + 8;
        for (i = 1; i <= 7 - j; i++) {
            temp = b_A[kAcol];
            if (b_A[kAcol] != 0.0) {
                ix = c + 1;
                i5 = (jBcol - j) + 16;
                for (k = 9 + jBcol; k + 1 <= i5; k++) {
                    b_A[k] += b_A[ix] * -temp;
                    ix++;
                }
            }

            kAcol += 8;
            jBcol += 8;
        }
    }

    for (jBcol = 0; jBcol < 7; jBcol++) {
        if (ipiv[jBcol] != jBcol + 1) {
            for (kAcol = 0; kAcol < 21; kAcol++) {
                temp = B[jBcol + (kAcol << 3)];
                B[jBcol + (kAcol << 3)] = B[(ipiv[jBcol] + (kAcol << 3)) - 1];
                B[(ipiv[jBcol] + (kAcol << 3)) - 1] = temp;
            }
        }
    }

    for (j = 0; j < 21; j++) {
        jBcol = j << 3;
        for (k = 0; k < 8; k++) {
            kAcol = k << 3;
            if (B[k + jBcol] != 0.0) {
                for (i = k + 1; i + 1 < 9; i++) {
                    B[i + jBcol] -= B[k + jBcol] * b_A[i + kAcol];
                }
            }
        }
    }

    for (j = 0; j < 21; j++) {
        jBcol = j << 3;
        for (k = 7; k > -1; k += -1) {
            kAcol = k << 3;
            if (B[k + jBcol] != 0.0) {
                temp = B[k + jBcol];
                B[k + jBcol] = temp / b_A[k + kAcol];
                for (i = 0; i + 1 <= k; i++) {
                    B[i + jBcol] -= B[k + jBcol] * b_A[i + kAcol];
                }
            }
        }
    }
}

//
// Arguments    : int x
// Return Type  : int
//
static int b_mod(int x)
{
    return x - x / 10 * 10;
}

//
// Arguments    : creal_T *x
// Return Type  : void
//
static void b_sqrt(creal_T *x)
{
    double absxi;
    double absxr;
    if (x->im == 0.0) {
        if (x->re < 0.0) {
            absxi = 0.0;
            absxr = sqrt(fabs(x->re));
        } else {
            absxi = sqrt(x->re);
            absxr = 0.0;
        }
    } else if (x->re == 0.0) {
        if (x->im < 0.0) {
            absxi = sqrt(-x->im / 2.0);
            absxr = -absxi;
        } else {
            absxi = sqrt(x->im / 2.0);
            absxr = absxi;
        }
    } else if (rtIsNaN(x->re) || rtIsNaN(x->im)) {
        absxi = rtNaN;
        absxr = rtNaN;
    } else if (rtIsInf(x->im)) {
        absxi = rtInf;
        absxr = x->im;
    } else if (rtIsInf(x->re)) {
        if (x->re < 0.0) {
            absxi = 0.0;
            absxr = rtInf;
        } else {
            absxi = rtInf;
            absxr = 0.0;
        }
    } else {
        absxr = fabs(x->re);
        absxi = fabs(x->im);
        if ((absxr > 4.4942328371557893E+307) || (absxi > 4.4942328371557893E+307))
        {
            absxr *= 0.5;
            absxi *= 0.5;
            absxi = hypot(absxr, absxi);
            if (absxi > absxr) {
                absxi = sqrt(absxi) * sqrt(1.0 + absxr / absxi);
            } else {
                absxi = sqrt(absxi) * 1.4142135623730951;
            }
        } else {
            absxi = sqrt((hypot(absxr, absxi) + absxr) * 0.5);
        }

        if (x->re > 0.0) {
            absxr = 0.5 * (x->im / absxi);
        } else {
            if (x->im < 0.0) {
                absxr = -absxi;
            } else {
                absxr = absxi;
            }

            absxi = 0.5 * (x->im / absxr);
        }
    }

    x->re = absxi;
    x->im = absxr;
}

//
// Arguments    : double x[441]
// Return Type  : void
//
static void c_sqrt(double x[441])
{
    int k;
    for (k = 0; k < 441; k++) {
        x[k] = sqrt(x[k]);
    }
}

//
// CCA calculate canonical correlations
//
//  [Wx Wy r] = cca(X,Y) where Wx and Wy contains the canonical correlation
//  vectors as columns and r is a vector with corresponding canonical
//  correlations. The correlations are sorted in descending order. X and Y
//  are matrices where each column is a sample. Hence, X and Y must have
//  the same number of columns.
//
//  Example: If X is M*K and Y is N*K there are L=MIN(M,N) solutions. Wx is
//  then M*L, Wy is N*L and r is L*1.
//
//
//  ?? 2000 Magnus Borga, Link鰌ings universitet
// Arguments    : const double X[12600]
//                const double Y[4800]
//                creal_T Wx[441]
//                creal_T Wy[168]
//                double r[21]
// Return Type  : void
//
static void cca(const double X[12600], const double Y[4800], creal_T Wx[441],
creal_T Wy[168], double r[21])
{
    static double b_X[17400];
    int j;
    int i0;
    double C[841];
    double Cyx[168];
    double Cyy[64];
    double a[168];
    static const double y[64] = { 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                  1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0,
                                  0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8,
                                  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                  0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8 };

    double invCyy[64];
    double b_r[441];
    static const double b_y[441] = { 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0E-8 };

    double b[168];
    int i1;
    creal_T V[441];
    double c_r[21];
    int iidx[21];
    int I[21];
    creal_T b_invCyy[168];
    double d0;

    //  --- Calculate covariance matrices ---
    for (j = 0; j < 21; j++) {
        for (i0 = 0; i0 < 600; i0++) {
            b_X[i0 + 600 * j] = X[j + 21 * i0];
        }
    }

    for (j = 0; j < 8; j++) {
        for (i0 = 0; i0 < 600; i0++) {
            b_X[i0 + 600 * (j + 21)] = Y[j + (i0 << 3)];
        }
    }

    cov(b_X, C);
    for (j = 0; j < 21; j++) {
        for (i0 = 0; i0 < 8; i0++) {
            Cyx[i0 + (j << 3)] = C[j + 29 * (21 + i0)];
        }
    }

    for (j = 0; j < 8; j++) {
        for (i0 = 0; i0 < 8; i0++) {
            Cyy[i0 + (j << 3)] = C[(i0 + 29 * (21 + j)) + 21] + y[i0 + (j << 3)];
        }

        //  --- Calcualte Wx and r ---
        // [Wx,r] = eig(inv(Cxx)*Cxy*invCyy*Cyx); % Basis in X
        memcpy(&a[21 * j], &C[29 * (21 + j)], 21U * sizeof(double));
    }

    inv(Cyy, invCyy);
    for (j = 0; j < 21; j++) {
        for (i0 = 0; i0 < 21; i0++) {
            b_r[i0 + 21 * j] = C[i0 + 29 * j] + b_y[i0 + 21 * j];
        }
    }

    mldivide(b_r, a);
    memcpy(&b[0], &Cyx[0], 168U * sizeof(double));
    b_mldivide(Cyy, b);
    for (j = 0; j < 21; j++) {
        for (i0 = 0; i0 < 21; i0++) {
            b_r[j + 21 * i0] = 0.0;
            for (i1 = 0; i1 < 8; i1++) {
                b_r[j + 21 * i0] += a[j + 21 * i1] * b[i1 + (i0 << 3)];
            }
        }
    }

    eig(b_r, Wx, V);

    //  Basis in X
    for (j = 0; j < 441; j++) {
        b_r[j] = V[j].re;
    }

    c_sqrt(b_r);

    //  Canonical correlations
    //  --- Sort correlations ---
    memcpy(&V[0], &Wx[0], 441U * sizeof(creal_T));
    fliplr(V);

    //  reverse order of eigenvectors
    b_diag(b_r, r);
    flipud(r);

    //  extract eigenvalues and reverse their order
    memcpy(&c_r[0], &r[0], 21U * sizeof(double));
    eml_sort(c_r, r, iidx);
    memcpy(&I[0], &iidx[0], 21U * sizeof(int));

    //  sort reversed eigenvalues in ascending order
    flipud(r);

    //  restore sorted eigenvalues into descending order
    for (j = 0; j < 21; j++) {
        memcpy(&Wx[21 * j], &V[21 * (I[j] - 1)], 21U * sizeof(creal_T));

        //  sort reversed eigenvectors in ascending order
    }

    fliplr(Wx);

    //  restore sorted eigenvectors into descending order
    //  --- Calcualte Wy  ---
    for (j = 0; j < 8; j++) {
        for (i0 = 0; i0 < 21; i0++) {
            d0 = 0.0;
            for (i1 = 0; i1 < 8; i1++) {
                d0 += invCyy[j + (i1 << 3)] * Cyx[i1 + (i0 << 3)];
            }

            b_invCyy[j + (i0 << 3)].re = d0;
            b_invCyy[j + (i0 << 3)].im = 0.0;
        }
    }

    for (j = 0; j < 8; j++) {
        for (i0 = 0; i0 < 21; i0++) {
            Wy[j + (i0 << 3)].re = 0.0;
            Wy[j + (i0 << 3)].im = 0.0;
            for (i1 = 0; i1 < 21; i1++) {
                Wy[j + (i0 << 3)].re += b_invCyy[j + (i1 << 3)].re * Wx[i1 + 21 * i0].re
                        - 0.0 * Wx[i1 + 21 * i0].im;
                Wy[j + (i0 << 3)].im += b_invCyy[j + (i1 << 3)].re * Wx[i1 + 21 * i0].im
                        + 0.0 * Wx[i1 + 21 * i0].re;
            }
        }
    }

    //  Basis in Y
    //  Wy = Wy./repmat(sqrt(sum(abs(Wy).^2)),sy,1); % Normalize Wy
}

//
// Arguments    : const double x[17400]
//                double xy[841]
// Return Type  : void
//
static void cov(const double x[17400], double xy[841])
{
    static double b_x[17400];
    int j;
    double d;
    int i;
    int k;
    memcpy(&b_x[0], &x[0], 17400U * sizeof(double));
    memset(&xy[0], 0, 841U * sizeof(double));
    for (j = 0; j < 29; j++) {
        d = 0.0;
        for (i = 0; i < 600; i++) {
            d += b_x[i + 600 * j];
        }

        d /= 600.0;
        for (i = 0; i < 600; i++) {
            b_x[i + 600 * j] -= d;
        }
    }

    for (j = 0; j < 29; j++) {
        d = 0.0;
        for (k = 0; k < 600; k++) {
            d += b_x[k + 600 * j] * b_x[k + 600 * j];
        }

        xy[j + 29 * j] = d / 599.0;
        for (i = j + 1; i + 1 < 30; i++) {
            d = 0.0;
            for (k = 0; k < 600; k++) {
                d += b_x[k + 600 * i] * b_x[k + 600 * j];
            }

            xy[i + 29 * j] = d / 599.0;
            xy[j + 29 * i] = xy[i + 29 * j];
        }
    }
}

//
// Arguments    : const creal_T v[21]
//                creal_T d[441]
// Return Type  : void
//
static void diag(const creal_T v[21], creal_T d[441])
{
    int j;
    for (j = 0; j < 441; j++) {
        d[j].re = 0.0;
        d[j].im = 0.0;
    }

    for (j = 0; j < 21; j++) {
        d[j + 21 * j] = v[j];
    }
}

//
// Arguments    : const double A[441]
//                creal_T V[441]
//                creal_T D[441]
// Return Type  : void
//
static void eig(const double A[441], creal_T V[441], creal_T D[441])
{
    creal_T b_A[441];
    int coltop;
    creal_T beta1[21];
    creal_T alpha1[21];
    double colnorm;
    int j;
    creal_T b_alpha1[21];
    creal_T dcv0[21];
    for (coltop = 0; coltop < 441; coltop++) {
        b_A[coltop].re = A[coltop];
        b_A[coltop].im = 0.0;
    }

    eml_matlab_zggev(b_A, &colnorm, alpha1, beta1, V);
    for (coltop = 0; coltop < 422; coltop += 21) {
        colnorm = eml_xnrm2(V, coltop + 1);
        for (j = coltop; j + 1 <= coltop + 21; j++) {
            V[j] = eml_div(V[j], colnorm);
        }
    }

    memcpy(&b_alpha1[0], &alpha1[0], 21U * sizeof(creal_T));
    b_eml_div(b_alpha1, beta1, dcv0);
    diag(dcv0, D);
}

//
// Arguments    : const creal_T x
//                double y
// Return Type  : creal_T
//
static creal_T eml_div(const creal_T x, double y)
{
    creal_T z;
    if (x.im == 0.0) {
        z.re = x.re / y;
        z.im = 0.0;
    } else if (x.re == 0.0) {
        z.re = 0.0;
        z.im = x.im / y;
    } else {
        z.re = x.re / y;
        z.im = x.im / y;
    }

    return z;
}

//
// Arguments    : creal_T V[441]
//                int ilo
//                int ihi
//                const int rscale[21]
// Return Type  : void
//
static void eml_matlab_zggbak(creal_T V[441], int ilo, int ihi, const int
rscale[21])
{
    int i;
    int j;
    double tmp_re;
    double tmp_im;
    if (ilo > 1) {
        for (i = ilo - 2; i + 1 >= 1; i--) {
            if (rscale[i] != i + 1) {
                for (j = 0; j < 21; j++) {
                    tmp_re = V[i + 21 * j].re;
                    tmp_im = V[i + 21 * j].im;
                    V[i + 21 * j] = V[(rscale[i] + 21 * j) - 1];
                    V[(rscale[i] + 21 * j) - 1].re = tmp_re;
                    V[(rscale[i] + 21 * j) - 1].im = tmp_im;
                }
            }
        }
    }

    if (ihi < 21) {
        for (i = ihi; i + 1 < 22; i++) {
            if (rscale[i] != i + 1) {
                for (j = 0; j < 21; j++) {
                    tmp_re = V[i + 21 * j].re;
                    tmp_im = V[i + 21 * j].im;
                    V[i + 21 * j] = V[(rscale[i] + 21 * j) - 1];
                    V[(rscale[i] + 21 * j) - 1].re = tmp_re;
                    V[(rscale[i] + 21 * j) - 1].im = tmp_im;
                }
            }
        }
    }
}

//
// Arguments    : creal_T A[441]
//                int *ilo
//                int *ihi
//                int rscale[21]
// Return Type  : void
//
static void eml_matlab_zggbal(creal_T A[441], int *ilo, int *ihi, int rscale[21])
{
    int32_T exitg2;
    int i;
    int j;
    boolean_T found;
    int ii;
    boolean_T exitg5;
    int nzcount;
    int jj;
    boolean_T exitg6;
    boolean_T b_A;
    boolean_T guard2 = false;
    double atmp_re;
    double atmp_im;
    int32_T exitg1;
    boolean_T exitg3;
    boolean_T exitg4;
    boolean_T guard1 = false;
    memset(&rscale[0], 0, 21U * sizeof(int));
    *ilo = 1;
    *ihi = 21;
    do {
        exitg2 = 0;
        i = 0;
        j = 0;
        found = false;
        ii = *ihi;
        exitg5 = false;
        while ((!exitg5) && (ii > 0)) {
            nzcount = 0;
            i = ii;
            j = *ihi;
            jj = 1;
            exitg6 = false;
            while ((!exitg6) && (jj <= *ihi)) {
                b_A = ((A[(ii + 21 * (jj - 1)) - 1].re != 0.0) || (A[(ii + 21 * (jj - 1))
                        - 1].im != 0.0));
                guard2 = false;
                if (b_A || (ii == jj)) {
                    if (nzcount == 0) {
                        j = jj;
                        nzcount = 1;
                        guard2 = true;
                    } else {
                        nzcount = 2;
                        exitg6 = true;
                    }
                } else {
                    guard2 = true;
                }

                if (guard2) {
                    jj++;
                }
            }

            if (nzcount < 2) {
                found = true;
                exitg5 = true;
            } else {
                ii--;
            }
        }

        if (!found) {
            exitg2 = 2;
        } else {
            if (i != *ihi) {
                for (ii = 0; ii < 21; ii++) {
                    atmp_re = A[(i + 21 * ii) - 1].re;
                    atmp_im = A[(i + 21 * ii) - 1].im;
                    A[(i + 21 * ii) - 1] = A[(*ihi + 21 * ii) - 1];
                    A[(*ihi + 21 * ii) - 1].re = atmp_re;
                    A[(*ihi + 21 * ii) - 1].im = atmp_im;
                }
            }

            if (j != *ihi) {
                for (ii = 0; ii + 1 <= *ihi; ii++) {
                    atmp_re = A[ii + 21 * (j - 1)].re;
                    atmp_im = A[ii + 21 * (j - 1)].im;
                    A[ii + 21 * (j - 1)] = A[ii + 21 * (*ihi - 1)];
                    A[ii + 21 * (*ihi - 1)].re = atmp_re;
                    A[ii + 21 * (*ihi - 1)].im = atmp_im;
                }
            }

            rscale[*ihi - 1] = j;
            (*ihi)--;
            if (*ihi == 1) {
                rscale[0] = 1;
                exitg2 = 1;
            }
        }
    } while (exitg2 == 0);

    if (exitg2 == 1) {
    } else {
        do {
            exitg1 = 0;
            i = 0;
            j = 0;
            found = false;
            jj = *ilo;
            exitg3 = false;
            while ((!exitg3) && (jj <= *ihi)) {
                nzcount = 0;
                i = *ihi;
                j = jj;
                ii = *ilo;
                exitg4 = false;
                while ((!exitg4) && (ii <= *ihi)) {
                    b_A = ((A[(ii + 21 * (jj - 1)) - 1].re != 0.0) || (A[(ii + 21 * (jj -
                                                                                     1)) - 1].im != 0.0));
                    guard1 = false;
                    if (b_A || (ii == jj)) {
                        if (nzcount == 0) {
                            i = ii;
                            nzcount = 1;
                            guard1 = true;
                        } else {
                            nzcount = 2;
                            exitg4 = true;
                        }
                    } else {
                        guard1 = true;
                    }

                    if (guard1) {
                        ii++;
                    }
                }

                if (nzcount < 2) {
                    found = true;
                    exitg3 = true;
                } else {
                    jj++;
                }
            }

            if (!found) {
                exitg1 = 1;
            } else {
                if (i != *ilo) {
                    for (ii = *ilo - 1; ii + 1 < 22; ii++) {
                        atmp_re = A[(i + 21 * ii) - 1].re;
                        atmp_im = A[(i + 21 * ii) - 1].im;
                        A[(i + 21 * ii) - 1] = A[(*ilo + 21 * ii) - 1];
                        A[(*ilo + 21 * ii) - 1].re = atmp_re;
                        A[(*ilo + 21 * ii) - 1].im = atmp_im;
                    }
                }

                if (j != *ilo) {
                    for (ii = 0; ii + 1 <= *ihi; ii++) {
                        atmp_re = A[ii + 21 * (j - 1)].re;
                        atmp_im = A[ii + 21 * (j - 1)].im;
                        A[ii + 21 * (j - 1)] = A[ii + 21 * (*ilo - 1)];
                        A[ii + 21 * (*ilo - 1)].re = atmp_re;
                        A[ii + 21 * (*ilo - 1)].im = atmp_im;
                    }
                }

                rscale[*ilo - 1] = j;
                (*ilo)++;
                if (*ilo == *ihi) {
                    rscale[*ilo - 1] = *ilo;
                    exitg1 = 1;
                }
            }
        } while (exitg1 == 0);
    }
}

//
// Arguments    : creal_T A[441]
//                double *info
//                creal_T alpha1[21]
//                creal_T beta1[21]
//                creal_T V[441]
// Return Type  : void
//
static void eml_matlab_zggev(creal_T A[441], double *info, creal_T alpha1[21],
creal_T beta1[21], creal_T V[441])
{
    double anrm;
    int i;
    boolean_T ilascl;
    double anrmto;
    creal_T b_A[441];
    int rscale[21];
    int ihi;
    double vtemp;
    double y;
    *info = 0.0;
    anrm = eml_matlab_zlangeM(A);
    if (!b_isfinite(anrm)) {
        for (i = 0; i < 21; i++) {
            alpha1[i].re = rtNaN;
            alpha1[i].im = 0.0;
            beta1[i].re = rtNaN;
            beta1[i].im = 0.0;
        }

        for (i = 0; i < 441; i++) {
            V[i].re = rtNaN;
            V[i].im = 0.0;
        }
    } else {
        ilascl = false;
        anrmto = anrm;
        if ((anrm > 0.0) && (anrm < 6.7178761075670888E-139)) {
            anrmto = 6.7178761075670888E-139;
            ilascl = true;
        } else {
            if (anrm > 1.4885657073574029E+138) {
                anrmto = 1.4885657073574029E+138;
                ilascl = true;
            }
        }

        if (ilascl) {
            eml_matlab_zlascl(anrm, anrmto, A);
        }

        memcpy(&b_A[0], &A[0], 441U * sizeof(creal_T));
        eml_matlab_zggbal(b_A, &i, &ihi, rscale);
        eml_matlab_zgghrd(i, ihi, b_A, V);
        eml_matlab_zhgeqz(b_A, i, ihi, V, info, alpha1, beta1);
        if (*info != 0.0) {
        } else {
            eml_matlab_ztgevc(b_A, V);
            eml_matlab_zggbak(V, i, ihi, rscale);
            for (i = 0; i < 21; i++) {
                vtemp = fabs(V[21 * i].re) + fabs(V[21 * i].im);
                for (ihi = 0; ihi < 20; ihi++) {
                    y = fabs(V[(ihi + 21 * i) + 1].re) + fabs(V[(ihi + 21 * i) + 1].im);
                    if (y > vtemp) {
                        vtemp = y;
                    }
                }

                if (vtemp >= 6.7178761075670888E-139) {
                    vtemp = 1.0 / vtemp;
                    for (ihi = 0; ihi < 21; ihi++) {
                        V[ihi + 21 * i].re *= vtemp;
                        V[ihi + 21 * i].im *= vtemp;
                    }
                }
            }

            if (ilascl) {
                b_eml_matlab_zlascl(anrmto, anrm, alpha1);
            }
        }
    }
}

//
// Arguments    : int ilo
//                int ihi
//                creal_T A[441]
//                creal_T Z[441]
// Return Type  : void
//
static void eml_matlab_zgghrd(int ilo, int ihi, creal_T A[441], creal_T Z[441])
{
    int j;
    static const signed char iv0[441] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };

    int jcol;
    int jrow;
    creal_T b_A;
    creal_T c_A;
    creal_T dc0;
    creal_T s;
    double c;
    double Z_im;
    double Z_re;
    double stemp_re;
    double stemp_im;
    double A_im;
    double A_re;
    for (j = 0; j < 441; j++) {
        Z[j].re = iv0[j];
        Z[j].im = 0.0;
    }

    if (ihi < ilo + 2) {
    } else {
        for (jcol = ilo - 1; jcol + 1 < ihi - 1; jcol++) {
            for (jrow = ihi - 1; jrow + 1 > jcol + 2; jrow--) {
                b_A = A[(jrow + 21 * jcol) - 1];
                c_A = A[jrow + 21 * jcol];
                eml_matlab_zlartg(b_A, c_A, &c, &s, &dc0);
                A[(jrow + 21 * jcol) - 1] = dc0;
                A[jrow + 21 * jcol].re = 0.0;
                A[jrow + 21 * jcol].im = 0.0;
                for (j = jcol + 1; j + 1 <= ihi; j++) {
                    Z_im = s.re * A[jrow + 21 * j].re - s.im * A[jrow + 21 * j].im;
                    Z_re = s.re * A[jrow + 21 * j].im + s.im * A[jrow + 21 * j].re;
                    stemp_re = c * A[(jrow + 21 * j) - 1].re + Z_im;
                    stemp_im = c * A[(jrow + 21 * j) - 1].im + Z_re;
                    Z_im = A[(jrow + 21 * j) - 1].re;
                    Z_re = A[(jrow + 21 * j) - 1].im;
                    A_im = A[(jrow + 21 * j) - 1].im;
                    A_re = A[(jrow + 21 * j) - 1].re;
                    A[jrow + 21 * j].re = c * A[jrow + 21 * j].re - (s.re * Z_im + s.im *
                                                                     Z_re);
                    A[jrow + 21 * j].im = c * A[jrow + 21 * j].im - (s.re * A_im - s.im *
                                                                     A_re);
                    A[(jrow + 21 * j) - 1].re = stemp_re;
                    A[(jrow + 21 * j) - 1].im = stemp_im;
                }

                s.re = -s.re;
                s.im = -s.im;
                for (j = ilo - 1; j + 1 <= ihi; j++) {
                    Z_im = s.re * A[j + 21 * (jrow - 1)].re - s.im * A[j + 21 * (jrow - 1)]
                            .im;
                    Z_re = s.re * A[j + 21 * (jrow - 1)].im + s.im * A[j + 21 * (jrow - 1)]
                            .re;
                    stemp_re = c * A[j + 21 * jrow].re + Z_im;
                    stemp_im = c * A[j + 21 * jrow].im + Z_re;
                    Z_im = A[j + 21 * jrow].re;
                    Z_re = A[j + 21 * jrow].im;
                    A_im = A[j + 21 * jrow].im;
                    A_re = A[j + 21 * jrow].re;
                    A[j + 21 * (jrow - 1)].re = c * A[j + 21 * (jrow - 1)].re - (s.re *
                                                                                 Z_im + s.im * Z_re);
                    A[j + 21 * (jrow - 1)].im = c * A[j + 21 * (jrow - 1)].im - (s.re *
                                                                                 A_im - s.im * A_re);
                    A[j + 21 * jrow].re = stemp_re;
                    A[j + 21 * jrow].im = stemp_im;
                }

                for (j = 0; j < 21; j++) {
                    stemp_re = c * Z[j + 21 * jrow].re + (s.re * Z[j + 21 * (jrow - 1)].re
                            - s.im * Z[j + 21 * (jrow - 1)].im);
                    stemp_im = c * Z[j + 21 * jrow].im + (s.re * Z[j + 21 * (jrow - 1)].im
                            + s.im * Z[j + 21 * (jrow - 1)].re);
                    Z_im = Z[j + 21 * jrow].im;
                    Z_re = Z[j + 21 * jrow].re;
                    Z[j + 21 * (jrow - 1)].re = c * Z[j + 21 * (jrow - 1)].re - (s.re *
                                                                                 Z[j + 21 * jrow].re + s.im * Z[j + 21 * jrow].im);
                    Z[j + 21 * (jrow - 1)].im = c * Z[j + 21 * (jrow - 1)].im - (s.re *
                                                                                 Z_im - s.im * Z_re);
                    Z[j + 21 * jrow].re = stemp_re;
                    Z[j + 21 * jrow].im = stemp_im;
                }
            }
        }
    }
}

//
// Arguments    : creal_T A[441]
//                int ilo
//                int ihi
//                creal_T Z[441]
//                double *info
//                creal_T alpha1[21]
//                creal_T beta1[21]
// Return Type  : void
//
static void eml_matlab_zhgeqz(creal_T A[441], int ilo, int ihi, creal_T Z[441],
double *info, creal_T alpha1[21], creal_T beta1[21])
{
    int i;
    double eshift_re;
    double eshift_im;
    double ctemp_re;
    double ctemp_im;
    double rho_re;
    double rho_im;
    double anorm;
    double tempr;
    double b_atol;
    double temp2;
    double ascale;
    boolean_T failed;
    int j;
    boolean_T guard1 = false;
    boolean_T guard2 = false;
    int ifirst;
    int istart;
    int ilast;
    int ilastm1;
    int iiter;
    boolean_T goto60;
    boolean_T goto70;
    boolean_T goto90;
    int jiter;
    int32_T exitg1;
    boolean_T exitg3;
    boolean_T ilazro;
    boolean_T b_guard1 = false;
    creal_T b_A;
    creal_T t1;
    creal_T d;
    double sigma2_re;
    double sigma2_im;
    double g2s;
    boolean_T exitg2;
    boolean_T guard3 = false;
    creal_T c_A;
    creal_T d_A;
    creal_T dc1;
    int x;
    for (i = 0; i < 21; i++) {
        alpha1[i].re = 0.0;
        alpha1[i].im = 0.0;
        beta1[i].re = 1.0;
        beta1[i].im = 0.0;
    }

    eshift_re = 0.0;
    eshift_im = 0.0;
    ctemp_re = 0.0;
    ctemp_im = 0.0;
    rho_re = 0.0;
    rho_im = 0.0;
    anorm = eml_matlab_zlanhs(A, ilo, ihi);
    tempr = 2.2204460492503131E-16 * anorm;
    b_atol = 2.2250738585072014E-308;
    if (tempr > 2.2250738585072014E-308) {
        b_atol = tempr;
    }

    temp2 = 2.2250738585072014E-308;
    if (anorm > 2.2250738585072014E-308) {
        temp2 = anorm;
    }

    ascale = 1.0 / temp2;
    failed = true;
    for (j = ihi; j + 1 < 22; j++) {
        alpha1[j] = A[j + 21 * j];
    }

    guard1 = false;
    guard2 = false;
    if (ihi >= ilo) {
        ifirst = ilo;
        istart = ilo;
        ilast = ihi - 1;
        ilastm1 = ihi - 2;
        iiter = 0;
        goto60 = false;
        goto70 = false;
        goto90 = false;
        jiter = 1;
        do {
            exitg1 = 0;
            if (jiter <= 30 * ((ihi - ilo) + 1)) {
                if (ilast + 1 == ilo) {
                    goto60 = true;
                } else if (fabs(A[ilast + 21 * ilastm1].re) + fabs(A[ilast + 21 *
                                                                   ilastm1].im) <= b_atol) {
                    A[ilast + 21 * ilastm1].re = 0.0;
                    A[ilast + 21 * ilastm1].im = 0.0;
                    goto60 = true;
                } else {
                    j = ilastm1;
                    exitg3 = false;
                    while ((!exitg3) && (j + 1 >= ilo)) {
                        if (j + 1 == ilo) {
                            ilazro = true;
                        } else if (fabs(A[j + 21 * (j - 1)].re) + fabs(A[j + 21 * (j - 1)].
                                                                       im) <= b_atol) {
                            A[j + 21 * (j - 1)].re = 0.0;
                            A[j + 21 * (j - 1)].im = 0.0;
                            ilazro = true;
                        } else {
                            ilazro = false;
                        }

                        if (ilazro) {
                            ifirst = j + 1;
                            goto70 = true;
                            exitg3 = true;
                        } else {
                            j--;
                        }
                    }
                }

                if (goto60 || goto70) {
                    ilazro = true;
                } else {
                    ilazro = false;
                }

                if (!ilazro) {
                    for (i = 0; i < 21; i++) {
                        alpha1[i].re = rtNaN;
                        alpha1[i].im = 0.0;
                        beta1[i].re = rtNaN;
                        beta1[i].im = 0.0;
                    }

                    for (i = 0; i < 441; i++) {
                        Z[i].re = rtNaN;
                        Z[i].im = 0.0;
                    }

                    *info = -1.0;
                    exitg1 = 1;
                } else {
                    b_guard1 = false;
                    if (goto60) {
                        goto60 = false;
                        alpha1[ilast] = A[ilast + 21 * ilast];
                        ilast = ilastm1;
                        ilastm1--;
                        if (ilast + 1 < ilo) {
                            failed = false;
                            guard2 = true;
                            exitg1 = 1;
                        } else {
                            iiter = 0;
                            eshift_re = 0.0;
                            eshift_im = 0.0;
                            b_guard1 = true;
                        }
                    } else {
                        if (goto70) {
                            goto70 = false;
                            iiter++;
                            if (b_mod(iiter) != 0) {
                                b_A.re = -(A[ilast + 21 * ilast].re - A[ilastm1 + 21 * ilastm1].
                                        re);
                                b_A.im = -(A[ilast + 21 * ilast].im - A[ilastm1 + 21 * ilastm1].
                                        im);
                                t1 = eml_div(b_A, 2.0);
                                anorm = A[ilastm1 + 21 * ilast].re * A[ilast + 21 * ilastm1].re
                                        - A[ilastm1 + 21 * ilast].im * A[ilast + 21 * ilastm1].im;
                                temp2 = A[ilastm1 + 21 * ilast].re * A[ilast + 21 * ilastm1].im
                                        + A[ilastm1 + 21 * ilast].im * A[ilast + 21 * ilastm1].re;
                                d.re = (t1.re * t1.re - t1.im * t1.im) + anorm;
                                d.im = (t1.re * t1.im + t1.im * t1.re) + temp2;
                                b_sqrt(&d);
                                rho_re = A[ilastm1 + 21 * ilastm1].re - (t1.re - d.re);
                                rho_im = A[ilastm1 + 21 * ilastm1].im - (t1.im - d.im);
                                sigma2_re = A[ilastm1 + 21 * ilastm1].re - (t1.re + d.re);
                                sigma2_im = A[ilastm1 + 21 * ilastm1].im - (t1.im + d.im);
                                anorm = rho_re - A[ilast + 21 * ilast].re;
                                temp2 = rho_im - A[ilast + 21 * ilast].im;
                                tempr = sigma2_re - A[ilast + 21 * ilast].re;
                                g2s = sigma2_im - A[ilast + 21 * ilast].im;
                                if (hypot(anorm, temp2) <= hypot(tempr, g2s)) {
                                    sigma2_re = rho_re;
                                    sigma2_im = rho_im;
                                    rho_re = t1.re - d.re;
                                    rho_im = t1.im - d.im;
                                } else {
                                    rho_re = t1.re + d.re;
                                    rho_im = t1.im + d.im;
                                }
                            } else {
                                eshift_re += A[ilast + 21 * ilastm1].re;
                                eshift_im += A[ilast + 21 * ilastm1].im;
                                sigma2_re = eshift_re;
                                sigma2_im = eshift_im;
                            }

                            j = ilastm1;
                            i = ilastm1 + 1;
                            exitg2 = false;
                            while ((!exitg2) && (j + 1 > ifirst)) {
                                istart = j + 1;
                                ctemp_re = A[j + 21 * j].re - sigma2_re;
                                ctemp_im = A[j + 21 * j].im - sigma2_im;
                                anorm = ascale * (fabs(ctemp_re) + fabs(ctemp_im));
                                temp2 = ascale * (fabs(A[i + 21 * j].re) + fabs(A[i + 21 * j].im));
                                tempr = anorm;
                                if (temp2 > anorm) {
                                    tempr = temp2;
                                }

                                if ((tempr < 1.0) && (tempr != 0.0)) {
                                    anorm /= tempr;
                                    temp2 /= tempr;
                                }

                                if ((fabs(A[j + 21 * (j - 1)].re) + fabs(A[j + 21 * (j - 1)].im))
                                        * temp2 <= anorm * b_atol) {
                                    goto90 = true;
                                    exitg2 = true;
                                } else {
                                    i = j;
                                    j--;
                                }
                            }

                            if (!goto90) {
                                istart = ifirst;
                                if (ifirst == ilastm1 + 1) {
                                    ctemp_re = rho_re;
                                    ctemp_im = rho_im;
                                } else {
                                    ctemp_re = A[(ifirst + 21 * (ifirst - 1)) - 1].re - sigma2_re;
                                    ctemp_im = A[(ifirst + 21 * (ifirst - 1)) - 1].im - sigma2_im;
                                }

                                goto90 = true;
                            }
                        }

                        if (goto90) {
                            goto90 = false;
                            anorm = fabs(ctemp_re);
                            tempr = fabs(ctemp_im);
                            if (tempr > anorm) {
                                anorm = tempr;
                            }

                            tempr = fabs(A[istart + 21 * (istart - 1)].re);
                            temp2 = fabs(A[istart + 21 * (istart - 1)].im);
                            if (temp2 > tempr) {
                                tempr = temp2;
                            }

                            if (tempr > anorm) {
                                anorm = tempr;
                            }

                            t1.re = ctemp_re;
                            t1.im = ctemp_im;
                            d = A[istart + 21 * (istart - 1)];
                            guard3 = false;
                            if (anorm >= 7.4428285367870146E+137) {
                                do {
                                    t1.re *= 1.3435752215134178E-138;
                                    t1.im *= 1.3435752215134178E-138;
                                    d.re *= 1.3435752215134178E-138;
                                    d.im *= 1.3435752215134178E-138;
                                    anorm *= 1.3435752215134178E-138;
                                } while (!(anorm < 7.4428285367870146E+137));

                                guard3 = true;
                            } else if (anorm <= 1.3435752215134178E-138) {
                                ilazro = ((A[istart + 21 * (istart - 1)].re == 0.0) && (A[istart
                                                                                        + 21 * (istart - 1)].im == 0.0));
                                if (ilazro) {
                                    sigma2_re = 1.0;
                                    t1.re = 0.0;
                                    t1.im = 0.0;
                                } else {
                                    do {
                                        t1.re *= 7.4428285367870146E+137;
                                        t1.im *= 7.4428285367870146E+137;
                                        d.re *= 7.4428285367870146E+137;
                                        d.im *= 7.4428285367870146E+137;
                                        anorm *= 7.4428285367870146E+137;
                                    } while (!(anorm > 1.3435752215134178E-138));

                                    guard3 = true;
                                }
                            } else {
                                guard3 = true;
                            }

                            if (guard3) {
                                anorm = t1.re * t1.re + t1.im * t1.im;
                                tempr = d.re * d.re + d.im * d.im;
                                temp2 = tempr;
                                if (1.0 > tempr) {
                                    temp2 = 1.0;
                                }

                                if (anorm <= temp2 * 2.0041683600089728E-292) {
                                    if ((ctemp_re == 0.0) && (ctemp_im == 0.0)) {
                                        sigma2_re = 0.0;
                                        tempr = hypot(d.re, d.im);
                                        t1.re = d.re / tempr;
                                        t1.im = -d.im / tempr;
                                    } else {
                                        g2s = sqrt(tempr);
                                        sigma2_re = hypot(t1.re, t1.im) / g2s;
                                        tempr = fabs(ctemp_re);
                                        temp2 = fabs(ctemp_im);
                                        if (temp2 > tempr) {
                                            tempr = temp2;
                                        }

                                        if (tempr > 1.0) {
                                            tempr = hypot(ctemp_re, ctemp_im);
                                            t1.re = ctemp_re / tempr;
                                            t1.im = ctemp_im / tempr;
                                        } else {
                                            anorm = 7.4428285367870146E+137 * ctemp_re;
                                            temp2 = 7.4428285367870146E+137 * ctemp_im;
                                            tempr = hypot(anorm, temp2);
                                            t1.re = anorm / tempr;
                                            t1.im = temp2 / tempr;
                                        }

                                        temp2 = d.re / g2s;
                                        anorm = -d.im / g2s;
                                        g2s = t1.re;
                                        t1.re = t1.re * temp2 - t1.im * anorm;
                                        t1.im = g2s * anorm + t1.im * temp2;
                                    }
                                } else {
                                    temp2 = sqrt(1.0 + tempr / anorm);
                                    sigma2_re = 1.0 / temp2;
                                    tempr += anorm;
                                    g2s = temp2 * t1.re / tempr;
                                    anorm = temp2 * t1.im / tempr;
                                    t1.re = g2s * d.re - anorm * -d.im;
                                    t1.im = g2s * -d.im + anorm * d.re;
                                }
                            }

                            j = istart;
                            i = istart - 2;
                            while (j < ilast + 1) {
                                if (j > istart) {
                                    c_A = A[(j + 21 * i) - 1];
                                    d_A = A[j + 21 * i];
                                    eml_matlab_zlartg(c_A, d_A, &sigma2_re, &t1, &dc1);
                                    A[(j + 21 * i) - 1] = dc1;
                                    A[j + 21 * i].re = 0.0;
                                    A[j + 21 * i].im = 0.0;
                                }

                                for (i = j - 1; i + 1 < 22; i++) {
                                    g2s = t1.re * A[j + 21 * i].re - t1.im * A[j + 21 * i].im;
                                    anorm = t1.re * A[j + 21 * i].im + t1.im * A[j + 21 * i].re;
                                    d.re = sigma2_re * A[(j + 21 * i) - 1].re + g2s;
                                    d.im = sigma2_re * A[(j + 21 * i) - 1].im + anorm;
                                    anorm = A[(j + 21 * i) - 1].re;
                                    temp2 = A[(j + 21 * i) - 1].im;
                                    tempr = A[(j + 21 * i) - 1].im;
                                    g2s = A[(j + 21 * i) - 1].re;
                                    A[j + 21 * i].re = sigma2_re * A[j + 21 * i].re - (t1.re *
                                                                                       anorm + t1.im * temp2);
                                    A[j + 21 * i].im = sigma2_re * A[j + 21 * i].im - (t1.re *
                                                                                       tempr - t1.im * g2s);
                                    A[(j + 21 * i) - 1] = d;
                                }

                                t1.re = -t1.re;
                                t1.im = -t1.im;
                                x = j;
                                if (ilast + 1 < j + 2) {
                                    x = ilast - 1;
                                }

                                for (i = 0; i + 1 <= x + 2; i++) {
                                    g2s = t1.re * A[i + 21 * (j - 1)].re - t1.im * A[i + 21 * (j -
                                                                                               1)].im;
                                    anorm = t1.re * A[i + 21 * (j - 1)].im + t1.im * A[i + 21 * (j
                                                                                                 - 1)].re;
                                    d.re = sigma2_re * A[i + 21 * j].re + g2s;
                                    d.im = sigma2_re * A[i + 21 * j].im + anorm;
                                    anorm = A[i + 21 * j].re;
                                    temp2 = A[i + 21 * j].im;
                                    tempr = A[i + 21 * j].im;
                                    g2s = A[i + 21 * j].re;
                                    A[i + 21 * (j - 1)].re = sigma2_re * A[i + 21 * (j - 1)].re -
                                            (t1.re * anorm + t1.im * temp2);
                                    A[i + 21 * (j - 1)].im = sigma2_re * A[i + 21 * (j - 1)].im -
                                            (t1.re * tempr - t1.im * g2s);
                                    A[i + 21 * j] = d;
                                }

                                for (i = 0; i < 21; i++) {
                                    g2s = t1.re * Z[i + 21 * (j - 1)].re - t1.im * Z[i + 21 * (j -
                                                                                               1)].im;
                                    anorm = t1.re * Z[i + 21 * (j - 1)].im + t1.im * Z[i + 21 * (j
                                                                                                 - 1)].re;
                                    d.re = sigma2_re * Z[i + 21 * j].re + g2s;
                                    d.im = sigma2_re * Z[i + 21 * j].im + anorm;
                                    anorm = Z[i + 21 * j].re;
                                    temp2 = Z[i + 21 * j].im;
                                    tempr = Z[i + 21 * j].im;
                                    g2s = Z[i + 21 * j].re;
                                    Z[i + 21 * (j - 1)].re = sigma2_re * Z[i + 21 * (j - 1)].re -
                                            (t1.re * anorm + t1.im * temp2);
                                    Z[i + 21 * (j - 1)].im = sigma2_re * Z[i + 21 * (j - 1)].im -
                                            (t1.re * tempr - t1.im * g2s);
                                    Z[i + 21 * j] = d;
                                }

                                i = j - 1;
                                j++;
                            }
                        }

                        b_guard1 = true;
                    }

                    if (b_guard1) {
                        jiter++;
                    }
                }
            } else {
                guard2 = true;
                exitg1 = 1;
            }
        } while (exitg1 == 0);
    } else {
        guard1 = true;
    }

    if (guard2) {
        if (failed) {
            *info = ilast + 1;
            for (i = 0; i + 1 <= ilast + 1; i++) {
                alpha1[i].re = rtNaN;
                alpha1[i].im = 0.0;
                beta1[i].re = rtNaN;
                beta1[i].im = 0.0;
            }

            for (i = 0; i < 441; i++) {
                Z[i].re = rtNaN;
                Z[i].im = 0.0;
            }
        } else {
            guard1 = true;
        }
    }

    if (guard1) {
        for (j = 0; j + 1 < ilo; j++) {
            alpha1[j] = A[j + 21 * j];
        }

        *info = 0.0;
    }
}

//
// Arguments    : const creal_T x[441]
// Return Type  : double
//
static double eml_matlab_zlangeM(const creal_T x[441])
{
    double y;
    int k;
    boolean_T exitg1;
    double absxk;
    y = 0.0;
    k = 0;
    exitg1 = false;
    while ((!exitg1) && (k < 441)) {
        absxk = hypot(x[k].re, x[k].im);
        if (rtIsNaN(absxk)) {
            y = rtNaN;
            exitg1 = true;
        } else {
            if (absxk > y) {
                y = absxk;
            }

            k++;
        }
    }

    return y;
}

//
// Arguments    : const creal_T A[441]
//                int ilo
//                int ihi
// Return Type  : double
//
static double eml_matlab_zlanhs(const creal_T A[441], int ilo, int ihi)
{
    double f;
    double scale;
    double sumsq;
    boolean_T firstNonZero;
    int j;
    int i3;
    int i;
    double reAij;
    double imAij;
    double temp2;
    f = 0.0;
    if (ilo > ihi) {
    } else {
        scale = 0.0;
        sumsq = 0.0;
        firstNonZero = true;
        for (j = ilo; j <= ihi; j++) {
            i3 = j + 1;
            if (ihi < j + 1) {
                i3 = ihi;
            }

            for (i = ilo; i <= i3; i++) {
                reAij = A[(i + 21 * (j - 1)) - 1].re;
                imAij = A[(i + 21 * (j - 1)) - 1].im;
                if (reAij != 0.0) {
                    reAij = fabs(reAij);
                    if (firstNonZero) {
                        sumsq = 1.0;
                        scale = reAij;
                        firstNonZero = false;
                    } else if (scale < reAij) {
                        temp2 = scale / reAij;
                        sumsq = 1.0 + sumsq * temp2 * temp2;
                        scale = reAij;
                    } else {
                        temp2 = reAij / scale;
                        sumsq += temp2 * temp2;
                    }
                }

                if (imAij != 0.0) {
                    reAij = fabs(imAij);
                    if (firstNonZero) {
                        sumsq = 1.0;
                        scale = reAij;
                        firstNonZero = false;
                    } else if (scale < reAij) {
                        temp2 = scale / reAij;
                        sumsq = 1.0 + sumsq * temp2 * temp2;
                        scale = reAij;
                    } else {
                        temp2 = reAij / scale;
                        sumsq += temp2 * temp2;
                    }
                }
            }
        }

        f = scale * sqrt(sumsq);
    }

    return f;
}

//
// Arguments    : const creal_T f
//                const creal_T g
//                double *cs
//                creal_T *sn
//                creal_T *r
// Return Type  : void
//
static void eml_matlab_zlartg(const creal_T f, const creal_T g, double *cs,
                              creal_T *sn, creal_T *r)
{
    double scale;
    double f2s;
    double g2;
    double fs_re;
    double fs_im;
    double gs_re;
    double gs_im;
    int count;
    int rescaledir;
    boolean_T guard1 = false;
    double g2s;
    scale = fabs(f.re);
    f2s = fabs(f.im);
    if (f2s > scale) {
        scale = f2s;
    }

    f2s = fabs(g.re);
    g2 = fabs(g.im);
    if (g2 > f2s) {
        f2s = g2;
    }

    if (f2s > scale) {
        scale = f2s;
    }

    fs_re = f.re;
    fs_im = f.im;
    gs_re = g.re;
    gs_im = g.im;
    count = 0;
    rescaledir = 0;
    guard1 = false;
    if (scale >= 7.4428285367870146E+137) {
        do {
            count++;
            fs_re *= 1.3435752215134178E-138;
            fs_im *= 1.3435752215134178E-138;
            gs_re *= 1.3435752215134178E-138;
            gs_im *= 1.3435752215134178E-138;
            scale *= 1.3435752215134178E-138;
        } while (!(scale < 7.4428285367870146E+137));

        rescaledir = 1;
        guard1 = true;
    } else if (scale <= 1.3435752215134178E-138) {
        if ((g.re == 0.0) && (g.im == 0.0)) {
            *cs = 1.0;
            sn->re = 0.0;
            sn->im = 0.0;
            *r = f;
        } else {
            do {
                count++;
                fs_re *= 7.4428285367870146E+137;
                fs_im *= 7.4428285367870146E+137;
                gs_re *= 7.4428285367870146E+137;
                gs_im *= 7.4428285367870146E+137;
                scale *= 7.4428285367870146E+137;
            } while (!(scale > 1.3435752215134178E-138));

            rescaledir = -1;
            guard1 = true;
        }
    } else {
        guard1 = true;
    }

    if (guard1) {
        scale = fs_re * fs_re + fs_im * fs_im;
        g2 = gs_re * gs_re + gs_im * gs_im;
        f2s = g2;
        if (1.0 > g2) {
            f2s = 1.0;
        }

        if (scale <= f2s * 2.0041683600089728E-292) {
            if ((f.re == 0.0) && (f.im == 0.0)) {
                *cs = 0.0;
                r->re = hypot(g.re, g.im);
                r->im = 0.0;
                f2s = hypot(gs_re, gs_im);
                sn->re = gs_re / f2s;
                sn->im = -gs_im / f2s;
            } else {
                g2s = sqrt(g2);
                *cs = hypot(fs_re, fs_im) / g2s;
                f2s = fabs(f.re);
                g2 = fabs(f.im);
                if (g2 > f2s) {
                    f2s = g2;
                }

                if (f2s > 1.0) {
                    f2s = hypot(f.re, f.im);
                    fs_re = f.re / f2s;
                    fs_im = f.im / f2s;
                } else {
                    g2 = 7.4428285367870146E+137 * f.re;
                    scale = 7.4428285367870146E+137 * f.im;
                    f2s = hypot(g2, scale);
                    fs_re = g2 / f2s;
                    fs_im = scale / f2s;
                }

                gs_re /= g2s;
                gs_im = -gs_im / g2s;
                sn->re = fs_re * gs_re - fs_im * gs_im;
                sn->im = fs_re * gs_im + fs_im * gs_re;
                r->re = *cs * f.re + (sn->re * g.re - sn->im * g.im);
                r->im = *cs * f.im + (sn->re * g.im + sn->im * g.re);
            }
        } else {
            f2s = sqrt(1.0 + g2 / scale);
            r->re = f2s * fs_re;
            r->im = f2s * fs_im;
            *cs = 1.0 / f2s;
            f2s = scale + g2;
            g2 = r->re / f2s;
            f2s = r->im / f2s;
            sn->re = g2 * gs_re - f2s * -gs_im;
            sn->im = g2 * -gs_im + f2s * gs_re;
            if (rescaledir > 0) {
                for (rescaledir = 1; rescaledir <= count; rescaledir++) {
                    r->re *= 7.4428285367870146E+137;
                    r->im *= 7.4428285367870146E+137;
                }
            } else {
                if (rescaledir < 0) {
                    for (rescaledir = 1; rescaledir <= count; rescaledir++) {
                        r->re *= 1.3435752215134178E-138;
                        r->im *= 1.3435752215134178E-138;
                    }
                }
            }
        }
    }
}

//
// Arguments    : double cfrom
//                double cto
//                creal_T A[441]
// Return Type  : void
//
static void eml_matlab_zlascl(double cfrom, double cto, creal_T A[441])
{
    double cfromc;
    double ctoc;
    boolean_T notdone;
    double cfrom1;
    double cto1;
    double mul;
    int i6;
    int i7;
    cfromc = cfrom;
    ctoc = cto;
    notdone = true;
    while (notdone) {
        cfrom1 = cfromc * 2.0041683600089728E-292;
        cto1 = ctoc / 4.9896007738368E+291;
        if ((fabs(cfrom1) > fabs(ctoc)) && (ctoc != 0.0)) {
            mul = 2.0041683600089728E-292;
            cfromc = cfrom1;
        } else if (fabs(cto1) > fabs(cfromc)) {
            mul = 4.9896007738368E+291;
            ctoc = cto1;
        } else {
            mul = ctoc / cfromc;
            notdone = false;
        }

        for (i6 = 0; i6 < 21; i6++) {
            for (i7 = 0; i7 < 21; i7++) {
                A[i7 + 21 * i6].re *= mul;
                A[i7 + 21 * i6].im *= mul;
            }
        }
    }
}

//
// Arguments    : const creal_T A[441]
//                creal_T V[441]
// Return Type  : void
//
static void eml_matlab_ztgevc(const creal_T A[441], creal_T V[441])
{
    double rworka[21];
    double anorm;
    int j;
    int jc;
    double d_re;
    double scale;
    double ascale;
    int je;
    double temp;
    double salpha_re;
    double salpha_im;
    double acoeff;
    boolean_T b0;
    boolean_T b1;
    double y;
    double acoefa;
    creal_T work1[21];
    int jr;
    double dmin;
    double d_im;
    double work1_re;
    double work1_im;
    creal_T work2[21];
    memset(&rworka[0], 0, 21U * sizeof(double));
    anorm = fabs(A[0].re) + fabs(A[0].im);
    for (j = 0; j < 20; j++) {
        for (jc = 0; jc <= j; jc++) {
            rworka[1 + j] += fabs(A[jc + 21 * (j + 1)].re) + fabs(A[jc + 21 * (j + 1)]
                    .im);
        }

        d_re = rworka[1 + j] + (fabs(A[(j + 21 * (j + 1)) + 1].re) + fabs(A[(j + 21 *
                                                                             (j + 1)) + 1].im));
        if (d_re > anorm) {
            anorm = d_re;
        }
    }

    scale = anorm;
    if (2.2250738585072014E-308 > anorm) {
        scale = 2.2250738585072014E-308;
    }

    ascale = 1.0 / scale;
    for (je = 0; je < 21; je++) {
        scale = (fabs(A[(21 * (20 - je) - je) + 20].re) + fabs(A[(21 * (20 - je) -
                                                                  je) + 20].im)) * ascale;
        if (1.0 > scale) {
            scale = 1.0;
        }

        temp = 1.0 / scale;
        salpha_re = ascale * (temp * A[(21 * (20 - je) - je) + 20].re);
        salpha_im = ascale * (temp * A[(21 * (20 - je) - je) + 20].im);
        acoeff = temp * ascale;
        if ((fabs(temp) >= 2.2250738585072014E-308) && (fabs(acoeff) <
                                                        2.1043767780094214E-291)) {
            b0 = true;
        } else {
            b0 = false;
        }

        if ((fabs(salpha_re) + fabs(salpha_im) >= 2.2250738585072014E-308) && (fabs
                                                                               (salpha_re) + fabs(salpha_im) < 2.1043767780094214E-291)) {
            b1 = true;
        } else {
            b1 = false;
        }

        scale = 1.0;
        if (b0) {
            scale = anorm;
            if (4.7520007369874279E+290 < anorm) {
                scale = 4.7520007369874279E+290;
            }

            scale *= 2.1043767780094214E-291 / fabs(temp);
        }

        if (b1) {
            d_re = 2.1043767780094214E-291 / (fabs(salpha_re) + fabs(salpha_im));
            if (d_re > scale) {
                scale = d_re;
            }
        }

        if (b0 || b1) {
            d_re = fabs(acoeff);
            y = fabs(salpha_re) + fabs(salpha_im);
            if (1.0 > d_re) {
                d_re = 1.0;
            }

            if (y > d_re) {
                d_re = y;
            }

            d_re = 1.0 / (2.2250738585072014E-308 * d_re);
            if (d_re < scale) {
                scale = d_re;
            }

            if (b0) {
                acoeff = ascale * (scale * temp);
            } else {
                acoeff *= scale;
            }

            if (b1) {
                salpha_re *= scale;
                salpha_im *= scale;
            } else {
                salpha_re *= scale;
                salpha_im *= scale;
            }
        }

        acoefa = fabs(acoeff);
        for (jr = 0; jr < 21; jr++) {
            work1[jr].re = 0.0;
            work1[jr].im = 0.0;
        }

        work1[20 - je].re = 1.0;
        work1[20 - je].im = 0.0;
        dmin = 2.2204460492503131E-16 * acoefa * anorm;
        d_re = 2.2204460492503131E-16 * (fabs(salpha_re) + fabs(salpha_im));
        if (d_re > dmin) {
            dmin = d_re;
        }

        if (2.2250738585072014E-308 > dmin) {
            dmin = 2.2250738585072014E-308;
        }

        for (jr = 0; jr <= 19 - je; jr++) {
            work1[jr].re = acoeff * A[jr + 21 * (20 - je)].re;
            work1[jr].im = acoeff * A[jr + 21 * (20 - je)].im;
        }

        work1[20 - je].re = 1.0;
        work1[20 - je].im = 0.0;
        for (j = -19; j + 19 <= 19 - je; j++) {
            jc = -je - j;
            d_re = acoeff * A[jc + 21 * jc].re - salpha_re;
            d_im = acoeff * A[jc + 21 * jc].im - salpha_im;
            if (fabs(d_re) + fabs(d_im) <= dmin) {
                d_re = dmin;
                d_im = 0.0;
            }

            if ((fabs(d_re) + fabs(d_im) < 1.0) && (fabs(work1[jc].re) + fabs(work1[jc]
                                                                              .im) >= 2.1401108748360902E+306 * (fabs(d_re) + fabs(d_im)))) {
                temp = 1.0 / (fabs(work1[jc].re) + fabs(work1[jc].im));
                for (jr = 0; jr <= 20 - je; jr++) {
                    work1[jr].re *= temp;
                    work1[jr].im *= temp;
                }
            }

            work1_re = -work1[jc].re;
            work1_im = -work1[jc].im;
            if (d_im == 0.0) {
                if (-work1[jc].im == 0.0) {
                    work1[jc].re = -work1[jc].re / d_re;
                    work1[jc].im = 0.0;
                } else if (-work1[jc].re == 0.0) {
                    work1[jc].re = 0.0;
                    work1[jc].im = work1_im / d_re;
                } else {
                    work1[jc].re = -work1[jc].re / d_re;
                    work1[jc].im = work1_im / d_re;
                }
            } else if (d_re == 0.0) {
                if (-work1[jc].re == 0.0) {
                    work1[jc].re = -work1[jc].im / d_im;
                    work1[jc].im = 0.0;
                } else if (-work1[jc].im == 0.0) {
                    work1[jc].re = 0.0;
                    work1[jc].im = -(work1_re / d_im);
                } else {
                    work1[jc].re = -work1[jc].im / d_im;
                    work1[jc].im = -(work1_re / d_im);
                }
            } else {
                temp = fabs(d_re);
                scale = fabs(d_im);
                if (temp > scale) {
                    y = d_im / d_re;
                    scale = d_re + y * d_im;
                    work1[jc].re = (-work1[jc].re + y * -work1[jc].im) / scale;
                    work1[jc].im = (work1_im - y * work1_re) / scale;
                } else if (scale == temp) {
                    if (d_re > 0.0) {
                        y = 0.5;
                    } else {
                        y = -0.5;
                    }

                    if (d_im > 0.0) {
                        scale = 0.5;
                    } else {
                        scale = -0.5;
                    }

                    work1[jc].re = (-work1[jc].re * y + -work1[jc].im * scale) / temp;
                    work1[jc].im = (work1_im * y - work1_re * scale) / temp;
                } else {
                    y = d_re / d_im;
                    scale = d_im + y * d_re;
                    work1[jc].re = (y * -work1[jc].re + -work1[jc].im) / scale;
                    work1[jc].im = (y * work1_im - work1_re) / scale;
                }
            }

            if (jc + 1 > 1) {
                if (fabs(work1[jc].re) + fabs(work1[jc].im) > 1.0) {
                    temp = 1.0 / (fabs(work1[jc].re) + fabs(work1[jc].im));
                    if (acoefa * rworka[jc] >= 2.1401108748360902E+306 * temp) {
                        for (jr = 0; jr <= 20 - je; jr++) {
                            work1[jr].re *= temp;
                            work1[jr].im *= temp;
                        }
                    }
                }

                d_re = acoeff * work1[jc].re;
                d_im = acoeff * work1[jc].im;
                for (jr = 0; jr < jc; jr++) {
                    work1[jr].re += d_re * A[jr + 21 * jc].re - d_im * A[jr + 21 * jc].im;
                    work1[jr].im += d_re * A[jr + 21 * jc].im + d_im * A[jr + 21 * jc].re;
                }
            }
        }

        for (jr = 0; jr < 21; jr++) {
            work2[jr].re = 0.0;
            work2[jr].im = 0.0;
        }

        for (jc = 0; jc <= 20 - je; jc++) {
            for (jr = 0; jr < 21; jr++) {
                scale = V[jr + 21 * jc].re * work1[jc].re - V[jr + 21 * jc].im *
                        work1[jc].im;
                y = V[jr + 21 * jc].re * work1[jc].im + V[jr + 21 * jc].im * work1[jc].
                        re;
                work2[jr].re += scale;
                work2[jr].im += y;
            }
        }

        scale = fabs(work2[0].re) + fabs(work2[0].im);
        for (jr = 0; jr < 20; jr++) {
            d_re = fabs(work2[jr + 1].re) + fabs(work2[jr + 1].im);
            if (d_re > scale) {
                scale = d_re;
            }
        }

        if (scale > 2.2250738585072014E-308) {
            temp = 1.0 / scale;
            for (jr = 0; jr < 21; jr++) {
                V[jr + 21 * (20 - je)].re = temp * work2[jr].re;
                V[jr + 21 * (20 - je)].im = temp * work2[jr].im;
            }
        } else {
            for (jr = 0; jr < 21; jr++) {
                V[jr + 21 * (20 - je)].re = 0.0;
                V[jr + 21 * (20 - je)].im = 0.0;
            }
        }
    }
}

//
// Arguments    : const double x[21]
//                double y[21]
//                int idx[21]
// Return Type  : void
//
static void eml_sort(const double x[21], double y[21], int idx[21])
{
    int k;
    boolean_T p;
    signed char idx0[21];
    int i;
    int i2;
    int j;
    int pEnd;
    int b_p;
    int q;
    int qEnd;
    int kEnd;
    for (k = 0; k < 21; k++) {
        idx[k] = k + 1;
    }

    for (k = 0; k < 19; k += 2) {
        if ((x[k] <= x[k + 1]) || rtIsNaN(x[k + 1])) {
            p = true;
        } else {
            p = false;
        }

        if (p) {
        } else {
            idx[k] = k + 2;
            idx[k + 1] = k + 1;
        }
    }

    for (i = 0; i < 21; i++) {
        idx0[i] = 1;
    }

    i = 2;
    while (i < 21) {
        i2 = i << 1;
        j = 1;
        for (pEnd = 1 + i; pEnd < 22; pEnd = qEnd + i) {
            b_p = j;
            q = pEnd - 1;
            qEnd = j + i2;
            if (qEnd > 22) {
                qEnd = 22;
            }

            k = 0;
            kEnd = qEnd - j;
            while (k + 1 <= kEnd) {
                if ((x[idx[b_p - 1] - 1] <= x[idx[q] - 1]) || rtIsNaN(x[idx[q] - 1])) {
                    p = true;
                } else {
                    p = false;
                }

                if (p) {
                    idx0[k] = (signed char)idx[b_p - 1];
                    b_p++;
                    if (b_p == pEnd) {
                        while (q + 1 < qEnd) {
                            k++;
                            idx0[k] = (signed char)idx[q];
                            q++;
                        }
                    }
                } else {
                    idx0[k] = (signed char)idx[q];
                    q++;
                    if (q + 1 == qEnd) {
                        while (b_p < pEnd) {
                            k++;
                            idx0[k] = (signed char)idx[b_p - 1];
                            b_p++;
                        }
                    }
                }

                k++;
            }

            for (k = 0; k + 1 <= kEnd; k++) {
                idx[(j + k) - 1] = idx0[k];
            }

            j = qEnd;
        }

        i = i2;
    }

    for (k = 0; k < 21; k++) {
        y[k] = x[idx[k] - 1];
    }
}

//
// Arguments    : const creal_T x[441]
//                int ix0
// Return Type  : double
//
static double eml_xnrm2(const creal_T x[441], int ix0)
{
    double y;
    double scale;
    int k;
    double absxk;
    double t;
    y = 0.0;
    scale = 2.2250738585072014E-308;
    for (k = ix0; k <= ix0 + 20; k++) {
        absxk = fabs(x[k - 1].re);
        if (absxk > scale) {
            t = scale / absxk;
            y = 1.0 + y * t * t;
            scale = absxk;
        } else {
            t = absxk / scale;
            y += t * t;
        }

        absxk = fabs(x[k - 1].im);
        if (absxk > scale) {
            t = scale / absxk;
            y = 1.0 + y * t * t;
            scale = absxk;
        } else {
            t = absxk / scale;
            y += t * t;
        }
    }

    return scale * sqrt(y);
}

//
// Arguments    : double x[441]
//                int ix0
//                int iy0
// Return Type  : void
//
static void eml_xswap(double x[441], int ix0, int iy0)
{
    int ix;
    int iy;
    int k;
    double temp;
    ix = ix0 - 1;
    iy = iy0 - 1;
    for (k = 0; k < 21; k++) {
        temp = x[ix];
        x[ix] = x[iy];
        x[iy] = temp;
        ix += 21;
        iy += 21;
    }
}

//
// Arguments    : creal_T x[441]
// Return Type  : void
//
static void fliplr(creal_T x[441])
{
    int b_j1;
    int i;
    double xtmp_re;
    double xtmp_im;
    for (b_j1 = 0; b_j1 < 10; b_j1++) {
        for (i = 0; i < 21; i++) {
            xtmp_re = x[i + 21 * b_j1].re;
            xtmp_im = x[i + 21 * b_j1].im;
            x[i + 21 * b_j1] = x[i + 21 * (20 - b_j1)];
            x[i + 21 * (20 - b_j1)].re = xtmp_re;
            x[i + 21 * (20 - b_j1)].im = xtmp_im;
        }
    }
}

//
// Arguments    : double x[21]
// Return Type  : void
//
static void flipud(double x[21])
{
    int i;
    double xtmp;
    for (i = 0; i < 10; i++) {
        xtmp = x[i];
        x[i] = x[20 - i];
        x[20 - i] = xtmp;
    }
}

//
// Arguments    : const double x[64]
//                double y[64]
// Return Type  : void
//
static void inv(const double x[64], double y[64])
{
    invNxN(x, y);
}

//
// Arguments    : const double x[64]
//                double y[64]
// Return Type  : void
//
static void invNxN(const double x[64], double y[64])
{
    double A[64];
    int i2;
    signed char ipiv[8];
    int j;
    int c;
    int jBcol;
    int ix;
    double smax;
    int k;
    double s;
    int i;
    int kAcol;
    signed char p[8];
    for (i2 = 0; i2 < 64; i2++) {
        y[i2] = 0.0;
        A[i2] = x[i2];
    }

    for (i2 = 0; i2 < 8; i2++) {
        ipiv[i2] = (signed char)(1 + i2);
    }

    for (j = 0; j < 7; j++) {
        c = j * 9;
        jBcol = 0;
        ix = c;
        smax = fabs(A[c]);
        for (k = 2; k <= 8 - j; k++) {
            ix++;
            s = fabs(A[ix]);
            if (s > smax) {
                jBcol = k - 1;
                smax = s;
            }
        }

        if (A[c + jBcol] != 0.0) {
            if (jBcol != 0) {
                ipiv[j] = (signed char)((j + jBcol) + 1);
                ix = j;
                jBcol += j;
                for (k = 0; k < 8; k++) {
                    smax = A[ix];
                    A[ix] = A[jBcol];
                    A[jBcol] = smax;
                    ix += 8;
                    jBcol += 8;
                }
            }

            i2 = (c - j) + 8;
            for (i = c + 1; i + 1 <= i2; i++) {
                A[i] /= A[c];
            }
        }

        jBcol = c;
        kAcol = c + 8;
        for (i = 1; i <= 7 - j; i++) {
            smax = A[kAcol];
            if (A[kAcol] != 0.0) {
                ix = c + 1;
                i2 = (jBcol - j) + 16;
                for (k = 9 + jBcol; k + 1 <= i2; k++) {
                    A[k] += A[ix] * -smax;
                    ix++;
                }
            }

            kAcol += 8;
            jBcol += 8;
        }
    }

    for (i2 = 0; i2 < 8; i2++) {
        p[i2] = (signed char)(1 + i2);
    }

    for (k = 0; k < 7; k++) {
        if (ipiv[k] > 1 + k) {
            jBcol = p[ipiv[k] - 1];
            p[ipiv[k] - 1] = p[k];
            p[k] = (signed char)jBcol;
        }
    }

    for (k = 0; k < 8; k++) {
        y[k + ((p[k] - 1) << 3)] = 1.0;
        for (j = k; j + 1 < 9; j++) {
            if (y[j + ((p[k] - 1) << 3)] != 0.0) {
                for (i = j + 1; i + 1 < 9; i++) {
                    y[i + ((p[k] - 1) << 3)] -= y[j + ((p[k] - 1) << 3)] * A[i + (j << 3)];
                }
            }
        }
    }

    for (j = 0; j < 8; j++) {
        jBcol = j << 3;
        for (k = 7; k > -1; k += -1) {
            kAcol = k << 3;
            if (y[k + jBcol] != 0.0) {
                y[k + jBcol] /= A[k + kAcol];
                for (i = 0; i + 1 <= k; i++) {
                    y[i + jBcol] -= y[k + jBcol] * A[i + kAcol];
                }
            }
        }
    }
}

//
// Arguments    : const double A[441]
//                double B[168]
// Return Type  : void
//
static void mldivide(const double A[441], double B[168])
{
    double b_A[441];
    signed char ipiv[21];
    int i4;
    int j;
    int c;
    int jBcol;
    int ix;
    double temp;
    int k;
    double s;
    int i;
    int kAcol;
    memcpy(&b_A[0], &A[0], 441U * sizeof(double));
    for (i4 = 0; i4 < 21; i4++) {
        ipiv[i4] = (signed char)(1 + i4);
    }

    for (j = 0; j < 20; j++) {
        c = j * 22;
        jBcol = 1;
        ix = c;
        temp = fabs(b_A[c]);
        for (k = 2; k <= 21 - j; k++) {
            ix++;
            s = fabs(b_A[ix]);
            if (s > temp) {
                jBcol = k;
                temp = s;
            }
        }

        if (b_A[(c + jBcol) - 1] != 0.0) {
            if (jBcol - 1 != 0) {
                ipiv[j] = (signed char)(j + jBcol);
                eml_xswap(b_A, j + 1, j + jBcol);
            }

            i4 = (c - j) + 21;
            for (i = c + 1; i + 1 <= i4; i++) {
                b_A[i] /= b_A[c];
            }
        }

        jBcol = c;
        kAcol = c + 21;
        for (i = 1; i <= 20 - j; i++) {
            temp = b_A[kAcol];
            if (b_A[kAcol] != 0.0) {
                ix = c + 1;
                i4 = (jBcol - j) + 42;
                for (k = 22 + jBcol; k + 1 <= i4; k++) {
                    b_A[k] += b_A[ix] * -temp;
                    ix++;
                }
            }

            kAcol += 21;
            jBcol += 21;
        }
    }

    for (jBcol = 0; jBcol < 20; jBcol++) {
        if (ipiv[jBcol] != jBcol + 1) {
            for (kAcol = 0; kAcol < 8; kAcol++) {
                temp = B[jBcol + 21 * kAcol];
                B[jBcol + 21 * kAcol] = B[(ipiv[jBcol] + 21 * kAcol) - 1];
                B[(ipiv[jBcol] + 21 * kAcol) - 1] = temp;
            }
        }
    }

    for (j = 0; j < 8; j++) {
        jBcol = 21 * j;
        for (k = 0; k < 21; k++) {
            kAcol = 21 * k;
            if (B[k + jBcol] != 0.0) {
                for (i = k + 1; i + 1 < 22; i++) {
                    B[i + jBcol] -= B[k + jBcol] * b_A[i + kAcol];
                }
            }
        }
    }

    for (j = 0; j < 8; j++) {
        jBcol = 21 * j;
        for (k = 20; k > -1; k += -1) {
            kAcol = 21 * k;
            if (B[k + jBcol] != 0.0) {
                temp = B[k + jBcol];
                B[k + jBcol] = temp / b_A[k + kAcol];
                for (i = 0; i + 1 <= k; i++) {
                    B[i + jBcol] -= B[k + jBcol] * b_A[i + kAcol];
                }
            }
        }
    }
}

//
// f-- the fundermental frequency
//  S-- the sampling rate
//  T-- the number of sampling points
//  N-- the number of harmonics
// Arguments    : double f
//                double S
//                double T
//                double N
//                double y[4800]
// Return Type  : void
//
static void refsig(double f, double S, double T, double N, double y[4800])
{
    int i;
    int j;
    double t;
    memset(&y[0], 0, 4800U * sizeof(double));
    for (i = 0; i < (int)N; i++) {
        for (j = 0; j < (int)T; j++) {
            t = (1.0 + (double)j) / S;
            y[((int)(2.0 * (1.0 + (double)i) - 1.0) + (j << 3)) - 1] = sin
                    (6.2831853071795862 * ((1.0 + (double)i) * f) * t);
            y[((int)(2.0 * (1.0 + (double)i)) + (j << 3)) - 1] = cos
                    (6.2831853071795862 * ((1.0 + (double)i) * f) * t);
        }
    }
}

//
// Arguments    : const double data[12600]
//                double left_fre
//                double right_fre
// Return Type  : double
//
void me_CCA(const double data[12600], double left_fre, double right_fre, double
*v, double *idx)
{
    double dv0[4800];
    double r1[21];
    creal_T unusedU1[168];
    creal_T unusedU0[441];
    double dv1[4800];
    double r2[21];
    int ixstart;
    int ix;
    boolean_T exitg3;
    double mtmp;
    boolean_T exitg2;
    int itmp;
    boolean_T exitg1;

    //  sampling rate（采样率）
    //  data length (4 s)
    //  生成时间窗向量1s 2s 3s 4s
    //  在每一个时间窗下的数据点数量
    //  stimulus frequencies 10, 9, 8, 6 Hz（参考频率）
    refsig(left_fre, 300.0, 600.0, 4.0, dv0);
    cca(data, dv0, unusedU0, unusedU1, r1);
    refsig(right_fre, 300.0, 600.0, 4.0, dv1);
    cca(data, dv1, unusedU0, unusedU1, r2);

    // idx表示类别
    ixstart = 1;
    *v = r1[0];
    if (rtIsNaN(r1[0])) {
        ix = 2;
        exitg3 = false;
        while ((!exitg3) && (ix < 22)) {
            ixstart = ix;
            if (!rtIsNaN(r1[ix - 1])) {
                *v = r1[ix - 1];
                exitg3 = true;
            } else {
                ix++;
            }
        }
    }

    if (ixstart < 21) {
        while (ixstart + 1 < 22) {
            if (r1[ixstart] > *v) {
                *v = r1[ixstart];
            }

            ixstart++;
        }
    }

    ixstart = 1;
    mtmp = r2[0];
    if (rtIsNaN(r2[0])) {
        ix = 2;
        exitg2 = false;
        while ((!exitg2) && (ix < 22)) {
            ixstart = ix;
            if (!rtIsNaN(r2[ix - 1])) {
                mtmp = r2[ix - 1];
                exitg2 = true;
            } else {
                ix++;
            }
        }
    }

    if (ixstart < 21) {
        while (ixstart + 1 < 22) {
            if (r2[ixstart] > mtmp) {
                mtmp = r2[ixstart];
            }

            ixstart++;
        }
    }

    ixstart = 1;
    itmp = 1;
    if (rtIsNaN(*v)) {
        ix = 2;
        exitg1 = false;
        while ((!exitg1) && (ix < 3)) {
            ixstart = 2;
            if (!rtIsNaN(mtmp)) {
                *v = mtmp;
                itmp = 2;
                exitg1 = true;
            } else {
                ix = 3;
            }
        }
    }

    if ((ixstart < 2) && (mtmp > *v)) {
        *v = mtmp;
        itmp = 2;
    }

    *idx = itmp;
}
