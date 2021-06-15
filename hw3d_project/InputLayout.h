#pragma once
#include "Bindable.h"

class InputLayout : public Bindable
{
public:
	InputLayout(Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& elementDesc, ID3DBlob* VSBlob);
	void Bind(Graphics& gfx)noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;



};

