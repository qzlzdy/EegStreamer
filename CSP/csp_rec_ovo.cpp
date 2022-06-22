#include "CCA/rt_nonfinite.h"
#include "csp_rec_ovo.h"

using namespace std;

// Function Declarations
static double KNN_(const double data[180], const double t_data[6]);
static double b_KNN_(const double data[180], const double t_data[6]);
static double c_KNN_(const double data[180], const double t_data[6]);
static void diag(const double v[36], double d[6]);
static void eml_sort(const double x[30], double y[30], int idx[30]);
static double sum(const double x[6]);

// Function Definitions

//
// KNN_: classifying using k-nearest neighbors algorithm. The nearest neighbors
// search method is euclidean distance
// Usage:
//        [predicted_labels,nn_index,accuracy] = KNN_(3,training,training_labels,testing,testing_labels)
//        predicted_labels = KNN_(3,training,training_labels,testing)
// Input:
//        - k: number of nearest neighbors
//        - data: (NxD) training data; N is the number of samples and D is the
//        dimensionality of each data point
//        - labels: training labels
//        - t_data: (MxD) testing data; M is the number of data points and D
//        is the dimensionality of each data point
//        - t_labels: testing labels (default = [])
// Output:
//        - predicted_labels: the predicted labels based on the k-NN
//        algorithm
//        - nn_index: the index of the nearest training data point for each training sample (Mx1).
//        - accuracy: if the testing labels are supported, the accuracy of
//        the classification is returned, otherwise it will be zero.
// Author: Mahmoud Afifi - York University
// Arguments    : const double data[180]
//                const double t_data[6]
// Return Type  : double
//
static double KNN_(const double data[180], const double t_data[6])
{
    double predicted_labels;
    double ed[30];
    int nb;
    double y[6];
    int x;
    double b_y;
    int max_count;
    double b_ed[30];
    int iidx[30];
    signed char a[3];
    static const signed char labels[30] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                            1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };

    signed char idx[3];
    signed char options_data[3];
    int32_T exitg2;
    int max_label;
    int loop_ub;
    int j;
    boolean_T b_x[3];
    boolean_T exitg1;
    boolean_T guard1 = false;

    // checks
    // initialization
    // ed: (MxN) euclidean distances
    // corresponding indices (MxN)
    // k-nearest neighbors for testing sample (Mxk)
    // calc euclidean distances between each testing data point and the training
    // data samples
    for (nb = 0; nb < 30; nb++) {
        // calc and store sorted euclidean distances with corresponding indices
        for (x = 0; x < 6; x++) {
            b_y = t_data[x] - data[nb + 30 * x];
            y[x] = b_y * b_y;
        }

        b_y = y[0];
        for (max_count = 0; max_count < 5; max_count++) {
            b_y += y[max_count + 1];
        }

        ed[nb] = sqrt(b_y);
    }

    memcpy(&b_ed[0], &ed[0], 30U * sizeof(double));
    eml_sort(b_ed, ed, iidx);
    for (x = 0; x < 30; x++) {
        ed[x] = iidx[x];
    }

    // find the nearest k for each data point of the testing data
    // get the majority vote
    for (x = 0; x < 3; x++) {
        a[x] = labels[(int)ed[x] - 1];
    }

    if (a[0] <= a[1]) {
        if (a[1] <= a[2]) {
            idx[0] = 1;
            idx[1] = 2;
            idx[2] = 3;
        } else if (a[0] <= a[2]) {
            idx[0] = 1;
            idx[1] = 3;
            idx[2] = 2;
        } else {
            idx[0] = 3;
            idx[1] = 1;
            idx[2] = 2;
        }
    } else if (a[0] <= a[2]) {
        idx[0] = 2;
        idx[1] = 1;
        idx[2] = 3;
    } else if (a[1] <= a[2]) {
        idx[0] = 2;
        idx[1] = 3;
        idx[2] = 1;
    } else {
        idx[0] = 3;
        idx[1] = 2;
        idx[2] = 1;
    }

    for (max_count = 0; max_count < 3; max_count++) {
        options_data[max_count] = a[idx[max_count] - 1];
    }

    nb = -1;
    max_count = 1;
    while (max_count <= 3) {
        x = options_data[max_count - 1];
        do {
            exitg2 = 0;
            max_count++;
            if (max_count > 3) {
                exitg2 = 1;
            } else {
                frexp((double)x / 2.0, &max_label);
                if (!(fabs((double)(x - options_data[max_count - 1])) < ldexp(1.0,
                                                                              max_label - 53))) {
                    exitg2 = 1;
                }
            }
        } while (exitg2 == 0);

        nb++;
        options_data[nb] = (signed char)x;
    }

    if (1 > nb + 1) {
        loop_ub = -1;
    } else {
        loop_ub = nb;
    }

    for (x = 0; x <= loop_ub; x++) {
        a[x] = options_data[x];
    }

    nb = loop_ub + 1;
    for (x = 0; x < nb; x++) {
        options_data[x] = a[x];
    }

    max_count = 0;
    max_label = 0;
    for (j = 0; j <= loop_ub; j++) {
        for (x = 0; x < 3; x++) {
            b_x[x] = (labels[(int)ed[x] - 1] == options_data[j]);
        }

        nb = 0;
        x = 1;
        exitg1 = false;
        while ((!exitg1) && (x < 4)) {
            guard1 = false;
            if (b_x[x - 1]) {
                nb++;
                if (nb >= 3) {
                    exitg1 = true;
                } else {
                    guard1 = true;
                }
            } else {
                guard1 = true;
            }

            if (guard1) {
                x++;
            }
        }

        if (1 > nb) {
            x = 0;
        } else {
            x = nb;
        }

        if (x > max_count) {
            max_label = options_data[j];
            max_count = x;
        }
    }

    predicted_labels = max_label;

    // calculate the classification accuracy
    return predicted_labels;
}

