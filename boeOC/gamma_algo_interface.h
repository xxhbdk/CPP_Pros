
#pragma once

#ifdef  JC_GAMMA_DLL_EXPORTS
#define JC_GAMMA_DLL_API  __declspec(dllexport)
#else
#define JC_GAMMA_DLL_API  __declspec(dllimport)
#endif


#include <string>

namespace GammaAlgo
{

#pragma region =============================================gamma算法宏定义======================================================

#define GAMMA_ERROR_SUCCESS					        0
#define GAMMA_FAILED							    -1

#define GAMMA_ERROR_BASE							(10000)
#define GAMMA_ERROR(idx)							((GAMMA_ERROR_BASE) + idx)

#define GAMMA_ERROR_INVALID_PARAM					GAMMA_ERROR(1)			// 无效参数 命令格式不正确(如缺少字段,字段值不合法等)
#define GAMMA_ERROR_PARAM_PTR_NULL				    GAMMA_ERROR(2)			// 输入的指针为空
#define GAMMA_ERROR_NOT_IMPL					    GAMMA_ERROR(3)			// 当前的方法没有实现
#define GAMMA_ERROR_PARAM_PATH                      GAMMA_ERROR(4)          // 输入的路径异常
#define GAMMA_ERROR_MEMERY_ALLOC                    GAMMA_ERROR(5)          // 内存申请失败
#define GAMMA_ERROR_PI_NOT_INIT                     GAMMA_ERROR(6)          // 算法未初始化成功
#define GAMMA_ERROR_OPEN_FILE                       GAMMA_ERROR(7)          // 文件打开失败


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


#pragma region =============================================gamma算法数据结构======================================================


	/*	寄存器相关数据	*/
	typedef struct _RGBRegValue_
	{
		int rRegValue_;
		int gRegValue_;
		int bRegValue_;
	}RGBRegValue;

	typedef struct _RGBVolValue_
	{
		float rVolValue_;				//寄存器电压值
		float gVolValue_;			//寄存器电压值最大值
		float bVolValue_;			//寄存器电压值最小值	
	}RGBVolValue;


	typedef struct _CielxylvInfo_
	{
		float x_;
		float y_;
		float lv_;
	}CIExylvInfo;

	typedef struct _GammaInputInfoReg_
	{
		int count_;                 // 该绑点第几次调节
		int gray_;
		int band_;
		int grayindex_;              // 灰阶序号
		int algMode_;               // 算法类型	
		RGBRegValue regValue_;
		RGBVolValue volValue_;
		CIExylvInfo realLvxy_;     // target lv x y	
	}GammaInputInfoReg;


	typedef struct _GammaOutputInfoReg_
	{
		RGBRegValue regValue_;
		RGBVolValue volValue_;
	}GammaOutputInfoReg;

	typedef struct _RegConfig_
	{
		int regValue_;
		//int regBeginValue_;				//寄存器值	
		int regMaxValue_;			//寄存器最大值
		int regMinValue_;			//寄存器最小值
	}RegConfig;


	typedef struct _VolConfig_
	{
		float volValue_;
		//float volBeginValue_
		float volMaxValue_;
		float volMinValue_;
	}VolConfig;

	typedef struct _RegOneUnitConfig_
	{
		RegConfig rRegConfig_;
		RegConfig gRegConfig_;
		RegConfig bRegConfig_;

	}RegOneUnitConfig;


	typedef struct _VolOneUnitConfig_
	{
		VolConfig rVolConfig_;
		VolConfig gVolConfig_;
		VolConfig bVolConfig_;
	}VolOneUnitConfig;


	typedef struct _GammaErrorConfig_
	{
		CIExylvInfo targetxylv_;
		float xLeftError_;
		float xRightError_;
		float yLeftError_;
		float yRightError_;
		float lvLeftError_;
		float lvRightError_;
	}GammaErrorConfig;

	typedef struct _GammaOneUnitData_
	{
		int gray_;
		int bandno_;
		int grayindex_;
		GammaErrorConfig errorConfig_;
		VolOneUnitConfig volConfig_;
		RegOneUnitConfig regConfig_;

	}GammaOneUnitData;


	typedef struct _GammaInitData_
	{
		int maxbindnum_;
		int maxRegStep_;
		float maxVolStep_;
		GammaOneUnitData* pGammaAllData_;
	}GammaInitData;


#pragma endregion


	/**********************************************************************************************************************
	* 功  能：gamma调节算法库 算法初始化函数
	* 参  数： in：
	*
	*         in  输入
	*                  输入 char *pID
	* 返回值：状态码（ERROR_SUCESS/ 失败）
	* 备  注:
	***********************************************************************************************************************/
	JC_GAMMA_DLL_API int gammaInit(const char* pID, const GammaInitData* pGammaData);

	/**********************************************************************************************************************
		* 功  能：gamma调节算法库 算法反初始化函数
		* 参  数： in：
		*                   handle           - 算法处理指针，不同线程具有不同handle
		*         out:
		*
		* 返回值：状态码（ERROR_SUCESS/ 失败）
		* 备  注:
		***********************************************************************************************************************/
	JC_GAMMA_DLL_API int gammaUnInit(const char* pID);

	/**********************************************************************************************************************
	* 功  能：gamma调节算法库 gamma调节函数
	* 参  数： in：
	*                   handle           - 算法处理指针，不同线程具有不同handle
	*                   pInput           - 输入数据
						nAlgMode         - 算法类型选择
						nCount           - 第几次循环
	*         out:
	*                   pOutput          - 输出数据
	*
	* 返回值：状态码（ERROR_SUCESS/ 失败）
	* 备  注:
	***********************************************************************************************************************/
	JC_GAMMA_DLL_API int gammaProgress(const char* pID, const GammaInputInfoReg* pInput, GammaOutputInfoReg* pOutput);



	/**********************************************************************************************************************
	* 功  能：gamma调节算法库 参数设置函数
	* 参  数： in：
	*                   handle           - 算法处理指针，不同线程具有不同handle
	*                   strFile          - 参数文件路径
	*         out:
	*
	* 返回值：状态码（ERROR_SUCESS/ 失败）
	* 备  注:
	***********************************************************************************************************************/
	JC_GAMMA_DLL_API int gammaSetPara(const char* pID, const char* pData, int nLen);

	/**********************************************************************************************************************
	* 功  能：gamma调节算法库 获取版本信息
	* 参  数： in：
	*
	*         out:
	*                   strVersion       - 版本信息
	*
	* 返回值：状态码（ERROR_SUCESS/ 失败）
	* 备  注:
	***********************************************************************************************************************/
	JC_GAMMA_DLL_API char* getAlgVesion();
	/**********************************************************************************************************************
	* 功  能：gamma调节算法库 打印回调函数
	* 参  数： in：
	*                   format           - 打印格式
	*         out:
	*
	* 返回值：状态码（ERROR_SUCESS/ 失败）
	* 备  注:
	***********************************************************************************************************************/
	typedef int (*pGammaPrintMsg)(const char* pID, char* pMsg, ...);
	JC_GAMMA_DLL_API int gammaRegisterFunGammaPrintMsg(pGammaPrintMsg funPtr);


} //namespace GammaAlgo end
