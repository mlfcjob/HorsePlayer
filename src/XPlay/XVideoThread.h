#pragma once
#include <QThread>
class XVideoThread : public QThread
{
public:
	//µ¥¼üÄ£Ê½
	static XVideoThread *Get()
	{
		static XVideoThread vt;
		return &vt;
	}

	void run();
	XVideoThread();
	virtual ~XVideoThread();
};

