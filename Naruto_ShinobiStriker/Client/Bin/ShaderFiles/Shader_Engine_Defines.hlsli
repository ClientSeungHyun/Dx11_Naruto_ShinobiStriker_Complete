
sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler LinearClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
};

sampler BorderSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = border; // U 방향으로 범위를 벗어난 경우
    AddressV = border; // V 방향으로 범위를 벗어난 경우
    BorderColor = float4(1.0f, 0.0f, 0.0f, 1.0f); // 빨간색으로 경계 색상 설정
};

RasterizerState RS_Default
{
    FillMode = solid;
    CullMode = back;
    FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
    FillMode = wireframe;
    CullMode = back;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_CW
{
    FillMode = solid;
    CullMode = front;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_None
{
    FillMode = solid;
    CullMode = none;
    FrontCounterClockwise = false;
};

BlendState BS_Default
{
    BlendEnable[0] = false; // 타겟별로 구분해서 처리 가능
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;

    // 배열을 지정하지 않으면 따로 설정하지 않은 타겟에 같은 것을 적용
    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};

BlendState BS_OnebyOne
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = add;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

DepthStencilState DSS_None
{
    DepthEnable = false; // 깊이 테스트 비활성화
    DepthWriteMask = zero; // 깊이 버퍼에 쓰기 비활성화
};