#pragma once
// 简易的transform类，用于描述对象的移动、旋转、缩放等操作相关的数据
#ifndef  TRANSFORM_H
#define TRANSFORM_H
#include <DirectXMath.h>
class Transform
{
public:
	//设置默认编译器描述
	Transform() = default;
	Transform(const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& position);
	~Transform() = default;

	Transform(const Transform&) = default;
	Transform& operator=(const Transform&) = default;

	Transform(Transform&&) = default;
	Transform& operator=(Transform&&) = default;

	//获取缩放大小，加const设置该函数为仅可读
	DirectX::XMFLOAT3 GetScale() const;
	DirectX::XMVECTOR GetScaleXM() const;

	//获取对象欧拉角(弧度制)
	//对象以Z-X-Y轴顺序旋转，以避免万向锁
	DirectX::XMFLOAT3 GetRotation() const;
	DirectX::XMVECTOR GetRotationXM() const;

	//获取对象位置
	DirectX::XMFLOAT3 GetPosition() const;
	DirectX::XMVECTOR GetPositionXM() const;

	//获取上方向轴
	DirectX::XMFLOAT3 GetUpAxis() const;
	DirectX::XMVECTOR GetUpAxisXM() const;

	//获取前方向轴
	DirectX::XMFLOAT3 GetForwardAxis() const;
	DirectX::XMVECTOR GetForwardAxisXM() const;

	//获取右方向轴
	DirectX::XMFLOAT3 GetRightAxis() const;
	DirectX::XMVECTOR GetRightAxisXM() const;

	//获取世界变换矩阵
	DirectX::XMFLOAT4X4 GetLocalToWorldMatrix() const;
	DirectX::XMMATRIX GetLocalToWorldMatrixXM() const;

	//获取逆世界变换矩阵
	DirectX::XMFLOAT4X4 GetWorldToLocalMatrix() const;
	DirectX::XMMATRIX GetWorldToLocalMatrixXM() const;

	//设置对象缩放比例
	void SetScale(const DirectX::XMFLOAT3& scale);
	void SetScale(float x, float y, float z);

	//设置对象欧拉角
	void SetRotation(const DirectX::XMFLOAT3& eulerAnglesInRadian);
	void SetRotation(float x, float y, float z);

	//设置对象位置
	void SetPosition(const DirectX::XMFLOAT3& position);
	void SetPosition(float x, float y, float z);

	//指定旋转对象
	void Rotate(const DirectX::XMFLOAT3 eulerAnglesInRadian);

	//指定以原点为中心绕某轴旋转
	void RotateAxis(const DirectX::XMFLOAT3& axis, float radian);
	//指定以某点为中心绕某轴旋转
	void RotateAround(const DirectX::XMFLOAT3& point, const DirectX::XMFLOAT3& axis, float radian);

	//沿某一方向平移
	void Translate(const DirectX::XMFLOAT3& direction, float distance);

	//观察某一点
	void LookAt(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up = { 0.0f,1.0f,0.0f });

	//沿某一方向观察
	void LookTo(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& up = { 0.0f,1.0f,0.0f });

	//从旋转矩阵获取旋转欧拉角
	static DirectX::XMFLOAT3 GetEulerAnglesFromRotationMatrix(const DirectX::XMFLOAT4X4& rotationMatrix);

private:
	DirectX::XMFLOAT3 m_Scale = { 1.0f,1.0f,1.0f };
	DirectX::XMFLOAT3 m_Rotation = {};
	DirectX::XMFLOAT3 m_Position = {};



};


#endif // ! TRANSFORM_H