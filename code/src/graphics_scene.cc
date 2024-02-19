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


#include "graphics_scene.h"
#include "window.h"
#include <iostream>

using namespace std;

// the information for doing the drag-and-drop is taken from:
// http://doc.qt.io/qt-4.8/dnd.html


/*****************************************************************************//**
 * Not clear to me: this function must be defined although it makes nothing
 *
 *
 *
 *****************************************************************************/

void _graphics_scene::dragEnterEvent(QGraphicsSceneDragDropEvent *Event)
{
  Q_UNUSED(Event)
}


/*****************************************************************************//**
 * Not clear to me: this function must be defined although it makes nothing
 *
 *
 *
 *****************************************************************************/

void _graphics_scene::dragMoveEvent(QGraphicsSceneDragDropEvent *Event)
{
  Q_UNUSED(Event)
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _graphics_scene::dropEvent(QGraphicsSceneDragDropEvent *Event)
{
  const QMimeData* Mime = Event->mimeData();

  if (Mime->hasText()) {
    QPointF Position=Event->scenePos();
    QString Text=Mime->text();
    Window->add_filter(Text.toInt(),(float) Position.x(),Position.y());
    Event->acceptProposedAction();
  }
}

