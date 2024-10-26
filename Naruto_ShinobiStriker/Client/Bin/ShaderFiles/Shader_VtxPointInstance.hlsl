
#include "Shader_Engine_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;
vector			g_vCamPosition;
vector			g_vColor = vector(0.f, 0.f, 0.f, 0.f);

struct VS_IN
{
	/* InputSlot : 0 */
	float3 vPosition : POSITION;	
	float2 vPSize : PSIZE;
	/* InputSlot : 1 */
    row_major float4x4 TransformMatrix : WORLD;
	float2 vLifeTime : COLOR0;
};

struct VS_OUT
{
	/* SV_ : ShaderValue */
	/* ���� �ؾ��� ������ ��� ���ĳ��� ���̹Ƿ� ���� dx�� �߰������� �ؾ��� �̸��� �ض�. */
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;
	float2 vLifeTime : COLOR0;	
};

struct VS_HITPARTICLE_IN
{
		/* InputSlot : 0 */
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
	
    float4 vTranslation : TEXCOORD0;
    float4 vDirection : TEXCOORD1;
    float4 vScale : TEXCOORD2;
    float2 vLifeTime : COLOR0;
};

struct VS_HITPARTICLE_OUT
{
    float4 vPosition : POSITION;
    float4 vDirection : TEXCOORD0;
    float4 vScale : TEXCOORD1;
    float2 vLifeTime : COLOR0;
};

/* 1. ������ ��ȯ������ �����Ѵ�. */
/* 2. ������ ���� ������ �����Ѵ�. */
VS_OUT VS_MAIN(/*����*/VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	
	
	/* ���û��¿����� ������ ����. */
	vector		vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);

	/* ���忡 Ư�� ���·� ��ġ�Ѵ�. */
	vPosition = mul(vPosition, g_WorldMatrix);
	
	Out.vPosition = vPosition;
	Out.vPSize = In.vPSize;
	Out.vLifeTime = In.vLifeTime;

	return Out;
}

VS_HITPARTICLE_OUT VS_HITPARTICLE_MAIN(VS_HITPARTICLE_IN In)
{
    VS_HITPARTICLE_OUT Out = (VS_HITPARTICLE_OUT) 0;
	
    Out.vPosition = In.vTranslation;
    Out.vDirection = In.vDirection;
    Out.vLifeTime = In.vLifeTime;
    Out.vScale = In.vScale;
	
    return Out;
}

struct GS_IN
{
	/* SV_ : ShaderValue */
	/* ���� �ؾ��� ������ ��� ���ĳ��� ���̹Ƿ� ���� dx�� �߰������� �ؾ��� �̸��� �ض�. */
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;
	float2 vLifeTime : COLOR0;
};

struct GS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float2 vLifeTime : COLOR0;
};

struct GS_EFFECT_IN
{
    float4 vPosition : POSITION;
    float4 vDirection : TEXCOORD0;
    float4 vScale : TEXCOORD1;
    float2 vLifeTime : COLOR0;
};

struct GS_EFFECT_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
};

// void GS_MAIN(triangle GS_IN In[3], )
[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
	GS_OUT			Out[4];

	float3		vLook = (g_vCamPosition - In[0].vPosition).xyz;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
	float3		vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

	Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
	Out[0].vTexcoord = float2(0.f, 0.f);
	Out[0].vLifeTime = In[0].vLifeTime;

	Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
	Out[1].vTexcoord = float2(1.f, 0.f);
	Out[1].vLifeTime = In[0].vLifeTime;

	Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
	Out[2].vTexcoord = float2(1.f, 1.f);
	Out[2].vLifeTime = In[0].vLifeTime;

	Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
	Out[3].vTexcoord = float2(0.f, 1.f);
	Out[3].vLifeTime = In[0].vLifeTime;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

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

[maxvertexcount(6)]
void GS_HITPARTICLE_MAIN(point GS_EFFECT_IN In[1], inout TriangleStream<GS_EFFECT_OUT> Container)
{
    GS_EFFECT_OUT Out[4];
	
    float3 vCameraLook = (In[0].vPosition - g_vCamPosition).xyz;
    float fSize = saturate(In[0].vDirection.y) + 0.05f;
	
    float3 vRight = normalize(In[0].vDirection) * fSize * 0.3f;
    float3 vLook = normalize(cross(float3(0.f, 1.f, 0.f), vRight));
    float3 vUp = normalize(cross(vLook, vRight)) * 0.04f;

    float fDotProduct = dot(vCameraLook, vLook);

    //Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
    //Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
    //Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
    //Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
	
    if (fDotProduct > 0.0f)
    {
        Out[0].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
        Out[1].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
        Out[2].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
        Out[3].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
    }
    else
    {
        Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
        Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
        Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
        Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
    }
	
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;

    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;

    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;

    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;

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
	float2 vTexcoord : TEXCOORD0;	
	float2 vLifeTime : COLOR0;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};


/* 1. �ȼ��� �������� ������ �����Ѵ�. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;	
	
    vector vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
    if (In.vLifeTime.y >= In.vLifeTime.x || vDiffuse.r <= 0.3f)
        discard;
	
    Out.vColor = vDiffuse * 0.6f + g_vColor;
    Out.vColor.a *= vDiffuse.r;
	

	return Out;
}

PS_OUT PS_HITPARTICLE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
    Out.vColor = vDiffuse  + g_vColor;
    Out.vColor.a *= vDiffuse.r;
	
    if (In.vLifeTime.y >= In.vLifeTime.x || Out.vColor.a <= 0.3f)
        discard;

    return Out;
}

PS_OUT PS_EXPLOSION_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.r < 0.3f)
        discard;
    
    Out.vColor = vDiffuse * g_vColor;
	
    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;

    return Out;
}

technique11	DefaultTechnique
{
	/* ������ + ������Ʈ + ssao + ��ָ��� + pbr*/
	pass UI
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass Hit
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_HITPARTICLE_MAIN();
        GeometryShader = compile gs_5_0 GS_HITPARTICLE_MAIN();
        PixelShader = compile ps_5_0 PS_HITPARTICLE_MAIN();
    }

    pass Explosion
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_EXPLOSION_MAIN();
    }
}