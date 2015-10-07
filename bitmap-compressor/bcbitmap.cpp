#include "stdafx.h"
#include "bcbitmap.h"
#include "bcdds.h"

#include <fstream>
#include <iostream>
#include <cstdlib>

using namespace std;

#pragma region Helpers
void BCBitmap::copy(char* dst, const uint8_t* src, int size)
{
	for (int i = 0; i < size; i++)
	{
		dst[i] = (char)src[i];
	}
}

std::ostream& operator<<(std::ostream& out, const BCBitmap& b)
{
	return out << "bitmap" << endl;
}
#pragma endregion

#pragma region Read bitmap data
void BCBitmap::loadBitmap(const string& imgPath)
{
	loaded = false;
	cout << "Loading " << imgPath << " to memory";
	fstream fs;
	try
	{
		fs.open(imgPath);
		if (fs.good())
		{
			// get length of file:
			fs.seekg(0, fs.end);
			int length = fs.tellg();
			fs.seekg(0, fs.beg);

			istream_iterator<uint8_t> it(fs);
			currPos = 0;
			loaded = readHeader(it) && readDib(it) && readPixelArray(it);
			fs.close();
		}
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
	}
	catch (...)
	{
		cerr << "An exception occured";
	}
	
	if (!loaded)
	{
		dib = nullptr;
		header = nullptr;
		pixelArray = nullptr;
		cerr << "Failed to load bitmpad" << endl;
	}	
}

bool BCBitmap::readHeader(istream_iterator<uint8_t>& it)
{
	shared_ptr<HEADER> h(new HEADER());

	uint8_t arr[2];
	if (!readNext(it, sizeof(arr), arr)) return false;
	copy(h->id, arr, sizeof(arr));
	if (!readNext(it, h->size)) return false;
	if (!readNext(it, h->reserve1)) return false;
	if (!readNext(it, h->reserve2)) return false;
	if (!readNext(it, h->offset)) return false;
	
	header = h;
	return true;
}
bool BCBitmap::readDib(istream_iterator<uint8_t>& it)
{
	if (header == nullptr)
		return false;

	shared_ptr<DIB> d(new DIB());

	if (!readNext(it, d->headerSize)) return false;
	if (!readNext(it, d->width)) return false;
	if (!readNext(it, d->height)) return false;
	if (!readNext(it, d->planes)) return false;
	if (!readNext(it, d->colourDepth)) return false;
	if (!readNext(it, d->compression)) return false;
	if (!readNext(it, d->imgSize)) return false;
	if (!readNext(it, d->xResolution)) return false;
	if (!readNext(it, d->yResolution)) return false;
	if (!readNext(it, d->pallete)) return false;
	if (!readNext(it, d->important)) return false;

	// To avoid extra padding bytes
	if (d->width % 4 != 0)
	{
		cerr << "Bitmap width not a multiple of 4" << endl;
		return false;
	}

	if (d->height % 4 != 0)
	{
		cerr << "Bitmap height not a multiple of 4" << endl;
		return false;
	}
	// Offset for later versions of header
	
	dib = d;
	return true;
}

bool BCBitmap::readPixelArray(istream_iterator<uint8_t>& it)
{
	if (header == nullptr)
		return false;

	int arraySize = dib->imgSize;

	// Store bitmap pixel data to memory
	auto arr = shared_ptr<uint8_t>(new (std::nothrow) uint8_t[arraySize]);
	int byteCount = 0;
	for (int i = 0; i < arraySize; i++)
	{
		if (!readNext(it, arr.get()[i]))
		{
			// Some pixels are missing in the file
			continue;
		}
		byteCount++;
	}

	pixelArray = arr;

	return true;
}

bool BCBitmap::readNext(istream_iterator<uint8_t>& it, int size, uint8_t* out)
{
	istream_iterator<uint8_t> eos;      // end-of-stream iterator
	for (int i = 0; i < size; i++)
	{
		if (it != eos)
		{
			out[i] = *it;
			++it;
			++currPos;
		}
		else
		{
			// Reached end of stream
			return false;
		}
	}

	return true;
}

