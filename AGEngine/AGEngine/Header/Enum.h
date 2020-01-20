#pragma once
enum VertexInput
{
	k_Position = 1<<0,
	k_Normal = 1 << 1,
	k_Tangent = 1 << 3,
	k_TexCoords = 1 << 4 ,
};
using VertexInputFlags = unsigned int;
