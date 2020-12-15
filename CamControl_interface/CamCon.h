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
	//��ʼ������
	int CamInit();
	int CamUnInit();
public:
	
private:
	//��ֹ����
	CamCon(const CamCon&);
	CamCon operator=(const CamCon&);
	
	gxdeviceinfo_vector vectorDeviceInfo;//�������߰����������Ϣ
	Mat img;
	
};
class SingleCam//�������
{
public:

	SingleCam(CGXDevicePointer m_ODP , CGXStreamPointer m_OSP, gxstring m_serialNum,cv::Rect roi = cv::Rect(PC_WIDTH / 3, 0, PC_WIDTH / 3, PC_HEIGHT) );
	~SingleCam();
public:
	void Record_start();
	void Record_stop();
	cv::Rect ROI;

	inline bool& IsOffline()
	{
		return m_bIsOffline;
	}

private:
	//��ֹ�������
	SingleCam(const SingleCam&);
	const SingleCam& operator=(const SingleCam&);
	//���ߴ���
	void __ProcessOffline();
	//��������
	void __Recovery();
	//����Handler
	IDeviceOfflineEventHandler *pDeviceOfflineEventHandler;///<�����¼��ص�����
	IFeatureEventHandler       *pFeatureEventHandler;///<Զ���豸�¼��ص�����
	ICaptureEventHandler       *pCaptureEventHandler;///<�ɼ��ص�����
	CGXFeatureControlPointer   ObjFeatureControlPtr;//�豸���Կ�����

	CGXDevicePointer ObjDevicePtr;//�豸ָ��
	CGXStreamPointer ObjStreamPtr;//����

	GX_DEVICE_OFFLINE_CALLBACK_HANDLE hDeviceOffline; //���߻ص����

	//��־λ����ʾ�豸�Ƿ�����
	bool m_bIsOffline;
	bool m_bIsSnap;
	bool m_bIsOpened;

	//���к�
	gxstring serialNum;
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



