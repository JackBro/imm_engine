immature engine
===============
C++  
immature engine
Quick-and-dirty

Introduction:
-------------
A small game engine. All features tend to be minimized. 
The engine platform only supports Microsoft Windows. 
The fundamental 3D rendering technique is derived from the textbook 
Introduction to 3D Game Programming with DirectX 11 by Frank Luna. 
This engine development methodology is Quick-and-dirty, no traditional API design, casual style. 
It may be difficult to use, because the engine is on low completion status.

In Development:
---------------
The project is in development, there is less document and feature now.
The framework of the engine is instability, will be changed frequently.
Just guarantee the able to compile the project correctly.

Compile Environment:
--------------------
Win 7.1 / Win 8.x  
Visual Studio 2013  
Windows SDK for Windows 8.1  
DirectX SDK (June 2010)

Runtime Environment:
--------------------
Win 7.1 / Win 8.x / Win 10  
DirectX End-User Runtimes (June 2010)  
Visual C++ Redistributable Packages for Visual Studio 2013  
Update KB2670838 (Windows 7.1 situation)  
DirectX 11 capable GPU  
1024X768 minimum display resolution  
DirectX 9.0c compliant sound card  
(Win 10 only supports Release Build with VS2013, I have not VS2015 yet.)  

Dependent .Lib Files:
---------------------
Those files should be in include_third_party\, 
notice corresponding x64/x86, Debug/Release version.
* **DirectXTK.lib**: DirectX Tool Kit
* **Effects11.lib**: Effects for Direct3D 11
* **lua.lib**: Lua 5.3.0
* **XInput1_3.lib**: Its orgin name is XInput.lib, from DirectX SDK (June 2010)
* **X3DAudio.lib**: DirectX SDK (June 2010)
* **XAPOFX.lib**: DirectX SDK (June 2010)
* **DirectXTKAudioDX.lib**: DirectXTK Audio

How to Compile:
---------------
In demo folder, every subfolder inlude a signle demo.
The compile option see bat_tool\1.bat.
* **init_d3d**: A basic window draw empty.
* **simple_secene**: A very simple scene draw 4 basic geometrics. 
This program is almost the same as the Released Demo (github's is updated, but not include assets).

Runtime Folder Tree:
--------------------
See minimum_root\, represents following tree.

	 |--ass_media
	 |--ass_model
	 |--ass_shader
	 |--ass_texture
	 |--misc
	 |   |--input
	 |   |--output
	 |--script
	     |--cmd_util
	     |--library
	     |--scene_common
	     |--scene00
	     |--text_en


Asset Tools:
------------
* **immature Blender export**: 
Export Blender model data to .m3d file format for immature engine. 
https://github.com/endrollex/imm_blender_export
* **immature engine console util b3m tool**: 
Convert .m3d file to binary .b3m file, it can be found in game input mode, 
"Alt+Delete" then type "help" for more info. 
Notice .b3m file corresponding x64/x86 version.

Released Demo:
--------------
* **x64**: http://pan.baidu.com/s/1c0yFrCc
* **x86**: http://pan.baidu.com/s/1mg5k7XY

Copyright and License:
----------------------
* Copyright 2015 Huang Yiting (http://endrollex.com)
* immature engine is distributed under the the terms of the BSD license
* Not include the files in folders of suffix "_third_party".