#ifdef BITMAPCOMPRESSOR_EXPORTS
#define BITMAPCOMPRESSOR_API __declspec(dllexport)
#else
#define BITMAPCOMPRESSOR_API __declspec(dllimport)
#endif

#include <string>

// Forward declarations
class BCDds;

class BITMAPCOMPRESSOR_API BCBitmap
{
public:
	BCBitmap();
	~BCBitmap();

	void loadBitmap(const std::string& imgPath);
	void saveBitmap(std::string& imgPath);
	BCDds* convertToDXT1();

	/*
	// TODO
	BCDds* convertToDXT2();
	BCDds* convertToDXT3();
	BCDds* convertToDXT4();
	BCDds* convertToDXT5();
	*/

};

