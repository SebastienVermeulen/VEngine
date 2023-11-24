#pragma once
#include "pch.h"

class VMath 
{
public:
	//TO-DO: Make own temp types that takes care of this before storing to buffers etc.
	static bool Compare(const DirectX::XMFLOAT4& foo, const DirectX::XMFLOAT4& bar)
	{
		return std::fabs(foo.x - bar.x) < FLT_EPSILON && std::fabs(foo.y - bar.y) < FLT_EPSILON
			&& std::fabs(foo.z - bar.z) < FLT_EPSILON && std::fabs(foo.w - bar.w) < FLT_EPSILON;
	}
	static bool Compare(const DirectX::XMFLOAT3& foo, const DirectX::XMFLOAT3& bar)
	{
		return std::fabs(foo.x - bar.x) < FLT_EPSILON && std::fabs(foo.y - bar.y) < FLT_EPSILON && std::fabs(foo.z - bar.z) < FLT_EPSILON;
	}
	static bool Compare(const DirectX::XMFLOAT2& foo, const DirectX::XMFLOAT2& bar)
	{
		return std::fabs(foo.x - bar.x) < FLT_EPSILON && std::fabs(foo.y - bar.y) < FLT_EPSILON;
	}
	static bool Compare(const DirectX::XMINT3& foo, const DirectX::XMINT3& bar)
	{
		return foo.x == bar.x && foo.y == bar.y && foo.z == bar.z;
	}
	//END-TO-DO
};