#include "GameApp.h"


GameApp::GameApp(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight)
    : DX12App(hInstance, windowName, initWidth, initHeight)
{
}



GameApp::~GameApp()
{
}

bool GameApp::Init()
{
    if (!DX12App::Init())
        return false;

    // Reset the command list to prep for initialization commands.
    // 重置命令列表为执行初始化命令做好准备
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
    BuildDescriptorHeaps();
    BuildConstantBuffers();
    BuildRootSignature();
    BuildShadersAndInputLayout();
    BuildBoxGeometry();
    BuildPSO();

    // Execute the initialization commands.
    // 执行初始化命令
    ThrowIfFailed(mCommandList->Close());
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // Wait until initialization is complete.
    // 等待初始化完成
    FlushCommandQueue();

    return true;
}

void GameApp::OnResize()
{
    DX12App::OnResize();
    // The window resized, so update the aspect ratio and recompute the projection matrix.
    // 若窗口尺寸被调整，则更新纵横比并重新计算投影矩阵
    XMMATRIX P=XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi,AspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&mProj, P);

}

void GameApp::Update(const DXGameTimer& gt)
{
    // Convert Spherical to Cartesian coordinates.
    // 将球坐标转换为笛卡尔坐标
    float x = mRadius * sinf(mPhi) * cosf(mTheta);
    float z = mRadius * sinf(mPhi) * sinf(mTheta);
    float y = mRadius * cosf(mPhi);

    // Build the view matrix.
    // 构建观察矩阵
    XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&mView, view);

    XMMATRIX world = XMLoadFloat4x4(&mWorld);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);
    XMMATRIX worldViewProj = world * view * proj;

    // Update the constant buffer with the latest worldViewProj matrix.
    // 使用最新的 worldViewProj 矩阵来更新常量缓冲区
    ObjectConstants objConstants;
    XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
    mObjectCB->CopyData(0, objConstants);
}

void GameApp::Draw(const DXGameTimer& gt)
{
    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    // 重复使用记录命令的相关内存
    // 只有当与GPU关联的命令列表执行完成时才将其重置
    ThrowIfFailed(mDirectCmdListAlloc->Reset());

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    // 将某个命令列表加入命令队列后，便重置该命令列表以此来复用命令列表及其内存
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

    // Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
    // 设置视口与裁剪矩阵，它们需要随着命令列表的重置而重置
    mCommandList->RSSetViewports(1, &mScreenViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);

    // Indicate a state transition on the resource usage.
    // 对资源状态进行转换，将资源从呈现状态转换为渲染目标状态
    mCommandList->ResourceBarrier(1,&CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT,D3D12_RESOURCE_STATE_RENDER_TARGET));

    // Clear the back buffer and depth buffer.
    // 清空后台缓冲区与深度缓冲区
    mCommandList->ClearRenderTargetView(CurrentBackBufferView(),Colors::LightGray,0,nullptr);
    mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // Specify the buffers we are going to render to.
    // 指定将要渲染的缓冲区
    mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

    // 获取描述符堆并绑定至命令列表
    // Bind the descriptor heap(s) to the pipeline
    ID3D12DescriptorHeap* descriptHeaps[] = { mCbvHeap.Get() };
    mCommandList->SetDescriptorHeaps(_countof(descriptHeaps), descriptHeaps);

    // 设置根签名
    // Sets the root signature for the command list
    mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

    // 设置顶点缓冲区与索引缓冲区
    // Sets the vertex buffer and index buffer
    mCommandList->IASetVertexBuffers(0, 1, &mBoxGeo->VertexBufferView());
    mCommandList->IASetIndexBuffer(&mBoxGeo->IndexBufferView());

    // 设置图元拓扑为三角形列表
    // Sets the primitive topology to a triangle list,
    mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 获取本次绘制所需的cbv并将描述符与渲染流水线绑定
    mCommandList->SetGraphicsRootDescriptorTable(0, mCbvHeap->GetGPUDescriptorHandleForHeapStart());

    // 通过索引进行绘制
    // Draw geometry by indices.
    mCommandList->DrawIndexedInstanced(mBoxGeo->DrawArgs["box"].IndexCount,1,0,0,0);
    
    // Indicate a state transition on the resource usage.
    // 再次对资源文件状态进行转换，将资源从渲染目标状态转换为呈现状态
    mCommandList->ResourceBarrier(
        1, &CD3DX12_RESOURCE_BARRIER::Transition(
            CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    //绘制游戏物体及画面
    DrawGame();


    // Done recording commands.
    // 完成对命令的记录,需在此之前进行游戏物体及逻辑的绘制
    ThrowIfFailed(mCommandList->Close());

    // Add the command list to the queue for execution.
    // 将待执行的命令列表加入命令队列
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // swap the back and front buffers
    // 交换后台缓冲区与前台缓冲区
    ThrowIfFailed(mSwapChain->Present(0, 0));
    mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

    // Wait until frame commands are complete.  
    // 等待此帧的命令执行完毕。
    FlushCommandQueue();
}

void GameApp::DrawGame()
{
    bool show_demo_window = false;
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    static int counter = 0;
    //test1    控制旋转
    ImGui::Begin("imgui!Test");                          // Create a window called "Hello, world!" and append into it.

    //ImGui::Text("Drag the slider to rotate the Angle of the Box.");               // Display some text (you can use a format strings too)
    //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

    //ImGui::SliderFloat("float", &mPhi, 0.1f, 1.0f);  //mPhi立方体的旋转角度
  //  ImGui::SliderFloat("float", &mTheta, 0.1f, 1.0f);  //mTheta也是旋转角度
    // Edit 1 float using a slider from 0.0f to 1.0f

    //if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    //    counter++;
    //ImGui::SameLine();
    //ImGui::Text("counter = %d", counter);

    //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Render();
    mCommandList->SetDescriptorHeaps(1, mSrvHeap.GetAddressOf());
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList.Get());
}
void GameApp::BuildDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCbvHeap)));
}

