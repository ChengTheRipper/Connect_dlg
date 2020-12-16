
// connect_dlgDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "connect_dlg.h"
#include "connect_dlgDlg.h"
#include "afxdialogex.h"

#include "CamControl_interface/CamCon.h"
#include "opencv_include.h"
#include "solution_opencv.h"


//#include <Gdiplus.h>
//#pragma comment(lib, "gdiplus.lib")
//using namespace Gdiplus;
//ULONG_PTR gdiplusToken; // 这个变量需要保存下来


#include "GetPic_mfc.h"

#include <string>
using namespace std;
using namespace cv;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CconnectdlgDlg 对话框



CconnectdlgDlg::CconnectdlgDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CONNECT_DLG_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CconnectdlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_SB_ROI1_H, m_slider_ROI1_width);
	DDX_Control(pDX, IDC_SB_ROI1_V, m_slider_ROI1_height);
}

BEGIN_MESSAGE_MAP(CconnectdlgDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN_PIC, &CconnectdlgDlg::OnBnClickedOpenPic)
	ON_BN_CLICKED(IDOK, &CconnectdlgDlg::OnBnClickedOk)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	
	
	ON_BN_CLICKED(IDC_BUTTON5, &CconnectdlgDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CconnectdlgDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON1, &CconnectdlgDlg::OnBnClickedButton1)
	ON_WM_HSCROLL()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SB_ROI1_H, &CconnectdlgDlg::OnNMCustomdrawSbRoi1H)
	ON_BN_CLICKED(IDC_BUTTON_ROI_UP, &CconnectdlgDlg::OnBnClickedButtonRoiUp)
	ON_BN_CLICKED(IDC_BUTTON_ROI_DOWN, &CconnectdlgDlg::OnBnClickedButtonRoiDown)
	ON_BN_CLICKED(IDC_BUTTON_ROI_LEFT, &CconnectdlgDlg::OnBnClickedButtonRoiLeft)
	ON_BN_CLICKED(IDC_BUTTON_ROI_RIGHT, &CconnectdlgDlg::OnBnClickedButtonRoiRight)
END_MESSAGE_MAP()


// CconnectdlgDlg 消息处理程序

BOOL CconnectdlgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//初始化相片容器，存放了相机原图，裁切后原图，缩放原图（显示用），缩放后的裁切原图，
	CPublic::Mat_Vec();
	//初始化图像显示端口
	PicConInit(this->m_hWnd, IDC_STATIC_CV);
	//设置鼠标回调函数
	cv::setMouseCallback(CPublic::MATWINDOW_NAME(), On_mouse, 0);
	//初始化滚动条
	//设置滚动条滚动范围
	m_slider_ROI1_width.SetRange(0, PC_WIDTH);
	//每十个单位画一个刻度
	m_slider_ROI1_width.SetTicFreq(50);
	//设置初始位置
	int Start = PC_WIDTH / 2;
	m_slider_ROI1_width.SetPos(Start);//滚动条初始位置
	SetDlgItemInt(IDC_EDIT_SB1_HORI, Start);//设置编辑框的初始值

   //滚动条2

	//设置滚动条滚动范围
	m_slider_ROI1_height.SetRange(0, PC_HEIGHT);
	//每十个单位画一个刻度
	m_slider_ROI1_height.SetTicFreq(50);
	//设置初始位置
	int Start2 = PC_HEIGHT / 2;
	m_slider_ROI1_height.SetPos(Start2);
	SetDlgItemInt(IDC_EDIT_SB1_VERT, Start2);//编辑框的初始值

	//初始化相机驱动，暂时先这么叫，主要注册公共函数
	CAMVEC();
	//初始化单个相机
	CAMVEC().init_all_cam(SCV());


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CconnectdlgDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CconnectdlgDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CconnectdlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CconnectdlgDlg::OnBnClickedOpenPic()
{
	CString picPath;   //定义图片路径变量
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,
		NULL, this);   //选择文件对话框
	if (dlg.DoModal() == IDOK)
	{
		picPath = dlg.GetPathName();  //获取图片路径
	}
	//CString to string  使用这个方法记得字符集选用“使用多字节字符”，不然会报错
	// Convert a TCHAR string to a LPCSTR
	CT2CA pszConvertedAnsiString(picPath);
	// construct a std::string using the LPCSTR input
	std::string strStd(pszConvertedAnsiString);
	string picpath(strStd);
	Mat image;//输入原图
	image = cv::imread(picpath);
	Mat imagedst;
	//以下操作获取图形控件尺寸并以此改变图片尺寸
	CRect rect;
	GetDlgItem(IDC_STATIC_CV)->GetClientRect(&rect);
	cv::Rect dst(rect.left, rect.top, rect.right, rect.bottom);
	resize(image, imagedst, cv::Size(rect.Width(), rect.Height()));
	//把这两幅图存放到公共容器中
	CPublic::Mat_Vec()[CPublic::ORIGINAL] = (image);
	CPublic::Mat_Vec()[CPublic::RESIZED]  = (imagedst);
	imshow(CPublic::MATWINDOW_NAME(), imagedst);
}


void CconnectdlgDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::OnCancel();
}


void CconnectdlgDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();

	//CRect pc_rect;
	//GetDlgItem(IDC_STATIC_CV)->GetClientRect(pc_rect);
	//int width = pc_rect.Width(), height = pc_rect.Height();
	//Gdiplus::Bitmap tmpBmp(width, height, PixelFormat32bppARGB);
	//// 创建绘制临时位图所需 Graphics 对象
	//Gdiplus::Graphics tmpG(&tmpBmp);

	//// 在此处编写绘图代码，绘制到 tmpG 对象
	//Gdiplus::Pen pen(Gdiplus::Color((ARGB)Gdiplus::Color::Black), 2);
	//Gdiplus::Point p1(0, 0);
	//Gdiplus::Point p2(30, 60);
	//tmpG.DrawLine(&pen, p1, p2);

	//// 创建窗口DC所需 Graphics 对象
	//HDC hdc = GetDC()->m_hDC;
	//Gdiplus::Graphics g(hdc);
	//// 将图片绘制到屏幕上
	//g.DrawImage(&tmpBmp, Gdiplus::Rect(0, 0, width, height),
	//	0, 0, width, height, Gdiplus::UnitPixel);
}


BOOL CconnectdlgDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	

	if (pMsg->message == WM_LBUTTONDOWN && GetDlgItem(IDC_STATIC_CV)->GetSafeHwnd() == pMsg->hwnd)
		OnLButtonDown(MK_LBUTTON, pMsg->pt);   //在此传递点击部位在对话框中的坐标

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CconnectdlgDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	

	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CconnectdlgDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	

	CDialogEx::OnLButtonDown(nFlags, point);
}



void CconnectdlgDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	if (SCV().empty())
	{
		AfxMessageBox(CString("scv empty!"));
		return;
	}
	StartCam(SCV()[CAM01]);
	/*auto s1 = SCV().back();
	CPublic::ROI_1() = &(s1->ROI);
	s1->Record_start();*/
}


void CconnectdlgDlg::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	//StopCam01();
	if (SCV().empty())
	{
		AfxMessageBox(CString("scv empty!"));
		return;
	}
	StopCam(SCV()[CAM01]);

	const Mat& input = CPublic::Mat_Vec()[CPublic::CROPPED_ORIGINAL];
	if (!input.empty())
	{
		solution_opencv s1(input);
		s1.auto_detect_default();
	}
	else
	{
		AfxMessageBox(CString(L"fuck"));
	}
}


void CconnectdlgDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString output;

	CPublic::Selection_enabled = true;//使能
	//关闭录制
	if (SCV().empty())
		return;
	auto s1 = SCV().back();
	s1->Record_stop();
	return;
}


void CconnectdlgDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (pScrollBar != NULL)
	{


		cv::Rect& ROI1 = CPublic::ROI1Temp();
		CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
		if (pSlider->GetDlgCtrlID() == IDC_SB_ROI1_H)
		{
			int ROI1_width = m_slider_ROI1_width.GetPos(); //获取roi1的宽度
			int temp = ROI1.x + ROI1_width;
			if (temp >= PC_WIDTH)
			{
				ROI1_width = PC_WIDTH - ROI1.x;
			}
			m_slider_ROI1_width.SetPos(ROI1_width);//滚动条位置
			SetDlgItemInt(IDC_EDIT_SB1_HORI, ROI1_width);
			ROI1.width = ROI1_width;

		}
		else if (pSlider->GetDlgCtrlID() == IDC_SB_ROI1_V)
		{
			int ROI1_height = m_slider_ROI1_height.GetPos(); //获取roi1的高度
			int temp = ROI1.y + ROI1_height;
			if (temp >= PC_HEIGHT)
			{
				ROI1_height = PC_HEIGHT - ROI1.y;
			}
			m_slider_ROI1_height.SetPos(ROI1_height);//滚动条位置
			SetDlgItemInt(IDC_EDIT_SB1_VERT, ROI1_height);
			ROI1.height = ROI1_height;

		}
		///temp
		/// 
		Mat src = CPublic::Mat_Vec()[CPublic::RESIZED].clone();
		if (src.empty())
		{
			CString a;
			MessageBox(CString("empty Mat"));
			return;
		}
		cv::Rect ROI1Resized(ROI1.x / 10, ROI1.y / 10, ROI1.width / 10, ROI1.height / 10);
		rectangle(src, ROI1Resized, Scalar(255, 0, 0));
		imshow(CPublic::MATWINDOW_NAME(), src);
		/// 
	}

	


	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CconnectdlgDlg::OnNMCustomdrawSbRoi1H(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


constexpr int roiSlice = 80;


void tmp_refresh()
{
	Mat src = CPublic::Mat_Vec()[CPublic::RESIZED].clone();
	if (src.empty())
	{
		return;
	}
	cv::Rect& ROI1 = CPublic::ROI1Temp();
	cv::Rect ROI1Resized(ROI1.x / 10, ROI1.y / 10, ROI1.width / 10, ROI1.height / 10);
	rectangle(src, ROI1Resized, Scalar(255, 0, 0));
	imshow(CPublic::MATWINDOW_NAME(), src);
}


void CconnectdlgDlg::OnBnClickedButtonRoiUp()
{
	// TODO: 在此添加控件通知处理程序代码
	cv::Rect& curROI = CPublic::ROI1Temp();

	int temp = curROI.y - roiSlice;
	
	if (temp <= 0)
		curROI.y = 0;
	else
		curROI.y = temp;
	

	tmp_refresh();
}


void CconnectdlgDlg::OnBnClickedButtonRoiDown()
{
	// TODO: 在此添加控件通知处理程序代码
	cv::Rect& curROI = CPublic::ROI1Temp();


	int temp = curROI.y + curROI.height + roiSlice;

	if (temp >= PC_HEIGHT)
		curROI.y = PC_HEIGHT - curROI.height;
	else
		curROI.y += roiSlice;
	
	tmp_refresh();
}




void CconnectdlgDlg::OnBnClickedButtonRoiLeft()
{
	// TODO: 在此添加控件通知处理程序代码
	cv::Rect& curROI = CPublic::ROI1Temp();

	int temp = curROI.x - roiSlice;

	if (temp <= 0)
		curROI.x = 0;
	else
		curROI.x = temp;
	tmp_refresh();
}



void CconnectdlgDlg::OnBnClickedButtonRoiRight()
{
	// TODO: 在此添加控件通知处理程序代码
	cv::Rect& curROI = CPublic::ROI1Temp();

	int temp = curROI.x + curROI.width + roiSlice;

	if (temp >= PC_WIDTH)
		curROI.x = PC_WIDTH - curROI.width;
	else
		curROI.x += roiSlice;

	tmp_refresh();
}
