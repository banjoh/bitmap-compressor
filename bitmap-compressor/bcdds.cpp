#include "stdafx.h"
#include "bcdds.h"
#include "bcbitmap.h"
#include "reader.h"

#include <fstream>
#include <iostream>

#define DDS_MAGIC 542327876 // "DDS "
#define DDS_HEADER_SIZE 124
#define DDS_PIXELFORMAT_SIZE 32

void BCDds::loadDds(const string& ddsFile)
{
	loaded = false;
	cout << "Loading " << ddsFile << " to memory";
	ifstream fs;
	try
	{
		fs.open(ddsFile, ios::binary);
		if (fs.good())
		{
			fs.unsetf(ios::skipws);
			loaded = readData(fs);
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
		cerr << "Failed to load dds file" << endl;
	}
}

bool BCDds::readHeader(istream_iterator<uint8_t>& it, DDS_HEADER& h)
{
	if (!Reader::readNext(it, h.dwSize)) return false;
	if (!Reader::readNext(it, h.dwFlags)) return false;
	if (!Reader::readNext(it, h.dwHeight)) return false;
	if (!Reader::readNext(it, h.dwWidth)) return false;
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
	d->dataLength = length - DDS_HEADER_SIZE - sizeof(d->dwMagic);
	d->bdata = unique_ptr<uint8_t>(new uint8_t[d->dataLength]);

	if (!Reader::readNext(it, d->dataLength, d->bdata.get())) return false;

	dxt = d;
	return true;
}

void BCDds::saveDds(const string& ddsFile)
{
	cout << "Saving bitmap to " << ddsFile;
	ofstream fs;
	fs.open(ddsFile, ios::trunc | ios::binary);
	fs.unsetf(ios::skipws);

	if (fs.good() && dxt != nullptr)
	{
		writeData(fs);
		fs.close();
	}
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
	for (int i = 0; i < dxt->dataLength; i++)
	{
		s.write((char*)&(dxt->bdata.get()[i]), sizeof(uint8_t));
	}
}

BCBitmap* BCDds::uncompress()
{
	return nullptr;
}

BCDds::BCDds()
{
}

BCDds::~BCDds()
{
}
