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


#include "ne_block.h"

#include <QPen>
#include <QGraphicsScene>
#include <QFontMetrics>
#include <QPainter>

#include "ne_port.h"
#include "nodes_editor.h"

using namespace std;



/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_ne_block::_ne_block(_nodes_editor *Nodes_editor1, QGraphicsItem *Parent) : QGraphicsPathItem(Parent)
{
  Nodes_editor=Nodes_editor1;

  QPainterPath Painter_path;
  Painter_path.addRoundedRect(-50, -50, 100, 100, 5, 5);
  setPath(Painter_path);
  setPen(QPen(Qt::darkGreen));
  setBrush(Qt::green);
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsSelectable);
  Horizontal_margin = 20;
  Vertical_margin = 5;
  Width = Horizontal_margin;
  Height = Vertical_margin;

  Name_port=nullptr;
  Type_port=nullptr;
  In_port1=nullptr;
  In_port2=nullptr;
  Out_port=nullptr;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_ne_port* _ne_block::add_port(const QString &Name, bool Is_output1, int Flags, int Pointer1)
{
  _ne_port *Port = new _ne_port(this);
  Port->set_name(Name);
  Port->set_is_output(Is_output1);
  Port->set_block(this);
  Port->set_port_flags(Flags);
  Port->set_pointer(Pointer1);

	QFontMetrics fm(scene()->font());
  int w = fm.horizontalAdvance("a");
	int h = fm.height();
  Height += h;
  Width=w*_ne_common_ns::NUM_CHARS_LINE_BLOCK;

	QPainterPath p;
  p.addRoundedRect(-Width/2, -Height/2, Width, Height, 5, 5);
	setPath(p);

  int y = -Height / 2 + Vertical_margin + Port->radius();
  foreach(QGraphicsItem *Port_aux, childItems()){
    if (Port_aux->type() != _ne_port::Type)
      continue;

    _ne_port *New_port = (_ne_port*) Port_aux;
    if (New_port->is_output())
      New_port->setPos(Width/2 + New_port->radius(), y);
    else
      New_port->setPos(-Width/2 - New_port->radius(), y);
    y += h;
  }

  return Port;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_ne_port* _ne_block::add_input_port(const QString &Name1)
{
  return add_port(Name1, false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_ne_port* _ne_block::add_output_port(const QString &Name1)
{
  return add_port(Name1, true);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_block::create(const QString &Type, const QString &Name1, int Num_inputs1, unsigned char Num_channels1)
{
  Name=Name1;
  Block_type=Type;
  Num_inputs=Num_inputs1;
  Num_channels=Num_channels1;

  Type_port=add_port(Type, 0, _ne_port::TypePort);
  Name_port=add_port(Name, 0, _ne_port::NamePort);
  if (Num_inputs==1) In_port1=add_input_port("in1");
  if (Num_inputs==2){
    In_port1=add_input_port("in1");
    In_port2=add_input_port("in2");
  }
  Out_port=add_output_port("out1");

}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_block::set_mat_pos(int Row1,int Col1)
{
  if (Row1>=0 && Col1>=0){
    if (Nodes_editor->get_mat_objects(Row1,Col1)==false){
      Row=Row1;
      Colum=Col1;
      setPos(Colum*Nodes_editor->step_width(),Row*Nodes_editor->step_height());
      Nodes_editor->set_mat_objects(Row,Colum,true);
    }
    else{
      cout << "Error: the position in mat is occoupied " << endl;
      exit(-1);
    }
  }
  else{
    cout << "Error: the position must be >=0 " << endl;
    exit(-1);
  }
}

#include <QStyleOptionGraphicsItem>


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_block::paint(QPainter *Painter, const QStyleOptionGraphicsItem *Option, QWidget *Widget)
{
  Q_UNUSED(Option)
  Q_UNUSED(Widget)

	if (isSelected()) {
    Painter->setPen(QPen(Qt::darkYellow));
    Painter->setBrush(Qt::yellow);
  } else { // not selected
    if (Num_channels==3){
      Painter->setPen(QPen(Qt::black));
      Painter->setBrush(_ne_block_ns::Orange);
    }
    else{
      Painter->setPen(QPen(Qt::black));
      Painter->setBrush(_ne_block_ns::Gray);
    }
	}

  Painter->drawPath(path());
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_block::contextMenuEvent(QGraphicsSceneContextMenuEvent *Event)
{
  if (isSelected()){
    QMenu menu;
    menu.addAction("Remove");
    QAction *Selection = menu.exec(Event->screenPos());
    if (Selection){
      Nodes_editor->remove_block(this);
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_ne_block* _ne_block::clone()
{
  _ne_block *Block = new _ne_block(0);
  this->scene()->addItem(Block);

  foreach(QGraphicsItem *Port_aux, childItems())
	{
    if (Port_aux->type() == _ne_port::Type)
		{
      _ne_port *New_port = (_ne_port*) Port_aux;
      Block->add_port(New_port->port_name(), New_port->is_output(), New_port->port_flags(), New_port->pointer());
		}
	}

  return Block;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

QVector<_ne_port*> _ne_block::ports()
{
  QVector<_ne_port*> Vec_ports;
  foreach(QGraphicsItem *Port_aux, childItems())
	{
    if (Port_aux->type() == _ne_port::Type)
      Vec_ports.append((_ne_port*) Port_aux);
	}
  return Vec_ports;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

QVariant _ne_block::itemChange(GraphicsItemChange Change, const QVariant &Value)
{
  Q_UNUSED(Change);

  return Value;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_block::name(QString Name1)
{
  Name=Name1;
  Name_port->set_name(Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_block::block_type(QString Type1)
{
  Block_type=Type1;
  Type_port->set_name(Block_type);
}



/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_ne_block *_ne_block::get_block_in_port1()
{
  _ne_port *In_port1=in_port1();

  // check if there are any connection
  if (In_port1==nullptr || In_port1->num_conections()==0) return nullptr;
  else{
    // get the vector of connections. There must be only one connection
    QVector<_ne_connection*> Connections=In_port1->connections();
    // get the first one
    _ne_connection *Connection=Connections[0];
    // get the output port of the connection, that is the port1
    _ne_port *Out_port1=Connection->port1();
    // get the block of the output port
    return (Out_port1->block());
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_ne_block *_ne_block::get_block_in_port2()
{
  _ne_port *In_port2=in_port2();

  // check if there are any connection
  if (In_port2==nullptr || In_port2->num_conections()==0) return nullptr;
  else{
    // get the vector of connections. There must be only one connection
    QVector<_ne_connection*> Connections=In_port2->connections();
    // get the first one
    _ne_connection *Connection=Connections[0];
    // get the output port of the connection, that is the port1
    _ne_port *Out_port1=Connection->port1();
    // get the block of the output port
    return (Out_port1->block());
  }
}
