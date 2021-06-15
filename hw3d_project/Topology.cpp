#include "Topology.h"

Topology::Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY IntopoType)
	:topoType(IntopoType)
{
	
}

void Topology::Bind(Graphics & gfx)noexcept
{
	GetContext(gfx)->IASetPrimitiveTopology(topoType);
}

