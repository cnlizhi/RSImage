//CRSImage.cpp

#include "stdafx.h"
#include "CRSImage.h"
#include <iostream>
#include <fstream> 
#include <sstream>
#include <iomanip>
#include <cmath>
using namespace std;

CRSImage::CRSImage()
	: ImgProcess(), m_bOpen(false),
	m_pppDN(NULL), m_bands(0), m_lines(0), m_samples(0),
	m_interleave(BSQ), m_datatype(0)
{
}

CRSImage::CRSImage(const CRSImage& rImage)
	: ImgProcess(rImage.ImgProcess), m_bOpen(rImage.m_bOpen), m_pppDN(rImage.m_pppDN),
	m_bands(rImage.m_bands), m_lines(rImage.m_lines), m_samples(rImage.m_samples),
	m_interleave(rImage.m_interleave), m_datatype(rImage.m_datatype)
{
}

CRSImage::~CRSImage()
{
	if (m_bOpen)
	{
		try
		{
			if (!CloseImage())
			{
				throw 0;
			}
		}
		catch (int)
		{
			cerr << "Destruction Called Failed." << endl;
		}
	}
}

bool CRSImage::OpenImage(const char* c_path)
{
	/*在控制台输入字符O或o，程序提示输入图像文件路径，回车后，读取指定图像文件。
	返回值：要求返回错误码，0-为读取成功。
	程序流程：
	1）读取图像文件头，得到行、列、波段数、排列方式、数据类型等信息；
	2）动态分配图像数据内存，文件IO读取二进制流。*/
	if (m_bOpen)
	{
		cerr << "Image Already Open." << endl;
		return false;
	}
	if (c_path == NULL)
	{
		cerr << "Enter Path Failed." << endl;
		return false;
	}
	string strpath = c_path;
	int	pos = strpath.rfind('.');
	if (pos >= 0)
	{
		strpath = strpath.substr(0, pos);
	}
	strpath.append(".hdr");
	if (!ReadMetaData(strpath.c_str()))
	{
		cerr << "Read Meta Data Failed." << endl;
		return false;
	}
	if (!InitBuffer())
	{
		cerr << "Initialize Buffer Failed." << endl;
		return false;
	}
	if (!ReadImgData(c_path))
	{
		cerr << "Read Image Data Failed." << endl;
		return false;
	}
	if (!ImgProcess.Set(m_pppDN, m_bands, m_lines, m_samples))
	{
		cerr << "Set Image Process Failed." << endl;
		return false;
	}
	cout << "Image open." << endl;
	m_bOpen = true;
	return true;
}

bool CRSImage::Information()
{
	/*在控制台输入字符I或i，若图像已经打开，输出图像相关统计信息。
	返回值：错误则输出错误信息，成功则输出图像信息。
	程序流程：若图像文件打开，则输出图像路径行、列、波段数、排列方式、数据类型等信息到控制台。*/
	if (!m_bOpen)
	{
		cerr << "Image Not Open." << endl;
		return false;
	}
	cout << "samples：   " << m_samples << endl;
	cout << "lines：     " << m_lines << endl;
	cout << "bands：     " << m_bands << endl;
	cout << "interleave: ";
	if (m_interleave == BSQ)
	{
		cout << "BSQ" << endl;
	}
	else if (m_interleave == BIP)
	{
		cout << "BIP" << endl;
	}
	else if (m_interleave == BIL)
	{
		cout << "BIL" << endl;
	}
	else
	{
		cerr << "Interleave Type Failed." << endl;
		return false;
	}
	cout << "datatype:   " << m_datatype << endl; //输出数据类型
	return true;
}

bool CRSImage::CloseImage()
{
	/*功能描述：在控制台输入字符C或c，关闭当前打开的文件。
	返回值：错误则返回错误信息，正确，返回0。
	程序流程：释放图像数据存储空间。*/
	if (!m_bOpen)
	{
		cerr << "Image Not Open." << endl;
		return false;
	}
	delete[] m_pppDN;
	ImgProcess.ReSet();
	m_bOpen = false;
	cout << "Image Close." << endl;
	return true;
}

