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


#ifndef NE_BLOCK_H
#define NE_BLOCK_H

#include "ne_connection.h"
#include <QColor>

#include <iostream>
#include <QGraphicsPathItem>

class _nodes_editor;
class _ne_port;

namespace _ne_block_ns
{
const QColor Orange={255,215,0};
const QColor Gray={240,240,240};
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _ne_block : public QGraphicsPathItem
{
public:
	enum { Type = QGraphicsItem::UserType + 3 };

  _ne_block(_nodes_editor *Nodes_editor1, QGraphicsItem *Parent = 0);

  _ne_port* add_port(const QString &Name, bool Is_output1, int Flags = 0, int Pointer1 = 0);
  _ne_port* add_input_port(const QString &Name1);
  _ne_port* add_output_port(const QString &Name1);
  void create(const QString &Type, const QString &Name1, int Num_inputs1, unsigned char Num_channels1);

  void paint(QPainter *Painter, const QStyleOptionGraphicsItem *Option, QWidget *Widget);
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *Event);

  _ne_block* clone();
  QVector<_ne_port*> ports();

	int type() const { return Type; }

  void set_mat_pos(int Row1,int Col1);
  int get_mat_row(){return Row;};
  int get_mat_col(){return Colum;};

  _nodes_editor *nodes_editor(){return Nodes_editor;};

  void name(QString Name1);
  QString name(){return Name;};

  void block_type(QString Type1);
  QString block_type(){return Block_type;};

  _ne_port *out_port(){return Out_port;};
  _ne_port *in_port1(){return In_port1;};
  _ne_port *in_port2(){return In_port2;};

  // for creating the blocks
  _ne_block *get_block_in_port1();
  _ne_block *get_block_in_port2();

  int num_inputs(){return Num_inputs;};

protected:
  QVariant itemChange(GraphicsItemChange Change, const QVariant &Value);

private:
  int Horizontal_margin;
  int Vertical_margin;
  int Width;
  int Height;
  int Row;
  int Colum;

  _nodes_editor *Nodes_editor;

  _ne_port *Name_port;
  _ne_port *Type_port;
  _ne_port *In_port1;
  _ne_port *In_port2;
  _ne_port *Out_port;

  int Num_inputs;
  QString Name;
  QString Block_type;
  unsigned char Num_channels;
};
#endif
