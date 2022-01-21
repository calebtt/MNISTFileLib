#pragma once
#include "stdafx.h"
#include "../MNISTFileLib/Idx3HeaderData.hpp"
#include "InputNode.hpp"
#include "BuildRandom.hpp"

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

	NodeIdType m_nodeId = 0;
	BiasType m_bias = 0.0;
	ActivationResultType m_activationResult = 0.0;
	//Weights assigned to each element of the incoming connections
	std::unordered_map<NodeIdType, WeightType> m_weightMap;
	//below, unused for the moment.
	//std::function<ActivationResultType(PixelType, WeightType)> activationFunction;
	//const auto BasicActivationFunction = [](auto pixData, auto weightData)
	//{

	//};

	/// <summary>Default ctor, a random bias value is assigned.</summary>
	HiddenNeuron(NodeIdType id) : m_nodeId(id)
	{
		const auto w = BuildRandom::BuildRandomVector<RandomType>(1, 1);
		m_bias = w[0] % 100 / 100.0;
	}

	/// <summary>Ctor for assigning a bias explicitly</summary>
	HiddenNeuron(NodeIdType id, BiasType bias) : m_nodeId(id), m_bias(bias) { }

	/// <summary>The activation function applied on the incoming connections. Result is stored internally as well.</summary>
	ActivationResultType ActivationFunction(const std::vector<InputNode> &inputNodeVector)
	{
		//The neuron assigns a weight to each of it's incoming connections.
		for (const auto& elem : inputNodeVector)
		{
			if (!m_weightMap[elem.m_nodeId])
			{
				//Randomly generate a weight.
				const auto w = BuildRandom::BuildRandomVector<RandomType>(1, 1);
				m_weightMap[elem.m_nodeId] = w[0] % 100 / 100.0;
			}
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
