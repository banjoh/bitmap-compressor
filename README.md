# Bitmap Compressor
Bitmap compressor implementation using S3 texture compression algotrithms. So far only DXT1 compression algorithm is implemented. Only bitmaps having widths and heights divisible by 4 (width % 4 == 0 and height % 4 == 0) are supported.

### Compiling and running
* Compile using Visual Studio 2015 (any distribution should work)
* Test the dll using two console applications like below
```
compress-bmp.exe <absolute_path_bmp> <absolute_path_dds>

uncompress-dds.exe <absolute_path_dds> <absolute_path_bmp>
```

After compiling, you will find some test .bmp and .dds files in the < ProjectRoot >/< Configuration >/TestData folder
