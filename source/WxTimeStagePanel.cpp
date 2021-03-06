#include "eanim/WxTimeStagePanel.h"
#include "eanim/config.h"
#include "eanim/MessageID.h"
#include "eanim/MsgHelper.h"
#include "eanim/AnimHelper.h"
#include "eanim/Callback.h"

#include <ee0/GameObj.h>
#include <ee0/VariantSet.h>
#include <ee0/SubjectMgr.h>

#include <guard/check.h>
#include <anim/KeyFrame.h>
#include <anim/Layer.h>
#include <anim/KeyFrame.h>

#include <wx/dcbuffer.h>
#include <wx/menu.h>

namespace
{

const int KEY_FRAME_CIRCLE_Y_OFFSET = eanim::FRAME_GRID_WIDTH * 0.5f + 1;
const int KEY_FRAME_CIRCLE_RADIUS   = eanim::FRAME_GRID_WIDTH * 0.4f;
const int KEY_FRAME_RECT_Y_OFFSET   = KEY_FRAME_CIRCLE_Y_OFFSET + 1;
const int FRAME_END_RECT_WIDTH      = KEY_FRAME_CIRCLE_RADIUS * 2;
const int FRAME_END_RECT_HEIGHT     = KEY_FRAME_CIRCLE_RADIUS * 3;

const uint32_t MESSAGES[] =
{
	ee0::MSG_SCENE_NODE_INSERT,
	ee0::MSG_SCENE_NODE_DELETE,
	ee0::MSG_SCENE_NODE_CLEAR,
	ee0::MSG_SCENE_NODE_UP_DOWN,

	ee0::MSG_UPDATE_NODES,

	eanim::MSG_SET_CURR_FRAME,
	eanim::MSG_SET_SELECTED_REGION,
};

}

namespace eanim
{

BEGIN_EVENT_TABLE(WxTimeStagePanel, wxPanel)
	EVT_PAINT(WxTimeStagePanel::OnPaint)
	EVT_SIZE(WxTimeStagePanel::OnSize)
	EVT_MOUSE_EVENTS(WxTimeStagePanel::OnMouse)
	EVT_KEY_DOWN(WxTimeStagePanel::OnKeyDown)
	EVT_KEY_UP(WxTimeStagePanel::OnKeyUp)

	EVT_MENU(Menu_CreateClassicTween, WxTimeStagePanel::OnCreateClassicTween)
	EVT_MENU(Menu_DeleteClassicTween, WxTimeStagePanel::OnDeleteClassicTween)
	EVT_MENU(Menu_InsertFrame, WxTimeStagePanel::OnInsertFrame)
	EVT_MENU(Menu_DeleteFrame, WxTimeStagePanel::OnDeleteFrame)
	EVT_MENU(Menu_InsertKeyFrame, WxTimeStagePanel::OnInsertKeyFrame)
	EVT_MENU(Menu_DeleteKeyFrame, WxTimeStagePanel::OnDeleteKeyFrame)

	EVT_UPDATE_UI(Menu_CreateClassicTween, WxTimeStagePanel::OnUpdateCreateClassicTween)
	EVT_UPDATE_UI(Menu_DeleteClassicTween, WxTimeStagePanel::OnUpdateDeleteClassicTween)
	EVT_UPDATE_UI(Menu_InsertFrame, WxTimeStagePanel::OnUpdateInsertFrame)
	EVT_UPDATE_UI(Menu_DeleteFrame, WxTimeStagePanel::OnUpdateDeleteFrame)
	EVT_UPDATE_UI(Menu_InsertKeyFrame, WxTimeStagePanel::OnUpdateInsertKeyFrame)
	EVT_UPDATE_UI(Menu_DeleteKeyFrame, WxTimeStagePanel::OnUpdateDeleteKeyFrame)
END_EVENT_TABLE()

eanim::LanguageEntry WxTimeStagePanel::entries[] =
{
	{ "������ͳ����", "Create Classic Tween" },
	{ "ɾ����ͳ����", "Delete Classic Tween" },
	{ "����֡", "Insert Frame" },
	{ "ɾ��֡", "Delete Frame" },
	{ "����ؼ�֡", "Insert Key Frame" },
	{ "ɾ���ؼ�֡", "Delete Key Frame" }
};

WxTimeStagePanel::WxTimeStagePanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
	                               const anim::PlayCtrl& pctrl)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(5000, 999))
	, m_sub_mgr(sub_mgr)
	, m_pctrl(pctrl)
	, m_editop(sub_mgr)
{
	m_layer_idx = m_frame_idx = m_valid_frame_idx = -1;
	m_col_min = m_col_max = -1;

	for (auto& msg : MESSAGES) {
		m_sub_mgr->RegisterObserver(msg, this);
	}
}

