
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
    AddressU = border; // U �������� ������ ��� ���
    AddressV = border; // V �������� ������ ��� ���
    BorderColor = float4(1.0f, 0.0f, 0.0f, 1.0f); // ���������� ��� ���� ����
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
    BlendEnable[0] = false; // Ÿ�ٺ��� �����ؼ� ó�� ����
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;

    // �迭�� �������� ������ ���� �������� ���� Ÿ�ٿ� ���� ���� ����
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
    DepthEnable = false; // ���� �׽�Ʈ ��Ȱ��ȭ
    DepthWriteMask = zero; // ���� ���ۿ� ���� ��Ȱ��ȭ
};