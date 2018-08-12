#pragma once

#include <ee0/GameObj.h>

#include <anim/Layer.h>

#include <vector>

namespace anim { class KeyFrame; class Layer; class PlayCtrl; }
namespace ee0 { class SubjectMgr; }

namespace eanim
{

class AnimHelper
{
public:
	static anim::Layer* GetLayer(const std::vector<anim::LayerPtr>& layers, int layer_idx);
	static anim::KeyFrame* GetKeyFrame(const std::vector<anim::LayerPtr>& layers,
		int layer_idx, int frame_idx);

	static int GetMaxFrame(const std::vector<anim::LayerPtr>& layers);
	static int GetMaxFrame(const std::vector<anim::LayerPtr>& layers, int layer_idx);

	static int GetCurrFrame(const std::vector<anim::LayerPtr>& layers,
		const anim::PlayCtrl& pctrl);

}; // AnimHelper

}