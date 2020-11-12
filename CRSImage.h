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
public: //���г�Ա����
	CRSImage(); //���캯��
	CRSImage(const CRSImage& rImage); //���ƹ��캯��
	~CRSImage(); //��������
	bool OpenImage(const char* c_path); //����ͼ���ļ�·�������ɶ����ļ�����
	bool Information(); //�����ǰͼ���·��������ֵ�����������������͡����з�ʽ��
	bool CloseImage(); //�رյ�ǰͼ��
	bool Statistics(); //���ͼ������ͳ�������ļ�δ�򿪣������ʾ 
	bool Histogram(); //���ͼ���ֱ��ͼ 
	const void Help() const; //���������Ϣ
	bool SaveAsFile(const char* c_path); //���뱣����ļ�·�������ͼ��Ϊ�������ļ�
	bool DisplayImage(); //����̨��ʾͼ��
	bool InitImage(); //��ʼ��ͼ��
	bool RotateImage(double angle); //ͼ����ת��������ת�Ƕȣ���ʱ����תͼ�� 
	bool Zoom(int enlarge, int decrease); //ͼ�����ţ��������ű����ߣ��������ͼ�� 
	bool Filter(char core); //�����˲��ˣ�ִ���˲�������˲���ͼ��
protected: //������Ա����
	bool ReadMetaData(const char* c_path); //OpenImage������ȡԪ���� 
	bool InitBuffer(); //OpenImage������ʼ������ 
	bool ReadImgData(const char* c_path); //OpenImage������ȡ�ļ�  
	const double GetAverage(DN** band) const; //Statistics�������ֵ
	const double GetVariance(DN** band) const; //Statistics�����󷽲�
	const int GetMaximum(DN** band) const; //Statistics���������ֵ
	const int GetMinimum(DN** band) const; //Statisitcs��������Сֵ
private: //˽�����ݳ�Ա
	CImgProcess ImgProcess; //����ͼ����
	bool m_bOpen; //�ж�ͼ���
	DN*** m_pppDN; //��ά����
	int m_samples; //�� 
	int m_lines; //�� 
	int m_bands; //����
	INTERLEAVE m_interleave; //���з�ʽ
	short m_datatype; //��������
};

#endif //_CREIMAGE_H