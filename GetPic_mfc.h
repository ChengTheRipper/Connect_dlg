#pragma once
//��ʼ��һ��PicCon�Ĵ�С
//�������ʼ���������MFC�ؼ�
#include "framework.h"
#include "opencv_include.h"
#include "connect_dlgDlg.h"
#include "CPublic.h"
#include "CamControl_interface/CamCon.h"
#include "resource.h"
#include "connect_dlgDlg.h"



//��ʼ��Picture Control �ؼ�
void PicConInit(HWND hWnd, int IDD);
//picture control �ؼ��� ���ص����������ڻ�òü���Ŀռ�
void On_mouse(int event, int x, int y, int flags, void* k);



//��ʼ�����е�Scroll Bar�ؼ�
void ScrollBarInit(HWND hWnd);