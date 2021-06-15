#pragma once
#include "Bindable.h"

class VertexShader : public Bindable
{
public:
	VertexShader(Graphics& gfx, const std::wstring& filePath);
	void Bind(Graphics& gfx)noexcept override;	
	ID3DBlob* GetByteBlob()const noexcept;
protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVSShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlobByte;
};

