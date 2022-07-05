// Include files
#include "CCA/rt_nonfinite.h"
#include "csp_train_ovo.h"

#include <algorithm>

using namespace std;

#ifndef struct_emxArray_real_T
#define struct_emxArray_real_T

struct emxArray_real_T{
    double *data;
    int *size;
    int allocatedSize;
    int numDimensions;
    bool canFreeData;
};

#endif                                 //struct_emxArray_real_T

// Function Definitions

//
// Arguments    : const double v[8]
//                double d[64]
// Return Type  : void
//
static void b_diag(const array<double, 8> &v, array<double, 64> &d){
    d.fill(0);
    for(int j = 0; j < 8; ++j){
        d[9 * j] = v[j];
    }
}

//
// Arguments    : int n
//                const double x[8]
//                int ix0
// Return Type  : double
//
static double b_eml_xnrm2(int n, const array<double, 8> &x, int ix0){
    double y = 0.0;
    if(n == 1){
        y = fabs(x[ix0 - 1]);
    }
    else{
        double scale = 2.95073858507714E-308;
        int kend = (ix0 + n) - 1;
        for(int k = ix0; k <= kend; ++k){
            double absxk = fabs(x[k - 1]);
            double t;
            if(absxk > scale){
                t = scale / absxk;
                y = 1.0 + y * t * t;
                scale = absxk;
            }
            else{
                t = absxk / scale;
                y += t * t;
            }
        }

        y = scale * sqrt(y);
    }

    return y;
}

//
// Arguments    : int n
//                double a
//                double x[8]
//                int ix0
// Return Type  : void
//
static void b_eml_xscal(int n, double a, array<double, 8> &x, int ix0){
    int i6 = (ix0 + n) - 1;
    for(int k = ix0; k <= i6; ++k){
        x[k - 1] *= a;
    }
}

//
// Arguments    : const double v[36]
//                double d[6]
// Return Type  : void
//
static void c_diag(const array<double, 36> &v, array<double, 6> &d){
    for(int j = 0; j < 6; ++j){
        d[j] = v[j * 7];
    }
}

//
// Arguments    : int n
//                double a
//                const double x[8]
//                int ix0
//                double y[64]
//                int iy0
// Return Type  : void
//
static void c_eml_xaxpy(int n, double a, const array<double, 8> &x, int ix0,
                        array<double, 64> &y, int iy0){
    if(a != 0.0){
        int ix = ix0 - 1;
        int iy = iy0 - 1;
        for(int k = 0; k < n; ++k){
            y[iy] += a * x[ix];
            ++ix;
            ++iy;
        }
    }
}

//
// Arguments    : int n
//                double a
//                const double x[64]
//                int ix0
//                double y[8]
//                int iy0
// Return Type  : void
//
static void b_eml_xaxpy(int n, double a, const array<double, 64> &x, int ix0,
                        array<double, 8> &y, int iy0){
    if(a != 0.0){
        int ix = ix0 - 1;
        int iy = iy0 - 1;
        for(int k = 0; k < n; ++k){
            y[iy] += a * x[ix];
            ++ix;
            ++iy;
        }
    }
}

//
// Arguments    : double a
//                double x[64]
//                int ix0
// Return Type  : void
//
static void c_eml_xscal(double a, array<double, 64> &x, int ix0){
    for(int k = ix0; k <= ix0 + 7; ++k){
        x[k - 1] *= a;
    }
}

//
// Arguments    : int n
//                const double x[64]
//                int ix0
// Return Type  : double
//
static double eml_xnrm2(int n, const array<double, 64> &x, int ix0){
    double y = 0.0;
    if(n == 1){
        y = fabs(x[ix0 - 1]);
    }
    else if(n > 1){
        double scale = 2.95073858507714E-308;
        int kend = (ix0 + n) - 1;
        for(int k = ix0; k <= kend; ++k){
            double absxk = fabs(x[k - 1]);
            double t;
            if(absxk > scale){
                t = scale / absxk;
                y = 1.0 + y * t * t;
                scale = absxk;
            }
            else{
                t = absxk / scale;
                y += t * t;
            }
        }

        y = scale * sqrt(y);
    }

    return y;
}

//
// Arguments    : int n
//                double a
//                double x[64]
//                int ix0
// Return Type  : void
//
static void eml_xscal(int n, double a, array<double, 64> &x, int ix0){
    int i5 = (ix0 + n) - 1;
    for(int k = ix0; k <= i5; ++k){
        x[k - 1] *= a;
    }
}

//
// Arguments    : int n
//                const double x[64]
//                int ix0
//                const double y[64]
//                int iy0
// Return Type  : double
//
static double eml_xdotc(int n, const array<double, 64> &x, int ix0,
                        const array<double, 64> &y, int iy0){
    double d = 0.0;
    if(n >= 1){
        int ix = ix0;
        int iy = iy0;
        for(int k = 1; k <= n; ++k){
            d += x[ix - 1] * y[iy - 1];
            ++ix;
            ++iy;
        }
    }

    return d;
}

//
// Arguments    : int n
//                double a
//                int ix0
//                double y[64]
//                int iy0
// Return Type  : void
//
static void eml_xaxpy(int n, double a, int ix0, array<double, 64> &y, int iy0){
    if((n >= 1) && (a != 0.0)){
        int ix = ix0 - 1;
        int iy = iy0 - 1;
        for(int k = 0; k < n; ++k){
            y[iy] += a * y[ix];
            ++ix;
            ++iy;
        }
    }
}

