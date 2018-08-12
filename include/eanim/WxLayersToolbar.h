#pragma once

#include <ee0/typedef.h>

#include <anim/Layer.h>

#include <wx/panel.h>

namespace anim { class PlayCtrl; }

namespace eanim
{

class WxLayersToolbar : public wxPanel
{
public:
	WxLayersToolbar(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
		anim::PlayCtrl& pctrl);

private:
	void InitLayout();

	void OnAddLayer(wxCommandEvent& event);
	void OnDelLayer(wxCommandEvent& event);

	void OnPressPlay(wxCommandEvent& event);

private:
	ee0::SubjectMgrPtr m_sub_mgr;

	anim::PlayCtrl& m_pctrl;

	wxButton* m_btn_play;

}; // WxLayersToolbar

}