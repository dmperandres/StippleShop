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


#ifndef _GRAPHICS_SCENE_H
#define _GRAPHICS_SCENE_H

#include <QGraphicsScene>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsView>
#include <QScrollBar>
#include <QMimeData>

#include <iostream>

class _window;


/*****************************************************************************//**
 * This class is for foing the drag and drop od the tree items
 *
 *
 *
 *****************************************************************************/

class _graphics_scene : public QGraphicsScene
{
Q_OBJECT

public:
  _graphics_scene(_window *Window1){Window=Window1;};

protected:
  void dragEnterEvent(QGraphicsSceneDragDropEvent *Event);
  void dragMoveEvent(QGraphicsSceneDragDropEvent *Event);
  void dropEvent(QGraphicsSceneDragDropEvent *Event);

private:
  _window *Window;
};

#endif
