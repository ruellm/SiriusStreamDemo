#h264 decoder dependency

1. Clone the source from here https://github.com/ruellm/h264_lib
2. Build the library in either [Debug] or [Release] mode
3. Place it in location 
   3rd_party/h264/lib/[os]/[mode]

   where 
	os 	win64 (windows 64)
		macos - Mac
		linux - Linux
	mode
		debug
		release
    example
	3rd_party/h264/lib/win64/debug/

Important notes:
1. QTCreator crashes when a release library is linked to a debug environment and vice versa, therefore a correct library should be linked.

2. Toolchain should match of that what will be used by QTCreator
   	