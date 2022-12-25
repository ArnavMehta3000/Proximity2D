struct VSInput
{
    float3 Position : POSITION;
    float3 Color : COLOR;
};

struct VSOutput
{
    float4 Position : SV_Position;
    float3 Color : COLOR;
};

VSOutput VSmain(VSInput input)
{
    VSOutput output;
    output.Position = float4(input.Position, 1.0f);
    output.Color = input.Color;
    return output;
}

float4 PSmain(VSOutput input) : SV_Target0
{
    return float4(1, 1, 1, 1);
}