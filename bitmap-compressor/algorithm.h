#pragma once

#ifdef BITMAPCOMPRESSOR_EXPORTS
#define BITMAPCOMPRESSOR_API __declspec(dllexport)
#else
#define BITMAPCOMPRESSOR_API __declspec(dllimport)
#endif

#define TEXEL_SIZE 16
#define TEXEL_WIDTH 4
#define COLOURS_SIZE 4
#define RED_MASK 0xF800
#define GREEN_MASK 0x7E0
#define BLUE_MASK 0x1F

#include "bcdds.h"
#include "bcbitmap.h"
#include "reader.h"

#include <cassert>
#include <cmath>

using namespace std;

class BITMAPCOMPRESSOR_API Algorithm
{
public:
	/*
	Create a TEXEL from 16 of pixels (4x4 square from pixel array)
	*/
	static TEXEL makeTexel(const PIXEL* pixels)
	{
		assert(pixels != nullptr);

		// Pick pixels closest/furthest to the black & white reference pixels
		// from the texel
		PIXEL colours[4];
		PIXEL ref;
		ref.red = ref.green = ref.blue = 0;
		colours[0] = pixels[pickPixel(pixels, TEXEL_SIZE, ref)];
		ref.red = ref.green = ref.blue = 255;
		colours[1] = pixels[pickPixel(pixels, TEXEL_SIZE, ref)];

		// Interpolate the rest of the colours
		interpolate(colours[0].red, colours[1].red, colours[2].red, colours[3].red);
		interpolate(colours[0].green, colours[1].green, colours[2].green, colours[3].green);
		interpolate(colours[0].blue, colours[1].blue, colours[2].blue, colours[3].blue);
		
		TEXEL texel;
		texel.rgb565_1 = (colours[0].red << 11) | (colours[0].green << 5) | colours[0].blue;
		texel.rgb565_2 = (colours[1].red << 11) | (colours[1].green << 5) | colours[1].blue;

		for (int i = 0; i < TEXEL_SIZE; i++)
		{
			// Iterate through the colours and pick the closest to this one
			uint8_t mask = (uint8_t)pickPixel(colours, COLOURS_SIZE, pixels[i]);
			setBits(texel.colours, i, mask);
		}

		return texel;
	}

	/*
	Iterate through the pixels[] and find the pixel closest in colour (distannce
	in 3D space)
	*/
	static int pickPixel(const PIXEL* pixels, const int size, const PIXEL& ref)
	{
		int idx = 0;
		int64_t shortestDist = abs(255*255 + 255*255 + 255*255);
		for (int i = 0; i < size; i++)
		{
			PIXEL p = pixels[i];
			// Calculate shortest distance to the reference pixel
			// Sqrt((x1-x2)^2+(y1-y1)^2+(z1-z2)^2). Sqrt is expensiver.
			// No need to take it since we are just comparing values

			int64_t d = abs(pow((ref.red - p.red), 2) + pow((ref.green - p.green), 2) + pow((ref.blue - p.blue), 2));
			if (d <= shortestDist)
			{
				idx = i;
				shortestDist = d;
			}
		}
		return idx;
	}

	/*
	Create a PIXEL[16] array from a texel which will be part of the bitmap data 
	*/
	static PIXEL* makePixels(const TEXEL& t)
	{
		PIXEL c[4];

		c[0].red = (t.rgb565_1 & RED_MASK) >> 11;
		c[0].green = (t.rgb565_1 & GREEN_MASK) >> 5;
		c[0].blue = (t.rgb565_1 & BLUE_MASK);

		c[1].red = (t.rgb565_2 & RED_MASK) >> 11;
		c[1].green = (t.rgb565_2 & GREEN_MASK) >> 5;
		c[1].blue = (t.rgb565_2 & BLUE_MASK);

		interpolate(c[0].red, c[1].red, c[2].red, c[3].red);
		interpolate(c[0].green, c[1].green, c[2].green, c[3].green);
		interpolate(c[0].blue, c[1].blue, c[2].blue, c[3].blue);

		PIXEL* pixels = new PIXEL[TEXEL_SIZE];
		for (int i = 0; i < TEXEL_SIZE; i++)
		{
			pixels[i] = c[getBits(t.colours, i)];
		}

		return pixels;
	}

	/*
	Interpolate c2 & c3 colours from c1 & c2
	*/
	static void interpolate(const uint8_t& c0, const uint8_t& c1, uint8_t& c2, uint8_t& c3)
	{
		// https://www.opengl.org/wiki/S3_Texture_Compression
		if (c0 > c1)
		{
			c2 = (2 * c0 + c1) / 3; //(2*color0 + color1) / 3
			c3 = (c0 + 2 * c1) / 3; //(color0 + 2 * color1) / 3
		}
		else
		{
			c2 = (c0 + c1) / 2; //(color0 + color1) / 2
			c3 = 0;				//Black
		}
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