bool BCBitmap::readNext(istream_iterator<uint8_t>& it, uint32_t& out)
{
	uint8_t arr[4];
	if (!readNext(it, sizeof(arr), arr)) return false;
	
	uint32_t temp = *(uint32_t *)arr;
	out = *(uint32_t *)arr;

	return true;
}

bool BCBitmap::readNext(istream_iterator<uint8_t>& it, uint8_t& out)
{
	uint8_t arr[1];
	if (!readNext(it, sizeof(arr), arr)) return false;

	out = *(uint8_t *)arr;

	return true;
}

bool BCBitmap::readNext(istream_iterator<uint8_t>& it, uint16_t& out)
{
	uint8_t arr[2];
	if (!readNext(it, sizeof(arr), arr)) return false;

	out = *(uint16_t *)arr;

	return true;
}

bool BCBitmap::readNext(istream_iterator<uint8_t>& it, int32_t& out)
{
	uint32_t a;
	if (!readNext(it, a)) return false;

	out = (int32_t)a;
	return true;
}
bool BCBitmap::readNext(istream_iterator<uint8_t>& it, int16_t& out)
{
	uint16_t a;
	if (!readNext(it, a)) return false;

	out = (int16_t)a;
	return true;
}
#pragma endregion

#pragma region Write bitmap data
void BCBitmap::saveBitmap(const string& imgPath)
{
	cout << "Saving bitmap to " << imgPath;
	ofstream fs;
	fs.open(imgPath, ofstream::out | ofstream::trunc);

	if (fs.good() && dib != nullptr && pixelArray != nullptr && header != nullptr)
	{
		writeHeader(fs);
		writeDib(fs);
		writePixelArray(fs);
		fs.close();
	}
}

void BCBitmap::writeHeader(ostream& s)
{
	s.write((char*) &header->id, 2);
	s.write((char*) &header->size, sizeof(uint32_t));
	s.write((char*) &header->reserve1, sizeof(uint16_t));
	s.write((char*) &header->reserve2, sizeof(uint16_t));
	s.write((char*) &header->offset, sizeof(uint32_t));
}

void BCBitmap::writeDib(ostream& s)
{
	s.write((char*) &dib->headerSize, sizeof(uint32_t));
	s.write((char*) &dib->width, sizeof(int32_t));
	s.write((char*) &dib->height, sizeof(int32_t));
	s.write((char*) &dib->planes, sizeof(uint16_t));
	s.write((char*) &dib->colourDepth, sizeof(uint16_t));
	s.write((char*) &dib->compression, sizeof(uint32_t));
	s.write((char*) &dib->imgSize, sizeof(uint32_t));
	s.write((char*) &dib->xResolution, sizeof(int32_t));
	s.write((char*) &dib->yResolution, sizeof(int32_t));
	s.write((char*) &dib->pallete, sizeof(uint32_t));
	s.write((char*) &dib->important, sizeof(uint32_t));
}

void BCBitmap::writePixelArray(ostream& s)
{
	int arraySize = dib->imgSize;

	for (int i = 0; i < arraySize; i++)
	{
		s.write((char*) &(pixelArray.get()[i]), sizeof(uint8_t));
	}
}
#pragma endregion

#pragma region Compress bitmap
BCDds* BCBitmap::compressDXT1()
{
	return nullptr;
}

BCDds* BCBitmap::compressDXT2()
{
	return nullptr;
}

BCDds* BCBitmap::compressDXT3()
{
	return nullptr;
}

BCDds* BCBitmap::compressDXT4()
{
	return nullptr;
}

BCDds* BCBitmap::compressDXT5()
{
	return nullptr;
}
#pragma endregion

#pragma region Constructor & Destructor
BCBitmap::BCBitmap()
{
}

BCBitmap::~BCBitmap()
{
	header.reset();
	dib.reset();
	pixelArray.reset();
}

