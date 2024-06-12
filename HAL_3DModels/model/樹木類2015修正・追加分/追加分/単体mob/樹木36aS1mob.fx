////////////////////////////////////////////////////////////////////////////////////////////////
//
//  樹木36aS1mob.fx : MikuMikuMob モデル複製・配置FX
//
//  MME : テクスチャ透過部を一定のα値でクリップします
//  (樹木などの複雑なテクスチャ透過モデルでアングルによって輪郭が半透明不透過による弊害を軽減します)
//  作成: 針金P( 舞力介入P氏のfull.fx改変 )
//
////////////////////////////////////////////////////////////////////////////////////////////////
// TexAlphaClipパラメータ定義

//透過α値の初期閾値
float DefAlphaThreshold = 0.1;


////////////////////////////////////////////////////////////////////////////////////////////////

// パラメータ定義
#define MMMob_ModelFileName "樹木36aS1mob.pmx" // モデルファイル名
#define MMMob_ArrangeFileName "MMMob_樹木36aS1mob.png" // 配置情報ファイル名
#define MMMob_TEX_WIDTH  1     // モデル配置テクスチャピクセル幅
#define MMMob_TEX_HEIGHT 2     // モデル配置テクスチャピクセル高さ
#define MMMob_SRC_HEAD_MAT  "0"  // 種モデル各オブジェクト先頭の材質番号
#define MMMob_SRC_TAIL_MAT  "1"  // 種モデル各オブジェクト最後の材質番号
#define MMMob_EDGE_ON   "10000"  // エッジを描画する材質番号
#define MMMob_EDGE_THICK  1.00f    // エッジ太さ比率
#define MMMob_MODEL_COUNT 1     // 種モデルオブジェクト数
#define MMMob_VERTEX_COUNT 5546   // 種モデル頂点数
#define MMMob_ORG_POS   float3(0.00, 0.0, 0.00)       // 種モデル配置座標

//#define MMMob_FLAG_HEIGHTMAP   // 高さマップ定義
#define MMMob_FOLLOWED_OBJ     // 高さマップの読み取り位置をモデルの動きに追従させる
//#define MMMob_FLAG_RANDMOVE    // 揺らぎプモーション定義
//#define MMMob_FLAG_RANDX       // X方向揺らぎプモーション定義
//#define MMMob_FLAG_RANDY       // Y方向揺らぎプモーション定義
//#define MMMob_FLAG_RANDZ       // Z方向揺らぎプモーション定義
//#define MMMob_FLAG_JUMP        // Y方向揺らぎジャンプモーション定義

#define MMMob_RENDER_EDGE      // エッジ描画処理を行う
#define MMMob_RENDER_SHADOW    // 地面影描画処理を行う
#define MMMob_RENDER_ZPLOT     // zplot描画処理を行う

#define MMMob_TRANSFORM_X     0.00f
#define MMMob_TRANSFORM_Y     MMMob_ColorToFloat(i+1, j)
#define MMMob_TRANSFORM_Z     0.00f
#define MMMob_TRANSFORM_RX    0.00000000f
#define MMMob_TRANSFORM_RY    0.00000000f
#define MMMob_TRANSFORM_RZ    0.00000000f
#define MMMob_TRANSFORM_SCALE 1.000f

#define MMMob_TRANSFORM_MODEL_X   0.00f
#define MMMob_TRANSFORM_MODEL_Y   0.00f
#define MMMob_TRANSFORM_MODEL_Z   0.00f

#define MMMob_TEX_WIDTH2    2     // MMMob_TEX_WIDTH*2
#define MMMob_TEX_WIDTH4    4     // MMMob_TEX_WIDTH*4
#define MMMob_TEX_WIDTH8    8     // MMMob_TEX_WIDTH*8
#define MMMob_TEX_WIDTH16   16     // MMMob_TEX_WIDTH*16
#define MMMob_TEX_WIDTH32   32     // MMMob_TEX_WIDTH*32
#define MMMob_TEX_WIDTH64   64     // MMMob_TEX_WIDTH*64
#define MMMob_TEX_WIDTH128  128     // MMMob_TEX_WIDTH*128

#define MMMob_EDGE_TEXCOORD  float4 MMMob_AddUV_Edge : TEXCOORD1  // エッジ情報を含む追加UVの入力セマンティック

//#define MMMob_DRAW_EDGE_HEAD   // オブジェクト先頭材質でエッジ描画する
//#define MMMob_DRAW_EDGE_TAIL   // オブジェクト最終材質でエッジ描画する

int MMMob_RepertCount = 1;  // シェーダ内描画反復回数
int MMMob_RepertIndex;        // シェーダ内描画反復回数のカウンタ

////////////////////////////////////////////////////////////////////////////////////////////////
// ボーン・モーフパラメータ

#ifndef MIKUMIKUMOVING
    float4x4 MMMob_ParentMat : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "全ての親"; >;
#else
    float4 MMMob_parentPos0 : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "全ての親"; >;
    float4x4 MMMob_parentMat : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "全ての親"; >;
    static float4x4 MMMob_ParentMat = float4x4(MMMob_parentMat[0], MMMob_parentMat[1], MMMob_parentMat[2], MMMob_parentPos0);
#endif
float3 MMMob_ParentPos : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "親移動"; >;

#ifdef MMMob_FOLLOWED_OBJ
    float3 MMMob_CenterPos : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "センター"; >;
#endif

#ifndef MIKUMIKUMOVING
#ifdef MMMob_RENDER_EDGE
    float MMMob_EdgeThick1 : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "エッジ細"; >;
    float MMMob_EdgeThick2 : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "エッジ太"; >;
    static float MMMob_EdgeThickness = (1.0f - MMMob_EdgeThick1 + 2.0f*MMMob_EdgeThick2)*MMMob_EDGE_THICK;
#endif
#endif

