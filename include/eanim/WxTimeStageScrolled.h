#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <wx/scrolwin.h>

namespace anim { class PlayCtrl; }

namespace eanim
{

class WxTimeStageScrolled : public wxScrolledWindow
{
public:
	WxTimeStageScrolled(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
		const anim::PlayCtrl& pctrl);

private:
	void InitLayout(const ee0::SubjectMgrPtr& sub_mgr, const anim::PlayCtrl& pctrl);

	void OnScroll(wxScrollWinEvent& event);

private:
	ee0::SubjectMgrPtr m_sub_mgr;

	DECLARE_EVENT_TABLE()

}; // WxTimeStageScrolled

}