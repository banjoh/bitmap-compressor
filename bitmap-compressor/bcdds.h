#ifdef BITMAPCOMPRESSOR_EXPORTS
#define BITMAPCOMPRESSOR_API __declspec(dllexport)
#else
#define BITMAPCOMPRESSOR_API __declspec(dllimport)
#endif

#include <string>

// Forward declarations
class BCBitmap;

class BITMAPCOMPRESSOR_API BCDds
{
public:
	BCDds();
	~BCDds();

	void loadDds(const std::string& ddsFile);
	void saveDds(std::string& ddsFile);
	BCBitmap* convertToBitmap();

private:

};
