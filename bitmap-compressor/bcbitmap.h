#pragma once

#ifdef BITMAPCOMPRESSOR_EXPORTS
#define BITMAPCOMPRESSOR_API __declspec(dllexport)
#else
#define BITMAPCOMPRESSOR_API __declspec(dllimport)
#endif

#define BI_RGB	0

#include <string>
#include <iterator>
#include <memory>

using namespace std;

// Forward declarations
class BCDds;

typedef struct HEADER
{
	char id[2];
	uint32_t size;
	uint16_t reserve1;
	uint16_t reserve2;
	uint32_t offset;
} HEADER;

typedef struct DIB
{
	uint32_t headerSize;
	int32_t width;
	int32_t height;
	uint16_t planes;
	uint16_t colourDepth;
	uint32_t compression;
	uint32_t imgSize;
	int32_t xResolution;
	int32_t yResolution;
	uint32_t pallete;
	uint32_t important;
} DIB;

typedef struct
{
	uint8_t red = 0x0;
	uint8_t green = 0x0;
	uint8_t blue = 0x0;
} PIXEL;

typedef struct PIXELDATA
{
	~PIXELDATA()
	{
		if (pixels != nullptr)
		{
			for (int i = 0; i < height; i++)
			{
				delete[] pixels[i];
				pixels[i] = nullptr;
			}
			delete[] pixels;
			pixels = nullptr;
		}
	};
	// 2D array of pixel data
	PIXEL** pixels = nullptr;
	int32_t width;
	int32_t height;
} PIXELDATA;

class BITMAPCOMPRESSOR_API BCBitmap
{
	friend class UnitTest1;
	friend class BCDds;
public:
	BCBitmap();
	~BCBitmap();

	void loadBitmap(const string& imgPath);
	bool saveBitmap(const string& imgPath);
	inline bool loaded() { return _loaded; }
	BCDds* compressDXT1();
	
	// TODO
	BCDds* compressDXT2();
	BCDds* compressDXT3();
	BCDds* compressDXT4();
	BCDds* compressDXT5();

private:
	// Read bitmap data
	bool readHeader(istream_iterator<uint8_t>& it);
	bool readDib(istream_iterator<uint8_t>& it);
	bool readPixelArray(istream_iterator<uint8_t>& it);

	// Write bitmap data
	void writeHeader(ostream& s);
	void writeDib(ostream& s);
	void writePixelArray(ostream& s);

private:
	volatile bool _loaded = false;
	shared_ptr<HEADER> header;
	shared_ptr<DIB> dib;
	unique_ptr<PIXELDATA> pixelData;
};

