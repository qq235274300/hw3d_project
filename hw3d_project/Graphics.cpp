#include "Graphics.h"
#include "dxerr.h"
#include <sstream>



 namespace wrl = Microsoft::WRL;
 namespace dx = DirectX;
//d3d11.lib
//d3dcompiler.lib
#pragma comment ( lib,"d3d11.lib" )
#pragma comment(lib,"D3DCompiler.lib")

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.RefreshRate.Numerator = 0 ;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Format  = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;  // 1 fornt  1 back ...
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	// for checking results of d3d functions
	HRESULT hr;

	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr, // default adpater
		D3D_DRIVER_TYPE_HARDWARE, // 硬件加速模式
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	)) ;

	//access swap backbuffer,Create render target view
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pRenderTargetView));
	
	//Create Depth-Stencil State
	wrl::ComPtr<ID3D11DepthStencilState> pDSS;
	D3D11_DEPTH_STENCIL_DESC  dssDesc; 
	dssDesc.DepthEnable = TRUE;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dssDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dssDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dssDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dssDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	GFX_THROW_INFO(pDevice->CreateDepthStencilState(&dssDesc,&pDSS));
	pContext->OMSetDepthStencilState(pDSS.Get(), 1u);
	
	wrl::ComPtr<ID3D11Texture2D> pTexture2D;
	D3D11_TEXTURE2D_DESC  pTdesc = {};
	pTdesc.Width = 800u;
	pTdesc.Height = 600u;
	pTdesc.MipLevels = 1u;
	pTdesc.ArraySize = 1u;
	pTdesc.Format = DXGI_FORMAT_D32_FLOAT;
	//抗锯齿关闭
	pTdesc.SampleDesc.Count = 1u;
	pTdesc.SampleDesc.Quality = 0u;
	pTdesc.Usage = D3D11_USAGE_DEFAULT;
	pTdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	pTdesc.CPUAccessFlags = 0;
	pTdesc.MiscFlags = 0;

	GFX_THROW_INFO(pDevice->CreateTexture2D(&pTdesc,nullptr, &pTexture2D));

	
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;

	GFX_THROW_INFO(pDevice->CreateDepthStencilView(pTexture2D.Get(), &descDSV, &pDSV));
	
	pContext->OMSetRenderTargets(1u, pRenderTargetView.GetAddressOf(), pDSV.Get());


	D3D11_VIEWPORT vp;
	vp.Width = 800.0f;
	vp.Height = 600.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);
}