//
// KNN_: classifying using k-nearest neighbors algorithm. The nearest neighbors
// search method is euclidean distance
// Usage:
//        [predicted_labels,nn_index,accuracy] = KNN_(3,training,training_labels,testing,testing_labels)
//        predicted_labels = KNN_(3,training,training_labels,testing)
// Input:
//        - k: number of nearest neighbors
//        - data: (NxD) training data; N is the number of samples and D is the
//        dimensionality of each data point
//        - labels: training labels
//        - t_data: (MxD) testing data; M is the number of data points and D
//        is the dimensionality of each data point
//        - t_labels: testing labels (default = [])
// Output:
//        - predicted_labels: the predicted labels based on the k-NN
//        algorithm
//        - nn_index: the index of the nearest training data point for each training sample (Mx1).
//        - accuracy: if the testing labels are supported, the accuracy of
//        the classification is returned, otherwise it will be zero.
// Author: Mahmoud Afifi - York University
// Arguments    : const double data[180]
//                const double t_data[6]
// Return Type  : double
//
static double b_KNN_(const double data[180], const double t_data[6])
{
    double predicted_labels;
    double ed[30];
    int nb;
    double y[6];
    int x;
    double b_y;
    int max_count;
    double b_ed[30];
    int iidx[30];
    signed char a[3];
    static const signed char labels[30] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                            1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };

    signed char idx[3];
    signed char options_data[3];
    int32_T exitg2;
    int max_label;
    int loop_ub;
    int j;
    boolean_T b_x[3];
    boolean_T exitg1;
    boolean_T guard1 = false;

    // checks
    // initialization
    // ed: (MxN) euclidean distances
    // corresponding indices (MxN)
    // k-nearest neighbors for testing sample (Mxk)
    // calc euclidean distances between each testing data point and the training
    // data samples
    for (nb = 0; nb < 30; nb++) {
        // calc and store sorted euclidean distances with corresponding indices
        for (x = 0; x < 6; x++) {
            b_y = t_data[x] - data[nb + 30 * x];
            y[x] = b_y * b_y;
        }

        b_y = y[0];
        for (max_count = 0; max_count < 5; max_count++) {
            b_y += y[max_count + 1];
        }

        ed[nb] = sqrt(b_y);
    }

    memcpy(&b_ed[0], &ed[0], 30U * sizeof(double));
    eml_sort(b_ed, ed, iidx);
    for (x = 0; x < 30; x++) {
        ed[x] = iidx[x];
    }

    // find the nearest k for each data point of the testing data
    // get the majority vote
    for (x = 0; x < 3; x++) {
        a[x] = labels[(int)ed[x] - 1];
    }

    if (a[0] <= a[1]) {
        if (a[1] <= a[2]) {
            idx[0] = 1;
            idx[1] = 2;
            idx[2] = 3;
        } else if (a[0] <= a[2]) {
            idx[0] = 1;
            idx[1] = 3;
            idx[2] = 2;
        } else {
            idx[0] = 3;
            idx[1] = 1;
            idx[2] = 2;
        }
    } else if (a[0] <= a[2]) {
        idx[0] = 2;
        idx[1] = 1;
        idx[2] = 3;
    } else if (a[1] <= a[2]) {
        idx[0] = 2;
        idx[1] = 3;
        idx[2] = 1;
    } else {
        idx[0] = 3;
        idx[1] = 2;
        idx[2] = 1;
    }

    for (max_count = 0; max_count < 3; max_count++) {
        options_data[max_count] = a[idx[max_count] - 1];
    }

    nb = -1;
    max_count = 1;
    while (max_count <= 3) {
        x = options_data[max_count - 1];
        do {
            exitg2 = 0;
            max_count++;
            if (max_count > 3) {
                exitg2 = 1;
            } else {
                frexp((double)x / 2.0, &max_label);
                if (!(fabs((double)(x - options_data[max_count - 1])) < ldexp(1.0,
                                                                              max_label - 53))) {
                    exitg2 = 1;
                }
            }
        } while (exitg2 == 0);

        nb++;
        options_data[nb] = (signed char)x;
    }

    if (1 > nb + 1) {
        loop_ub = -1;
    } else {
        loop_ub = nb;
    }

    for (x = 0; x <= loop_ub; x++) {
        a[x] = options_data[x];
    }

    nb = loop_ub + 1;
    for (x = 0; x < nb; x++) {
        options_data[x] = a[x];
    }

    max_count = 0;
    max_label = 0;
    for (j = 0; j <= loop_ub; j++) {
        for (x = 0; x < 3; x++) {
            b_x[x] = (labels[(int)ed[x] - 1] == options_data[j]);
        }

        nb = 0;
        x = 1;
        exitg1 = false;
        while ((!exitg1) && (x < 4)) {
            guard1 = false;
            if (b_x[x - 1]) {
                nb++;
                if (nb >= 3) {
                    exitg1 = true;
                } else {
                    guard1 = true;
                }
            } else {
                guard1 = true;
            }

            if (guard1) {
                x++;
            }
        }

        if (1 > nb) {
            x = 0;
        } else {
            x = nb;
        }

        if (x > max_count) {
            max_label = options_data[j];
            max_count = x;
        }
    }

    predicted_labels = max_label;

    // calculate the classification accuracy
    return predicted_labels;
}

