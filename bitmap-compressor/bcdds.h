#pragma once

#ifdef BITMAPCOMPRESSOR_EXPORTS
#define BITMAPCOMPRESSOR_API __declspec(dllexport)
#else
#define BITMAPCOMPRESSOR_API __declspec(dllimport)
#endif

#include <string>
#include <memory>
#include <iterator>

#define DDS_MAGIC				542327876 // "DDS "
#define DDS_HEADER_SIZE			124
#define DDS_PIXELFORMAT_SIZE	32
#define DDSD_CAPS 				0x1
#define DDSD_PIXELFORMAT		0x1000
#define DDSCAPS_TEXTURE			0x1000
#define DDPF_FOURCC				0x4
#define DXT1_FOURCC				'1TXD'

class BCBitmap;

using namespace std;

typedef struct {
	uint32_t	dwSize;
	uint32_t	dwFlags;
	uint32_t	dwFourCC;
	uint32_t	dwRGBBitCount;
	uint32_t	dwRBitMask;
	uint32_t	dwGBitMask;
	uint32_t	dwBBitMask;
	uint32_t	dwABitMask;
} DDS_PIXELFORMAT;

typedef struct {
	uint32_t           dwSize;
	uint32_t           dwFlags;
	uint32_t           dwHeight;
	uint32_t           dwWidth;
	uint32_t           dwPitchOrLinearSize;
	uint32_t           dwDepth;
	uint32_t           dwMipMapCount;
	uint32_t           dwReserved1[11];
	DDS_PIXELFORMAT	   ddspf;
	uint32_t           dwCaps;
	uint32_t           dwCaps2;
	uint32_t           dwCaps3;
	uint32_t           dwCaps4;
	uint32_t           dwReserved2;
} DDS_HEADER;

typedef struct
{
	uint16_t rgb565_1 = 0x0;
	uint16_t rgb565_2 = 0x0;
	uint32_t colours = 0x0;
} TEXEL;

typedef struct
{
	uint32_t				dwMagic;
	DDS_HEADER				header;
	unique_ptr<TEXEL>		texels;
	int						texelLength;
} DXT;

class BITMAPCOMPRESSOR_API BCDds
{
	friend class UnitTest1;
	friend class BCBitmap;
public:
	BCDds();
	~BCDds();

	void loadDds(const string& ddsFile);
	bool saveDds(const string& ddsFile);
	BCBitmap* uncompress();

	inline bool loaded() { return _loaded; }

private:
	bool readHeader(istream_iterator<uint8_t>& it, DDS_HEADER& h);
	bool readData(istream& is);
	void writeData(ostream& s);

private:
	shared_ptr<DXT> dxt;
	volatile bool _loaded = false;
};
