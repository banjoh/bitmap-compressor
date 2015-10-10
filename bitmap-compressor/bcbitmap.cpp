#include "stdafx.h"
#include "bcdds.h"
#include "bcbitmap.h"
#include "reader.h"
#include "algorithm.h"

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;

#pragma region Read bitmap data
void BCBitmap::loadBitmap(const string& imgPath)
{
	_loaded = false;
	cout << "Loading " << imgPath << " to memory" << endl;
	ifstream fs;
	try
	{
		fs.open(imgPath, ios::binary);
		if (fs.good())
		{
			fs.unsetf(ios::skipws);
			istream_iterator<uint8_t> it(fs);
			_loaded = readHeader(it) && readDib(it) && readPixelArray(it);
			fs.close();
		}
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
	}
	catch (...)
	{
		cerr << "An exception occured" << endl;
	}
	
	if (!_loaded)
	{
		dib = nullptr;
		header = nullptr;
		cerr << "Failed to load bitmap" << endl;
	}	
}

bool BCBitmap::readHeader(istream_iterator<uint8_t>& it)
{
	shared_ptr<HEADER> h(new HEADER());

	uint8_t arr[2];
	if (!Reader::readNext(it, sizeof(arr), arr)) return false;
	Reader::copy(h->id, arr, sizeof(arr));
	if (!Reader::readNext(it, h->size)) return false;
	if (!Reader::readNext(it, h->reserve1)) return false;
	if (!Reader::readNext(it, h->reserve2)) return false;
	if (!Reader::readNext(it, h->offset)) return false;
	
	header = h;
	return true;
}
bool BCBitmap::readDib(istream_iterator<uint8_t>& it)
{
	if (header == nullptr)
		return false;

	shared_ptr<DIB> d(new DIB());

	if (!Reader::readNext(it, d->headerSize)) return false;
	if (!Reader::readNext(it, d->width)) return false;
	if (!Reader::readNext(it, d->height)) return false;
	if (!Reader::readNext(it, d->planes)) return false;
	if (!Reader::readNext(it, d->colourDepth)) return false;
	if (!Reader::readNext(it, d->compression)) return false;
	if (!Reader::readNext(it, d->imgSize)) return false;
	if (!Reader::readNext(it, d->xResolution)) return false;
	if (!Reader::readNext(it, d->yResolution)) return false;
	if (!Reader::readNext(it, d->pallete)) return false;
	if (!Reader::readNext(it, d->important)) return false;

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

	unique_ptr<PIXELDATA> pixelData(new PIXELDATA());

	// Create a 2D array of pixel data
	PIXEL** pixels = new PIXEL*[dib->height];
	for (int i = 0; i < dib->height; i++)
	{
		pixels[i] = new PIXEL[dib->width];
		for (int j = 0; j < dib->width; j++)
		{
			if (!Reader::readNext(it, pixels[i][j]))
			{
				return false;
			}
		}
	}
	pixelData->pixels = pixels;
	pixelData->height = dib->height;
	pixelData->width = dib->width;

	return true;
}
#pragma endregion

#pragma region Write bitmap data
bool BCBitmap::saveBitmap(const string& imgPath)
{
	cout << "Saving bitmap to " << imgPath << endl;
	ofstream fs;
	fs.open(imgPath, ofstream::out | ofstream::trunc | ios::binary);
	fs.unsetf(ios::skipws);

	if (fs.good() && dib != nullptr && header != nullptr && pixelData != nullptr)
	{
		writeHeader(fs);
		writeDib(fs);
		writePixelArray(fs);
		fs.close();
		return true;
	}

	return false;
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
	PIXEL** pixels = pixelData->pixels;
	int count = 0;
	for (int i = 0; i < pixelData->height; i++)
	{
		for (int j = 0; j < pixelData->width; j++)
		{
			PIXEL p = pixels[i][j];
			s.write((char*)&(p.red), sizeof(uint8_t));
			s.write((char*)&(p.green), sizeof(uint8_t));
			s.write((char*)&(p.blue), sizeof(uint8_t));
			count++;
		}
	}
}
#pragma endregion

#pragma region Compress bitmap
BCDds* BCBitmap::compressDXT1()
{
	unique_ptr<BCDds> bds(new BCDds());
	shared_ptr<DXT> d(new DXT());
	
	d->dwMagic = DDS_MAGIC;

	d->header.dwSize = DDS_HEADER_SIZE;
	d->header.dwFlags = DDSD_PIXELFORMAT | DDSD_CAPS;
	d->header.dwHeight = dib->width;
	d->header.dwWidth = dib->height;
	// DXT1 max( 1, ((width+3)/4) ) * block-size
	d->header.dwPitchOrLinearSize = max(1, ((d->header.dwWidth + 3) / 4)) * 8;
	d->header.dwDepth = 0;
	d->header.dwMipMapCount = 0;
	d->header.dwReserved1[11];
	for (int i = 0; i < 11; i++)
	{
		d->header.dwReserved1[i] = 0;
	}

	d->header.ddspf.dwSize = DDS_PIXELFORMAT_SIZE;
	d->header.ddspf.dwFlags = DDPF_FOURCC;
	d->header.ddspf.dwFourCC = DXT1_FOURCC;
	d->header.ddspf.dwRGBBitCount = 0;
	d->header.ddspf.dwRBitMask = 0;
	d->header.ddspf.dwGBitMask = 0;
	d->header.ddspf.dwBBitMask = 0;
	d->header.ddspf.dwABitMask = 0;

	d->header.dwCaps = DDSCAPS_TEXTURE;
	d->header.dwCaps2 = 0;
	d->header.dwCaps3 = 0;
	d->header.dwCaps4 = 0;
	d->header.dwReserved2 = 0;

	auto p = pixelData->pixels;
	shared_ptr<PIXEL> sqr(new PIXEL[TEXEL_WIDTH *  TEXEL_WIDTH]);
	auto texelWidth = dib->width / TEXEL_WIDTH;
	auto texelHeight = dib->height / TEXEL_WIDTH;
	d->texelLength = texelWidth * texelHeight;
	d->texels = unique_ptr<TEXEL>(new TEXEL[d->texelLength]);
	int txIdx = 0;
	int x = 0;
	int y = 0;

	// Encode pixels
	for (int i = 0; i < texelHeight; i++)
	{
		// Outer loop to iterate through the height
		for (int j = 0; j < texelWidth; j++)
		{
			// Inner loop for the width
			int idx = 0;
			for (int k = 0; k < TEXEL_WIDTH; k++)
			{
				for (int l = 0; l < TEXEL_WIDTH; l++)
				{
					x = (j * TEXEL_WIDTH) + l;
					
					// Write pixel data from bottom to top
					y = dib->height - (((i * TEXEL_WIDTH) + k) + 1);

					sqr.get()[idx] = p[y][x];
					++idx;
				}
			}

			d->texels.get()[txIdx] = Algorithm::makeTexel(sqr.get());
			++txIdx;
		}
	}

	bds->dxt = d;
	return bds.release();
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
	pixelData.reset();
}

