#pragma once

#include <ee0/typedef.h>

#include <anim/Layer.h>

#include <vector>

namespace eanim
{

class TimeStageEdit
{
public:
	TimeStageEdit(const ee0::SubjectMgrPtr& sub_mgr);

	void OnMouseLeftDown(int row, int col);
	void OnMouseDragging(int row, int col);
	void OnMouseLeftClick(int row, int col);

	void CopySelection();
	void PasteSelection();
	void DeleteSelection();

private:
	void UpdateRegion(int layer, int frame);

private:
	ee0::SubjectMgrPtr m_sub_mgr;

	int m_row, m_col;
	int m_col_min, m_col_max;

}; // TimeStageEdit

}