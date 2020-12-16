#include "CamCon.h"



int CamCon::init_all_cam(vector<shared_ptr<SingleCam>>& cam_vec)
{
	try
	{
		if (!cam_vec.empty())
			return -1;
		int i = 0;
		const size_t totalDeviceNum = vectorDeviceInfo.size();
		CString a;
		a.Format(_T("%d ���豸"), totalDeviceNum);
		AfxMessageBox(a);
		while (i < totalDeviceNum)
		{

			AfxMessageBox(CString(vectorDeviceInfo[i].GetVendorName()));
			AfxMessageBox(CString(vectorDeviceInfo[i].GetSN()));
			CGXDevicePointer ObjDevicePtr = IGXFactory::GetInstance().OpenDeviceBySN(
				vectorDeviceInfo[i++].GetSN(),
				GX_ACCESS_EXCLUSIVE);
			CGXStreamPointer ObjStreamPtr = ObjDevicePtr->OpenStream(0);
			shared_ptr<SingleCam> p(new SingleCam(ObjDevicePtr, ObjStreamPtr));
			cam_vec.push_back(p);
		}
	}
	
	
	catch (CGalaxyException& e)
	{
		cout << "������: " << e.GetErrorCode() << endl;
		cout << "����������Ϣ: " << e.what() << endl;
		AfxMessageBox(CString(e.what()));
	}
	catch (std::exception& e)
	{
		cout << "����������Ϣ: " << e.what() << endl;
		AfxMessageBox(CString(e.what()));
	}

	return 0;
}

int CamCon::CamInit()
{
	//��ʼ��
	IGXFactory::GetInstance().Init();
	//ö���豸
	IGXFactory::GetInstance().UpdateDeviceList(1000, vectorDeviceInfo);
	if (0 == vectorDeviceInfo.size())
	{
		//cout << "�޿����豸!" << endl;
		CString C(L"�޿����豸!");
		AfxMessageBox(C);
		return -1;
	}
			
}

int CamCon::CamUnInit()
{
	IGXFactory::GetInstance().Uninit();
	return 0;
}

SingleCam::SingleCam(CGXDevicePointer m_ODP, CGXStreamPointer m_OSP, cv::Rect roi ) : ObjDevicePtr(m_ODP), ObjStreamPtr(m_OSP), ROI(roi)
{
	//�豸����
	GX_DEVICE_OFFLINE_CALLBACK_HANDLE hDeviceOffline = NULL;
	pDeviceOfflineEventHandler = new CSampleDeviceOfflineEventHandler();
	hDeviceOffline = ObjDevicePtr->RegisterDeviceOfflineCallback(pDeviceOfflineEventHandler, NULL);
	//��ȡԶ���豸���Կ�����
	ObjFeatureControlPtr = ObjDevicePtr->GetRemoteFeatureControl();
	//ע��ص��ɼ�
	pCaptureEventHandler = new CSampleCaptureEventHandler();
	ObjStreamPtr->RegisterCaptureCallback(pCaptureEventHandler, &ROI);
	//ObjStreamPtr->RegisterCaptureCallback(pCaptureEventHandler, NULL);
	
	
}
SingleCam::~SingleCam()
{
	ObjStreamPtr->UnregisterCaptureCallback();
	ObjStreamPtr->Close();
	ObjDevicePtr->Close();
	//�����¼��ص�ָ��
	if (NULL != pCaptureEventHandler)
	{
		delete pCaptureEventHandler;
		pCaptureEventHandler = NULL;
	}
	if (NULL != pDeviceOfflineEventHandler)
	{
		delete pDeviceOfflineEventHandler;
		pDeviceOfflineEventHandler = NULL;
	}
	if (NULL != pFeatureEventHandler)
	{
		delete pFeatureEventHandler;
		pFeatureEventHandler = NULL;
	}
}

