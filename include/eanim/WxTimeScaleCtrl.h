#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <anim/Layer.h>

#include <wx/panel.h>

namespace anim { class PlayCtrl; }

namespace eanim
{

class WxTimeScaleCtrl : public wxPanel, public ee0::Observer
{
public:
	WxTimeScaleCtrl(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
		anim::PlayCtrl& pctrl);
	virtual ~WxTimeScaleCtrl();

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

private:
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnMouse(wxMouseEvent& event);

	void OnUpdateNode();
	void OnSetCurrFrame(const ee0::VariantSet& variants);
	void OnWndScroll(const ee0::VariantSet& variants);

private:
	ee0::SubjectMgrPtr m_sub_mgr;

	anim::PlayCtrl& m_pctrl;

	int m_frame_idx;

	int m_start_x;

	DECLARE_EVENT_TABLE()

}; // WxTimeScaleCtrl

}