//
// KNN_: classifying using k-nearest neighbors algorithm. The nearest neighbors
// search method is euclidean distance
// Usage:
//        [predicted_labels,nn_index,accuracy] = KNN_(3,training,training_labels,testing,testing_labels)
//        predicted_labels = KNN_(3,training,training_labels,testing)
// Input:
//        - k: number of nearest neighbors
//        - data: (NxD) training data; N is the number of samples and D is the
//        dimensionality of each data point
//        - labels: training labels
//        - t_data: (MxD) testing data; M is the number of data points and D
//        is the dimensionality of each data point
//        - t_labels: testing labels (default = [])
// Output:
//        - predicted_labels: the predicted labels based on the k-NN
//        algorithm
//        - nn_index: the index of the nearest training data point for each training sample (Mx1).
//        - accuracy: if the testing labels are supported, the accuracy of
//        the classification is returned, otherwise it will be zero.
// Author: Mahmoud Afifi - York University
// Arguments    : const double data[180]
//                const double t_data[6]
// Return Type  : double
//
static double c_KNN_(const double data[180], const double t_data[6])
{
    double predicted_labels;
    double ed[30];
    int nb;
    double y[6];
    int x;
    double b_y;
    int max_count;
    double b_ed[30];
    int iidx[30];
    signed char a[3];
    static const signed char labels[30] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                                            2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };

    signed char idx[3];
    signed char options_data[3];
    int32_T exitg2;
    int max_label;
    int loop_ub;
    int j;
    boolean_T b_x[3];
    boolean_T exitg1;
    boolean_T guard1 = false;

    // checks
    // initialization
    // ed: (MxN) euclidean distances
    // corresponding indices (MxN)
    // k-nearest neighbors for testing sample (Mxk)
    // calc euclidean distances between each testing data point and the training
    // data samples
    for (nb = 0; nb < 30; nb++) {
        // calc and store sorted euclidean distances with corresponding indices
        for (x = 0; x < 6; x++) {
            b_y = t_data[x] - data[nb + 30 * x];
            y[x] = b_y * b_y;
        }

        b_y = y[0];
        for (max_count = 0; max_count < 5; max_count++) {
            b_y += y[max_count + 1];
        }

        ed[nb] = sqrt(b_y);
    }

    memcpy(&b_ed[0], &ed[0], 30U * sizeof(double));
    eml_sort(b_ed, ed, iidx);
    for (x = 0; x < 30; x++) {
        ed[x] = iidx[x];
    }

    // find the nearest k for each data point of the testing data
    // get the majority vote
    for (x = 0; x < 3; x++) {
        a[x] = labels[(int)ed[x] - 1];
    }

    if (a[0] <= a[1]) {
        if (a[1] <= a[2]) {
            idx[0] = 1;
            idx[1] = 2;
            idx[2] = 3;
        } else if (a[0] <= a[2]) {
            idx[0] = 1;
            idx[1] = 3;
            idx[2] = 2;
        } else {
            idx[0] = 3;
            idx[1] = 1;
            idx[2] = 2;
        }
    } else if (a[0] <= a[2]) {
        idx[0] = 2;
        idx[1] = 1;
        idx[2] = 3;
    } else if (a[1] <= a[2]) {
        idx[0] = 2;
        idx[1] = 3;
        idx[2] = 1;
    } else {
        idx[0] = 3;
        idx[1] = 2;
        idx[2] = 1;
    }

    for (max_count = 0; max_count < 3; max_count++) {
        options_data[max_count] = a[idx[max_count] - 1];
    }

    nb = -1;
    max_count = 1;
    while (max_count <= 3) {
        x = options_data[max_count - 1];
        do {
            exitg2 = 0;
            max_count++;
            if (max_count > 3) {
                exitg2 = 1;
            } else {
                frexp((double)x / 2.0, &max_label);
                if (!(fabs((double)(x - options_data[max_count - 1])) < ldexp(1.0,
                                                                              max_label - 53))) {
                    exitg2 = 1;
                }
            }
        } while (exitg2 == 0);

        nb++;
        options_data[nb] = (signed char)x;
    }

    if (1 > nb + 1) {
        loop_ub = -1;
    } else {
        loop_ub = nb;
    }

    for (x = 0; x <= loop_ub; x++) {
        a[x] = options_data[x];
    }

    nb = loop_ub + 1;
    for (x = 0; x < nb; x++) {
        options_data[x] = a[x];
    }

    max_count = 0;
    max_label = 0;
    for (j = 0; j <= loop_ub; j++) {
        for (x = 0; x < 3; x++) {
            b_x[x] = (labels[(int)ed[x] - 1] == options_data[j]);
        }

        nb = 0;
        x = 1;
        exitg1 = false;
        while ((!exitg1) && (x < 4)) {
            guard1 = false;
            if (b_x[x - 1]) {
                nb++;
                if (nb >= 3) {
                    exitg1 = true;
                } else {
                    guard1 = true;
                }
            } else {
                guard1 = true;
            }

            if (guard1) {
                x++;
            }
        }

        if (1 > nb) {
            x = 0;
        } else {
            x = nb;
        }

        if (x > max_count) {
            max_label = options_data[j];
            max_count = x;
        }
    }

    predicted_labels = max_label;

    // calculate the classification accuracy
    return predicted_labels;
}