#ifdef MMMob_FLAG_RANDMOVE
    float MMMob_RandTime : TIME;
    float MMMob_MorphRandMove : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "揺ランダム"; >;
    #ifdef MMMob_FLAG_RANDX
    float MMMob_MorphXAmp : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "X振幅"; >;
    float MMMob_MorphXFreq : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "X周波数"; >;
    float MMMob_MorphXPhase : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "X位相"; >;
    #endif
    #ifdef MMMob_FLAG_RANDY
    float MMMob_MorphYAmp : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "Y振幅"; >;
    float MMMob_MorphYFreq : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "Y周波数"; >;
    float MMMob_MorphYPhase : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "Y位相"; >;
    #endif
    #ifdef MMMob_FLAG_RANDZ
    float MMMob_MorphZAmp : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "Z振幅"; >;
    float MMMob_MorphZFreq : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "Z周波数"; >;
    float MMMob_MorphZPhase : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "Z位相"; >;
    #endif
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
// 高さマップ関連

#ifdef MMMob_FLAG_HEIGHTMAP
    float MMMob_MorphHMRightAng : CONTROLOBJECT < string name = MMMob_ModelFileName; string item = "地面直角"; >;
    float3 MMMob_MapAreaPos  : CONTROLOBJECT < string name = "HeightMap.x"; >;
    float MMMob_MapAreaAcsRy : CONTROLOBJECT < string name = "HeightMap.x"; string item = "Ry"; >;
    float MMMob_MapAreaAcsSi : CONTROLOBJECT < string name = "HeightMap.x"; string item = "Si"; >;
    static float MMMob_MapAreaSinR = sin(MMMob_MapAreaAcsRy)/MMMob_MapAreaAcsSi;
    static float MMMob_MapAreaCosR = cos(MMMob_MapAreaAcsRy)/MMMob_MapAreaAcsSi;
    
    // 高さマップテクスチャのサイズ
    #define MMMob_HeightMapWIDTH   1024
    #define MMMob_HeightMapHEIGHT  1024
    
    // 高さマップに使うオフスクリーンバッファ
    shared texture MMMob_HeightMapRT : OFFSCREENRENDERTARGET;
    sampler MMMob_SmpHeightMap : register(s2) = sampler_state
    {
        Texture = <MMMob_HeightMapRT>;
        AddressU = BORDER;
        AddressV = BORDER;
        BorderColor = 0;
        MinFilter = LINEAR;
        MagFilter = LINEAR;
    };
    
    // 高さマップから指定位置の高さを取得
    float MMMob_PickupHeight(float x, float z)
    {
        x -= MMMob_MapAreaPos.x;
        z -= MMMob_MapAreaPos.z;
        float x1 = MMMob_MapAreaCosR*x - MMMob_MapAreaSinR*z;
        float z1 = MMMob_MapAreaSinR*x + MMMob_MapAreaCosR*z;
        x1 = x1*0.1f + 0.5f + 0.5/MMMob_HeightMapWIDTH;
        z1 = 0.5f - z1*0.1f + 0.5/MMMob_HeightMapHEIGHT;
        float h = tex2Dlod(MMMob_SmpHeightMap, float4(x1, z1, 0, 0)).r;
        return h;
    }
    
    // 高さマップから指定位置の地面に垂直となる回転行列を取得
    float3x3 MMMob_PickupRotMat(float x, float z, float hStep)
    {
        float3 PosX1 = float3(x-hStep, MMMob_PickupHeight(x-hStep, z), z);
        float3 PosX2 = float3(x+hStep, MMMob_PickupHeight(x+hStep, z), z);
        float3 PosZ1 = float3(x, MMMob_PickupHeight(x, z-hStep), z-hStep);
        float3 PosZ2 = float3(x, MMMob_PickupHeight(x, z+hStep), z+hStep);
        float lenX = length( (PosX2 - PosX1) * float3(1.0f, MMMob_MorphHMRightAng, 1.0f) );
        float lenZ = length( (PosZ2 - PosZ1) * float3(1.0f, MMMob_MorphHMRightAng, 1.0f) );
        float sinX = MMMob_MorphHMRightAng * (PosZ1.y - PosZ2.y) / lenZ;
        float cosX = 2.0f * hStep / lenZ;
        float sinZ = MMMob_MorphHMRightAng * (PosX2.y - PosX1.y) / lenX;
        float cosZ = 2.0f * hStep / lenX;
        float3x3 rotMat = { cosZ,  cosX*sinZ,  sinX*sinZ,
                           -sinZ,  cosX*cosZ,  sinX*cosZ,
                            0.0f,      -sinX,       cosX };
        return rotMat;
    }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
// エッジ,地面影,Zプロット関連

#ifndef MIKUMIKUMOVING

#ifdef MMMob_RENDER_EDGE
    // エッジ太さ・色
    float4 MMMob_EdgeColor : EDGECOLOR;
    #ifdef MMMob_EDGE_TEXCOORD
        #define MMMob_EDGE_THICKNESS  (MMMob_AddUV_Edge.w * MMMob_EdgeThickness)
        #define MMMob_EDGE_COLOR      float4(MMMob_AddUV_Edge.xyz, MMMob_EdgeColor.a)
    #else
        #define MMMob_EDGE_THICKNESS  MMMob_EdgeThickness
        #define MMMob_EDGE_COLOR      MMMob_EdgeColor
        #define MMMob_EDGE_TEXCOORD   float4 MMMob_DummyTex : TEXCOORD5
    #endif

    // オブジェクト描画テクニックで MMMob_EdgeColor を取得するためのダミー処理
    // 頂点シェーダ
    float4 MMMob_DummyEdge_VS(float4 Pos : POSITION) : POSITION 
    {
        return float4(0,0,0,0);
    }
    // ピクセルシェーダ
    float4 MMMob_DummyEdge_PS() : COLOR
    {
        return float4(0,0,0,0);
    }
    // 輪郭描画用テクニック
    technique MMMob_DummyEdgeTec < string MMDPass = "edge"; > {
        pass DrawEdge {
            AlphaBlendEnable = FALSE;
            AlphaTestEnable  = FALSE;
            VertexShader = compile vs_2_0 MMMob_DummyEdge_VS();
            PixelShader  = compile ps_2_0 MMMob_DummyEdge_PS();
        }
    }
#else
    #define MMMob_EDGE_THICKNESS  (0.0f)
    #define MMMob_EDGE_COLOR      float4(0,0,0,1);
    #ifndef MMMob_EDGE_TEXCOORD
        #define MMMob_EDGE_TEXCOORD   float4 MMMob_DummyTex : TEXCOORD5
    #endif
#endif

float4x4 MMMob_PrjMtx : PROJECTION;
#define MMMob_IsParth  (MMMob_PrjMtx._44 < 0.5f)

