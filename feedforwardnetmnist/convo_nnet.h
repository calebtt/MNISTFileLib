#pragma once
#include "stdafx.h"

//1. Each neuron(also referred to as a "node") receives one or more inputs from an external source or from other neurons.
//2. Each input is multiplied by a weight to indicate the input's relative importance.
//3. The sum of the weighted input(s) is fed into the neuron.
//4. Bias is added to the sum of the weighted inputs.
//5. An activation function within the neuron performs a calculation on the total.
//6. The result is the neuron's output, which is passed to other neurons or delivered to the external world as the machine's output.
//7. The output passes through a loss function or cost function that evaluates the accuracy of the neural network's prediction, and results are
//	fed back through the network, indicating adjustments that must be made to the weights and biases.
//8. The process is repeated in multiple iterations to optimize the accuracy of the output; weightsand biases are adjusted with each iteration.
//9. NOTE backpropagation is used to determine the gradient descent, aka minima. It is computed for each input set for each iteration.

struct Perceptron;
struct NNFunctions
{
	/**
	 * \brief The so-called "net input function" is the algorithm for computing the weight_array * input_array + bias.
	 *	This function is used to map inputs to an equal number of first layer neurons.
	 * \remarks The inputs vector and the weights vector must be the same length! Use <c>NetLayerFunction</c> otherwise
	 * \param inputs 
	 * \param weights 
	 * \param bias 
	 * \return the inner_product of inputs * weights with the bias added.
	 */
	[[nodiscard]]
	inline
	static
	constexpr
	auto NetInputFunction(
		const std::ranges::range auto& inputs,
		const std::ranges::range auto& weights,
		const double bias = 0.0) noexcept
		-> double
	{
		using std::begin, std::end, std::inner_product;
		assert(inputs.size() == weights.size());
		return inner_product(begin(inputs), end(inputs), begin(weights), bias);
	}

	/**
	 * \brief This function is used to multiply (possibly differently sized) vectors with each other for most operations a NN will perform.
	 * \param inputs LHS input range
	 * \param weights RHS input range
	 * \param bias 
	 * \return resultant sum
	 */
	static
	auto NetLayerFunction(
		const std::ranges::range auto& inputs,
		const std::ranges::range auto& weights,
		const double bias = 0.0) noexcept
		-> double
	{
		const auto productFn = [bias](const auto& inputItem, const auto& weightsItem)
		{
			double runningSum{};
			for (const auto& lelem : inputItem)
			{
				for (const auto& selem : weightsItem)
				{
					runningSum += lelem * selem;
				}
			}
			return runningSum + bias;
		};
		return productFn(inputs, weights);
	}

	/// <summary>
	/// The so-called "cost function" is (actual-expected)^2
	/// </summary>
	/// <returns> relative amount it should increase or decrease </returns>
	[[nodiscard]]
	static
	auto CostFunction(
		const double actual,
		const double expected) noexcept
		-> double
	{
		return std::exp2(actual - expected);
	}

	/// <summary>
	/// Returns a random <c>double</c> value between min and max for the type.
	/// </summary>
	[[nodiscard]]
	static
	auto getRandomDouble(const double minVal = 0.0, const double maxVal = 1.0)
		-> double
	{
		static thread_local std::random_device rd;
		static thread_local std::mt19937 gen(rd());
		static thread_local std::uniform_real_distribution<> realDist(minVal, maxVal);
		return realDist(gen);
	}
};

namespace Acts
{
	/*
	 * Activation functions model a threshhold behavior.
	 * They usually constrain the output values.
	 * Importantly, they provide nonlinearity to the neuron.
	 * Also enable training by backpropagation (must be differentiable).
	 */

	/**
	 * \brief For the case where outputs are represented as values <b>between</b> -1(false) and 1(true)
	 * \param x floating point weighted sum
	 * \return double result between -1 and 1
	 */
	[[nodiscard]]
	inline
	auto HyperbolicTangentFunctionFFN_Multilayer(const double x) noexcept
		-> double
	{
		return std::tanh(x);
		//static constexpr auto minimumVal{ -1.0 };
		//static constexpr auto maximumVal{ 1.0 };
		//const auto result = std::tanh(x);
		//return std::clamp(result, minimumVal, maximumVal);
	}

	/**
	 * \brief For the case where (neuron) outputs are represented as values 0(false) and 1(true)
	 * \param x floating point weighted sum
	 * \return bool result 
	 */
	inline
	auto BinaryStepFunctionFFN_Multilayer(const double x) noexcept
		-> bool
	{
		return x >= 0.0;
	}

	/**
	 * \brief Sigmoid fn is the most commonly used fn to constrain output values.
	 *	For the case where outputs are represented as values between 0.0 and 1.0
	 * \param x floating point weighted sum
	 * \return double result between 0.0 and 1.0
	 */
	inline
	auto SigmoidFunctionFFN_Multilayer(const double x) noexcept
		-> double
	{
		//The graph of this is useful.
		//   1
		//-------
		// 1+e^(-x)
		static constexpr auto minimumVal{ 0.0 };
		static constexpr auto maximumVal{ 1.0 };
		const double result = 1.0 / (1.0 + std::exp(-x));
		//return result;
		return static_cast<double>(std::clamp(result, minimumVal, maximumVal));
	}

	/**
	 * \brief Rectified Linear Unit function AKA ReLU.
	 *	Constrains output between 0.0 and positive infinity.
	 * \param x floating point weighted sum
	 * \return floating point between 0.0 and positive infinity
	 */
	inline
	constexpr
	auto RectifiedLinearUnitFFN_Multilayer(const double x) noexcept
		-> double
	{
		const auto result = x < decltype(x){} ? 0 : x;
		return result;
	}

	/**
	 * \brief Used to find the maximal value in a range.
	 * \param inp range to search
	 * \return value of the maximal element if found, otherwise 0
	 */
	inline
	constexpr
	auto ArgMaxFFN_Multilayer(const std::vector<double>& inp) noexcept
		-> double
	{
		using std::ranges::end, std::ranges::max_element;
		const auto maxElem = max_element(inp);
		return (maxElem != end(inp) ? *maxElem : 0.0);
	}
}