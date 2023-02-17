struct VSInput
{
    float3 Position : POSITION;
    float2 TexCoord : COLOR;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : COLOR;
};

cbuffer CameraMatrices : register(b0)
{
    matrix WorldMatrix;
    matrix ViewMatrix;
    matrix ProjectionMatrix;
};

VSOutput VSmain(VSInput input)
{
    VSOutput output;
    output.Position = float4(input.Position, 1.0f);
    output.TexCoord    = input.TexCoord;
    return output;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    return float4(input.TexCoord, 1, 1);
    //return float4(0.15f, 0.25f, 0.35f, 1.0f);
}