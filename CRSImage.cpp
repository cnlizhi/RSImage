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
	/*�ڿ���̨�����ַ�O��o��������ʾ����ͼ���ļ�·�����س��󣬶�ȡָ��ͼ���ļ���
	����ֵ��Ҫ�󷵻ش����룬0-Ϊ��ȡ�ɹ���
	�������̣�
	1����ȡͼ���ļ�ͷ���õ��С��С������������з�ʽ���������͵���Ϣ��
	2����̬����ͼ�������ڴ棬�ļ�IO��ȡ����������*/
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
	/*�ڿ���̨�����ַ�I��i����ͼ���Ѿ��򿪣����ͼ�����ͳ����Ϣ��
	����ֵ�����������������Ϣ���ɹ������ͼ����Ϣ��
	�������̣���ͼ���ļ��򿪣������ͼ��·���С��С������������з�ʽ���������͵���Ϣ������̨��*/
	if (!m_bOpen)
	{
		cerr << "Image Not Open." << endl;
		return false;
	}
	cout << "samples��   " << m_samples << endl;
	cout << "lines��     " << m_lines << endl;
	cout << "bands��     " << m_bands << endl;
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
	cout << "datatype:   " << m_datatype << endl; //�����������
	return true;
}

bool CRSImage::CloseImage()
{
	/*�����������ڿ���̨�����ַ�C��c���رյ�ǰ�򿪵��ļ���
	����ֵ�������򷵻ش�����Ϣ����ȷ������0��
	�������̣��ͷ�ͼ�����ݴ洢�ռ䡣*/
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
	/*�����������ڿ���̨�����ַ�S��s�������ǰͼ���ļ���ͳ����Ϣ��ͼ���ļ�δ�򿪣��򷵻���ʾ��Ϣ��
	����ֵ������-������Ϣ/��ȷ-����success��
	�������̣�
	1������ͼ�����ݣ�
	2��ͳ�������Ϣ����ֵ��������ֵ����Сֵ��*/
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
	/*�����������ڿ���̨�����ַ�H��h�������ǰͼ���ֱ��ͼ��Ϣ��ͼ���ļ�δ�򿪣��򷵻���ʾ��Ϣ��
	����ֵ������-������Ϣ/��ȷ-����success��
	�������̣�
	1������ͼ������������ݣ�
	2��ͳ��ֱ��ͼ*/
	if (!m_bOpen)
	{
		cerr << "Image Not Open." << endl;
		return false;
	}
	cout << "Histogram" << endl;
	cout << "Every '*' means 1000, and the top of '*' less than 1000." << endl; //ÿ��*����1000
	for (int i = 0; i < m_bands; i++)
	{
		int get = 0; //���ڸ�Array���鸳ֵ
		int Buffer[_MAX_PATH]; //һά�������ڴ洢ÿ����ֵ����Ƶ��
		int x_max = GetMaximum(m_pppDN[i]); //ֱ��ͼ��x�����ֵ
		int	y_max = 0; //ֱ��ͼ��y�����ֵ
		for (int x = 0; x < 256; x++) //Buffer�����ʼ��
		{
			Buffer[x] = 0;
		}
		for (int j = 0; j < m_lines; j++) //�õ�Buffer����ֵ
		{
			for (int k = 0; k < m_samples; k++)
			{
				get = m_pppDN[i][j][k];
				Buffer[get]++;
			}
		}
		for (int x = 0; x < x_max; x++) //ȷ���ò��������Ƶ�ʳ��ֵ�ֵ
		{
			y_max = (Buffer[x] > y_max) ? Buffer[x] : y_max;
		}
		cout << "Band " << i + 1 << endl;
		cout << "     |" << endl;
		for (int y = (y_max / 1000 * 1000 + 1000); y >= 0; y = y - 1000) //y����1000Ϊ��λ
		{ //ͨ���ݼ�ymax�ķ�ʽʵ���������
			cout << setw(5) << y << "|"; //���y��
			for (int x = 0; x < x_max; x++) //���ͼ��
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
		for (int x = 0; x < x_max; x++) //���x��
		{
			cout << "_";
		}
		cout << endl;
		cout << "      0";
		for (int x = 0; x < x_max; x++) //���x������
		{
			if ((x + 2) % 10 == 0) //ÿ10����λ���һ��ֵ
			{
				cout << setw(3) << x + 2;
			}
			else if (((x + 1) % 10 == 0) || (x % 10 == 0)) {} //�������ֵ�����λ�ò�������
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
	/*�����������ڿ���̨�����ַ�?����������˵���Ϣ��
	����ֵ������0��
	�������̣���ʾ�˵���Ϣ��*/
	cout << "�ڿ���̨��������ĸ�˵���ִ����Ӧ�Ĺ��ܡ��书�ܲ˵����£�" << endl;
	cout << "��� ���ܲ˵�          ����\n"
		<< "00   X �C Exit         �˳�����\n"
		<< "01   O �C Open Image   ����ͼ���ļ�·�������ɶ����ļ�����\n"
		<< "02   I �C Information  �����ǰͼ���·��������ֵ�����������������͡����з�ʽ��\n"
		<< "03   C �C Close Image  �رյ�ǰͼ��\n"
		<< "04   S �C Statistics   ���ͼ������ͳ�������ļ�δ�򿪣������ʾ\n"
		<< "05   H �C Histogram    ���ͼ���ֱ��ͼ\n"
		<< "06   ? �C Help         �������Ϣ\n"
		<< "07   A �C Save as File ���뱣����ļ�·�������ͼ��Ϊ�������ļ�\n"
		<< "08   D -  Display      ����̨��ʾͼ��\n"
		<< "09   T -  Init Image   ��ʼ��ͼ��\n"
		<< "10   R �C Rotate Image ͼ����ת��������ת�Ƕȣ���ʱ����תͼ��\n"
		<< "11   Z �C Zoom         ͼ�����ţ��������ű����ߣ��������ͼ��\n"
		<< "12   F -  Filter       �����˲��ˣ�ִ���˲�������˲���ͼ��\n" << endl;
	return;
}

bool CRSImage::SaveAsFile(const char* c_path) //���뱣����ļ�·�������ͼ��Ϊ�������ļ�
{
	/*�����������ڿ���̨�����ַ�A��a����ʾ��������ļ�·����д�����ļ���
	����ֵ������-������Ϣ����ȷ-����success��
	�������̣�1��д���������ļ���*/
	if (!m_bOpen)
	{
		cerr << "Image Not Open." << endl;
		return false;
	}
	if (c_path == NULL) //·��������󱨴� 
	{
		cerr << "Enter Path Failed." << endl;
		return false;
	}
	DN*** pppDN = ImgProcess.GetDN();
	const int bands = ImgProcess.GetBands();
	const int lines = ImgProcess.GetRows();
	const int samples = ImgProcess.GetColumns();
	ofstream ofile(c_path, ios::binary); //����ofstream�����file�������Ʒ�ʽ��·��
	if (!ofile.is_open()) //��ͬ��������д���ļ���ʽ��ͬ
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
		int* point = new int[bands]; //ָ�����ڴ���BIP����������
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
	ofile.close(); //�ر�
	return true;
}

bool CRSImage::DisplayImage() //����̨��ʾͼ��
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

bool CRSImage::InitImage() //��ʼ��ͼ��
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
	/*�����������ڿ���̨�����ַ�R��r����ʾ������ת�Ƕȣ�0-360�㣩����ͼ����ʱ����תָ���Ƕȡ�
	����ֵ������0��
	�������̣�
	1��������ת�Ƕȣ�0��360�㣩��
	2����תͼ��*/
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
	/*�����������ڿ���̨�����ַ�Z��z����ʾ�������ű���������ͼ��
	����ֵ������0��
	�������̣�
	1���������ű�����
	2��ִ��ͼ�����š�*/
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
	/*�����������ڿ���̨�����ַ�F��f����ʾ�����˲��ˣ�ִ��ͼ������
	����ֵ������0��
	�������̣�
	1�������˲��ˣ�
	2��ִ��ͼ������*/
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
	ifstream ifile; //����һ��ifstream�����file 
	char chgetline[_MAX_PATH];
	string strgetline;
	string strtext;
	stringstream ss;
	ifile.open(cpath, ios_base::in);
	if (!ifile.is_open()) //�򿪴��󱨴� 
	{
		return false;
	}
	while (!ifile.eof()) //ֱ���ļ�ĩβֹͣ
	{
		ifile.getline(chgetline, _MAX_PATH);
		strgetline = chgetline;
		ss.clear();
		ss.str(strgetline);
		ss >> strtext;
		if (strtext == "samples")
		{
			ss >> strtext >> m_samples; //��ȡ��
		}
		else if (strtext == "lines")
		{
			ss >> strtext >> m_lines; //��ȡ��
		}
		else if (strtext == "bands")
		{
			ss >> strtext >> m_bands; //��ȡ����
		}
		else if (strtext == "interleave")
		{
			ss >> strtext >> strtext; //��ȡ���з�ʽ 
			if (strtext == "bsq")
			{
				m_interleave = BSQ; //����˳���ʽ 
			}
			else if (strtext == "bip")
			{
				m_interleave = BIP; //���ΰ��н����ʽ 
			}
			else if (strtext == "bil")
			{
				m_interleave = BIL; //���ΰ���Ԫ�����ʽ
			}
		}
		else if (strtext == "data")
		{
			ss >> strtext;
			if (strtext == "type")
			{
				ss >> strtext >> m_datatype; //��ȡ��������
			}
		}
	}
	ifile.close();
	return true;
}

bool CRSImage::InitBuffer()
{
	int i, j;
	m_pppDN = new DN**[m_bands]; //��pppData���г�ʼ��
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
	ifstream ifile(cpath, ios::binary); //����һ��ifstream�����file�������Ʒ�ʽ��·��
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
		int* point = new int[m_bands]; //ָ�����ڴ���BIP����������
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

const double CRSImage::GetAverage(DN** band) const //Statistics�������ֵ
{
	double sum = 0;
	for (int i = 0; i < m_lines; i++)
	{
		for (int j = 0; j < m_samples; j++)
		{
			sum += band[i][j]; //�ۼ�
		}
	}
	return sum / static_cast<double>(m_lines*m_samples); //ǿ��ת��
}

const double CRSImage::GetVariance(DN** band) const //Statistics�����󷽲�
{
	double sum = 0;
	double Average = GetAverage(band); //�����ε��þ�ֵ����
	for (int i = 0; i < m_lines; i++)
	{
		for (int j = 0; j < m_samples; j++)
		{
			sum += pow((band[i][j] - Average), 2); //��ƽ����
		}
	}
	return sum / static_cast<double>(m_lines*m_samples);
}

const int CRSImage::GetMaximum(DN** band) const //Statistics���������ֵ
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

const int CRSImage::GetMinimum(DN** band) const //Statisitcs��������Сֵ
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