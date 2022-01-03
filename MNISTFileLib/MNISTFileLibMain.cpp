#include <iostream>
#include <fstream>
#include "Idx3HeaderData.hpp"
#include "Idx3ImageDataBuffer.hpp"

void read_vector(std::string path)
{
	auto HandleErrorCondition = [](const std::string_view s, int errCode = 1)
	{
		std::cerr << s << std::endl;
		exit(errCode); // not thread safe!
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
			HandleErrorCondition("Failed to read header!");
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
				HandleErrorCondition("Failed during reading the images!");
			if (currentImage.ImageSize != currentImage.buffer.size())
				HandleErrorCondition("Size mismatch, expected" + std::to_string(currentImage.ImageSize) + " bytes, got " + std::to_string(currentImage.buffer.size()) + " bytes.");
			imagesRead++;
		}
		std::cout << "Read " << imagesRead << " images." << std::endl;
	}
	else
	{
		HandleErrorCondition("File failed to open.");
	}
}

int main()
{
	using namespace std;
	const std::string fileName = "t10k-images.idx3-ubyte";
	cout << "Beginning call to read..." << endl;
	read_vector(fileName);
	cout << "Ended call to read..." << endl;
}