immature engine
===============
C++  
immature engine  

Introduction:
-------------
This special-purpose engine only design for the action game [Dragon's Ingot](http://dragonsingot.com/).
All features tend to be minimized. 
The fundamental 3D rendering technique is derived from 
the [DirectX 11 book](http://www.amazon.com/dp/1936420228/) by Frank Luna.
**It is very difficult to use due to weak development scale and absence of visualization scene editor.**
**Not recommended this engine for serious using.**

Current State:
---------------
Dragon's Ingot has not released yet.
For the responsibility of a open source project, 
just guarantee the able to compile the engine demos correctly and resolving bugs.

Disadvantage List:
------------------
* The disavantage list is the bad features that will not change even in the future.
* [cl.exe](https://msdn.microsoft.com/en-us/library/9s7c9wdw.aspx) Command-Line Build.
* No visualization scene editor.
* The platform only supports Microsoft Windows. 
* Just as the name implies, the engine technology is not advanced.
* Little document support.

Compile Environment:
--------------------
Win 10  
Visual Studio 2015 (require Common Tools for Visual C++ 2015)  
Windows SDK for Windows 10  
DirectX SDK (June 2010)  

Runtime Environment:
--------------------
Win 7.1 / Win 8.x / Win 10  
DirectX End-User Runtimes (June 2010)  
Visual C++ Redistributable for Visual Studio 2015  
Update KB2670838 (Windows 7.1 situation)  
DirectX 11 capable GPU  
1024X768 minimum display resolution  
DirectX 9.0c compliant sound card  

Dependent .Lib Files for compiling:
---------------------
Those files should be in VC++ Library Directories, 
notice corresponding x64/x86, Debug/Release version.
The .lib files you can get from each corresponding project.
* **DirectXTK.lib**: [DirectX Tool Kit](https://github.com/Microsoft/DirectXTK) (April 26, 2016)
* **DirectXTKAudioDX.lib**: [DirectX Tool Kit](https://github.com/Microsoft/DirectXTK)  (April 26, 2016)
* **Effects11.lib**: [Effects for Direct3D 11](https://github.com/Microsoft/FX11) (April 26, 2016 (11.16))
* **lua.lib**: [Lua](https://www.lua.org/) 5.3.2
* **XInput1_3.lib**: Its orgin name is XInput.lib, from 
[DirectX SDK (June 2010)](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
* **X3DAudio.lib**: [DirectX SDK (June 2010)](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
* **XAPOFX.lib**: [DirectX SDK (June 2010)](https://www.microsoft.com/en-us/download/details.aspx?id=6812)

How to Compile:
---------------
In demo folder, every subfolder include a signle demo.
The compile option see bat_tool\1.bat.
* **init_d3d**: A basic window draw empty.
* **m3dtob3m**: Command line tool, convert text .m3d model file to binary .b3m file. 
Those two types both can be read by engine.
* **simple_scene**: A very simple scene draw 4 basic geometrics. 
[(Screen Capture)](https://github.com/endrollex/imm_engine/tree/master/demo/simple_scene/simple_scene.png)

Runtime Folder Tree:
--------------------
See demo\simple_scene\, represents following tree.

	 |--ast_media
	 |--ast_model
	 |--ast_shader
	 |--ast_texture
	 |--script
         |--library
	     |--scene_common
	     |--scene00
	     |--text_en


Asset Tools:
------------
* **[immature Blender export](https://github.com/endrollex/imm_blender_export)**: 
Export Blender model data to .m3d file format for immature engine.
* **m3dtob3m.exe**: 
Convert .m3d file to binary .b3m file. 
Notice .b3m file corresponding x64/x86 version.
* **CSV based scene data**: 
Unfortunately, this engine has not a scene editor let you can "drag and drop".
You must write a game object by handwriting, this limits the complexity of a scene.

Copyright and License:
----------------------
* Copyright 2015-2016 Huang Yiting (http://endrollex.com)
* immature engine is distributed under the the terms of the BSD license
* Not include the files in folders of suffix "_3rd_party".
