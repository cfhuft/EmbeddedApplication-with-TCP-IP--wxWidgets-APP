#include "cGUI.h"

wxBEGIN_EVENT_TABLE(cGUI, wxFrame) //begin event table
EVT_BUTTON(1, OnSTOP)
EVT_BUTTON(2, OnBatteryCheck)
EVT_BUTTON(3, OnMeasureRotation)
EVT_BUTTON(4, OnMotorControl)
EVT_COMMAND_SCROLL(5, OnScroll)
EVT_BUTTON(6, OnClear)
EVT_SOCKET(7, OnSocketEvent)
EVT_MENU(8, OnOpenConnection)
EVT_MENU(9, OnCloseConnection)
EVT_MENU(10, OnAbout)
EVT_MENU(11, OnQuit)

wxEND_EVENT_TABLE()

cGUI::cGUI() : wxFrame(nullptr, wxID_ANY, "Application", wxPoint(600, 300))
{
	panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(580, 440));
	sizer = new wxBoxSizer(wxVERTICAL);
	panel->SetBackgroundColour(wxColour(*wxLIGHT_GREY));

	btn1 = new wxButton(panel, 1, "Stop", wxPoint(180, 103), wxSize(80, 30));
	btn2 = new wxButton(panel, 2, "Battery Check", wxPoint(20, 30), wxSize(140, 35));
	btn3 = new wxButton(panel, 3, "Measure Rotation", wxPoint(20, 100), wxSize(140, 35));
	btn4 = new wxButton(panel, 4, "PWM Control", wxPoint(20, 290), wxSize(140, 35));
	btn5 = new wxButton(panel, 6, "Clear", wxPoint(235, 400), wxSize(80, 30));
	
	slider = new wxSlider(panel, 5, 200, -10, 10, wxPoint(30, 210), wxSize(250, 50));
	slider->SetValue(0);
	slider_label1 = new wxStaticText(panel, wxID_ANY, "-100", wxPoint(7, 215));
	slider_label2 = new wxStaticText(panel, wxID_ANY, "100", wxPoint(279, 215));
	text = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(20, 260), wxSize(170, 20));
	list = new wxListBox(panel, wxID_ANY, wxPoint(320, 5), wxSize(255, 430));

	sock = new wxSocketClient();
	sock->SetEventHandler(*this, 7);
	sock->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
	sock->Notify(true);

	dialog = new wxDialog(this, wxID_ANY, "Connect", wxPoint(800, 350), wxSize(220, 250));
	panel_dialog = new wxPanel(dialog, wxID_ANY);
	tc1 = new wxTextCtrl(panel_dialog, wxID_ANY, "", wxPoint(50, 50), wxSize(100, 20), wxTE_LEFT, wxTextValidator(wxFILTER_NONE, &IP));
	tc2 = new wxTextCtrl(panel_dialog, wxID_ANY, "", wxPoint(50, 100), wxSize(100, 20), wxTE_LEFT, wxTextValidator(wxFILTER_NONE, &port));
	text1 = new wxStaticText(panel_dialog, wxID_ANY, "Enter IP to connect to: ", wxPoint(40, 30));
	text2 = new wxStaticText(panel_dialog, wxID_ANY, "Enter port number: ", wxPoint(50, 80));
	dialogButton = new wxButton(panel_dialog, wxID_OK, "Ok", wxPoint(20, 150), wxSize(70, 30));
	dialogButton = new wxButton(panel_dialog, wxID_CANCEL, "Cancel", wxPoint(110, 150), wxSize(70, 30));

	fileMenu = new wxMenu();
	helpMenu = new wxMenu();
	menuBar = new wxMenuBar();
	SetMenuBar(menuBar);
	fileMenu->Append(8, "Connect");
	fileMenu->Append(9, "Disconnect");
	helpMenu->Append(10, "&About\tF1");
	fileMenu->Append(11, "E&xit\tAlt-F4");
	menuBar->Append(fileMenu, "&File");
	menuBar->Append(helpMenu, "&Help");

	sizer->Add(panel, 1, wxEXPAND | wxALL, 0);
	this->SetSizerAndFit(sizer);
	sizer->Layout();

	UpdateStatus();
}

void cGUI::OnSTOP(wxCommandEvent& event)
{
	if (sock->IsConnected()) 
	{
		cGUI::Send(0);
		list->AppendString(wxT("Stopped\n"));
	}
	event.Skip();
}

void cGUI::OnBatteryCheck(wxCommandEvent& event)
{
	if (sock->IsConnected())
	{
		int32_t option;
		option = 1 << 16;
		option = option | 0;
		cGUI::state = 1;
		cGUI::Send(option);
	}
	event.Skip();
}

void cGUI::OnMeasureRotation(wxCommandEvent& event)
{
	if (sock->IsConnected())
	{
		int32_t option;
		option = 2 << 16;
		option = option | 0;
		cGUI::state = 2;
		cGUI::Send(option);
	}
	event.Skip();
}

void cGUI::OnMotorControl(wxCommandEvent& event)
{
	if (wxAtoi(text->GetValue()) >= -100 && wxAtoi(text->GetValue()) <= 100)
	{
		slider->SetValue(wxAtoi(text->GetValue())/10);
		if (sock->IsConnected())
		{
			int32_t option;
			option = 3 << 16;
			option = option | (wxAtoi(text->GetValue())&0x0000ffff);
			list->AppendString(wxString::Format(wxT("PWM: %d%c\n"), wxAtoi(text->GetValue()), '%'));
			cGUI::Send(option);
		}
	}
	event.Skip();
}

