#pragma once
#include "../MNISTFileLib/Idx3HeaderData.hpp"
#include "BuildRandom.hpp"

//See: https://archive.ph/ifpY5
struct InputNode
{
	using PixelType = Idx3Lib::Idx3HeaderData::Bits8Type;
	using WeightType = double;
	using NodeIdType = size_t;
	using RandomType = unsigned int;

	NodeIdType m_nodeId{};
	PixelType m_bwPixel{};
	WeightType m_weight{};

	/// <summary>Default ctor, data members default init.</summary>
	InputNode() = default;
	/// <summary>ctor assigns a random weight value</summary>
	InputNode(const NodeIdType nodeId, const PixelType pixData) : m_nodeId(nodeId), m_bwPixel(pixData)
	{
		//randomly generate a weight
		const auto w = BuildRandom::BuildRandomVector<RandomType>(1, 1);
		m_weight = w[0] % 100 / 100.0;
	}
	/// <summary>ctor to assign a specific weight</summary>
	InputNode(const NodeIdType nodeId, const PixelType pixData, const WeightType weight) : m_nodeId(nodeId), m_bwPixel(pixData), m_weight(weight)	{ }
};