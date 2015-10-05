// console-app.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\bitmap-compressor\bcbitmap.h"
#include "..\bitmap-compressor\bcdds.h"

int main()
{
	BCBitmap b;
	b.loadBitmap("C:/Dev/bitmap-compressor/Debug/TestData/img1.bmp");

    return 0;
}

