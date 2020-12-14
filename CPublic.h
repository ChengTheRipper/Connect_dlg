#pragma once
#include "opencv_include.h"
#include "STL_include.h"


//���������������
enum
{
	PC_WIDTH = 5496,
	PC_HEIGHT = 3672,
	AS_RATIO = 10,
};

struct CPublic
{
		
	//����һ�������Mat��vector������
	//��һ���������ȫû���Ź���ԭͼ
	//�ڶ���λ�ô�������Ź���ԭͼ
	//��������Ų��к��ԭͼ
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
	
	
	//��������ʾ��������
	static constexpr char* MATWINDOW_NAME()
	{
		return "view";
	}
	
	//�������1��ROI��ָ����
	static cv::Rect*& ROI_1()
	{
		static cv::Rect* roi_1 = NULL;
		return roi_1;
	}

	//�����ã�ģ�����1��ROI,ֱ�ӷ���һ��Rect����
	static cv::Rect& ROI1Temp()
	{
		static cv::Rect ro1(PC_WIDTH / 3, 0, PC_WIDTH / 3, PC_HEIGHT);
		return ro1;
	}

	//�������2��ROI����ʱ�Ȳ�����
	static cv::Rect* ROI_2();

	/*static bool& Selection_enabled()
	{
		static bool flag = false;
		return flag;
	}*/
	static bool Selection_enabled;


	
};

