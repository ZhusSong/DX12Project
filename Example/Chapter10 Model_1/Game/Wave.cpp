#include "Wave.h"
#include <ppl.h>
#include <algorithm>
#include <vector>
#include <cassert>
Waves::Waves(int m, int n, float dx, float dt, float speed, float damping)
{
    mNumRows = m;
    mNumCols = n;

    mVertexCount = m * n;
    mTriangleCount = (m - 1) * (n - 1) * 2;

    mTimeStep = dt;
    mSpatialStep = dx;

    float d = damping * dt + 2.0f;
    float e = (speed * speed) * (dt * dt) / (dx * dx);
    mK1 = (damping * dt - 2.0f) / d;
    mK2 = (4.0f - 8.0f * e) / d;
    mK3 = (2.0f * e) / d;

    mPrevSolution.resize(m * n);
    mCurrSolution.resize(m * n);
    mNormals.resize(m * n);
    mTangentX.resize(m * n);

    // Generate grid vertices in system memory.
    // 在系统内存中生成栅格顶点

    float halfWidth = (n - 1) * dx * 0.5f;
    float halfDepth = (m - 1) * dx * 0.5f;
    for (int i = 0; i < m; ++i)
    {
        float z = halfDepth - i * dx;
        for (int j = 0; j < n; ++j)
        {
            float x = -halfWidth + j * dx;

            mPrevSolution[i * n + j] =DirectX::XMFLOAT3(x, 0.0f, z);
            mCurrSolution[i * n + j] = DirectX::XMFLOAT3(x, 0.0f, z);
            mNormals[i * n + j] = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
            mTangentX[i * n + j] = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
        }
    }
}

Waves::~Waves()
{
}

int Waves::RowCount() const
{
    return mNumRows;
}

int Waves::ColumnCount() const
{
    return  mNumCols;
}

int Waves::VertexCount() const
{
    return mVertexCount;
}

int Waves::TriangleCount() const
{
    return mTriangleCount;
}

float Waves::Width() const
{
    return mNumCols * mSpatialStep;
}

float Waves::Depth() const
{
    return mNumRows * mSpatialStep;
}


void Waves::Update(float dt)
{
	static float t = 0;

	// Accumulate time.
	// 累积时间
	t += dt;

	// Only update the simulation at the specified time step.
	// 仅在特殊时间块之后更新模拟结果
	if (t >= mTimeStep)
	{
		// Only update interior points; we use zero boundary conditions.
		// 只更新内部点，我们使用零边界条件
		concurrency::parallel_for(1, mNumRows - 1, [this](int i)
			//for(int i = 1; i < mNumRows-1; ++i)
			{
				for (int j = 1; j < mNumCols - 1; ++j)
				{
					// After this update we will be discarding the old previous
					// buffer, so overwrite that buffer with the new update.
					// Note how we can do this inplace (read/write to same element) 
					// because we won't need prev_ij again and the assignment happens last.

					// Note j indexes x and i indexes z: h(x_j, z_i, t_k)
					// Moreover, our +z axis goes "down"; this is just to 
					// keep consistent with our row indices going down.

					// 在这次更新之后我们要丢弃旧缓冲区，因此要用新的更新覆盖此缓冲区
					// 注意我们通过读/写同一元素来实现这一点，因为我们不再需要prev_IJ，而且赋值发生在最后
					// 	注意 j 表示 x 索引，i 表示 z 索引：h(x_j, z_i, t_k)
					// 此外，我们的 +z 轴是 “向下 ”的，这只是为了与我们的行索引向下保持一致。
				
					mPrevSolution[i * mNumCols + j].y =
						mK1 * mPrevSolution[i * mNumCols + j].y +
						mK2 * mCurrSolution[i * mNumCols + j].y +
						mK3 * (mCurrSolution[(i + 1) * mNumCols + j].y +
							mCurrSolution[(i - 1) * mNumCols + j].y +
							mCurrSolution[i * mNumCols + j + 1].y +
							mCurrSolution[i * mNumCols + j - 1].y);
				}
			});

		// We just overwrote the previous buffer with the new data, so
		// this data needs to become the current solution and the old
		// current solution becomes the new previous solution.
		// 我们刚用新数据覆写了旧缓冲区，因此这个数据需要成为当前解，而旧的当前解成为新的旧解
		std::swap(mPrevSolution, mCurrSolution);

		// reset time
		// 重置时间
		t = 0.0f; 

		//
		// Compute normals using finite difference scheme.
		// 使用有限差分法计算法线
		concurrency::parallel_for(1, mNumRows - 1, [this](int i)
			//for(int i = 1; i < mNumRows - 1; ++i)
			{
				for (int j = 1; j < mNumCols - 1; ++j)
				{
					float l = mCurrSolution[i * mNumCols + j - 1].y;
					float r = mCurrSolution[i * mNumCols + j + 1].y;
					float t = mCurrSolution[(i - 1) * mNumCols + j].y;
					float b = mCurrSolution[(i + 1) * mNumCols + j].y;
					mNormals[i * mNumCols + j].x = -r + l;
					mNormals[i * mNumCols + j].y = 2.0f * mSpatialStep;
					mNormals[i * mNumCols + j].z = b - t;

					XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&mNormals[i * mNumCols + j]));
					XMStoreFloat3(&mNormals[i * mNumCols + j], n);

					mTangentX[i * mNumCols + j] = XMFLOAT3(2.0f * mSpatialStep, r - l, 0.0f);
					XMVECTOR T = XMVector3Normalize(XMLoadFloat3(&mTangentX[i * mNumCols + j]));
					XMStoreFloat3(&mTangentX[i * mNumCols + j], T);
				}
			});
	}
}

void  Waves::Disturb(int i, int j, float magnitude)
{
    // Don't disturb boundaries.
	// 防止越界
    assert(i > 1 && i < mNumRows - 2);
    assert(j > 1 && j < mNumCols - 2);

    float halfMag = 0.5f * magnitude;

    // Disturb the ijth vertex height and its neighbors.
	// 
    mCurrSolution[i * mNumCols + j].y += magnitude;
    mCurrSolution[i * mNumCols + j + 1].y += halfMag;
    mCurrSolution[i * mNumCols + j - 1].y += halfMag;
    mCurrSolution[(i + 1) * mNumCols + j].y += halfMag;
    mCurrSolution[(i - 1) * mNumCols + j].y += halfMag;
}