WxTimeStagePanel::~WxTimeStagePanel()
{
	for (auto& msg : MESSAGES) {
		m_sub_mgr->UnregisterObserver(msg, this);
	}
}

void WxTimeStagePanel::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	bool dirty = false;
	switch (msg)
	{
	case ee0::MSG_SCENE_NODE_INSERT:
		dirty = InsertSceneNode(variants);
		break;
	case ee0::MSG_SCENE_NODE_DELETE:
		dirty = DeleteSceneNode(variants);
		break;
	case ee0::MSG_SCENE_NODE_CLEAR:
		dirty = ClearSceneNode();
		break;
	case ee0::MSG_SCENE_NODE_UP_DOWN:
		dirty = ReorderSceneNode(variants);
		break;

	case ee0::MSG_UPDATE_NODES:
		OnUpdateNode();
		break;

	case eanim::MSG_SET_CURR_FRAME:
		OnSetCurrFrame(variants);
		break;
	case eanim::MSG_SET_SELECTED_REGION:
		OnSetSelectedRegion(variants);
		break;
	}

	if (dirty) {
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
		Refresh(false);
	}
}

void WxTimeStagePanel::OnSize(wxSizeEvent& event)
{
	Refresh(false);
}

void WxTimeStagePanel::OnPaint(wxPaintEvent& event)
{
	wxBufferedPaintDC dc(this);

	DrawBackground(dc);
	DrawLayersDataBg(dc);
	DrawSelected(dc);
	DrawLayersDataFlag(dc);
	DrawCurrPosFlag(dc);
}

void WxTimeStagePanel::OnMouse(wxMouseEvent& event)
{
	int row = event.GetY() / eanim::FRAME_GRID_HEIGHT,
		col = event.GetX() / eanim::FRAME_GRID_WIDTH;

	if (event.RightDown()) {
		m_editop.OnMouseLeftDown(row, col);
		MousePopupMenu(event.GetX(), event.GetY());
	} else if (event.LeftDown()) {
		m_editop.OnMouseLeftDown(row, col);
	} else if (event.Dragging()) {
		m_editop.OnMouseDragging(row, col);
	} else if (event.Moving()) {
		SetFocus();
	} else if (event.LeftDClick()) {
		m_editop.OnMouseLeftClick(row, col);
	}
}

void WxTimeStagePanel::OnKeyDown(wxKeyEvent& event)
{

}

void WxTimeStagePanel::OnKeyUp(wxKeyEvent& event)
{

}

