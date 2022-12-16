#include "stdafx.h"

#include <cassert>

#include "InputNode.hpp"
#include "HiddenNeuron.hpp"
#include "OutputNeuron.hpp"
#include "../MNISTFileLib/Idx3HeaderData.hpp"
#include "../MNISTFileLib/Idx3ImageDataBuffer.hpp"
#include "RandomGen.hpp"
#include <cmath>
#include <expected>
#include <format>

#include "convo_nnet.h"
//#include <boost/multiprecision/cpp_dec_float.hpp>
#include <imgui.h>
//#include <imgui_impl_glfw.h>

struct Perceptron
{
	//using Floating_t = boost::multiprecision::cpp_dec_float<3>;
	using Floating_t = double;
	using Weights_t = std::vector<Floating_t>;
	static constexpr int FloatPrecision{ 5 };
	static constexpr bool DebugLogging{ false };
	static constexpr double WeightMin{ 0.0 };
	static constexpr double WeightMax{ 1.0 };
private:
	inline static thread_local std::random_device rd;
	inline static thread_local std::mt19937 gen{ rd() };
	inline static thread_local std::uniform_real_distribution<Floating_t> realDist{
		WeightMin,
		WeightMax };
public:
	// Note, the weights for a connection (feed-forward assumed) are stored on the RIGHT hand
	// side of the multiplication being performed.
	Weights_t weights;
	Floating_t bias{ 1.0 };
	// result of an activation, if Run() has been called.
	std::optional<Floating_t> runResult{};
	// The activation function.
	std::function<Floating_t(Floating_t)> actFn{ RectifiedLinearUnitFFN_Multilayer };
	// The random floating point value generation function.
	std::function<Floating_t()> doubleFn{ [&]() { return Perceptron::GetCanonicalDouble(); } };
	// The net layer multiplication function.
	//std::function<double()
public:
	Perceptron() = default;
	explicit Perceptron(const std::size_t NumWeights)
	{
		weights.resize(NumWeights, 0.0);
		for (std::size_t n = 0; n < NumWeights; ++n)
		{
			weights[n] = doubleFn();
			if constexpr (DebugLogging)
			{
				std::cout << "Weight added: " << std::setprecision(4) << weights[n] << '\n';
			}
		}
	}
	Perceptron(const Perceptron& other) = delete;
	Perceptron(Perceptron&& other) noexcept = delete;
	Perceptron& operator=(const Perceptron& other) = delete;
	Perceptron& operator=(Perceptron&& other) noexcept = delete;
public:
	auto Resize(const std::size_t newSize)
	{
		weights.resize(newSize, 0.0);
		for (std::size_t n = 0; n < newSize; ++n)
		{
			weights[n] = doubleFn();
			if constexpr (DebugLogging)
			{
				std::cout << "Weight added for resize: " << std::setprecision(4) << weights[n] << '\n';
			}
		}
	}
	/// <summary>
	/// "Run" the single perceptron against the input vector.
	///	Stores the activation result internally, also returns a copy.
	/// </summary>
	/// <returns>Activation result</returns>
	auto Run(const std::ranges::range auto& inputsVector) noexcept
		-> Floating_t
	{
		Floating_t temp = actFn(NNFunctions::NetLayerFunction(inputsVector, weights, bias));
		runResult = temp;
		return temp;
	}
	/// <summary>
	/// "Run" the single perceptron against the input vector of Perceptrons.
	///	Stores the activation result internally, also returns a copy.
	/// </summary>
	/// <returns>Activation result</returns>
	auto Run(const std::vector<Perceptron>& inputsVector) noexcept -> Floating_t
	{
		Floating_t temp = actFn(PerceptronNetLayerFunction(inputsVector, weights, bias));
		runResult = temp;
		return temp;
	}
	[[nodiscard]]
	static
	auto GetCanonicalDouble() noexcept -> Floating_t
	{
		//return 0.01;
		return realDist(gen);
	}

	/**
	 * \brief For vector of perceptron
	 */
	static
	auto PerceptronNetLayerFunction(
			const std::vector<Perceptron>& inputs,
			const std::ranges::range auto& weights,
			const auto bias = 0.0) noexcept
		-> double
	{
		double runningSum{};
		for (const auto& el : inputs)
		{
			for (const auto& rl : weights)
			{
				assert(el.runResult.has_value());
				runningSum += el.runResult.value_or(0) * rl;
			}
		}
		return runningSum + bias;
	}

	/**
	 * \brief Rectified Linear Unit function AKA ReLU
	 *  Constrains output between 0.0 and positive infinity.
	 * \param x floating point weighted sum
	 * \return floating point between 0.0 and positive infinity
	 */
	inline
	static
	constexpr
	auto RectifiedLinearUnitFFN_Multilayer(const Floating_t x) noexcept
		-> Floating_t
	{
		const auto result = x < decltype(x){} ? 0 : x;
		return result;
	}
};

