Prerequisites
=============
- Microsoft Visual Studio 2013 
- Tundra - https://github.com/deplinenoise/tundra/releases
- DirectX SDK - http://www.microsoft.com/en-us/download/details.aspx?id=6812
- Python 2.7 - https://www.python.org/downloads/


Setup
=============
First you need to install the dependencies required to compile the project. These can be found here: https://www.dropbox.com/s/wha3akq0kkqoi6r/External.zip - Extract the External folder to the root of the project.


Visual Studio
=============
With the command-line in the root folder of the project; write 'tundra2 -g msvc120' to generate project files for Visual Studio 2013.
You can then open 't2-output/Sandbox.sln' to get started.


Game Content
=============
To build the game content located in the Content folder you will need to run the Asset Builder. This will compile all assets to their runtime formats.
There's tools available for performing this action. Just run 'build.py build_assets' from the command-line.
The newly built assets will be located in Binaries/Content, and they will automatically be loaded when running the engine.