// MMDのエッジ描画は行わない
technique MMMob_EdgeTec < string MMDPass = "edge"; > { }

// Z値プロット描画非表示指定の場合
#ifndef MMMob_RENDER_ZPLOT
    technique MMMob_ZplotTec < string MMDPass = "zplot"; >{ }
#endif

#endif

// 非セルフシャドウ地面影描画非表示指定の場合
#ifndef MMMob_RENDER_SHADOW
    technique MMMob_ShadowTec < string MMDPass = "shadow"; >{ }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
// 配置情報テクスチャ

texture2D MMMob_ArrangeTex <
    string ResourceName = MMMob_ArrangeFileName;
>;
sampler MMMob_ArrangeSmp : register(s3) = sampler_state{
    texture = <MMMob_ArrangeTex>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;
};

////////////////////////////////////////////////////////////////////////////////////////////////

// 整数除算
int MMMob_Div(int a, int b) {
    return floor((a+0.1f)/b);
}

// 整数剰余算
int MMMob_Mod(int a, int b) {
    return (a - MMMob_Div(a,b)*b);
};

// 配置情報テクスチャからデータを取り出す
float MMMob_ColorToFloat(int i, int j)
{
    float4 d = tex2Dlod(MMMob_ArrangeSmp, float4((i+0.5)/MMMob_TEX_WIDTH8, (j+0.5)/MMMob_TEX_HEIGHT, 0, 0));
    float tNum = (65536.0f * d.x + 256.0f * d.y + d.z) * 255.0f;
    int pNum = round(d.w * 255.0f);
    int sgn = 1 - 2 * (pNum % 2);
    float data = tNum * pow(10.0f, pNum/2 - 64) * sgn;
    return data;
}

// 種モデルオブジェクト位置を取り出す
float3 MMMob_GetModelPos(int modelIndex)
{
    int index = MMMob_MODEL_COUNT * MMMob_RepertCount + modelIndex;
    int i = (index/MMMob_TEX_HEIGHT) * 8;
    int j = index % MMMob_TEX_HEIGHT;
    return float3(MMMob_TRANSFORM_MODEL_X, MMMob_TRANSFORM_MODEL_Y, MMMob_TRANSFORM_MODEL_Z);
}

// 揺らぎプモーション補正値
static float3 MMMob_RandomMove;

////////////////////////////////////////////////////////////////////////////////////////////////
// 複製モデルの配置座標変換行列
float4x4 MMMob_TransMatrixBase(int objIndex, int modelIndex)
{
    int i = (objIndex/MMMob_TEX_HEIGHT) * 8;
    int j = objIndex % MMMob_TEX_HEIGHT;

    float x = MMMob_TRANSFORM_X;
    float y = MMMob_TRANSFORM_Y;
    float z = MMMob_TRANSFORM_Z;
    float rx = MMMob_TRANSFORM_RX;
    float ry = MMMob_TRANSFORM_RY;
    float rz = MMMob_TRANSFORM_RZ;
    float scale = MMMob_TRANSFORM_SCALE;

    // 揺らぎ位置補正
    MMMob_RandomMove = float3(0.0f, 0.0f, 0.0f);
    #ifdef MMMob_FLAG_RANDMOVE
        float rand0 = MMMob_ColorToFloat(i+7, j);
        float3 rand = float3(frac((rand0+0.531f)*43.0f), rand0, frac((rand0+0.357f)*37.0f));
        float3 Random = float3(1.0f, 1.0f, 1.0f) - rand * MMMob_MorphRandMove * 0.5f;
        #ifdef MMMob_FLAG_RANDX
            MMMob_RandomMove.x = 10.0f * Random.x * MMMob_MorphXAmp
                          * sin(4.0f * 3.1416f * MMMob_MorphXFreq * Random.x * MMMob_RandTime + MMMob_MorphXPhase * 6.283f * rand.x);
            x += MMMob_RandomMove.x;
        #endif
        #ifdef MMMob_FLAG_RANDY
            #ifdef MMMob_FLAG_JUMP
            MMMob_RandomMove.y = 10.0f * Random.y * MMMob_MorphYAmp
                         * abs(sin(4.0f * 3.1416f * MMMob_MorphYFreq * Random.y * MMMob_RandTime + MMMob_MorphYPhase * 3.1416f * rand.y));
            #else
            MMMob_RandomMove.y = 10.0f * Random.y * MMMob_MorphYAmp
                         * sin(4.0f * 3.1416f * MMMob_MorphYFreq * Random.y * MMMob_RandTime + MMMob_MorphYPhase * 3.1416f * rand.y);
            #endif
            y += MMMob_RandomMove.y;
        #endif
        #ifdef MMMob_FLAG_RANDZ
            MMMob_RandomMove.z = 10.0f * Random.z * MMMob_MorphZAmp
                         * sin(4.0f * 3.1416f * MMMob_MorphZFreq * Random.z * MMMob_RandTime + MMMob_MorphZPhase * 6.283f * rand.z);
            z += MMMob_RandomMove.z;
        #endif
    #endif

    float sinx, cosx, siny, cosy, sinz, cosz;
    sincos(rx, sinx, cosx);
    sincos(ry, siny, cosy);
    sincos(rz, sinz, cosz);

    float4x4 TransMatrix;
    TransMatrix._11 = (cosy * cosz + sinx * siny * sinz) * scale;
    TransMatrix._12 = -cosx * sinz * scale;
    TransMatrix._13 = (siny * cosz - sinx * cosy * sinz) * scale;
    TransMatrix._14 = 0.0f;
    TransMatrix._21 = (cosy * sinz - sinx * siny * cosz) * scale;
    TransMatrix._22 = cosx * cosz * scale;
    TransMatrix._23 = (siny * sinz + sinx * cosy * cosz) * scale;
    TransMatrix._24 = 0.0f;
    TransMatrix._31 = -cosx * siny * scale;
    TransMatrix._32 = -sinx * scale;
    TransMatrix._33 = cosx * cosy * scale;
    TransMatrix._34 = 0.0f;
    TransMatrix._41 = x;
    TransMatrix._42 = y;
    TransMatrix._43 = z;
    TransMatrix._44 = 1.0f;

    // 種モデルのオブジェクト位置
    float3 modelPos = MMMob_GetModelPos(modelIndex);

    // 全ての親ボーンWorldMatrixの逆行列
    float3x3 mat3x3_inv = transpose((float3x3)MMMob_ParentMat);
    float4x4 invParent = float4x4( mat3x3_inv[0], 0.0f, 
                                   mat3x3_inv[1], 0.0f, 
                                   mat3x3_inv[2], 0.0f, 
                                  -mul(MMMob_ParentMat._41_42_43, mat3x3_inv), 1.0f );
    invParent._41_42_43 -= modelPos;

    // 配置変換行列
    TransMatrix = mul( mul( invParent, TransMatrix ), MMMob_ParentMat );
    TransMatrix._41_42_43 -= MMMob_ORG_POS;

    #ifdef MMMob_FLAG_HEIGHTMAP
        // 高さマップによる配置変換
        #ifdef MMMob_FOLLOWED_OBJ
            float4 objPos = mul( float4(MMMob_CenterPos, 1.0f) + float4(mul(modelPos, (float3x3)MMMob_ParentMat), 0.0f), TransMatrix );
        #else
            float4 objPos = mul( float4(MMMob_ParentPos, 1.0f) + float4(mul(modelPos, (float3x3)MMMob_ParentMat), 0.0f), TransMatrix );
        #endif
        TransMatrix._42 += MMMob_PickupHeight(objPos.x, objPos.z);

        // 地形の傾きに対する回転
        float3x3 rotMat = MMMob_PickupRotMat(objPos.x, objPos.z, 2.5f);
        float3 pos0 = float3(objPos.x, TransMatrix._42, objPos.z);
        TransMatrix = mul( TransMatrix, float4x4(rotMat[0], 0.0f,
                                                 rotMat[1], 0.0f,
                                                 rotMat[2], 0.0f,
                                                 pos0-mul(pos0, rotMat), 1.0f) );
    #endif

    return TransMatrix;
}


