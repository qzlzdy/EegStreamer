#include "kcca.h"
#include "CCA/rt_nonfinite.h"

#include <armadillo>
using namespace arma;

#define MIN(a,b) a<b?a:b



// Function Declarations
//为21*600的二维矩阵，其顺序为先列后行，就是
extern void me_KCCA(const double data[12600], double, double,
                   double *v, double *idx)
{
//    double dv0[4800];
    double r1[34];
//    creal_T unusedU1[168];
//    creal_T unusedU0[441];
    //double dv1[4800];
    double r2[34];
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
//    refsig(left_fre, 300.0, 600.0, 4.0, dv0);
//    cca(data, dv0, unusedU0, unusedU1, r1);
//    refsig(right_fre, 300.0, 600.0, 4.0, dv1);
//    cca(data, dv1, unusedU0, unusedU1, r2);

    /*KCCA的参数*/
    mat  ref1, ref2;
    colvec b1, b2, wp1, wp2, wq1, wq2 ;
    QString kerneltype = "gauss" ;
    double kernelpar = 85 ;
    double reg = 1 ;
    unsigned long long Mmax_c = 17 ;
    uword N = 4 ;

    mat zp(data,21,600);

    ref1 = refsig(7.75, 300, 600, N) ;
    ref2 = refsig(9.75, 300, 600, N) ;

    kcca(zp.t(), ref1.t(), kerneltype, kernelpar, reg, 1, "ICD", Mmax_c, wq1, wp1, b1) ;
    kcca(zp.t(), ref2.t(), kerneltype, kernelpar, reg, 1, "ICD", Mmax_c, wq2, wp2, b2) ;

    for (int i = 0; i < 34; i++)
    {
        r1[i] = b1(33-i);
        r2[i] = b2(33-i);
    }


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

mat kernel(mat X1, mat X2, QString ktype, double kpar)
{
    mat K, distmat, mat1, mat2, norms1, norms2;
    double sgm;
    if ("gauss" == ktype)
    {
        sgm = kpar ;
        unsigned long long dim1 = size(X1, 0) ;
        unsigned long long dim2 = size(X2, 0) ;
        //        X1.print("X1:");
        norms1 = sum(square(X1), 1) ;
        norms2 = sum(square(X2), 1) ;
        //        norms1.print("norms1:");
        //        norms2.print("norms2:");
        mat1 = repmat(norms1, 1, dim2) ;
        mat2 = repmat(trans(norms2), dim1, 1) ;
        distmat = mat1+mat2-2*X1*trans(X2) ;
        K = exp(-distmat/(2*pow(sgm, 2))) ;
    }
    else if ("gauss-diag" == ktype)
    {
        sgm = kpar ;
        K = exp(-sum(square((X1-X2)), 1)/(2*pow(sgm, 2))) ;
    }
    else if ("poly" == ktype)
    {
        double p = kpar;
        double c = kpar;
        K = pow((X1*trans(X2)+c), p) ;
    }
    else if ("linear" == ktype)
    {
        K = X1*trans(X2) ;
    }
    else
    {
        std::cerr << "unknown kernel type" << std::endl ;
    }
    return K ;
}

void kernel_icd(mat X, QString ktype, double kpar, unsigned long long m,
                mat& G, Row<uword>& subset)
{
    mat x ;
    uvec ind;
    double m1, m2;
    uword j;
    double dtrace;
    unsigned long long n = size(X, 0) ;
    double precision = 1e-6;
    uvec perm = regspace<uvec>(0, n-1);
    rowvec d = zeros<rowvec>(n) ;
    G = zeros<mat>(n, m) ;
    subset.zeros(m) ;
    for (uword i=0; i < m; i++)
    {
        x = X.rows(perm.rows(i,n-1)) ;
        //        x.print("x:");
        if (i==0)
        {
            d.cols(i,n-1) = trans(kernel(x, x, ktype + "-diag", kpar)) ;
        }
        else
        {
            d.cols(i,n-1) = trans(kernel(x, x, ktype + "-diag", kpar))
                    - trans(sum(pow(G(span(i,n-1),span(0,i-1)),2),1));
        }

        dtrace = sum(d.cols(i,n-1));

        if (dtrace <= 0)
        {
            std::cerr << "Negative diagonal entry " << dtrace << std::endl ;
        }
        if (dtrace <= precision)
        {
            G.shed_cols(i,m-1);
            subset.shed_cols(i,m-1);
            break ;
        }
        //        G.print("G:");
        m2 = d.cols(i,n-1).max();
        j = d.cols(i,n-1).index_max();
        //        d.print("d:");
        //      d.cols(i,n-1).print("d(i:n-1):");
        j = j + i;
        m1 = sqrt(m2) ;
        subset(i) = j ;
        uvec tmp = {i,j};

//        cout << "size of G: " << size(G) << endl;

        perm.rows(uvec({i,j})) = perm.rows(uvec({j,i})); //permute elements i and j
        if (i)
        {
            G(uvec({j,i}),regspace<uvec>(0, i-1)) = G(uvec({i,j}),regspace<uvec>(0, i-1));
        }
        G(i, i) = m1 ;
        //        G.print("G:");
//        cout << "size of G: " << size(G) << endl;
        if (i == 0)
        {
            G(span(i+1,n-1),span(i,i)) =
                    kernel(X.row(perm(i)), X.rows(perm.rows(i+1,n-1)), "gauss", kpar).t()/m1;
        }
        else
        {
            G(span(i+1,n-1),span(i,i)) =
                    (kernel(X.row(perm(i)), X.rows(perm.rows(i+1,n-1)), "gauss", kpar).t()
                     - G(span(i+1,n-1),span(0,i-1)) * G(span(i,i),span(0,i-1)).t())/m1;
        }
        //        G.print("G:");
//        cout << "size of G: " << size(G) << endl;

    }
    ind = sort_index(perm, "ascend") ;
    G = G.rows(ind) ;
}

void kcca(mat X1, mat X2, QString kernel, double kernelpar, double reg, uword numeig,
          QString decomp, unsigned long long lrank, colvec& y1, colvec& y2, colvec& beta)
{
    mat D, G1, G2, I, I11, I22, K1, K2, N0, R, Z, Z11, Z12, Z22, alpha,
            alpha1, alpha2, alpha_norms, alphas;

    vec betas, betass;
    uvec ind;
    uword minrank;
    uword N = size(X1, 0) ;
    uword N1,N2;
    Row<uword> subset;

    cx_vec eigval;
    cx_mat eigvec;

    if (decomp == "ICD")
    {
        kernel_icd(X1, kernel, kernelpar, lrank, G1, subset) ;
        kernel_icd(X2, kernel, kernelpar, lrank, G2, subset) ;

        G1 = G1-repmat(mean(G1), N, 1) ;
        G2 = G2-repmat(mean(G2), N, 1) ;

        minrank = MIN(size(G1, 1), size(G2, 1)) ;
        numeig = MIN(numeig, minrank) ;

        N1 = size(G1, 1) ;
        N2 = size(G2, 1) ;

        Z11 = zeros<mat>(N1, N1) ;
        Z22 = zeros<mat>(N2, N2) ;
        Z12 = zeros<mat>(N1, N2) ;
        I11 = eye(N1, N1) ;
        I22 = eye(N2, N2) ;

        R = join_cols(join_rows(Z11, trans(G1)*G2), join_rows(trans(G2)*G1, Z22)) ;
        D = join_cols(join_rows(trans(G1)*G1+reg*I11, Z12),
                      join_rows(trans(Z12), trans(G2)*G2+reg*I22)) ;

        eig_pair(eigval, eigvec, R, D);

        betas = real(eigval);
        alphas = real(eigvec);

        betass = sort(betas);
        ind = sort_index(betas);

        alpha = alphas.cols(ind(regspace<uvec>(ind.n_rows-1, -1, ind.n_rows-numeig))) ;
        alpha_norms = sqrt((sum(square(alpha), 0))) ;
        alpha = alpha/repmat(alpha_norms, N1+N2, 1) ;
//        beta = betass(regspace<uvec>(betass.n_rows-1, -1, betass.n_rows-numeig)) ;
        beta = betass;
        alpha1 = alpha.rows(0, N1-1) ;
        alpha2 = alpha.rows(N1, alpha.n_rows-1) ;
        y1 = G1*alpha1 ;
        y2 = G2*alpha2 ;
    }
    else if ("full" == decomp)
    {
        //       I = eye(N) ;
        //       Z = arma::zeros<mat>(N) ;
        //       N0 = eye(N)-1*1.0/N*arma::ones<mat>(N) ;
        //       K1 = N0*kernel(X1, X1, kernel(), kernelpar)*N0 ;
        //       K2 = N0*kernel(X2, X2, kernel(), kernelpar)*N0 ;
        //       minrank = min(rank(K1), rank(K2)) ;
        //       numeig = min(numeig, minrank) ;
        //       R = 1/2.0*{arma::join_rows(K1, K2), arma::join_rows(K1, K2)} ;
        //       D = {arma::join_rows(K1+reg*I, Z), arma::join_rows(Z, K2+reg*I)} ;
        //       [alphas, betas] = eig(R, D) ;
        //       [betass, ind] = sort(arma::real(diagvec(betas))) ;
        //       alpha = alphas.cols(ind(m2cpp::fspan(ind.n_rows, -1, ind.n_rows-numeig+1))) ;
        //       alpha_norms = sqrt((arma::sum(arma::square(alpha), 1))) ;
        //       alpha = alpha/repmat(alpha_norms, 2*N, 1) ;
        //       beta = betass(m2cpp::fspan(betass.n_rows, -1, betass.n_rows-numeig+1)) ;
        //       alpha1 = alpha.rows(m2cpp::fspan(1, 1, N)) ;
        //       alpha2 = alpha.rows(m2cpp::fspan(N+1, 1, alpha.n_rows)) ;
        //       y1 = K1*alpha1 ;
        //       y2 = K2*alpha2 ;
    }
}

mat refsig(double f, double S, uword T, uword N)
{
    mat y ;
    double t;
    y = arma::zeros<mat>(2*N, T) ;
    for (uword i=0; i<N; i++)
    {
        for (uword j=0; j<T; j++)
        {
            t = double(j/S) ;
            y(2*i, j) = sin(2*datum::pi*((i+1)*f)*t) ;
            y(2*i+1, j) = cos(2*datum::pi*((i+1)*f)*t) ;
        }
    }
    return y ;
}