// MNIST data file.
auto GetImageBufferFromFile(const std::string& path) -> std::expected<std::vector<Idx3Lib::Idx3ImageDataBuffer>, std::string>;
auto run_test_and_gate() -> void;
auto run_test_image_net()-> void;


int main()
{
	using namespace std;
	//run_test_and_gate();
	run_test_image_net();

	//const std::string imFileName = "../MNISTtestdata/train-images.idx3-ubyte";
	//constexpr size_t LayerCount1{ 16 };
	//constexpr size_t LayerCount2{ 16 };
	//constexpr size_t LayerCount3{ 10 };

	//// Input data, the vector of image data buffers, wherein each image has a size around 784 from MNIST
	//const auto inputData = GetImageBufferFromFile(imFileName);
	//if(!inputData.has_value())
	//{
	//	cerr << "Failed to read image data!\n";
	//	return 1;
	//}
	//// Alias for the inputs vector.
	//const auto& inputBuffer = inputData.value();

	//// Perceptrons have a weight for every single connection, meaning a weight for all 784 input nodes
	//// for the first layer. 16 for the second layer, and 10 for the output layer (one for each number).
	//// i.e., 16 vectors of double, each with 784 doubles for the first layer.
	//// First, a lambda to initialize each vector with a random weight.
	//const auto initializeWeightVector = [&](const size_t connectionCount, auto& layer) {
	//	for (auto& e : layer)
	//	{
	//		e.resize(connectionCount);
	//		// Setting each weight initially to a random value between [0.0] and [1.0] (inclusive)
	//		ranges::for_each(e, [](auto& e) { e = getRandomDouble(); });
	//	}
	//};



	//// Construct and init the layers.
	//array<vector<double>, LayerCount1> firstLayer;
	//// So, the connection count for the first layer is 784 for each image pixel value.
	//initializeWeightVector(inputBuffer.front().GetImageSize(), firstLayer);
	//// And similarly for the second layer, the connection count is 16 for each element of the first layer.
	//array<vector<double>, LayerCount2> secondLayer;
	//initializeWeightVector(firstLayer.size(), secondLayer);
	//// And for the output layer, 16 connections matching up to the second layer, and 10 elements (1 for each number).
	//array<vector<double>, LayerCount3> outputLayer;
	//initializeWeightVector(secondLayer.size(), outputLayer);

	// Perform inner_product of input image data to first layer perceptrons.


	// Added to the inner_product computation performed between input array and weight array.
	//double biasValue{ -15.0 };
}


auto GetImageBufferFromFile(const std::string& path)
-> std::expected<std::vector<Idx3Lib::Idx3ImageDataBuffer>, std::string>
{
	std::osyncstream ss(std::cout);
	//open in file
	std::ifstream currentFile(path, std::ios::in | std::ios::binary);
	if (currentFile)
	{
		//read header
		Idx3Lib::Idx3HeaderData myHeader;
		currentFile >> myHeader;
		if (!currentFile)
		{
			return std::unexpected("Failed to read header!");
		}
		ss << "Logged a header: " << myHeader << '\n';
		ss << "Done getting input file header." << '\n';

		//compute image size from the header attributes [Row Size] and [Col Size]
		//const size_t image_size = static_cast<size_t>(myHeader.num_columns) * myHeader.num_rows;
		const size_t NumImages = myHeader.num_images;
		//store images in vector here
		std::vector<Idx3Lib::Idx3ImageDataBuffer> imageList;
		ss << "Constructing image list...\n";
		//for each image based on the [Num Images] position
		for (size_t i = 0; i < NumImages; i++)
		{
			Idx3Lib::Idx3ImageDataBuffer currentImage(myHeader.num_columns, myHeader.num_rows);
			currentFile >> currentImage;
			if (!currentFile && !currentFile.eof())
			{
				return std::unexpected("Failed during reading the images!");
			}
			if (currentImage.LastReadCount != currentImage.ImageBuffer.size())
			{
				return std::unexpected(std::format("Size mismatch, expected {} bytes, got {} bytes.", 
					currentImage.ImageBuffer.size(),
					currentImage.LastReadCount
				));
			}
			imageList.emplace_back(currentImage);
		}
		ss << std::format("Added images.\n", imageList.size());
		return imageList;
	}
	return std::unexpected("File failed to open.");
}


/**
 * \brief This simulates an AND gate, by modifying the bias the weights usually will fall into a range
 * making it possible to discriminate well enough to replicate the AND gate over a reasonable range of weight values.
 */
