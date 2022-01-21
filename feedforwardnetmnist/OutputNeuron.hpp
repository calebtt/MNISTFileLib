#pragma once
#include "stdafx.h"
#include "HiddenNeuron.hpp"
#include "BuildRandom.hpp"

/// <summary>This type will assign randomly generated weights to incoming connections
///	when ActivationFunction() is called, these can be modified via the m_weightMap data member.
///	If the input node's ID is stored in the map with a weight value, it will not be over-written during
///	calculation of the ActivationFunction.</summary>
struct OutputNeuron
{
	using WeightType = double;
	using NodeIdType = size_t;
	//using BiasType = double;
	using ActivationResultType = HiddenNeuron::ActivationResultType;
	using RandomType = unsigned int;

	NodeIdType m_nodeId = 0;
	ActivationResultType m_threshold = 0.0;
	//hidden neuron layer connection weights
	std::unordered_map<NodeIdType, WeightType> m_weightMap;

	/// <summary>Ctor for assigning a threshold explicitly</summary>
	OutputNeuron(NodeIdType id, ActivationResultType threshold) : m_nodeId(id), m_threshold(threshold)
	{
		//The neuron assigns a weight to each of it's incoming connections.
	}

	/// <summary>The activation function applied on the incoming connections. Result is stored internally as well.</summary>
	bool ActivationFunction(const std::vector<HiddenNeuron>& hiddenNeuronVector)
	{
		for (const auto& elem : hiddenNeuronVector)
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
		for (const auto& elem : hiddenNeuronVector)
		{
			runningSum += elem.m_activationResult * m_weightMap[elem.m_nodeId];
		}
		m_threshold = runningSum;
		return runningSum > m_threshold;
	}
};
