
#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
vector g_vCamPosition;

float3 g_vWorldPos;

float4 g_vColor;

struct VS_IN
{
    float4 vRight : TEXCOORD0;
    float4 vUp : TEXCOORD1;
    float4 vLook : TEXCOORD2;
    float4 vTranslation : TEXCOORD3;
};

struct VS_OUT
{
    float4 vRight : TEXCOORD0;
    float4 vUp : TEXCOORD1;
    float4 vLook : TEXCOORD2;
    float4 vTranslation : TEXCOORD3;
};

/* 1. 정점의 변환과정을 수행한다. */
/* 2. 정점의 구성 정보를 변형한다. */
VS_OUT VS_MAIN( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    Out.vRight = In.vRight;
    Out.vUp = In.vUp;
    Out.vLook = In.vLook;
    Out.vTranslation = In.vTranslation;
    
    return Out;
}

struct GS_IN
{
	/* SV_ : ShaderValue */
	/* 내가 해야할 연산은 모두 마쳐놓은 것이므로 이후 dx가 추가적으로 해야할 이릉ㄹ 해라. */
    float4 vRight : TEXCOORD0;
    float4 vUp : TEXCOORD1;
    float4 vLook : TEXCOORD2;
    float4 vTranslation : TEXCOORD3;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vWorldPosition : TEXCOORD0;
    float4 vCenterPos : TEXCOORD1;
};

// void GS_MAIN(triangle GS_IN In[3], )
[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
    GS_OUT Out[4];
    Out[0].vPosition = float4(In[0].vRight.xyz, 1.f);
    Out[1].vPosition = float4(In[0].vUp.xyz, 1.f);
    Out[2].vPosition = float4(In[0].vLook.xyz, 1.f);
    Out[3].vPosition = float4(In[0].vTranslation.xyz, 1.f);
    
    Out[0].vWorldPosition = float4(In[0].vRight.xyz, 1.f);
    Out[1].vWorldPosition = float4(In[0].vUp.xyz, 1.f);
    Out[2].vWorldPosition = float4(In[0].vLook.xyz, 1.f);
    Out[3].vWorldPosition = float4(In[0].vTranslation.xyz, 1.f);
    
    for (int i = 0; i < 4; ++i)
    {
        Out[i].vCenterPos = (Out[0].vWorldPosition + Out[1].vWorldPosition + Out[2].vWorldPosition + Out[3].vWorldPosition) / 4.f;
    }
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);

    Container.Append(Out[0]);
    Container.Append(Out[1]);
    Container.Append(Out[2]);
    Container.RestartStrip();

    Container.Append(Out[0]);
    Container.Append(Out[2]);
    Container.Append(Out[3]);
    Container.RestartStrip();
}

/* Triangle : 정점 세개가 다 vs_main을 통과할때까지 대기 */
/* 세개가 모두다 통과되면. 밑의 과정을 수행. */
/* 리턴된 정점의 w로 정점의 xyzw를 나눈다. 투영 */
/* 정점의 위치를 뷰포트로 변환다. (윈도우좌표로 변환한다)*/
/* 래스터라이즈 : 정점정보를 기반으로하여 픽셀이 만들어진다. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vWorldPosition : TEXCOORD0;
    float4 vCenterPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};


/* 1. 픽셀의 최종적인 색상을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    //// 1. 중심점 위치와 현재 픽셀 위치를 사용하여 거리 계산
    //float3 centerPos = In.vCenterPos.xyz;
    //float fDist = length(centerPos - In.vWorldPosition.xyz);
    
    //// 2. 거리 정규화
    //float maxDist = 2.0f; // 필요에 따라 최대 거리 조정
    //float normalizedDist = saturate(fDist / maxDist); // 0~1로 정규화

    //// 3. 색상 조정: 거리 값이 작을수록 흰색으로
    //float4 whiteColor = float4(1.0, 1.0, 1.0, 1.0); // 흰색
    //Out.vColor = lerp(g_vColor, whiteColor, fDist * 10.f); // 거리 값에 따라 색상 혼합
    
    Out.vColor = g_vColor;
    
    return Out;
}



technique11 DefaultTechnique
{
	/* 빛연산 + 림라이트 + ssao + 노멀맵핑 + pbr*/
    pass FootTrail
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
	/* 디스토션 + 블렌딩 */
	//pass Effect
	//{
	//	VertexShader = compile vs_5_0 VS_MAIN_Special();
	//	PixelShader = compile ps_5_0 PS_MAIN_Special();
	//}
}