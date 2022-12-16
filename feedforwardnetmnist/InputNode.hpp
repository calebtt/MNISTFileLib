#pragma once
#include "../MNISTFileLib/Idx3HeaderData.hpp"
#include "convo_nnet.h"

//See: https://archive.ph/ifpY5
struct InputNode
{
	using PixelType = Idx3Lib::Idx3HeaderData::Bits8Type;
	using WeightType = double;
	using NodeIdType = size_t;
	using RandomType = unsigned;

	NodeIdType m_nodeId{};
	PixelType m_bwPixel{};
	WeightType m_weight{NNFunctions::getRandomDouble()};

	/// <summary>Default ctor, data members default init.</summary>
	InputNode() = default;
	/// <summary>ctor assigns a random weight value</summary>
	InputNode(const NodeIdType nodeId, const PixelType pixData) : m_nodeId(nodeId), m_bwPixel(pixData)
	{
	}
	/// <summary>ctor to assign a specific weight</summary>
	InputNode(const NodeIdType nodeId, const PixelType pixData, const WeightType weight) : m_nodeId(nodeId), m_bwPixel(pixData), m_weight(weight)
	{
		
	}

	InputNode(const InputNode& other) = default;
	InputNode(InputNode&& other) = default;
	InputNode& operator=(const InputNode& other) = default;
	InputNode& operator=(InputNode&& other) = default;
};