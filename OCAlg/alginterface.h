#pragma once

#ifdef  JC_GAMMA_DLL_EXPORTS
#define JC_GAMMA_DLL_API  __declspec(dllexport)
#else
#define JC_GAMMA_DLL_API  __declspec(dllimport)
#endif


#include <string>

namespace GammaAlg
{

#pragma region =============================================gamma�㷨�궨��======================================================

#define GAMMA_ERROR_SUCCESS					        0
#define GAMMA_FAILED							    -1

#define GAMMA_ERROR_BASE							(10000)
#define GAMMA_ERROR(idx)							((GAMMA_ERROR_BASE) + idx)

#define GAMMA_ERROR_INVALID_PARAM					GAMMA_ERROR(1)			// ��Ч���� �����ʽ����ȷ(��ȱ���ֶ�,�ֶ�ֵ���Ϸ���)
#define GAMMA_ERROR_PARAM_PTR_NULL				    GAMMA_ERROR(2)			// �����ָ��Ϊ��
#define GAMMA_ERROR_NOT_IMPL					    GAMMA_ERROR(3)			// ��ǰ�ķ���û��ʵ��
#define GAMMA_ERROR_PARAM_PATH                      GAMMA_ERROR(4)          // �����·���쳣
#define GAMMA_ERROR_MEMERY_ALLOC                    GAMMA_ERROR(5)          // �ڴ�����ʧ��
#define GAMMA_ERROR_PI_NOT_INIT                     GAMMA_ERROR(6)          // �㷨δ��ʼ���ɹ�
#define GAMMA_ERROR_OPEN_FILE                       GAMMA_ERROR(7)          // �ļ���ʧ��


#define PI_N_DELETE(X)                    if(X)      { delete []X; X = NULL;}
#define PI_DELETE(X)                      if(X)      { delete X;   X = NULL;}
#define PI_FREE(X)                        if(X)      { free(X);    X = NULL;}
#define PI_CLOSE(X)                       if(X)      { fclose(X);  X = NULL;}
#define CHECK_ERROR(state, error_code)    if (state) { LogError("error happend"); return error_code; }
#define CHECK_FORCE_STOP(state)           if (state) { state = !state; return ERROR_PI_FORCE_STOP; }
#define CHECK_ERROR_RETURN(state)         if (state) { return; }
#define CHECK_ERROR_CONTINUE(state)       if (state) { continue; }
#define CHECK_SUCESS(state, error_code)   if (state) { return error_code; }

#define g_CHAN_NUM 3

#pragma endregion


#pragma region =============================================gamma�㷨���ݽṹ======================================================


    /*�Ĵ����������*/
    typedef struct _RGBRegValue_
    {
        int rRegValue_;
        int gRegValue_;
        int bRegValue_;
    }RGBRegValue;

    typedef struct _RGBVolValue_
    {
        double rVolValue_;
        double gVolValue_;
        double bVolValue_;
    }RGBVolValue;

    typedef struct _CIElxylvInfo_
    {
        double x_;
        double y_;
        double lv_;
    }CIExylvInfo;

    typedef struct _GammaInputInfoReg_
    {
        int count_;                     // �ð��ڼ��ε���
        int bandNo_;                    // band��
        int grayValue_;                 // �ҽ�ֵ
        int bindIndex_;                 // ������
        int algMode_;                   // �㷨����	
        RGBRegValue regValue_;
        RGBVolValue volValue_;
        CIExylvInfo realxylv_;          // ��ʵLv��x��y
    }GammaInputInfoReg;

    typedef struct _GammaOutputInfoReg_
    {
        RGBRegValue regValue_;
        RGBVolValue volValue_;
    }GammaOutputInfoReg;

    typedef struct _RegConfig_
    {
        int regValue_;                  // �Ĵ�����ʼֵ
        int regMaxValue_;			    // �Ĵ������ֵ
        int regMinValue_;			    // �Ĵ�����Сֵ
    }RegConfig;

    typedef struct _VolConfig_
    {
        double volValue_;               // ��ѹ��ʼֵ
        double volMaxValue_;            // ��ѹ���ֵ
        double volMinValue_;            // ��ѹ��Сֵ	
    }VolConfig;

    typedef struct _RegOneUnitConfig_
    {
        RegConfig rRegConfig_;
        RegConfig gRegConfig_;
        RegConfig bRegConfig_;
    }RegOneUnitConfig;

    typedef struct _VolOneUnitConfig_
    {
        double GrayV0;
        double G_VREG1;
        double G_VREF1;
        double G_VTUPP;
        VolConfig rVolConfig_;
        VolConfig gVolConfig_;
        VolConfig bVolConfig_;
    }VolOneUnitConfig;

