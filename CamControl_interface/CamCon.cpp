#include "CamCon.h"



int CamCon::init_all_cam(vector<shared_ptr<SingleCam>>& cam_vec)
{
	bool bIsDeviceOpen = false;              ///< 设备是否已打开标识
	bool bIsStreamOpen = false;              ///< 设备流是否已打开标识
	try
	{
		if (!cam_vec.empty())
			return -1;
		int i = 0;
		const size_t totalDeviceNum = vectorDeviceInfo.size();
		CString a;
		a.Format(_T("%d 个设备"), totalDeviceNum);
		AfxMessageBox(a);

		if (totalDeviceNum == 0) // 完全搜不到设备
		{
			throw exception("No Device Detected");
		}
		//暂时先不对单相机做任何处理
		/*else if (totalDeviceNum == 1)
		{
			throw exception("Only one Cam Is Detected");
		}*/

		CGXDevicePointer ObjDevicePtr;
		CGXStreamPointer ObjStreamPtr;
		while (i < totalDeviceNum)
		{

			AfxMessageBox(CString(vectorDeviceInfo[i].GetVendorName()));
			AfxMessageBox(CString(vectorDeviceInfo[i].GetSN()));//通过Serial Number 进行开机，有好处，比MAC更加直观
			gxstring serialNum = vectorDeviceInfo[i].GetSN();
		
			ObjDevicePtr = IGXFactory::GetInstance().OpenDeviceBySN(
				vectorDeviceInfo[i++].GetSN(),
				GX_ACCESS_EXCLUSIVE);
			bIsDeviceOpen = true;

			ObjStreamPtr = ObjDevicePtr->OpenStream(0);

			shared_ptr<SingleCam> p(new SingleCam(ObjDevicePtr, ObjStreamPtr, serialNum));
			cam_vec.push_back(p);
		}
	}
	
	
	catch (CGalaxyException& e)
	{
		cout << "错误码: " << e.GetErrorCode() << endl;
		cout << "错误描述信息: " << e.what() << endl;
		AfxMessageBox(CString(e.what()));
		//相机初始化出现问题，提示是否选择是否进行相机重连
		//1.检查电缆
		//2.进行复位
		//如果cam_vec不为空，则直接清空数组进行执行关设备，关流
		if (!cam_vec.empty())
			cam_vec.clear();
	}
	catch (std::exception& e)
	{
		cout << "错误描述信息: " << e.what() << endl;
		
		//相机初始化出现问题，选择是否进行复位行动

		if (string(e.what()) == "No Device Detected")
		{
			//执行相应的措施，比如提供一个按钮再次执行init_all_cam
			AfxMessageBox(CString(e.what()));
		}
		//else if (string(e.what()) == "Only one Cam Is Detected")
		//{
		//	//执行相应的措施，比如提供一个按钮再次执行init_all_cam
		//	//或者允许单相机执行检测任务，这一部分先不做
		//}
		//如果cam_vec不为空，则直接清空数组进行执行关设备，关流
		if (!cam_vec.empty())
			cam_vec.clear();
	}

	return 0;
}

int CamCon::CamInit()
{
	//初始化
	IGXFactory::GetInstance().Init();
	//枚举设备
	IGXFactory::GetInstance().UpdateDeviceList(1000, vectorDeviceInfo);
	if (0 == vectorDeviceInfo.size())
	{
		//cout << "无可用设备!" << endl;
		CString C(L"无可用设备!");
		AfxMessageBox(C);
		return -1;
	}
			
}

int CamCon::CamUnInit()
{
	IGXFactory::GetInstance().Uninit();
	return 0;
}

