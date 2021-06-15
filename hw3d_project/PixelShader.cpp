#include "PixelShader.h"

PixelShader::PixelShader(Graphics& gfx, const std::wstring& filePath)
{
	INFOMAN(gfx);
	GFX_THROW_INFO(D3DReadFileToBlob(filePath.c_str(), &pBlobByte));
	GetDevice(gfx)->CreatePixelShader(pBlobByte->GetBufferPointer(),
		pBlobByte->GetBufferSize(),
		nullptr,
		&pPSShader);

}

void PixelShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShader(pPSShader.Get(), nullptr, 0u);

}

ID3DBlob * PixelShader::GetByteBlob() const noexcept
{
	return pBlobByte.Get();
}

