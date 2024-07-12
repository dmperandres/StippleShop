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


#include "nodes_editor.h"

#include <QGraphicsScene>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QInputDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>

#include <filter.h>

#include <qlineedit.h>

#include "ne_port.h"
#include "ne_connection.h"
#include "ne_block.h"

//using namespace std;
using namespace _f_filter_ns;

// static
//int _nodes_editor::NUM_FILTERS=0;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_nodes_editor::_nodes_editor(QWidget *Parent) :
    QWidget(Parent)
{
  Step_height=_ne_common_ns::STEP_HEIGHT_DEFAULT;
  Step_width=_ne_common_ns::STEP_WIDTH_DEFAULT;

  for (unsigned int i=0;i<_ne_common_ns::NUM_MAX_ROWS;i++){
    for (unsigned int j=0;j<_ne_common_ns::NUM_MAX_ROWS;j++){
      Objects[i][j]=false;
    }
  }

  clear_vectors();

  Connection = nullptr;
  Selected_block=nullptr;

  Changed=true;

  Num_filters=0;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_nodes_editor::~_nodes_editor()
{
  for (auto Block: List_blocks){
    delete Block;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::clear()
{
  for (auto Block: List_blocks){
    delete Block;
  }

  List_blocks.clear();

  for (unsigned int i=0;i<_ne_common_ns::NUM_MAX_ROWS;i++){
    for (unsigned int j=0;j<_ne_common_ns::NUM_MAX_ROWS;j++){
      Objects[i][j]=false;
    }
  }

  clear_vectors();

  Connection = nullptr;
  Selected_block=nullptr;

  Changed=true;

  Filters_json_data.clear();
  Map_blocks.clear();
  Map_data_blocks.clear();
  Map_filters_parameters.clear();

  Num_filters=0;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::install(QGraphicsScene *Scene1)
{
  Scene1->installEventFilter(this);
  Scene = Scene1;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::set_font_sizes(int Height1,int Width1)
{
  Font_height=Height1;
  Font_width=Width1;

  Step_width=Font_width*_ne_common_ns::NUM_CHARS_LINE_BLOCK*_ne_common_ns::STEP_WIDTH_FACTOR;
  Step_height=Font_height*_ne_common_ns::NUM_LINES_BLOCK*_ne_common_ns::STEP_HEIGHT_FACTOR;
  // compute the space between boxes
  // step-letters-circles-offset
  Space_width=Step_width-Font_width*_ne_common_ns::NUM_CHARS_LINE_BLOCK-4*_ne_common_ns::RADIUS-2*_ne_common_ns::CHANNEL_WIDTH_OFFSET;
  Space_height=Step_height-Font_height*_ne_common_ns::NUM_LINES_BLOCK-4*_ne_common_ns::RADIUS-2*_ne_common_ns::CHANNEL_HEIGHT_OFFSET;

  Block_width=Font_width*_ne_common_ns::NUM_CHARS_LINE_BLOCK;
  Block_height=Font_height*_ne_common_ns::NUM_LINES_BLOCK;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

QGraphicsItem* _nodes_editor::item_at(const QPointF &Point)
{
  QList<QGraphicsItem*> items = Scene->items(QRectF(Point - QPointF(1,1), QSize(3,3)));

	foreach(QGraphicsItem *item, items)
		if (item->type() > QGraphicsItem::UserType)
			return item;

	return 0;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

QGraphicsItem* _nodes_editor::item_selected()
{
  QList<QGraphicsItem*> items = Scene->items();

  foreach(QGraphicsItem *item, items)
    if (item->type() > QGraphicsItem::UserType && item->isSelected())
      return item;

  return 0;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::resizeEvent(QResizeEvent * event)
{
  Q_UNUSED(event)
  adjustSize();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

bool _nodes_editor::eventFilter(QObject *Object, QEvent *Event)
{
  QGraphicsSceneMouseEvent *me = (QGraphicsSceneMouseEvent*) Event;

  switch ((int) Event->type()){
  case QEvent::GraphicsSceneMousePress:
    switch ((int) me->button()){
    case Qt::LeftButton:
      {
        QGraphicsItem *item = item_at(me->scenePos());
        _ne_port *port1=(_ne_port *)item;

        if (item && (item->type() == _ne_port::Type) && port1->is_output()){
          Connection = new _ne_connection(this,0);
          Scene->addItem(Connection);
          Connection->set_port1((_ne_port*) item);
          Connection->set_pos1(item->scenePos());
          Connection->set_pos2(me->scenePos());
          Connection->update_path();
          return true;
        }
        else{
          QPointF P=me->scenePos();
          int Col=round(P.x()/Step_width);
          int Row=round(P.y()/Step_height);
          if ((Col==0 && Row>1) || Col>0){
            if (item && item->type() == _ne_block::Type){
              Selected_block = (_ne_block*) item;

              QPointF P=me->scenePos();
              X_old=round(P.x()/Step_width);
              Y_old=round(P.y()/Step_height);

              Objects[Y_old][X_old]=false;
            }
          }
        }
      }
      break;
    }
    break;
  case QEvent::GraphicsSceneMouseMove:
    {
      QPointF Position=me->scenePos();

      if (Position.x()<_ne_common_ns::STEP_WIDTH_DEFAULT/2) Position.setX(_ne_common_ns::STEP_WIDTH_DEFAULT/2);

      if (Connection){
        Connection->set_pos2(Position);
        Connection->update_path();
        return true;
      }
      if (Selected_block){
        Selected_block->setPos(Position);

      }
    }
    break;
  case QEvent::GraphicsSceneMouseRelease:
    if (me->button() == Qt::LeftButton){
      if (Connection){ // end point of a connection
        QGraphicsItem *item = item_at(me->scenePos());
        if (item && item->type() == _ne_port::Type){
          _ne_port *port1 = Connection->port1();
          _ne_port *port2 = (_ne_port*) item;

          if (port1->block() != port2->block() && !port2->is_output() && !port1->is_connected(port2) && port2->num_conections()==0){
            Connection->set_pos2(port2->scenePos());
            Connection->set_port2(port2);

            redraw_paths();
            Connection = nullptr;

            Changed=true;

            return true;
          }
          else{
            Scene->removeItem(Connection);
            delete Connection;
            Connection = nullptr;
            return true;
          }
        }
        else{
          Scene->removeItem(Connection);
          delete Connection;
          Connection = nullptr;
          return true;
        }
      }
      else{ // the dragged block is left
        if (Selected_block){

          QPointF P=me->scenePos();
          int x=round(P.x()/Step_width);
          int y=round(P.y()/Step_height);

          if (x<1) x=1;
          if (y<0) y=0;

          if (Objects[y][x]!=false){
            x--;
            do{
              x++;
              y=0;
              while (Objects[y][x]!=false && y<x){
                y++;
              }
            } while (Objects[y][x]!=false);
          }

          Selected_block->set_mat_pos(y,x);

          redraw_paths();

          Changed=true;

          Selected_block=nullptr;
        }
      }
    }
    break;
  case QEvent::GraphicsSceneMouseDoubleClick:
    {
      if (me->button() == Qt::LeftButton){
        QPointF P=me->scenePos();
        int Col=round(P.x()/Step_width);
        int Row=round(P.y()/Step_height);
        if ((Col==0 && Row>1) || Col>0){
          QGraphicsItem *item = item_at(me->scenePos());

          if (item && item->type() == _ne_block::Type){
            _ne_block *Block=static_cast<_ne_block *>(item);
            QString text = QInputDialog::getText(me->widget(),tr("Edit Text"), tr("Enter new text:"),QLineEdit::Normal, Block->name());
            if (!text.isEmpty()) Block->name(text);
            Block->update();
            Changed=true;
          }
        }
      }
      break;
    }
  case QEvent::KeyRelease:
    {
      QKeyEvent* key = static_cast<QKeyEvent*>(Event);
      switch(key->key()){
      case Qt::Key_Delete:
        {
          QGraphicsItem *item = item_selected();
          if (item && (item->type() == _ne_block::Type)){
            _ne_block *Block=static_cast<_ne_block *>(item);
            std::string Name=Block->name().toStdString();

            QPointF P=Block->pos();
            int x=round(P.x()/Step_width);
            int y=round(P.y()/Step_height);

            Objects[y][x]=false;
            List_blocks.remove(Block);
            Map_filters_parameters.erase(Name);
            delete Block;
            redraw_paths();
            Changed=true;
          }
          break;
        }
      default:
        break;
      }
    }
    break;
  default:break;
  }
  return QObject::eventFilter(Object, Event);
}


/*****************************************************************************//**
 * Computes the positions of the path for each block taking into account the space between columns and rows and the positions of the blocks
 * The idea is to compute the number of paths that are in each row and col. This helps to compute how to share the space between different connections
 * Also, each conection is asigned a position, from 0 to m, being m the number of connections
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::redraw_paths()
{
  clear_vectors();
  // traverse the blocks
  _ne_port *Port1;
  _ne_port *Port2;
  QVector<_ne_connection*> Connections;
  _ne_block *Block2;
  int Row1,Col1,Row2,Col2;
  for (auto *Block1: List_blocks){
    // get output port
    Port1=Block1->out_port();
    // get the mat position
    Row1=Block1->get_mat_row();
    Col1=Block1->get_mat_col();
    // get connections (usually one)
    Connections=Port1->connections();
    // for each connection
    for (auto *Connection: Connections){
      // get the second port, the por to which is connected the block1 as input
      Port2=Connection->port2();
      // get the block of the port. This the input block of block1
      Block2=Port2->block();
      // now we get the mat position
      Row2=Block2->get_mat_row();
      Col2=Block2->get_mat_col();
      // compute the indices for the path
      compute_path_indices(Row1,Col1,Row2,Col2,Connection);
    }
  }

  for (auto *Block1: List_blocks){
    // get output port
    Port1=Block1->out_port();
    // get connections (usually one)
    Connections=Port1->connections();
    // for each connection
    for (auto *Connection: Connections){
      // update the path
      Connection->update_final_path();
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::compute_path_indices(int Row1,int Col1,int Row2,int Col2,_ne_connection* Connection)
{
  int Colum;

  if (Row1==Row2){ // the same row
    if (Col2>Col1){ // the second block is at the right of block1
      if ((Col2-Col1)==1){ // the blocks are consecutive, nothing to do  B1->B2
        if (Col1>=0){
          Colum=increment_col(Col1);
          Connection->set_col1(Col1,Colum);
          Connection->set_col2(Col1,Colum);
          Connection->set_row(-1,-1); // direct connection
        }
      }
      else{ // the blocks are not consecutive
        // check if there is a block in between
        bool Found=false;
        for (int i=Col1+1;i<Col2;i++){
          if (Objects[Row1][i]!=false) Found=true;
        }

        if (Found==false){ // direct connetion B1-->B2
          if (Col1>=0){
            Colum=increment_col(Col1);
            Connection->set_col1(Col1,Colum);
            Connection->set_col2(Col1,Colum);
            Connection->set_row(-1,-1);
          }
        }
        else{ // the conection is done throug the row below the block
          // B1-   B  ->B2
          //    |----|
          if (Col1>=0 && (Col2-1)>=0){
            Connection->set_col1(Col1,increment_col(Col1));
            Connection->set_col2(Col2-1,increment_col(Col2-1));
            Connection->set_row(Row1,increment_row(Row1));
          }
        }
      }
    }
    else{ // the second block is at the left of block1
      if ((Col1-Col2)==1){ // the blocks are consecutive,
        //   ->b2   b1-
        //  |----------|
        if (Col1>=0 && (Col2-1)>=0){
          Connection->set_col1(Col1,increment_col(Col1));
          Connection->set_col2(Col2-1,increment_col(Col2-1));
          Connection->set_row(Row1,increment_row(Row1));
        }
      }
      else{ // the blocks are not consecutive
        //   ->b2   b1-
        //  |----------|
        if (Col1>=0 && (Col2-1)>=0){
          Connection->set_col1(Col1,increment_col(Col1));
          Connection->set_col2(Col2-1,increment_col(Col2-1));
          Connection->set_row(Row1,increment_row(Row1));
        }
      }
    }
  }
  else{ // rows are different
    if (Row1<Row2){ // block1 is up block2 down
      if (Col1==Col2){ // in the same col
        //   b1-
        //   ---|
        //  |->b2
        if (Col1>=0 && (Col2-1)>=0){
          Connection->set_col1(Col1,increment_col(Col1));
          Connection->set_col2(Col2-1,increment_col(Col2-1));
          Connection->set_row(Row1,increment_row(Row1));
        }
      }
      else{ // different columns
        if (Col1<Col2){// the second block is at the right of block1
          if ((Col2-1)==Col1){ // only onw col of difference
            //   b1-
            //      |
            //      |->b2
            if (Col1>=0){
              Colum=increment_col(Col1);
              Connection->set_col1(Col1,Colum);
              Connection->set_col2(Col2-1,Colum);
              Connection->set_row(Row1,increment_row(Row1));
            }
          }
          else{
            //   b1-
            //      |---
            //          |->b2
            if (Col1>=0 && (Col2-1)>=0){
              Connection->set_col1(Col1,increment_col(Col1));
              Connection->set_col2(Col2-1,increment_col(Col2-1));
              Connection->set_row(Row1,increment_row(Row1));
            }
          }
        }
        else{ // the second block is at the left of block1
          //          b1-
          //  ----------|
          //  |->b2
          if (Col1>=0 && (Col2-1)>=0){
            Connection->set_col1(Col1,increment_col(Col1));
            Connection->set_col2(Col2-1,increment_col(Col2-1));
            Connection->set_row(Row1,increment_row(Row1));
          }
        }
      }
    }
    else{ // block2 is up
      if (Col1==Col2){ // in the same col
        //   ->b2
        //  |-----|
        //     b1-
        if (Col1>=0 && (Col2-1)>=0){
          Connection->set_col1(Col1,increment_col(Col1));
          Connection->set_col2(Col2-1,increment_col(Col2-1));
          Connection->set_row(Row2,increment_row(Row2));
        }
      }
      else{ // different columns
        if (Col1<Col2){// the second block is at the right of block1
          if ((Col2-1)==Col1){ // one colum od difference
            //        ->b2
            //        |
            //     b1-
            if (Col1>=0){
              Colum=increment_col(Col1);
              Connection->set_col1(Col1,Colum);
              Connection->set_col2(Col2-1,Colum);
              Connection->set_row(Row2,increment_row(Row2));
            }
          }
          else{
            //               ->b2
            //        |-----|
            //     b1-
            if (Col1>=0 && (Col2-1)>=0){
              Connection->set_col1(Col1,increment_col(Col1));
              Connection->set_col2(Col2-1,increment_col(Col2-1));
              Connection->set_row(Row2,increment_row(Row2));
            }
          }
        }
        else{ // the second block is at the left of block1
          //   ->b2
          //  |-------------|
          //             b1-
          if (Col1>=0 && (Col2-1)>=0){
            Connection->set_col1(Col1,increment_col(Col1));
            Connection->set_col2(Col2-1,increment_col(Col2-1));
            Connection->set_row(Row2,increment_row(Row2));
          }
        }
      }
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

const int MIN_INDEX_GRID_LINES=-10;
const int MAX_INDEX_GRID_LINES=50;
const int MAX_LINE_VALUE=10000;

void _nodes_editor::add_grid()
{
  QPen Pen;
  Pen.setColor(QColor(_ne_common_ns::Gray.Red,_ne_common_ns::Gray.Green,_ne_common_ns::Gray.Blue));
  QGraphicsLineItem *Line;

  for (int i=_ne_common_ns::MIN_INDEX_GRID_LINES;i<_ne_common_ns::MAX_INDEX_GRID_LINES;i++){
    Line=new QGraphicsLineItem;
    Line->setZValue(-1000);
    Line->setPen(Pen);
    Line->setLine(i*Step_width,-_ne_common_ns::MAX_LINE_VALUE,i*Step_width,_ne_common_ns::MAX_LINE_VALUE);
    Scene->addItem(Line);
  }

  for (int i=_ne_common_ns::MIN_INDEX_GRID_LINES;i<_ne_common_ns::MAX_INDEX_GRID_LINES;i++){
    Line=new QGraphicsLineItem;
    Line->setZValue(-1000);
    Line->setPen(Pen);
    Line->setLine(-_ne_common_ns::MAX_LINE_VALUE,i*Step_height,_ne_common_ns::MAX_LINE_VALUE,i*Step_height);
    Scene->addItem(Line);
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::clear_vectors()
{
  for (unsigned int i=0;i<_ne_common_ns::NUM_MAX_ROWS;i++) Rows[i]=0;
  for (unsigned int i=0;i<_ne_common_ns::NUM_MAX_COLS;i++) Cols[i]=0;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

int _nodes_editor::increment_col(int Col1)
{
  int i;
  if (Col1>=0){
    i=Cols[Col1]++;
  }
  else{
    std::cout << "Error: the index Col1 < 0" << std::endl;
    exit(-1);
  }
  return i;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

int _nodes_editor::increment_row(int Row1)
{
  int i;
  if (Row1>=0){
    i=Rows[Row1]++;
  }
  else{
    std::cout << "Error: the index Row1 < 0" << std::endl;
    exit(-1);
  }
  return i;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::set_mat_objects(int Row,int Col,bool Value)
{
  if (Row>=0 && Row<_ne_common_ns::NUM_MAX_ROWS && Col>=0 && Col<_ne_common_ns::NUM_MAX_COLS) Objects[Row][Col]=Value;
  else{
    std::cout << "Error: in mat object index " << std::endl;
    exit(-1);
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

bool _nodes_editor::get_mat_objects(int Row,int Col)
{
  if (Row>=0 && Row<_ne_common_ns::NUM_MAX_ROWS && Col>=0 && Col<_ne_common_ns::NUM_MAX_COLS) return Objects[Row][Col];
  else{
    std::cout << "Error: in mat object index " << std::endl;
    exit(-1);
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::add_connection(_ne_block *Block_out,_ne_block *Block_in,int Num_port)
{
  Connection = new _ne_connection(this,0);
  Scene->addItem(Connection);
  Connection->set_port1(Block_out->out_port());
  Connection->set_pos1(Block_out->out_port()->scenePos());
  if (Num_port==0){
    Connection->set_port2(Block_in->in_port1());
    Connection->set_pos2(Block_in->in_port1()->scenePos());
  }
  else{
    Connection->set_port2(Block_in->in_port2());
    Connection->set_pos2(Block_in->in_port2()->scenePos());
  }

  Connection->update_path();

  Connection=nullptr;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::read_data_effect(std::string Name)
{
  QJsonObject Filter_object;
  QStringList Key_list;
  std::map<std::string,std::string> Values;

  QFile File(Name.c_str());

  if (!File.open(QIODevice::ReadOnly)) {
      qWarning("Couldn't open json file.");
      exit(-1);
  }

  QByteArray Data=File.readAll();
  // the file is converted to a Json document
  QJsonDocument Document(QJsonDocument::fromJson(Data));
  // A json object is obtained from the document
  QJsonObject Object=Document.object();
  // filters appear as an array
  QJsonArray Filter_array=Object["effect"].toArray();
  // each filter is treated
  for (int i=0;i<Filter_array.size();i++) {
    // the data of each filter is put on an json object
    Filter_object=Filter_array[i].toObject();
    Key_list=Filter_object.keys();

    Values.clear();
    for (int j=0;j<Key_list.size();j++){
      //cout << Key_list[j].toStdString() << " " << Filter_object.value(Key_list[j]).toString().toStdString() << endl;
      Values[Key_list[j].toStdString()]=Filter_object.value(Key_list[j]).toString().toStdString();
    }
    Filters_json_data.push_back(Values);
  }
  File.close();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::create_blocks_from_effect()
{
  std::string Type;
  std::string Name;
  _block Block1;
  _ne_block *Block,*Block_out,*Block_in;

  // used the json data to create the blocks infor
  try
  {
    for (int i=0;i<(int)Filters_json_data.size();i++){

      Type=Filters_json_data[i].at("type_filter");
      Name=Filters_json_data[i].at("output_image_0");

      // now check the name of the input images
      if (Filters_json_data[i].at("input_image_0")=="NULL"){
        std::cout << "Error: the input image of filter " << Type <<" cannot be NULL. At least, it must be COLOR or GRAY" << std::endl;
      }
      else{
        Block1.Name=Name;
        Block1.Type=Type;
        Block1.In1=Filters_json_data[i].at("input_image_0");
        Block1.In2=Filters_json_data[i].at("input_image_1");
        Block1.Row=-1;
        Block1.Col=-1;

        //
        Filters_json_data[i]["_INI_"]="FILE";
        Map_filters_parameters[Name]=Filters_json_data[i];
      }

      Map_data_blocks[Name]=Block1;
    }
  }
  catch (const std::out_of_range& oor) {
    //std::cerr << "Bad index in Filters" << endl;
    QMessageBox MsgBox;
    MsgBox.setText("Error in the name of one or several filters or a final comma");
    MsgBox.exec();
    exit(-1);
  }

  // add the number of blocks to the counter
  Num_filters=Filters_json_data.size();

  // find the final blocks (the have no next one)
  std::vector<std::string> Vec_final_blocks;
  bool Found;

  for (auto &Block_aux: Map_data_blocks){
    Name=Block_aux.second.Name;
    Found=false;
    for (auto &Block_aux1: Map_data_blocks){
      if (Block_aux.first!=Block_aux1.first){
        if (Block_aux1.second.In1==Name || Block_aux1.second.In2==Name){
          Found=true;
          break;
        }
      }
    }
    if (Found==false)
      Vec_final_blocks.push_back(Name);
  }

  // compute the position of each block recursively
  Pos_row=0;
  for (unsigned int i=0;i<Vec_final_blocks.size();i++){
    compute_position(Vec_final_blocks[i]);
  }

  // generate the blocks
  Block=new _ne_block(this,0);
  Scene->addItem(Block);
  Block->create("COLOR","COLOR",0,3);
  Block->set_mat_pos(0,0);
  Block->setFlag(QGraphicsItem::ItemIsMovable,false);
  Block->setFlag(QGraphicsItem::ItemIsSelectable,false);
  List_blocks.push_back(Block);
  Map_blocks["COLOR"]=Block;

  Block=new _ne_block(this,0);
  Scene->addItem(Block);
  Block->create("GRAY","GRAY",0,1);
  Block->set_mat_pos(1,0);
  Block->setFlag(QGraphicsItem::ItemIsMovable,false);
  Block->setFlag(QGraphicsItem::ItemIsSelectable,false);
  List_blocks.push_back(Block);
  Map_blocks["GRAY"]=Block;

  int Num_inputs;
  int Num_channels;
  for (auto &Block_aux: Map_data_blocks){
    Name=Block_aux.second.Name;
    if (Name!="COLOR" && Name!="GRAY"){
      Type=Block_aux.second.Type;
      Block=new _ne_block(this,0);
      Scene->addItem(Block);
      Num_inputs=_f_filter_ns::Filter_name_parameters[Type_filter_name[Type]].Num_input_ports;
      Num_channels=_f_filter_ns::Filter_name_parameters[Type_filter_name[Type]].Num_channels;
      assert(Num_inputs>0);
      Block->create(QString::fromStdString(Type),QString::fromStdString(Name),Num_inputs,Num_channels);
      Block->set_mat_pos(Block_aux.second.Row,Block_aux.second.Col);
      List_blocks.push_back(Block);
      Map_blocks[Name]=Block;
    }
  }

  // generate the connections

  // the information of COLOR and GRAY blocks is added new. If it is added before, it is necessary to control them in the searches
  Block1.Name="COLOR";
  Block1.Type="COLOR";
  Block1.In1="NULL";
  Block1.In2="NULL";
  Map_data_blocks["COLOR"]=Block1;

  Block1.Name="GRAY";
  Block1.Type="GRAY";
  Map_data_blocks["GRAY"]=Block1;

  for (auto &Block_aux: Map_data_blocks){
    Block_out=Map_blocks[Block_aux.first];
    Name=Block_aux.second.Name;
    Found=false;
    for (auto &Block_aux1: Map_data_blocks){
      if (Block_aux.first!=Block_aux1.first){
        // check if the in 1 is equal to the block name then connect
        if (Block_aux1.second.In1==Name){
          Block_in=Map_blocks[Block_aux1.second.Name];
          add_connection(Block_out,Block_in,0);
        }
        // check if the in 2 is equal to the block name then connect
        if (Block_aux1.second.In2==Name){
          Block_in=Map_blocks[Block_aux1.second.Name];
          add_connection(Block_out,Block_in,1);
        }
      }
    }
  }

  redraw_paths();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

bool _nodes_editor::find_root(_ne_block *Block1)
{
  _ne_block *Block_aux;

  if (Block1->name()=="COLOR" || Block1->name()=="GRAY") return true;
  else{
    // get the name of the block that is connected to port1, otherwise return ""
    Block_aux=Block1->get_block_in_port1();
    if (Block_aux==nullptr) return false;
    else{
      if (find_root(Block_aux)==false) return false;
      else{
        if (Block1->num_inputs()==2){
          Block_aux=Block1->get_block_in_port2();
          if (Block_aux==nullptr) return false;
          else{
            if (find_root(Block_aux)==false) return false;
            else return true;
          }
        }
        else return true;
      }
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

bool _nodes_editor::create_blocks_from_editor()
{
  std::string Type;
  std::string Name;
  _block Block1;
  _ne_block *Block,*Block_in;

  // first all the data is cleared
  Map_data_blocks.clear();

  // check if there is any block that is not connected to the color or gray block
  bool Found=false;
  for (std::list<_ne_block *>::iterator Block_aux=List_blocks.begin();Block_aux!=List_blocks.end();Block_aux++){
    Found=find_root(*Block_aux);
    if (Found==false) break;
  }

  if (Found==false){
    // error
    QMessageBox::warning(this, tr("Error"),tr("Error: there are blocks that are not connected"), QMessageBox::Ok);
    return false;
  }

  // used the _ne_blocks data to create the _blocks infor
  for (std::list<_ne_block *>::iterator Block_aux=List_blocks.begin();Block_aux!=List_blocks.end();Block_aux++){
    // get the block
    Block=*Block_aux;

    // only the added filters are included
    if (Block->name().toStdString()!="COLOR" && Block->name().toStdString()!="GRAY"){
      Block1.Name=Block->name().toStdString();
      Block1.Type=Block->block_type().toStdString();
      // first port
      Block_in=Block->get_block_in_port1();
      if (Block_in==nullptr) Block1.In1="NULL";
      else Block1.In1=Block_in->name().toStdString();
      // second port
      Block_in=Block->get_block_in_port2();
      if (Block_in==nullptr) Block1.In2="NULL";
      else Block1.In2=Block_in->name().toStdString();
      Block1.Row=-1;
      Block1.Col=-1;

      Map_data_blocks[Block1.Name]=Block1;
    }
  }

  // the structure is ok. Now it is necessary to establish the correct order of the blocks to run correctly:
  // all the previous connected blocks must be computed to update one

  // find the final blocks (they have no next one)
  std::vector<std::string> Vec_final_blocks;

  for (auto &Block_aux: Map_data_blocks){
    Name=Block_aux.second.Name;
    Found=false;
    for (auto &Block_aux1: Map_data_blocks){
      if (Block_aux.first!=Block_aux1.first){
        if (Block_aux1.second.In1==Name || Block_aux1.second.In2==Name){
          Found=true;
          break;
        }
      }
    }
    if (Found==false)
      Vec_final_blocks.push_back(Name);
  }

  // compute the position of each block recursively
  Pos_row=0;
  for (unsigned int i=0;i<Vec_final_blocks.size();i++){
    compute_position(Vec_final_blocks[i]);
  }

  // now compute the linear position
  // ugly method but I need to finish asap
  std::string Mat_names[_ne_common_ns::NUM_MAX_ROWS][_ne_common_ns::NUM_MAX_COLS];
  for (unsigned int Row=0;Row<_ne_common_ns::NUM_MAX_ROWS;Row++){
    for (unsigned int Col=0;Col<_ne_common_ns::NUM_MAX_COLS;Col++){
      Mat_names[Row][Col]="";
    }
  }

  // get the information of each block
  int Max_col=0;
  for (auto &Block_aux: Map_data_blocks){
    Mat_names[Block_aux.second.Row][Block_aux.second.Col]=Block_aux.second.Name;
    if (Block_aux.second.Col>Max_col) Max_col=Block_aux.second.Col;
  }

  // move by columns to get the correct order
  Vec_order.clear();

  for (unsigned int Col=0;Col<_ne_common_ns::NUM_MAX_COLS;Col++){
    for (unsigned int Row=0;Row<_ne_common_ns::NUM_MAX_ROWS;Row++){
      if (Mat_names[Row][Col]!="") Vec_order.push_back(Mat_names[Row][Col]);
    }
  }
  return true;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_position _nodes_editor::compute_position(std::string Name)
{
  _block &Block=Map_data_blocks[Name];
  _position Position1;
  _position Position2;

  if (Block.In2=="NULL"){ // only one connection
    if (Block.In1=="COLOR" || Block.In1=="GRAY"){ // the end of the chain
      Position1.Row=Pos_row;
      Position1.Col=1;
      Pos_row++;
      Block.Row=Position1.Row;
      Block.Col=Position1.Col;
      return Position1;
    }
    else{// not the end -> compute recursively
      Position1=compute_position(Block.In1);
    }
    Position1.Col++;
    Block.Row=Position1.Row;
    Block.Col=Position1.Col;
    return Position1;
  }
  else{// there are two connections
    if (Block.In1=="COLOR" || Block.In1=="GRAY"){ // the end of the chain
      Position1.Row=Pos_row;
      Position1.Col=1;
    }
    else Position1=compute_position(Block.In1);

    if (Block.In2=="COLOR" || Block.In2=="GRAY"){ // the end of the chain
      Position2.Row=Pos_row;
      Position2.Col=1;
    }
    else Position2=compute_position(Block.In2);

		if (Position1.Col == Position2.Col && Position1.Row == Position2.Row) { // the 2 inputs are the same
			Block.Row = Position1.Row;
			Block.Col = Position1.Col;
			return Position1;
		}
		else {
			if (Position1.Col >= Position2.Col) {
				Position1.Col++;
				if (Position2.Row >= Position1.Row) Position1.Row = Position2.Row;
				Block.Row = Position1.Row;
				Block.Col = Position1.Col;
				return Position1;
			}
			else {
				Position2.Col++;
				if (Position1.Row >= Position2.Row) Position2.Row = Position1.Row;
				Block.Row = Position2.Row;
				Block.Col = Position2.Col;
				return Position2;
			}
		}
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::add_block(std::string Type,std::string Name,int Num_parameters,int Num_imput_ports, unsigned char Num_channels1)
{
  Q_UNUSED(Num_parameters)
  if (get_mat_objects(2,0)==false){
    Changed=true;
    std::string Name1=Name+"_"+std::to_string(Num_filters++);

    _ne_block *Block = new _ne_block(this,0);
    Scene->addItem(Block);
    Block->create(QString::fromStdString(Type),QString::fromStdString(Name1),Num_imput_ports,Num_channels1);
    Block->set_mat_pos(2,0);
    List_blocks.push_back(Block);

    std::map<std::string,std::string> Map_aux;
    Map_aux["_INI_"]="EDITOR";
    Map_filters_parameters[Name1]=Map_aux;

  }
  else{
     QMessageBox::warning(this, tr("Warning"),tr("Please, move the block before add a new one"), QMessageBox::Ok);
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::add_block(std::string Type, std::string Name, int Num_parameters, int Num_imput_ports, unsigned char Num_channels1, float Col, float Row)
{
  Q_UNUSED(Num_parameters)

  int Col1=round(Col/Step_width);
  int Row1=round(Row/Step_height);

  if (get_mat_objects(Row1,Col1)==false){
    Changed=true;
    std::string Name1=Name+"_"+std::to_string(Num_filters++);

    _ne_block *Block = new _ne_block(this,0);
    Scene->addItem(Block);
    Block->create(QString::fromStdString(Type),QString::fromStdString(Name1),Num_imput_ports,Num_channels1);
    Block->set_mat_pos(Row1,Col1);
    List_blocks.push_back(Block);

    std::map<std::string,std::string> Map_aux;
    Map_aux["_INI_"]="EDITOR";
    Map_filters_parameters[Name1]=Map_aux;

  }
  else{
     QMessageBox::warning(this, tr("Warning"),tr("Please, move the block to a unoccupied place"), QMessageBox::Ok);
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::add_initial_blocks()
{
  _ne_block *Block;

  // generate the blocks
  Block=new _ne_block(this,0);
  Scene->addItem(Block);
  Block->create("COLOR","COLOR",0,3);
  Block->set_mat_pos(0,0);
  Block->setFlag(QGraphicsItem::ItemIsMovable,false);
  Block->setFlag(QGraphicsItem::ItemIsSelectable,false);
  List_blocks.push_back(Block);
  Map_blocks["COLOR"]=Block;

  Block=new _ne_block(this,0);
  Scene->addItem(Block);
  Block->create("GRAY","GRAY",0,1);
  Block->set_mat_pos(1,0);
  Block->setFlag(QGraphicsItem::ItemIsMovable,false);
  Block->setFlag(QGraphicsItem::ItemIsSelectable,false);
  List_blocks.push_back(Block);
  Map_blocks["GRAY"]=Block;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::remove_block(_ne_block *Block)
{
  std::string Name=Block->name().toStdString();

  int x=Block->get_mat_col();
  int y=Block->get_mat_row();

  // remove the object from the matrix
  Objects[y][x]=false;
  // remove one
  if (Block->num_inputs()==1) Cols[x-1]--;
  if (Block->num_inputs()==2) Cols[x-1]=Cols[x-1]-2;

  List_blocks.remove(Block);
  Map_filters_parameters.erase(Name);
  delete Block;
  redraw_paths();
  Changed=true;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _nodes_editor::remove_connection(_ne_connection *Connection1)
{
  delete Connection1;
  redraw_paths();
  Changed=true;
}
