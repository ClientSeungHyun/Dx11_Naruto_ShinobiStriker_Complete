#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vColor = vector(0.f, 0.f, 0.f, 0.f);
vector g_vMaskColor = vector(0.f, 0.f, 0.f, 0.f);

float2 g_vTexcoord = float2(0.5f, 0.5f);
float g_fRadian = 0.f;
float g_fTime = 0.f;

int g_iNumCol;
int g_iNumRow;
int g_iCol;
int g_iRow;

bool g_isChange = false;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_MaskTexture;
texture2D g_MaskTexture2;
texture2D g_OpacityTexture;


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
    
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_RASENGAN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMask2 = g_MaskTexture2.Sample(LinearSampler, In.vTexcoord);
    
    float alpha = max(vMask.r, vMask2.r);
    if ((vDiffuse.r + vDiffuse.g + vDiffuse.b) < 0.15f)
        discard;

    float cosTheta = cos(g_fRadian);
    float sinTheta = sin(g_fRadian);
    
    float2 textureCenter = float2(0.5, 0.5);
    float2 centeredTexCoord = In.vTexcoord - textureCenter;
    
    float2 rotatedTexCoord;
    rotatedTexCoord.x = cosTheta * centeredTexCoord.x - sinTheta * centeredTexCoord.y;
    rotatedTexCoord.y = sinTheta * centeredTexCoord.x + cosTheta * centeredTexCoord.y;
    
    float2 finalTexCoord = rotatedTexCoord + textureCenter;
    vDiffuse = g_DiffuseTexture.Sample(LinearSampler, finalTexCoord);
    
    Out.vColor = vDiffuse + vMask + g_vColor;
    Out.vColor.a *= Out.vColor.r;
    Out.vColor.g += Out.vColor.r;
    Out.vColor.rg -= vDiffuse.r;
    
    return Out;
}

PS_OUT PS_MAIN_CHIDORI(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 texCoordRangeMin = float2(g_vTexcoord.x - 0.5f, g_vTexcoord.y - 0.5f); // 텍스처 좌표의 최소 값
    float2 texCoordRangeMax = float2(g_vTexcoord.x, g_vTexcoord.y); // 텍스처 좌표의 최대 값

// 현재 텍스처 좌표를 [0, 1] 범위에서 [0, 0.5] 범위로 매핑
    float2 adjustedTexCoord = lerp(texCoordRangeMin, texCoordRangeMax, In.vTexcoord);
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, adjustedTexCoord);
    
    Out.vColor = vDiffuse * g_vColor;
    Out.vColor.a *= vDiffuse.r;
    
    return Out;
}

PS_OUT PS_MAIN_ROAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vDiffuse;
    Out.vColor.a *= vDiffuse.r;
    
    if (Out.vColor.a <= 0.3f)
        discard;
    
    return Out;
}

PS_OUT PS_MAIN_FIRE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord;
    vTexcoord.x = (g_iCol + In.vTexcoord.y) / g_iNumCol;
    vTexcoord.y = (g_iRow + In.vTexcoord.x) / g_iNumRow;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, vTexcoord);
    
    if (vDiffuse.r <= 0.3f)
        discard;
    
    Out.vColor = vDiffuse * g_vColor;
    if(vDiffuse.r >= 0.95f)
        Out.vColor = vDiffuse * g_vMaskColor;
    
    Out.vColor.a *= vDiffuse.r;
    
    return Out;
}

PS_OUT PS_MAIN_EXPLOSION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    if (vDiffuse.r <= 0.2f)
        discard;
    
    Out.vColor = vDiffuse * 0.5f + g_vColor;
    Out.vColor.a *= pow(vDiffuse.r * g_vColor.a, 2.f);
    
    return Out;
}

PS_OUT PS_MAIN_GROUNDSLASH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord;
    vTexcoord.x = In.vTexcoord.x;
    vTexcoord.y = In.vTexcoord.y + g_vTexcoord.y;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord);
    vector vMask = g_MaskTexture.Sample(BorderSampler, vTexcoord);
    
    if (vDiffuse.r <= 0.2f)
        discard;
    
    Out.vColor = vMask * g_vColor;
    Out.vColor.a *= pow(vDiffuse.r * g_vColor.a, 2.f);
    
    return Out;
}