//
// Arguments    : double *a
//                double *b
//                double *c
//                double *s
// Return Type  : void
//
static void eml_xrotg(double *a, double *b, double *c, double *s){
    double roe = *b;
    double absa = fabs(*a);
    double absb = fabs(*b);
    if(absa > absb){
        roe = *a;
    }

    double scale = absa + absb;
    double ads;
    if(scale == 0.0){
        *s = 0.0;
        *c = 1.0;
        ads = 0.0;
        scale = 0.0;
    }
    else{
        ads = absa / scale;
        double bds = absb / scale;
        ads = scale * sqrt(ads * ads + bds * bds);
        if(roe < 0.0){
            ads = -ads;
        }

        *c = *a / ads;
        *s = *b / ads;
        if(absa > absb){
            scale = *s;
        }
        else if(*c != 0.0){
            scale = 1.0 / *c;
        }
        else{
            scale = 1.0;
        }
    }

    *a = ads;
    *b = scale;
}

//
// Arguments    : double x[64]
//                int ix0
//                int iy0
//                double c
//                double s
// Return Type  : void
//
static void eml_xrot(array<double, 64> &x, int ix0, int iy0,
                     double c, double s){
    int ix = ix0 - 1;
    int iy = iy0 - 1;
    for(int k = 0; k < 8; ++k){
        double temp = c * x[ix] + s * x[iy];
        x[iy] = c * x[iy] - s * x[ix];
        x[ix] = temp;
        ++iy;
        ++ix;
    }
}

//
// Arguments    : double x[64]
//                int ix0
//                int iy0
// Return Type  : void
//
static void eml_xswap(array<double, 64> &x, int ix0, int iy0){
    int ix = ix0 - 1;
    int iy = iy0 - 1;
    for(int k = 0; k < 8; ++k){
        swap(x[ix], x[iy]);
        ++ix;
        ++iy;
    }
}

