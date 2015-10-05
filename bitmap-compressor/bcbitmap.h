#ifdef BITMAPCOMPRESSOR_EXPORTS
#define BITMAPCOMPRESSOR_API __declspec(dllexport)
#else
#define BITMAPCOMPRESSOR_API __declspec(dllimport)
#endif

#include <string>

// Forward declarations
class BCDds;

typedef struct
{
	unsigned char id[2];
} ID;

typedef struct
{
	ID id;
	uint32_t size;
	uint16_t reserve1;
	uint16_t reserve2;
	uint32_t offset;
} HEADER, *PHEADER;

typedef struct
{

} DIB, *PDIB;

typedef struct
{

} COLOUR;

class BITMAPCOMPRESSOR_API BCBitmap
{
public:
	BCBitmap();
	~BCBitmap();

	void loadBitmap(const std::string& imgPath);
	void saveBitmap(std::string& imgPath);
	inline bool bitmapLoaded() { return loaded; }
	BCDds* compressDXT1();

	/*
	// TODO
	BCDds* convertToDXT2();
	BCDds* convertToDXT3();
	BCDds* convertToDXT4();
	BCDds* convertToDXT5();
	*/

private:
	PHEADER extractHeader(istream_iterator<char>& it);
	PDIB extractDib(istream_iterator<char>& it);

private:
	bool loaded;
	PHEADER header;
	PDIB dib;
};