bool CRSImage::Statistics()
{
	/*功能描述：在控制台输入字符S或s，输出当前图像文件的统计信息；图像文件未打开，则返回提示信息。
	返回值：错误-错误信息/正确-返回success。
	程序流程：
	1）遍历图像数据；
	2）统计相关信息（均值、方差、最大值、最小值）*/
	if (!m_bOpen)
	{
		cerr << "Image Not Open." << endl;
		return false;
	}
	cout << "bands  Average  Variance  Maximum  Minimum" << endl;
	for (int band = 0; band < m_bands; band++)
	{
		cout << setw(5) << band + 1
			<< "   " << setw(6) << setprecision(5) << GetAverage(m_pppDN[band])
			<< "    " << setw(6) << setprecision(5) << GetVariance(m_pppDN[band])
			<< "   " << setw(6) << GetMaximum(m_pppDN[band])
			<< "   " << setw(6) << GetMinimum(m_pppDN[band]) << endl;
	}
	return true;
}

bool CRSImage::Histogram()
{
	/*功能描述：在控制台输入字符H或h，输出当前图像的直方图信息；图像文件未打开，则返回提示信息。
	返回值：错误-错误信息/正确-返回success。
	程序流程：
	1）遍历图像各个波段数据；
	2）统计直方图*/
	if (!m_bOpen)
	{
		cerr << "Image Not Open." << endl;
		return false;
	}
	cout << "Histogram" << endl;
	cout << "Every '*' means 1000, and the top of '*' less than 1000." << endl; //每个*代表1000
	for (int i = 0; i < m_bands; i++)
	{
		int get = 0; //用于给Array数组赋值
		int Buffer[_MAX_PATH]; //一维数组用于存储每个数值出现频率
		int x_max = GetMaximum(m_pppDN[i]); //直方图中x轴最大值
		int	y_max = 0; //直方图中y轴最大值
		for (int x = 0; x < 256; x++) //Buffer数组初始化
		{
			Buffer[x] = 0;
		}
		for (int j = 0; j < m_lines; j++) //得到Buffer数组值
		{
			for (int k = 0; k < m_samples; k++)
			{
				get = m_pppDN[i][j][k];
				Buffer[get]++;
			}
		}
		for (int x = 0; x < x_max; x++) //确定该波段中最高频率出现的值
		{
			y_max = (Buffer[x] > y_max) ? Buffer[x] : y_max;
		}
		cout << "Band " << i + 1 << endl;
		cout << "     |" << endl;
		for (int y = (y_max / 1000 * 1000 + 1000); y >= 0; y = y - 1000) //y轴以1000为单位
		{ //通过递减ymax的方式实现逐行输出
			cout << setw(5) << y << "|"; //输出y轴
			for (int x = 0; x < x_max; x++) //输出图像
			{
				if (Buffer[x] / (y + 1) == 0)
				{
					cout << " ";
				}
				else
				{
					cout << "*";
				}
			}
			cout << endl;
		}
		cout << "     |";
		for (int x = 0; x < x_max; x++) //输出x轴
		{
			cout << "_";
		}
		cout << endl;
		cout << "      0";
		for (int x = 0; x < x_max; x++) //输出x轴坐标
		{
			if ((x + 2) % 10 == 0) //每10个单位输出一个值
			{
				cout << setw(3) << x + 2;
			}
			else if (((x + 1) % 10 == 0) || (x % 10 == 0)) {} //控制输出值后面的位置不被覆盖
			else
			{
				cout << " ";
			}
		}
		cout << "\n" << endl;
	}
	return true;
}

