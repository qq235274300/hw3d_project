#include "VertexBuffer.h"

void VertexBuffer::Bind(Graphics& gfx)noexcept
{
	
	GetContext(gfx)->IASetVertexBuffers
	(
		0u,//���޵�VertextBuffer����
		1u,
		pVertexBuffer.GetAddressOf(),
		&strideByte,
		&offset
	);
}

