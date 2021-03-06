#include "eanim/WxTimeScaleCtrl.h"
#include "eanim/config.h"
#include "eanim/MessageID.h"
#include "eanim/AnimHelper.h"
#include "eanim/MsgHelper.h"
#include "eanim/Callback.h"

#include <ee0/VariantSet.h>
#include <ee0/SubjectMgr.h>

#include <guard/check.h>
#include <node2/CompAnim.h>
#include <node2/CompAnimInst.h>

#include <wx/dcbuffer.h>

namespace
{

static const int DIVISION_HEIGHT = 4;
static const int TEXT_Y = 4;

const uint32_t MESSAGES[] =
{
	ee0::MSG_UPDATE_NODES,
	eanim::MSG_SET_CURR_FRAME,
	eanim::MSG_WND_SCROLL,
};

}

namespace eanim
{

BEGIN_EVENT_TABLE(WxTimeScaleCtrl, wxPanel)
	EVT_PAINT(WxTimeScaleCtrl::OnPaint)
	EVT_SIZE(WxTimeScaleCtrl::OnSize)
	EVT_MOUSE_EVENTS(WxTimeScaleCtrl::OnMouse)
END_EVENT_TABLE()

WxTimeScaleCtrl::WxTimeScaleCtrl(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
	                             anim::PlayCtrl& pctrl)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1, FRAME_GRID_HEIGHT))
	, m_sub_mgr(sub_mgr)
	, m_pctrl(pctrl)
	, m_frame_idx(0)
	, m_start_x(0)
{
	for (auto& msg : MESSAGES) {
		m_sub_mgr->RegisterObserver(msg, this);
	}
}

WxTimeScaleCtrl::~WxTimeScaleCtrl()
{
	for (auto& msg : MESSAGES) {
		m_sub_mgr->UnregisterObserver(msg, this);
	}
}

void WxTimeScaleCtrl::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_UPDATE_NODES:
		OnUpdateNode();
		break;
	case eanim::MSG_SET_CURR_FRAME:
		OnSetCurrFrame(variants);
		break;
	case eanim::MSG_WND_SCROLL:
		OnWndScroll(variants);
		break;
	}
}

void WxTimeScaleCtrl::OnSize(wxSizeEvent& event)
{
	Refresh(false);
}

void WxTimeScaleCtrl::OnPaint(wxPaintEvent& event)
{
	wxBufferedPaintDC dc(this);

	// background
	dc.SetPen(wxPen(MEDIUM_GRAY));
	dc.SetBrush(wxBrush(MEDIUM_GRAY));
	dc.DrawRectangle(GetSize());

	// curr pos
	if (m_frame_idx != -1)
	{
		int curr_pos = m_frame_idx;
		dc.SetPen(wxPen(DARK_RED));
		dc.SetBrush(wxBrush(MEDIUM_RED));
		dc.DrawRectangle(FRAME_GRID_WIDTH * curr_pos - m_start_x, 2, FRAME_GRID_WIDTH + 1, FRAME_GRID_HEIGHT - 2);
		dc.DrawLine(FRAME_GRID_WIDTH * (curr_pos + 0.5f) - m_start_x, FRAME_GRID_HEIGHT, FRAME_GRID_WIDTH * (curr_pos + 0.5f) - m_start_x, 100);
		if (curr_pos % 5 != 0)
		{
			wxSize size = dc.GetTextExtent(wxString::Format(wxT("%d"), curr_pos));
			float x = FRAME_GRID_WIDTH * (curr_pos + 0.5f) - size.GetWidth() / 2;
			dc.DrawText(wxString::Format(wxT("%d"), curr_pos), x - m_start_x, TEXT_Y);
		}
	}

	// scale
	dc.SetPen(wxPen(TEXT_COLOR));
	for (int i = 0; i <= MAX_FRAME_COUNT; ++i)
	{
		dc.DrawLine(FRAME_GRID_WIDTH * i - m_start_x, FRAME_GRID_HEIGHT, FRAME_GRID_WIDTH * i - m_start_x, FRAME_GRID_HEIGHT - DIVISION_HEIGHT);
		if (i % 5 == 0)
		{
			wxSize size = dc.GetTextExtent(wxString::Format(wxT("%d"), i));
			float x = FRAME_GRID_WIDTH * (i + 0.5f) - size.GetWidth() / 2;
			dc.DrawText(wxString::Format(wxT("%d"), i), x - m_start_x, TEXT_Y);
		}
	}
}

void WxTimeScaleCtrl::OnMouse(wxMouseEvent& event)
{
	if (event.LeftDown() || event.Dragging())
	{
		int x = event.GetX() + m_start_x;
		int frame = (int)(x / FRAME_GRID_WIDTH);
		frame = std::min(eanim::AnimHelper::GetMaxFrame(Callback::GetAllLayers()), frame);
		if (m_frame_idx != frame)
		{
			m_frame_idx = frame;
			Refresh(false);

			eanim::MsgHelper::SetCurrFrame(*m_sub_mgr, -1, frame);
		}
	}
}

void WxTimeScaleCtrl::OnUpdateNode()
{
	int frame_idx = eanim::AnimHelper::GetCurrFrame(Callback::GetAllLayers(), m_pctrl);
	if (frame_idx != m_frame_idx)
	{
		m_frame_idx = frame_idx;
		Refresh(false);
	}
}

void WxTimeScaleCtrl::OnSetCurrFrame(const ee0::VariantSet& variants)
{
	int layer, frame;

	auto var_layer = variants.GetVariant("layer");
	GD_ASSERT(var_layer.m_type == ee0::VT_INT, "err layer");
	layer = var_layer.m_val.l;

	auto var_frame = variants.GetVariant("frame");
	GD_ASSERT(var_frame.m_type == ee0::VT_INT, "err frame");
	frame = var_frame.m_val.l;

	if (layer == -1 && frame == -1)
	{
		m_frame_idx = -1;
	}
	else
	{
		int max_frame = eanim::AnimHelper::GetMaxFrame(Callback::GetAllLayers(), layer);
		if (max_frame >= 0) {
			m_frame_idx = std::min(max_frame, frame);
		}
	}
	Refresh(false);
}

void WxTimeScaleCtrl::OnWndScroll(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("x");
	GD_ASSERT(var.m_type == ee0::VT_LONG, "err val");
	m_start_x = var.m_val.l;
	Refresh(false);
}

}