#ifdef BITMAPCOMPRESSOR_EXPORTS
#define BITMAPCOMPRESSOR_API __declspec(dllexport)
#else
#define BITMAPCOMPRESSOR_API __declspec(dllimport)
#endif

#include <string>

// Forward declarations
class BCBitmap;

enum Format { UNKNOWN, DXT1, DXT2, DXT3, DXT4, DXT5 };

class BITMAPCOMPRESSOR_API BCDds
{
public:
	BCDds();
	~BCDds();

	void loadDds(const std::string& ddsFile);
	void saveDds(std::string& ddsFile);
	BCBitmap* uncompress();

	inline Format format() { return f; };

private:
	Format f;
};
