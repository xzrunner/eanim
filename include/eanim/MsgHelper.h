#pragma once

namespace ee0 { class SubjectMgr; }

namespace eanim
{

class MsgHelper
{
public:
	static void SetSelectedRegion(ee0::SubjectMgr& sub_mgr, int col);
	static void SetCurrFrame(ee0::SubjectMgr& sub_mgr, int layer, int frame);

}; // MsgHelper

}