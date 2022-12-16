#pragma once
#include "stdafx.h"
#include "../MNISTFileLib/Idx3HeaderData.hpp"
#include "InputNode.hpp"
#include "convo_nnet.h"

/// <summary>This type will assign randomly generated weights to incoming connections
///	when ActivationFunction() is called, these can be modified via the m_weightMap data member.
///	If the input node's ID is stored in the map with a weight value, it will not be over-written during
///	calculation of the ActivationFunction.</summary>
struct HiddenNeuron
{
	using PixelType = Idx3Lib::Idx3HeaderData::Bits8Type;
	using WeightType = double;
	using NodeIdType = size_t;
	using BiasType = double;
	using ActivationResultType = double;
	using RandomType = unsigned int;

	NodeIdType m_nodeId{};
	BiasType m_bias{NNFunctions::getRandomDouble()};
	ActivationResultType m_activationResult{};
	//Weights assigned to each element of the incoming connections
	std::unordered_map<NodeIdType, WeightType> m_weightMap;

	/// <summary>Default ctor, a random bias value is assigned.</summary>
	explicit HiddenNeuron(const NodeIdType id) : m_nodeId(id)
	{
	}

	/// <summary>Ctor for assigning a bias explicitly</summary>
	HiddenNeuron(const NodeIdType id, const BiasType bias) : m_nodeId(id), m_bias(bias) { }

	HiddenNeuron(const HiddenNeuron& other) = default;

	HiddenNeuron(HiddenNeuron&& other) noexcept
		: m_nodeId(other.m_nodeId),
		  m_bias(other.m_bias),
		  m_activationResult(other.m_activationResult),
		  m_weightMap(std::move(other.m_weightMap))
	{
	}

	HiddenNeuron& operator=(const HiddenNeuron& other)
	{
		if (this == &other)
			return *this;
		m_nodeId = other.m_nodeId;
		m_bias = other.m_bias;
		m_activationResult = other.m_activationResult;
		m_weightMap = other.m_weightMap;
		return *this;
	}

	HiddenNeuron& operator=(HiddenNeuron&& other) noexcept
	{
		if (this == &other)
			return *this;
		m_nodeId = other.m_nodeId;
		m_bias = other.m_bias;
		m_activationResult = other.m_activationResult;
		m_weightMap = std::move(other.m_weightMap);
		return *this;
	}

	/// <summary>The activation function applied on the incoming connections. Result is stored internally as well.</summary>
	ActivationResultType ActivationFunction(const std::vector<InputNode> &inputNodeVector)
	{
		//The neuron assigns a weight to each of it's incoming connections.
		for (const auto& elem : inputNodeVector)
		{
			m_weightMap[elem.m_nodeId] = NNFunctions::getRandomDouble();
		}
		// might want a big number lib for this with many input nodes, boost has one that seemed good
		ActivationResultType runningSum = 0.0;
		//activation function stuff.
		for(const auto &elem: inputNodeVector)
		{
			runningSum += elem.m_bwPixel * m_weightMap[elem.m_nodeId];
		}
		m_activationResult = runningSum + m_bias;
		return m_activationResult;
	}
};
