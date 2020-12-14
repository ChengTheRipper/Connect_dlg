#pragma once
#include "GetPic_mfc.h"

void PicConInit(HWND hWnd_dlg, int IDD)
{
	cv::namedWindow(CPublic::MATWINDOW_NAME(), cv::WINDOW_AUTOSIZE);
	HWND hWnd_pic = GetDlgItem(hWnd_dlg, IDD);

	//�趨�������Ӳ�Դ�С
	CRect rect;
	GetWindowRect(hWnd_pic, rect);
	MoveWindow(hWnd_pic, rect.left, rect.top, 
		PC_WIDTH / AS_RATIO, PC_HEIGHT / AS_RATIO, true);

	HWND hWnd = (HWND)cvGetWindowHandle(CPublic::MATWINDOW_NAME());
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, hWnd_pic);
	::ShowWindow(hParent, SW_HIDE);

	
	//ScreenToClient(&rect);
	//GetDlgItem(IDC_WAVE_DRAW)->MoveWindow(rect.left, rect.top, 800, 450, true);    //�̶�Picture Control�ؼ��Ĵ�С

}


void ROIRefresh(const cv::Rect& box, cv::Rect* input)
{
	//����һ��box�Ŵ�
	cv::Rect bigbox(box.x * 10, box.y * 10, box.width * 10, box.height * 10);
	cv::Rect* roi = CPublic::ROI_1();
	if (roi != NULL)
	{
		*roi = bigbox;//���µ�ǰ���ROI
	}
}


void On_mouse(int event, int x, int y, int flags, void* k)//1�����󶨵Ĵ�����
{
	static cv::Rect box = cv::Rect();
	static bool Is_drawing = false;
	if (CPublic::Selection_enabled == false)//û��ʹ�������˳�
		return;

	//�����ô���
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

			//����ROI
			ROIRefresh(box, CPublic::ROI_1());
			cv::Mat output = CPublic::Mat_Vec()[CPublic::RESIZED].clone();
			cv::rectangle(output, box, cv::Scalar(255, 0, 0));
			cv::imshow(CPublic::MATWINDOW_NAME(), output);
		}
		break;
	case CV_EVENT_LBUTTONUP:
		if (box.width < 0) {//�ų���Ϊ����������������ж���Ϊ���Ż����㣬�������ƶ�ʱÿ�θ��¶�Ҫ���㳤��ľ���ֵ
			box.x = box.x + box.width;//����ԭ��λ�ã�ʹ֮ʼ�շ������Ͻ�Ϊԭ��
			box.width = -1 * box.width;//���ȡ��
		}
		if (box.height < 0) {//ͬ��
			box.y = box.y + box.height;
			box.height = -1 * box.width;
		}

		//����ROI
		ROIRefresh(box, CPublic::ROI_1());

		////�����ô���
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
	//�������������ʾ��ľ��
	HWND hWndEditROI1Hori = GetDlgItem(hWnd, IDC_EDIT_SB1_HORI);
	HWND hWndEditROI1Vert = GetDlgItem(hWnd, IDC_EDIT_SB1_VERT);

	//������й������ľ��
	HWND hWndScrollBarROI1Hori = GetDlgItem(hWnd, IDC_SB_ROI1_H);
	HWND hWndScrollBarROI1Vert = GetDlgItem(hWnd, IDC_SB_ROI1_V);

	//���ù������Ĺ�����Χ
	SetScrollRange(hWndScrollBarROI1Hori, SB_HORZ, 1, 100, true);
	SetScrollRange(hWndScrollBarROI1Vert, SB_HORZ, 1, 100, true);

	//���ù������̶�
	

	//����ˮƽ�������ĳ�ʼλ��
	SetScrollPos(hWndScrollBarROI1Hori, SB_HORZ, 20, true);
	SetScrollPos(hWndScrollBarROI1Vert, SB_HORZ, 20, true);

	//�ڱ༭������ʾ����
	SetDlgItemInt(hWnd, IDC_EDIT_SB1_HORI, 20, false);
	SetDlgItemInt(hWnd, IDC_EDIT_SB1_VERT, 20, false);

	return;
}
