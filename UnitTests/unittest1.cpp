#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\bitmap-compressor\bcbitmap.h"
#include "..\bitmap-compressor\bcdds.h"
#include "..\bitmap-compressor\reader.h"
#include "..\bitmap-compressor\algorithm.h"

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

	TEST_METHOD(TestSetBit1)
	{
		uint32_t val = 0x0;
		int idx = 0;
		uint8_t mask = 0x1;
		Algorithm::setBits(val, idx, mask);
		uint32_t expected = 0x1;
		Assert::AreEqual(val, expected);
	}

	TEST_METHOD(TestSetBit2)
	{
		uint32_t val = 0x0;
		int idx = 11;
		uint8_t mask = 0x3;
		Algorithm::setBits(val, idx, mask);
		uint32_t expected = 0xC00000;
		Assert::AreEqual(val, expected);
	}

	TEST_METHOD(TestSetBit3)
	{
		uint32_t val = 0x0;
		Algorithm::setBits(val, 0, 0x1);
		Algorithm::setBits(val, 5, 0x3);
		Algorithm::setBits(val, 15, 0x2);
		uint32_t expected = 0x80000C01;
		Assert::AreEqual(val, expected);
	}

	TEST_METHOD(TestGetBit1)
	{
		uint32_t val = 0x3;
		Assert::AreEqual(Algorithm::getBits(val, 0), 3);
	}

	TEST_METHOD(TestGetBit2)
	{
		uint32_t val = 0x22030C3;
		Assert::AreEqual(Algorithm::getBits(val, 0), 3);
		Assert::AreEqual(Algorithm::getBits(val, 2), 0);
		Assert::AreEqual(Algorithm::getBits(val, 3), 3);
		Assert::AreEqual(Algorithm::getBits(val, 6), 3);
		Assert::AreEqual(Algorithm::getBits(val, 9), 0);
		Assert::AreEqual(Algorithm::getBits(val, 10), 2);
		Assert::AreEqual(Algorithm::getBits(val, 12), 2);
	}

	TEST_METHOD(TestCompress1)
	{
		BCBitmap b;
		b.loadBitmap("C:/Dev/bitmap-compressor/Debug/TestData/img1.bmp");
		Assert::IsTrue(b.bitmapLoaded());
		BCDds* dds = b.compressDXT1();
		Assert::IsNotNull(dds);
		dds->saveDds("C:/Dev/bitmap-compressor/Debug/TestData/img3.dds");
	}
};