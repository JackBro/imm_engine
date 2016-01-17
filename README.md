immature engine
===============
C++  
immature engine  

Introduction:
-------------
The engine of the action game [Dragon's Ingot](http://dragonsingot.com/). All features tend to be minimized. 
The fundamental 3D rendering technique is derived from 
the [DirectX 11 book](http://www.amazon.com/Introduction-3D-Game-Programming-DirectX/dp/1936420228/) by Frank Luna.
**It is very difficult to use due to weak development scale and absence of visualization scene editor.**
**Not recommended this engine for serious use.**

Current State:
---------------
The development task is focusing on making the game Dragon's Ingot.
Just guarantee the able to compile the engine demo correctly.

Disadvantage List:
------------------
* The disavantage list is the bad features that will not change even in the future.
* No visualization scene editor.
* Some game date is directly mixed with source code, such as UI and game logic.
* The platform only supports Microsoft Windows. 
* Horrible API design, evil magic code style.
* Very high coupling class structure.
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

Dependent .Lib Files:
---------------------
Those files should be in VC++ Library Directories, 
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
In demo folder, every subfolder include a signle demo.
The compile option see bat_tool\1.bat.
* **init_d3d**: A basic window draw empty.
* **m3dtob3m**: Asset tool, convert .m3d file to binary .b3m file. Those two types both can be read by engine.
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

Released Demo:
--------------
* **x64**: http://pan.baidu.com/s/1c0yFrCc
* **x86**: http://pan.baidu.com/s/1mg5k7XY

Copyright and License:
----------------------
* Copyright 2015 Huang Yiting (http://endrollex.com)
* immature engine is distributed under the the terms of the BSD license
* Not include the files in folders of suffix "_3rd_party".