#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\bitmap-compressor\bcbitmap.h"
#include "..\bitmap-compressor\bcdds.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestInit1)
		{
			BCDds* b = new BCDds();
			Assert::IsNotNull(b);
		}

	};
}