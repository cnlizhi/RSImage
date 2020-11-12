//CRSImage.h

#pragma once
#ifndef _CRSIMAGE_H
#define _CRSIMAGE_H
#include "CImgProcess.h"
#include <string>
using namespace std;

typedef unsigned char DN;
enum INTERLEAVE { BSQ, BIP, BIL };

class CRSImage
{
public: //公有成员函数
	CRSImage(); //构造函数
	CRSImage(const CRSImage& rImage); //复制构造函数
	~CRSImage(); //析构函数
	bool OpenImage(const char* c_path); //输入图像文件路径，即可读入文件数据
	bool Information(); //输出当前图像的路径，行列值、波段数、数据类型、排列方式等
	bool CloseImage(); //关闭当前图像
	bool Statistics(); //输出图像数据统计量，文件未打开，输出提示 
	bool Histogram(); //输出图像的直方图 
	const void Help() const; //输出帮助信息
	bool SaveAsFile(const char* c_path); //输入保存的文件路径，输出图像为二进制文件
	bool DisplayImage(); //控制台显示图像
	bool InitImage(); //初始化图像
	bool RotateImage(double angle); //图像旋转，输入旋转角度，逆时针旋转图像 
	bool Zoom(int enlarge, int decrease); //图像缩放，输入缩放比例尺，输出缩放图像 
	bool Filter(char core); //输入滤波核，执行滤波，输出滤波后图像
protected: //保护成员函数
	bool ReadMetaData(const char* c_path); //OpenImage函数读取元数据 
	bool InitBuffer(); //OpenImage函数初始化缓冲 
	bool ReadImgData(const char* c_path); //OpenImage函数读取文件  
	const double GetAverage(DN** band) const; //Statistics函数求均值
	const double GetVariance(DN** band) const; //Statistics函数求方差
	const int GetMaximum(DN** band) const; //Statistics函数求最大值
	const int GetMinimum(DN** band) const; //Statisitcs函数求最小值
private: //私有数据成员
	CImgProcess ImgProcess; //数字图像处理
	bool m_bOpen; //判断图像打开
	DN*** m_pppDN; //三维数组
	int m_samples; //列 
	int m_lines; //行 
	int m_bands; //波段
	INTERLEAVE m_interleave; //排列方式
	short m_datatype; //数据类型
};

#endif //_CREIMAGE_H