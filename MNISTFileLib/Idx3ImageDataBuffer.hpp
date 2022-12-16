#pragma once
#include "stdafx.h"
namespace Idx3Lib
{
	/// <summary> Class object used for reading an image from the MNIST data source
	///	of the IDX3 format. Size of the image to be read must be known before-hand, and
	///	the file pointer in the ifstream should be placed appropriately to read the beginning of the image data. </summary>
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
	public:
		static constexpr bool SwitchEndian = (std::endian::native != std::endian::big);
		using Bits8Type = unsigned char;
		static_assert(sizeof(Bits8Type) == 1);
		std::vector<Bits8Type> ImageBuffer;
		std::size_t LastReadCount{};
		unsigned Col_Count{};
		unsigned Row_Count{};

		/// <summary> Specify the image size in bytes to be read. </summary>
		///	<param name="col_count">num columns</param>
		///	<param name="row_count">num rows</param>
		explicit Idx3ImageDataBuffer(const unsigned col_count, const unsigned row_count)
		: ImageBuffer(col_count * row_count), Col_Count(col_count), Row_Count(row_count) { }

		/// <summary> Resets the size of the buffer, which determines how many bytes will be read
		///	when the stream operator overloads are used on an input file. </summary>
		/// <param name="newSize"> Image size to be read in (in bytes). </param>
		void ResetBufferSize(const std::size_t newSize)
		{
			ImageBuffer.resize(newSize);
		}

		/**
		 * \brief Gets the size pair for the image.
		 * \return Returns pair of column_count, row_count
		 */
		[[nodiscard]]
		auto GetImageSize() const noexcept
			-> std::size_t
		{
			return Col_Count * Row_Count;
		}

		/// <summary> Reads image data into the struct, precondition is that the file stream's read pointer is at the beginning of the image data!
		/// Reads ImageSize bytes. input operator </summary>
		friend std::istream& operator>>(std::ifstream& is, Idx3ImageDataBuffer& obj)
		{
			is.read(reinterpret_cast<char*> (obj.ImageBuffer.data()), static_cast<std::streamsize>(sizeof(Bits8Type) * obj.ImageBuffer.size()));
			obj.LastReadCount = static_cast<size_t>(is.gcount());
			return is;
		}

		/// <summary> Copies internal buffer into output stream. output operator </summary>
		friend std::ofstream& operator<<(std::ofstream& os, const Idx3ImageDataBuffer& obj)
		{
			// If we don't test to see if the file is open first and in goodbit state, it may throw an exception.
			for (const auto& elem : obj.ImageBuffer)
			{
				if constexpr (obj.SwitchEndian)
				{
					const auto fixed = static_cast<char>(swap_endian(elem));
					os.put(fixed);
				}
				else
				{
					os.put(static_cast<char>(elem));
				}
			}
			return os;
		}
	};
}