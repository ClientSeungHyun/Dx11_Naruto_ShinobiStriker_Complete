
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

/* 1. ������ ��ȯ������ �����Ѵ�. */
/* 2. ������ ���� ������ �����Ѵ�. */
VS_OUT VS_MAIN( /*����*/VS_IN In)
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
	/* ���� �ؾ��� ������ ��� ���ĳ��� ���̹Ƿ� ���� dx�� �߰������� �ؾ��� �̸��� �ض�. */
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

/* Triangle : ���� ������ �� vs_main�� ����Ҷ����� ��� */
/* ������ ��δ� ����Ǹ�. ���� ������ ����. */
/* ���ϵ� ������ w�� ������ xyzw�� ������. ���� */
/* ������ ��ġ�� ����Ʈ�� ��ȯ��. (��������ǥ�� ��ȯ�Ѵ�)*/
/* �����Ͷ����� : ���������� ��������Ͽ� �ȼ��� ���������. */

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


/* 1. �ȼ��� �������� ������ �����Ѵ�. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    //// 1. �߽��� ��ġ�� ���� �ȼ� ��ġ�� ����Ͽ� �Ÿ� ���
    //float3 centerPos = In.vCenterPos.xyz;
    //float fDist = length(centerPos - In.vWorldPosition.xyz);
    
    //// 2. �Ÿ� ����ȭ
    //float maxDist = 2.0f; // �ʿ信 ���� �ִ� �Ÿ� ����
    //float normalizedDist = saturate(fDist / maxDist); // 0~1�� ����ȭ

    //// 3. ���� ����: �Ÿ� ���� �������� �������
    //float4 whiteColor = float4(1.0, 1.0, 1.0, 1.0); // ���
    //Out.vColor = lerp(g_vColor, whiteColor, fDist * 10.f); // �Ÿ� ���� ���� ���� ȥ��
    
    Out.vColor = g_vColor;
    
    return Out;
}



technique11 DefaultTechnique
{
	/* ������ + ������Ʈ + ssao + ��ָ��� + pbr*/
    pass FootTrail
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
	/* ����� + ���� */
	//pass Effect
	//{
	//	VertexShader = compile vs_5_0 VS_MAIN_Special();
	//	PixelShader = compile ps_5_0 PS_MAIN_Special();
	//}
}