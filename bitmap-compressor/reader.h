#ifdef BITMAPCOMPRESSOR_EXPORTS
#define BITMAPCOMPRESSOR_API __declspec(dllexport)
#else
#define BITMAPCOMPRESSOR_API __declspec(dllimport)
#endif

#include <string>
#include <iterator>
#include <memory>

using namespace std;

class BITMAPCOMPRESSOR_API Reader
{
	friend class UnitTest1;
public:

	// Read data from stream iterator
	static bool readNext(istream_iterator<uint8_t>& it, int size, uint8_t* out)
	{
		istream_iterator<uint8_t> eos;      // end-of-stream iterator
		for (int i = 0; i < size; i++)
		{
			if (it != eos)
			{
				out[i] = *it;
				++it;
			}
			else
			{
				// Reached end of stream
				return false;
			}
		}

		return true;
	}

	static bool readNext(istream_iterator<uint8_t>& it, int size, uint32_t* out)
	{
		for (int i = 0; i < size; i++)
		{
			if (!readNext(it, out[i])) return false;
		}

		return true;
	}

	static bool readNext(istream_iterator<uint8_t>& it, uint32_t& out)
	{
		uint8_t arr[4];
		if (!readNext(it, sizeof(arr), arr)) return false;

		uint32_t temp = *(uint32_t *)arr;
		out = *(uint32_t *)arr;

		return true;
	}

	static bool readNext(istream_iterator<uint8_t>& it, uint8_t& out)
	{
		uint8_t arr[1];
		if (!readNext(it, sizeof(arr), arr)) return false;

		out = *(uint8_t *)arr;

		return true;
	}

	static bool readNext(istream_iterator<uint8_t>& it, uint16_t& out)
	{
		uint8_t arr[2];
		if (!readNext(it, sizeof(arr), arr)) return false;

		out = *(uint16_t *)arr;

		return true;
	}

	static bool readNext(istream_iterator<uint8_t>& it, int32_t& out)
	{
		uint32_t a;
		if (!readNext(it, a)) return false;

		out = (int32_t)a;
		return true;
	}
	static bool readNext(istream_iterator<uint8_t>& it, int16_t& out)
	{
		uint16_t a;
		if (!readNext(it, a)) return false;

		out = (int16_t)a;
		return true;
	}

	// Helpers
	static void copy(char* dst, const uint8_t* src, int size)
	{
		for (int i = 0; i < size; i++)
		{
			dst[i] = (char)src[i];
		}
	}
};

