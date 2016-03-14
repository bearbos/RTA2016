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
	float4 light;
}
float4 main(V_IN input) : SV_TARGET
{
	float4 lightDir = normalize(light - input.worldPos);
	float ratio = clamp(dot(lightDir, input.normals), 0, 1);
	float len = length(light - input.worldPos);
	float attenuation = 1.0f - clamp(len / 15.0f, 0, 1);
	float4 color = texutre.Sample(filter, float2(input.uv.x, 1 - input.uv.y));
		color = color * ratio *attenuation;
	color.a = 1;
	return color;
}