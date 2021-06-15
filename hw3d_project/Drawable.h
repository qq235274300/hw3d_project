#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

class Bindable;

class Drawable
{
	template<class T>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;

	void Draw(Graphics& gfx) const noexcept;
	void AddBind(std::unique_ptr<Bindable> bind) noexcept;
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;

	virtual void Update(float dt) noexcept = 0;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;

private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;
	std::vector<std::unique_ptr<Bindable>> binds;
	const class IndexBuffer* pIndexBuffer = nullptr;
};
