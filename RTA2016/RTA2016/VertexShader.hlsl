#pragma pack_matrix(row_major)
struct V_IN
{
	float4 posL : POSITION;
	float4 normals : NORMALS;
	float2 uv : UV;
};
struct V_OUT
{
	float4 posH : SV_POSITION;
	float4 normals : NORMAL;
	float2 uv : UV;
};
cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
}
cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}
V_OUT main( V_IN input )
{
	V_OUT outPut = (V_OUT)0;
	float4 localPos = input.posL;
	localPos = mul(localPos, worldMatrix);
	localPos = mul(localPos, viewMatrix);
	localPos = mul(localPos, projectionMatrix);
	outPut.posH = localPos;
	outPut.normals = input.normals;
	outPut.uv = input.uv;
	return outPut;
}