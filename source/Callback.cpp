#include "eanim/Callback.h"

namespace
{

eanim::Callback::Funs FUNS;

}

namespace eanim
{

void Callback::RegisterCallback(const Callback::Funs& funs)
{
	FUNS = funs;
}

void Callback::RefreshAllNodes()
{
	return FUNS.refresh_all_nodes();
}

const std::vector<anim::LayerPtr>& Callback::GetAllLayers()
{
	return FUNS.get_all_layers();
}

void Callback::AddLayer(anim::LayerPtr& layer)
{
	FUNS.add_layer(layer);
}

void Callback::SwapLayer(int from, int to)
{
	FUNS.swap_layer(from, to);
}

bool Callback::RemoveAllLayers()
{
	return FUNS.remove_all_layers();
}

}