#pragma once

#ifdef BITMAPCOMPRESSOR_EXPORTS
#define BITMAPCOMPRESSOR_API __declspec(dllexport)
#else
#define BITMAPCOMPRESSOR_API __declspec(dllimport)
#endif

#define TEXEL_SIZE 16
#define TEXEL_WIDTH 4
#define RED_MASK 0xF800
#define GREEN_MASK 0x7E0
#define BLUE_MASK 0x1F

#include "bcdds.h"
#include "bcbitmap.h"
#include "reader.h"

#include <cstdlib>
#include <ctime>
#include <cassert>

using namespace std;

class BITMAPCOMPRESSOR_API Algorithm
{
public:
	static TEXEL makeTexel(const PIXEL* pixels)
	{
		assert(pixels != nullptr);

		srand(time(0)); // use current time as seed for random generator

		// Pick two pixels randomly
		// TODO: Fix me
		PIXEL p[2];
		p[0] = pixels[rand() % TEXEL_SIZE];
		p[1] = pixels[rand() % TEXEL_SIZE];

		TEXEL t;
		t.rgb565_0 = (p[0].red << 11) | (p[0].green << 5) | p[0].blue;
		t.rgb565_3 = (p[1].red << 11) | (p[1].green << 5) | p[1].blue;

		// TODO: Fix me
		for (int i = 0; i < TEXEL_SIZE; i++)
		{
			// TODO: Improve this logic. It sucks
			// Got through the pixels and pick the colour closest to the pixel
			uint8_t mask = rand() % 4;
			setBits(t.colours, i, mask);
		}

		return t;
	}

	static PIXEL* makePixels(const TEXEL& t)
	{
		//uint16_t red_mask = 0xF800;
		//uint16_t green_mask = 0x7E0;
		//uint16_t blue_mask = 0x1F;

		PIXEL p[4];

		p[0].red = (t.rgb565_0 & RED_MASK) >> 11;
		p[0].green = (t.rgb565_0 & GREEN_MASK) >> 5;
		p[0].blue = (t.rgb565_0 & BLUE_MASK);

		p[3].red = (t.rgb565_3 & RED_MASK) >> 11;
		p[3].green = (t.rgb565_3 & GREEN_MASK) >> 5;
		p[3].blue = (t.rgb565_3 & BLUE_MASK);

		interpolateColours(p);

		PIXEL* pixels = new PIXEL[TEXEL_SIZE];
		for (int i = 0; i < TEXEL_SIZE; i++)
		{
			pixels[i] = p[getBits(t.colours, i)];
		}

		return pixels;
	}

	static void interpolateColours(PIXEL* pArr)
	{
		// TODO: Interpolate the other 2
		pArr[1] = pArr[0];
		pArr[2] = pArr[3];
	}

	static void setBits(uint32_t& val, const int& idx, const uint8_t& mask)
	{
		//http://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit-in-c-c
		// Set the bits to the index
		int _idx = ((idx + 1) * 2) - 1; // idx is a zero-based index

		val ^= (-((mask >> 0) & 1) ^ val) & (1 << _idx- 1);
		val ^= (-((mask >> 1) & 1) ^ val) & (1 << _idx);
	}

	static int getBits(const uint32_t val, const int idx)
	{
		int ret = 0;
		int _idx = ((idx + 1) * 2) - 1; // idx is a zero-based index

		//number ^= (-x ^ number) & (1 << n);
		int x = (val >> (_idx - 1)) & 1;
		ret ^= (-x ^ ret) & (1 << 0);
		x = (val >> _idx) & 1;
		ret ^= (-x ^ ret) & (1 << 1);

		return ret;
	}
};

