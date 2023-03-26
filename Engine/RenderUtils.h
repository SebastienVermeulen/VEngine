#pragma once
#include "pch.h"
#include "Renderable.h"

static bool IsRenderTypeInVector(RenderType type, std::vector<RenderType>& vector)
{
	auto determineVectorRenderCapable = [](RenderType type, std::vector<RenderType> vector)
	{
		for (RenderType vectorType : vector)
		{
			if (vectorType == type)
				return true;
		}
		return false;
	};
	return determineVectorRenderCapable(type, vector);
}
static std::vector<RenderType> FilterRenderTypesFromVectors(std::vector<RenderType>& types, std::vector<RenderType>& vector)
{
	// Reserve maximum capacity to avoid multiple allocations
	std::vector<RenderType> output;
	std::vector<RenderType>& vectorToCheck = vector;
	output.reserve(vectorToCheck.size());

	for (RenderType type : types)
	{
		if (IsRenderTypeInVector(type, vectorToCheck))
		{
			output.push_back(type);
		}
	}
	return output;
}

