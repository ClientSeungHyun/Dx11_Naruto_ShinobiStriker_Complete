#include "Shader_Engine_Defines.hlsli"

/* float2 float3 float4 == vector */
/* float1x3, float3x3, float1x3, float4x4 == matrix */

/* VertexShader */
/* �����ϳ��� � ��ȯ�� ���İ��� �Ǵ����� ���� ������ ���� ����. */
/* ���� ���̴��� ���� �ϳ��� �޾ƿ���. ������ ��ģ�� ������ ��ģ ������ �������ش�. */
/* �簢�����۸� �׸���. 6��ȣ��. */
// VS_MAIN(/*����*/float3 vPosition  : POSITION, float2 vTexcoord : TEXCOORD0)
//{
//
//}

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_MaskTexture;
texture2D g_MaskTexture2;

float g_fTime;
float g_fAlpha = 1.f;
float2 g_TextCoord;
float4 g_vColor;

//sampler LinearSampler = sampler_state
//{
//    Filter = MIN_MAG_MIP_LINEAR;
//    AddressU = wrap;
//    AddressV = WRAP;
//};

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
	
};

struct VS_OUT
{
	/* SV_ : ShaderValue */
	/* ���� �ؾ��� ������ ��� ���ĳ��� ���̹Ƿ� ���� dx�� �߰������� �ؾ��� �̸��� �ض�. */
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
	
};

/* 1. ������ ��ȯ������ �����Ѵ�. */
/* 2. ������ ���� ������ �����Ѵ�. */
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
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

PS_OUT PS_MAIN_SETALPHA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a *= g_fAlpha;
    
    return Out;
}

PS_OUT PS_MAIN_HP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vBaseColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * vector(1.f, 0.f, 0.f, 1.f);
    vector vGaugeColor = g_MaskTexture.Sample(LinearSampler, In.vTexcoord) * vector(0.f, 1.f, 0.f, 1.f);
    vector vDamageColor = g_MaskTexture2.Sample(LinearSampler, In.vTexcoord) * vector(1.f, 0.f, 0.f, 1.f);
    
    float fLeftMargin = 0.19f;
    float fRightMargin = 0.81f;
    
    float fFinalTexcoord = (In.vTexcoord.x - fLeftMargin) / (fRightMargin - fLeftMargin);
    if (fFinalTexcoord < g_TextCoord.x)
    {
        Out.vColor = vGaugeColor;
    }
    else
    {
        Out.vColor = vBaseColor;
    }
    
    vDamageColor.ra *= g_fAlpha;
    Out.vColor += vDamageColor;
    
    return Out;
}

PS_OUT PS_MAIN_SKILL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vBaseColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    vector vBaseSkillBack = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSkillIcon = g_MaskTexture2.Sample(LinearSampler, In.vTexcoord);
    
    if (g_TextCoord.y >= In.vTexcoord.y)
    {
        float fGrayScale = (vSkillIcon.r + vSkillIcon.g + vSkillIcon.b) / 3.0f;
        fGrayScale *= 0.3f;
        vSkillIcon = vector(fGrayScale, fGrayScale, fGrayScale, vSkillIcon.a);
    }
    
    float fDiagonal = In.vTexcoord.x + In.vTexcoord.y;
    float fWhiteLineWidth = 0.07f;
    float fTimeFactor = sin(g_fTime) + 1.f;
    float fLineEffect = (saturate((fDiagonal - fTimeFactor) / fWhiteLineWidth) - saturate((fDiagonal - (fTimeFactor + fWhiteLineWidth)) / fWhiteLineWidth)) * 10.f;
    vSkillIcon = lerp(vSkillIcon, vector(1.0f, 1.0f, 1.0f, vSkillIcon.a), fLineEffect);
    
    vector vFinalColor = vBaseSkillBack + (vBaseColor * vSkillIcon);
    
    Out.vColor = vFinalColor * g_fAlpha;
    
    return Out;
}

PS_OUT PS_MAIN_SPSKILL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vBaseColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    vector vSkillIcon = g_MaskTexture2.Sample(LinearSampler, In.vTexcoord);

    if (g_TextCoord.y >= In.vTexcoord.y)
    {
        float fGrayScale = (vSkillIcon.r + vSkillIcon.g + vSkillIcon.b) / 3.0f;
        fGrayScale *= 0.3f;
        vSkillIcon = vector(fGrayScale, fGrayScale, fGrayScale, vSkillIcon.a);
    }
    
    vector vFinalColor = vBaseColor * vSkillIcon;
    
    Out.vColor = vFinalColor * g_fAlpha;
    
    return Out;
}

PS_OUT PS_MAIN_BOSSHPGUAGE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vGaugeColor = g_Texture.Sample(LinearSampler, In.vTexcoord) ;
    if(vGaugeColor.r < 0.8f 
        || In.vTexcoord.x > g_TextCoord.x)
        discard;
    
    float fLeftMargin = 0.19f;
    float fRightMargin = 0.81f;
    
    Out.vColor = vGaugeColor * vector(1.f, 0.f, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_TARGET(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if(vDiffuse.r < 0.3f)
        discard;
    
    Out.vColor = vDiffuse;
    Out.vColor.a *= g_fAlpha;
    
    return Out;
}

PS_OUT PS_MAIN_TITLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vDiffuse * g_vColor;
    Out.vColor.a = g_vColor.a;
    
    return Out;
}

PS_OUT PS_MAIN_TITLEBUTTON(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vDiffuse * g_vColor;
    Out.vColor.a = vDiffuse.a * g_vColor.a;
    
    return Out;
}

PS_OUT PS_MAIN_BOSSTARGET(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vDiffuse;
    
    return Out;
}

technique11 DefaultTechnique
{
	/* ������ + ������Ʈ + ssao + ��ָ��� + pbr*/
    pass UI
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass UI_SetAlpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SETALPHA();
    }

    pass UI_HP
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HP();
    }

    pass UI_SKILL
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SKILL();
    }

    // 4
    pass UI_SPSKILL
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPSKILL();
    }
    
    // 5
    pass UI_BossHPGuage
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BOSSHPGUAGE();
    }

    // 6
    pass UI_Target
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TARGET();
    }

    // 7
    pass UI_Title
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TITLE();
    }

    // 8
    pass UI_TitleButton
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TITLEBUTTON();
    }

    // 9
    pass UI_BossTarget
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BOSSTARGET();
    }

}