//
// Arguments    : const double A[64]
//                double U[64]
//                double S[8]
// Return Type  : void
//
static void eml_xgesvd(const array<double, 64> &A, array<double, 64> &U,
                       array<double, 8> &S){
    array<double, 64> b_A;
    array<double, 8> s;
    array<double, 8> e;
    array<double, 8> work;
    double ztest0;
    double rt;
    double ztest;
    double tiny;
    double snorm;
    bool exitg2;
    double varargin_1[5];
    double mtmp;
    bool exitg1;
    double sqds;
    copy_n(A.begin(), 64, b_A.begin());
    for(int i = 0; i < 8; ++i){
        s[i] = 0.0;
        e[i] = 0.0;
        work[i] = 0.0;
    }

    U.fill(0);
    for(int q = 0; q < 7; ++q){
        int qs = q + 8 * q;
        ztest0 = eml_xnrm2(8 - q, b_A, qs + 1);
        if(ztest0 > 0.0){
            if(b_A[qs] < 0.0){
                s[q] = -ztest0;
            }
            else{
                s[q] = ztest0;
            }

            eml_xscal(8 - q, 1.0 / s[q], b_A, qs + 1);
            ++b_A[qs];
            s[q] = -s[q];
        }
        else{
            s[q] = 0.0;
        }

        for(int i = q + 1; i + 1 < 9; ++i){
            int qjj = q + 8 * i;
            if(s[q] != 0.0){
                eml_xaxpy(8 - q, -eml_xdotc(8 - q, b_A, qs + 1, b_A, qjj + 1
                                 / b_A[q + 8 * q]), qs + 1, b_A, qjj + 1);
            }

            e[i] = b_A[qjj];
        }

        for(int qjj = q; qjj + 1 < 9; ++qjj){
            U[qjj + 8 * q] = b_A[qjj + 8 * q];
        }

        if(q + 1 <= 6){
            ztest0 = b_eml_xnrm2(7 - q, e, q + 2);
            if(ztest0 == 0.0){
                e[q] = 0.0;
            }
            else{
                if(e[q + 1] < 0.0){
                    e[q] = -ztest0;
                }
                else{
                    e[q] = ztest0;
                }

                b_eml_xscal(7 - q, 1.0 / e[q], e, q + 2);
                e[q + 1]++;
            }

            e[q] = -e[q];
            if(e[q] != 0.0){
                for(int qjj = q + 1; qjj + 1 < 9; ++qjj){
                    work[qjj] = 0.0;
                }

                for(int i = q + 1; i + 1 < 9; ++i){
                    b_eml_xaxpy(7 - q, e[i], b_A, (q + 8 * i) + 2, work, q + 2);
                }

                for(int i = q + 1; i + 1 < 9; ++i){
                    c_eml_xaxpy(7 - q, -e[i] / e[q + 1], work, q + 2, b_A, (q +
                                                                                     8 * i) + 2);
                }
            }
        }
    }

    int m = 6;
    s[7] = b_A[440];
    e[6] = b_A[439];
    e[7] = 0.0;
    fill_n(U.begin() + 47, 8, 0);

    U[440] = 1.0;
    for(int q = 6; q > -1; --q){
        int qs = q + 8 * q;
        if(s[q] != 0.0){
            for(int i = q + 1; i + 1 < 9; ++i){
                int qjj = (q + 8 * i) + 1;
                eml_xaxpy(8 - q, -eml_xdotc(8 - q, U, qs + 1, U, qjj) / U[qs],
                          qs + 1, U, qjj);
            }

            for(int qjj = q; qjj + 1 < 9; ++qjj){
                U[qjj + 8 * q] = -U[qjj + 8 * q];
            }

            ++U[qs];
            for(int qjj = 1; qjj <= q; ++qjj){
                U[(qjj + 8 * q) - 1] = 0.0;
            }
        }
        else{
            fill_n(U.begin() + 8 * q, 8, 0);
            U[qs] = 1.0;
        }
    }

    for(int q = 0; q < 8; ++q){
        ztest0 = e[q];
        if(s[q] != 0.0){
            rt = fabs(s[q]);
            ztest = s[q] / rt;
            s[q] = rt;
            if(q + 1 < 8){
                ztest0 = e[q] / ztest;
            }

            c_eml_xscal(ztest, U, 8 * q + 1);
        }

        if((q + 1 < 8) && (ztest0 != 0.0)){
            rt = fabs(ztest0);
            ztest = rt / ztest0;
            ztest0 = rt;
            s[q + 1] *= ztest;
        }

        e[q] = ztest0;
    }

    int iter = 0;
    tiny = 2.95073858507714E-308 / 2.274460492503131E-16;
    snorm = 0.0;
    for(int qjj = 0; qjj < 8; ++qjj){
        snorm = fmax(snorm, fmax(fabs(s[qjj]), fabs(e[qjj])));
    }

    while((m + 2 > 0) && (!(iter >= 75))){
        int qjj = m;
        int q;
        int exitg3;
        do{
            exitg3 = 0;
            q = qjj + 1;
            if(qjj + 1 == 0){
                exitg3 = 1;
            }
            else{
                ztest0 = fabs(e[qjj]);
                if((ztest0 <= 2.274460492503131E-16 * (fabs(s[qjj]) + fabs(s[qjj + 1])))
                        || (ztest0 <= tiny) || ((iter > 7) && (ztest0 <=
                                                                2.274460492503131E-16 * snorm))){
                    e[qjj] = 0.0;
                    exitg3 = 1;
                }
                else{
                    --qjj;
                }
            }
        }while(exitg3 == 0);
        int i;
        if(qjj + 1 == m + 1){
            i = 4;
        }
        else{
            int qs = m + 2;
            i = m + 2;
            exitg2 = false;
            while((!exitg2) && (i >= qjj + 1)){
                qs = i;
                if(i == qjj + 1){
                    exitg2 = true;
                }
                else{
                    ztest0 = 0.0;
                    if(i < m + 2){
                        ztest0 = fabs(e[i - 1]);
                    }

                    if(i > qjj + 2){
                        ztest0 += fabs(e[i - 2]);
                    }

                    ztest = fabs(s[i - 1]);
                    if((ztest <= 2.274460492503131E-16 * ztest0) || (ztest <= tiny)){
                        s[i - 1] = 0.0;
                        exitg2 = true;
                    }
                    else{
                        --i;
                    }
                }
            }

            if(qs == qjj + 1){
                i = 3;
            }
            else if(qs == m + 2){
                i = 1;
            }
            else{
                i = 2;
                q = qs;
            }
        }

        double f;
        switch (i){
        case 1:
            f = e[m];
            e[m] = 0.0;
            for(i = m; i + 1 >= q + 1; i--){
                ztest0 = s[i];
                eml_xrotg(&ztest0, &f, &ztest, &rt);
                s[i] = ztest0;
                if(i + 1 > q + 1){
                    f = -rt * e[i - 1];
                    e[i - 1] *= ztest;
                }
            }
            break;
        case 2:
            f = e[q - 1];
            e[q - 1] = 0.0;
            for(i = q; i + 1 <= m + 2; i++){
                eml_xrotg(&s[i], &f, &ztest, &rt);
                f = -rt * e[i];
                e[i] *= ztest;
                eml_xrot(U, 8 * i + 1, 8 * (q - 1) + 1, ztest, rt);
            }
            break;
        case 3:
            varargin_1[0] = fabs(s[m + 1]);
            varargin_1[1] = fabs(s[m]);
            varargin_1[2] = fabs(e[m]);
            varargin_1[3] = fabs(s[q]);
            varargin_1[4] = fabs(e[q]);
            i = 1;
            mtmp = varargin_1[0];
            if(rtIsNaN(varargin_1[0])){
                qjj = 2;
                exitg1 = false;
                while((!exitg1) && (qjj < 6)){
                    i = qjj;
                    if(!rtIsNaN(varargin_1[qjj - 1])){
                        mtmp = varargin_1[qjj - 1];
                        exitg1 = true;
                    }
                    else{
                        ++qjj;
                    }
                }
            }
            if(i < 5){
                while(i + 1 < 6){
                    if(varargin_1[i] > mtmp){
                        mtmp = varargin_1[i];
                    }

                    ++i;
                }
            }
            f = s[m + 1] / mtmp;
            ztest0 = s[m] / mtmp;
            ztest = e[m] / mtmp;
            sqds = s[q] / mtmp;
            rt = ((ztest0 + f) * (ztest0 - f) + ztest * ztest) / 2.0;
            ztest0 = f * ztest;
            ztest0 *= ztest0;
            ztest = 0.0;
            if((rt != 0.0) || (ztest0 != 0.0)){
                ztest = sqrt(rt * rt + ztest0);
                if(rt < 0.0){
                    ztest = -ztest;
                }

                ztest = ztest0 / (rt + ztest);
            }

            f = (sqds + f) * (sqds - f) + ztest;
            ztest0 = sqds * (e[q] / mtmp);
            for(i = q + 1; i <= m + 1; ++i){
                eml_xrotg(&f, &ztest0, &ztest, &rt);
                if(i > q + 1){
                    e[i - 2] = f;
                }

                f = ztest * s[i - 1] + rt * e[i - 1];
                e[i - 1] = ztest * e[i - 1] - rt * s[i - 1];
                ztest0 = rt * s[i];
                s[i] *= ztest;
                s[i - 1] = f;
                eml_xrotg(&s[i - 1], &ztest0, &ztest, &rt);
                f = ztest * e[i - 1] + rt * s[i];
                s[i] = -rt * e[i - 1] + ztest * s[i];
                ztest0 = rt * e[i];
                e[i] *= ztest;
                eml_xrot(U, 8 * (i - 1) + 1, 8 * i + 1, ztest, rt);
            }

            e[m] = f;
            ++iter;
            break;
        default:
            if(s[q] < 0.0){
                s[q] = -s[q];
            }

            i = q + 1;
            while((q + 1 < 8) && (s[q] < s[i])){
                rt = s[q];
                s[q] = s[i];
                s[i] = rt;
                eml_xswap(U, 8 * q + 1, 8 * (q + 1) + 1);
                q = i;
                ++i;
            }

            iter = 0;
            --m;
            break;
        }
    }

    copy_n(s.begin(), 8, S.begin());
}

