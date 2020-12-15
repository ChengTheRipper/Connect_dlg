#pragma once
#include "opencv_include.h"
#include "STL_include.h"
#include <bitset>

//相机参数，放缩比
enum
{
	PC_WIDTH = 5496,
	PC_HEIGHT = 3672,
	AS_RATIO = 10,
};

struct CPublic
{
		
	//返回一个存放着Mat的vector的引用
	//第一个存放着完全没缩放过的原图
	//第二个位置存放着缩放过的原图
	//第三个存放裁切后的原图
	enum
	{
		ORIGINAL,
		RESIZED,
		CROPPED_ORIGINAL,
		CROPPED_RESIZED,
	};
	static std::vector<cv::Mat>& Mat_Vec()
	{
		static std::vector<cv::Mat> Mat_Vec(4);
		return Mat_Vec;
	}
	
	
	//返回主显示窗口名称
	static constexpr char* MATWINDOW_NAME()
	{
		return "view";
	}
	
	//返回相机1的ROI的指针句柄
	static cv::Rect*& ROI_1()
	{
		static cv::Rect* roi_1 = NULL;
		return roi_1;
	}

	//测试用，模拟相机1的ROI,直接返回一个Rect引用
	static cv::Rect& ROI1Temp()
	{
		static cv::Rect ro1(PC_WIDTH / 3, 0, PC_WIDTH / 3, PC_HEIGHT);
		return ro1;
	}

	static std::vector<bool>&  allRuntimeFlag()
	{
		static std::vector<bool> b;
		return b;
	}

	//返回相机2的ROI，暂时先不用做
	static cv::Rect* ROI_2();

	/* static bool& Selection_enabled()
	{
		static bool flag = false;
		return flag;
	}
	*/
	static bool Selection_enabled;


	

	
	
};

