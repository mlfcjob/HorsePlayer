#pragma once
#include <QThread>
class XVideoThread : public QThread
{
public:
	//����ģʽ
	static XVideoThread *Get()
	{
		static XVideoThread vt;
		return &vt;
	}

	void run();
	XVideoThread();
	virtual ~XVideoThread();
};

