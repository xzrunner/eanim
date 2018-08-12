#pragma once

#include <anim/Layer.h>

#include <functional>
#include <vector>

namespace eanim
{

class Callback
{
public:
	struct Funs
	{
		std::function<void()>                               refresh_all_nodes;
		std::function<const std::vector<anim::LayerPtr>&()> get_all_layers;
		std::function<void(anim::LayerPtr&)>                add_layer;
		std::function<void(int from, int to)>               swap_layer;
		std::function<bool()>                               remove_all_layers;
	};

	static void RegisterCallback(const Funs& funs);

	//////////////////////////////////////////////////////////////////////////

	static void RefreshAllNodes();

	static const std::vector<anim::LayerPtr>& GetAllLayers();
	static void AddLayer(anim::LayerPtr& layer);
	static void SwapLayer(int from, int to);
	static bool RemoveAllLayers();

}; // Callback

}