void GameApp::BuildConstantBuffers()
{
    mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 1, true);

    UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

    D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();
    // Offset to the ith object constant buffer in the buffer.
    // 偏移到常量缓冲区中第i个物体对应的常量数据，此处取i=0
    int boxCBufIndex = 0;
    cbAddress += boxCBufIndex * objCBByteSize;

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.BufferLocation = cbAddress;
    cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

    md3dDevice->CreateConstantBufferView(
        &cbvDesc,
        mCbvHeap->GetCPUDescriptorHandleForHeapStart());
}
void GameApp::BuildRootSignature()
{
    // Shader programs typically require resources as input (constant buffers,
    // textures, samplers).  The root signature defines the resources the shader
    // programs expect.  If we think of the shader programs as a function, and
    // the input resources as function parameters, then the root signature can be
    // thought of as defining the function signature.  
    // ***************************************************************
    // 着色器程序一般需要以资源作为输入，例如常量缓冲区、纹理。采样器等
    // 根签名则定义了着色器程序所需的具体资源
    // 若将着色器程序看作一个函数，则将输入的资源当做像函数传递的参数数据，
    // 那么便可认为根签名定义的是函数签名
    
    // Root parameter can be a table, root descriptor or root constants.
    // 根签名可以是描述符表、根描述符或根常量
    CD3DX12_ROOT_PARAMETER slotRootParameter[1];

    // Create a single descriptor table of CBVs.
    // 创建由单个CBV所组成的描述符表
    CD3DX12_DESCRIPTOR_RANGE cbvTable;
    cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 
        1,  // 表中的描述符数量
        0); // 将这段描述符区域绑定至此基准着色器寄存器(base shader register)
    slotRootParameter[0].InitAsDescriptorTable(
        1,              // 描述符区域的数量
        &cbvTable);     // 指向描述符区域数组的指针


    // A root signature is an array of root parameters.
    // 根签名由一组根参数构成
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    // 用单个寄存器槽来创建一个根签名，该槽位指向一个仅含有单个常量缓冲区的的描述符区域
    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    ThrowIfFailed(hr);

    ThrowIfFailed(md3dDevice->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&mRootSignature)));
}
void GameApp::BuildShadersAndInputLayout()
{
    HRESULT hr = S_OK;

    mvsByteCode = d3dUtil::CompileShader(L"HLSL\\06_color_vs.hlsl", nullptr, "VS", "vs_5_0");
    mpsByteCode = d3dUtil::CompileShader(L"HLSL\\06_color_ps.hlsl", nullptr, "PS", "vs_5_0");

    mInputLayout =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };
}


void GameApp::BuildBoxGeometry()
{
    std::array<Vertex, 8> vertices =
    {
        Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
        Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
        Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
        Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
        Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
        Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
        Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
        Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
    };
    std::array<std::uint16_t, 36> indices =
    {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 6, 5,
        4, 7, 6,

        // left face
        4, 5, 1,
        4, 1, 0,

        // right face
        3, 2, 6,
        3, 6, 7,

        // top face
        1, 5, 6,
        1, 6, 2,

        // bottom face
        4, 0, 3,
        4, 3, 7
    };
    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    mBoxGeo = std::make_unique<MeshGeometry>();
    mBoxGeo->Name = "boxGeo";

    ThrowIfFailed(D3DCreateBlob(vbByteSize, &mBoxGeo->VertexBufferCPU));
    CopyMemory(mBoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &mBoxGeo->IndexBufferCPU));
    CopyMemory(mBoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    mBoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
        mCommandList.Get(), vertices.data(), vbByteSize, mBoxGeo->VertexBufferUploader);

    mBoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
        mCommandList.Get(), indices.data(), ibByteSize, mBoxGeo->IndexBufferUploader);

    mBoxGeo->VertexByteStride = sizeof(Vertex);
    mBoxGeo->VertexBufferByteSize = vbByteSize;
    mBoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
    mBoxGeo->IndexBufferByteSize = ibByteSize;

    SubmeshGeometry submesh;
    submesh.IndexCount = (UINT)indices.size();
    submesh.StartIndexLocation = 0;
    submesh.BaseVertexLocation = 0;

    mBoxGeo->DrawArgs["box"] = submesh;
}
void GameApp::BuildPSO()
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    //输入布局描述
    psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
    //与此pso相绑定的根签名的指针
    psoDesc.pRootSignature = mRootSignature.Get();
    //待绑定的顶点着色器
    psoDesc.VS =
    {
        reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
        mvsByteCode->GetBufferSize()
    };
    //待绑定的像素着色器
    psoDesc.PS =
    {
        reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
        mpsByteCode->GetBufferSize()
    };
    //光栅器的光栅化状态
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    //混合状态
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    //深度/模板测试的状态
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    //采样个数
    psoDesc.SampleMask = UINT_MAX;
    //指定图元拓扑为三角形
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    //需渲染的目标数量
    psoDesc.NumRenderTargets = 1;
    //渲染目标的格式
    psoDesc.RTVFormats[0] = mBackBufferFormat;
    //指定多重采样的数量与质量
    psoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
    psoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
    //深度/模板缓冲区的格式
    psoDesc.DSVFormat = mDepthStencilFormat;
    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));

}