void cGUI::OnScroll(wxScrollEvent& event)
{
	if (sock->IsConnected())
	{
		int32_t option;
		option = 3 << 16;
		option = option | ((slider->GetValue()*10)&0x0000ffff);
		list->AppendString(wxString::Format(wxT("PWM: %d%c\n"), (slider->GetValue() * 10), '%'));
		cGUI::Send(option);
	}
	event.Skip();
}

void cGUI::Send(int32_t value)
{
	if (sock->Write(&value, sizeof(int32_t)).LastCount() == 0)
	{
		list->AppendString(wxT("Write error.\n"));
		return;
	}
	UpdateStatus();
}

void cGUI::OnSocketEvent(wxSocketEvent& event)
{
	wxSocketBase* sockBase = event.GetSocket();

	switch (event.GetSocketEvent())
	{
		case wxSOCKET_LOST: {
			list->AppendString(wxT("Connection not found or lost.\n"));
			break;
		}
		case wxSOCKET_CONNECTION: {
			list->AppendString(wxString::Format(wxT("Connected to: %s : %u\n"), addr.IPAddress(), addr.Service()));
			break; 
		}
		case wxSOCKET_INPUT:
		{
		uint32_t data;
		int16_t angle;
		uint16_t parameter1;
		uint16_t parameter2;
		uint16_t voltage;
		uint16_t cell1;
		uint16_t cell2;
		wxUint32 len = sockBase->Read(&data, 4).LastCount();

		if (!len) {
			list->AppendString(wxT("Failed to read the message.\n"));
			return;
		}
		else {
			if (cGUI::state == 1) {
				parameter1 = data;
				parameter2 = data >> 16;

				if (parameter2 > 3370) { parameter2 = 3370;}
				if (parameter1 > 1646) { parameter1 = 1646;}		
				cell1 = (parameter1 - 1281) * (100 - 0) / (1646 - 1281) + 0;
				voltage = (parameter2 - 2623) * (100 - 0) / (3370 - 2623) + 0;
				cell2 = voltage*2 - cell1;
				list->AppendString(wxString::Format(wxT("Cell 1: %d%c (%.2fV)\n"), cell1, '%', 3.25+((float)cell1/100*0.92)));
				list->AppendString(wxString::Format(wxT("Cell 2: %d%c (%.2fV)\n"), cell2, '%', 3.25 +((float)cell2/100*0.92)));
				list->AppendString(wxString::Format(wxT("Overall: %d%c (%.2fV)\n"), voltage, '%', 6.5+((float)voltage/100*1.85)));
			}
			else if (cGUI::state == 2) {
				angle = data;
				list->AppendString(wxString::Format(wxT("Angle: %d°\n"), angle));
			}
		}
		break;
		}
		default:;
	}
	UpdateStatus();
}

void cGUI::OnClear(wxCommandEvent& event)
{
	list->Clear();
	event.Skip();
}

void cGUI::OnOpenConnection(wxCommandEvent& WXUNUSED(event))
{
	if (sock->IsConnected()) 
	{
		list->AppendString(wxT("Already connected to a server.\n"));
	}
	else 
	{
		if (dialog->ShowModal() == wxID_OK)
		{
			if (strlen(cGUI::IP) != 0 && strlen(cGUI::port) != 0)
			{
				addr.Hostname(cGUI::IP);			//IP
				addr.Service(cGUI::port);			//port
				sock->Connect(addr, false);
				list->AppendString(wxString::Format(wxT("Trying to connect to %s port %u \n"),
					addr.IPAddress(), addr.Service()));
				
				
			}
		}
	}
	UpdateStatus();
}

void cGUI::OnCloseConnection(wxCommandEvent& WXUNUSED(event))
{
	if (sock->IsConnected())
	{
		sock->Close();
		list->AppendString(wxString::Format(wxT("Disconnected from %s : %u \n"), addr.IPAddress(), addr.Service()));
	}
	UpdateStatus();
}

void cGUI::UpdateStatus()
{
	if (sock->IsConnected()) 
	{
		static_txt_panel = new wxPanel(this, wxID_ANY, wxPoint(10, 390), wxSize(115, 40));
		static_txt_panel->SetBackgroundColour(wxColour(*wxGREEN));
		static_txt = new wxStaticText(static_txt_panel, wxID_ANY, (wxString::Format(wxT("     Connected to:\n%s : %u    "), addr.IPAddress(), addr.Service())), wxPoint(5, 5));
	}
	else 
	{
		static_txt_panel = new wxPanel(this, wxID_ANY, wxPoint(10, 390), wxSize(115, 40));
		static_txt_panel->SetBackgroundColour(wxColour(*wxYELLOW));
		static_txt = new wxStaticText(static_txt_panel, wxID_ANY, "              Not\n        connected           ", wxPoint(5, 5));
	}
	list->SetSelection(list->GetCount() - 1);
}

void cGUI::OnAbout(wxCommandEvent& event)
{
	wxMessageBox(wxString::Format
	(
		"Motor control application\n"
		"\n"
		"Author: Samo Novak \n"
		"Website: http://somethingsomething.com"

	), "About", 0, this);
}

void cGUI::OnQuit(wxCommandEvent& event)
{
	Close(true);
}