    typedef struct _GammaErrorConfig_
    {
        CIExylvInfo targetxylv_;           // Ŀ��Lv��x��y
        double xLeftError_;
        double xRightError_;
        double yLeftError_;
        double yRightError_;
        double lvLeftError_;
        double lvRightError_;
    }GammaErrorConfig;

    typedef struct _GammaOneUnitData_
    {
        int grayValue_;
        int bandNo_;
        int bindIndex_;
        GammaErrorConfig errorConfig_;
        VolOneUnitConfig volConfig_;
        RegOneUnitConfig regConfig_;
    }GammaOneUnitData;

    typedef struct _GammaInitData_
    {
        int allBindNum_;                   // ���а����
        int maxRegStep_;                   // �Ĵ�������������
        double maxVolStep_;                // ��ѹ����������
        GammaOneUnitData* pGammaAllData_;
    }GammaInitData;


#pragma endregion


	/**********************************************************************************************************************
	* ��  �ܣ�gamma�����㷨�� �㷨��ʼ������
	* ��  ���� in��
	*
	*         in  ����
	*                  ���� char *pID
	* ����ֵ��״̬�루ERROR_SUCESS/ ʧ�ܣ�
	* ��  ע:
	***********************************************************************************************************************/
	JC_GAMMA_DLL_API int gammaInit(const char* pID, const GammaInitData* pGammaData);

	/**********************************************************************************************************************
		* ��  �ܣ�gamma�����㷨�� �㷨����ʼ������
		* ��  ���� in��
		*                   handle           - �㷨����ָ�룬��ͬ�߳̾��в�ͬhandle
		*         out:
		*
		* ����ֵ��״̬�루ERROR_SUCESS/ ʧ�ܣ�
		* ��  ע:
		***********************************************************************************************************************/
	JC_GAMMA_DLL_API int gammaUnInit(const char* pID);

	/**********************************************************************************************************************
	* ��  �ܣ�gamma�����㷨�� gamma���ں���
	* ��  ���� in��
	*                   pID 			-- �߳�ID
	*                   pInput          -- �������		
	*         out:
	*                   pOutput         -- �������
	*
	* ����ֵ��״̬�루ERROR_SUCESS/ ʧ�ܣ�
	* ��  ע:
	***********************************************************************************************************************/
	JC_GAMMA_DLL_API int gammaOneStepCalc(const char* pID, const GammaInputInfoReg* pInput, GammaOutputInfoReg* pOutput);
	
	/**********************************************************************************************************************
	* ��  �ܣ�gamma�����㷨�� gamma���ں���
	* ��  ���� in��
	*                   pID 			-- �߳�ID
	*					maxCountLimit   -- �����ڴ���
	*                   pInput          -- �������		
	*         out:
	*                   pOutput         -- �������
	*
	* ����ֵ��״̬�루ERROR_SUCESS/ ʧ�ܣ�
	* ��  ע:
	***********************************************************************************************************************/
	JC_GAMMA_DLL_API int gammaOneBindCalc(const char* pID, const int maxCountLimit, const GammaInputInfoReg* pInput, GammaOutputInfoReg* pOutput);

	/**********************************************************************************************************************
	* ��  �ܣ�gamma�����㷨�� �������ú���
	* ��  ���� in��
	*                   handle           - �㷨����ָ�룬��ͬ�߳̾��в�ͬhandle
	*                   strFile          - �����ļ�·��
	*         out:
	*
	* ����ֵ��״̬�루ERROR_SUCESS/ ʧ�ܣ�
	* ��  ע:
	***********************************************************************************************************************/
	JC_GAMMA_DLL_API int gammaSetPara(const char* pID, const char* pData, int nLen);

	/**********************************************************************************************************************
	* ��  �ܣ�gamma�����㷨�� ��ȡ�汾��Ϣ
	* ��  ���� in��
	*
	*         out:
	*                   strVersion       - �汾��Ϣ
	*
	* ����ֵ��״̬�루ERROR_SUCESS/ ʧ�ܣ�
	* ��  ע:
	***********************************************************************************************************************/
	JC_GAMMA_DLL_API char* getAlgVesion();

	/**********************************************************************************************************************
	* ��  �ܣ�gamma�����㷨�� ��ӡ�ص�����
	* ��  ���� in��
	*                   format           - ��ӡ��ʽ
	*         out:
	*
	* ����ֵ��״̬�루ERROR_SUCESS/ ʧ�ܣ�
	* ��  ע:
	***********************************************************************************************************************/
	typedef int (*pGammaPrintMsg)(const char* pID, char* pMsg, ...);
	typedef int (*pCreateLogFile)(const char* fileName, int len);
	JC_GAMMA_DLL_API int gammaRegisterFunGammaPrintMsg(pGammaPrintMsg funPtr);
	JC_GAMMA_DLL_API int gammaRegisterFunCreateLogFile(pCreateLogFile funPtr);

} //namespace GammaAlg end
