/*! \file
 * Copyright Domingo Martín Perandres
 * email: dmartin@ugr.es
 * web: http://calipso.ugr.es/dmartin
 * 2019
 * GPL
 *
 * This code is based on the work of STANISLAW ADASZEWSKI
 * http://algoholic.eu/qnodeseditor-qt-nodesports-based-data-processing-flow-editor/
 * Copyright (c) 2012, STANISLAW ADASZEWSKI
 * All rights reserved.
 */


#include "ne_connection.h"
#include "ne_port.h"
#include "ne_block.h"
#include "nodes_editor.h"
#include "ne_common.h"

#include <QBrush>
#include <QPen>
#include <QGraphicsScene>


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_ne_connection::_ne_connection(_nodes_editor *Nodes_editor1, QGraphicsItem *Parent) : QGraphicsPathItem(Parent)
{
  Nodes_editor=Nodes_editor1;

	setPen(QPen(Qt::black, 2));
	setBrush(Qt::NoBrush);
	setZValue(-1);
  Port1 = 0;
  Port2 = 0;

  setFlag(QGraphicsItem::ItemIsSelectable);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_ne_connection::~_ne_connection()
{
  if (Port1)
    Port1->connections().remove(Port1->connections().indexOf(this));
  if (Port2)
    Port2->connections().remove(Port2->connections().indexOf(this));
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_connection::set_pos1(const QPointF &Point)
{
  Pos1 = Point;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_connection::set_pos2(const QPointF &Point)
{
  Pos2 = Point;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_connection::set_port1(_ne_port *Port)
{
  Port1 = Port;

  Port1->connections().append(this);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_connection::set_port2(_ne_port *Port)
{
  Port2 = Port;

  Port2->connections().append(this);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_connection::update_pos_from_ports()
{
  Pos1 = Port1->scenePos();
  Pos2 = Port2->scenePos();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_connection::update_path()
{
  QPainterPath Path;

  Path.moveTo(Pos1);

  qreal dx = Pos2.x() - Pos1.x();

  Path.lineTo(Pos1.x()+dx/2,Pos1.y());
  Path.lineTo(Pos1.x()+dx/2,Pos2.y());
  Path.lineTo(Pos2.x(),Pos2.y());

  setPath(Path);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_connection::update_final_path()
{
  QPainterPath Path;

  _ne_port *Port=port1(); // output port
  _ne_block *Block=Port->block(); // Block output
  _nodes_editor *Nodes_editor=Block->nodes_editor();
  int Counter_c1=Nodes_editor->get_col(Col1)-1;
  int Counter_c2=Nodes_editor->get_col(Col2)-1;
  int Counter_r1=Nodes_editor->get_row(Row)-1;
  int Step_col1;
  int Step_col2;
  int Step_row=round((float)Nodes_editor->space_height()*(float)Counter_row/(float)Counter_r1);

  if (Counter_c1==0) Step_col1=0;
  else Step_col1=round((float)Nodes_editor->space_width()*(float)Counter_col1/(float)Counter_c1);
  if (Counter_c2==0) Step_col2=0;
  else Step_col2=round((float)Nodes_editor->space_width()*(1.0-(float)Counter_col2/(float)Counter_c2));
  if (Counter_r1==0) Step_row=0;
  else Step_row=round((float)Nodes_editor->space_height()*(float)Counter_row/(float)Counter_r1);

  if (Counter_row!=-1){ // indirect
#ifdef USE_COLORS
    QPen Pen;
    Pen.setColor(QColor(_ne_common_ns::Colors[Counter_col1%_ne_common_ns::MAX_NUM_COLORS].Red,_ne_common_ns::Colors[Counter_col1%_ne_common_ns::MAX_NUM_COLORS].Green,_ne_common_ns::Colors[Counter_col1%_ne_common_ns::MAX_NUM_COLORS].Blue));
    setPen(Pen);
#endif

    if (Col1==Col2){
      Path.moveTo(Pos1);
      Path.lineTo(Pos1.x()+_ne_common_ns::RADIUS+_ne_common_ns::CHANNEL_WIDTH_OFFSET+Step_col1,Pos1.y());
      Path.lineTo(Pos1.x()+_ne_common_ns::RADIUS+_ne_common_ns::CHANNEL_WIDTH_OFFSET+Step_col1,Pos2.y());
      Path.lineTo(Pos2.x(),Pos2.y());
    }
    else{
      Path.moveTo(Pos1);
      Path.lineTo(Pos1.x()+_ne_common_ns::RADIUS+_ne_common_ns::CHANNEL_WIDTH_OFFSET+Step_col1,Pos1.y());
      Path.lineTo(Pos1.x()+_ne_common_ns::RADIUS+_ne_common_ns::CHANNEL_WIDTH_OFFSET+Step_col1,Row*Nodes_editor->step_height()+Nodes_editor->block_height()/2+_ne_common_ns::RADIUS+_ne_common_ns::CHANNEL_HEIGHT_OFFSET+Step_row);
      Path.lineTo(Pos2.x()-_ne_common_ns::RADIUS-_ne_common_ns::CHANNEL_WIDTH_OFFSET-Step_col2,Row*Nodes_editor->step_height()+Nodes_editor->block_height()/2+_ne_common_ns::RADIUS+_ne_common_ns::CHANNEL_HEIGHT_OFFSET+Step_row);
      Path.lineTo(Pos2.x()-_ne_common_ns::RADIUS-_ne_common_ns::CHANNEL_WIDTH_OFFSET-Step_col2,Pos2.y());
      Path.lineTo(Pos2.x(),Pos2.y());
    }
  }
  else{ // direct
    Path.moveTo(Pos1);
    Path.lineTo(Pos1.x()+_ne_common_ns::RADIUS+_ne_common_ns::CHANNEL_WIDTH_OFFSET+Step_col1,Pos1.y());
    Path.lineTo(Pos1.x()+_ne_common_ns::RADIUS+_ne_common_ns::CHANNEL_WIDTH_OFFSET+Step_col1,Pos2.y());
    Path.lineTo(Pos2.x(),Pos2.y());
  }

  if (isSelected()) {
    setPen(QPen(Qt::darkYellow));
  }
  else {
      setPen(QPen(Qt::black));
  }

  setPath(Path);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_ne_port* _ne_connection::port1() const
{
  return Port1;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_ne_port* _ne_connection::port2() const
{
  return Port2;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_connection::paint(QPainter *Painter, const QStyleOptionGraphicsItem *Option, QWidget *Widget)
{
  Q_UNUSED(Option)
  Q_UNUSED(Widget)

  if (isSelected()) {
    Painter->setPen(QPen(Qt::gray,1,Qt::DashDotLine));
//    painter->setBrush(Qt::darkYellow);
  }
  else { // not selected
    Painter->setPen(QPen(Qt::black));
//    painter->setBrush(_qneblock_ns::Gray);
  }

  Painter->drawPath(path());
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_connection::contextMenuEvent(QGraphicsSceneContextMenuEvent *Event)
{
  setSelected(true);

  QMenu Menu;
  Menu.addAction("Remove");
  QAction *Selection = Menu.exec(Event->screenPos());
  if (Selection){
    Nodes_editor->remove_connection(this);
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

QVariant _ne_connection::itemChange(GraphicsItemChange Change, const QVariant &Value)
{
  Q_UNUSED(Change);

  return Value;
}
