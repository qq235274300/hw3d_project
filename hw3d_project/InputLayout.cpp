#include "InputLayout.h"

InputLayout::InputLayout(Graphics & gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& elementDesc, ID3DBlob* VSBlob)
{
	INFOMAN(gfx);
	
	GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(elementDesc.data(), 
		(UINT)elementDesc.size(), 
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(), &pInputLayout));
}

void InputLayout::Bind(Graphics & gfx) noexcept
{
	GetContext(gfx)->IASetInputLayout(pInputLayout.Get());
}