const void CRSImage::Help() const
{
	/*功能描述：在控制台输入字符?，程序输出菜单信息。
	返回值：返回0。
	程序流程：显示菜单信息。*/
	cout << "在控制台上输入字母菜单，执行相应的功能。其功能菜单如下：" << endl;
	cout << "序号 功能菜单          功能\n"
		<< "00   X – Exit         退出程序\n"
		<< "01   O – Open Image   输入图像文件路径，即可读入文件数据\n"
		<< "02   I – Information  输出当前图像的路径，行列值、波段数、数据类型、排列方式等\n"
		<< "03   C – Close Image  关闭当前图像\n"
		<< "04   S – Statistics   输出图像数据统计量，文件未打开，输出提示\n"
		<< "05   H – Histogram    输出图像的直方图\n"
		<< "06   ? – Help         输出本信息\n"
		<< "07   A – Save as File 输入保存的文件路径，输出图像为二进制文件\n"
		<< "08   D -  Display      控制台显示图像\n"
		<< "09   T -  Init Image   初始化图像\n"
		<< "10   R – Rotate Image 图像旋转，输入旋转角度，逆时针旋转图像\n"
		<< "11   Z – Zoom         图像缩放，输入缩放比例尺，输出缩放图像\n"
		<< "12   F -  Filter       输入滤波核，执行滤波，输出滤波后图像\n" << endl;
	return;
}

bool CRSImage::SaveAsFile(const char* c_path) //输入保存的文件路径，输出图像为二进制文件
{
	/*功能描述：在控制台输入字符A或a，提示输入另存文件路径，写数据文件。
	返回值：错误-错误信息；正确-返回success。
	程序流程：1）写二进制流文件。*/
	if (!m_bOpen)
	{
		cerr << "Image Not Open." << endl;
		return false;
	}
	if (c_path == NULL) //路径输入错误报错 
	{
		cerr << "Enter Path Failed." << endl;
		return false;
	}
	DN*** pppDN = ImgProcess.GetDN();
	const int bands = ImgProcess.GetBands();
	const int lines = ImgProcess.GetRows();
	const int samples = ImgProcess.GetColumns();
	ofstream ofile(c_path, ios::binary); //定义ofstream类对象file，二进制方式打开路径
	if (!ofile.is_open()) //不同数据类型写入文件方式不同
	{
		cerr << "Open Path Failed." << endl;
		return false;
	}
	int i, j;
	if (m_interleave == BSQ)
	{
		for (i = 0; i < bands && !ofile.eof(); i++)
		{
			for (j = 0; j < lines && !ofile.eof(); j++)
			{
				ofile.write((char*)pppDN[i][j], sizeof(DN)*samples);
			}
		}
		if (i < bands || j < lines)
		{
			cerr << "Write BSQ Failed." << endl;
			ofile.close();
			return false;
		}
	}
	else if (m_interleave == BIL)
	{
		for (i = 0; i < lines && !ofile.eof(); i++)
		{
			for (j = 0; j < bands && !ofile.eof(); j++)
			{
				ofile.write((char*)pppDN[j][i], sizeof(DN)*samples);
			}
		}
		if (i < lines || j < bands)
		{
			cerr << "Write BIL Failed." << endl;
			ofile.close();
			return false;
		}
	}
	else if (m_interleave == BIP)
	{
		int* point = new int[bands]; //指针用于处理BIP型数据类型
		if (point == NULL)
		{
			cerr << "Point Failed." << endl;
			ofile.close();
			return false;
		}
		for (i = 0; i < samples*lines && !ofile.eof(); i++)
		{
			ofile.write((char*)point, sizeof(DN)*bands);
			for (j = 0; j < bands; j++)
			{
				pppDN[j][i / lines][i%lines] = point[j];
			}
		}
		if (i < samples*lines)
		{
			cerr << "Write BIP Failed." << endl;
			ofile.close();
			return false;
		}
	}
	ofile.close(); //关闭
	return true;
}

bool CRSImage::DisplayImage() //控制台显示图像
{
	if (!m_bOpen)
	{
		cerr << "Image Not Open." << endl;
		return false;
	}
	if (ImgProcess.Display())
	{
		return false;
	}
	return true;
}

bool CRSImage::InitImage() //初始化图像
{
	if (!m_bOpen)
	{
		return false;
	}
	ImgProcess.ReSet();
	return true;
}

