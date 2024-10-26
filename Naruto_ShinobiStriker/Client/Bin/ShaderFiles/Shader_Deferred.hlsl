#include "Shader_Engine_Defines.hlsli"

matrix          g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix          g_ViewMatrixInv, g_ProjMatrixInv;
matrix          g_LightViewMatrix, g_LightProjMatrix;
matrix          g_StaticLightViewMatrix, g_StaticLightProjMatrix;
texture2D       g_Texture;


vector          g_vLightDir;
vector          g_vLightPos;
float           g_fLightRange;

vector          g_vLightDiffuse;
vector          g_vLightAmbient;
vector          g_vLightSpecular;
texture2D       g_NormalTexture;

texture2D       g_DiffuseTexture;
vector          g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector          g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
texture2D       g_ShadeTexture;
texture2D       g_DepthTexture;
texture2D       g_SpecularTexture;
texture2D       g_LightDepthTexture;
texture2D       g_StaticLightDepthTexture;
texture2D       g_BackTexture;
texture2D       g_FinalTexture;
texture2D       g_BlurXTexture;
texture2D       g_BlurYTexture;

vector          g_vCamPosition;

float g_fWeights[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1.f, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

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

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_Target1;
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 2000.f;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    Out.vShade = floor(Out.vShade * 4.0f) / 4.0f; // �� ���� �ܰ�� ī�� ������
    
    vector vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));

    vector vPosition = (vector) 0;

	/* ������������ ȭ�鿡 �׷����� �ȼ��� ��ġ�� ���Ѵ�. */
	/* ������ġ * ������� * ����� * ������� / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* �佺���̽� ���� ȭ�鿡 �׷����� �ȼ��� ��ġ�� ���Ѵ�.*/
	/* ������ġ * ������� * �����  */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);

	/* ���� ���� ȭ�鿡 �׷����� �ȼ��� ��ġ�� ���Ѵ�.*/
    vPosition = mul(vPosition, g_ViewMatrixInv);

    vector vLook = vPosition - g_vCamPosition;

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 2000.f;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    vector vPosition = (vector) 0;

	/* ������������ ȭ�鿡 �׷����� �ȼ��� ��ġ�� ���Ѵ�. */
	/* ������ġ * ������� * ����� * ������� / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* �佺���̽� ���� ȭ�鿡 �׷����� �ȼ��� ��ġ�� ���Ѵ�.*/
	/* ������ġ * ������� * �����  */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);

	/* ���� ���� ȭ�鿡 �׷����� �ȼ��� ��ġ�� ���Ѵ�.*/
    vPosition = mul(vPosition, g_ViewMatrixInv);

    vector vLightDir = vPosition - g_vLightPos;
    vector vLook = normalize(vPosition - g_vCamPosition);
    
    float fAtt = saturate((g_fLightRange - length(vLightDir)) / g_fLightRange);

    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
    Out.vShade = floor(Out.vShade * 2.0f) / 2.0f + 0.1f;
    
    vector vReflect = reflect(normalize(vLightDir), normalize(vNormal));

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f) * fAtt;

    return Out;
}

PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.a == 0.f)
        discard;

    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);    
    
    // ���� �ؽ�ó ���ø�
    float fDepth = g_DepthTexture.Sample(LinearSampler, In.vTexcoord).y * 2000.f;
    
    // �ֺ� �ȼ��� ���� ���� ���ø�
    float fLeftDepth = g_DepthTexture.Sample(LinearSampler, In.vTexcoord + float2(-1.0 / 1280.f, 0)).y * 2000.f;
    float fRightDepth = g_DepthTexture.Sample(LinearSampler, In.vTexcoord + float2(1.0 / 1280.f, 0)).y * 2000.f;
    float fTopDepth = g_DepthTexture.Sample(LinearSampler, In.vTexcoord + float2(0, -1.0 / 720.f)).y * 2000.f;
    float fBottomDepth = g_DepthTexture.Sample(LinearSampler, In.vTexcoord + float2(0, 1.0 / 720.f)).y * 2000.f;

    // ���� ���� ���
    float fDepthValue = 1.5f; // ���� ������ �Ӱ谪
    float fEdgeValue = 0.0;
    if (abs(fDepth - fLeftDepth) > fDepthValue ||
        abs(fDepth - fRightDepth) > fDepthValue ||
        abs(fDepth - fTopDepth) > fDepthValue ||
        abs(fDepth - fBottomDepth) > fDepthValue)
    {
        fEdgeValue = 1.; // �ܰ��� ����
    }

    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 2000.f;

	/* 1. ���� �׷����� �ȼ��� ���������� ��ġ�� ��ȯ�ϱ����ؼ� �켱 ����� ��ġȯ�Ͽ� ������ġ�� ���Ѵ�. */
    vector vPosition = (vector) 0;

	/* ������������ ȭ�鿡 �׷����� �ȼ��� ��ġ�� ���Ѵ�. */
	/* ������ġ * ������� * ����� * ������� / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* �佺���̽� ���� ȭ�鿡 �׷����� �ȼ��� ��ġ�� ���Ѵ�.*/
	/* ������ġ * ������� * �����  */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);

	/* ���� ���� ȭ�鿡 �׷����� �ȼ��� ��ġ�� ���Ѵ�.*/
    vPosition = mul(vPosition, g_ViewMatrixInv);

    float fCamDistance = length(g_vCamPosition - vPosition);
        
    vector vLine = (fEdgeValue > 0.0 && fCamDistance < 20.f) ? float4(1.f, 1.f, 1.f, 1.f) : float4(0.f, 0.f, 0.f, 0.f); // �ܰ����� ������, ���δ� ���
    
    Out.vColor = vDiffuse * vShade - vLine; //+ vSpecular;
    
	/* 2. ������� �ȼ� ��ġ���ٰ� ������������ ������� ������� ���Ͽ� ���������� �����̽��� ��ȯ�Ѵ�. */
    vector vOldPos = mul(vPosition, g_LightViewMatrix);
    vector vStaticOldPos = mul(vPosition, g_StaticLightViewMatrix);
    vOldPos = mul(vOldPos, g_LightProjMatrix);
    vStaticOldPos = mul(vStaticOldPos, g_StaticLightProjMatrix);
    
    float fLightDepth = vOldPos.w;

    float2 vTexcoord;
    vTexcoord.x = (vOldPos.x / vOldPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (vOldPos.y / vOldPos.w) * -0.5f + 0.5f;

    float fOldLightDepth = g_LightDepthTexture.Sample(LinearSampler, vTexcoord).r * 2000.f;

    if (fLightDepth - 0.1f > fOldLightDepth)
        Out.vColor = vector(Out.vColor.rgb * 0.6f, Out.vColor.a);
    
    vTexcoord.x = (vStaticOldPos.x / vStaticOldPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (vStaticOldPos.y / vStaticOldPos.w) * -0.5f + 0.5f;
    fLightDepth = vStaticOldPos.w;
    float fStaticOldLightDepth = g_StaticLightDepthTexture.Sample(LinearSampler, vTexcoord).r * 2000.f;
    if (fLightDepth - 0.1f > fStaticOldLightDepth)
        Out.vColor = vector(Out.vColor.rgb * 0.6f, Out.vColor.a);
    
    return Out;
}

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_BackTexture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vBlurUV = (float2) 0.f;

    for (int i = -6; i < 7; i++)
    {
        vBlurUV = In.vTexcoord + float2(1.f / 1280.f * i, 0.f);
        Out.vColor += g_fWeights[i + 6] * g_FinalTexture.Sample(LinearSampler, vBlurUV);
		
    }

    Out.vColor /= 6.21f;

    return Out;
}

PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vBlurUV = (float2) 0.f;

    for (int i = -6; i < 7; i++)
    {
        vBlurUV = In.vTexcoord + float2(0.f, 1.f / 720.f * i);
        Out.vColor += g_fWeights[i + 6] * g_BlurXTexture.Sample(LinearSampler, vBlurUV);

    }

    Out.vColor /= 6.21f;

    return Out;
}

PS_OUT PS_MAIN_BLUR_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vBlur = g_BlurYTexture.Sample(LinearSampler, In.vTexcoord);
    vector vFinal = g_FinalTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = vBlur + vFinal * 0.5f;

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

    // 1
    pass Light_Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OnebyOne, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
    }

    // 2
    pass Light_Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OnebyOne, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
    }

    // 3
    pass Deferred
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }

    // 4
    pass Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }
    
    // 5
    pass Blur_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    // 6
    pass Blur_Y
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

    // 7
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


