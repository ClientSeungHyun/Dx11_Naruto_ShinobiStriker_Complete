#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vColor = vector(0.f, 0.f, 0.f, 0.f);

float g_fTexCoordX = 0.f;
float g_fTexCoordY = 0.f;
float g_fDissolve= 1.f;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_MaskTexture;
texture2D g_MaskTexture2;
texture2D g_DissolveTexture;


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
};


PS_OUT PS_MAIN_MASK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    In.vTexcoord.x += g_fTexCoordX;
    In.vTexcoord.y += g_fTexCoordY;
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
    if(vMask.r <= 0.2)
        discard;
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= vMask.r * g_vColor.a;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_NOMASK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    In.vTexcoord.x += g_fTexCoordX;
    In.vTexcoord.y += g_fTexCoordY;
    
    Out.vDiffuse = g_vColor;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_TWOMASK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    In.vTexcoord.x += g_fTexCoordX;
    In.vTexcoord.y += g_fTexCoordY;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMask2 = g_MaskTexture2.Sample(LinearSampler, In.vTexcoord);
    
    float alpha = max(vDiffuse.r, max(vMask.r, vMask2.r));
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse.a *= alpha;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_FIREBALL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    In.vTexcoord.x += g_fTexCoordX;
    In.vTexcoord.y += g_fTexCoordY;
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);

    vector orangeColor = vector(1.0f, 0.5f, 0.0f, 1.0f); 
    
    Out.vDiffuse = lerp(orangeColor, g_vColor, vMask.r);    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_AMATERAS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    In.vTexcoord.y -= g_fTexCoordY;
    vector vOpacity = g_DissolveTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    vector vBlueColor = g_vColor;
    
    Out.vDiffuse = vDiffuse * vBlueColor;
    Out.vDiffuse.a = vOpacity.r;
    if(Out.vDiffuse.a <= 0.2f)
        discard;
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_EXPLOSIONSPHERE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vDisslove = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);
    
    In.vTexcoord.x -= g_fTexCoordX;
    In.vTexcoord.y -= g_fTexCoordY;
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    float threshold = 0.1; // "검은색에 가까운" 기준
    float3 colorOutput = (vDiffuse.r < threshold && vDiffuse.g < threshold && vDiffuse.b < threshold) ? g_vColor.rgb : g_vColor.rgb * 1.2f;

    if (saturate(g_fDissolve * (vDisslove.r + 0.2f)) <= 0.3f)
        discard;
    
    Out.vDiffuse = float4(colorOutput, g_fDissolve);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_RASENGANRING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    In.vTexcoord.x += g_fTexCoordX;
    In.vTexcoord.y += g_fTexCoordY;
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
    if(vMask.r <=0.3f)
        discard;
    
    Out.vDiffuse = vector(g_vColor.xyz, vMask.r * g_vColor.w);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_GROUNDSLASH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    In.vTexcoord.x += g_fTexCoordX;
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (In.vTexcoord.x * 3.5f < -1.f)
        discard;
    
    Out.vDiffuse = vDiffuse + g_vColor;
    Out.vDiffuse.a = g_vColor.a;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_BLADESLASH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.r < 0.2f)
        discard;
    
    
    Out.vDiffuse = vDiffuse + g_vColor;
    Out.vDiffuse.a = g_vColor.a;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_FOXEXPLOSIONSPHERE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vDisslove = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);
    
    In.vTexcoord.x -= g_fTexCoordX;
    if (In.vTexcoord.x > 2.f)
        discard;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vColor = (vDiffuse.r < 0.1f) ? g_vColor.rgb : g_vColor.rgb * 1.5f;

    
    Out.vDiffuse = float4(vColor, 0.8f);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_TYPEZERO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    In.vTexcoord.x += g_fTexCoordX;
    vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord);
    if (vDiffuse.r <= 0.2f)
        discard;
    
    Out.vDiffuse = float4(vDiffuse.xyz + g_vColor.xyz, 1.f);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_RASENKOSTAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    In.vTexcoord.x += g_fTexCoordX;
    In.vTexcoord.y += g_fTexCoordY;
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.r <= 0.2f)
        discard;
    
    Out.vDiffuse = float4(vDiffuse.xyz + g_vColor.xyz, 1.f);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_SMOKERING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (g_fDissolve > vDiffuse.r)
        discard;
    
    Out.vDiffuse = g_vColor;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.f, 0.f, 0.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    // 0
    pass Model_Mask
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MASK();
    }

    // 1
    pass NoMask_Depth
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NOMASK();
    }

    // 2
    pass NoMask_NoDepth
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NOMASK();
    }

    // 3
    pass Model_TwoMask
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TWOMASK();
    }

    // 4
    pass Model_FireBall
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FIREBALL();
    }

    // 5
    pass Model_Amateras
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_AMATERAS();
    }

    // 6
    pass Model_ExplosionSphere
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EXPLOSIONSPHERE();
    }

    // 7
    pass Model_RasenganRing
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RASENGANRING();
    }

    // 8
    pass Model_GroundSlahs
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GROUNDSLASH();
    }

    // 9
    pass Model_BladeSlash
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLADESLASH();
    }

    // 10
    pass Model_FoxExplosion
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FOXEXPLOSIONSPHERE();
    }

    // 11
    pass Model_TypeZero
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TYPEZERO();
    }

    // 12
    pass Model_RasenkoStar
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RASENKOSTAR();
    }

    // 13
    pass Model_SmokeRing
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SMOKERING();
    }

}