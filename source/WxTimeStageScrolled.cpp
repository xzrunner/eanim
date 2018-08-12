#include "eanim/WxTimeStageScrolled.h"
#include "eanim/MessageID.h"
#include "eanim/WxTimeStagePanel.h"

#include <guard/check.h>
#include <ee0/SubjectMgr.h>

#include <wx/sizer.h>

namespace eanim
{

BEGIN_EVENT_TABLE(WxTimeStageScrolled, wxScrolledWindow)
	EVT_SCROLLWIN(WxTimeStageScrolled::OnScroll)
END_EVENT_TABLE()

WxTimeStageScrolled::WxTimeStageScrolled(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
	                                     const anim::PlayCtrl& pctrl)
	: wxScrolledWindow(parent)
	, m_sub_mgr(sub_mgr)
{
	SetScrollbars(1, 1, 20, 10, 0, 0);
	InitLayout(sub_mgr, pctrl);
}

void WxTimeStageScrolled::InitLayout(const ee0::SubjectMgrPtr& sub_mgr, const anim::PlayCtrl& pctrl)
{
	auto sizer = new wxBoxSizer(wxVERTICAL);
	auto layers = new WxTimeStagePanel(this, sub_mgr, pctrl);
	sizer->Add(layers, 1, wxEXPAND);
	SetSizer(sizer);
}

void WxTimeStageScrolled::OnScroll(wxScrollWinEvent& event)
{
	auto pos = GetViewStart();

	ee0::VariantSet vars;

	ee0::Variant var_x;
	var_x.m_type = ee0::VT_LONG;
	var_x.m_val.l = pos.x;
	vars.SetVariant("x", var_x);

	ee0::Variant var_y;
	var_y.m_type = ee0::VT_LONG;
	var_y.m_val.l = pos.y;
	vars.SetVariant("y", var_y);

	m_sub_mgr->NotifyObservers(eanim::MSG_WND_SCROLL, vars);

	event.Skip();
}

}