#Speex audio compression dependencies

1. Clone the source from here https://github.com/ruellm/speex-1.2rc1
2. Build the library in either [Debug] or [Release] mode
3. Place it in location
   3rd_party/speex-1.2rc1/lib/[os]/[mode] <br/>
   Where
   <br/>
   os <br/>
   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;win64 (windows 64) <br/>
   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;macos - Mac <br/>
   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;linux - Linux <br/>
   <br/>
   mode <br/>
   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; debug <br/>
   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; release 
   <br/>    
   example <br/>
   3rd_party/speex-1.2rc1/lib/win64/debug/

Important notes:
1. QTCreator crashes when a release library is linked to a debug environment and vice versa, therefore a correct library should be linked.

2. Toolchain should match of that what will be used by QTCreator
   	