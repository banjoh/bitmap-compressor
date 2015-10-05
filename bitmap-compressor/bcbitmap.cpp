#include "stdafx.h"
#include "bcbitmap.h"
#include "bcdds.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <stdexcept>

using namespace std;

void BCBitmap::loadBitmap(const string& imgPath)
{
	cout << "Loading " << imgPath << " to memory";
	fstream fs;
	fs.open(imgPath);

	if (fs.good())
	{
		istream_iterator<char> eos;      // end-of-stream iterator
		istream_iterator<char> it(fs);   // stdin iterator
		
		while (it != eos)
		{
			// Iterate through byte stream
			char val = *it;
			++it;
			//cout << val;
		}

		fs.close();
		loaded = true;
	}
}

PHEADER BCBitmap::extractHeader(istream_iterator<char>& it)
{
	return nullptr;
}
PDIB BCBitmap::extractDib(istream_iterator<char>& it)
{
	return nullptr;
}

void BCBitmap::saveBitmap(string& imgPath)
{
	cout << "Saving bitmap to " << imgPath;
	fstream fs;
	fs.open(imgPath);

	if (fs.good())
	{
		
	}
}

BCDds* BCBitmap::compressDXT1()
{
	return nullptr;
}

BCBitmap::BCBitmap()
{
	loaded = false;
}

BCBitmap::~BCBitmap()
{
}