bool CRSImage::RotateImage(double angle)
{
	/*功能描述：在控制台输入字符R或r，提示输入旋转角度（0-360°），将图像逆时针旋转指定角度。
	返回值：返回0。
	程序流程：
	1）输入旋转角度（0—360°）；
	2）旋转图像。*/
	if (!m_bOpen)
	{
		cerr << "Image Not Open." << endl;
		return false;
	}
	ImgProcess.SetAngle(angle);
	if (!ImgProcess.Display())
	{
		return false;
	}
	return true;
}

bool CRSImage::Zoom(int enlarge, int decrease)
{
	/*功能描述：在控制台输入字符Z或z，提示输入缩放比例，缩放图像。
	返回值：返回0。
	程序流程：
	1）输入缩放比例；
	2）执行图像缩放。*/
	if (!m_bOpen)
	{
		cerr << "Image Not Open." << endl;
		return false;
	}
	if (!ImgProcess.Enlarge(enlarge))
	{
		cerr << "Enlarge Failed." << endl;
		return false;
	}
	if (!ImgProcess.Decrease(decrease))
	{
		cerr << "Decrease Failed." << endl;
		return false;
	}
	if (!ImgProcess.Display())
	{
		return false;
	}
	cout << m_lines << endl;
	cout << m_samples << endl;
	return true;
}

bool CRSImage::Filter(char core)
{
	/*功能描述：在控制台输入字符F或f，提示输入滤波核，执行图像卷积。
	返回值：返回0。
	程序流程：
	1）输入滤波核；
	2）执行图像卷积。*/
	if (!m_bOpen)
	{
		cerr << "Image Not Open." << endl;
		return false;
	}
	int size = 0;
	if (core == 'M' || core == 'S' || core == 'D' || core == 'E')
	{
		size = 3;
	}
	else if (core == 'G')
	{
		size = 5;
	}
	else if (core == 'C')
	{
		cout << "Please enter size:";
		cin >> size;
	}
	else
	{
		cerr << "Core Is Illegal." << endl;
		return false;
	}
	if (size % 2 == 0)
	{
		cerr << "Size Can Not Be Odd." << endl;
		return false;
	}
	if (!ImgProcess.Filter(core, size))
	{
		cerr << "Filter Failed." << endl;
		return false;
	}
	if (!ImgProcess.Display())
	{
		return false;
	}
	return true;
}

bool CRSImage::ReadMetaData(const char* cpath)
{
	ifstream ifile; //定义一个ifstream类对象file 
	char chgetline[_MAX_PATH];
	string strgetline;
	string strtext;
	stringstream ss;
	ifile.open(cpath, ios_base::in);
	if (!ifile.is_open()) //打开错误报错 
	{
		return false;
	}
	while (!ifile.eof()) //直到文件末尾停止
	{
		ifile.getline(chgetline, _MAX_PATH);
		strgetline = chgetline;
		ss.clear();
		ss.str(strgetline);
		ss >> strtext;
		if (strtext == "samples")
		{
			ss >> strtext >> m_samples; //读取列
		}
		else if (strtext == "lines")
		{
			ss >> strtext >> m_lines; //读取行
		}
		else if (strtext == "bands")
		{
			ss >> strtext >> m_bands; //读取波段
		}
		else if (strtext == "interleave")
		{
			ss >> strtext >> strtext; //读取排列方式 
			if (strtext == "bsq")
			{
				m_interleave = BSQ; //波段顺序格式 
			}
			else if (strtext == "bip")
			{
				m_interleave = BIP; //波段按行交叉格式 
			}
			else if (strtext == "bil")
			{
				m_interleave = BIL; //波段按像元交叉格式
			}
		}
		else if (strtext == "data")
		{
			ss >> strtext;
			if (strtext == "type")
			{
				ss >> strtext >> m_datatype; //读取数据类型
			}
		}
	}
	ifile.close();
	return true;
}

