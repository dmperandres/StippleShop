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


#ifndef NODES_EDITOR_H
#define NODES_EDITOR_H

#include <QWidget>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QGraphicsView>
#include <vector>
#include <iostream>
#include <string>
#include <map>

#include <QMenu>
#include <QPoint>
#include <QFile>

#include "ne_port.h"
#include "ne_common.h"

#include "filter.h"

class QGraphicsScene;
class _ne_connection;
class QGraphicsItem;
class QPointF;
class _ne_block;
class QNEMainWindow;


typedef struct {
  std::string Type;
  std::string Name;
  std::string In1;
  std::string In2;
  int Row;
  int Col;
} _block;

typedef struct {
  int Row;
  int Col;
} _position;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

class _nodes_editor : public QWidget
{
Q_OBJECT

public:
  explicit _nodes_editor(QWidget *Parent = 0);
  ~_nodes_editor();

  void clear();

  void install(QGraphicsScene *Scene1);
  void set_font_sizes(int Height1,int Width1);
  int  step_width(){return Step_width;};
  int  step_height(){return Step_height;};
  int  space_width(){return Space_width;};
  int  space_height(){return Space_height;};
  int  block_width(){return Block_width;};
  int  block_height(){return Block_height;};

  bool eventFilter(QObject *Object, QEvent *Event);

  void add_grid();

  void add_block(std::string Type, std::string Name, int Num_parameters, int Num_imput_ports, unsigned char Num_channels1);
  void add_block(std::string Type,std::string Name,int Num_parameters,int Num_imput_ports, unsigned char Num_channels1,float Col,float Row);
  void add_initial_blocks();

  void clear_vectors();
  int increment_col(int Col1);
  int get_col(int Col1){return Cols[Col1];};
  int increment_row(int Row1);
  int get_row(int Row1){return Rows[Row1];};

  void set_mat_objects(int Row,int Col,bool Value);
  bool get_mat_objects(int Row,int Col);

  void compute_path_indices(int Row1, int Col1, int Row2, int Col2, _ne_connection *Connection);
  void redraw_paths();

  // for creating the blocks and connections
  void read_data_effect(std::string Name);
  void create_blocks_from_effect();
  bool create_blocks_from_editor();

  _position compute_position(std::string Name);
  void add_connection(_ne_block *Block_out,_ne_block *Block_in,int Num_port);

  bool find_root(_ne_block *Block1);

  void changed(bool Changed1){Changed=Changed1;};
  bool changed(){return Changed;};

  void remove_block(_ne_block *Block1);
  void remove_connection(_ne_connection *Connection1);

  std::map<std::string,_block> *map_data_blocks(){return &Map_data_blocks;};
  std::vector<std::string> *vec_order(){return &Vec_order;};
  std::map<std::string,std::map<std::string,std::string>> *map_filters_parameters(){return &Map_filters_parameters;};

private:
  QGraphicsItem *item_at(const QPointF &Point);

  QGraphicsItem *item_selected();

  void resizeEvent(QResizeEvent * event);

private:
  QGraphicsScene *Scene;
  _ne_connection *Connection;
  _ne_block *Selected_block;

  int Font_height;
  int Font_width;
  int Step_height;
  int Step_width;
  int Space_height;
  int Space_width;
  int Block_height;
  int Block_width;

  int X_old;
  int Y_old;

  // for doing the interaction with blocks
  bool Objects[_ne_common_ns::NUM_MAX_ROWS][_ne_common_ns::NUM_MAX_COLS];
  int Cols[_ne_common_ns::NUM_MAX_COLS];
  int Rows[_ne_common_ns::NUM_MAX_ROWS];

  // to order the filters when created from the editor
  std::vector<std::string> Vec_order;

  // for reading the file
  std::vector<std::map<std::string,std::string> > Filters_json_data;

  // these are used to load data
  std::map<std::string,_ne_block *> Map_blocks;
  std::map<std::string,_block> Map_data_blocks;
  std::map<std::string,std::map<std::string,std::string>> Map_filters_parameters;

  int Pos_row;

  // this is used to edit data
  std::list<_ne_block *> List_blocks;

//  static int NUM_FILTERS;
  int Num_filters;

  bool Changed;

};

#endif
