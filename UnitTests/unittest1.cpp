#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\bitmap-compressor\bcbitmap.h"
#include "..\bitmap-compressor\bcdds.h"
#include "..\bitmap-compressor\reader.h"

#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

TEST_CLASS(UnitTest1)
{
public:

	TEST_METHOD(TestLoadBitmap1)
	{
		BCBitmap b;
		b.loadBitmap("C:/Dev/bitmap-compressor/Debug/TestData/img1.bmp");
		Assert::IsTrue(b.bitmapLoaded());
	}

	TEST_METHOD(TestSaveBitmap1)
	{
		BCBitmap b;
		b.loadBitmap("C:/Dev/bitmap-compressor/Debug/TestData/img1.bmp");
		Assert::IsTrue(b.bitmapLoaded());
		b.saveBitmap("C:/Dev/bitmap-compressor/Debug/TestData/img2.bmp");
		// How do we assert?
	}

	TEST_METHOD(TestLoadDds1)
	{
		BCDds d;
		d.loadDds("C:/Dev/bitmap-compressor/Debug/TestData/img1.dds");
		Assert::IsTrue(d.ddsLoaded());
	}

	TEST_METHOD(TestSaveDds1)
	{
		BCDds d;
		d.loadDds("C:/Dev/bitmap-compressor/Debug/TestData/img1.dds");
		Assert::IsTrue(d.ddsLoaded());
		d.saveDds("C:/Dev/bitmap-compressor/Debug/TestData/img2.dds");
		// How do we assert?
	}

	TEST_METHOD(TestToChar)
	{
		BCBitmap b;
		uint8_t arr[4];
		stringstream s;
		s << "1234";
		istream_iterator<uint8_t> it(s);
		Assert::IsTrue(Reader::readNext(it, sizeof(arr), arr));
		char a[4];
		Reader::copy(a, arr, 4);
		Assert::AreEqual(a[0], '1');
		Assert::AreEqual(a[1], '2');
		Assert::AreEqual(a[2], '3');
		Assert::AreEqual(a[3], '4');
	}

	TEST_METHOD(TestToInt1)
	{
		BCBitmap b;
		uint32_t num;
		uint8_t v[4];
		v[0] = 0xFF;
		v[1] = 0xFF;
		v[2] = 0xFF;
		v[3] = 0xFF;
		stringstream s;
		for (size_t i = 0; i < sizeof(v); i++)
		{
			s << v[i];
		}
		s.flush();
		istream_iterator<uint8_t> it(s);
		Assert::IsTrue(Reader::readNext(it, num));
		uint32_t val = 4294967295;
		Assert::AreEqual(num, val);
	}

	TEST_METHOD(TestToInt2)
	{
		BCBitmap b;
		uint16_t num;
		uint8_t v[4];
		v[0] = 0xFF;
		v[1] = 0xFF;
		stringstream s;
		for (size_t i = 0; i < sizeof(v); i++)
		{
			s << v[i];
		}
		s.flush();
		istream_iterator<uint8_t> it(s);
		Assert::IsTrue(Reader::readNext(it, num));
		uint16_t val = 65535;
		Assert::IsTrue(num == val);
	}

};