#include "TextureData.h"
void TextureData::LoadImageInfo(ImageInfo imageInfo) {
	mWidth = imageInfo.Width;
	mHeight = imageInfo.Height;
	mBPP = imageInfo.BPP;
	mSource = imageInfo.Source;
}

void TextureData::InitBuffer(ID3D12Device* device) {
	D3D12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC::Tex2D(mFormat, mWidth, mHeight);
	device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&mResource));
	const UINT64 uploaderBufferSize = GetRequiredIntermediateSize(mResource, 0, 1);
	device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(uploaderBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mUploader));

	uint64_t pixelRowPitch = (uint64_t(mWidth) * uint64_t(mBPP) + 7) / 8;

	void* pixel = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, uploaderBufferSize);
	if (pixel == nullptr) return;
	mSource->CopyPixels(nullptr, pixelRowPitch, static_cast<UINT>(pixelRowPitch * mHeight), reinterpret_cast<BYTE*>(pixel));
	UINT numRows;
	UINT64 rowSizeInBytes, totalBytes;
	device->GetCopyableFootprints(&texDesc, 0, 1, 0, &mFootprint, &numRows, &rowSizeInBytes, &totalBytes);
	BYTE* data = nullptr;
	mUploader->Map(0, nullptr, reinterpret_cast<void**>(&data));
	BYTE* destSlice = reinterpret_cast<BYTE*>(data) + mFootprint.Offset;
	const BYTE* srcSlice = reinterpret_cast<const BYTE*>(pixel);

	for (size_t i = 0; i < numRows; i++)
		memcpy(destSlice + static_cast<size_t>(mFootprint.Footprint.RowPitch) * i,
			srcSlice + static_cast<size_t>(pixelRowPitch) * i,
			pixelRowPitch);
	mUploader->Unmap(0, nullptr);
	HeapFree(GetProcessHeap(), 0, pixel);
}

void TextureData::InitBuffer(ID3D12Device* device, UINT offsetX, UINT offsetY, UINT width, UINT height) {
	D3D12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC::Tex2D(mFormat, width, height);
	device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&mResource));
	const UINT64 uploaderBufferSize = GetRequiredIntermediateSize(mResource, 0, 1);
	device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(uploaderBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mUploader));

	uint64_t pixelRowPitch = (uint64_t(width) * uint64_t(mBPP) + 7) / 8;

	void* pixel = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, uploaderBufferSize);
	if (pixel == nullptr) return;
	WICRect scissor = { offsetX, offsetY, width, height };
	mSource->CopyPixels(&scissor, pixelRowPitch, static_cast<UINT>(pixelRowPitch * height), reinterpret_cast<BYTE*>(pixel));
	UINT numRows;
	UINT64 rowSizeInBytes, totalBytes;
	device->GetCopyableFootprints(&texDesc, 0, 1, 0, &mFootprint, &numRows, &rowSizeInBytes, &totalBytes);
	BYTE* data = nullptr;
	mUploader->Map(0, nullptr, reinterpret_cast<void**>(&data));
	BYTE* destSlice = reinterpret_cast<BYTE*>(data) + mFootprint.Offset;
	const BYTE* srcSlice = reinterpret_cast<const BYTE*>(pixel);

	for (size_t i = 0; i < numRows; i++)
		memcpy(destSlice + static_cast<size_t>(mFootprint.Footprint.RowPitch) * i,
			srcSlice + static_cast<size_t>(pixelRowPitch) * i,
			pixelRowPitch);
	mUploader->Unmap(0, nullptr);
	HeapFree(GetProcessHeap(), 0, pixel);
}

void TextureData::InitBufferAsCubeMap(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) {
	UINT X = mWidth / 4;
	UINT Y = mHeight / 3;

	UINT offsetX[6] = { 2 * X, 0, 1 * X, 1 * X, 1 * X, 3 * X };
	UINT offsetY[6] = { 1 * Y, 1 * Y, 0, 2 * Y, 1 * Y, 1 * Y };

	D3D12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC::Tex2D(mFormat, X, Y, 6, 1);
	device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&mResource));
	const UINT64 uploaderBufferSize = GetRequiredIntermediateSize(mResource, 0, 6);
	device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(uploaderBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mUploader));

	uint64_t pixelRowPitch = (uint64_t(X) * uint64_t(mBPP) + 7) / 8;

	std::vector<D3D12_SUBRESOURCE_DATA> subresources(6);
	std::vector<void*> pixel(6);
	for (size_t i = 0; i < 6; i++)
	{
		pixel[i] = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pixelRowPitch * Y);
		if (pixel[i] == nullptr) return;
		WICRect scissor = { offsetX[i], offsetY[i], X, Y };
		mSource->CopyPixels(&scissor, pixelRowPitch, static_cast<UINT>(pixelRowPitch * Y), reinterpret_cast<BYTE*>(pixel[i]));

		subresources[i].pData = pixel[i];
		subresources[i].RowPitch = pixelRowPitch;
		subresources[i].SlicePitch = pixelRowPitch * Y;
	}

	UpdateSubresources(cmdList, mResource, mUploader, 0, 0, 6, subresources.data());

	for (size_t i = 0; i < pixel.size(); i++)
		HeapFree(GetProcessHeap(), 0, pixel[i]);

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(mResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList->ResourceBarrier(1, &barrier);
}

void TextureData::SetCopyCommand(ID3D12GraphicsCommandList* cmdList) {
	CD3DX12_TEXTURE_COPY_LOCATION dst(mResource, 0);
	CD3DX12_TEXTURE_COPY_LOCATION src(mUploader, mFootprint);
	cmdList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(mResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList->ResourceBarrier(1, &barrier);
}

ID3D12Resource* TextureData::GetTexture() {
	return mResource;
}

bool LoadPixelWithWIC(const wchar_t* path, GUID tgFormat, ImageInfo& imageInfo) {
	IWICImagingFactory* factory;
	HRESULT res = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
	IWICBitmapDecoder* decoder;
	res = factory->CreateDecoderFromFilename(path, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);

	if (FAILED(res))
		return false;

	IWICBitmapFrameDecode* frame;
	decoder->GetFrame(0, &frame);
	IWICFormatConverter* converter;
	factory->CreateFormatConverter(&converter);
	converter->Initialize(frame, tgFormat, WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom);
	converter->QueryInterface(IID_PPV_ARGS(&imageInfo.Source));
	imageInfo.Source->GetSize(&imageInfo.Width, &imageInfo.Height);
	IWICComponentInfo* info;
	factory->CreateComponentInfo(tgFormat, &info);
	IWICPixelFormatInfo* pixelInfo;
	info->QueryInterface(IID_PPV_ARGS(&pixelInfo));
	pixelInfo->GetBitsPerPixel(&imageInfo.BPP);

	factory->Release();
	decoder->Release();
	converter->Release();
	info->Release();
	pixelInfo->Release();

	return true;
}