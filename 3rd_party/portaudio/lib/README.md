# PortAudio audio capture and playback library

1. Download the source from http://files.portaudio.com/download.html
   This demo is using the version pa_stable_v190600_20161030.
   Disable DirectSound  and ASIO support during build.
   
2. Build the library in either [Debug] or [Release] mode
3. Place it in location
   3rd_party/portaudio/lib/[os]/[mode] <br/>
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
   3rd_party/portaudio/lib/win64/debug/

Important notes:
1. QTCreator crashes when a release library is linked to a debug environment and vice versa, therefore a correct library should be linked.

2. Toolchain should match of that what will be used by QTCreator
   	