void WxTimeStagePanel::DrawBackground(wxBufferedPaintDC& dc)
{
	// background
	dc.SetPen(wxPen(eanim::LIGHT_GRAY));
	dc.SetBrush(wxBrush(eanim::LIGHT_GRAY));
	dc.DrawRectangle(GetSize());

	auto& layers = Callback::GetAllLayers();
	const size_t size = layers.size();
	const float width = eanim::FRAME_GRID_WIDTH * eanim::MAX_FRAME_COUNT,
		        height = eanim::FRAME_GRID_HEIGHT * size;

	// white grids
	dc.SetPen(*wxWHITE_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);
 	dc.DrawRectangle(0, 0, width, height);

	// 5 times gray grids
	dc.SetPen(wxPen(eanim::LIGHT_GRAY));
	dc.SetBrush(wxBrush(eanim::LIGHT_GRAY));
	for (size_t i = 0; i < eanim::MAX_FRAME_COUNT; ++i)
	{
		if (i % 5 == 0)
		{
			const float x = eanim::FRAME_GRID_WIDTH * i;
			dc.DrawRectangle(x, 0, eanim::FRAME_GRID_WIDTH, height);
		}
	}

	// grid lines
	dc.SetPen(wxPen(eanim::MEDIUM_GRAY));
	for (size_t i = 0; i <= size; ++i)
	{
		const float y = eanim::FRAME_GRID_HEIGHT * i;
		dc.DrawLine(0, y, width, y);
	}
	for (size_t i = 0; i <= eanim::MAX_FRAME_COUNT; ++i)
	{
		const float x = eanim::FRAME_GRID_WIDTH * i;
		dc.DrawLine(x, 0, x, height);
	}
}

void WxTimeStagePanel::DrawLayersDataBg(wxBufferedPaintDC& dc)
{
	auto& layers = Callback::GetAllLayers();
	for (size_t i = 0, n = layers.size(); i < n; ++i)
	{
		size_t idx = n - i - 1;
		auto& frames = layers[idx]->GetAllKeyFrames();
		// during
		for (auto itr = frames.begin(); itr != frames.end(); ++itr)
		{
			auto next = itr;
			++next;
			float x = eanim::FRAME_GRID_WIDTH * (*itr)->GetFrameIdx(),
				  y = eanim::FRAME_GRID_HEIGHT * i;
			float width = eanim::FRAME_GRID_WIDTH + 1,
				  height = eanim::FRAME_GRID_HEIGHT + 1;
			if (next != frames.end()) {
				width = std::max(eanim::FRAME_GRID_WIDTH * ((*next)->GetFrameIdx() - (*itr)->GetFrameIdx()), eanim::FRAME_GRID_WIDTH) + 1;
			}
			dc.SetPen(*wxBLACK_PEN);
//			dc.SetBrush(*wxWHITE_BRUSH);
			dc.SetBrush(wxColour(255, 255, 255, 128));
			dc.DrawRectangle(x, y, width, height);
		}
		// Classic Tween
		for (auto itr = frames.begin(); itr != frames.end(); ++itr)
		{
			if ((*itr)->GetTween())
			{
				auto next = itr;
				++next;
				if (next != frames.end())
				{
					float x = eanim::FRAME_GRID_WIDTH * (*itr)->GetFrameIdx(),
						  y = eanim::FRAME_GRID_HEIGHT * i;
					float width = eanim::FRAME_GRID_WIDTH * ((*next)->GetFrameIdx() - (*itr)->GetFrameIdx()) + 1,
						  height = eanim::FRAME_GRID_HEIGHT + 1;

					dc.SetPen(*wxBLACK_PEN);
					dc.SetBrush(wxBrush(eanim::CLASSIC_TWEEN_COLOR));
					dc.DrawRectangle(x, y, width, height);
				}
			}
		}
	}
}

