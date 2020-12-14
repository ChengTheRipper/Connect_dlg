#pragma once

#include "..\pch.h"
#include "..\opencv_include.h"
#include "..\GetPic_mfc.h"
#include "..\CPublic.h"

using namespace std;
using namespace cv;

enum
{
	CAM01,
	CAM02,
};


class CSampleDeviceOfflineEventHandler : public IDeviceOfflineEventHandler
{
public:

	void DoOnDeviceOfflineEvent(void* pUserParam);
};

//用户继承属性更新事件处理类

class CSampleFeatureEventHandler : public IFeatureEventHandler
{
public:
	void DoOnFeatureEvent(const GxIAPICPP::gxstring& strFeatureName, void* pUserParam)
	{
		cout << "收到曝光结束事件!" << endl;
	}
};

//用户继承采集事件处理类
class CSampleCaptureEventHandler : public ICaptureEventHandler
{
public:
	inline static cv::Mat& src()//原始图像
	{
		static cv::Mat src;
		return src;
	}
	void DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam);
	
};
class SingleCam;

class CamCon //Cam总线
{
public:
	CamCon() { CamInit(); };
	~CamCon() { CamUnInit(); };
	int init_all_cam(vector<shared_ptr<SingleCam>>& cam_vec);
public:
	
private:
	//禁止复制
	CamCon(const CamCon&);
	CamCon operator=(const CamCon&);
	//初始化函数
	int CamInit();
	int CamUnInit();
	gxdeviceinfo_vector vectorDeviceInfo;//整个总线包含的相机信息
	Mat img;
	
};
class SingleCam//独立相机
{
public:

	SingleCam(CGXDevicePointer m_ODP , CGXStreamPointer m_OSP, cv::Rect roi = cv::Rect(PC_WIDTH / 3, 0, PC_WIDTH / 3, PC_HEIGHT) );
	~SingleCam();
public:
	void Record_start();
	void Record_stop();
	cv::Rect ROI;
private:
	//禁止复制相机,想办法绕过去，这里允许复制使得vec可行
	SingleCam(const SingleCam&);
	const SingleCam& operator=(const SingleCam&);
	//
	IDeviceOfflineEventHandler *pDeviceOfflineEventHandler;///<掉线事件回调对象
	IFeatureEventHandler       *pFeatureEventHandler;///<远端设备事件回调对象
	ICaptureEventHandler       *pCaptureEventHandler;///<采集回调对象
	CGXFeatureControlPointer   ObjFeatureControlPtr;//设备属性控制器

	CGXDevicePointer ObjDevicePtr;//设备指针
	CGXStreamPointer ObjStreamPtr;//打开流

	
};



//之后双相机会改的代码
int StartCam(std::shared_ptr<SingleCam> p);
int StopCam(std::shared_ptr<SingleCam> p);

static CamCon& CAMVEC()
{
	static CamCon c1;
	return c1;
}
static vector<shared_ptr<SingleCam>>& SCV()
{
	static vector<shared_ptr<SingleCam>> c1;
	return c1;
}

void NormalROI(Rect& input);