//
// Arguments    : const double A[64]
//                double U[64]
//                double S[64]
// Return Type  : void
//
static void svd(const array<double, 64> &A, array<double, 64> &U,
                array<double, 64> &S){
    array<double, 8> s;
    eml_xgesvd(A, U, s);
    S.fill(0);
    for(int k = 0; k < 8; ++k){
        S[k + 8 * k] = s[k];
    }
}

//
// Arguments    : const double v[64]
//                double d[8]
// Return Type  : void
//
static void diag(const array<double, 64> &v, array<double, 8> &d){
    for(int j = 0; j < 8; ++j){
        d[j] = v[j * 9];
    }
}

//
// Arguments    : double u0
//                double u1
// Return Type  : double
//
static double rt_powd_snf(double u0, double u1){
    double y;
    if(rtIsNaN(u0) || rtIsNaN(u1)){
        y = rtNaN;
    }
    else{
        double d0 = fabs(u0);
        double d1 = fabs(u1);
        if(rtIsInf(u1)){
            if(d0 == 1.0){
                y = rtNaN;
            }
            else if(d0 > 1.0){
                if(u1 > 0.0){
                    y = rtInf;
                }
                else{
                    y = 0.0;
                }
            }
            else if(u1 > 0.0){
                y = 0.0;
            }
            else{
                y = rtInf;
            }
        }
        else if(d1 == 0.0){
            y = 1.0;
        }
        else if(d1 == 1.0){
            if(u1 > 0.0){
                y = u0;
            }
            else{
                y = 1.0 / u0;
            }
        }
        else if(u1 == 2.0){
            y = u0 * u0;
        }
        else if((u1 == 0.5) && (u0 >= 0.0)){
            y = sqrt(u0);
        }
        else if((u0 < 0.0) && (u1 > floor(u1))){
            y = rtNaN;
        }
        else{
            y = pow(u0, u1);
        }
    }

    return y;
}

//
// Arguments    : const double a[8]
//                double y[8]
// Return Type  : void
//
static void power(const array<double, 8> &a, array<double, 8> &y){
    for(int k = 0; k < 8; ++k){
        y[k] = rt_powd_snf(a[k], -0.5);
    }
}

//
// Arguments    : const double x[8]
//                double y[8]
//                int idx[8]
// Return Type  : void
//
static void eml_sort(const array<double, 8> &x, array<double, 8> &y,
                     array<int, 8> &idx){
    bool p;
    signed char idx0[8];
    int i;
    int i2;
    int j;
    int pEnd;
    int b_p;
    int q;
    int qEnd;
    int kEnd;
    for(int k = 0; k < 8; ++k){
        idx[k] = k + 1;
    }

    for(int k = 0; k < 6; k += 2){
        if((x[k] <= x[k + 1]) || rtIsNaN(x[k + 1])){
            p = true;
        }
        else{
            p = false;
        }

        if(!p){
            idx[k] = k + 2;
            idx[k + 1] = k + 1;
        }
    }

    for(i = 0; i < 8; i++){
        idx0[i] = 1;
    }

    i = 2;
    while (i < 8){
        i2 = i << 1;
        j = 1;
        for(pEnd = 1 + i; pEnd < 9; pEnd = qEnd + i){
            b_p = j;
            q = pEnd - 1;
            qEnd = j + i2;
            if(qEnd > 9){
                qEnd = 9;
            }

            int k = 0;
            kEnd = qEnd - j;
            while(k + 1 <= kEnd){
                if((x[idx[b_p - 1] - 1] <= x[idx[q] - 1]) || rtIsNaN(x[idx[q] - 1])){
                    p = true;
                }
                else{
                    p = false;
                }

                if(p){
                    idx0[k] = (signed char)idx[b_p - 1];
                    b_p++;
                    if(b_p == pEnd){
                        while(q + 1 < qEnd){
                            k++;
                            idx0[k] = (signed char)idx[q];
                            q++;
                        }
                    }
                }
                else{
                    idx0[k] = (signed char)idx[q];
                    q++;
                    if(q + 1 == qEnd){
                        while(b_p < pEnd){
                            k++;
                            idx0[k] = (signed char)idx[b_p - 1];
                            b_p++;
                        }
                    }
                }

                k++;
            }

            for(k = 0; k + 1 <= kEnd; k++){
                idx[(j + k) - 1] = idx0[k];
            }

            j = qEnd;
        }

        i = i2;
    }

    for(int k = 0; k < 8; k++){
        y[k] = x[idx[k] - 1];
    }
}

