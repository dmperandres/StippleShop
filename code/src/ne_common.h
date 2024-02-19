/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2019
 * GPL
 *
 * In case that you use all or part of this code, please include a reference to this article:

 * Domingo Martín, Germán Arroyo, Alejandro Rodríguez and Tobias Isenberg.
 * A survey of digital stippling.
 * Computer & Graphics 67, PP. 24-44, 2017.
 * DOI information: https://doi.org/10.1016/j.cag.2017.05.001
 */


#ifndef NE_COMMON_H
#define NE_COMMON_H

namespace _ne_common_ns {
  const int NUM_MAX_ROWS=10;
  const int NUM_MAX_COLS=10;

  const int MIN_INDEX_GRID_LINES=-10;
  const int MAX_INDEX_GRID_LINES=50;
  const int MAX_LINE_VALUE=10000;

  const unsigned char LINE_RED=200;
  const unsigned char LINE_GREEN=200;
  const unsigned char LINE_BLUE=200;

  const unsigned int NUM_CHARS_LINE_BLOCK=18;
  const unsigned int NUM_LINES_BLOCK=5;

  const float STEP_WIDTH_FACTOR=1.5;
  const float STEP_HEIGHT_FACTOR=2;

  const int STEP_HEIGHT_DEFAULT=100;
  const int STEP_WIDTH_DEFAULT=100;

  const int CHANNEL_WIDTH_OFFSET=5;
  const int CHANNEL_HEIGHT_OFFSET=5; // 60
  const int WIRE_OFFSET=5;

  const int RADIUS=5;
  const int MARGIN=2;

  struct _color{
    unsigned char Red;
    unsigned char Green;
    unsigned char Blue;
  };

  const _color Colors[]={{255,0,0},{0,200,0},{0,0,255},{0,200,200},{200,0,200}};
  const int MAX_NUM_COLORS=5;
  const _color Gray={200,200,200};

}

#endif
