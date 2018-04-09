#include "QtScreenShared.h"
#include <windows.h>
#include <QString>
#include <QGridLayout>
#include <QDir>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

//#include <atlstr.h>

QtScreenShared::QtScreenShared(QWidget *parent) : QMainWindow(parent)
{
	//ui.setupUi(this);

	//���ý���
	QLabel *pLabel = new QLabel("hello Qt");
	QVBoxLayout *pLayout = new QVBoxLayout;
	pLayout->addWidget(pLabel);
	setLayout(pLayout);
}

//��ȡִ��ʱ��
void GetThunderTime()
{
	LARGE_INTEGER litmp;
	_int64 QPart1, QPart2;
	double dfMinus, dfFreq, dfTim;
	QueryPerformanceFrequency(&litmp);
	dfFreq = (double)litmp.QuadPart;
	QueryPerformanceCounter(&litmp);
	QPart1 = litmp.QuadPart;

	//3 ���жϽ���λ�� ��ȡ����ʱ��

	QueryPerformanceCounter(&litmp);
	QPart2 = litmp.QuadPart;
	dfMinus = (double)(QPart2 - QPart1);
	dfTim = dfMinus / dfFreq * 1000;
	//��ʾʱ��
	QString msg4 = "ʱ�䣺", msg3, msg5 = "����";
	//msg3.Format("%10.9f", dfTim);
	msg3.asprintf("%10.9f", dfTim);
	QString st = msg4 + msg3 + msg5;
}


