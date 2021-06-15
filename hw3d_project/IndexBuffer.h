#pragma once
#include "Bindable.h"
#include <vector>

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices);
	void Bind(Graphics& gfx)noexcept override;
	UINT GetCount()const noexcept;
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndicesBuffer;
	UINT count = 0u;
};