void WxTimeStagePanel::DrawLayersDataFlag(wxBufferedPaintDC& dc)
{
	auto& layers = Callback::GetAllLayers();
	for (size_t i = 0, n = layers.size(); i < n; ++i)
	{
		size_t idx = n - i - 1;
		auto& frames = layers[idx]->GetAllKeyFrames();
		// key frame start (circle)
		for (auto& frame : frames)
		{
			float x = eanim::FRAME_GRID_WIDTH * (frame->GetFrameIdx() + 0.5f),
				  y = eanim::FRAME_GRID_HEIGHT * (i + 1) - KEY_FRAME_CIRCLE_Y_OFFSET;
			dc.SetPen(*wxBLACK_PEN);
			if (frame->GetAllNodes().empty()) {
				dc.SetBrush(*wxWHITE_BRUSH);
			} else {
				dc.SetBrush(*wxBLACK_BRUSH);
			}
			dc.DrawCircle(x, y, KEY_FRAME_CIRCLE_RADIUS);
		}
		// key frame end (rect)
		for (auto itr = frames.begin(); itr != frames.end(); ++itr)
		{
			if (itr == frames.begin()) {
				continue;
			}

			auto prev = itr;
			--prev;
			if ((*prev)->GetFrameIdx() == (*itr)->GetFrameIdx() - 1) {
				continue;
			}

			float x = eanim::FRAME_GRID_WIDTH * ((*itr)->GetFrameIdx() - 0.5f) - FRAME_END_RECT_WIDTH * 0.5f,
				  y = eanim::FRAME_GRID_HEIGHT * (i + 1) - KEY_FRAME_RECT_Y_OFFSET - FRAME_END_RECT_HEIGHT * 0.5f;
			dc.SetPen(*wxBLACK_PEN);
			dc.SetBrush(*wxWHITE_BRUSH);
			dc.DrawRectangle(x, y, FRAME_END_RECT_WIDTH, FRAME_END_RECT_HEIGHT);
		}
	}
}

void WxTimeStagePanel::DrawCurrPosFlag(wxBufferedPaintDC& dc)
{
	const float x = eanim::FRAME_GRID_WIDTH * (m_valid_frame_idx + 0.5f);
	dc.SetPen(eanim::DARK_RED);
	dc.DrawLine(x, 0, x, eanim::FRAME_GRID_HEIGHT * Callback::GetAllLayers().size());
}

void WxTimeStagePanel::DrawSelected(wxBufferedPaintDC& dc)
{
	if (m_layer_idx == -1) {
		return;
	}
	auto& layers = Callback::GetAllLayers();
	int row = layers.size() - 1 - m_layer_idx;
	if (m_col_min != -1 && m_col_max != -1)
	{
		dc.SetPen(wxPen(eanim::DARK_BLUE));
		dc.SetBrush(wxBrush(eanim::DARK_BLUE));
		dc.DrawRectangle(
			eanim::FRAME_GRID_WIDTH * m_col_min,
			eanim::FRAME_GRID_HEIGHT * row,
			eanim::FRAME_GRID_WIDTH * (m_col_max - m_col_min + 1),
			eanim::FRAME_GRID_HEIGHT
		);
	}
	else
	{
		dc.SetPen(wxPen(eanim::DARK_BLUE));
		dc.SetBrush(wxBrush(eanim::DARK_BLUE));
		dc.DrawRectangle(
			eanim::FRAME_GRID_WIDTH * m_frame_idx,
			eanim::FRAME_GRID_HEIGHT * row,
			eanim::FRAME_GRID_WIDTH,
			eanim::FRAME_GRID_HEIGHT
		);
	}
}

bool WxTimeStagePanel::InsertSceneNode(const ee0::VariantSet& variants)
{
#ifndef GAME_OBJ_ECS
	auto frame = eanim::AnimHelper::GetKeyFrame(Callback::GetAllLayers(), m_layer_idx, m_frame_idx);
	if (!frame) {
		return false;
	}

	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	frame->AddNode(*obj);
	m_sub_mgr->NotifyObservers(eanim::MSG_REFRESH_ANIM_COMP);
#else
	// todo ecs
#endif // GAME_OBJ_ECS

	return true;
}

bool WxTimeStagePanel::DeleteSceneNode(const ee0::VariantSet& variants)
{
#ifndef GAME_OBJ_ECS
	auto frame = eanim::AnimHelper::GetKeyFrame(Callback::GetAllLayers(), m_layer_idx, m_frame_idx);
	if (!frame) {
		return false;
	}

	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	bool ret = frame->RemoveNode(*obj);
	m_sub_mgr->NotifyObservers(eanim::MSG_REFRESH_ANIM_COMP);
	return ret;
#else
	// todo ecs
	return false;
#endif // GAME_OBJ_ECS
}

