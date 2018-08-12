#include "eanim/WxTimelinePanel.h"
#include "eanim/WxTimeScaleCtrl.h"
#include "eanim/WxTimeStageScrolled.h"

#include <ee0/SubjectMgr.h>

#include <eanim/WxLayersScrolled.h>
#include <eanim/WxLayersToolbar.h>
#include <eanim/MsgHelper.h>
#include <node2/CompAnim.h>
#include <node2/CompAnimInst.h>

#include <wx/splitter.h>
#include <wx/sizer.h>

namespace eanim
{

WxTimelinePanel::WxTimelinePanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
	                             anim::PlayCtrl& pctrl)
	: wxPanel(parent)
{
	InitLayout(sub_mgr, pctrl);

	eanim::MsgHelper::SetCurrFrame(*sub_mgr, 0, 0);
}

void WxTimelinePanel::InitLayout(const ee0::SubjectMgrPtr& sub_mgr, anim::PlayCtrl& pctrl)
{
	wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY);

	wxWindow* left_panel = new wxPanel(splitter);
	wxWindow* right_panel = new wxPanel(splitter);
	{
		wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->Add(new eanim::WxLayersToolbar(left_panel, sub_mgr, pctrl), 1, wxEXPAND);
		sizer->Add(new eanim::WxLayersScrolled(left_panel, sub_mgr), 99, wxEXPAND);
		left_panel->SetSizer(sizer);
	}
	{
		wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->Add(new eanim::WxTimeScaleCtrl(right_panel, sub_mgr, pctrl), 0, wxEXPAND);
		sizer->Add(new eanim::WxTimeStageScrolled(right_panel, sub_mgr, pctrl), 99, wxEXPAND);
		right_panel->SetSizer(sizer);
	}
	splitter->SetSashGravity(0.1f);
	splitter->SplitVertically(left_panel, right_panel);

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(splitter, 1, wxEXPAND);
	SetSizer(sizer);
}

}