#pragma once

#include <ee0/typedef.h>

#include <wx/panel.h>

namespace anim { class PlayCtrl; }

namespace eanim
{

class WxTimelinePanel : public wxPanel
{
public:
	WxTimelinePanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
		anim::PlayCtrl& pctrl);

private:
	void InitLayout(const ee0::SubjectMgrPtr& sub_mgr, anim::PlayCtrl& pctrl);

}; // WxTimelinePanel

}