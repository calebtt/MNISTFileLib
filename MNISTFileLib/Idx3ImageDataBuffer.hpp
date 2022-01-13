#pragma once
#include "stdafx.h"
namespace Idx3Lib
{
	/// <summary>
	/// Class is not reusable! You will need to instantiate a new object
	/// to change the image size.
	/// </summary>
	struct Idx3ImageDataBuffer
	{
		//Source: https://deepai.org/dataset/mnist
		/*
		 * [offset]    [type]          [value]          [description]
			0000     32 bit integer  0x00000803(2051) magic number
			0004     32 bit integer  60000            number of images
			0008     32 bit integer  28               number of rows
			0012     32 bit integer  28               number of columns
			0016     unsigned byte   ??               pixel
			0017     unsigned byte   ??               pixel
			........
			xxxx     unsigned byte   ??               pixel
			Pixels are organized row-wise.
			Pixel values are 0 to 255. 0 means background (white), 255 means foreground (black).
		 */
		static constexpr bool SwitchEndian = (std::endian::native != std::endian::big);
		using Bits8Type = unsigned char;
		static_assert(sizeof(Bits8Type) == 1);
		const size_t ImageSize;
		std::vector<Bits8Type> buffer;
		explicit Idx3ImageDataBuffer(size_t imageSize) : ImageSize(imageSize) { }
		/// <summary>
		/// Reads image data into the struct, precondition is that the file stream's read pointer is at the beginning of the image data!
		/// Reads ImageSize bytes.
		/// input operator
		/// </summary>
		friend std::istream& operator>>(std::ifstream& is, Idx3ImageDataBuffer& obj)
		{
			obj.buffer.resize(obj.ImageSize);
			is.read(reinterpret_cast<char*> (obj.buffer.data()), sizeof(Bits8Type)*obj.buffer.size());
			return is;
		}
		/// <summary>
		/// Copies internal buffer into output stream.
		/// output operator
		/// </summary>
		friend std::ofstream& operator<<(std::ofstream& os, const Idx3ImageDataBuffer& obj)
		{
			std::for_each(std::begin(obj.buffer), std::end(obj.buffer), [&os,&obj](auto& elem)
				{
					if (obj.SwitchEndian)
					{
						auto fixed = swap_endian(elem);
						os.put(fixed);
					}
					else
					{
						os.put(elem);
					}
				});
			return os;
		}
	};
}