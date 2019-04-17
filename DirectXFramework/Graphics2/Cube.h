#pragma once

#ifndef CUBE
#define CUBE

#include "SceneNode.h"
#include "Framework.h"
#include "DirectXCore.h"
#include <vector>
#include "DirectXFramework.h"
#include "WICTextureLoader.h"

#endif

class Cube : public SceneNode
{
public:
	Cube() : SceneNode(L"Root") {};
	Cube(wstring name, wstring texture);
	~Cube();
	bool Initialise() override;
	void Render() override;
	void Shutdown() override;

	//Initialise methods
	void SetupFramework();
	void BuildGeometryBuffers();
	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	void BuildTexture();


private:
	//specify texture or color

	ComPtr<ID3D11Device>			_device;
	ComPtr<ID3D11DeviceContext>		_deviceContext;

	ComPtr<ID3D11Buffer>			_vertexBuffer;
	ComPtr<ID3D11Buffer>			_indexBuffer;

	ComPtr<ID3DBlob>				_vertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>				_pixelShaderByteCode = nullptr;
	ComPtr<ID3D11VertexShader>		_vertexShader;
	ComPtr<ID3D11PixelShader>		_pixelShader;
	ComPtr<ID3D11InputLayout>		_layout;
	ComPtr<ID3D11Buffer>			_constantBuffer;

	ComPtr<ID3D11ShaderResourceView> _texture;;
	wstring                         _textureName;
	D3D11_VIEWPORT					_screenViewport;

	XMFLOAT4X4						_viewTransformation;
	XMFLOAT4X4						_projectionTransformation;
};