// ここより上側で定義されている関数・変数・マクロの使用は推奨しない

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// エフェクトコード本体で使用する関数・マクロ定義

//--------------------------------------------------
// 複製モデルのオブジェクト番号を得る
//--------------------------------------------------
int MOB_ObjectIndex(int vIndex)
{
    #if MMMob_MODEL_COUNT > 1
    int modelIndex = MMMob_Div(vIndex*MMMob_MODEL_COUNT, MMMob_VERTEX_COUNT);
    #else
    int modelIndex = 0;
    #endif
    return (MMMob_MODEL_COUNT * MMMob_RepertIndex + modelIndex);
}

//--------------------------------------------------
// 複製モデルのオブジェクト毎の一様乱数を得る(0～1)
//--------------------------------------------------
float MOB_ObjectRandom(int vIndex)
{
    int objIndex = MOB_ObjectIndex(vIndex);
    int i = (objIndex/MMMob_TEX_HEIGHT) * 8;
    int j = objIndex % MMMob_TEX_HEIGHT;
    return MMMob_ColorToFloat(i+7, j);
}

//--------------------------------------------------
// 複製モデルの配置座標変換行列(頂点インデックスより指定)
//--------------------------------------------------
float4x4 MOB_TransMatrix(int vIndex)
{
    #if MMMob_MODEL_COUNT > 1
    int modelIndex = MMMob_Div(vIndex*MMMob_MODEL_COUNT, MMMob_VERTEX_COUNT);
    #else
    int modelIndex = 0;
    #endif
    int objIndex = MMMob_MODEL_COUNT * MMMob_RepertIndex + modelIndex;
    return MMMob_TransMatrixBase(objIndex, modelIndex);
}

//--------------------------------------------------
// 複製モデルの配置座標変換行列(オブジェクト番号より指定)
//--------------------------------------------------
float4x4 MOB_TransMatrixObjIndex(int objIndex)
{
    #if MMMob_MODEL_COUNT > 1
    int modelIndex = MMMob_Mod(objIndex, MMMob_MODEL_COUNT);
    #else
    int modelIndex = 0;
    #endif
    return MMMob_TransMatrixBase(objIndex, modelIndex);
}

//--------------------------------------------------
// 配置変換後の座標と法線を取得する関数
//--------------------------------------------------
// 配置変換後の座標･法線定義
struct MOB_TRANSFORM {
    float4 Pos;     // 複製モデル配置変換後の頂点座標
    float3 Normal;  // 複製モデル配置変換後の頂点法線
};

MOB_TRANSFORM MOB_TransformPositionNormal(float4 Pos, float3 Normal, int vIndex)
{
    MOB_TRANSFORM Out;

    // 複製モデルの配置座標変換
    float4x4 TransMatrix = MOB_TransMatrix(vIndex);
    Out.Pos = mul( Pos, TransMatrix );

    // 配置座標変換による頂点法線
    Out.Normal = normalize( mul( Normal, (float3x3)TransMatrix ) );

    return Out;
}

//--------------------------------------------------
// 配置変換後の座標のみを取得する関数
//--------------------------------------------------
float4 MOB_TransformPosition(float4 Pos, int vIndex)
{
    return mul( Pos, MOB_TransMatrix(vIndex) );
}

//--------------------------------------------------
// 法線方向に押し出したエッジ頂点座標を取得する関数
//--------------------------------------------------
float4 MOB_EdgePosition(float4 Pos, float3 Normal, float EdgeThick, float3 CamPos)
{
    #ifndef MIKUMIKUMOVING
    // カメラとの距離
    float dist = max( length( CamPos - Pos.xyz ), 5.0f );
    // 頂点を法線方向に押し出す
    if( MMMob_IsParth ){
        // パースペクティブon
        Pos.xyz += Normal * ( EdgeThick * pow( abs(dist), 0.9f ) * 0.0015f * pow( abs(2.4142f / MMMob_PrjMtx._22), 0.7f ) );
    }else{
        // パースペクティブoff
        Pos.xyz += Normal * ( EdgeThick * 0.0025f / MMMob_PrjMtx._11 );
    }
    #else
    // カメラとの距離
    float dist = distance(Pos.xyz, CamPos);
    // 頂点を法線方向に押し出す
    if (MMM_IsDinamicProjection){
        Pos += float4(Normal, 0) * EdgeThick * dist * MMM_GetDynamicFovEdgeRate(dist);
    }else{
        Pos += float4(Normal, 0) * EdgeThick * dist;
    }
    #endif

    return Pos;
}

