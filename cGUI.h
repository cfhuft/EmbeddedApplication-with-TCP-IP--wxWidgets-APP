#pragma once
#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/dialog.h>


class cGUI : public wxFrame
{
public:
	cGUI();

	void OnSTOP(wxCommandEvent& event);
	void OnBatteryCheck(wxCommandEvent& event);
	void OnMeasureRotation(wxCommandEvent& event);
	void OnMotorControl(wxCommandEvent& event);
	void OnScroll(wxScrollEvent& event);
	void OnClear(wxCommandEvent& event);
	void OnSocketEvent(wxSocketEvent& event);
	void OnOpenConnection(wxCommandEvent& event);
	void OnCloseConnection(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);

	void Send(int32_t value);
	void UpdateStatus();
private:
	wxPanel* panel;
	wxSizer* sizer;
	wxButton* btn1;
	wxButton* btn2;
	wxButton* btn3;
	wxButton* btn4;
	wxButton* btn5;
	wxListBox* list;
	wxTextCtrl* text;
	wxSocketClient* sock;
	wxIPV4address addr;
	wxSlider* slider;
	wxStaticText* slider_label1;
	wxStaticText* slider_label2;
	wxMenu* fileMenu;
	wxMenu* helpMenu;
	wxMenuBar* menuBar;
	wxDialog* dialog;
	wxPanel* panel_dialog;
	wxTextCtrl* tc1;
	wxTextCtrl* tc2;
	wxStaticText* text1;
	wxStaticText* text2;
	wxButton* dialogButton;
	wxString IP;
	wxString port;
	wxStaticText* static_txt;
	wxPanel* static_txt_panel;
	uint8_t state = 0;

	wxDECLARE_EVENT_TABLE();
};