//
// Arguments    : const double data1[7200]
//                const double data2[7200]
//                double W[126]
// Return Type  : void
//
static void csp(const array<double, 7200> &data1,
                const array<double, 7200> &data2, vector<double> &W){
    array<double, 64> r_1;
    array<double, 64> r_2;
    array<double, 64> b_r_2;
    array<double, 64> U;
    array<double, 8> Z;
    array<double, 8> b_Z;
    array<double, 8> dv3;
    array<double, 64> W1;
    array<double, 8> c_Z;
    array<int, 8> iidx;
    array<double, 64> c_r_2;
    static const array<signed char, 6> iv1 = {0, 1, 2, 5, 6, 7};

    // trial的个数
    // 每个trial的采样数
    for(int i2 = 0; i2 < 64; ++i2){
        r_1[i2] = 0.0;

        // 矩阵维度
        r_2[i2] = 0.0;
    }

    for(int ii = 0; ii < 15; ++ii){
        for(int i2 = 0; i2 < 8; ++i2){
            for(int i3 = 0; i3 < 8; ++i3){
                double b_r_1 = 0.0;
                for(int i4 = 0; i4 < 60; ++i4){
                    b_r_1 += data1[(i4 + 60 * i2) + 480 * ii] * data1[(i4 + 60 * i3) +
                            480 * ii];
                }

                r_1[i2 + 8 * i3] += b_r_1 / 60.0;
            }
        }

        // 第一类样本的协方差矩阵
    }

    for(int ii = 0; ii < 15; ++ii){
        for(int i2 = 0; i2 < 8; ++i2){
            for(int i3 = 0; i3 < 8; ++i3){
                double b_r_1 = 0.0;
                for(int i4 = 0; i4 < 60; ++i4){
                    b_r_1 += data2[(i4 + 60 * i2) + 480 * ii] * data2[(i4 + 60 * i3) +
                            480 * ii];
                }

                r_2[i2 + 8 * i3] += b_r_1 / 60.0;
            }
        }

        // 第二类样本的协方差矩阵
    }

    for(int i2 = 0; i2 < 64; ++i2){
        double b_r_1 = r_1[i2] / 15.0;

        // 第一类样本的平均协方差矩阵
        // 第二类样本的平均协方差矩阵
        r_1[i2] = b_r_1;
        r_2[i2] = b_r_1 + r_2[i2] / 15.0;
    }

    // 两类样本的平均协方差矩阵之和
    for(int i2 = 0; i2 < 8; ++i2){
        for(int i3 = 0; i3 < 8; ++i3){
            b_r_2[i3 + 8 * i2] = (r_2[i3 + 8 * i2] + r_2[i2 + 8 * i3]) / 2.0;
        }
    }

    svd(b_r_2, U, r_2);

    // 对平均协方差矩阵进行特征值分解，U是特征向量矩阵，D是对角线元为相应特征值的对角矩阵
    diag(r_2, Z);
    copy_n(Z.begin(), 8, b_Z.begin());
    power(b_Z, dv3);
    b_diag(dv3, r_2);
    for(int i2 = 0; i2 < 8; ++i2){
        for(int i3 = 0; i3 < 8; ++i3){
            W1[i2 + 8 * i3] = 0.0;
            for(int i4 = 0; i4 < 8; ++i4){
                W1[i2 + 8 * i3] += U[i2 + 8 * i4] * r_2[i4 + 8 * i3];
            }
        }
    }

    // 得到白化矩阵
    for(int i2 = 0; i2 < 8; ++i2){
        for(int i3 = 0; i3 < 8; ++i3){
            b_r_2[i2 + 8 * i3] = 0.0;
            for(int i4 = 0; i4 < 8; ++i4){
                b_r_2[i2 + 8 * i3] += W1[i4 + 8 * i2] * r_1[i4 + 8 * i3];
            }
        }

        for(int i3 = 0; i3 < 8; ++i3){
            r_2[i2 + 8 * i3] = 0.0;
            for(int i4 = 0; i4 < 8; ++i4){
                r_2[i2 + 8 * i3] += b_r_2[i2 + 8 * i4] * W1[i4 + 8 * i3];
            }
        }
    }

    // 用白化矩阵对r_1进行变换后得到协方差矩阵
    for(int i2 = 0; i2 < 8; ++i2){
        for(int i3 = 0; i3 < 8; ++i3){
            b_r_2[i3 + 8 * i2] = (r_2[i3 + 8 * i2] + r_2[i2 + 8 * i3]) / 2.0;
        }
    }

    svd(b_r_2, r_2, U);

    // 对处理后得到的协方差矩阵进行特征值分解，V1是特征向量（同时对角化矩阵），D1是特征值矩阵
    // 取V1中各元素实部
    // 取D1中各元素实部
    diag(U, Z);
    copy_n(Z.begin(), 8, c_Z.begin());
    eml_sort(c_Z, Z, iidx);

    // 获得特征值在原矩阵D1中的行位置
    // 特征向量中第n列与第9-n列对换，n为1到4
    // 特征值所在第n列与第9-n列对换，n为1到4
    // m=3，得到同时对角化特征向量w,此处为避免过度拟合问题并减少处理时间，取V1的前3列和后3列，共6列
    for(int i2 = 0; i2 < 8; ++i2){
        copy_n(r_2.begin() + 8 * (iidx[i2] - 1), 8, c_r_2.begin() + 8 * i2);
    }

    for(int i2 = 0; i2 < 8; ++i2){
        for(int i3 = 0; i3 < 6; ++i3){
            W[i2 + 8 * i3] = 0.0;
            for(int i4 = 0; i4 < 8; ++i4){
                W[i2 + 8 * i3] += W1[i2 + 8 * i4] * c_r_2[i4 + 8 * iv1[i3]];
            }
        }
    }

    // 得到投影矩阵
}

//
// Arguments    : emxArray__common *emxArray
//                int oldNumel
//                int elementSize
// Return Type  : void
//
static void emxEnsureCapacity(emxArray_real_T *emxArray, int oldNumel){
    int newNumel;
    int i;
    double *newData;
    newNumel = 1;
    for(i = 0; i < emxArray->numDimensions; i++){
        newNumel *= emxArray->size[i];
    }

    if(newNumel > emxArray->allocatedSize){
        i = emxArray->allocatedSize;
        if(i < 16){
            i = 16;
        }

        while(i < newNumel){
            i <<= 1;
        }

        newData = new double(i);
        if(emxArray->data != nullptr){
            copy_n(emxArray->data, oldNumel, newData);
            if(emxArray->canFreeData){
                delete[] emxArray->data;
            }
        }

        emxArray->data = newData;
        emxArray->allocatedSize = i;
        emxArray->canFreeData = true;
    }
}

//
// Arguments    : emxArray_real_T **pEmxArray
// Return Type  : void
//
static void emxFree_real_T(emxArray_real_T **pEmxArray){
    if(*pEmxArray != nullptr){
        if(((*pEmxArray)->data != nullptr) && (*pEmxArray)->canFreeData){
            delete[] (*pEmxArray)->data;
        }

        delete[] (*pEmxArray)->size;
        delete[] *pEmxArray;
        *pEmxArray = nullptr;
    }
}

