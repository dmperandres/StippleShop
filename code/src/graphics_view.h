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


#ifndef _GRAPHICS_VIEW_H
#define _GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QTimeLine>
#include <QWidget>
#include <QScrollBar>
#include <iostream>


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _graphics_view : public QGraphicsView
{
Q_OBJECT

public:
  _graphics_view(QWidget *Parent=0);

  void scaling_factor(float Factor1);

protected:
  void wheelEvent(QWheelEvent *Event);

private:
  float Factor;
};

#endif
