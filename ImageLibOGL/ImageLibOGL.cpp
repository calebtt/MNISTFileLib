// ImageLibOGL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <format>
#include <string>
#include <expected>
#include <optional>
#include <vector>

#include "ImgWindow.h"
#include "../MNISTFileLib/Idx3HeaderData.hpp"
#include "../MNISTFileLib/Idx3ImageDataBuffer.hpp"

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
		const size_t image_size = static_cast<size_t>(myHeader.num_columns) * myHeader.num_rows;
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

int main()
{
	// 1. Read all of the MNIST images into memory (they are small).
	//const std::string imFileName = "../MNISTtestdata/train-images.idx3-ubyte";
	const std::string imFileName = "../MNISTtestdata/t10k-images.idx3-ubyte";
	const auto imageBuffer = GetImageBufferFromFile(imFileName);
	if (!imageBuffer)
	{
		std::cerr << "Error opening file and/or reading contents.\n";
		std::cerr << std::format("Message: {}\n", imageBuffer.error());
		return 1;
	}
	// 2. Choose a single image to work with.
	const auto& currentImage = imageBuffer.value().front();
	const auto imageSize = currentImage.GetImageSize();

	std::cout << "MNIST image size reported as: " << imageSize << '\n';

    test_glfw_ogl3(currentImage);
}
