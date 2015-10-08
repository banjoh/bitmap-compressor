// uncompress-dds.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\bitmap-compressor\bcbitmap.h"
#include "..\bitmap-compressor\bcdds.h";
#include <iostream>
#include <cstdlib>
#include <memory>

int main( int argc, char *argv[], char *envp[] )
{
	if (argc < 2)
	{
		cerr << "USAGE: uncompress-dds.exe <absolute_path_dds> <absolute_path_bmp>" << endl;
		return 1;
	}	

	try
	{
		cout << endl;
		unique_ptr<BCDds> d(new BCDds());
		d->loadDds(argv[1]);
		if (d->loaded())
		{
			unique_ptr<BCBitmap> b(d->uncompress());
			b->saveBitmap(argv[2]);
		}
	}
	catch (std::exception& ex)
	{
		cerr << ex.what() << endl;
	}
	
    return 0;
}