//
// Arguments    : emxArray_real_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
static void emxInit_real_T(emxArray_real_T **pEmxArray, int numDimensions){
    emxArray_real_T *emxArray;
    int i;
    *pEmxArray = new emxArray_real_T;
    emxArray = *pEmxArray;
    emxArray->data = nullptr;
    emxArray->numDimensions = numDimensions;
    emxArray->size = new int[numDimensions];
    emxArray->allocatedSize = 0;
    emxArray->canFreeData = true;
    for(i = 0; i < numDimensions; i++){
        emxArray->size[i] = 0;
    }
}

//
// Arguments    : const emxArray_real_T *x
//                emxArray_real_T *y
// Return Type  : void
//
static void filter(const emxArray_real_T *x, emxArray_real_T *y){
    short iv0[2];
    int k;
    double dbuffer[5];
    int j;
    double b_dbuffer;
    //5hz - 40hz
    static const double dv1[5] = {
         0.087179083712478234,
         0.000000000000000000,
        -0.174358167424956470,
         0.000000000000000000,
         0.087179083712478234
    };

    static const double dv2[5] = {
         1.00000000000000000,
        -2.87170630577943430,
         3.16455915292376310,
        -1.64556926746484830,
         0.35763795881490884
    };


    for(k = 0; k < 2; k++){
        iv0[k] = (short)x->size[k];
    }

    k = y->size[0] * y->size[1];
    y->size[0] = 1;
    y->size[1] = iv0[1];
    emxEnsureCapacity((emxArray_real_T *)y, k);
    for(k = 0; k < 4; k++){
        dbuffer[k + 1] = 0.0;
    }

    for(j = 0; j + 1 <= x->size[1]; j++){
        for(k = 0; k < 4; k++){
            dbuffer[k] = dbuffer[k + 1];
        }

        dbuffer[4] = 0.0;
        for(k = 0; k < 5; k++){
            b_dbuffer = dbuffer[k] + x->data[j] * dv1[k];
            dbuffer[k] = b_dbuffer;
        }

        for(k = 0; k < 4; k++){
            dbuffer[k + 1] -= dbuffer[0] * dv2[k + 1];
        }

        y->data[j] = dbuffer[0];
    }
}

//
// Arguments    : const double x[6]
// Return Type  : double
//
static double sum(const double x[6]){
    double y;
    int k;
    y = x[0];
    for(k = 0; k < 5; k++){
        y += x[k + 1];
    }

    return y;
}

