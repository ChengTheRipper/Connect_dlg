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

//�û��̳����Ը����¼�������

class CSampleFeatureEventHandler : public IFeatureEventHandler
{
public:
	void DoOnFeatureEvent(const GxIAPICPP::gxstring& strFeatureName, void* pUserParam)
	{
		cout << "�յ��ع�����¼�!" << endl;
	}
};

//�û��̳вɼ��¼�������
class CSampleCaptureEventHandler : public ICaptureEventHandler
{
public:
	inline static cv::Mat& src()//ԭʼͼ��
	{
		static cv::Mat src;
		return src;
	}
	void DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam);
	
};
class SingleCam;

class CamCon //Cam����
{
public:
	CamCon() { CamInit(); };
	~CamCon() { CamUnInit(); };
	int init_all_cam(vector<shared_ptr<SingleCam>>& cam_vec);
public:
	
private:
	//��ֹ����
	CamCon(const CamCon&);
	CamCon operator=(const CamCon&);
	//��ʼ������
	int CamInit();
	int CamUnInit();
	gxdeviceinfo_vector vectorDeviceInfo;//�������߰����������Ϣ
	Mat img;
	
};
class SingleCam//�������
{
public:

	SingleCam(CGXDevicePointer m_ODP , CGXStreamPointer m_OSP, cv::Rect roi = cv::Rect(PC_WIDTH / 3, 0, PC_WIDTH / 3, PC_HEIGHT) );
	~SingleCam();
public:
	void Record_start();
	void Record_stop();
	cv::Rect ROI;
private:
	//��ֹ�������,��취�ƹ�ȥ������������ʹ��vec����
	SingleCam(const SingleCam&);
	const SingleCam& operator=(const SingleCam&);
	//
	IDeviceOfflineEventHandler *pDeviceOfflineEventHandler;///<�����¼��ص�����
	IFeatureEventHandler       *pFeatureEventHandler;///<Զ���豸�¼��ص�����
	ICaptureEventHandler       *pCaptureEventHandler;///<�ɼ��ص�����
	CGXFeatureControlPointer   ObjFeatureControlPtr;//�豸���Կ�����

	CGXDevicePointer ObjDevicePtr;//�豸ָ��
	CGXStreamPointer ObjStreamPtr;//����

	
};



//֮��˫�����ĵĴ���
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

