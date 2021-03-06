#pragma once

#include "eanim/WxLayersPanel.h"

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <wx/scrolwin.h>

namespace eanim
{

class WxLayersScrolled : public wxScrolledWindow, public ee0::Observer
{
public:
	WxLayersScrolled(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr);
	virtual ~WxLayersScrolled();

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

private:
	void InitLayout(const ee0::SubjectMgrPtr& sub_mgr);

private:
	ee0::SubjectMgrPtr m_sub_mgr;

}; // WxLayersScrolled

}