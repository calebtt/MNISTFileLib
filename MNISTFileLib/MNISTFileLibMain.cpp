#include <iostream>
#include <fstream>
#include <future>
#include <thread>

#include "Idx3HeaderData.hpp"
#include "Idx3ImageDataBuffer.hpp"

bool read_vector(const std::string &path)
{
	auto HandleErrorCondition = [](const std::string_view s)
	{
		std::cerr << s << std::endl;
		return false;
	};
	std::ifstream currentFile(path, std::ios::in | std::ios::binary);
	if (currentFile)
	{
		//read header
		Idx3Lib::Idx3HeaderData myHeader;
		currentFile >> myHeader;
		std::cerr << "Logged a header: " << myHeader << std::endl;
		if (!currentFile)
		{
			return HandleErrorCondition("Failed to read header!");
		}
		std::cout << "Done getting input file header." << std::endl;
		//compute image size from the header attributes [Row Size] and [Col Size]
		const size_t image_size = static_cast<size_t>(myHeader.num_columns) * myHeader.num_rows;
		const size_t NumImages = myHeader.num_images;
		size_t imagesRead = 0;
		std::cout << "Reading images..." << std::endl;
		//for each image based on the [Num Images] position
		for (size_t i = 0; i < NumImages; i++)
		{
			Idx3Lib::Idx3ImageDataBuffer currentImage(image_size);
			currentFile >> currentImage;
			if (!currentFile && !currentFile.eof())
				return HandleErrorCondition("Failed during reading the images!");
			if (currentImage.ImageSize != currentImage.buffer.size())
				return HandleErrorCondition("Size mismatch, expected" + std::to_string(currentImage.ImageSize) + " bytes, got " + std::to_string(currentImage.buffer.size()) + " bytes.");
			imagesRead++;
		}
		std::cout << "Read " << imagesRead << " images." << std::endl;
	}
	else
	{
		return HandleErrorCondition("File failed to open.");
	}
	return true;
}

int main()
{
	using namespace std;
	const std::string firstFileName = "t10k-images.idx3-ubyte";
	const std::string secondFileName = "train-images.idx3-ubyte";
	auto ReadFileLocal = [](const std::string fileName)
	{
		cout << "Reading file: " << fileName << endl;
		const bool result = read_vector(fileName);
		cout << "Finished reading file: " << fileName << endl;
		return result;
	};
	cout << "Beginning file reads..." << endl;
	future<bool> firstReturnVal = async(ReadFileLocal, firstFileName);
	future<bool> secondReturnVal = async(ReadFileLocal, secondFileName);

	firstReturnVal.wait();
	if (firstReturnVal.valid())
		cout << "First thread using file: " << firstFileName << " completed with result: " << firstReturnVal.get() << endl;
	secondReturnVal.wait();
	if (secondReturnVal.valid())
		cout << "Second thread using file: " << secondFileName << " completed with result: " << secondReturnVal.get() << endl;
	cout << "Ended file reads..." << endl;
}