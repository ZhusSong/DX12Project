#pragma once
//工具类，用于便捷生成数种常见的几何体

#include <cstdint>
#include <DirectXMath.h>
#include <vector>
class GeometryGenerator
{
public:
	using uint16 = std::uint16_t;
	using uint32= std::uint32_t;
	//Vertex struct and constructors. Contains position,normal,TangentU and tex's variables
	//顶点数据结构体及构造函数，包含位置(p)、法线(n)、切线空间(t)、纹理(uv)等数据
	struct Vertex
	{
		Vertex() {}
		Vertex(
			const DirectX::XMFLOAT3& p,
			const DirectX::XMFLOAT3& n,
			const DirectX::XMFLOAT3& t,
			const DirectX::XMFLOAT2& uv) :
			Position(p),
			Normal(n),
			TangentU(t),
			TexC(uv) {}
		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v) :
			Position(px, py, pz),
			Normal(nx, ny, nz),
			TangentU(tx, ty, tz),
			TexC(u, v) {}

		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT3 TangentU;
		DirectX::XMFLOAT2 TexC;
	};
	struct MeshData
	{
		//顶点数据
		std::vector<Vertex> Vertices;
		//索引数据
		std::vector<uint32> Indices32;
		//获取索引
		std::vector<uint16>& GetIndices16()
		{
			if (mIndices16.empty())
			{
				mIndices16.resize(Indices32.size());
				for (size_t i = 0; i < Indices32.size(); ++i)
					mIndices16[i] = static_cast<uint16>(Indices32[i]);
			}

			return mIndices16;
		}
	private:
		std::vector<uint16> mIndices16;
	};
	///<summary>
	/// Creates a box centered at the origin with the given dimensions, where each
	/// face has m rows and n columns of vertices.
	/// 创建一个以指定位置为中心，尺寸为指定值的盒子，每个面有m行n列顶点
	///</summary>
	MeshData CreateBox(float width, float height, float depth, uint32 numSubdivisions);

	///<summary>
	/// Creates a sphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.
	/// 创建一个以原点为中心、半径为指定值的球体。切片和堆叠参数控制细分程度。
	///</summary>
	MeshData CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);

	///<summary>
	/// Creates a geosphere centered at the origin with the given radius.  The
	/// depth controls the level of tessellation.
	/// 创建一个以原点为中心、半径为指定值的几何球体(使用面积相同且边长相等的三角形构建的球体)。参数控制细分程度。
	///</summary>
	MeshData CreateGeosphere(float radius, uint32 numSubdivisions);

	///<summary>
	/// Creates a cylinder parallel to the y-axis, and centered about the origin.  
	/// The bottom and top radius can vary to form various cone shapes rather than true
	// cylinders.  The slices and stacks parameters control the degree of tessellation.
	/// 创建一个创建一个平行于 y 轴、以原点为中心的圆柱体。
	/// 底部和顶部的半径可以改变，以形成各种圆锥体形状，而不仅是圆柱体。参数控制细分程度。
	///</summary>
	MeshData CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);

	///<summary>
	/// Creates an mxn grid in the xz-plane with m rows and n columns, centered
	/// at the origin with the specified width and depth.
	/// 创建一个行数为m，列数为n的mxn栅格
	///</summary>
	MeshData CreateGrid(float width, float depth, uint32 m, uint32 n);

	///<summary>
	/// Creates a quad aligned with the screen.  This is useful for postprocessing and screen effects.
	/// 创建与屏幕对齐的四边形。
	///</summary>
	MeshData CreateQuad(float x, float y, float w, float h, float depth);

private:
	// 对输入的顶点数据进行细分
	void Subdivide(MeshData& meshData);
	// 计算中点
	Vertex MidPoint(const Vertex& v0, const Vertex& v1);
	// 构建柱面的端面几何体
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
	// 构建柱面的底面几何体
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
};