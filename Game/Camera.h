#pragma once
//摄像机功能
//提供第一人称与第三人称摄像机
//***********************************
#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>
#include <d3d12.h>
#include "WinAPISetting.h"
#include "Transform.h"
//摄像机类
class Camera
{
public:

	//设置默认编译器描述
	Camera() = default;
	virtual ~Camera() = 0;

	//获取摄像机位置
	DirectX::XMFLOAT3 GetPosition() const;
	DirectX::XMVECTOR GetPositionXM() const;

	//获取摄像机旋转
	//获取绕X轴旋转的欧拉角弧度
	float GetRotationX() const;

	//获取绕Y轴旋转的欧拉角弧度
	float GetRotationY() const;

	//获取摄像机的坐标轴向量
	//获取右方向向量
	DirectX::XMFLOAT3 GetRightAxis() const;
	DirectX::XMVECTOR GetRightAxisXM() const;

	//获取上方向向量
	DirectX::XMFLOAT3 GetUpAxis() const;
	DirectX::XMVECTOR GetUpAxisXM() const;

	//获取前方向(视线方向)向量
	DirectX::XMFLOAT3 GetLookAxis() const;
	DirectX::XMVECTOR GetLookAxisXM() const;


	//获取矩阵
	DirectX::XMMATRIX GetViewXM() const;
	DirectX::XMMATRIX GetProjXM() const;
	DirectX::XMMATRIX GetViewProjXM() const;

	//获取视口
	D3D12_VIEWPORT GetViewPort() const;

	//获取视锥体
	void SetFrustum(float fovY, float aspect, float nearZ, float farZ);

	//设置视口
	void SetViewPort(const D3D12_VIEWPORT& viewPort);
	void SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

protected:
	//摄像机对象
	Transform m_Transform = {};

	//视椎体属性
	float m_NearZ = 0.0f;
	float m_FarZ = 0.0f;
	float m_Aspect = 0.0f;
	float m_FovY = 0.0f;

	D3D12_VIEWPORT m_ViewPort = {};
};
//第一人称视角
class FirstPersonCamera :public Camera
{
public:
	FirstPersonCamera() = default;
	~FirstPersonCamera() override;

	//设置摄像机位置
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& pos);

	//设置摄像机朝向
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);
	void LookTo(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& to, const DirectX::XMFLOAT3& up);

	//平移
	void Move(float d);

	//平面移动
	void Walk(float d);

	//前进
	void MoveForward(float d);

	//上下观察
	//正rad值向上观察
	//负rad值向下观察
	void Pitch(float rad);

	//左右观察
	//正rad值向左观察
	//负rad值向右观察
	void RotateY(float rad);
};


class ThirdPersonCamera :public Camera
{
public:
	ThirdPersonCamera() = default;
	~ThirdPersonCamera() override;

	//获取当前跟踪的物体
	DirectX::XMFLOAT3 GetTargetPosition() const;

	//设置摄像机位置
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& pos);

	//获取与物体距离
	float GetDistance() const;

	//绕物体垂直旋转(弧度限制在[0，pi/3])
	void RotateX(float rad);

	//绕物体水平旋转
	void RotateY(float rad);
	//拉进物体
	void Approach(float dist);

	//设置初始绕X轴的弧度(弧度限制在[0，pi/3])
	void SetRotationX(float rad);
	//平移
	void Move(float d);

	//平面移动
	void Walk(float d);

	//前进
	void MoveForward(float d);
	//设置初始绕Y轴的弧度
	void SetRotationY(float rad);

	//设置并绑定待跟踪物体的位置
	void SetTarget(const DirectX::XMFLOAT3& target);

	//设置初始距离
	void SetDistance(float dist);

	//设置最小最大允许距离
	void SetDistanceMinMax(float minDist, float maxDist);
private:
	DirectX::XMFLOAT3 m_Target = {};
	float m_Distance = 0.0f;
	float m_MinDist = 0.0f;
	float m_MaxDist = 0.0f;
};




#endif