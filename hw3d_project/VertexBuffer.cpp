#include "VertexBuffer.h"

void VertexBuffer::Bind(Graphics& gfx)noexcept
{
	
	GetContext(gfx)->IASetVertexBuffers
	(
		0u,//有限的VertextBuffer数量
		1u,
		pVertexBuffer.GetAddressOf(),
		&strideByte,
		&offset
	);
}

