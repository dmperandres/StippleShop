# StippleShop

## General
This repository contains the source code for the StippleShop program that we developed as part of our publication:

Domingo Martín, Germán Arroyo, Alejandro Rodríguez and Tobias Isenberg. A survey of digital stippling. Computer & Graphics 67: 24-44, 2017.
DOI: [10.1016/j.cag.2017.05.001](https://doi.org/10.1016/j.cag.2017.05.001). An open access paper version is [available on HAL](https://hal.inria.fr/hal-01528484).

## Bibtex
```
@article{Martin:2017:SDS,
  author      = {Domingo Mart{\'i}n and Germ{\'a}n Arroyo and Alejandro Rodr{\'i}guez and Tobias Isenberg},
  title       = {A Survey of Digital Stippling},
  journal     = {Computers \& Graphics},
  year        = {2017},
  volume      = {67},
  month       = oct,
  pages       = {24--44},
  doi         = {10.1016/j.cag.2017.05.001},
  doi_url     = {https://doi.org/10.1016/j.cag.2017.05.001},
  oa_hal_url  = {https://hal.inria.fr/hal-01528484},
  url         = {https://tobias.isenberg.cc/VideosAndDemos/Martin2017SDS},
  github_url  = {https://github.com/dmperandres/StippleShop},
  pdf         = {https://tobias.isenberg.cc/personal/papers/Martin_2017_SDS.pdf},
}
```

## Project website and pre-compiled binaries
https://tobias.isenberg.cc/VideosAndDemos/Martin2017SDS

This website also provides pre-compiled binary releases for both Windows and Linux.

## Note
Please note the software is provided "as is".  Use it at your own risk, although data loss is unlikely. Do take the standard precautions like saving your work in other programs.

## License
[GNU General Public License v3](https://www.gnu.org/licenses/gpl-3.0.en.html)
(see [license.txt](license.txt)).

## Requirements
This project compiles under Linux and Windows. In both cases the project requires these additional components:
* ```Qt SDK``` (libraries + QtCreator + MingGW compiler), checked with version 6.2.4: https://www.qt.io/
* ```OpenCV```, checked with version 4.9: https://opencv.org/
* ```CMake```, checked with version 3.27.9: https://cmake.org/
* ```GLEW```, checked with version 2.1: https://glew.sourceforge.net/
Below we explain how these requirements are installed, both for Windows and Linux.

## Build process for Windows (tested under Windows 10)

### Installation of the compilation requirements under Windows
* download the binary [Qt](https://www.qt.io/) open-source online installer from https://www.qt.io/download-open-source and run it
  * this requires a free qt.io account
  * we recommend to install Qt to the default folder `C:\Qt`
  * in the "Select Components" dialog, under "Qt 5.15.2", leave all components unselected and only select the "MinGW 8.1.0 64-bit" option
  * also unselect the "Preview" on the right side
  * leave the rest of the dialogs on their defaults, and accept the license
* check that the MinGW compiler is installed in folder `C:\Qt\tools\mingw810_64\bin`
* set the necessary environment variables
  * open Windows' environments evariables dialog: Settings > System > About > Advanced System Settings > Environment Variables...
  * under system variables, add the following variables (adjust if you installed Qt to a different directory):
    * `CMAKE_C_COMPILER`: `C:\Qt\Tools\mingw810_64\bin\x86_64-w64-mingw32-gcc.exe`
    * `CMAKE_CXX_COMPILER`: `C:\Qt\Tools\mingw810_64\bin\x86_64-w64-mingw32-g++.exe`
    * `CMAKE_MAKE_PROGRAM`: `C:\Qt\Tools\mingw810_64\bin\mingw32-make.exe`
  * restart/reboot Windows to make these variables known to the system
* download the [OpenCV](https://opencv.org/) Windows installer from https://opencv.org/releases/ (we tested it with version 4.6) and run it
  * it is essentially a self-extracting archive, extract it to a directory of your choice (by default it is your user's `Download` folder)
  * this process creates a `opencv` subdirectory that includes the source files of OpenCV
  * inside this `opencv` folder create a new subdirectory called `mybyuild`, this will hold the results of the build
* download the [CMake](https://cmake.org/) binary Windows installer (64 bit msi) from https://cmake.org/download/ (we tested with version 3.27.9) and run it
  * [//]: # (https://github.com/Kitware/CMake/releases/tag/v3.25.0)
  * accept the license
  * ask the installer to add CMake to the system PATH for all users
  * use the default target directory, and then install
* now we compile [OpenCV](https://opencv.org/)
  * to create the needed makefiles, run the CMake program you just installed (find it via your installed programs)
  * click the "Enviroment..." button and verify that the `CMAKE_C_COMPILER`, `CMAKE_CXX_COMPILER`, and `CMAKE_MAKE_PROGRAM` variables are defined, then close the dialog
  * click the "Browse Source..." button and select the "sources" folder of OpenCV, by default this should be ```C:\Users\[your windows user]\Downloads\opencv\sources```
  * click the "Browse Build..." button and select the "mybuild" folder you previously created for OpenCV, by default this should be ```C:\Users\[your windows user]\Downloads\opencv\mybuild```
  * click the "Configure" button and select "MingGW Makefiles" from the drop-down list, then click the "Finish" button
* _not complete yet, will be finalized later_

### StippleShop Compilation under Windows
* get the StippleShop sources, e.g., by downloading a zip archive from [`https://github.com/dmperandres/StippleShop/archive/refs/heads/master.zip`](https://github.com/dmperandres/StippleShop/archive/refs/heads/master.zip) and then extracting the archive
* in the main directory of the StippleShop code (`cd ~/code/StippleShop-master/src` or similar), edit the [```stippleshop.pro```](src/stippleshop.pro) project file with a text file editor ([Nodepad++](https://notepad-plus-plus.org/downloads/) or similar)
  * at the top, switch the compilation to Windows (uncomment ```DEFINES += WINDOWS``` and comment out ```DEFINES += LINUX```)
  * you also need to adjust the ```INCLUDEPATH``` paths to your respective library versions of OpenCV and GLEW (at the bottom of the file).
  * in the [```stippleshop.pro```](src/stippleshop.pro) project file you can also adjust the filters to be included. By default, however, you can leave these settings as they are.
* _not complete yet, will be finalized later_

## Build process for Linux (tested with [Kubuntu](https://kubuntu.org/) 22.04.3 & [Linux Mint](https://linuxmint.com/) 21.3)

### Installation of the requirements under Linux
* update your environment: `sudo apt update ; sudo apt upgrade`
* install the compiler and other complementary tools: `sudo apt install build-essential libxcb-cursor0 cmake cmake-qt-gui wget git`
* create a folder to place all the necessary code and libraries and change to that folder: `cd ~ ; mkdir code ; cd code`
* now we get and compile [OpenCV](https://opencv.org/)
  * download the sources: `wget https://github.com/opencv/opencv/archive/4.9.0.zip`
  * unzip the downloaded archive: `unzip 4.9.0.zip`
  * change the name of the created folder: `mv opencv-4.9.0 opencv-4.9`
  * create a new folder to hold the created library: `mkdir opencv-4.9.0`
  * change to the sources directory: `cd opencv-4.9`
  * create a new folder to hold the object code: `mkdir build`
  * start the CMake GUI: `cmake-gui`
  * click the "Browse Source..." button and select the "sources" folder of OpenCV, by default this should be `/home/user/code/opencv-4.9` with `user` being your Linux user name
  * click the "Browse Build..." button and select the "build" folder you previously created for OpenCV, by default this should be `/home/user/code/opencv-4.9/build`
  * click the "Configure" button and select "Unix Makefiles" from the drop-down list, leave "Use native compilers" selected, then click the "Finish" button
  * this runs some processes and then shows some options; from these options select BUILD_JPEG and BUILD_PNG, in addition to the already selected ones
  * specify where the library and complementary files will be placed for a local installation: in the field CMAKE_INSTALL_PREFIX change `/usr/local` to `/home/user/code/opencv-4.9.0`
  * click the "Generate" button to generate the Makefiles, then close the CMake GUI window
  * back in the command line, change to build folder: `cd build`
  * compile OpenCV by running `make` (depending on your machine and its processors it could be done quicker with the `-j` option and specifying the number of threads to use; e.g., `make -j4`); this process may take quite a while ...
  * finally do the local installation: `make install`
  * this should have added the compiled OpenCV to `~/code/opencv-4.9.0`
* now we get and and compile [GLEW](https://glew.sourceforge.net/)
  * change back to the code directory: `cd ~/code`
  * download the sources: `wget https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0.zip/download -O glew.zip`
  * unzip the downloaded archive: `unzip glew.zip`
  * rename the generated archive to avoid errors with the installation later: `mv glew-2.1.0 glew-2.1`
  * create a folder for the resulting installation: `mkdir glew-2.1.0`
  * if you follow this process on a real machine then the needed OpenGL drivers and development files for X11 should be installed already; nothing do to at this point
  * if, in contrast, you follow this process in a virtual machine then we have to install the needed OpenGL drivers and development files for X11 (using free drivers)
    * run `sudo apt install freeglut3-dev libx11-dev`
  * change to the GLEW sources folder: `cd glew-2.1`
  * edit the Makefile to specify Makefile a local installation instead of a global one: `nano Makefile`, look for `GLEW_DEST`, change `/usr` to `/home/user/code/glew-2.1.0`, save the file with Ctrl-O, and then exit nano with Ctrl-X
  * compile GLEW: `make`
  * install GLEW: `make install`
* now we get [Qt](https://www.qt.io/)
  * open a browser and go to [`https://www.qt.io/download-open-source?hsLang=en`](https://www.qt.io/download-open-source?hsLang=en)
  * scroll down to find the "Download the Qt Online Installer" button and click it
  * select Linux and click on the "Qt Online Installer for Linux (64-bit)" button to download the install script
  * change to folder where you saved the downloaded script, make the script executable, and run it; e.g., `cd ~/Downloads ; chmod 755 qt-unified-linux-x64-4.6.1-online.run ; ./qt-unified-linux-x64-4.6.1-online.run`
  * this opens a dialog to configure the Qt installation
  * you need a Qt account to proceed on the first screen, so either use your existing account or generate a new account; then "Next >"
  * agree to the license on the next screen, either enter a company name or check the box that states that you are an individual, then "Next >"
  * simply "Next >" on the Welcome dialog
  * make a decision on the Contribution to Qt Development dialog, then "Next >"
  * in the Installation folder dialog, specify the target directory; e.g., `/home/user/code/Qt`
  * also leave the "Custom installation" box checked, then click "Next >"
  * in the Components tree, unselect "Qt Design Studio > Qt Design Studio X.Y.Z" and select "Qt > Qt 6.2.4"; in addition
  * if there are space issues, you can also unselect items that you do not need:
    * under the "Qt > Qt 6.2.4" subtree, you can de-select everything except "Desktop gcc 64-bit" and "Qt 5 Compatibility Module"
    * on the right side unselect "Preview"
  * add any additional components you may want, but leave the rest of the pre-selections as they are, then click "Next >"
  * in the License Agreement dialog, check the box that you agree to the license conditions, then click "Next >"
  * click "Next >" to start the installation of Qt (if you do not have enough disk space then "Next >" is greyed out)

### StippleShop Compilation under Linux
* get the StippleShop sources: `cd ~/code ; git clone https://github.com/dmperandres/StippleShop.git`
* change to the main directory of the StippleShop code: `cd ~/code/StippleShop/src`
* edit the [```stippleshop.pro```](src/stippleshop.pro) project file; e.g., `nano stippleshop.pro`
  * at the bottom of the file, adjust the ```INCLUDEPATH``` and ```LIBS``` paths to your respective user name and to your library versions of OpenCV and GLEW:
  ```
  INCLUDEPATH += /home/user/code/opencv-4.9.0/include/opencv4
  INCLUDEPATH += /home/user/code/opencv-4.9.0/include/opencv4/opencv2
  INCLUDEPATH += /home/user/code/glew-2.1.0/include
  INCLUDEPATH += $$PWD/shaders
  INCLUDEPATH += $${FILE_IO_PATH}
  INCLUDEPATH += $${COMMON_CLASSES_PATH}
  
  LIBS += \
      -L/home/user/code/glew-2.1.0/lib -lGLEW \
      -L/home/user/code/opencv-4.9.0/lib -lopencv_core -lopencv_highgui -lopenc>
      -L/usr/X11R6/lib64 -lGL
  }
  ```
  * in the [```stippleshop.pro```](src/stippleshop.pro) project file you can also adjust the filters to be included. By default, however, you can leave these settings as they are.
  * once done with the edits in [```stippleshop.pro```](src/stippleshop.pro), save it and exit the editor
* run QtCreator: `cd ~/code ; Qt/Tools/QtCreator/bin/qtcreator`
* open StippleShop project via "File > Open file or project..." and find the [```stippleshop.pro```](src/stippleshop.pro) in the folder ```~/code/StippleShop/scr```
* click on the "Configure project" button
* click on the "stippleshop [master]" project object in the list on the left, then in the vertical toolbar further to the left click on "Projects" (the one with wrench icon), and in the dialog that opens up unselect "Shadow build"
* if you want you can also edit/further adjust the [```stippleshop.pro```](src/stippleshop.pro) project file here, by clicking on the "Edit" icon on the vertical toolbar on the very left (the one with the document icon), then unfolding the tree under the "stipplesop [master]" project object, and double-clicking the [```stippleshop.pro```](src/stippleshop.pro) project file; make sure to save the project file done (Ctrl-S or File > Save stippleshop.pro)
* initiate the build via Ctrl-B or Build > Build Project "stippleshop"
* _not complete yet, currently in the process of being written_

## Example tutorial to create a simple stippled vector image
1. Run the compiled binary
2. *more steps*
3. *more steps*

For further documentation on other use and the different functionality see the detailed description in the [```StippleShop_manual.pdf```](doc/StippleShop_manual.pdf) manual.
