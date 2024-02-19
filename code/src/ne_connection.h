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


#ifndef NE_CONNECTION_H
#define NE_CONNECTION_H

#include <QGraphicsPathItem>

class _ne_port;
class _nodes_editor;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _ne_connection : public QGraphicsPathItem
{
public:
	enum { Type = QGraphicsItem::UserType + 2 };

    _ne_connection(_nodes_editor *Nodes_editor1,QGraphicsItem *Parent = 0);
  ~_ne_connection();

  void set_pos1(const QPointF &Pos);
  void set_pos2(const QPointF &Pos);
  void set_port1(_ne_port *Port);
  void set_port2(_ne_port *Port);
  void update_pos_from_ports();
  void update_path();
  void update_final_path();

  _ne_port* port1() const;
  _ne_port* port2() const;

	int type() const { return Type; }

  void set_col1(int Col,int Counter){Col1=Col;Counter_col1=Counter;};
  void set_col2(int Col,int Counter){Col2=Col;Counter_col2=Counter;};
  void set_row(int Row1,int Counter){Row=Row1;Counter_row=Counter;};

  void paint(QPainter *Painter, const QStyleOptionGraphicsItem *Option, QWidget *Widget);
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *Event);

protected:
QVariant itemChange(GraphicsItemChange Change, const QVariant &Value);

private:
  QPointF Pos1;
  QPointF Pos2;
  _ne_port *Port1;
  _ne_port *Port2;

  _nodes_editor *Nodes_editor;

  int Col1;
  int Counter_col1;
  int Col2;
  int Counter_col2;
  int Row;
  int Counter_row;
};
#endif
