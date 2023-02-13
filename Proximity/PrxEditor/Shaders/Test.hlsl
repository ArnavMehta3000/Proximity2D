struct VSInput
{
    float3 Position : POSITION;
    float2 TexCoord : COLOR;
};

struct VSOutput
{
    float4 Position : SV_Position;
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

float4 PSmain(VSOutput input) : SV_Target0
{
    return float4(input.TexCoord, 1, 1);
}