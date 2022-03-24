#pragma once
#include <iostream>
#include <string.h>
#include "cGUI.h"


class cApplication : public wxApp
{
public:
	cApplication();
	~cApplication(); 
	virtual bool OnInit();

private:
	cGUI* m_frame1 = nullptr;
};