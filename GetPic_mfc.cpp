#pragma once
#include "GetPic_mfc.h"

void PicConInit(HWND hWnd_dlg, int IDD)
{
	cv::namedWindow(CPublic::MATWINDOW_NAME(), cv::WINDOW_AUTOSIZE);
	HWND hWnd_pic = GetDlgItem(hWnd_dlg, IDD);

	//设定这玩意的硬性大小
	CRect rect;
	GetWindowRect(hWnd_pic, rect);
	MoveWindow(hWnd_pic, rect.left, rect.top, 
		PC_WIDTH / AS_RATIO, PC_HEIGHT / AS_RATIO, true);

	HWND hWnd = (HWND)cvGetWindowHandle(CPublic::MATWINDOW_NAME());
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, hWnd_pic);
	::ShowWindow(hParent, SW_HIDE);

	
	//ScreenToClient(&rect);
	//GetDlgItem(IDC_WAVE_DRAW)->MoveWindow(rect.left, rect.top, 800, 450, true);    //固定Picture Control控件的大小

}


void ROIRefresh(const cv::Rect& box, cv::Rect* input)
{
	//生成一个box放大
	cv::Rect bigbox(box.x * 10, box.y * 10, box.width * 10, box.height * 10);
	cv::Rect* roi = CPublic::ROI_1();
	if (roi != NULL)
	{
		*roi = bigbox;//更新当前相机ROI
	}
}


void On_mouse(int event, int x, int y, int flags, void* k)//1相机框绑定的处理函数
{
	static cv::Rect box = cv::Rect();
	static bool Is_drawing = false;
	if (CPublic::Selection_enabled == false)//没有使能马上退出
		return;

	//测试用代码
	//Mat src = CSampleCaptureEventHandler::src().clone(), resized;

	//
	switch (event)
	{
	default:
		break;
	case CV_EVENT_LBUTTONDOWN:
		Is_drawing = true;
		box = cv::Rect(x, y, 0, 0);
		
		break;
	case CV_EVENT_MOUSEMOVE:
		if (Is_drawing)
		{
			box.width  = x - box.x;
			box.height = y - box.y;

			//更新ROI
			ROIRefresh(box, CPublic::ROI_1());
			cv::Mat output = CPublic::Mat_Vec()[CPublic::RESIZED].clone();
			cv::rectangle(output, box, cv::Scalar(255, 0, 0));
			cv::imshow(CPublic::MATWINDOW_NAME(), output);
		}
		break;
	case CV_EVENT_LBUTTONUP:
		if (box.width < 0) {//排除宽为负的情况，在这里判断是为了优化计算，不用再移动时每次更新都要计算长宽的绝对值
			box.x = box.x + box.width;//更新原点位置，使之始终符合左上角为原点
			box.width = -1 * box.width;//宽度取正
		}
		if (box.height < 0) {//同上
			box.y = box.y + box.height;
			box.height = -1 * box.width;
		}

		//更新ROI
		ROIRefresh(box, CPublic::ROI_1());

		////测试用代码
		Mat src = CSampleCaptureEventHandler::src().clone(), resized;

		resize(src, resized, Size(PC_WIDTH / AS_RATIO, PC_HEIGHT / AS_RATIO));
		cv::Rect bigbox(box.x * AS_RATIO, box.y * AS_RATIO,
			box.width * AS_RATIO, box.height * AS_RATIO);
		CPublic::Mat_Vec()[CPublic::ORIGINAL] = src;
		CPublic::Mat_Vec()[CPublic::RESIZED] = resized;
		CPublic::Mat_Vec()[CPublic::CROPPED_ORIGINAL] = src(bigbox);
		CPublic::Mat_Vec()[CPublic::CROPPED_RESIZED] = resized(box);
		
		Is_drawing = false;
		CPublic::Selection_enabled = false;
		
		break;
	
	
	}
	
}

void ScrollBarInit(HWND hWnd)
{
	//获得所有数字显示框的句柄
	HWND hWndEditROI1Hori = GetDlgItem(hWnd, IDC_EDIT_SB1_HORI);
	HWND hWndEditROI1Vert = GetDlgItem(hWnd, IDC_EDIT_SB1_VERT);

	//获得所有滚动条的句柄
	HWND hWndScrollBarROI1Hori = GetDlgItem(hWnd, IDC_SB_ROI1_H);
	HWND hWndScrollBarROI1Vert = GetDlgItem(hWnd, IDC_SB_ROI1_V);

	//设置滚动条的滚动范围
	SetScrollRange(hWndScrollBarROI1Hori, SB_HORZ, 1, 100, true);
	SetScrollRange(hWndScrollBarROI1Vert, SB_HORZ, 1, 100, true);

	//设置滚动条刻度
	

	//设置水平滚动条的初始位置
	SetScrollPos(hWndScrollBarROI1Hori, SB_HORZ, 20, true);
	SetScrollPos(hWndScrollBarROI1Vert, SB_HORZ, 20, true);

	//在编辑框中显示数字
	SetDlgItemInt(hWnd, IDC_EDIT_SB1_HORI, 20, false);
	SetDlgItemInt(hWnd, IDC_EDIT_SB1_VERT, 20, false);

	return;
}
