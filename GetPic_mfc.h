#pragma once
//初始化一个PicCon的大小
//在这里初始化大多数的MFC控件
#include "framework.h"
#include "opencv_include.h"
#include "connect_dlgDlg.h"
#include "CPublic.h"
#include "CamControl_interface/CamCon.h"
#include "resource.h"
#include "connect_dlgDlg.h"



//初始化Picture Control 控件
void PicConInit(HWND hWnd, int IDD);
//picture control 控件的 鼠标回调函数，用于获得裁剪后的空间
void On_mouse(int event, int x, int y, int flags, void* k);



//初始化所有的Scroll Bar控件
void ScrollBarInit(HWND hWnd);