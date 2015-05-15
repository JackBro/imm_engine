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
It may be difficult to use, suggest doing "Mod" on this game engine, 
because it is on low completion status.

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

Dependent .Lib Files:
---------------------
Those files should be in include_third_party\, 
notice corresponding x64/x86, Debug/Release version.
* **DirectXTK.lib**: DirectX Tool Kit
* **Effects11.lib**: Effects for Direct3D 11 (FX11)
* **lua.lib**: Lua 5.3.0
* **lua_static.lib**: Lua 5.3.0
* **XInput1_3.lib**: Its orgin name is XInput.lib, from DirectX SDK (June 2010)
* **X3DAudio.lib**: DirectX SDK (June 2010)
* **XAPOFX.lib**: DirectX SDK (June 2010)
* **DirectXTKAudioDX.lib**: DirectXTK Audio

How to Compile:
---------------
In demo folder, every subfolder inlude a signle demo.
The compile option see bat_tool\1.bat.
* **init_d3d**: Basic windows draw empty.
* **simple_secene**: The current work (see Released Demo). 
Assets of this demo are absent in the repository (Some assets can be found in Released Demo),
The non-existent assets need to be deleted from scripts and source files. 
Sorry for the inconvenience, the practicable hello world script will be developed recently.

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
	     |--scene


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