bool CRSImage::InitBuffer()
{
	int i, j;
	m_pppDN = new DN**[m_bands]; //对pppData进行初始化
	if (m_pppDN == NULL)
	{
		return false;
	}
	for (i = 0; i < m_bands; i++)
	{
		m_pppDN[i] = 0UL;
	}
	for (i = 0; i < m_bands; ++i)
	{
		m_pppDN[i] = new DN*[m_lines];
		if (m_pppDN[i] == NULL)
		{
			return false;
		}
		for (j = 0; j < m_lines; ++j)
		{
			m_pppDN[i][j] = 0UL;
		}
		for (j = 0; j < m_lines; ++j)
		{
			m_pppDN[i][j] = new DN[m_samples];
		}
	}
	return true;
}

bool CRSImage::ReadImgData(const char* cpath)
{
	ifstream ifile(cpath, ios::binary); //定义一个ifstream类对象file，二进制方式打开路径
	if (!ifile.is_open())
	{
		cerr << "Open Path Failed." << endl;
		return false;
	}
	int i, j;
	if (m_interleave == BSQ)
	{
		for (i = 0; i < m_bands && !ifile.eof(); i++)
		{
			for (j = 0; j < m_lines && !ifile.eof(); j++)
			{
				ifile.read((char*)m_pppDN[i][j], sizeof(DN)*m_samples);
			}
		}
		if (i < m_bands || j < m_lines)
		{
			cerr << "Read BSQ Failed." << endl;
			ifile.close();
			return false;
		}
	}
	else if (m_interleave == BIL)
	{
		for (i = 0; i < m_lines && !ifile.eof(); i++)
		{
			for (j = 0; j < m_bands && !ifile.eof(); j++)
			{
				ifile.read((char*)m_pppDN[j][i], sizeof(DN)*m_samples);
			}
		}
		if (i < m_lines || j < m_bands)
		{
			cerr << "Read BIL Failed." << endl;
			ifile.close();
			return false;
		}
	}
	else if (m_interleave == BIP)
	{
		int* point = new int[m_bands]; //指针用于处理BIP型数据类型
		if (point == NULL)
		{
			cerr << "Point Failed." << endl;
			ifile.close();
			return false;
		}
		for (i = 0; i < m_samples*m_lines && !ifile.eof(); i++)
		{
			ifile.read((char*)point, sizeof(DN)*m_bands);
			for (j = 0; j < m_bands; j++)
			{
				m_pppDN[j][i / m_lines][i%m_lines] = point[j];
			}
		}
		if (i < m_samples*m_lines)
		{
			cerr << "Read BIP Failed." << endl;
			ifile.close();
			return false;
		}
	}
	ifile.close();
	return true;
}

const double CRSImage::GetAverage(DN** band) const //Statistics函数求均值
{
	double sum = 0;
	for (int i = 0; i < m_lines; i++)
	{
		for (int j = 0; j < m_samples; j++)
		{
			sum += band[i][j]; //累加
		}
	}
	return sum / static_cast<double>(m_lines*m_samples); //强制转换
}

const double CRSImage::GetVariance(DN** band) const //Statistics函数求方差
{
	double sum = 0;
	double Average = GetAverage(band); //避免多次调用均值函数
	for (int i = 0; i < m_lines; i++)
	{
		for (int j = 0; j < m_samples; j++)
		{
			sum += pow((band[i][j] - Average), 2); //差平方和
		}
	}
	return sum / static_cast<double>(m_lines*m_samples);
}

const int CRSImage::GetMaximum(DN** band) const //Statistics函数求最大值
{
	int result = 0;
	for (int i = 0; i < m_lines; i++)
	{
		for (int j = 0; j < m_samples; j++)
		{
			result = (band[i][j] > result) ? band[i][j] : result;
		}
	}
	return result;
}

const int CRSImage::GetMinimum(DN** band) const //Statisitcs函数求最小值
{
	int result = 256;
	for (int i = 0; i < m_lines; i++)
	{
		for (int j = 0; j < m_samples; j++)
		{
			result = (band[i][j] < result) ? band[i][j] : result;
		}
	}
	return result;
}