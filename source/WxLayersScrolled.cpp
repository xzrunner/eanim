#include "eanim/WxLayersScrolled.h"
#include "eanim/MessageID.h"

#include <ee0/SubjectMgr.h>

#include <guard/check.h>
#include <anim/Layer.h>

#include <wx/sizer.h>

namespace
{

const uint32_t MESSAGES[] =
{
	eanim::MSG_WND_SCROLL,
};

}

namespace eanim
{

WxLayersScrolled::WxLayersScrolled(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr)
	: wxScrolledWindow(parent)
	, m_sub_mgr(sub_mgr)
{
	ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_NEVER);

	SetScrollbars(0, 1, 0, 10, 0, 0);
	InitLayout(sub_mgr);

	for (auto& msg : MESSAGES) {
		m_sub_mgr->RegisterObserver(msg, this);
	}
}

WxLayersScrolled::~WxLayersScrolled()
{
	for (auto& msg : MESSAGES) {
		m_sub_mgr->UnregisterObserver(msg, this);
	}
}

void WxLayersScrolled::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case MSG_WND_SCROLL:
		{
			auto var = variants.GetVariant("y");
			GD_ASSERT(var.m_type == ee0::VT_LONG, "err val");
			Scroll(0, var.m_val.l);
		}
		break;
	}
}

void WxLayersScrolled::InitLayout(const ee0::SubjectMgrPtr& sub_mgr)
{
	auto sizer = new wxBoxSizer(wxVERTICAL);
	auto layers = new WxLayersPanel(this, sub_mgr);
	sizer->Add(layers, 1, wxEXPAND);
	SetSizer(sizer);
}

}