SingleCam::SingleCam(CGXDevicePointer m_ODP, CGXStreamPointer m_OSP, gxstring m_serialNum, cv::Rect roi )
	: ObjDevicePtr(m_ODP), ObjStreamPtr(m_OSP), serialNum(m_serialNum),ROI(roi), m_bIsOffline(false), m_bIsSnap(false), m_bIsOpened(true)/*此时设备和流一定是打开的*/
{
	bool bIsDeviceOpen = true;              ///< 设备是否已打开标识
	bool bIsStreamOpen = true;              ///< 设备流是否已打开标识


	try 
	{
		//注册设备掉线处理函数
		//GX_DEVICE_OFFLINE_CALLBACK_HANDLE hDeviceOffline = NULL;
		pDeviceOfflineEventHandler = new CSampleDeviceOfflineEventHandler();
		hDeviceOffline = ObjDevicePtr->RegisterDeviceOfflineCallback(pDeviceOfflineEventHandler, this);
		//获取远端设备属性控制器
		ObjFeatureControlPtr = ObjDevicePtr->GetRemoteFeatureControl();
		//注册回调采集
		pCaptureEventHandler = new CSampleCaptureEventHandler();
		ObjStreamPtr->RegisterCaptureCallback(pCaptureEventHandler, this);
		//ObjStreamPtr->RegisterCaptureCallback(pCaptureEventHandler, NULL);
	}
	
	catch (CGalaxyException &e)
	{
		//判断设备流是否已打开
		if (bIsStreamOpen)
		{
			ObjStreamPtr->Close();
		}

		//判断设备是否已打开
		if (bIsDeviceOpen)
		{
			ObjDevicePtr->Close();
		}

		cout << "<" << e.GetErrorCode() << ">" << "<" << e.what() << ">" << endl;
		//提示处理函数注册失败，流和设备已经关闭
	}
	catch (std::exception &e)
	{
		//判断设备流是否已打开
		if (bIsStreamOpen)
		{
			ObjStreamPtr->Close();
		}

		//判断设备是否已打开
		if (bIsDeviceOpen)
		{
			ObjDevicePtr->Close();
		}

		cout << "<" << e.what() << ">" << endl;
		//提示处理注册失败，流和设备已经关闭

	}
	
}
SingleCam::~SingleCam()
{
	ObjStreamPtr->UnregisterCaptureCallback();
	ObjStreamPtr->Close();//关闭视频流
	ObjDevicePtr->Close();//关闭设备
	//销毁事件回调指针
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
		m_bIsSnap = true;//置位执行采集标志位
	}
	catch (CGalaxyException& e)
	{
		cout << "错误码: " << e.GetErrorCode() << endl;
		cout << "错误描述信息: " << e.what() << endl;
		AfxMessageBox(CString(e.what()));
	}
	catch (std::exception& e)
	{
		cout << "错误描述信息: " << e.what() << endl;
	}

}
void SingleCam::Record_stop()
{
	
	ObjFeatureControlPtr->GetCommandFeature("AcquisitionStop")->Execute();
	ObjStreamPtr->StopGrab();
	m_bIsSnap = false;	

	//停止采集，填充Mat_Vec
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

void SingleCam::__ProcessOffline()
{
	try
	{
		printf("**********************Process Offline**********************\r");
		//判断设备是否停止采集
		if (m_bIsSnap)
		{
			cout << "\n<Send stop snap command to device>" << endl;
			ObjFeatureControlPtr->GetCommandFeature("AcquisitionStop")->Execute();
		}
	}
	catch (CGalaxyException &e)
	{
		cout << "<" << e.GetErrorCode() << ">" << "<" << e.what() << ">" << endl;
	}
	catch (std::exception &e)
	{
		cout << "<" << e.what() << ">" << endl;
	}

	try
	{
		//判断设备是否停止采集
		if (m_bIsSnap)
		{
			ObjStreamPtr->StopGrab();
			m_bIsSnap = false;
		}
	}
	catch (CGalaxyException)
	{
		//do noting
	}
	catch (std::exception)
	{
		//do noting
	}

	try
	{
		//判断设备是否打开
		if (m_bIsOpened)
		{
		//注销掉线回调函数
			cout << "<Unregister device Offline callback>" << endl;
			ObjDevicePtr->UnregisterDeviceOfflineCallback(hDeviceOffline);

			//关闭流和设备
			cout << "<Close Device>" << endl;
			ObjStreamPtr->Close();
			ObjDevicePtr->Close();
			m_bIsOpened = false;

		}
	}
	catch (CGalaxyException)
	{
		//do noting
	}
	catch (std::exception)
	{
		//do noting
	}
}

int StartCam(std::shared_ptr<SingleCam> p)
{
	/*if (SCV().empty())
	{
		AfxMessageBox(CString(" StartCam01 scv empty!"));
		return -1;
	}
	auto s1 = SCV().back();*/

	//将ROI_1的指针指向了相机里面的ROI
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


//设备掉线事件处理器
void CSampleDeviceOfflineEventHandler::DoOnDeviceOfflineEvent(void* pUserParam)
{
	SingleCam* Cam = static_cast<SingleCam*>(pUserParam);
	Cam->IsOffline() = true;
}

void CSampleCaptureEventHandler::DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam)
{
	SingleCam* Cam = static_cast<SingleCam*>(pUserParam);
	if (Cam->IsOffline())
	{
		//掉线处理
		//重连
	}
	else
	{
		//继续采集
		src().create(objImageDataPointer->GetHeight(), objImageDataPointer->GetWidth(), CV_8UC3);
		void* pRGB24Buffer = NULL;
		//假设原始数据是BayerRG8图像
		pRGB24Buffer = objImageDataPointer->ConvertToRGB24(GX_BIT_0_7, GX_RAW2RGB_NEIGHBOUR, true);
		memcpy(src().data, pRGB24Buffer, (objImageDataPointer->GetHeight()) * (objImageDataPointer->GetWidth()) * 3);
		cv::flip(src(), src(), ROTATE_90_CLOCKWISE);

		Mat img;
		resize(src(), img, Size(src().cols / AS_RATIO, src().rows / AS_RATIO));

		cv::Rect& roi = Cam->ROI;//我们直接把类带进来，得劲，与其放一个成员指针，何不直接来整个类？

		cv::Rect roi_resized(roi.x / 10, roi.y / 10, roi.width / 10, roi.height / 10);

		rectangle(img, roi_resized, Scalar(0, 0, 255), 8);
		cv::imshow(CPublic::MATWINDOW_NAME(), img);
		cv::waitKey(1);
		//cout << "帧数：" << objImageDataPointer->GetFrameID() << endl;
	}

	
}
