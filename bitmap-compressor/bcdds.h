#ifdef BITMAPCOMPRESSOR_EXPORTS
#define BITMAPCOMPRESSOR_API __declspec(dllexport)
#else
#define BITMAPCOMPRESSOR_API __declspec(dllimport)
#endif

#include <string>
#include <memory>
#include <iterator>

// Forward declarations
class BCBitmap;

using namespace std;

enum Format { UNKNOWN, DXT1, DXT2, DXT3, DXT4, DXT5 };

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
	uint32_t				dwMagic;
	DDS_HEADER				header;
	//DDS_HEADER_DXT10		header10;
	unique_ptr<uint8_t>		bdata;
	int						dataLength;
} DXT;

class BITMAPCOMPRESSOR_API BCDds
{
	friend class UnitTest1;
public:
	BCDds();
	~BCDds();

	void loadDds(const string& ddsFile);
	void saveDds(const string& ddsFile);
	BCBitmap* uncompress();

	inline Format format() { return f; };
	inline bool ddsLoaded() { return loaded; }

private:
	bool readHeader(istream_iterator<uint8_t>& it, DDS_HEADER& h);
	bool readData(istream& is);
	void writeData(ostream& s);

private:
	Format f;
	shared_ptr<DXT> dxt;
	volatile bool loaded = false;
};
