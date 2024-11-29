#include "GameObject.h"
using namespace DirectX;

GameObject::GameObject() :
	mIndexCount(),
	mMaterial(),
	mVertexStride()
{
}


Transform& GameObject::GetTransform()
{
	return mTransform;
}

const Transform& GameObject::GetTransform() const
{
	return mTransform;
}

void GameObject::SetBuffer()
{
	mVertexBuffer.Reset();
	mIndexBuffer.Reset();
	
}

void GameObject::SetTexture()
{
	//mTexture = texture;
}

void GameObject::SetMaterial()
{
	//m_Material = material;
}

void GameObject::Draw()
{

}

