// Description:
// initial value predition based on inertial weighting model

// 1. parser of csv file containing all reference screens MSG;
// 2. matrix transformation and calculation;
// 3. input of optimal value and output of initial value.

#include <stdio.h>
#include "inertial_weighting.h"

#define REF_LIST_SIZE REF_SCREEN_NUM * REF_BAND_NUM * REF_BIND_NUM                 // CSV待解析行数
#define BUFFER_SIZE 1024                                                           // 默认缓冲区大小
#define epsilon 1.e-9                                                              // 默认计算精度
#define omega 0.                                                                   // 默认超参数

void loadRefCSV_IW(const char* filename, RefReg_IW* RefReg_list);                  // 加载参考CSV文件
void initW_IW(W_IW* W);                                                            // 初始化W矢量
void predInitialRGB_IW(TarReg_IW* initRGB, RefReg_IW* RefReg_list, W_IW* W);       // 预测初始RGB
void feedOptimalRGB_IW(TarReg_IW* optiRGB, RefReg_IW* RefReg_list, W_IW* W);       // 喂入最优RGB, 修正W

void loadRefCSV_IW(const char* filename, RefReg_IW* RefReg_list) {
    FILE* f;

    if (fopen_s(&f, filename, "rt"))
        return;

    int totalIdx;
    char line[BUFFER_SIZE];
    fgets(line, BUFFER_SIZE, f);

    for (int i = 0; i < REF_LIST_SIZE; ++i) {
        fgets(line, BUFFER_SIZE, f);
        sscanf_s(line, "%d-%d-%d-%d,%x,%x,%x", &(RefReg_list[i].screenIdx),
            &(RefReg_list[i].bandIdx),
            &totalIdx,
            &(RefReg_list[i].grayVal),
            &(RefReg_list[i].Reg_rgb[0]),
            &(RefReg_list[i].Reg_rgb[1]),
            &(RefReg_list[i].Reg_rgb[2]));
    }

    fclose(f);
}

void initW_IW(W_IW* W) {
    for (int i = 0; i < REF_SCREEN_NUM; ++i) {
        W->W255[i][0] = 1. / REF_SCREEN_NUM;
        W->Wold[i][0] = 1. / REF_SCREEN_NUM;
    }
}

void updateA(int bandIdx, int grayVal, RefReg_IW* RefReg_list, double(*A)[REF_SCREEN_NUM]) {
    int screenCnt = 0;

    for (int i = 0; i < REF_LIST_SIZE; ++i) {
        if (bandIdx == RefReg_list[i].bandIdx && grayVal == RefReg_list[i].grayVal) {
            A[0][screenCnt] = (double)RefReg_list[i].Reg_rgb[0];
            A[1][screenCnt] = (double)RefReg_list[i].Reg_rgb[1];
            A[2][screenCnt] = (double)RefReg_list[i].Reg_rgb[2];
            A[3][screenCnt] = 1.;

            ++screenCnt;
        }
    }
}

void updateInitRGB(TarReg_IW* initRGB, double(*A)[REF_SCREEN_NUM], double(*W)[1]) {
    for (int i = 0; i < 4; ++i) {
        double res = 0;

        for (int j = 0; j < REF_SCREEN_NUM; ++j) {
            res += A[i][j] * W[j][0];
        }

        initRGB->D[i][0] = (int)(res + 0.5);
    }
}

void predInitialRGB_IW(TarReg_IW* initRGB, RefReg_IW* RefReg_list, W_IW* W) {
    int bandIdx = initRGB->bandIdx;
    int grayVal = initRGB->grayVal;

    double A[4][REF_SCREEN_NUM];
    updateA(bandIdx, grayVal, RefReg_list, A);

    double(*W_old)[1] = (grayVal == 255) ? W->W255 : W->Wold;
    updateInitRGB(initRGB, A, W_old);
}

void updateD(TarReg_IW* optiRGB, double(*D)[1]) {
    for (int i = 0; i < 4; ++i) {
        D[i][0] = (double)optiRGB->D[i][0];
    }
}

void getItem0(double(*A)[REF_SCREEN_NUM], double(*item0)[4]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            item0[i][j] = 0;
            for (int k = 0; k < REF_SCREEN_NUM; ++k)
                item0[i][j] += A[i][k] * A[j][k];
            if (i == j)
                item0[i][j] += epsilon;
        }
    }
}

double calcDet_3x3(double(*mat)[3]) {
    double item1 = mat[0][0] * mat[1][1] * mat[2][2] + mat[0][1] * mat[1][2] * mat[2][0] + mat[1][0] * mat[2][1] * mat[0][2];
    double item2 = mat[0][2] * mat[1][1] * mat[2][0] + mat[0][1] * mat[1][0] * mat[2][2] + mat[0][0] * mat[1][2] * mat[2][1];

    return item1 - item2;
}

