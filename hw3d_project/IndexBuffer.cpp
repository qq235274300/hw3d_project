#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(Graphics & gfx, const std::vector<unsigned short>& indices)
	: count(UINT(indices.size()))
{
	INFOMAN(gfx);
	D3D11_BUFFER_DESC  IndexDesc = {};
	IndexDesc.ByteWidth = UINT(sizeof(unsigned short)* count);
	IndexDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexDesc.CPUAccessFlags = 0u;
	IndexDesc.MiscFlags = 0u;
	IndexDesc.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA pIndexData = {};
	pIndexData.pSysMem = indices.data();

	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&IndexDesc, &pIndexData, &pIndicesBuffer));
}

void IndexBuffer::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->IASetIndexBuffer(pIndicesBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

UINT IndexBuffer::GetCount() const noexcept
{
	return count;
}
