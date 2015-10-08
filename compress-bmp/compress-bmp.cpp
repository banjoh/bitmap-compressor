// compress-bmp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\bitmap-compressor\bcbitmap.h"
#include "..\bitmap-compressor\bcdds.h";
#include <iostream>
#include <cstdlib>
#include <memory>
 
int main(int argc, char *argv[], char *envp[])
{
	if (argc < 2)
	{
		cerr << "USAGE: compress-bmp.exe <absolute_path_bmp> <absolute_path_dds>" << endl;
		return 1;
	}

	try
	{
		cout << endl;
		unique_ptr<BCBitmap> b(new BCBitmap());
		b->loadBitmap(argv[1]);
		if (b->loaded())
		{
			unique_ptr<BCDds> d(b->compressDXT1());
			d->saveDds(argv[2]);
		}
		
	}
	catch (std::exception& ex)
	{
		cerr << ex.what() << endl;
	}

	return 0;
}

