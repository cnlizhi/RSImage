//RSImage
/*
编写一个遥感图像处理控制台程序，要求在控制台上输入字母菜单，执行相应的功能。要求用类对象，实现主体功能。其功能菜单如下：
序号 功能菜单          功能
00   X – Exit         退出程序
01   O – Open Image   输入图像文件路径，即可读入文件数据
02   I – Information  输出当前图像的路径，行列值、波段数、数据类型、排列方式等
03   C – Close Image  关闭当前图像
04   S – Statistics   输出图像数据统计量，文件未打开，输出提示
05   H – Histogram    输出图像的直方图
06   ? – Help         输出本信息
07   A – Save as File 输入保存的文件路径，输出图像为二进制文件
08   D -  Display      控制台显示图像
09   T -  Init Image   控制台显示图像
10   R – Rotate Image 图像旋转，输入旋转角度，逆时针旋转图像
11   Z – Zoom         图像缩放，输入缩放比例尺，输出缩放图像
12   F -  Filter       输入滤波核，执行滤波，输出滤波后图像
*/
//by 李智
//recode date 2018.5.24 ~ 2018.5.26
//date 2017.12.4 ~ 2017.12.11

#include "stdafx.h"
#include "CRSImage.h"
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

char CmdControl(string str); //控制输入合法
string GetPath(string path); //得到路径

int main()
{
	cout << "                        RSImage\n" << endl;
	CRSImage Image;
	Image.Help(); //帮助界面 
	bool bExit = false; //程序开关 
	string cmd; //命令符
	do
	{
		cout << "Please enter a letter:";
		cin >> cmd;
		switch (CmdControl(cmd)) //执行命令 
		{
		case 'X':
			cout << "Exit." << endl;
			bExit = true; //退出程序 
			Image.~CRSImage();
			break;
		case 'O':
		{
			string path; //打开文件路径
			cout << "Please enter path:";
			//cin >> path; //输入路径
			path = GetPath("test");
			cout << endl;
			if (!Image.OpenImage(path.c_str())) //输入图像文件路径，即可读入文件数据
			{
				cout << "Open image Failed!" << endl;
			}
			break;
		}
		case 'I':
			if (!Image.Information()) //输出当前图像的路径，行列值、波段数、数据类型、排列方式等
			{
				cerr << "Information Print Failed." << endl;
			}
			break;
		case 'C':
			if (!Image.CloseImage()) //关闭当前图像
			{
				cerr << "Close Image Failed." << endl;
			}
			break;
		case 'S':
			if (!Image.Statistics()) //输出图像数据统计量，文件未打开，输出提示
			{
				cerr << "Statistics Print Failed." << endl;
			}
			break;
		case 'H':
			if (!Image.Histogram()) //输出图像的直方图
			{
				cerr << "Histogram Print Failed." << endl;
			}
			break;
		case '?':
			Image.Help(); //输出帮助信息
			break;
		case 'A':
		{
			string Apath; //另存为文件路径
			cout << "Please enter document save as file path:";
			cin >> Apath; //输入路径 
			cout << endl;
			if (!Image.SaveAsFile(Apath.c_str())) //输入保存的文件路径，输出图像为二进制文件
			{
				cout << "Save as file failed!" << endl;
			}
			break;
		}
		case 'D':
			if (!Image.DisplayImage()) //控制台显示图像
			{
				cerr << "Display Image Failed." << endl;
			}
			break;
		case 'T':
			if (!Image.InitImage()) //初始化图像
			{
				cerr << "Init Image Failed." << endl;
			}
			break;
		case 'R':
			int angle;
			cout << "Please enter how many degrees you want to turn: (0~360)";
			cin >> angle;
			if (!Image.RotateImage(angle)) //图像旋转，输入旋转角度，逆时针旋转图像
			{
				cerr << "Rotate Image Failed." << endl;
			}
			break;
		case 'Z':
		{
			int enlarge, decrease = 0;
			string strZoom;
			char ctext;
			stringstream ss;
			cout << "Please enter measuring scale: (enlarge:decrease)" << endl;
			cin >> strZoom;
			if (strZoom[0] < 49 || strZoom[0] > 57)
			{
				cerr << "Enter Failed." << endl;
				break;
			}
			ss.clear();
			ss.str(strZoom);
			ss >> enlarge >> ctext >> decrease;
			if (!Image.Zoom(enlarge, decrease)) //图像缩放，输入缩放比例尺，输出缩放图像
			{
				cerr << "Zoom Image Failed." << endl;
			}
			break;
		}
		case 'F':
		{
			string core;
			cout << "序号 滤波种类\n"
				<< "01   M – Mean Filter       均值滤波\n"
				<< "02   G – Gauss Filter      高斯滤波\n"
				<< "03   S – Sharpness Filter  图像锐化滤波\n"
				<< "04   D – Edge Detection    边缘检测\n"
				<< "05   E – Embossing Filter  浮雕\n"
				<< "06   C – Custom Filter 自定义滤波核" << endl;
			cout << "Please enter filter core:";
			cin >> core;
			Image.Filter(CmdControl(core)); //输入滤波核，执行滤波，输出滤c波后图像
			break;
		}
		default:
			cerr << "Command Illegal." << endl;
			break;
		}
	} while (!bExit);
	return 0;
}

char CmdControl(string str)
{
	transform(str.begin(), str.end(), str.begin(), toupper);
	if (str.size() != 1)
	{
		return 'N';
	}
	return str[0];
}

string GetPath(string path) //得到路径
{
	TCHAR szPath[_MAX_PATH] = { 0 }; //存储当前程序路径
	GetModuleFileName(NULL, szPath, _MAX_PATH); //得到当前程序路径
	(_tcsrchr(szPath, _T('\\')))[1] = 0;
	int iLen = WideCharToMultiByte(CP_ACP, 0, szPath, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, szPath, -1, chRtn, iLen, NULL, NULL);
	string strPath(chRtn);
	delete chRtn;
	strPath = strPath.substr(0, strPath.find_last_of("\\")); //消除当前路径最后两个"\\"
	strPath = strPath.substr(0, strPath.find_last_of("\\")); //消除当前路径最后两个"\\"
	strPath.append("\\"); //在路径后增加"\\"
	strPath.append(path); //在路径后增加path
	strPath.append(".img"); //在路径后增加".img"
	return strPath;
}