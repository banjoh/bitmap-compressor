#include "stdafx.h"
#include "bcdds.h"
#include "bcbitmap.h"
#include "reader.h"
#include "algorithm.h"

#include <fstream>
#include <iostream>

void BCDds::loadDds(const string& ddsFile)
{
	_loaded = false;
	cout << "Loading " << ddsFile << " to memory" << endl;
	ifstream fs;
	try
	{
		fs.open(ddsFile, ios::binary);
		if (fs.good())
		{
			fs.unsetf(ios::skipws);
			_loaded = readData(fs);
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
		cerr << "Failed to load dds file" << endl;
	}
}

bool BCDds::readHeader(istream_iterator<uint8_t>& it, DDS_HEADER& h)
{
	if (!Reader::readNext(it, h.dwSize)) return false;
	if (!Reader::readNext(it, h.dwFlags)) return false;
	if (!Reader::readNext(it, h.dwHeight)) return false;
	if (!Reader::readNext(it, h.dwWidth)) return false;

	if (h.dwWidth % 4 != 0)
	{
		cerr << "Bitmap width not a multiple of 4" << endl;
		return false;
	}

	if (h.dwHeight % 4 != 0)
	{
		cerr << "Bitmap height not a multiple of 4" << endl;
		return false;
	}

	if (!Reader::readNext(it, h.dwPitchOrLinearSize)) return false;
	if (!Reader::readNext(it, h.dwDepth)) return false;
	if (!Reader::readNext(it, h.dwMipMapCount)) return false;
	if (!Reader::readNext(it, sizeof(h.dwReserved1) / sizeof(uint32_t), h.dwReserved1)) return false;

	if (!Reader::readNext(it, h.ddspf.dwSize)) return false;
	if (!Reader::readNext(it, h.ddspf.dwFlags)) return false;
	if (!Reader::readNext(it, h.ddspf.dwFourCC)) return false;
	if (!Reader::readNext(it, h.ddspf.dwRGBBitCount)) return false;
	if (!Reader::readNext(it, h.ddspf.dwRBitMask)) return false;
	if (!Reader::readNext(it, h.ddspf.dwGBitMask)) return false;
	if (!Reader::readNext(it, h.ddspf.dwBBitMask)) return false;
	if (!Reader::readNext(it, h.ddspf.dwABitMask)) return false;
	
	if (!Reader::readNext(it, h.dwCaps)) return false;
	if (!Reader::readNext(it, h.dwCaps2)) return false;
	if (!Reader::readNext(it, h.dwCaps3)) return false;
	if (!Reader::readNext(it, h.dwCaps4)) return false;
	if (!Reader::readNext(it, h.dwReserved2)) return false;

	return true;
}

bool BCDds::readData(istream& is)
{
	is.seekg(0, is.end);
	int length = is.tellg();
	is.seekg(0, is.beg);

	if (length < DDS_HEADER_SIZE) return false;

	shared_ptr<DXT> d(new DXT());

	istream_iterator<uint8_t> it(is);
	if (!Reader::readNext(it, d->dwMagic)) return false;

	// Check if this is a valid DDS file
	if (d->dwMagic != DDS_MAGIC) return false;

	if (!readHeader(it, d->header)) return false;
	// Header sizes are also a fixed number
	if (d->header.dwSize != DDS_HEADER_SIZE || d->header.ddspf.dwSize != DDS_PIXELFORMAT_SIZE)
		return false;

	// Read in the rest of the data
	int dataLength = length - DDS_HEADER_SIZE - sizeof(d->dwMagic);
	d->texelLength = dataLength / sizeof(TEXEL);
	d->texels = unique_ptr<TEXEL>(new TEXEL[d->texelLength]);

	for (int i = 0; i < d->texelLength; i++)
	{
		if (!Reader::readNext(it, d->texels.get()[i])) return false;
	}

	dxt = d;
	return true;
}

bool BCDds::saveDds(const string& ddsFile)
{
	cout << "Saving bitmap to " << ddsFile << endl;
	ofstream fs;
	fs.open(ddsFile, ios::trunc | ios::binary);
	fs.unsetf(ios::skipws);

	if (fs.good() && dxt != nullptr)
	{
		writeData(fs);
		fs.close();
		return true;
	}

	return false;
}

void BCDds::writeData(ostream& s)
{
	s.write((char*)&dxt->dwMagic, sizeof(uint32_t));
	s.write((char*)&(dxt->header.dwSize), sizeof(uint32_t));
	s.write((char*)&(dxt->header.dwFlags), sizeof(uint32_t));
	s.write((char*)&(dxt->header.dwHeight), sizeof(uint32_t));
	s.write((char*)&(dxt->header.dwWidth), sizeof(uint32_t));
	s.write((char*)&(dxt->header.dwPitchOrLinearSize), sizeof(uint32_t));
	s.write((char*)&(dxt->header.dwDepth), sizeof(uint32_t));
	s.write((char*)&(dxt->header.dwMipMapCount), sizeof(uint32_t));

	int size = sizeof(dxt->header.dwReserved1) / sizeof(uint32_t);
	for (int i = 0; i < size; i++)
	{
		s.write((char*)&(dxt->header.dwReserved1[i]), sizeof(uint32_t));
	}
	s.write((char*)&(dxt->header.ddspf.dwSize), sizeof(uint32_t));
	s.write((char*)&(dxt->header.ddspf.dwFlags), sizeof(uint32_t));
	s.write((char*)&(dxt->header.ddspf.dwFourCC), sizeof(uint32_t));
	s.write((char*)&(dxt->header.ddspf.dwRGBBitCount), sizeof(uint32_t));
	s.write((char*)&(dxt->header.ddspf.dwRBitMask), sizeof(uint32_t));
	s.write((char*)&(dxt->header.ddspf.dwGBitMask), sizeof(uint32_t));
	s.write((char*)&(dxt->header.ddspf.dwBBitMask), sizeof(uint32_t));
	s.write((char*)&(dxt->header.ddspf.dwABitMask), sizeof(uint32_t));

	s.write((char*)&(dxt->header.dwCaps), sizeof(uint32_t));
	s.write((char*)&(dxt->header.dwCaps2), sizeof(uint32_t));
	s.write((char*)&(dxt->header.dwCaps3), sizeof(uint32_t));
	s.write((char*)&(dxt->header.dwCaps4), sizeof(uint32_t));
	s.write((char*)&(dxt->header.dwReserved2), sizeof(uint32_t));

	// Write data
	for (int i = 0; i < dxt->texelLength; i++)
	{
		s.write((char*)&(dxt->texels.get()[i].rgb565_1), sizeof(uint16_t));
		s.write((char*)&(dxt->texels.get()[i].rgb565_2), sizeof(uint16_t));
		s.write((char*)&(dxt->texels.get()[i].colours), sizeof(uint32_t));
	}
}

BCBitmap* BCDds::uncompress()
{
	unique_ptr<BCBitmap> b(new BCBitmap());
	shared_ptr<HEADER> h(new HEADER());
	shared_ptr<DIB> d(new DIB());
	b->header = h;
	b->dib = d;

	// HEADER
	h->id[0] = 'B';
	h->id[1] = 'M';
	h->size;
	h->reserve1 = 0;
	h->reserve2 = 0;
	h->offset = 54; //TODO;

	// DIB
	d->headerSize = 40;
	d->width = dxt->header.dwWidth;
	d->height = dxt->header.dwHeight;
	d->planes = 1;
	d->colourDepth = 24;
	d->compression = BI_RGB;
	d->imgSize = d->width * d->height * (d->colourDepth / sizeof(uint8_t));
	d->xResolution;
	d->yResolution;
	d->pallete = 0;
	d->important = 0;
	
	//PIXELDATA
	b->pixelData = unique_ptr<PIXELDATA>(new PIXELDATA());
	b->pixelData->width = d->width;
	b->pixelData->height = d->height;

	// Allocate memory for the pixels
	b->pixelData->pixels = new PIXEL*[b->pixelData->height];
	int count = 0;
	for (int i = 0; i < b->pixelData->height; i++)
	{
		b->pixelData->pixels[i] = new PIXEL[b->pixelData->width];
	}

	TEXEL* ts = dxt->texels.get();
	int texelWidth = d->width / TEXEL_WIDTH;
	int texelHeight = d->height / TEXEL_WIDTH;
	
	if (dxt->texelLength != texelHeight * texelWidth)
		return nullptr;

	int t_idx = 0;
	int x = 0;
	int y = 0;
	PIXEL** p2d = b->pixelData->pixels;
	for (int i = 0; i < texelHeight; i++)
	{
		for (int j = 0; j < texelWidth; j++)
		{
			PIXEL* ps = Algorithm::makePixels(ts[t_idx++]);
			int ps_idx = 0;
			for (int k = 0; k < TEXEL_WIDTH; k++)
			{
				for (int l = 0; l < TEXEL_WIDTH; l++)
				{
					x = (j * TEXEL_WIDTH) + l;
					
					// Write pixel data from bottom to top
					y = d->height - (((i * TEXEL_WIDTH) + k) + 1);
					p2d[y][x] = ps[ps_idx++];
				}
			}
		}
	}

	return b.release();
}

BCDds::BCDds()
{
}

BCDds::~BCDds()
{
}
