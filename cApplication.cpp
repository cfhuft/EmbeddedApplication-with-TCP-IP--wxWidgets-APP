#include "cApplication.h"

wxIMPLEMENT_APP(cApplication);
cApplication::cApplication()
{
}

cApplication::~cApplication()
{
}

bool cApplication::OnInit()
{
	m_frame1 = new cGUI(); //allocate new memory in heap for class cMain()
	m_frame1->Show(); //show window GUI
	return true;
}