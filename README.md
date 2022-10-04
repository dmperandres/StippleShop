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

## Project website
https://tobias.isenberg.cc/VideosAndDemos/Martin2017SDS

## Note
Please note the software is provided "as is".  Use it at your own risk, although data loss is unlikely. Do take the standard precautions like saving your work in other programs.

## License
[GNU General Public License v3](https://www.gnu.org/licenses/gpl-3.0.en.html)
(see [license.txt](license.txt)).

## Requirements
This project compiles under Linux and Windows. In both cases the project requires these additional components:
* ```Qt SDK``` (libs + QtCreator), checked with version 5.15.2: https://www.qt.io/
* ```OpenCV```, checked with version 4.6: https://opencv.org/
* ```GLEW```, checked with version 2.1: https://glew.sourceforge.net/
* ```OpenGL```, checked with version 4.6): https://www.opengl.org/

### Installation of the requirements under Linux

* First, install the GPU drivers for your respective graphics card. For example for an nVidia card under Ubuntu, *[something that needs to be done]*.
* Install the compilation environment: ```sudo apt-get install build-essential```
* *more*

### Installation of the requirements under Windows

*to be added later*

## Preparation/configuration
To prepare for compilation, edit the [```stippleshop.pro```](src/stippleshop.pro) project file. At the top, if you do not want to compile under Linux, you need to switch the compilation to Windows (uncomment ```DEFINES = WINDOWS``` and comment out ```DEFINES += LINUX```). You also need to adjust the ```INCLUDEPATH``` paths to your respective library versions of OpenCV and GLEW (at the bottom of the file). In the [```stippleshop.pro```](src/stippleshop.pro) project file you can also adjust the filters to be included.

## Compilation Linux

*We need detailed steps here.*

## Compilation Windows

We need detailed steps here.

## Example tutorial to create a simple stippled vector image
1. Run the compiled binary
2. *more steps*
3. *more steps*

For further documentation on other use and the different functionality see the detailed description in the [```StippleShop_manual.pdf```](doc/StippleShop_manual.pdf) manual.