//--------------------------------------------------
// 配置変換後のエッジ頂点座標を取得する関数
//--------------------------------------------------
float4 MOB_TransformEdgePosition(float4 Pos, float3 Normal, float EdgeThick, float3 CamPos, int vIndex)
{
    // 複製モデルの配置座標変換
    float4x4 TransMatrix = MOB_TransMatrix(vIndex);
    Pos = mul( Pos, TransMatrix );

    // 配置座標変換による頂点法線
    Normal = normalize( mul( Normal, (float3x3)TransMatrix ) );

    return MOB_EdgePosition(Pos, Normal, EdgeThick, CamPos);
}

//--------------------------------------------------
// 配置変換後のオブジェクト原点位置を取得する関数
//--------------------------------------------------
float3 MOB_TransformOriginPosition(int vIndex)
{
    // 種モデルのオブジェクト位置
    #if MMMob_MODEL_COUNT > 1
    int modelIndex = MMMob_Div(vIndex*MMMob_MODEL_COUNT, MMMob_VERTEX_COUNT);
    float3 modelPos = MMMob_ORG_POS + MMMob_GetModelPos(modelIndex);
    #else
    float3 modelPos = MMMob_ORG_POS;
    #endif
    modelPos -= MMMob_RandomMove;

    // 複製モデルの配置座標変換
    float4x4 TransMatrix = MOB_TransMatrix(vIndex);

    return mul( float4(modelPos, 1.0f), TransMatrix ).xyz;
}

//--------------------------------------------------
// マクロ定義
//--------------------------------------------------
#ifndef MIKUMIKUMOVING
    #define MOB_EDGE_TEXCOORD  MMMob_EDGE_TEXCOORD   // エッジ情報入力セマンテック
    #define MOB_EDGE_THICK     MMMob_EDGE_THICKNESS  // エッジの太さ
    #define MOB_EDGE_COLOR     MMMob_EDGE_COLOR      // エッジの色
#endif

#ifdef MMMob_DRAW_EDGE_HEAD
    #define MOB_DRAW_EDGE_HEAD   // オブジェクト先頭材質でエッジ描画する
#endif
#ifdef MMMob_DRAW_EDGE_TAIL
    #define MOB_DRAW_EDGE_TAIL   // オブジェクト最終材質でエッジ描画する
#endif

#define MOB_EDGE_ON       MMMob_EDGE_ON        // エッジを描画する材質番号
#define MOB_SRC_HEAD_MAT  MMMob_SRC_HEAD_MAT   // 種モデル各オブジェクト先頭の材質番号
#define MOB_SRC_TAIL_MAT  MMMob_SRC_TAIL_MAT   // 種モデル各オブジェクト最後の材質番号

#define MOB_TEX_WIDTH     MMMob_TEX_WIDTH      // モデル配置テクスチャピクセル幅
#define MOB_TEX_HEIGHT    MMMob_TEX_HEIGHT     // モデル配置テクスチャピクセル高さ

#define MOB_TEX_WIDTH2    MMMob_TEX_WIDTH2     // MOB_TEX_WIDTH*2
#define MOB_TEX_WIDTH4    MMMob_TEX_WIDTH4     // MOB_TEX_WIDTH*4
#define MOB_TEX_WIDTH8    MMMob_TEX_WIDTH8     // MOB_TEX_WIDTH*8
#define MOB_TEX_WIDTH16   MMMob_TEX_WIDTH16    // MOB_TEX_WIDTH*16
#define MOB_TEX_WIDTH32   MMMob_TEX_WIDTH32    // MOB_TEX_WIDTH*32
#define MOB_TEX_WIDTH64   MMMob_TEX_WIDTH64    // MOB_TEX_WIDTH*64
#define MOB_TEX_WIDTH128  MMMob_TEX_WIDTH128   // MOB_TEX_WIDTH*128

//--------------------------------------------------
// スクリプト定義
//--------------------------------------------------
#define MOB_LOOPSCRIPT_OBJECT       "LoopByCount=MMMob_RepertCount; LoopGetIndex=MMMob_RepertIndex; Pass=DrawObject; LoopEnd=;"
#define MOB_LOOPSCRIPT_OBJECT_EDGE  "LoopByCount=MMMob_RepertCount; LoopGetIndex=MMMob_RepertIndex; Pass=DrawObject; Pass=DrawEdge; LoopEnd=;"
#define MOB_LOOPSCRIPT_EDGE         "LoopByCount=MMMob_RepertCount; LoopGetIndex=MMMob_RepertIndex; Pass=DrawEdge; LoopEnd=;"
#define MOB_LOOPSCRIPT_SHADOW       "LoopByCount=MMMob_RepertCount; LoopGetIndex=MMMob_RepertIndex; Pass=DrawShadow; LoopEnd=;"
#define MOB_LOOPSCRIPT_ZPLOT        "LoopByCount=MMMob_RepertCount; LoopGetIndex=MMMob_RepertIndex; Pass=ZValuePlot; LoopEnd=;"
#define MOB_LOOPSCRIPT(n)           "LoopByCount=MMMob_RepertCount; LoopGetIndex=MMMob_RepertIndex;" n "LoopEnd=;"


////////////////////////////////////////////////////////////////////////////////////////////////
// 以下エフェクト本体コード

////////////////////////////////////////////////////////////////////////////////////////////////

float AlphaClip : CONTROLOBJECT < string name = "(self)"; string item = "αｸﾘｯﾌﾟ"; >;
static float AlphaThreshold = lerp(DefAlphaThreshold, 0.99f, AlphaClip);

float Script : STANDARDSGLOBAL <
    string ScriptOutput = "color";
    string ScriptClass = "sceneorobject";
    string ScriptOrder = "standard";
> = 0.8;

// 座標変換行列
float4x4 WorldViewProjMatrix : WORLDVIEWPROJECTION;
float4x4 WorldMatrix         : WORLD;
float4x4 ViewMatrix          : VIEW;
float4x4 ViewProjMatrix      : VIEWPROJECTION;
float4x4 LightViewProjMatrix : VIEWPROJECTION < string Object = "Light"; >;

