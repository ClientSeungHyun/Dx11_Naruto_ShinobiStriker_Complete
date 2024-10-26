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


VS_OUT VS_MAIN( /*����*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* ������ ��ġ�� ���� �� ������ȯ�Ѵ�.*/		
	/* Ŭ���̾�Ʈ���� ���������� �����ϴ� TrnasformCoord�Լ��ʹ� �ٸ��� */
	/* mul�Լ��� ��쿡�� �����ϰ� ����� ���ϱ⸸ ������ �ϰ� w�����⿬����ü�� �������� �ʴ´�. */
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

/* Triangle : ���� ������ �� vs_main�� ����Ҷ����� ��� */
/* ������ ��δ� ����Ǹ�. ���� ������ ����. */
/* ���ϵ� ������ w�� ������ xyzw�� ������. ���� */
/* ������ ��ġ�� ����Ʈ�� ��ȯ��. (��������ǥ�� ��ȯ�Ѵ�)*/
/* �����Ͷ����� : ���������� ��������Ͽ� �ȼ��� ���������. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
	
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

/* 1. �ȼ��� �������� ������ �����Ѵ�. */
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
    
    // ��� ���
    float brightness = dot(vDiffuse.rgb, float3(0.299, 0.587, 0.114)); // YUV �������� ��� ��� ���

    // �Ӱ谪 ����
    float bloomThreshold = 0.1f; // �� ���� �����Ͽ� bloom ȿ���� �ΰ����� �����մϴ�

    // bloom curve ����
    float bloomValue = max(0, brightness - bloomThreshold); // �Ӱ谪 �ʰ� �� bloom �� ����
    bloomValue = pow(bloomValue, 2.0); // curve ���� (��: ����)
    bloomValue = min(bloomValue, 1.0);

// ���� ���� ��⸦ �������� bloom ���� ����
    float3 brightColor = vDiffuse.rgb * (1.0 + bloomValue * 0.5); // bloom ���� ����
    if (brightness < 0.1)
    {
        brightColor = vDiffuse.rgb; // ��ο� �κ��� ���� ���� ����
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