//
// Arguments    : const double v[36]
//                double d[6]
// Return Type  : void
//
static void diag(const double v[36], double d[6])
{
    int j;
    for (j = 0; j < 6; j++) {
        d[j] = v[j * 7];
    }
}

//
// Arguments    : const double x[30]
//                double y[30]
//                int idx[30]
// Return Type  : void
//
static void eml_sort(const double x[30], double y[30], int idx[30])
{
    int k;
    boolean_T p;
    signed char idx0[30];
    int i;
    int i2;
    int j;
    int pEnd;
    int b_p;
    int q;
    int qEnd;
    int kEnd;
    for (k = 0; k < 30; k++) {
        idx[k] = k + 1;
    }

    for (k = 0; k < 30; k += 2) {
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

    for (i = 0; i < 30; i++) {
        idx0[i] = 1;
    }

    i = 2;
    while (i < 30) {
        i2 = i << 1;
        j = 1;
        for (pEnd = 1 + i; pEnd < 31; pEnd = qEnd + i) {
            b_p = j;
            q = pEnd - 1;
            qEnd = j + i2;
            if (qEnd > 31) {
                qEnd = 31;
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

    for (k = 0; k < 30; k++) {
        y[k] = x[idx[k] - 1];
    }
}

//
// Arguments    : const double x[6]
// Return Type  : double
//
static double sum(const double x[6])
{
    double y;
    int k;
    y = x[0];
    for (k = 0; k < 5; k++) {
        y += x[k + 1];
    }

    return y;
}

//
// data-21*600
//  W-21*6
//  sample-6*38
//  classlable-1*38
// Arguments    : const double data[12600]
//                const double W1[126]
//                const double sample1[180]
//                const double W2[126]
//                const double sample2[180]
//                const double W3[126]
//                const double sample3[180]
// Return Type  : double
//
double csp_rec_ovo(const double data[12600], vector<double> W1, vector<double>
sample1, vector<double> W2, vector<double> sample2,
vector<double> W3, vector<double> sample3)
{
    double V;
    double b_data[12600];
    int idx;
    int b_idx;
    double c_data[36];
    double d_data[3600];
    double e_data[126];
    int ii;
    double p1[6];
    double p1_real[6];
    double b_sample1[180];
    double predic_label_1;
    double predic_label_2;
    double predic_label_3;
    double predeic_lable[3];
    boolean_T exitg3;
    boolean_T guard3 = false;
    boolean_T exitg2;
    boolean_T guard2 = false;
    int c_idx;
    boolean_T exitg1;
    boolean_T guard1 = false;
    int i0;
    int i1;
    int i2;
    for (idx = 0; idx < 21; idx++) {
        for (b_idx = 0; b_idx < 600; b_idx++) {
            b_data[b_idx + 600 * idx] = data[idx + 21 * b_idx];
        }
    }

    // 训练样本分类标记
    // % 左-右分类 1 2
    for (idx = 0; idx < 6; idx++) {
        for (b_idx = 0; b_idx < 600; b_idx++) {
            d_data[idx + 6 * b_idx] = 0.0;
            for (ii = 0; ii < 21; ii++) {
                d_data[idx + 6 * b_idx] += b_data[b_idx + 600 * ii] * W1[ii + 21 * idx];
            }
        }

        for (b_idx = 0; b_idx < 21; b_idx++) {
            e_data[idx + 6 * b_idx] = 0.0;
            for (ii = 0; ii < 600; ii++) {
                e_data[idx + 6 * b_idx] += d_data[idx + 6 * ii] * b_data[ii + 600 *
                        b_idx];
            }
        }

        for (b_idx = 0; b_idx < 6; b_idx++) {
            c_data[idx + 6 * b_idx] = 0.0;
            for (ii = 0; ii < 21; ii++) {
                c_data[idx + 6 * b_idx] += e_data[idx + 6 * ii] * W1[ii + 21 * b_idx];
            }
        }
    }

    diag(c_data, p1);
    for (idx = 0; idx < 6; idx++) {
        p1[idx] /= 600.0;
    }

    // 取第一类的数据
    for (b_idx = 0; b_idx < 6; b_idx++) {
        p1_real[b_idx] = log(p1[b_idx] / sum(p1));
        for (idx = 0; idx < 30; idx++) {
            b_sample1[idx + 30 * b_idx] = sample1[b_idx + 6 * idx];
        }
    }

    predic_label_1 = KNN_(b_sample1, p1_real);

    // % 左-空闲分类 1 3
    for (idx = 0; idx < 6; idx++) {
        for (b_idx = 0; b_idx < 600; b_idx++) {
            d_data[idx + 6 * b_idx] = 0.0;
            for (ii = 0; ii < 21; ii++) {
                d_data[idx + 6 * b_idx] += b_data[b_idx + 600 * ii] * W2[ii + 21 * idx];
            }
        }

        for (b_idx = 0; b_idx < 21; b_idx++) {
            e_data[idx + 6 * b_idx] = 0.0;
            for (ii = 0; ii < 600; ii++) {
                e_data[idx + 6 * b_idx] += d_data[idx + 6 * ii] * b_data[ii + 600 *
                        b_idx];
            }
        }

        for (b_idx = 0; b_idx < 6; b_idx++) {
            c_data[idx + 6 * b_idx] = 0.0;
            for (ii = 0; ii < 21; ii++) {
                c_data[idx + 6 * b_idx] += e_data[idx + 6 * ii] * W2[ii + 21 * b_idx];
            }
        }
    }

    diag(c_data, p1_real);
    for (idx = 0; idx < 6; idx++) {
        p1_real[idx] /= 600.0;
    }

    // 取第一类的数据
    for (b_idx = 0; b_idx < 6; b_idx++) {
        p1[b_idx] = log(p1_real[b_idx] / sum(p1_real));
        for (idx = 0; idx < 30; idx++) {
            b_sample1[idx + 30 * b_idx] = sample2[b_idx + 6 * idx];
        }
    }

    predic_label_2 = b_KNN_(b_sample1, p1);

    // % 右-空闲分类 2 3
    for (idx = 0; idx < 6; idx++) {
        for (b_idx = 0; b_idx < 600; b_idx++) {
            d_data[idx + 6 * b_idx] = 0.0;
            for (ii = 0; ii < 21; ii++) {
                d_data[idx + 6 * b_idx] += b_data[b_idx + 600 * ii] * W3[ii + 21 * idx];
            }
        }

        for (b_idx = 0; b_idx < 21; b_idx++) {
            e_data[idx + 6 * b_idx] = 0.0;
            for (ii = 0; ii < 600; ii++) {
                e_data[idx + 6 * b_idx] += d_data[idx + 6 * ii] * b_data[ii + 600 *
                        b_idx];
            }
        }

        for (b_idx = 0; b_idx < 6; b_idx++) {
            c_data[idx + 6 * b_idx] = 0.0;
            for (ii = 0; ii < 21; ii++) {
                c_data[idx + 6 * b_idx] += e_data[idx + 6 * ii] * W3[ii + 21 * b_idx];
            }
        }
    }

    diag(c_data, p1_real);
    for (idx = 0; idx < 6; idx++) {
        p1_real[idx] /= 600.0;
    }

    // 取第一类的数据
    for (b_idx = 0; b_idx < 6; b_idx++) {
        p1[b_idx] = log(p1_real[b_idx] / sum(p1_real));
        for (idx = 0; idx < 30; idx++) {
            b_sample1[idx + 30 * b_idx] = sample3[b_idx + 6 * idx];
        }
    }

    predic_label_3 = c_KNN_(b_sample1, p1);

    // % 投票
    predeic_lable[0] = predic_label_1;
    predeic_lable[1] = predic_label_2;
    predeic_lable[2] = predic_label_3;
    b_idx = 0;
    ii = 1;
    exitg3 = false;
    while ((!exitg3) && (ii < 4)) {
        guard3 = false;
        if (predeic_lable[ii - 1] == 1.0) {
            b_idx++;
            if (b_idx >= 3) {
                exitg3 = true;
            } else {
                guard3 = true;
            }
        } else {
            guard3 = true;
        }

        if (guard3) {
            ii++;
        }
    }

    idx = 0;
    ii = 1;
    exitg2 = false;
    while ((!exitg2) && (ii < 4)) {
        guard2 = false;
        if (predeic_lable[ii - 1] == 2.0) {
            idx++;
            if (idx >= 3) {
                exitg2 = true;
            } else {
                guard2 = true;
            }
        } else {
            guard2 = true;
        }

        if (guard2) {
            ii++;
        }
    }

    c_idx = 0;
    ii = 1;
    exitg1 = false;
    while ((!exitg1) && (ii < 4)) {
        guard1 = false;
        if (predeic_lable[ii - 1] == 3.0) {
            c_idx++;
            if (c_idx >= 3) {
                exitg1 = true;
            } else {
                guard1 = true;
            }
        } else {
            guard1 = true;
        }

        if (guard1) {
            ii++;
        }
    }

    V = -1.0;
    if (1 > b_idx) {
        i0 = 0;
    } else {
        i0 = b_idx;
    }

    if (i0 == 2) {
        V = 1.0;
    }

    if (1 > idx) {
        i1 = 0;
    } else {
        i1 = idx;
    }

    if (i1 == 2) {
        V = 2.0;
    }

    if (1 > c_idx) {
        i2 = 0;
    } else {
        i2 = c_idx;
    }

    if (i2 == 2) {
        V = 3.0;
    }

    return V;
}

//
// Arguments    : void
// Return Type  : void
//
void csp_rec_ovo_initialize()
{
    rt_InitInfAndNaN(8U);
}

//
// Arguments    : void
// Return Type  : void
//
void csp_rec_ovo_terminate()
{
    // (no terminate code required)
}
