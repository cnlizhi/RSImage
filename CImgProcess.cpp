//CImgProcess.cpp

#include "stdafx.h"
#include "CImgProcess.h"
#include "Windows.h"
#include <iostream>
#include <cmath>
using namespace std;

#define _WIN32_WINNT 0x0500
#define _MAIN_FILTER 1,1,1,1,1,1,1,1,1
#define _GAUSS_FILTER 1,4,7,4,1,4,16,26,16,4,7,26,41,26,7,4,16,26,16,4,1,4,7,4,1
#define _SHARPNESS_FILTER -1,-1,-1,-1,9,-1,-1,-1,-1
#define _EDGE_DETECTION -1,-1,-1,-1,8,-1,-1,-1,-1
#define _EMBOSSING_FILTER -1,-1,0,-1,0,1,0,1,1
const double PI = 3.141592653589793;

CImgProcess::CImgProcess()
	: m_pppDN(NULL), m_columns(0), m_rows(0), m_bands(0), m_angle(0)
{
}

CImgProcess::CImgProcess(DN*** pppDN, int bands, int rows, int columns)
	: m_pppDN(pppDN), m_bands(bands), m_rows(rows), m_columns(columns), m_angle(0)
{
}

CImgProcess::CImgProcess(const CImgProcess& rIProcess)
	: m_pppDN(rIProcess.m_pppDN), m_bands(rIProcess.m_bands),
	m_rows(rIProcess.m_rows), m_columns(rIProcess.m_columns), m_angle(rIProcess.m_angle)
{
}

CImgProcess::~CImgProcess()
{
	if (m_pppDN != NULL)
	{
		delete[] m_pppDN;
	}
}

bool CImgProcess::Set(DN*** pppDN, int bands, int rows, int columns)
{
	if (pppDN == NULL)
	{
		return false;
	}
	m_pppDN = new DN**[bands];
	if (m_pppDN == NULL)
	{
		return false;
	}
	for (int i = 0; i < bands; ++i)
	{
		m_pppDN[i] = 0UL;
		m_pppDN[i] = new DN*[rows];
		if (m_pppDN[i] == NULL)
		{
			return false;
		}
		for (int j = 0; j < rows; ++j)
		{
			m_pppDN[i][j] = 0UL;
			m_pppDN[i][j] = new DN[columns];
		}
	}
	for (int i = 0; i < bands; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			for (int k = 0; k < columns; k++)
			{
				m_pppDN[i][j][k] = pppDN[i][j][k];
			}
		}
	}
	m_bands = bands;
	m_rows = rows;
	m_columns = columns;
	return true;
}

void CImgProcess::ReSet()
{
	m_pppDN = NULL;
	m_bands = 0;
	m_rows = 0;
	m_columns = 0;
	m_angle = 0;
	return;
}

bool CImgProcess::Display()
{
	if (m_bands > 2)
	{
		if (!Print(m_pppDN[2], m_pppDN[1], m_pppDN[0]))
		{
			return false;
		}
	}
	else
	{
		if (!Print(m_pppDN[0], m_pppDN[0], m_pppDN[0]))
		{
			return false;
		}
	}
	return true;
}

void CImgProcess::SetAngle(double angle)
{
	m_angle = angle;
	return;
}

bool CImgProcess::Enlarge(int enlarge)
{
	if (enlarge == 0)
	{
		return false;
	}
	m_columns *= enlarge;
	m_rows *= enlarge;
	DN*** pppDNBuff = new DN**[m_bands];
	if (pppDNBuff == NULL)
	{
		return false;
	}
	for (int i = 0; i < m_bands; ++i)
	{
		pppDNBuff[i] = 0UL;
		pppDNBuff[i] = new DN*[m_rows];
		if (pppDNBuff[i] == NULL)
		{
			return false;
		}
		for (int j = 0; j < m_rows; ++j)
		{
			pppDNBuff[i][j] = 0UL;
			pppDNBuff[i][j] = new DN[m_columns];
		}
	}
	for (int i = 0; i < m_bands; i++)
	{
		for (int j = 0; j < m_rows; j++)
		{
			for (int k = 0; k < m_columns; k++)
			{
				pppDNBuff[i][j][k] = m_pppDN[i][j / enlarge][k / enlarge];
			}
		}
	}
	delete[] m_pppDN;
	m_pppDN = new DN**[m_bands];
	if (m_pppDN == NULL)
	{
		return false;
	}
	for (int i = 0; i < m_bands; ++i)
	{
		m_pppDN[i] = 0UL;
		m_pppDN[i] = new DN*[m_rows];
		if (m_pppDN[i] == NULL)
		{
			return false;
		}
		for (int j = 0; j < m_rows; ++j)
		{
			m_pppDN[i][j] = 0UL;
			m_pppDN[i][j] = new DN[m_columns];
		}
	}
	for (int i = 0; i < m_bands; i++)
	{
		for (int j = 0; j < m_rows; j++)
		{
			for (int k = 0; k < m_columns; k++)
			{
				m_pppDN[i][j][k] = pppDNBuff[i][j][k];
			}
		}
	}
	delete[] pppDNBuff;
	return true;
}

