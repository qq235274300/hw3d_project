#include "VertexShader.h"
VertexShader::VertexShader(Graphics& gfx, const std::wstring& filePath)
{
	INFOMAN(gfx);
	GFX_THROW_INFO(D3DReadFileToBlob(filePath.c_str(), &pBlobByte));
	GetDevice(gfx)->CreateVertexShader(pBlobByte->GetBufferPointer(),
		pBlobByte->GetBufferSize(), 
		nullptr,
		&pVSShader);
}
void VertexShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->VSSetShader(pVSShader.Get(), nullptr, 0u);
}
ID3DBlob* VertexShader::GetByteBlob() const noexcept
{
	return pBlobByte.Get();
}