PS_OUT PS_MAIN_FOXSPHERE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    if (vDiffuse.r <= 0.2f)
        discard;
    
    float2 vCenter = float2(0.5f, 0.5f);
    float fDist = length(vCenter - In.vTexcoord);
    
    float fGlow = saturate((fDist - 0.3f) * 4.0f);

    //float factor = 1.f;
    //if (!g_isChange)
    //    factor = lerp(2.5f + fGlow, 3.0f + fGlow, saturate((fDist - 0.3f) / 0.15f));
    //else
    //    factor = lerp(2.5f + fGlow, 3.0f + fGlow, saturate((fDist - 0.3f) / 0.15f));
    
    Out.vColor = g_vColor;
    
    if(!g_isChange)
    {
        if (fDist > 0.45f)
            Out.vColor.r *= (3.f + fGlow);
        else if (fDist > 0.3f)
            Out.vColor.r *= (2.5f + fGlow);
        Out.vColor.a = 0.9f;
    }
    else
    {
        if (fDist < 0.35f)
            Out.vColor.rgb += 1.3f - fDist;
        else if (fDist < 0.45f)
            Out.vColor.rgb += 1.f - fDist;
    }
    
    
    return Out;
}

PS_OUT PS_MAIN_RASENKOHIT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vDiffuse.r <= 0.2f)
        discard;
 
    Out.vColor = g_vColor;
    Out.vColor.a = g_vColor.a;
    
    return Out;
}

PS_OUT PS_MAIN_TSUKUYOMI01(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fCosTheta = cos(g_fRadian);
    float fSinTheta = sin(g_fRadian);
    
    float2 fTextureCenter = float2(0.5, 0.5);
    float2 fTexCoord = In.vTexcoord - fTextureCenter;
    
    float2 rotatedTexCoord;
    rotatedTexCoord.x = fCosTheta * fTexCoord.x - fSinTheta * fTexCoord.y;
    rotatedTexCoord.y = fSinTheta * fTexCoord.x + fCosTheta * fTexCoord.y;
    
    float2 fFinalTexCoord = rotatedTexCoord + fTextureCenter;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, fFinalTexCoord);
    
    if (vDiffuse.r <= 0.2f)
        discard;
 
    Out.vColor = vDiffuse * g_vColor;
    Out.vColor.a = g_vColor.a;
    
    return Out;
}

PS_OUT PS_MAIN_TSUKUYOMI02(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fCosTheta = cos(g_fRadian);
    float fSinTheta = sin(g_fRadian);
    
    float2 fTextureCenter = float2(0.5, 0.5);
    float2 fTexCoord = In.vTexcoord - fTextureCenter;
    
    float2 rotatedTexCoord;
    rotatedTexCoord.x = fCosTheta * fTexCoord.x - fSinTheta * fTexCoord.y;
    rotatedTexCoord.y = fSinTheta * fTexCoord.x + fCosTheta * fTexCoord.y;
    
    float2 fFinalTexCoord = rotatedTexCoord + fTextureCenter;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, fFinalTexCoord);
    vector vMask = g_MaskTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    vector vFinalColor = vDiffuse + vMask;
    
    if (vFinalColor.r <= 0.2f)
        discard;
 
    Out.vColor = vFinalColor * g_vColor;
    Out.vColor.a = g_vColor.a;
    
    return Out;
}

PS_OUT PS_MAIN_SMOKE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord;
    vTexcoord.x = (g_iCol + In.vTexcoord.y) / g_iNumCol;
    vTexcoord.y = (g_iRow + In.vTexcoord.x) / g_iNumRow;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, vTexcoord);
    
    if (vDiffuse.r <= 0.3f)
        discard;
    
    Out.vColor = vDiffuse * g_vColor;    
    Out.vColor.a *= vDiffuse.r;
    
    return Out;
}

PS_OUT PS_MAIN_DISTORTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    Out.vColor = vDiffuse;
    Out.vColor.b = g_fTime;
    
    return Out;
}

technique11 DefaultTechnique
{
    //0
    pass Rasengan
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RASENGAN();
    }
    
    //1
    pass RasenShuriken
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RASENGAN();
    }

    //2
    pass Chidori
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CHIDORI();
    }

    //3
    pass Roar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ROAR();
    }

    //4
    pass Fire
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FIRE();
    }

    //5
    pass Explosion
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EXPLOSION();
    }

    //6
    pass GroundSlash
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GROUNDSLASH();
    }

    //7
    pass FoxSphere
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FOXSPHERE();
    }

    //8
    pass RasenkoHit
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RASENKOHIT();
    }

    //9
    pass Tsukuyomi01
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TSUKUYOMI01();
    }

    //10
    pass Tsukuyomi02
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TSUKUYOMI02();
    }

    //11
    pass Smoke
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SMOKE();
    }

    //12
    pass Distortion
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
    }
}