#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <anim/Layer.h>

#include <wx/panel.h>

namespace eanim
{

class WxLayersPanel : public wxPanel, public ee0::Observer
{
public:
	WxLayersPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr);
	virtual ~WxLayersPanel();

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

private:
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void OnMouse(wxMouseEvent& event);

	void OnSetCurrFrame(const ee0::VariantSet& variants);

private:
	ee0::SubjectMgrPtr m_sub_mgr;

	int m_drag_flag_line;

	int m_curr_layer;

	bool m_is_drag_open;
	int m_xpress, m_ypress;

	DECLARE_EVENT_TABLE()

}; // WxLayersPanel

}