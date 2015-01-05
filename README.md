#SelfLoader

##About
SelfLoader is a proof of concept of loading APIs on Windows from code without any imports, not even *ntdll.dll* or *kernel32.dll*. That is, code is completely self-hosted.
When following all the rules (like you usually should), the bare minimum you need in order to import an external dependency is *LoadLibraryW*, *FreeLibrary* and *GetProcAddress*
from *kernel32.dll*. SelfLoader can find the function pointers to these functions by reading the **Process Environment Block (PEB)**, without depending on any APIs like *kernel32.dll*
or the *C runtime*.

##Overview
The Visual Studio project **SelfLoader** shows how to bootstrap *LoadLibraryW*, *GetProcAddress* and *FreeLibrary*, and uses those to get the function pointer to MessageBoxW from *user32.dll*.
The other project **EnumPEBModules**, displays a bunch of information about the modules found via the PEB.
For more information on the technique used, look at these links:

* [Win32 Hacks: Loading API functions from a process' Process Environment Block (PEB) Part 1 of 2 (blogger.com)](http://pimpmycode.blogspot.no/2015/01/win32-hacks-loading-api-functions-from.html)
* [Win32 Hacks: Loading API functions from a process' Process Environment Block (PEB) Part 2 of 2 (blogger.com)](http://pimpmycode.blogspot.no/2015/01/win32-hacks-loading-api-functions-from_4.html)
* [Win32 Thread Information Block (Wikipedia)](https://en.wikipedia.org/wiki/Win32_Thread_Information_Block)
* [Win32 Process Environment Block (Wikipedia)](https://en.wikipedia.org/wiki/Process_Environment_Block)
* [Win32 PEB Loader Data (shitwefoundout.com)](http://shitwefoundout.com/wiki/Win32_PEB_Loader_data)
* https://code.google.com/p/corkami/wiki/PE


##Building
Currently the repository only contains Visual Studio Express 2013 solution files. CMakeList.txt for CMake is coming soon. Building the Visual Studio projects requires
[**NASM**](http://www.nasm.us/) to be in your path. [Link to the latest **NASM** Windows Installer](http://www.nasm.us/pub/nasm/releasebuilds/2.11.06/win32/)

##Things to do
Some effort used to find the correct TIB and PEB structure layouts for all the different versions of Windows would be nice, and could possibly change this code from being a curiosity into
something that is half-way usable for some things.