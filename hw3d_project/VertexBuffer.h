#pragma once
#include "Bindable.h"

class VertexBuffer : public Bindable
{
public:
	template<typename C>
	VertexBuffer(Graphics& gfx, const std::vector<C>& vertices)
		:strideByte(sizeof(C))
	{
		
		INFOMAN(gfx);
		D3D11_BUFFER_DESC  pDesc = {};
		pDesc.ByteWidth = UINT(sizeof(C) * vertices.size());
		pDesc.Usage = D3D11_USAGE_DEFAULT;
		pDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		pDesc.CPUAccessFlags = 0u;
		pDesc.MiscFlags = 0u;
		pDesc.StructureByteStride = strideByte;

		D3D11_SUBRESOURCE_DATA pData = {};
		pData.pSysMem = vertices.data(); //Pointer to the initialization data.
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&pDesc, &pData, &pVertexBuffer));
	}

	void Bind(Graphics& gfx)noexcept override;

protected:
	UINT strideByte = 0u;
	UINT offset = 0u;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	
};

