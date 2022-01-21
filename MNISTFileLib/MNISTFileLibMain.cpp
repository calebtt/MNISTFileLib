#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <future>
#include <thread>

#include "Idx3HeaderData.hpp"
#include "Idx3ImageDataBuffer.hpp"

bool read_vector(const std::string &path)
{
	std::osyncstream ss(std::cout);
	auto HandleErrorCondition = [&ss](const std::string_view s)
	{
		ss << s << std::endl;
		return false;
	};
	std::ifstream currentFile(path, std::ios::in | std::ios::binary);
	if (currentFile)
	{
		//read header
		Idx3Lib::Idx3HeaderData myHeader;
		currentFile >> myHeader;
		ss << "Logged a header: " << myHeader << std::endl;
		if (!currentFile)
		{
			return HandleErrorCondition("Failed to read header!");
		}
		ss << "Done getting input file header." << std::endl;
		//compute image size from the header attributes [Row Size] and [Col Size]
		const size_t image_size = static_cast<size_t>(myHeader.num_columns) * myHeader.num_rows;
		const size_t NumImages = myHeader.num_images;
		size_t imagesRead = 0;
		ss << "Reading images..." << std::endl;
		//for each image based on the [Num Images] position
		std::shared_ptr<Idx3Lib::Idx3ImageDataBuffer> imagePointer;
		for (size_t i = 0; i < NumImages; i++)
		{
			std::shared_ptr<Idx3Lib::Idx3ImageDataBuffer> currentImage = std::make_shared<Idx3Lib::Idx3ImageDataBuffer>(image_size);
			currentFile >> *currentImage;
			if (!currentFile && !currentFile.eof())
				return HandleErrorCondition("Failed during reading the images!");
			if (currentImage->ImageSize != currentImage->buffer.size())
				return HandleErrorCondition("Size mismatch, expected" + std::to_string(currentImage->ImageSize) + " bytes, got " + std::to_string(currentImage->buffer.size()) + " bytes.");
			imagesRead++;
			imagePointer = currentImage;
		}
		ss << "Read " << imagesRead << " images." << std::endl;
	}
	else
	{
		return HandleErrorCondition("File failed to open.");
	}
	return true;
}

bool CopyDataFile(const std::string &path)
{
	std::osyncstream ss(std::cout);
	auto HandleErrorCondition = [&ss](const std::string_view s)
	{
		ss << s << std::endl;
		return false;
	};
	//open in and out files
	std::ifstream currentFile(path, std::ios::in | std::ios::binary);
	auto id = std::this_thread::get_id();
	std::stringstream strs;
	strs << id;
	std::ofstream outFile(std::format("{0}.txt", strs.str()), std::ios::binary); //unique thread id filename
	if (currentFile)
	{
		//read header
		Idx3Lib::Idx3HeaderData myHeader;
		currentFile >> myHeader;
		if (!currentFile)
		{
			return HandleErrorCondition("Failed to read header!");
		}
		else
			ss << "Logged a header: " << myHeader << std::endl;
		ss << "Done getting input file header." << std::endl;
		ss << "Writing header to output." << std::endl;
		outFile << myHeader;

		//compute image size from the header attributes [Row Size] and [Col Size]
		const size_t image_size = static_cast<size_t>(myHeader.num_columns) * myHeader.num_rows;
		const size_t NumImages = myHeader.num_images;
		size_t imagesRead = 0;
		ss << "Copying images..." << std::endl;
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
			outFile << currentImage;
		}
		ss << "Copied " << imagesRead << " images." << std::endl;
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
	std::osyncstream ss(std::cout);
	auto ReadFileLocal = [&ss](const std::string fileName)
	{
		ss << "Reading file: " << fileName << endl;
		const bool result = read_vector(fileName);
		ss << "Finished reading file: " << fileName << endl;
		return result;
	};
	ss << "Beginning async file copies..." << endl;
	future<bool> firstReturnVal = async(CopyDataFile, firstFileName);
	future<bool> secondReturnVal = async(CopyDataFile, secondFileName);

	firstReturnVal.wait();
	if (firstReturnVal.valid())
		ss << "First thread using file: " << firstFileName << " completed with result: " << firstReturnVal.get() << endl;
	secondReturnVal.wait();
	if (secondReturnVal.valid())
		ss << "Second thread using file: " << secondFileName << " completed with result: " << secondReturnVal.get() << endl;
	ss << "Ended file copies..." << endl;
}