#include "eanim/AnimHelper.h"

//#include "frame/GameObjFactory.h"
//#include "frame/MessageID.h"

#include <ee0/SubjectMgr.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <anim/Layer.h>
#include <anim/KeyFrame.h>
#include <anim/Utility.h>
#include <anim/PlayCtrl.h>

namespace eanim
{

anim::Layer* AnimHelper::GetLayer(const std::vector<anim::LayerPtr>& layers, int layer_idx)
{
	if (layer_idx >= 0 && static_cast<size_t>(layer_idx) < layers.size()) {
		return layers[layer_idx].get();
	} else {
		return nullptr;
	}
}

anim::KeyFrame* AnimHelper::GetKeyFrame(const std::vector<anim::LayerPtr>& layers,
	                                    int layer_idx, int frame_idx)
{
	auto layer = GetLayer(layers, layer_idx);
	if (!layer) {
		return nullptr;
	} else {
		return layer->GetCurrKeyFrame(frame_idx);
	}
}

int AnimHelper::GetMaxFrame(const std::vector<anim::LayerPtr>& layers)
{
	return anim::Utility::GetMaxFrame(layers);
}

int AnimHelper::GetMaxFrame(const std::vector<anim::LayerPtr>& layers, int layer_idx)
{
	if (layer_idx < 0 || static_cast<size_t>(layer_idx) >= layers.size()) {
		return -1;
	}

	auto& layer = layers[layer_idx];
	auto& frames = layer->GetAllKeyFrames();
	if (frames.empty()) {
		return -1;
	} else {
		return frames.back()->GetFrameIdx();
	}
}

int AnimHelper::GetCurrFrame(const std::vector<anim::LayerPtr>& layers,
	                         const anim::PlayCtrl& pctrl)
{
	int frame_idx = pctrl.GetFrame();
	int max_frame = GetMaxFrame(layers);
	if (max_frame > 0) {
		frame_idx = frame_idx % (max_frame + 1);
	}
	return frame_idx;
}

}