// ライト方向,カメラ位置
float3 LightDirection : DIRECTION < string Object = "Light"; >;
float3 CameraPosition : POSITION  < string Object = "Camera"; >;

// マテリアル色
float4 MaterialDiffuse   : DIFFUSE  < string Object = "Geometry"; >;
float3 MaterialAmbient   : AMBIENT  < string Object = "Geometry"; >;
float3 MaterialEmmisive  : EMISSIVE < string Object = "Geometry"; >;
float3 MaterialSpecular  : SPECULAR < string Object = "Geometry"; >;
float  SpecularPower     : SPECULARPOWER < string Object = "Geometry"; >;
float3 MaterialToon      : TOONCOLOR;
float4 GroundShadowColor : GROUNDSHADOWCOLOR;
// ライト色
float3 LightDiffuse  : DIFFUSE  < string Object = "Light"; >;
float3 LightAmbient  : AMBIENT  < string Object = "Light"; >;
float3 LightSpecular : SPECULAR < string Object = "Light"; >;
static float4 DiffuseColor  = MaterialDiffuse  * float4(LightDiffuse, 1.0f);
static float3 AmbientColor  = MaterialAmbient  * LightAmbient + MaterialEmmisive;
static float3 SpecularColor = MaterialSpecular * LightSpecular;

// テクスチャ材質モーフ値
float4 TextureAddValue : ADDINGTEXTURE;
float4 TextureMulValue : MULTIPLYINGTEXTURE;
float4 SphereAddValue  : ADDINGSPHERETEXTURE;
float4 SphereMulValue  : MULTIPLYINGSPHERETEXTURE;

bool use_texture;       // テクスチャの有無
bool use_spheremap;     // スフィアマップの有無
bool use_subtexture;    // サブテクスチャフラグ

bool parthf;   // パースペクティブフラグ
bool transp;   // 半透明フラグ
bool spadd;    // スフィアマップ加算合成フラグ
#define SKII1  1500
#define SKII2  8000
#define Toon   3

// オブジェクトのテクスチャ
texture ObjectTexture: MATERIALTEXTURE;
sampler ObjTexSampler = sampler_state {
    texture = <ObjectTexture>;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
    MIPFILTER = LINEAR;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
};

// スフィアマップのテクスチャ
texture ObjectSphereMap: MATERIALSPHEREMAP;
sampler ObjSphareSampler = sampler_state {
    texture = <ObjectSphereMap>;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
    MIPFILTER = LINEAR;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
};