void Graphics::EndFrame()
{
	//1u 表示已当前CPU赫兹 进行刷新，2u 为一半
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	if (FAILED(hr = pSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}

void Graphics::DrawIndexed(UINT count) 
{
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}

void Graphics::ClearBuffer(float red, float green, float blue)
{
	const float Color[] = { red,green,blue,1.0f };
	pContext->ClearRenderTargetView(pRenderTargetView.Get(), Color);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH,1u,1u);
}

void Graphics::DrawTestTriangle(float angel,float posx, float posz)
{
	HRESULT hr;
	struct Vertex
	{
		struct 
		{
			float x;
			float y;
			float z;
		}pos;	
	};

	const Vertex vertexs[] =
	{
		//顺时针  
		/*{0.f,0.5f,255,0,0,0},
		{0.5,-0.5f,0,255,0,0},
		{-0.5f,-0.5f,0,0,255,0},

		{-0.3f,0.3f,0,255,0,0},
		{0.3f,0.3f,0,0,255,0},
		{0.0f,-1.0f,255,0,0,0},*/


		{ -.5f, 0.5f, -0.5f },
		{ 0.5f, 0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ 0.5f, -0.5f, -0.5f },
		{ -0.5f, 0.5f, 0.5f },
		{ 0.5f, 0.5f, 0.5f},
		{ -0.5f, -0.5f,0.5f },
		{ 0.5f, -0.5f, 0.5f },
	};
	
	const UINT strideByte = sizeof(Vertex);
	const UINT offset = 0u;
	const UINT DataByte = sizeof(vertexs);

	namespace wrl = Microsoft::WRL;
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	
	D3D11_BUFFER_DESC  pDesc = {};
	pDesc.ByteWidth = DataByte;
	pDesc.Usage = D3D11_USAGE_DEFAULT;
	pDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	pDesc.CPUAccessFlags = 0u;
	pDesc.MiscFlags = 0u;
	pDesc.StructureByteStride = strideByte;
	//数据
	D3D11_SUBRESOURCE_DATA pData = {};
	pData.pSysMem = vertexs; //Pointer to the initialization data.

	//pDevice Setup Shit(Create)
	GFX_THROW_INFO(pDevice->CreateBuffer(&pDesc, &pData, &pVertexBuffer));
	//IA : InputAssemple  DX Pipeline, Bind VertexBuffer To Pipeline
	pContext->IASetVertexBuffers
	(
		0u,//有限的VertextBuffer数量
		1u,
		pVertexBuffer.GetAddressOf(),
		&strideByte,
		&offset
	);

	 const unsigned short Indices[]
	{
		 /*0,1,2,
		 0,2,3,
		 0,4,1,
		 2,1,5,*/

	    0, 1, 2,    // side 1
		2, 1, 3,
		4, 0, 6,    // side 2
		6, 0, 2,
		7, 5, 6,    // side 3
		6, 5, 4,
		3, 1, 7,    // side 4
		7, 1, 5,
		4, 5, 0,    // side 5
		0, 5, 1,
		3, 7, 2,    // side 6
		2, 7, 6,
	};

	wrl::ComPtr<ID3D11Buffer> pIndicesBuffer;
	D3D11_BUFFER_DESC  IndexDesc = {};
	IndexDesc.ByteWidth = sizeof(Indices);
	IndexDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexDesc.CPUAccessFlags = 0u;
	IndexDesc.MiscFlags = 0u;
	IndexDesc.StructureByteStride = sizeof(unsigned short );

	D3D11_SUBRESOURCE_DATA pIndexData = {};
	pIndexData.pSysMem = Indices;

	GFX_THROW_INFO(pDevice->CreateBuffer(&IndexDesc, &pIndexData, &pIndicesBuffer));
	pContext->IASetIndexBuffer(pIndicesBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	//Create ConstBuffer
	struct ConstBuffer
	{
		//dx里面的data SIDM处理过 不可直接访问其中的元素
		dx::XMMATRIX transform;

	};
	const ConstBuffer cb =
	{
		{
			dx::XMMatrixTranspose
			(
				//dx::XMMatrixRotationZ(angel) * dx::XMMatrixScaling(3.f / 4.f,1.f,1.f) * dx::XMMatrixTranslation(posx,posy,0.f)
				dx::XMMatrixRotationX(angel) * dx::XMMatrixRotationY(angel)  * dx::XMMatrixTranslation(posx,0.f,posz+4.f)* dx::XMMatrixPerspectiveLH(1.f,3.f / 4.f,1.f,50.f)
			)		
		}
	};

	//Matrix Const Buffer for VSShader
	wrl::ComPtr<ID3D11Buffer> constBuffer;
	D3D11_BUFFER_DESC constbufferDesc = {};
	constbufferDesc.ByteWidth = sizeof(cb);
	constbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constbufferDesc.StructureByteStride = 0u;
	constbufferDesc.MiscFlags = 0u;
	
	D3D11_SUBRESOURCE_DATA constBufferSource = {};
	constBufferSource.pSysMem = &cb;
	
	pDevice->CreateBuffer(&constbufferDesc, &constBufferSource, &constBuffer);
	//Bind ConstantBuffer
	pContext->VSSetConstantBuffers(0u, 1u, constBuffer.GetAddressOf());
	

	struct ConstBuffer2
	{
		struct 
		{
			float r;
			float g;
			float b;
			float a;
		}face_color[6];
	};

	const ConstBuffer2 cb2
	{
		{
			{1.f,0.f,0.f},
			{0.f,1.f,1.f},
			{1.f,0.f,1.f},
			{1.f,1.f,0.f},
			{1.f,0.f,0.5f},
			{0.f,0.3f,1.f},
		}
	};



	//Color Const Buffer for PSShader
	wrl::ComPtr<ID3D11Buffer> constBuffer2;
	D3D11_BUFFER_DESC constbufferDesc2 = {};
	constbufferDesc2.ByteWidth = sizeof(cb2);
	constbufferDesc2.Usage = D3D11_USAGE_DEFAULT;
	constbufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constbufferDesc2.CPUAccessFlags = 0u;
	constbufferDesc2.StructureByteStride = 0u;
	constbufferDesc2.MiscFlags = 0u;

	D3D11_SUBRESOURCE_DATA constBufferSource2 = {};
	constBufferSource2.pSysMem = &cb2;

	GFX_THROW_INFO(pDevice->CreateBuffer(&constbufferDesc2, &constBufferSource2, &constBuffer2));
	//Bind ConstantBuffer
	pContext->PSSetConstantBuffers(0u, 1u, constBuffer2.GetAddressOf());




	//Create Pixel Shader(Read from file)
	wrl::ComPtr<ID3D11PixelShader> PixelShader;
	wrl::ComPtr<ID3DBlob> FileBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &FileBlob));
	pDevice->CreatePixelShader(FileBlob->GetBufferPointer(), FileBlob->GetBufferSize(), nullptr, &PixelShader);
	//Bind Pixel Shader , Pixel Shader Expect a RenderTarget
	pContext->PSSetShader(PixelShader.Get(), nullptr, 0u);

	//Create Vertex Shader(Read from file)
	wrl::ComPtr<ID3D11VertexShader> VertexShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &FileBlob));
	GFX_THROW_INFO(pDevice->CreateVertexShader(FileBlob->GetBufferPointer(), FileBlob->GetBufferSize(), nullptr, &VertexShader));
	//Bind Vertex Shader
	pContext->VSSetShader(VertexShader.Get(), nullptr, 0u);

	//InputAssemple Create InputLayout,
	wrl::ComPtr<ID3D11InputLayout> InputLayout;
	const D3D11_INPUT_ELEMENT_DESC elementDesc[] = { 
		{"Position",0u,DXGI_FORMAT_R32G32B32_FLOAT ,0u, 0u, D3D11_INPUT_PER_VERTEX_DATA ,0u}, 
		//{"Color",0u,DXGI_FORMAT_R8G8B8A8_UNORM ,0u, 12u, D3D11_INPUT_PER_VERTEX_DATA ,0u}, // UNORM 归一化为浮点值
	};
	GFX_THROW_INFO(pDevice->CreateInputLayout(elementDesc, (UINT)std::size(elementDesc), FileBlob->GetBufferPointer(), FileBlob->GetBufferSize(), &InputLayout));
	//Bind InputLayout
	pContext->IASetInputLayout(InputLayout.Get());

	//Topology 拓扑类型 在IA
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//set Viewport
	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.Width = 800.f;
	vp.Height = 600.f;
	//Set Viewport 在光栅化阶段 Rastenizer
	pContext->RSSetViewports(1u, &vp);

	//Output Merger (RenderTargetViews RTVs,DepthStencilViews DTV)
	//pContext->OMSetRenderTargets(1u, pRenderTargetView.GetAddressOf(), nullptr);

	//pContext Render Shit , Draw() Need Viewport
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(Indices),0u,0u));
}

// Graphics exception stuff
Graphics::HrException::HrException(int line, const char * file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr)
{// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m; 
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
		if (!info.empty())
		{
			oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
		}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Chili Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr; 
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}


std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char * file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}


const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Chili Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}