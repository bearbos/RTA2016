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
	float ratio = clamp(dot(-light, input.normals), 0, 1);
	float4 color = texutre.Sample(filter, float2(input.uv.x, 1 - input.uv.y));
		color = color * ratio;
	color.a = 1;
	return color;
}