// トゥーンマップのテクスチャ
texture ObjectToonTexture: MATERIALTOONTEXTURE;
sampler ObjToonSampler = sampler_state {
    texture = <ObjectToonTexture>;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
    MIPFILTER = NONE;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// 輪郭描画

struct VS_EDGE {
    float4 Pos       : POSITION; // 射影変換座標
    float4 EdgeColor : COLOR0;   // エッジ色
};

// 頂点シェーダ
VS_EDGE Edge_VS(float4 Pos : POSITION, float3 Normal : NORMAL, MOB_EDGE_TEXCOORD, int vIndex : _INDEX)
{
    VS_EDGE Out = (VS_EDGE)0;

    // 種モデルのワールド座標変換
    Pos = mul( Pos, WorldMatrix );

    // ワールド座標変換による頂点法線
    Normal = mul( Normal, (float3x3)WorldMatrix );

    // 配置変換後のエッジ頂点座標
    Pos = MOB_TransformEdgePosition(Pos, Normal, MOB_EDGE_THICK, CameraPosition, vIndex);

    // カメラ視点のビュー射影変換
    Out.Pos = mul( Pos, ViewProjMatrix );

    // エッジ色
    Out.EdgeColor = MOB_EDGE_COLOR;

    return Out;
}

// ピクセルシェーダ
float4 Edge_PS(VS_EDGE IN) : COLOR
{
    // 輪郭色で塗りつぶし
    return IN.EdgeColor;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// 非セルフシャドウ地面影描画

// 頂点シェーダ
float4 Shadow_VS(float4 Pos : POSITION, int vIndex : _INDEX) : POSITION
{
    // 複製モデルの配置座標変換
    Pos = MOB_TransformPosition(Pos, vIndex);

    // カメラ視点のワールドビュー射影変換(アクセサリは正しい位置への変換は出来ない)
    return  mul( Pos, WorldViewProjMatrix );
}

// ピクセルシェーダ
float4 Shadow_PS() : COLOR
{
    // 地面影色で塗りつぶし
    return GroundShadowColor;
}

// 影描画用テクニック
technique ShadowTec < string MMDPass = "shadow"; string Script = MOB_LOOPSCRIPT_SHADOW; >
{
    pass DrawShadow {
        VertexShader = compile vs_3_0 Shadow_VS();
        PixelShader  = compile ps_3_0 Shadow_PS();
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////
// オブジェクト描画（セルフシャドウOFF）

struct VS_OUTPUT {
    float4 Pos    : POSITION;   // 射影変換座標
    float2 Tex    : TEXCOORD1;  // テクスチャ
    float3 Normal : TEXCOORD2;  // 法線
    float3 Eye    : TEXCOORD3;  // カメラとの相対位置
    float2 SpTex  : TEXCOORD4;  // スフィアマップテクスチャ座標
    float4 Color  : COLOR0;     // ディフューズ色
};

// 頂点シェーダ
VS_OUTPUT Basic_VS(float4 Pos : POSITION, float3 Normal : NORMAL, float2 Tex : TEXCOORD0, float2 Tex2 : TEXCOORD1, int vIndex : _INDEX, uniform bool useToon)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    // 種モデルのワールド座標変換
    Pos = mul( Pos, WorldMatrix );

    // ワールド座標変換による頂点法線
    Normal = mul( Normal, (float3x3)WorldMatrix );

    // 配置変換後の座標と法線を取得
    MOB_TRANSFORM TrOut = MOB_TransformPositionNormal(Pos, Normal, vIndex);

    // 配置変換後の頂点法線
    Out.Normal = TrOut.Normal;

    // カメラ視点のビュー射影変換
    Out.Pos = mul( TrOut.Pos, ViewProjMatrix );

    // カメラとの相対位置
    Out.Eye = CameraPosition - TrOut.Pos.xyz;

    // ディフューズ色＋アンビエント色 計算
    Out.Color.rgb = AmbientColor;
    if( !useToon ) {
        Out.Color.rgb += max(0, dot( Out.Normal, -LightDirection )) * DiffuseColor.rgb;
    }
    Out.Color.a = DiffuseColor.a;
    Out.Color = saturate( Out.Color );

    // テクスチャ座標
    Out.Tex = Tex;

    if( use_spheremap ) {
        if( use_subtexture ) {
            // PMXサブテクスチャ座標
            Out.SpTex = Tex2;
        } else {
            // スフィアマップテクスチャ座標
            float2 NormalWV = mul( Out.Normal, (float3x3)ViewMatrix ).xy;
            Out.SpTex.x = NormalWV.x * 0.5f + 0.5f;
            Out.SpTex.y = NormalWV.y * -0.5f + 0.5f;
        }
    }

    return Out;
}

// ピクセルシェーダ
float4 Basic_PS(VS_OUTPUT IN, uniform bool useToon) : COLOR0
{
    // スペキュラ色計算
    float3 HalfVector = normalize( normalize(IN.Eye) + -LightDirection );
    float3 Specular = pow( max(0, dot( HalfVector, normalize(IN.Normal) )), SpecularPower ) * SpecularColor;

    float4 Color = IN.Color;
    if( use_texture ) {
        // テクスチャ適用
        Color *= tex2D( ObjTexSampler, IN.Tex );
        // α値一定以下のテクスチャ透過部は表示しない
        clip(Color.a - AlphaThreshold);
    }
    if( use_spheremap ) {
        // スフィアマップ適用
        float4 TexColor = tex2D(ObjSphareSampler,IN.SpTex);
        if(spadd) Color.rgb += TexColor.rgb;
        else      Color.rgb *= TexColor.rgb;
        Color.a *= TexColor.a;
    }

    if( useToon ) {
        // トゥーン適用
        float LightNormal = dot( IN.Normal, -LightDirection );
        Color *= tex2D( ObjToonSampler, float2(0.0f, 0.5f-LightNormal*0.5f) );
    }

    // スペキュラ適用
    Color.rgb += Specular;

    return Color;
}

///////////////////////////////////////////////////////////////////////////////////////////////

// オブジェクト描画用テクニック（アクセサリ用）
technique MainTec0 < string MMDPass = "object"; bool UseToon = false; string Script = MOB_LOOPSCRIPT_OBJECT; >
{
    pass DrawObject {
        VertexShader = compile vs_3_0 Basic_VS(false);
        PixelShader  = compile ps_3_0 Basic_PS(false);
    }
}

// オブジェクト描画用テクニック（PMXモデル用,エッジ有り）
technique MainTec1 < string MMDPass = "object"; string Subset = MOB_EDGE_ON; bool UseToon = true;
                     string Script = MOB_LOOPSCRIPT_OBJECT_EDGE; >
{
    pass DrawObject {
        VertexShader = compile vs_3_0 Basic_VS(true);
        PixelShader  = compile ps_3_0 Basic_PS(true);
    }
    pass DrawEdge {
        CullMode = CW;
        VertexShader = compile vs_3_0 Edge_VS();
        PixelShader  = compile ps_3_0 Edge_PS();
    }
}

// オブジェクト描画用テクニック（PMXモデル用,エッジ無し）
technique MainTec2 < string MMDPass = "object"; bool UseToon = true; string Script = MOB_LOOPSCRIPT_OBJECT; >
{
    pass DrawObject {
        VertexShader = compile vs_3_0 Basic_VS(true);
        PixelShader  = compile ps_3_0 Basic_PS(true);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// セルフシャドウ用Z値プロット

struct VS_ZValuePlot_OUTPUT {
    float4 Pos          : POSITION;   // 射影変換座標
    float4 ShadowMapTex : TEXCOORD0;  // Zバッファテクスチャ
};

// 頂点シェーダ
VS_ZValuePlot_OUTPUT ZValuePlot_VS( float4 Pos : POSITION, int vIndex : _INDEX )
{
    VS_ZValuePlot_OUTPUT Out = (VS_ZValuePlot_OUTPUT)0;

    // 種モデルのワールド座標変換
    Pos = mul( Pos, WorldMatrix );

    // 複製モデルの配置座標変換
    Pos = MOB_TransformPosition(Pos, vIndex);

    // ライトの目線によるビュー射影変換
    Out.Pos = mul( Pos, LightViewProjMatrix );

    // テクスチャ座標を頂点に合わせる
    Out.ShadowMapTex = Out.Pos;

    return Out;
}

// ピクセルシェーダ
float4 ZValuePlot_PS( float4 ShadowMapTex : TEXCOORD0 ) : COLOR
{
    // R色成分にZ値を記録する
    return float4(ShadowMapTex.z/ShadowMapTex.w, 0, 0, 1);
}

// Z値プロット用テクニック
technique ZplotTec < string MMDPass = "zplot"; string Script = MOB_LOOPSCRIPT_ZPLOT; >
{
    pass ZValuePlot {
        AlphaBlendEnable = FALSE;
        VertexShader = compile vs_3_0 ZValuePlot_VS();
        PixelShader  = compile ps_3_0 ZValuePlot_PS();
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////
// オブジェクト描画（セルフシャドウON）

// シャドウバッファのサンプラ。"register(s0)"なのはMMDがs0を使っているから
sampler DefSampler : register(s0);

struct BufferShadow_OUTPUT {
    float4 Pos      : POSITION;   // 射影変換座標
    float4 ZCalcTex : TEXCOORD0;  // Z値
    float2 Tex      : TEXCOORD1;  // テクスチャ
    float3 Normal   : TEXCOORD2;  // 法線
    float3 Eye      : TEXCOORD3;  // カメラとの相対位置
    float2 SpTex    : TEXCOORD4;  // スフィアマップテクスチャ座標
    float4 Color    : COLOR0;     // ディフューズ色
};

// 頂点シェーダ
BufferShadow_OUTPUT BufferShadow_VS(float4 Pos : POSITION, float3 Normal : NORMAL, float2 Tex : TEXCOORD0, float2 Tex2 : TEXCOORD1, int vIndex : _INDEX, uniform bool useToon)
{
    BufferShadow_OUTPUT Out = (BufferShadow_OUTPUT)0;

    // 種モデルのワールド座標変換
    Pos = mul( Pos, WorldMatrix );

    // ワールド座標変換による頂点法線
    Normal = mul( Normal, (float3x3)WorldMatrix );

    // 配置変換後の座標と法線を取得
    MOB_TRANSFORM TrOut = MOB_TransformPositionNormal(Pos, Normal, vIndex);

    // 配置変換後の頂点法線
    Out.Normal = TrOut.Normal;

    // カメラ視点のビュー射影変換
    Out.Pos = mul( TrOut.Pos, ViewProjMatrix );

    // カメラとの相対位置
    Out.Eye = CameraPosition - TrOut.Pos.xyz;

    // ライト視点によるビュー射影変換
    Out.ZCalcTex = mul( TrOut.Pos, LightViewProjMatrix );

    // ディフューズ色＋アンビエント色 計算
    Out.Color.rgb = AmbientColor;
    if ( !useToon ) {
        Out.Color.rgb += max(0, dot( Out.Normal, -LightDirection )) * DiffuseColor.rgb;
    }
    Out.Color.a = DiffuseColor.a;
    Out.Color = saturate( Out.Color );

    // テクスチャ座標
    Out.Tex = Tex;

    if( use_spheremap ) {
        if( use_subtexture ) {
            // PMXサブテクスチャ座標
            Out.SpTex = Tex2;
        } else {
            // スフィアマップテクスチャ座標
            float2 NormalWV = mul( Out.Normal, (float3x3)ViewMatrix ).xy;
            Out.SpTex.x = NormalWV.x * 0.5f + 0.5f;
            Out.SpTex.y = NormalWV.y * -0.5f + 0.5f;
        }
    }

    return Out;
}

// ピクセルシェーダ
float4 BufferShadow_PS(BufferShadow_OUTPUT IN, uniform bool useToon) : COLOR
{
    // スペキュラ色計算
    float3 HalfVector = normalize( normalize(IN.Eye) + -LightDirection );
    float3 Specular = pow( max(0, dot( HalfVector, normalize(IN.Normal) )), SpecularPower ) * SpecularColor;

    float4 Color = IN.Color;
    float4 ShadowColor = float4(saturate(AmbientColor), Color.a);  // 影の色
    if( use_texture ) {
        // テクスチャ適用
        float4 TexColor = tex2D( ObjTexSampler, IN.Tex );
        // テクスチャ材質モーフ数
        TexColor.rgb = lerp(1, TexColor * TextureMulValue + TextureAddValue, TextureMulValue.a + TextureAddValue.a).rgb;
        Color *= TexColor;
        ShadowColor *= TexColor;
        // α値一定以下のテクスチャ透過部は表示しない
        clip(Color.a - AlphaThreshold);
    }
    if( use_spheremap ) {
        // スフィアマップ適用
        float4 TexColor = tex2D(ObjSphareSampler,IN.SpTex);
        // スフィアテクスチャ材質モーフ数
        TexColor.rgb = lerp(spadd?0:1, TexColor * SphereMulValue + SphereAddValue, SphereMulValue.a + SphereAddValue.a).rgb;
        if(spadd) {
            Color.rgb += TexColor.rgb;
            ShadowColor.rgb += TexColor.rgb;
        } else {
            Color.rgb *= TexColor.rgb;
            ShadowColor.rgb *= TexColor.rgb;
        }
        Color.a *= TexColor.a;
        ShadowColor.a *= TexColor.a;
    }
    // スペキュラ適用
    Color.rgb += Specular;

    // テクスチャ座標に変換
    IN.ZCalcTex /= IN.ZCalcTex.w;
    float2 TransTexCoord;
    TransTexCoord.x = (1.0f + IN.ZCalcTex.x)*0.5f;
    TransTexCoord.y = (1.0f - IN.ZCalcTex.y)*0.5f;

    if( any( saturate(TransTexCoord) - TransTexCoord ) ) {
        // シャドウバッファ外
        return Color;
    } else {
        float comp;
        if(parthf) {
            // セルフシャドウ mode2
            comp=1-saturate(max(IN.ZCalcTex.z-tex2D(DefSampler,TransTexCoord).r , 0.0f)*SKII2*TransTexCoord.y-0.3f);
        } else {
            // セルフシャドウ mode1
            comp=1-saturate(max(IN.ZCalcTex.z-tex2D(DefSampler,TransTexCoord).r , 0.0f)*SKII1-0.3f);
        }
        if ( useToon ) {
            // トゥーン適用
            comp = min(saturate(dot(IN.Normal,-LightDirection)*Toon),comp);
            ShadowColor.rgb *= MaterialToon;
        }
        
        float4 ans = lerp(ShadowColor, Color, comp);
        if( transp ) ans.a = 0.5f;
        return ans;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

// オブジェクト描画用テクニック（アクセサリ用）
technique MainTecBS0  < string MMDPass = "object_ss"; bool UseToon = false; string Script = MOB_LOOPSCRIPT_OBJECT; >
{
    pass DrawObject {
        VertexShader = compile vs_3_0 BufferShadow_VS(false);
        PixelShader  = compile ps_3_0 BufferShadow_PS(false);
    }
}

// オブジェクト描画用テクニック（PMXモデル用,エッジ有り）
technique MainTecBS1  < string MMDPass = "object_ss"; string Subset = MOB_EDGE_ON; bool UseToon = true;
                        string Script = MOB_LOOPSCRIPT_OBJECT_EDGE; >
{
    pass DrawObject {
        VertexShader = compile vs_3_0 BufferShadow_VS(true);
        PixelShader  = compile ps_3_0 BufferShadow_PS(true);
    }
    pass DrawEdge {
        CullMode = CW;
        VertexShader = compile vs_3_0 Edge_VS();
        PixelShader  = compile ps_3_0 Edge_PS();
    }
}

// オブジェクト描画用テクニック（PMXモデル用,エッジ無し）
technique MainTecBS2  < string MMDPass = "object_ss"; bool UseToon = true; string Script = MOB_LOOPSCRIPT_OBJECT; >
{
    pass DrawObject {
        VertexShader = compile vs_3_0 BufferShadow_VS(true);
        PixelShader  = compile ps_3_0 BufferShadow_PS(true);
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////

