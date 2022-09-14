# StippleShop

## General
This repository contains the source code for the StippleShop program that we developed as part of our publication:

Domingo Martín, Germán Arroyo, Alejandro Rodríguez and Tobias Isenberg. A survey of digital stippling. Computer & Graphics 67: 24-44, 2017.
DOI: [10.1016/j.cag.2017.05.001](https://doi.org/10.1016/j.cag.2017.05.001)

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

## Additional filter plugins
Some additional code parts for specific filters have unclear or incompatible licenses and are thus not included here. They can be obtained from (http://calipso.ugr.es/dmartin/Research/Software).

## Requirements
This project compiles under Linux and Windows. For Linux it requires the modules ```module1```, ```module2```, and ```module3```. To install those use your system's default installation mechanism. For Windows, ```software1```, ```software2```, and ```software3``` need to be installed.

In addition, in both cases the project requires:
* ```Qt IDE``` version ?: (add.link.com)
* ```QtCreator``` version ?: (add.link.com)
* ```OpenCV``` version ?: (add.link.com)
* ```GLEW``` version ?: (add.link.com)
* anything else?

## Preparation/configuration
To prepare for compilation, edit the [```stippleshop.pro```](src/stippleshop.pro) project file. At the top, if you do not want to compile under Linux, you need to switch the compilation to Windows (uncomment ```DEFINES = WINDOWS``` and comment out ```DEFINES += LINUX```). You also need to adjust the ```INCLUDEPATH``` paths to your respective library versions of OpenCV and GLEW (at the bottom of the file).

In the [```stippleshop.pro```](src/stippleshop.pro) project file you can also adjust the filters to be included. If those filters not included in this repository (see above) should be included, download them from the mentioned URL and enable the corresponding lines in the [```stippleshop.pro```](src/stippleshop.pro) project file.

## Compilation Linux

We need detailed steps here.

## Compilation Windows

We need detailed steps here.

## Example tutorial to create a simple stippled vector image
1. Run the compiled binary
2. more steps
3. more steps

For further documentation on other use and the different functionality see the detailed description in the [```StippleShop_manual.pdf```](doc/StippleShop_manual.pdf) manual.
