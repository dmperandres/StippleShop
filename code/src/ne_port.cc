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


#include "ne_common.h"
#include "ne_port.h"
#include "ne_connection.h"
#include <QGraphicsScene>
#include <QFontMetrics>

#include <QPen>




/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_ne_port::_ne_port(QGraphicsItem *Parent):
    QGraphicsPathItem(Parent)
{
  Label = new QGraphicsTextItem(this);

  Radius = _ne_common_ns::RADIUS;
  Margin = _ne_common_ns::MARGIN;

  QPainterPath Painter_path;
  Painter_path.addEllipse(-Radius, -Radius, 2*Radius, 2*Radius);
  setPath(Painter_path);

	setPen(QPen(Qt::darkRed));
	setBrush(Qt::red);

	setFlag(QGraphicsItem::ItemSendsScenePositionChanges);

  Port_flags = 0;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_ne_port::~_ne_port()
{
  foreach(_ne_connection *Conn_aux, Connections)
    delete Conn_aux;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_port::set_block(_ne_block *Block1)
{
  Block = Block1;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_port::set_name(const QString &Name1)
{
  Name = Name1;
  Label->setPlainText(Name1);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_port::set_is_output(bool Output)
{
  Is_output = Output;

	QFontMetrics fm(scene()->font());

  if (Is_output)
    Label->setPos(-Radius - Margin - Label->boundingRect().width(), -Label->boundingRect().height()/2);
	else
    Label->setPos(Radius + Margin, -Label->boundingRect().height()/2);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

int _ne_port::radius()
{
  return Radius;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

bool _ne_port::is_output()
{
  return Is_output;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

QVector<_ne_connection*>& _ne_port::connections()
{
  return Connections;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_port::set_port_flags(int Flags)
{
  Port_flags = Flags;

  if (Port_flags & TypePort){
    QFont Font(scene()->font());
    Font.setBold(true);
    Font.setStretch(80);
    Label->setFont(Font);
		setPath(QPainterPath());
  }
  else{
    if (Port_flags & NamePort){
      QFont Font(scene()->font());
      Label->setFont(Font);
      setPath(QPainterPath());
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_ne_block* _ne_port::block() const
{
  return Block;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

quint64 _ne_port::pointer()
{
  return Pointer;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _ne_port::set_pointer(quint64 Pointer1)
{
  Pointer = Pointer1;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

bool _ne_port::is_connected(_ne_port *Port)
{
  foreach(_ne_connection *Conn_aux, Connections)
    if (Conn_aux->port1() == Port || Conn_aux->port2() == Port)
			return true;

	return false;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

QVariant _ne_port::itemChange(GraphicsItemChange Change, const QVariant &Value)
{
  if (Change == ItemScenePositionHasChanged)
	{
    foreach(_ne_connection *Conn_aux, Connections)
		{
      Conn_aux->update_pos_from_ports();
      Conn_aux->update_path();
		}
	}
  return Value;
}
