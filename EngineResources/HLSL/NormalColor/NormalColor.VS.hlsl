#include "NormalColor.hlsli"

struct TransformMatrix {
	float4x4 WVP;
	float4x4 world;
};
struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

ConstantBuffer<TransformMatrix> gTransformMatrix : register(b0);

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	output.position = mul(input.position, gTransformMatrix.WVP);
	output.normal = input.normal;
	return output;
}