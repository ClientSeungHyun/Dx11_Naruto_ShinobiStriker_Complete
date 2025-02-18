#include "Shader_Engine_Defines.hlsli"

/* float2 float3 float4 == vector */
/* float1x3, float3x3, float1x3, float4x4 == matrix */

/* VertexShader */
/* 정점하나당 어떤 변환을 거쳐가면 되는지를 직접 연산을 통해 구현. */
/* 정점 쉐이더는 정점 하나를 받아오고. 연산을 거친후 연산을 마친 정점을 리턴해준다. */
/* 사각형버퍼를 그린다. 6번호출. */
// VS_MAIN(/*정점*/float3 vPosition  : POSITION, float2 vTexcoord : TEXCOORD0)
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
	/* 내가 해야할 연산은 모두 마쳐놓은 것이므로 이후 dx가 추가적으로 해야할 이릉ㄹ 해라. */
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
	
};

/* 1. 정점의 변환과정을 수행한다. */
/* 2. 정점의 구성 정보를 변형한다. */
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
	/* 빛연산 + 림라이트 + ssao + 노멀맵핑 + pbr*/
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