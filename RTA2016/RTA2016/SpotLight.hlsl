#pragma pack_matrix(row_major)
texture2D texutre : register(t0);

SamplerState filter : register(s0);

struct V_IN
{
	float4 posH : SV_POSITION;
	float4 normals : NORMAL;
	float4 worldPos : POS;
	float2 uv : UV;
};
cbuffer OBJECT : register(b0)
{
	float4x4 light;
}
float4 main(V_IN input) : SV_TARGET
{
	float4 lightNormal = normalize(float4(light._41, light._42, light._43, light._44) - input.worldPos);
	float surfaceRatio = clamp(dot(-lightNormal, float4(light._31, light._32, light._33, light._34)), 0, 1);
	float spotFactor = (surfaceRatio > 0.05f) ? 1 : 0;
	float ratio = clamp(dot(lightNormal, input.normals), 0, 1);
	float attenuation = 1.0f - clamp((0.99f - surfaceRatio) / (0.99f - 0.98f), 0, 1);
	float4 color = texutre.Sample(filter, float2(input.uv.x, 1 - input.uv.y));
	color = color * ratio * spotFactor * attenuation;
	color.a = 1;
	return color;
}