bool CImgProcess::Decrease(int decrease)
{
	if (decrease == 0)
	{
		return false;
	}
	m_columns /= decrease;
	m_rows /= decrease;
	DN*** pppDNBuff = new DN**[m_bands];
	if (pppDNBuff == NULL)
	{
		return false;
	}
	for (int i = 0; i < m_bands; ++i)
	{
		pppDNBuff[i] = 0UL;
		pppDNBuff[i] = new DN*[m_rows];
		if (pppDNBuff[i] == NULL)
		{
			return false;
		}
		for (int j = 0; j < m_rows; ++j)
		{
			pppDNBuff[i][j] = 0UL;
			pppDNBuff[i][j] = new DN[m_columns];
		}
	}
	for (int i = 0; i < m_bands; i++)
	{
		for (int j = 0; j < m_rows; j++)
		{
			for (int k = 0; k < m_columns; k++)
			{
				pppDNBuff[i][j][k] = m_pppDN[i][j*decrease][k*decrease];
			}
		}
	}
	delete[] m_pppDN;
	m_pppDN = new DN**[m_bands];
	if (m_pppDN == NULL)
	{
		return false;
	}
	for (int i = 0; i < m_bands; ++i)
	{
		m_pppDN[i] = 0UL;
		m_pppDN[i] = new DN*[m_rows];
		if (m_pppDN[i] == NULL)
		{
			return false;
		}
		for (int j = 0; j < m_rows; ++j)
		{
			m_pppDN[i][j] = 0UL;
			m_pppDN[i][j] = new DN[m_columns];
		}
	}
	for (int i = 0; i < m_bands; i++)
	{
		for (int j = 0; j < m_rows; j++)
		{
			for (int k = 0; k < m_columns; k++)
			{
				m_pppDN[i][j][k] = pppDNBuff[i][j][k];
			}
		}
	}
	delete[] pppDNBuff;
	return true;
}

bool CImgProcess::Filter(char core, int size)
{
	double* CoreTemp = NULL;
	switch (core)
	{
	case 'M':
	{
		double BuffCore[9] = { _MAIN_FILTER };
		CoreTemp = BuffCore;
		break;
	}
	case 'G':
	{
		double BuffCore[25] = { _GAUSS_FILTER };
		CoreTemp = BuffCore;
		break;
	}
	case 'S':
	{
		double BuffCore[9] = { _SHARPNESS_FILTER };
		CoreTemp = BuffCore;
		break;
	}
	case 'D':
	{
		double BuffCore[9] = { _EDGE_DETECTION };
		CoreTemp = BuffCore;
		break;
	}
	case 'E':
	{
		double BuffCore[9] = { _EMBOSSING_FILTER };
		CoreTemp = BuffCore;
		break;
	}
	case 'C':
		cout << "Please enter " << size << "*" << size << " numbers:" << endl;
		for (int index = 0; index < size*size; index++)
		{
			double i;
			cin >> i;
			CoreTemp[index] = i;
		}
		break;
	default:
		return false;
		break;
	}
	DN*** pppDNBuff = new DN**[m_bands];
	if (pppDNBuff == NULL)
	{
		return false;
	}
	for (int i = 0; i < m_bands; ++i)
	{
		pppDNBuff[i] = 0UL;
		pppDNBuff[i] = new DN*[m_rows];
		if (pppDNBuff[i] == NULL)
		{
			return false;
		}
		for (int j = 0; j < m_rows; ++j)
		{
			pppDNBuff[i][j] = 0UL;
			pppDNBuff[i][j] = new DN[m_columns];
		}
	}
	for (int i = 0; i < m_bands; i++)
	{
		for (int j = 0; j < m_rows; j++)
		{
			for (int k = 0; k < m_columns; k++)
			{
				pppDNBuff[i][j][k] = m_pppDN[i][j][k];
			}
		}
	}
	double total = 0;
	for (int i = 0; i < size*size; i++)
	{
		total += CoreTemp[i];
	}
	for (int i = 0; i < m_bands; i++)
	{
		for (int j = size / 2; j < m_rows - size / 2; j++)
		{
			for (int k = size / 2; k < m_columns - size / 2; k++)
			{
				double sum = 0;
				int index = 0;
				for (int l = j - size / 2; l < j + size / 2 + 1; l++)
				{
					for (int m = k - size / 2; m < k + size / 2 + 1; m++)
					{
						sum += pppDNBuff[i][l][m] * CoreTemp[index++];
					}
				}
				if (total != 0)
				{
					sum /= total;
				}
				if (sum > 255)
				{
					sum = 255;
				}
				if (sum < 0)
				{
					sum = 0;
				}
				m_pppDN[i][j][k] = static_cast<DN>(sum);
			}

		}
	}
	delete[] pppDNBuff;
	return true;
}