double calcDet_4x4(double(*mat)[4]) {
    double item1 = mat[0][0] * mat[1][1] * mat[2][2] * mat[3][3] - mat[0][0] * mat[1][1] * mat[2][3] * mat[3][2] - mat[0][0] * mat[1][2] * mat[2][1] * mat[3][3] + mat[0][0] * mat[1][2] * mat[2][3] * mat[3][1];
    double item2 = mat[0][0] * mat[1][3] * mat[2][1] * mat[3][2] - mat[0][0] * mat[1][3] * mat[2][2] * mat[3][1] - mat[0][1] * mat[1][0] * mat[2][2] * mat[3][3] + mat[0][1] * mat[1][0] * mat[2][3] * mat[3][2];
    double item3 = mat[0][1] * mat[1][2] * mat[2][0] * mat[3][3] - mat[0][1] * mat[1][2] * mat[2][3] * mat[3][0] - mat[0][1] * mat[1][3] * mat[2][0] * mat[3][2] + mat[0][1] * mat[1][3] * mat[2][2] * mat[3][0];
    double item4 = mat[0][2] * mat[1][0] * mat[2][1] * mat[3][3] - mat[0][2] * mat[1][0] * mat[2][3] * mat[3][1] - mat[0][2] * mat[1][1] * mat[2][0] * mat[3][3] + mat[0][2] * mat[1][1] * mat[2][3] * mat[3][0];
    double item5 = mat[0][2] * mat[1][3] * mat[2][0] * mat[3][1] - mat[0][2] * mat[1][3] * mat[2][1] * mat[3][0] - mat[0][3] * mat[1][0] * mat[2][1] * mat[3][2] + mat[0][3] * mat[1][0] * mat[2][2] * mat[3][1];
    double item6 = mat[0][3] * mat[1][1] * mat[2][0] * mat[3][2] - mat[0][3] * mat[1][1] * mat[2][2] * mat[3][0] - mat[0][3] * mat[1][2] * mat[2][0] * mat[3][1] + mat[0][3] * mat[1][2] * mat[2][1] * mat[3][0];

    return item1 + item2 + item3 + item4 + item5 + item6;
}

// 计算伴随矩阵元素
double calcAdjointEle(double(*item0)[4], int i, int j) {
    double subItem[3][3];
    int row = 0;
    for (int i2 = 0; i2 < 4; ++i2) {
        if (i2 == i) continue;
        int col = 0;
        for (int j2 = 0; j2 < 4; ++j2) {
            if (j2 == j) continue;
            subItem[row][col] = item0[i2][j2];
            ++col;
        }
        ++row;
    }

    int symbol = -1;
    if ((i + j + 2) % 2 == 0)
        symbol = 1;

    double val = calcDet_3x3(subItem) * symbol;
    return val;
}

void getItem1(double(*item0)[4], double det_item0, double(*item1)[4]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j)
            item1[j][i] = calcAdjointEle(item0, i, j) / det_item0;
    }
}

void getItem2(double(*A)[REF_SCREEN_NUM], double(*W_old)[1], double(*D)[1], double(*item2)[1]) {
    for (int i = 0; i < 4; ++i) {
        item2[i][0] = 0;
        for (int j = 0; j < REF_SCREEN_NUM; ++j)
            item2[i][0] += A[i][j] * W_old[j][0];

        item2[i][0] -= D[i][0] * (1. + omega);
    }
}


void updateW(int grayVal, double(*A)[REF_SCREEN_NUM], double(*D)[1], W_IW* W) {
    double(*W_old)[1] = (grayVal == 255) ? W->W255 : W->Wold;

    double item0[4][4];
    getItem0(A, item0);

    double det_item0 = calcDet_4x4(item0);
    double item1[4][4];
    getItem1(item0, det_item0, item1);         // calculate inverse matrix

    double item2[4][1];
    getItem2(A, W_old, D, item2);

    double beta[4][1];
    for (int i = 0; i < 4; ++i) {
        beta[i][0] = 0;
        for (int j = 0; j < 4; ++j)
            beta[i][0] += item1[i][j] * item2[j][0];
    }

    for (int i = 0; i < REF_SCREEN_NUM; ++i) {
        for (int j = 0; j < 4; ++j) {
            W_old[i][0] -= A[j][i] * beta[j][0];
        }
        W_old[i][0] /= (1. + omega);

        if (grayVal == 255)
            W->Wold[i][0] = W_old[i][0];
    }
}

void feedOptimalRGB_IW(TarReg_IW* optiRGB, RefReg_IW* RefReg_list, W_IW* W) {
    int bandIdx = optiRGB->bandIdx;
    int grayVal = optiRGB->grayVal;

    double A[4][REF_SCREEN_NUM];
    updateA(bandIdx, grayVal, RefReg_list, A);

    double D[4][1];
    updateD(optiRGB, D);

    updateW(grayVal, A, D, W);
}