void SingleCam::Record_start()
{
	try
	{
		ObjStreamPtr->StartGrab();
		ObjFeatureControlPtr->GetCommandFeature("AcquisitionStart")->Execute();
	}
	catch (CGalaxyException& e)
	{
		cout << "������: " << e.GetErrorCode() << endl;
		cout << "����������Ϣ: " << e.what() << endl;
		AfxMessageBox(CString(e.what()));
	}
	catch (std::exception& e)
	{
		cout << "����������Ϣ: " << e.what() << endl;
	}

}
void SingleCam::Record_stop()
{
	
	ObjFeatureControlPtr->GetCommandFeature("AcquisitionStop")->Execute();
	ObjStreamPtr->StopGrab();

	//ֹͣ�ɼ������Mat_Vec
	Mat src = CSampleCaptureEventHandler::src().clone(), resized;
	resize(src, resized, Size(PC_WIDTH / AS_RATIO, PC_HEIGHT / AS_RATIO));
	CPublic::Mat_Vec()[CPublic::ORIGINAL] = src;
	CPublic::Mat_Vec()[CPublic::RESIZED] = resized;
	CPublic::Mat_Vec()[CPublic::CROPPED_ORIGINAL] = src(ROI);
	cv::Rect roiResized(ROI.x / AS_RATIO, ROI.y / AS_RATIO, ROI.width / AS_RATIO, ROI.height / AS_RATIO);
	CPublic::Mat_Vec()[CPublic::CROPPED_RESIZED] = resized(roiResized);
	//imshow("resized", CPublic::Mat_Vec()[CPublic::CROPPED_ORIGINAL]);
	imwrite("test_pic.bmp", CPublic::Mat_Vec()[CPublic::CROPPED_ORIGINAL]);

}

int StartCam(std::shared_ptr<SingleCam> p)
{
	/*if (SCV().empty())
	{
		AfxMessageBox(CString(" StartCam01 scv empty!"));
		return -1;
	}
	auto s1 = SCV().back();*/

	//��ROI_1��ָ��ָ������������ROI
	CPublic::ROI_1() = &(p->ROI);
	p->Record_start();
	return 0;
}

int StopCam(std::shared_ptr<SingleCam> p)
{
	/*if (SCV().empty())
	{
		AfxMessageBox(CString("  StopCam01 scv empty!"));
		return -1;
	}*/
	//auto s1 = SCV().back();

	p->Record_stop();
	return 0;
}

void NormalROI(Rect& input)
{
	if (input.x < 0)
		input.x = 0;
	else if (input.x > PC_WIDTH)
		input.x = PC_WIDTH;
	else if (input.y < 0)
		input.y = 0;
	else if (input.y > PC_HEIGHT)
		input.y = PC_HEIGHT;
}

void CSampleDeviceOfflineEventHandler::DoOnDeviceOfflineEvent(void* pUserParam)
{
	cout << "�յ��豸�����¼�!" << endl;

}

void CSampleCaptureEventHandler::DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam)
{
	/*Mat img;*/
	src().create(objImageDataPointer->GetHeight(), objImageDataPointer->GetWidth(), CV_8UC3);
	void* pRGB24Buffer = NULL;
	//����ԭʼ������BayerRG8ͼ��
	pRGB24Buffer = objImageDataPointer->ConvertToRGB24(GX_BIT_0_7, GX_RAW2RGB_NEIGHBOUR, true);
	memcpy(src().data, pRGB24Buffer, (objImageDataPointer->GetHeight()) * (objImageDataPointer->GetWidth()) * 3);
	cv::flip(src(), src(), ROTATE_90_CLOCKWISE);

	Mat img;
	resize(src(), img, Size(src().cols / AS_RATIO, src().rows / AS_RATIO));

	cv::Rect* roi = (cv::Rect*)(pUserParam);

	cv::Rect roi_resized(roi->x / 10, roi->y / 10, roi->width / 10, roi->height / 10);

	rectangle(img, roi_resized, Scalar(0, 0, 255), 8);
	cv::imshow(CPublic::MATWINDOW_NAME(), img);
	cv::waitKey(1);
	//cout << "֡����" << objImageDataPointer->GetFrameID() << endl;
}