auto run_test_and_gate()->void
{
	using namespace std;
	// The inputs are two elements, corresponding to AND gate input values.
	const auto doRun = [](auto& p)
	{
		cout << "Bias: " << p.bias << '\n';
		cout << "Run: " << (p.Run(vector{ 0, 0 })) << '\n';
		cout << "Run: " << (p.Run(vector{ 1, 0 })) << '\n';
		cout << "Run: " << (p.Run(vector{ 0, 1 })) << '\n';
		cout << "Run: " << (p.Run(vector{ 1, 1 })) << '\n';
	};

	//const auto SigmoidAct = [](const double x) { return Acts::SigmoidFunctionFFN_Multilayer(x); };
	//const auto HTanAct = [](const double x) -> double { return Acts::HyperbolicTangentFunctionFFN_Multilayer(x); };
	Perceptron p(2);
	p.actFn = Acts::RectifiedLinearUnitFFN_Multilayer;
	//p.doubleFn = getRandomDouble;

	//p.weights = { 0.5, 0.5 };
	for (double d = -15.0; d < 15.0; d += 0.1)
	{
		p.bias = d;
		doRun(p);
		string buf;
		getline(cin, buf);
	}
}

/**
 * \brief This is a more involved test, it loads MNIST image data and builds a
 * [input nodes:784] -> [layer1:16] -> [layer2:16] -> [ouputs:10] NN that uses 784 input nodes.
 */
auto run_test_image_net()->void
{
	using std::cout, std::cerr, std::begin, std::end, std::for_each, std::vector, std::size_t, std::string;
	constexpr size_t L1Size{ 16 };
	constexpr size_t L2Size{ 16 };
	constexpr size_t OutSize{ 10 };

	// 1. Read all of the MNIST images into memory (they are small).
	const std::string imFileName = "../MNISTtestdata/train-images.idx3-ubyte";
	const auto imageBuffer = GetImageBufferFromFile(imFileName);
	if(!imageBuffer)
	{
		cerr << "Error opening file and/or reading contents.\n";
		cerr << std::format("Message: {}'\n'", imageBuffer.error());
		return;
	}
	// 2. Choose a single image to work with.
	const auto& currentImage = imageBuffer.value().front();
	const auto imageSize = currentImage.GetImageSize();

	cout << "MNIST image size reported as: " << imageSize << '\n';

	// 3. Construct each layer, note that the input nodes (image data) aren't a distinct layer.
	// Our perceptron type should default to a ReLU activation function instead of sigmoid, so
	// we don't set that explicitly here.
	vector<Perceptron> layer1(L1Size);
	vector<Perceptron> layer2(L2Size );
	vector<Perceptron> outputNodes(OutSize);

	assert(layer1.size() == L1Size);
	assert(layer2.size() == L2Size);
	assert(outputNodes.size() == OutSize);

	// 4. Set the layer(s) weight count (number of connections from the "left" side) for each perceptron.
	// (the weights for each connection are stored in the rhs of the multiplication operation, i.e., input nodes have no
	// internal connection weight stored, but the first layer does.)
	for (auto& elem : layer1)
	{
		elem.Resize(imageSize);
	}
	for (auto& elem : layer2)
	{
		elem.Resize(L1Size);
	}
	for(auto& elem: outputNodes)
	{
		elem.bias = -182'175'371'903.0;
		elem.Resize(L2Size);
	}
	cout << "Done constructing NN layers...\n";

	// 5. Perform the array multiplications for input nodes -> first layer.
	for (auto& elem : layer1)
	{
		// the run result is stored in the perceptron as well
		elem.Run(currentImage.ImageBuffer);
	}
	// 6. Perform the array multiplications for first layer -> second layer.
	for (auto& elem : layer2)
	{
		// Run the single perceptron from layer2 against the layer1 perceptrons.
		elem.Run(layer1);
	}
	// 7. Perform the array multiplication for second layer -> output layer.
	for(size_t i{}; i < outputNodes.size(); ++i)
	{
		const auto res = outputNodes[i].Run(layer2);
		cout << std::format("{}:{:24.5f}\n", i, res);
		//cout << std::fixed << std::setprecision(4) << i << ": " << res << '\n';
	}

	cout << "Done multiplying the arrays.\nEnter to continue.\n";
	string temp;
	std::getline(std::cin, temp);

	//auto getInputsToNextLayer = [](const std::ranges::range auto& im, std::vector<Perceptron>& layer)
	//{
	//	std::vector<double> inputs;
	//	inputs.reserve(layer.size());
	//	for (auto& elem : layer)
	//	{
	//		inputs.emplace_back(NetLayerFunction(im, elem.weights));
	//	}
	//	return inputs;
	//};
	// For each training image, get the list of inputs to the next layer,
	// and run that activation too.
	//for (const auto& im : imageBuffer.value())
	//{
	//	const auto lin = getInputsToNextLayer(im.ImageBuffer, layer1);
	//	assert(lin.size() == 16);
	//	const auto lin2 = getInputsToNextLayer(lin, layer2);
	//	assert(lin2.size() == 16);
	//	//for_each(begin(lin2), end(lin2), [](const auto elem)
	//	//	{
	//	//		cout << elem << " ";
	//	//	});
	//	//cout << '\n';
	//}

}

