Disclaimer
==========
The code and tools have not been maintained in a while so I would be surprised if it compiles and runs as expected.


Prerequisites
=============
- Microsoft Visual Studio 2013 
- Tundra - https://github.com/deplinenoise/tundra/releases
- DirectX SDK - http://www.microsoft.com/en-us/download/details.aspx?id=6812
- Python 3 - https://www.python.org/downloads/


Setup
=====
First you need to install the dependencies required to compile the project. These can be found here: https://www.dropbox.com/s/wha3akq0kkqoi6r/External.zip - Extract the External folder to the root of the project.


Visual Studio
=============
With the command-line in the root folder of the project; write 'tundra2 -g msvc120' to generate project files for Visual Studio 2013.
You can then open 't2-output/Sandbox.sln' to get started.

The minimal way to get started if you only want to compile the project is to run 'python build.py release'. This will result in a fully built release in the 'Release' folder.

Game Content
============
To build the game content located in the Content folder you will need to run the Asset Builder. This will compile all assets to their runtime formats.
There's tools available for performing this action. Just run 'python build.py build_assets' from the command-line.
The newly built assets will be located in Binaries/Content, and they will automatically be loaded when running the engine.
