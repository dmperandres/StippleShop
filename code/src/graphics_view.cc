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


#include "graphics_view.h"


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_graphics_view::_graphics_view(QWidget *Parent):QGraphicsView(Parent)
{
  Factor=1.0;
}



/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _graphics_view::scaling_factor(float Factor1)
{
  Factor=Factor1;
  resetTransform();
  scale(Factor,Factor);
  horizontalScrollBar()->setValue(-1000);
  verticalScrollBar()->setValue(-1000);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _graphics_view::wheelEvent(QWheelEvent *Event)
{
  // See QWheelEvent documentation
  QPoint Num_degrees = Event->angleDelta();
  int Steps = Num_degrees.y();

  if (Steps<0) Factor*=1.1;
  else Factor/=1.1;

  resetTransform();
  scale(Factor,Factor);
  horizontalScrollBar()->setValue(-1000);
  verticalScrollBar()->setValue(-1000);
}
