#pragma once
#include "stdafx.h"
namespace Idx3Lib
{
	struct Idx3HeaderData
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

			xxxx     unsigned byte   ??               pixel
			Pixels are organized row-wise.
			Pixel values are 0 to 255. 0 means background (white), 255 means foreground (black).
		 */
		constexpr static int NUM_ELEMENTS = 4; // number of data members in this class that are part of the header.
		const bool SwitchEndian = (std::endian::native != std::endian::big);
		using Bits32Type = unsigned int;
		using Bits8Type = unsigned char;
		static_assert(sizeof(Bits32Type) == 4);
		Bits32Type magic = 0;
		Bits32Type num_images = 0;
		Bits32Type num_rows = 0;
		Bits32Type num_columns = 0;
		/// <summary>
		/// Reads header data into struct, precondition is that the file stream's read pointer is at the beginning of the file!
		/// input operator
		/// </summary>
		friend std::istream& operator>>(std::ifstream& is, Idx3HeaderData& obj)
		{
			is.read(reinterpret_cast<char*> (&obj.magic), sizeof(Idx3HeaderData::Bits32Type));
			is.read(reinterpret_cast<char*> (&obj.num_images), sizeof(Idx3HeaderData::Bits32Type));
			is.read(reinterpret_cast<char*> (&obj.num_rows), sizeof(Idx3HeaderData::Bits32Type));
			is.read(reinterpret_cast<char*> (&obj.num_columns), sizeof(Idx3HeaderData::Bits32Type));

			if (obj.SwitchEndian)
			{
				obj.magic = obj.swap_endian(obj.magic);
				obj.num_images = obj.swap_endian(obj.num_images);
				obj.num_rows = obj.swap_endian(obj.num_rows);
				obj.num_columns = obj.swap_endian(obj.num_columns);
			}
			return is;
		}
		//output operator
		friend std::ostream& operator<<(std::ostream& os, const Idx3HeaderData& obj)
		{
			return os
				<< "magic: " << obj.magic
				<< " num_images: " << obj.num_images
				<< " num_rows: " << obj.num_rows
				<< " num_columns: " << obj.num_columns;
		}
		//explicit specialization for ofstream writing
		friend std::ostream& operator<<(std::ofstream& os, const Idx3HeaderData& obj)
		{
			os.write(reinterpret_cast<const char*> (&obj.magic), sizeof(Idx3HeaderData::Bits32Type));
			os.write(reinterpret_cast<const char*> (&obj.num_images), sizeof(Idx3HeaderData::Bits32Type));
			os.write(reinterpret_cast<const char*> (&obj.num_rows), sizeof(Idx3HeaderData::Bits32Type));
			os.write(reinterpret_cast<const char*> (&obj.num_columns), sizeof(Idx3HeaderData::Bits32Type));
			return os;
		}
		//Stackoverflow user Alexandre C.
		template <typename T>
		T swap_endian(T u)
		{
			static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");
			union
			{
				T u;
				unsigned char u8[sizeof(T)];
			} source, dest;
			source.u = u;
			for (size_t k = 0; k < sizeof(T); k++)
				dest.u8[k] = source.u8[sizeof(T) - k - 1];
			return dest.u;
		}
	};
}