DN*** CImgProcess::GetDN() const
{
	return m_pppDN;
}

const int CImgProcess::GetBands() const
{
	return m_bands;
}

const int CImgProcess::GetRows() const
{
	return m_rows;
}

const int CImgProcess::GetColumns() const
{
	return m_columns;
}

bool CImgProcess::Print(DN** Red, DN** Grn, DN** Blu)
{
	HWND hwnd = NULL;
	HDC hdc = NULL;
	hwnd = GetConsoleWindow();
	hdc = GetDC(hwnd);
	if (hdc == NULL)
	{
		return false;
	}
	DN** Col[3] = { Red,Grn,Blu };
	for (int i = 0; i < 3; i++)
	{
		if (Col[i] == NULL)
		{
			return false;
		}
		int Maximum = GetMaximum(Col[i]);
		int Minimum = GetMinimum(Col[i]);
		for (int j = 0; j < m_rows; j++)
		{
			for (int k = 0; k < m_columns; k++)
			{
				Col[i][j][k] = (Col[i][j][k] - Minimum) * 255 / (Maximum - Minimum);
			}
		}
	}
	int txradius = m_columns / 2;
	int tyradius = m_rows / 2;
	double rad = 0;
	if (!IsLargeSize())
	{
		txradius = static_cast<int>(sqrt(pow(m_columns / 2, 2) + pow(m_rows / 2, 2)));
		tyradius = txradius;
		rad = m_angle * PI / 180;
	}
	for (int i = 0; i < m_rows; i++)
	{
		for (int j = 0; j < m_columns; j++)
		{
			int tx = static_cast<int>(((i - m_rows / 2)*sin(rad) + (j - m_columns / 2)*cos(rad) + txradius));
			int ty = static_cast<int>(((i - m_rows / 2)*cos(rad) - (j - m_columns / 2)*sin(rad) + tyradius));
			SetPixel(hdc, tx, ty, RGB(Red[i][j], Grn[i][j], Blu[i][j])); //Öð¸öÏñÔªÊäÈë
		}
	}
	ReleaseDC(hwnd, hdc);
	return true;
}

bool CImgProcess::IsLargeSize()
{
	if (m_rows > 1000 || m_columns > 1000)
	{
		return true;
	}
	return false;
}

const int CImgProcess::GetMaximum(DN** band) const
{
	int result = 0;
	for (int i = 0; i < m_rows; i++)
	{
		for (int j = 0; j < m_columns; j++)
		{
			result = (band[i][j] > result) ? band[i][j] : result;
		}
	}
	return result;
}

const int CImgProcess::GetMinimum(DN** band) const
{
	int result = 256;
	for (int i = 0; i < m_rows; i++)
	{
		for (int j = 0; j < m_columns; j++)
		{
			result = (band[i][j] < result) ? band[i][j] : result;
		}
	}
	return result;
}