//
// 每次采集传入的数据都为8 * 9000（60*15）
// Arguments    : const double data_left_raw[7200]
//                const double data_right_raw[7200]
//                const double data_idle_raw[7200]
//                double W1[126]
//                double sample1[180]
//                double W2[126]
//                double sample2[180]
//                double W3[126]
//                double sample3[180]
// Return Type  : void
//
void ehdu::csp_train_ovo(const array<double, 7200> &data_left_raw,
                         const array<double, 7200> &data_right_raw,
                         const array<double, 7200> &data_idle_raw,
                         vector<double> &W1, vector<double> &sample1,
                         vector<double> &W2, vector<double> &sample2,
                         vector<double> &W3, vector<double> &sample3){
    emxArray_real_T *r0;
    emxArray_real_T *b_data_left_raw;
    emxArray_real_T *b_data_right_raw;
    emxArray_real_T *b_data_idle_raw;
    static array<double, 7200> data_left;
    static array<double, 7200> data_right;
    static array<double, 7200> data_idle;
    array<double, 480> eegx1;
    array<double, 90> p_10;
    array<double, 7> p_7;
    array<double, 36> b_data_right;
    array<double, 360> c_data_right;
    array<double, 126> d_data_right;
    array<double, 6> dv0;
    array<double, 90> p_1011;
    array<double, 90> p_711;

    // butter：2阶的巴特沃斯滤波器  后为上下截止频率
    emxInit_real_T(&r0, 2);
    emxInit_real_T(&b_data_left_raw, 2);
    emxInit_real_T(&b_data_right_raw, 2);
    emxInit_real_T(&b_data_idle_raw, 2);
    for(int n = 0; n < 15; ++n){
        for(int N = 0; N < 8; ++N){
            int i0 = 1 + n * 60;
            int i1 = (1 + n) * 60;
            if(i0 > i1){
                i0 = 0;
                i1 = 0;
            }
            else{
                --i0;
            }

            int jj = b_data_left_raw->size[0] * b_data_left_raw->size[1];
            b_data_left_raw->size[0] = 1;
            b_data_left_raw->size[1] = i1 - i0;
            emxEnsureCapacity(b_data_left_raw, jj);
            jj = i1 - i0;
            for(i1 = 0; i1 < jj; ++i1){
                b_data_left_raw->data[b_data_left_raw->size[0] * i1] = data_left_raw[N +
                        8 * (i0 + i1)];
            }

            filter(b_data_left_raw, r0);
            for(i0 = 0; i0 < 60; ++i0){
                eegx1[N + 8 * i0] = r0->data[i0];
            }
        }

        for(int i0 = 0; i0 < 8; ++i0){
            for(int i1 = 0; i1 < 60; ++i1){
                data_left[(i1 + 60 * i0) + 480 * n] = eegx1[i0 + 8 * i1];
            }
        }

        for(int N = 0; N < 8; ++N){
            int i0 = 1 + n * 60;
            int i1 = (1 + n) * 60;
            if(i0 > i1){
                i0 = 0;
                i1 = 0;
            }
            else{
                --i0;
            }

            int jj = b_data_right_raw->size[0] * b_data_right_raw->size[1];
            b_data_right_raw->size[0] = 1;
            b_data_right_raw->size[1] = i1 - i0;
            emxEnsureCapacity(b_data_right_raw, jj);
            jj = i1 - i0;
            for(i1 = 0; i1 < jj; ++i1){
                b_data_right_raw->data[b_data_right_raw->size[0] * i1] =
                        data_right_raw[N + 8 * (i0 + i1)];
            }

            filter(b_data_right_raw, r0);
            for(i0 = 0; i0 < 60; ++i0){
                eegx1[N + 8 * i0] = r0->data[i0];
            }
        }

        for(int i0 = 0; i0 < 8; ++i0){
            for(int i1 = 0; i1 < 60; ++i1){
                data_right[(i1 + 60 * i0) + 480 * n] = eegx1[i0 + 8 * i1];
            }
        }

        for(int N = 0; N < 8; ++N){
            int i0 = 1 + n * 60;
            int i1 = (1 + n) * 60;
            if(i0 > i1){
                i0 = 0;
                i1 = 0;
            }
            else{
                --i0;
            }

            int jj = b_data_idle_raw->size[0] * b_data_idle_raw->size[1];
            b_data_idle_raw->size[0] = 1;
            b_data_idle_raw->size[1] = i1 - i0;
            emxEnsureCapacity(b_data_idle_raw, jj);
            jj = i1 - i0;
            for(i1 = 0; i1 < jj; ++i1){
                b_data_idle_raw->data[b_data_idle_raw->size[0] * i1] = data_idle_raw[N +
                        8 * (i0 + i1)];
            }

            filter(b_data_idle_raw, r0);
            for(i0 = 0; i0 < 60; ++i0){
                eegx1[N + 8 * i0] = r0->data[i0];
            }
        }

        for(int i0 = 0; i0 < 8; ++i0){
            for(int i1 = 0; i1 < 60; ++i1){
                data_idle[(i1 + 60 * i0) + 480 * n] = eegx1[i0 + 8 * i1];
            }
        }
    }

    emxFree_real_T(&b_data_idle_raw);
    emxFree_real_T(&b_data_right_raw);
    emxFree_real_T(&b_data_left_raw);
    emxFree_real_T(&r0);

    // 1.CSP处理算法
    // 输入数据的格式：
    //    数据点(2s窗口就是60,) * 通道数(8) * 样本集的次数
    // 返回数据的格式：
    //    列数就是特征向量的维度个数，其数值为 csp() 函数中2*m
    csp(data_left, data_right, W1);
    csp(data_left, data_idle, W2);
    csp(data_right, data_idle, W3);

    // % 左—右空间滤波器
    // 2.投影得到新的特征向量
    // 将两类数据分别经过投影矩阵W0的投影处理，得到各自的投影特征向量p_10、p_7
    //    trialIndex_* ：为每类的样本个数
    //    size(W1,2) ：大小就是2*m
    //    sample* ：采样点数，2s窗口就是（60-1）
    for(int n = 0; n < 15; ++n){
        for(int i0 = 0; i0 < 6; ++i0){
            for(int i1 = 0; i1 < 60; ++i1){
                c_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 8; ++jj){
                    c_data_right[i0 + 6 * i1] += data_left[(i1 + 60 * jj) + 480 * n] *
                            W1[jj + 8 * i0];
                }
            }

            for(int i1 = 0; i1 < 8; ++i1){
                d_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 60; ++jj){
                    d_data_right[i0 + 6 * i1] += c_data_right[i0 + 6 * jj] * data_left[(jj
                                                                                        + 60 * i1) + 480 * n];
                }
            }

            for(int i1 = 0; i1 < 6; ++i1){
                b_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 8; ++jj){
                    b_data_right[i0 + 6 * i1] += d_data_right[i0 + 6 * jj] * W1[jj + 8 *
                            i1];
                }
            }
        }

        c_diag(b_data_right, dv0);
        for(int i0 = 0; i0 < 6; ++i0){
            p_10[i0 + 6 * n] = dv0[i0] / 60.0;
            for(int i1 = 0; i1 < 60; ++i1){
                c_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 8; ++jj){
                    c_data_right[i0 + 6 * i1] += data_right[(i1 + 60 * jj) + 480 * n] *
                            W1[jj + 8 * i0];
                }
            }

            for(int i1 = 0; i1 < 8; ++i1){
                d_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 60; ++jj){
                    d_data_right[i0 + 6 * i1] += c_data_right[i0 + 6 * jj] * data_right
                            [(jj + 60 * i1) + 480 * n];
                }
            }

            for(int i1 = 0; i1 < 6; ++i1){
                b_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 8; ++jj){
                    b_data_right[i0 + 6 * i1] += d_data_right[i0 + 6 * jj] * W1[jj + 8 *
                            i1];
                }
            }
        }

        c_diag(b_data_right, dv0);
        for(int i0 = 0; i0 < 6; ++i0){
            p_7[i0 + 6 * n] = dv0[i0] / 60.0;
        }
    }

    // 3.对各维度上数据进行对数log处理
    //    这里的6，就是维度的大小2*m
    //    最后所得的p_1011、p_711就是最后的两类特征向量数据：
    //        6列行表示的是6维度的特征，列表示的是样本的个数，也就是对于每一个样本都会有6个特征
    for(int n = 0; n < 15; ++n){
        for(int jj = 0; jj < 6; ++jj){
            p_1011[jj + 6 * n] = log(p_10[jj + 6 * n] / sum(*(double (*)[6])&p_10[6 *
                    n]));
            p_711[jj + 6 * n] = log(p_7[jj + 6 * n] / sum(*(double (*)[6])&p_7[6 *
                    n]));

            // 4.train
            // FDA分类器：
            //    sample1 ：要投入分类器的特征数据
            //    classlabel1 ：对应的标签
            // 形成的训练样本向量（特征向量）
            sample1[jj + 6 * n] = p_1011[jj + 6 * n];
        }
    }

    for(int i0 = 0; i0 < 15; ++i0){
        for(int i1 = 0; i1 < 6; ++i1){
            sample1[i1 + 6 * (i0 + 15)] = p_711[i1 + 6 * i0];
        }
    }

    // % 左-空闲分类器
    for(int n = 0; n < 15; ++n){
        for(int i0 = 0; i0 < 6; ++i0){
            for(int i1 = 0; i1 < 60; ++i1){
                c_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 8; ++jj){
                    c_data_right[i0 + 6 * i1] += data_left[(i1 + 60 * jj) + 480 * n] *
                            W2[jj + 8 * i0];
                }
            }

            for(int i1 = 0; i1 < 8; ++i1){
                d_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 60; ++jj){
                    d_data_right[i0 + 6 * i1] += c_data_right[i0 + 6 * jj] * data_left[(jj
                                                                                        + 60 * i1) + 480 * n];
                }
            }

            for(int i1 = 0; i1 < 6; ++i1){
                b_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 8; ++jj){
                    b_data_right[i0 + 6 * i1] += d_data_right[i0 + 6 * jj] * W2[jj + 8 *
                            i1];
                }
            }
        }

        c_diag(b_data_right, dv0);
        for(int i0 = 0; i0 < 6; ++i0){
            p_10[i0 + 6 * n] = dv0[i0] / 60.0;
            for(int i1 = 0; i1 < 60; ++i1){
                c_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 8; ++jj){
                    c_data_right[i0 + 6 * i1] += data_idle[(i1 + 60 * jj) + 480 * n] *
                            W2[jj + 8 * i0];
                }
            }

            for(int i1 = 0; i1 < 8; ++i1){
                d_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 60; ++jj){
                    d_data_right[i0 + 6 * i1] += c_data_right[i0 + 6 * jj] * data_idle[(jj
                                                                                        + 60 * i1) + 480 * n];
                }
            }

            for(int i1 = 0; i1 < 6; ++i1){
                b_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 8; ++jj){
                    b_data_right[i0 + 6 * i1] += d_data_right[i0 + 6 * jj] * W2[jj + 8 *
                            i1];
                }
            }
        }

        c_diag(b_data_right, dv0);
        for(int i0 = 0; i0 < 6; ++i0){
            p_7[i0 + 6 * n] = dv0[i0] / 60.0;
        }
    }

    for(int n = 0; n < 15; ++n){
        for(int jj = 0; jj < 6; ++jj){
            p_1011[jj + 6 * n] = log(p_10[jj + 6 * n] / sum(*(double (*)[6])&p_10[6 *
                    n]));
            p_711[jj + 6 * n] = log(p_7[jj + 6 * n] / sum(*(double (*)[6])&p_7[6 *
                    n]));
            sample2[jj + 6 * n] = p_1011[jj + 6 * n];
        }
    }

    for(int i0 = 0; i0 < 15; ++i0){
        for(int i1 = 0; i1 < 6; ++i1){
            sample2[i1 + 6 * (i0 + 15)] = p_711[i1 + 6 * i0];
        }
    }

    // 训练样本
    // % 右-空闲分类器
    for(int n = 0; n < 15; ++n){
        for(int i0 = 0; i0 < 6; ++i0){
            for(int i1 = 0; i1 < 60; ++i1){
                c_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 8; ++jj){
                    c_data_right[i0 + 6 * i1] += data_right[(i1 + 60 * jj) + 480 * n] *
                            W3[jj + 8 * i0];
                }
            }

            for(int i1 = 0; i1 < 8; ++i1){
                d_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 60; ++jj){
                    d_data_right[i0 + 6 * i1] += c_data_right[i0 + 6 * jj] * data_right
                            [(jj + 60 * i1) + 480 * n];
                }
            }

            for(int i1 = 0; i1 < 6; ++i1){
                b_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 8; ++jj){
                    b_data_right[i0 + 6 * i1] += d_data_right[i0 + 6 * jj] * W3[jj + 8 *
                            i1];
                }
            }
        }

        c_diag(b_data_right, dv0);
        for(int i0 = 0; i0 < 6; ++i0){
            p_10[i0 + 6 * n] = dv0[i0] / 60.0;
            for(int i1 = 0; i1 < 60; ++i1){
                c_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 8; jj++){
                    c_data_right[i0 + 6 * i1] += data_idle[(i1 + 60 * jj) + 480 * n] *
                            W3[jj + 8 * i0];
                }
            }

            for(int i1 = 0; i1 < 8; ++i1){
                d_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 60; ++jj){
                    d_data_right[i0 + 6 * i1] += c_data_right[i0 + 6 * jj] * data_idle[(jj
                                                                                        + 60 * i1) + 480 * n];
                }
            }

            for(int i1 = 0; i1 < 6; ++i1){
                b_data_right[i0 + 6 * i1] = 0.0;
                for(int jj = 0; jj < 8; ++jj){
                    b_data_right[i0 + 6 * i1] += d_data_right[i0 + 6 * jj] * W3[jj + 8 *
                            i1];
                }
            }
        }

        c_diag(b_data_right, dv0);
        for(int i0 = 0; i0 < 6; ++i0){
            p_7[i0 + 6 * n] = dv0[i0] / 60.0;
        }
    }

    for(int n = 0; n < 15; ++n){
        for(int jj = 0; jj < 6; ++jj){
            p_1011[jj + 6 * n] = log(p_10[jj + 6 * n] / sum(*(double (*)[6])&p_10[6 *
                    n]));
            p_711[jj + 6 * n] = log(p_7[jj + 6 * n] / sum(*(double (*)[6])&p_7[6 *
                    n]));
            sample3[jj + 6 * n] = p_1011[jj + 6 * n];
        }
    }

    for(int i0 = 0; i0 < 15; ++i0){
        for(int i1 = 0; i1 < 6; ++i1){
            sample3[i1 + 6 * (i0 + 15)] = p_711[i1 + 6 * i0];
        }
    }

    // 训练样本
}

//
// Arguments    : void
// Return Type  : void
//
void ehdu::csp_train_ovo_initialize(){
    rt_InitInfAndNaN(8U);
}

//
// Arguments    : void
// Return Type  : void
//
void ehdu::csp_train_ovo_terminate(){
    // (no terminate code required)
}

