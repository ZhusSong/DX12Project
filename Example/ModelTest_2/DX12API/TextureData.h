#pragma once
#ifndef TEXTUREDATA_H
#define TEXTUREDATA_H
#include <wincodec.h>
#include "d3dUtil.h"

struct ImageInfo {
	UINT Width, Height, BPP;
	IWICBitmapSource* Source;
};
class TextureData {
public:
	DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	void LoadImageInfo(ImageInfo imageInfo);
	void InitBuffer(ID3D12Device* device);
	void InitBuffer(ID3D12Device* device, UINT offsetX, UINT offsetY, UINT width, UINT height);
	void InitBufferAsCubeMap(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	void SetCopyCommand(ID3D12GraphicsCommandList* cmdList);
	ID3D12Resource* GetTexture();

private:
	UINT mWidth, mHeight, mBPP;
	ID3D12Resource* mResource;
	IWICBitmapSource* mSource;
	ID3D12Resource* mUploader;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT mFootprint;
};

bool LoadPixelWithWIC(const wchar_t* path, GUID tgFormat, ImageInfo& imageInfo);


#endif // !TEXTURE_H
