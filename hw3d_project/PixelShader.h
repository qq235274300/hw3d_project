#pragma once
#include "Bindable.h"

class PixelShader : public Bindable
{
public:
	PixelShader(Graphics& gfx, const std::wstring& filePath);
	void Bind(Graphics& gfx)noexcept override;
	ID3DBlob* GetByteBlob()const noexcept;
protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPSShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlobByte;
};

