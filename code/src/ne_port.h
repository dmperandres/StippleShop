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


#ifndef NE_PORT_H
#define NE_PORT_H

#include <QGraphicsPathItem>

class _ne_block;
class _ne_connection;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _ne_port : public QGraphicsPathItem
{
public:
	enum { Type = QGraphicsItem::UserType + 1 };
	enum { NamePort = 1, TypePort = 2 };

  _ne_port(QGraphicsItem *Parent = 0);
  ~_ne_port();

  void set_block(_ne_block *Block1);
  void set_name(const QString &Name1);
  void set_is_output(bool Output);
	int radius();
  bool is_output();
  QVector<_ne_connection*>& connections();
  void set_port_flags(int Flags);

  const QString& port_name() const { return Name; }
  int port_flags() const { return Port_flags; }

	int type() const { return Type; }

  _ne_block* block() const;

  quint64 pointer();
  void set_pointer(quint64 Pointer1);

  bool is_connected(_ne_port *Port);

  int num_conections(){return Connections.size();};

protected:
  QVariant itemChange(GraphicsItemChange Change, const QVariant &Value);

private:
  _ne_block *Block;
  QString Name;
  bool Is_output;
  QGraphicsTextItem *Label;
  int Radius;
  int Margin;
  QVector<_ne_connection*> Connections;
  int Port_flags;
  quint64 Pointer;
};

#endif
