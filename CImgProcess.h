//CImgProcess.h

#pragma once
#ifndef _CIMGPROCESS_H
#define _CIMGPROCESS_H

#include <ostream>

typedef unsigned char DN;

class CImgProcess
{
public:
	CImgProcess();
	CImgProcess(DN*** pppDN, int bands, int rows, int columns);
	CImgProcess(const CImgProcess& rIProcess);
	~CImgProcess();
	bool Set(DN*** pppDN, int bands, int rows, int columns);
	void ReSet();
	bool Display();
	void SetAngle(double angle);
	bool Enlarge(int encrease);
	bool Decrease(int decrease);
	bool Filter(char core, int size);
	DN*** GetDN() const;
	const int GetBands() const;
	const int GetRows() const;
	const int GetColumns() const;
protected:
	bool Print(DN** Red, DN** Grn, DN** Blu);
	bool IsLargeSize();
	const int GetMaximum(DN** band) const;
	const int GetMinimum(DN** band) const;
private:
	DN*** m_pppDN;
	int m_columns;
	int m_rows;
	int m_bands;
	double m_angle;
};

#endif //_CIMGPROCESS_H