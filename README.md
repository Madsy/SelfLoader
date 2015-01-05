#SelfLoader

##About
SelfLoader is a proof of concept of loading APIs on Windows from code without any imports, not even *ntdll.dll* or *kernel32.dll*. That is, code is completely self-hosted.
When following all the rules (like you usually should), the bare minimum you need in order to import an external dependency is *LoadLibraryW*, *FreeLibrary* and *GetProcAddress*
from *kernel32.dll*. SelfLoader can find the function pointers to these functions by reading the **Process Environment Block (PEB)**, without depending on any APIs like *kernel32.dll*
or the *C runtime*.

##Overview
The Visual Studio project **SelfLoader** shows how to bootstrap *LoadLibraryW*, *GetProcAddress* and *FreeLibrary*, and uses those to get the function pointer to MessageBoxW from *user32.dll*.
The other project **EnumPEBModules**, displays a bunch of information about the modules found via the PEB.

##Building
Currently the repository only contains Visual Studio Express 2013 solution files. CMakeList.txt for CMake is coming soon. Building the Visual Studio projects requires
[**NASM**](http://www.nasm.us/) to be in your path. [Link to the latest **NASM** Windows Installer](http://www.nasm.us/pub/nasm/releasebuilds/2.11.06/win32/)

##Things to do
Some effort used to find the correct TIB and PEB structure layouts for all the different versions of Windows would be nice, and could possibly change this code from being a curiosity into
something that is half-way usable for some things.