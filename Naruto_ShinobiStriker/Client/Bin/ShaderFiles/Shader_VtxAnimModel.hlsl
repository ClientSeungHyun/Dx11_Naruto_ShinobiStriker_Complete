#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_MaskTexture;
texture2D g_NormalTexture;
texture2D g_DissloveTexture;

vector g_vLightDir;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.0f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

float4 g_vColor = float4(0.f, 0.f, 0.f, 0.f);
float g_fAlpha = 1.f;
vector g_vCamPosition;

float g_fDissolveRatio = 1.f;

/* 뼈행렬들(내 모델 전체의 뼈행렬들(x), 현재 그리는 메시에게 영향을 주는 뼈행렬들(o) */
matrix g_BoneMatrices[512];

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;

    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vVertexWorlPos : TEXCOORD3;
};

VS_OUT VS_MAIN( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
		
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
	
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vVertexWorlPos = mul(vPosition, g_WorldMatrix); //버텍스 월드 위치
    
    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vVertexWorlPos : TEXCOORD3;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    //vector vMtrlNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);    
    
    if (0.3f > vMtrlDiffuse.a)
        discard;    
    
    float3 g_vRimLightColor = float3(0.f, 0.f, 0.f);
    float fRimPower = 5.f;
   
    vector vEmissive = vector(1.f, 1.f, 1.f, 1.f);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    /* x : w나누기를 한 값 0~1  y: 정규화된 z값 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

struct PS_OUT_LIGHTDEPTH
{
    vector vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_LIGHTDEPTH(PS_IN In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;

    Out.vLightDepth = vector(In.vProjPos.w / 2000.f, 0.f, 0.f, 0.f);

    
    return Out;
}


PS_OUT PS_MAIN_MASK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    vMtrlDiffuse.xyz += g_vColor.xyz;
    
    if (0.3f > vMtrlDiffuse.a)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_SUSANOO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDisslove= g_DissloveTexture.Sample(LinearSampler, In.vTexcoord);    
    
    if (0.3f > vMtrlDiffuse.a)
        discard;
    
    if (g_fDissolveRatio > vDisslove.r)
        discard;
    
    float3 g_vRimLightColor = float3(0.f, 0.f, 0.f);
    float fRimPower = 5.f;
   
    vector vEmissive = vector(1.f, 1.f, 1.f, 1.f);
    
    Out.vDiffuse = vMtrlDiffuse * vEmissive;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    /* x : w나누기를 한 값 0~1  y: 정규화된 z값 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_MOTIONTRAIL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    //vector vMtrlNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);    
    
    if (0.3f > vMtrlDiffuse.a)
        discard;
    
    float3 g_vRimLightColor = float3(0.f, 0.f, 0.f);
    float fRimPower = 5.f;
   
    vector vEmissive = vector(1.f, 1.f, 1.f, 1.f);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vDiffuse.a = Out.vDiffuse.a  * g_fAlpha;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    /* x : w나누기를 한 값 0~1  y: 정규화된 z값 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass AnimModel
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass AnimModel_Mask
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MASK();
    }

    pass Light_Depth
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LIGHTDEPTH();
    }

    pass AnimModel_Sasanoo
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SUSANOO();
    }

    pass MotionTrail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MOTIONTRAIL();
    }
}