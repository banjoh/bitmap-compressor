#ifdef BITMAPCOMPRESSOR_EXPORTS
#define BITMAPCOMPRESSOR_API __declspec(dllexport)
#else
#define BITMAPCOMPRESSOR_API __declspec(dllimport)
#endif

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

} COLOUR;

class BITMAPCOMPRESSOR_API BCBitmap
{
	friend class UnitTest1;
public:
	BCBitmap();
	~BCBitmap();

	void loadBitmap(const string& imgPath);
	void saveBitmap(const string& imgPath);
	inline bool bitmapLoaded() { return loaded; }
	BCDds* compressDXT1();
	
	// TODO
	BCDds* compressDXT2();
	BCDds* compressDXT3();
	BCDds* compressDXT4();
	BCDds* compressDXT5();

	friend std::ostream& operator<<(std::ostream& out, const BCBitmap& b);

private:
	// Read bitmap data
	bool readHeader(istream_iterator<uint8_t>& it);
	bool readDib(istream_iterator<uint8_t>& it);
	bool readPixelArray(istream_iterator<uint8_t>& it);
	bool readNext(istream_iterator<uint8_t>& it, int size, uint8_t* out);
	bool readNext(istream_iterator<uint8_t>& it, uint32_t& out);
	bool readNext(istream_iterator<uint8_t>& it, uint16_t& out);
	bool readNext(istream_iterator<uint8_t>& it, int32_t& out);
	bool readNext(istream_iterator<uint8_t>& it, int16_t& out);
	bool readNext(istream_iterator<uint8_t>& it, uint8_t& out);

	// Write bitmap data
	void writeHeader(ostream& s);
	void writeDib(ostream& s);
	void writePixelArray(ostream& s);

	// Helpers
	void copy(char* dst, const uint8_t* src, int size);

private:
	volatile bool loaded = false;
	shared_ptr<HEADER> header;
	shared_ptr<DIB> dib;
	shared_ptr<uint8_t> pixelArray;
	int currPos = 0;
	int fileLength = 0;
};

