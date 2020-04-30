cbuffer LightCBuf
{
    float3 lightPos;
}

static const float3 materialColour = { 0.7f, 0.7f, 0.9f };
static const float3 ambient = { 0.15f, 0.15f, 0.15f };
static const float3 diffuseColour = { 1.0f, 1.0f, 1.0f };
static const float diffuseIntensity = 1.0f;
static const float attConst = 1.0f;
static const float attLin = 0.045f;
static const float attQuad = 0.0075f;

Texture2D tex;

SamplerState splr;

float4 main(float3 worldPos : Position, float3 n : Normal, float2 tc : Texcoord) : SV_TARGET
{
    //const float3 vToL = lightPos - worldPos;
    const float3 vToL = 5;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;

    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));

    const float3 diffuse = diffuseColour * diffuseIntensity * att * max(0.0f, dot(dirToL, n));

    float4 colour = tex.Sample(splr, tc);
    //colour.a = 0.5f;

    return float4(saturate((diffuse + ambient) * materialColour), 1.0f) * colour;

}