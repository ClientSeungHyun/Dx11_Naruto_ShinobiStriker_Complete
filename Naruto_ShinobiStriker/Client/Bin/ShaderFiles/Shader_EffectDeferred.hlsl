#include "Shader_Engine_Defines.hlsli"

matrix          g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix          g_ViewMatrixInv, g_ProjMatrixInv;

texture2D       g_Texture;
texture2D       g_DiffuseTexture;
texture2D       g_DepthTexture;
texture2D       g_BackTexture;
texture2D       g_FinalTexture;
texture2D       g_BlurXTexture;
texture2D       g_BlurYTexture;

texture2D g_DownSampleTexture;

vector          g_vCamPosition;


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};


VS_OUT VS_MAIN( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* 정점에 위치를 월드 뷰 투영변환한다.*/		
	/* 클라이언트에서 곱셈연산을 수행하는 TrnasformCoord함수와는 다르게 */
	/* mul함수의 경우에는 순수하게 행렬의 곱하기만 수행을 하고 w나누기연산자체는 수행하지 않는다. */
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

/* Triangle : 정점 세개가 다 vs_main을 통과할때까지 대기 */
/* 세개가 모두다 통과되면. 밑의 과정을 수행. */
/* 리턴된 정점의 w로 정점의 xyzw를 나눈다. 투영 */
/* 정점의 위치를 뷰포트로 변환다. (윈도우좌표로 변환한다)*/
/* 래스터라이즈 : 정점정보를 기반으로하여 픽셀이 만들어진다. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
	
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

/* 1. 픽셀의 최종적인 색상을 결정한다. */
PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vBack = g_BackTexture.Sample(LinearSampler, In.vTexcoord);
    
    // 밝기 계산
    float brightness = dot(vDiffuse.rgb, float3(0.299, 0.587, 0.114)); // YUV 색공간의 밝기 계수 사용

    // 임계값 설정
    float bloomThreshold = 0.1f; // 이 값을 조정하여 bloom 효과의 민감도를 설정합니다

    // bloom curve 적용
    float bloomValue = max(0, brightness - bloomThreshold); // 임계값 초과 시 bloom 값 추출
    bloomValue = pow(bloomValue, 2.0); // curve 적용 (예: 제곱)
    bloomValue = min(bloomValue, 1.0);

// 원본 색의 밝기를 기준으로 bloom 값을 조정
    float3 brightColor = vDiffuse.rgb * (1.0 + bloomValue * 0.5); // bloom 비율 조정
    if (brightness < 0.1)
    {
        brightColor = vDiffuse.rgb; // 어두운 부분은 원래 색상 유지
    }
    
    Out.vColor.xyz = vBack.xyz + brightColor.xyz;
    Out.vColor.a = vDiffuse.z;
    return Out;
}

PS_OUT PS_MAIN_DOWNSAMPLING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 scaledTexcoord = In.vTexcoord * 32.f;
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, scaledTexcoord);
    
    Out.vColor = vDiffuse;
    
    return Out;
}

float g_fWeights[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1.f, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};



PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vBlurUV = (float2) 0.f;

    for (int i = -6; i < 7; i++)
    {
        vBlurUV = In.vTexcoord * 32.f + float2(1.f / 45.f * i, 0.f);
        vector vColor = g_FinalTexture.Sample(LinearClampSampler, vBlurUV);
        Out.vColor += g_fWeights[i + 6] * vColor;
		
    }

    Out.vColor.rgb /= 5.11f;

    return Out;
}

PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vBlurUV = (float2) 0.f;

    for (int i = -6; i < 7; i++)
    {
        vBlurUV = In.vTexcoord * 32.f + float2(0.f, 1.f / 22.5f * i);
        vector vColor = g_BlurXTexture.Sample(LinearClampSampler, vBlurUV);
        Out.vColor += g_fWeights[i + 6] * vColor;

    }

    Out.vColor.rgb /= 5.11f;

    return Out;
}

PS_OUT PS_MAIN_BLUR_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vBlur = g_BlurYTexture.Sample(LinearSampler, In.vTexcoord);
    vector vBack = g_BackTexture.Sample(LinearSampler, In.vTexcoord);
    

    Out.vColor = vDiffuse + vBlur;

    return Out;
}

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Deferred
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }

    pass DownSampling
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DOWNSAMPLING();
    }

    pass Blur_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass Blur_Y
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

    pass BlurFinal
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_FINAL();
    }
}


