#include "eanim/WxLayersToolbar.h"
#include "eanim/config.h"
#include "eanim/MsgHelper.h"
#include "eanim/MessageID.h"
#include "eanim/AnimHelper.h"
#include "eanim/Callback.h"

#include <ee0/SubjectMgr.h>

#include <anim/KeyFrame.h>
#include <anim/Layer.h>
#include <anim/PlayCtrl.h>

#include <wx/sizer.h>
#include <wx/button.h>

namespace eanim
{

WxLayersToolbar::WxLayersToolbar(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
	                             anim::PlayCtrl& pctrl)
	: wxPanel(parent)
	, m_sub_mgr(sub_mgr)
	, m_pctrl(pctrl)
{
	SetBackgroundColour(MEDIUM_GRAY);

	InitLayout();
}

void WxLayersToolbar::InitLayout()
{
	wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

	wxButton* btn_add = new wxButton(this, wxID_ANY, "+", wxDefaultPosition, wxSize(25, 25));
	Connect(btn_add->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(WxLayersToolbar::OnAddLayer));
	sizer->Add(btn_add, 0, wxLEFT | wxRIGHT, 5);

	wxButton* btn_del = new wxButton(this, wxID_ANY, "-", wxDefaultPosition, wxSize(25, 25));
	Connect(btn_del->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(WxLayersToolbar::OnDelLayer));
	sizer->Add(btn_del, 0, wxLEFT | wxRIGHT, 5);

	sizer->AddSpacer(20);

	m_btn_play = new wxButton(this, wxID_ANY, ">", wxDefaultPosition, wxSize(25, 25));
	Connect(m_btn_play->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(WxLayersToolbar::OnPressPlay));
	sizer->Add(m_btn_play, 0, wxLEFT | wxRIGHT, 5);

	SetSizer(sizer);
}

void WxLayersToolbar::OnAddLayer(wxCommandEvent& event)
{
	int count = Callback::GetAllLayers().size();

	auto layer = std::make_unique<::anim::Layer>();
	layer->SetName("Layer" + std::to_string(count));

	layer->AddKeyFrame(std::make_unique<::anim::KeyFrame>(0));

	Callback::AddLayer(layer);
	m_sub_mgr->NotifyObservers(eanim::MSG_REFRESH_ANIM_COMP);

	eanim::MsgHelper::SetCurrFrame(*m_sub_mgr, count, 0);
}

void WxLayersToolbar::OnDelLayer(wxCommandEvent& event)
{

}

void WxLayersToolbar::OnPressPlay(wxCommandEvent& event)
{
	m_pctrl.SetActive(!m_pctrl.IsActive());
	m_btn_play->SetLabelText(m_pctrl.IsActive() ? "||" : ">");

	// update tree panel
	if (!m_pctrl.IsActive()) {
		Callback::RefreshAllNodes();
	}
}

}