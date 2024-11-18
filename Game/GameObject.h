#pragma once

//游戏对象类，集成了一个对象所需的组件
#ifndef  GAMEOBJECT_H
#define GAMEOBJECT_H

#include "GeometryGenerator.h"
#include "Transform.h"
#include "RenderState.h"

class GameObject
{
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	GameObject();

	Transform& GetTransform();

	// 获取物体变换
	const Transform& GetTransform() const;

	// 设置纹理
	void SetTexture();

	// 设置材质
	void SetMaterial();

	// 绘制
	void Draw();

private:
	Transform mTransform;

	// 顶点字节大小
	UINT m_VertexStride;
	// 索引数目
	UINT m_IndexCount;

};

#endif