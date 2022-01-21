#include "stdafx.h"
#include "InputNode.hpp"
#include "HiddenNeuron.hpp"
#include "OutputNeuron.hpp"
#include "../MNISTFileLib/Idx3HeaderData.hpp"
#include "../MNISTFileLib/Idx3ImageDataBuffer.hpp"
#include "BuildRandom.hpp"

int main()
{
	using namespace std;
	using RandomType = unsigned int;
	constexpr size_t NumberOfHiddenNeurons = 10;
	//read image data
	ifstream inFile("train-images.idx3-ubyte", std::ios::binary);
	Idx3Lib::Idx3HeaderData header;
	inFile >> header;
	Idx3Lib::Idx3ImageDataBuffer imageData(header.num_columns * header.num_rows);
	inFile >> imageData;
	//build ffn layers with pixel data (which happens to be black and white bytes)
	vector<InputNode> inputLayer;
	vector<HiddenNeuron> hiddenLayer;
	const auto randomVector = BuildRandom::BuildRandomVector<RandomType>(imageData.ImageSize, imageData.ImageSize);
	const auto randomNeuronVector = BuildRandom::BuildRandomVector<RandomType>(NumberOfHiddenNeurons, NumberOfHiddenNeurons);
	for (size_t i = 0; i < imageData.ImageSize; i++)
	{
		InputNode n(imageData.buffer[i], i);
		//cout << "Weight:" << n.m_weight << " ";
		inputLayer.emplace_back(n);
	}
	for (size_t i = 0; i < NumberOfHiddenNeurons; i++)
	{
		HiddenNeuron n(i, randomNeuronVector[i] % 100 / 100.0);
		//cout << "Bias:" << n.m_bias << " ";
		hiddenLayer.emplace_back(n);
	}
	//Apply activation function on the neurons
	cout << "Hidden layer neuron function results:" << endl;
	for (auto& elem : hiddenLayer)
	{
		cout << "Activation result: " << elem.ActivationFunction(inputLayer) << endl;
	}
	//Apply function on output neuron.
	const double TValue = 13'500.0;
	cout << "Now for a (single neuron) output layer, some threshhold value has been arbitrarily chosen:" << TValue << endl;
	OutputNeuron on(0, TValue);
	const bool result = on.ActivationFunction(hiddenLayer);
	if (result)
		cout << "The output neuron reports true." << endl;
	else
		cout << "The output neuron reports false." << endl;

	cout << "[Enter] to exit..." << endl;
	cin.get();
}