bool WxTimeStagePanel::ClearSceneNode()
{
	bool ret = Callback::RemoveAllLayers();
	m_sub_mgr->NotifyObservers(eanim::MSG_REFRESH_ANIM_COMP);
	return ret;
}

bool WxTimeStagePanel::ReorderSceneNode(const ee0::VariantSet& variants)
{
#ifndef GAME_OBJ_ECS
	auto frame = eanim::AnimHelper::GetKeyFrame(Callback::GetAllLayers(), m_layer_idx, m_frame_idx);
	if (!frame) {
		return false;
	}

	auto obj_var = variants.GetVariant("obj");
	GD_ASSERT(obj_var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(obj_var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	auto up_var = variants.GetVariant("up");
	GD_ASSERT(up_var.m_type == ee0::VT_BOOL, "no var in vars: up");
	bool up = up_var.m_val.bl;

	std::vector<ee0::GameObj> all_objs = frame->GetAllNodes();
	if (all_objs.empty()) {
		return false;
	}

	int idx = -1;
	for (int i = 0, n = all_objs.size(); i < n; ++i)
	{
		if (all_objs[i] == *obj) {
			idx = i;
			break;
		}
	}

	GD_ASSERT(idx >= 0, "not find");

	bool ret = false;
	if (up && idx != all_objs.size() - 1)
	{
		std::swap(all_objs[idx], all_objs[idx + 1]);
		frame->SetNodes(all_objs);
		ret = true;
	}
	else if (!up && idx != 0)
	{
		std::swap(all_objs[idx], all_objs[idx - 1]);
		frame->SetNodes(all_objs);
		ret = true;
	}
	if (ret) {
		m_sub_mgr->NotifyObservers(eanim::MSG_REFRESH_ANIM_COMP);
	}
	return ret;
#else
	// todo ecs
	return false;
#endif // GAME_OBJ_ECS
}

void WxTimeStagePanel::OnSetCurrFrame(const ee0::VariantSet& variants)
{
	int layer, frame;

	auto var_layer = variants.GetVariant("layer");
	GD_ASSERT(var_layer.m_type == ee0::VT_INT, "err layer");
	layer = var_layer.m_val.l;

	auto var_frame = variants.GetVariant("frame");
	GD_ASSERT(var_frame.m_type == ee0::VT_INT, "err frame");
	frame = var_frame.m_val.l;

	bool dirty = false;
	m_col_min = m_col_max = -1;
	if (layer == -1 && frame == -1)
	{
		m_layer_idx = m_frame_idx = m_valid_frame_idx = -1;
		m_col_min = m_col_max = -1;
		dirty = true;
	}
	else
	{
		if (layer != -1 && layer != m_layer_idx)
		{
			m_layer_idx = layer;
			dirty = true;
		}
		if (frame != m_frame_idx)
		{
			if (frame != -1) {
				m_frame_idx = frame;
			}
 			dirty = true;
		}
		if (auto layer = eanim::AnimHelper::GetLayer(Callback::GetAllLayers(), m_layer_idx))
		{
			int max_frame = layer->GetMaxFrameIdx();
			if (max_frame >= 0)
			{
				int valid_frame_idx = std::min(max_frame, m_frame_idx);
				if (valid_frame_idx != m_valid_frame_idx) {
					m_valid_frame_idx = valid_frame_idx;
					dirty = true;
				}
			}
		}
	}

	if (dirty) {
		Refresh(false);
	}
}

void WxTimeStagePanel::OnSetSelectedRegion(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("col");
	GD_ASSERT(var.m_type == ee0::VT_INT, "err col");
	int col = var.m_val.l;

	m_col_min = m_col_max = -1;

	if (auto layer = eanim::AnimHelper::GetLayer(Callback::GetAllLayers(), m_layer_idx))
	{
		int max_frame = layer->GetMaxFrameIdx();
		if (max_frame >= 0)
		{
			m_col_min = std::min(std::min(m_frame_idx, col), max_frame);
			m_col_max = std::min(std::max(m_frame_idx, col), max_frame);
			if (m_col_min == m_col_max) {
				eanim::MsgHelper::SetCurrFrame(*m_sub_mgr, -1, m_col_min);
			}
			Refresh(false);
		}
	}
}

void WxTimeStagePanel::MousePopupMenu(int x, int y)
{
	wxMenu menu;

	menu.Append(Menu_CreateClassicTween, entries[Menu_CreateClassicTween].text[eanim::currLanguage]);
	menu.Append(Menu_DeleteClassicTween, entries[Menu_DeleteClassicTween].text[eanim::currLanguage]);
	menu.AppendSeparator();
	menu.Append(Menu_InsertFrame, entries[Menu_InsertFrame].text[eanim::currLanguage]);
	menu.Append(Menu_DeleteFrame, entries[Menu_DeleteFrame].text[eanim::currLanguage]);
	menu.AppendSeparator();
	menu.Append(Menu_InsertKeyFrame, entries[Menu_InsertKeyFrame].text[eanim::currLanguage]);
	menu.Append(Menu_DeleteKeyFrame, entries[Menu_DeleteKeyFrame].text[eanim::currLanguage]);

	PopupMenu(&menu, x, y);
}

void WxTimeStagePanel::OnCreateClassicTween(wxCommandEvent& event)
{
	if (auto frame = eanim::AnimHelper::GetKeyFrame(Callback::GetAllLayers(), m_layer_idx, m_frame_idx))
	{
		frame->SetTween(true);
		m_sub_mgr->NotifyObservers(eanim::MSG_REFRESH_ANIM_COMP);
		Refresh(false);
	}
}

void WxTimeStagePanel::OnDeleteClassicTween(wxCommandEvent& event)
{
	if (auto frame = eanim::AnimHelper::GetKeyFrame(Callback::GetAllLayers(), m_layer_idx, m_frame_idx))
	{
		frame->SetTween(false);
		m_sub_mgr->NotifyObservers(eanim::MSG_REFRESH_ANIM_COMP);
		Refresh(false);
	}
}

void WxTimeStagePanel::OnInsertFrame(wxCommandEvent& event)
{
	OnInsertFrame();
}

void WxTimeStagePanel::OnDeleteFrame(wxCommandEvent& event)
{
	OnDeleteFrame();
}

void WxTimeStagePanel::OnInsertKeyFrame(wxCommandEvent& event)
{
	auto layer = eanim::AnimHelper::GetLayer(Callback::GetAllLayers(), m_layer_idx);
	if (!layer) {
		return;
	}

	auto new_frame = std::make_unique<::anim::KeyFrame>(m_frame_idx);

	if (!layer->GetAllKeyFrames().empty())
	{
		if (layer->GetMaxFrameIdx() < m_frame_idx)
		{
			// copy from end
			auto end_frame = layer->GetEndFrame();
			if (end_frame)
			{
				*new_frame = *end_frame;
				new_frame->SetFrameIdx(m_frame_idx);
			}
		}
		else
		{
			auto prev = layer->GetPrevKeyFrame(m_frame_idx),
				 next = layer->GetNextKeyFrame(m_frame_idx);
			GD_ASSERT(prev, "err prev");
			// in middle, need tween
			if (prev->GetTween() && next)
			{
				// todo lerp
			}
			else
			{
				*new_frame = *prev;
			}
		}
	}

	layer->InsertKeyFrame(new_frame);

	m_sub_mgr->NotifyObservers(eanim::MSG_REFRESH_ANIM_COMP);

	eanim::MsgHelper::SetCurrFrame(*m_sub_mgr, m_layer_idx, m_frame_idx);

	Refresh(false);
}

void WxTimeStagePanel::OnDeleteKeyFrame(wxCommandEvent& event)
{
	if (auto layer = eanim::AnimHelper::GetLayer(Callback::GetAllLayers(), m_layer_idx))
	{
		layer->RemoveKeyFrame(m_frame_idx);
		m_sub_mgr->NotifyObservers(eanim::MSG_REFRESH_ANIM_COMP);
		Refresh(false);
	}
}

void WxTimeStagePanel::OnUpdateCreateClassicTween(wxUpdateUIEvent& event)
{
	if (auto frame = eanim::AnimHelper::GetKeyFrame(Callback::GetAllLayers(), m_layer_idx, m_frame_idx)) {
		event.Enable(!frame->GetTween());
	} else {
		event.Enable(false);
	}
}

void WxTimeStagePanel::OnUpdateDeleteClassicTween(wxUpdateUIEvent& event)
{
	if (auto frame = eanim::AnimHelper::GetKeyFrame(Callback::GetAllLayers(), m_layer_idx, m_frame_idx)) {
		event.Enable(frame->GetTween());
	} else {
		event.Enable(false);
	}
}

void WxTimeStagePanel::OnUpdateInsertFrame(wxUpdateUIEvent& event)
{
	if (auto frame = eanim::AnimHelper::GetKeyFrame(Callback::GetAllLayers(), m_layer_idx, m_frame_idx)) {
		event.Enable(frame->GetFrameIdx() != m_frame_idx);
	} else {
		event.Enable(false);
	}
}

void WxTimeStagePanel::OnUpdateDeleteFrame(wxUpdateUIEvent& event)
{
	if (auto frame = eanim::AnimHelper::GetKeyFrame(Callback::GetAllLayers(), m_layer_idx, m_frame_idx)) {
		event.Enable(frame->GetFrameIdx() != m_frame_idx);
	} else {
		event.Enable(false);
	}
}

void WxTimeStagePanel::OnUpdateInsertKeyFrame(wxUpdateUIEvent& event)
{
	if (auto layer = eanim::AnimHelper::GetLayer(Callback::GetAllLayers(), m_layer_idx)) {
		event.Enable(!layer->IsKeyFrame(m_frame_idx));
	} else {
		event.Enable(false);
	}
}

void WxTimeStagePanel::OnUpdateDeleteKeyFrame(wxUpdateUIEvent& event)
{
	if (auto layer = eanim::AnimHelper::GetLayer(Callback::GetAllLayers(), m_layer_idx)) {
		event.Enable(layer->IsKeyFrame(m_frame_idx));
	} else {
		event.Enable(false);
	}
}

void WxTimeStagePanel::OnInsertFrame(wxKeyEvent& event)
{
	OnInsertFrame();
}

void WxTimeStagePanel::OnDeleteFrame(wxKeyEvent& event)
{
	OnDeleteFrame();
}

void WxTimeStagePanel::OnInsertFrame()
{
	if (auto layer = eanim::AnimHelper::GetLayer(Callback::GetAllLayers(), m_layer_idx))
	{
		layer->InsertNullFrame(m_frame_idx);
		m_sub_mgr->NotifyObservers(eanim::MSG_REFRESH_ANIM_COMP);
		Refresh(false);
	}
}

void WxTimeStagePanel::OnDeleteFrame()
{
	if (auto layer = eanim::AnimHelper::GetLayer(Callback::GetAllLayers(), m_layer_idx))
	{
		layer->RemoveNullFrame(m_frame_idx);
		m_sub_mgr->NotifyObservers(eanim::MSG_REFRESH_ANIM_COMP);
		Refresh(false);
	}
}

void WxTimeStagePanel::OnUpdateNode()
{
	int frame_idx = eanim::AnimHelper::GetCurrFrame(Callback::GetAllLayers(), m_pctrl);
	if (frame_idx != m_frame_idx)
	{
		m_frame_idx = frame_idx;
		m_valid_frame_idx = frame_idx;
		Refresh(false);
	}
}

}