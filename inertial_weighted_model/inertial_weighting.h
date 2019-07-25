#pragma once

#define REF_SCREEN_NUM 59     // reference screens number
#define REF_BAND_NUM 6        // bands number on each screen
#define REF_BIND_NUM 15       // binds number on each band

typedef struct {
    int screenIdx;            // screen index
    int bandIdx;              // band index
    int grayVal;              // gray value
    int Reg_rgb[3];           // rgb register value
} RefReg_IW;

typedef struct {
    int bandIdx;
    int grayVal;
    int D[4][1];
} TarReg_IW;

typedef struct {
    double W255[REF_SCREEN_NUM][1];
    double Wold[REF_SCREEN_NUM][1];
} W_IW;

void loadRefCSV_IW(const char* filename, RefReg_IW* RefReg_list);                  // ���زο�CSV�ļ�(һ��������һ��)
void initW_IW(W_IW* W);                                                            // ��ʼ��Wʸ��(һ������ʼ��һ��)
void predInitialRGB_IW(TarReg_IW* initRGB, RefReg_IW* RefReg_list, W_IW* W);       // Ԥ���ʼRGB
void feedOptimalRGB_IW(TarReg_IW* optiRGB, RefReg_IW* RefReg_list, W_IW* W);       // ι������RGB