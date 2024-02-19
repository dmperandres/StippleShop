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


#include "glwidget.h"

using namespace _f_gl_widget_ns;
using namespace _f_filter_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

int _gl_widget::Counter_draw=0;

_gl_widget::_gl_widget(_window *Window1, QWidget *Parent)
: QOpenGLWidget(Parent)
{
  Window=Window1;

  // controls if an image is loaded
  Image_loaded=false;

  // controls if the effect has been read
  Effect_ready=false;

  // hiding widgets
  for(int i=0; i<Filters.size();i++){
    Filters.get_data(i)->hide();
  }

  // create tabs
  Images_tab=new _images_tab(this);
  Filters_tab=new _filters_tab(this);

  // scale and tranlation values for each image
  // it must be initialized
  Scale_drawing_mode.resize(NUM_TABS);
  Translation.resize(NUM_TABS);

  // for tab 0 y 3
  for (unsigned int i=0;i<1;i++){
    Scale_drawing_mode[0].push_back(1.0);
    Translation[0].push_back(_vertex2f(0,0));
    Scale_drawing_mode[3].push_back(1.0);
    Translation[3].push_back(_vertex2f(0,0));
    }

  Selected_image_index=0;
  Selected_filter_index=0;
  Selected_dots_index=0;
  Selected_dot_design_index=0;

  Selected_tab=0;
  Selected_index=0;

  Painting=false;
  Drawn=false;

  // it is necessary the ppi level!!!!
  // for the 5 textures packets of dots
  Dots.resize(3); // 3 ppi level
  for (int i=0;i<3;i++){
    Dots[i].resize(9);
    for (int j=4;j<=8;j++){
      Dots[i][j]=new cv::Mat;
    }
  }

  read_dots();

  Active_dots_tabs=false;
  Image_loaded=false;

  Vec_images.resize(3);

  Image_cv_gray=make_shared<cv::Mat>();
  Image_cv_gray1=make_shared<cv::Mat>();
  Image_cv_color=make_shared<cv::Mat>();
  Frame_buffer=make_shared<cv::Mat>();
  Original_image=make_shared<cv::Mat>();

}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_gl_widget::~_gl_widget()
{

  #ifdef DEFINE_FILTER_EXAMPLE_BASED_STIPPLING
  // delete images of dots
  #ifdef FULL
  for (int i=0;i<_f_stippling_ebg::LAST_RESOLUTION;i++){
  #else
  for (int i=_f_stippling_ebg::RESOLUTION_300PPI;i<_f_stippling_ebg::RESOLUTION_600PPI;i++){
  #endif
    for (int j=_f_stippling_ebg::MIN_DOT_SIZE;j<=_f_stippling_ebg::MAX_DOT_SIZE;j++){
      delete Dots[i][j];
    }
  }
  #endif

  clear_data();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::clear_data()
{
  Filters.clear();
  Images.clear();
  Graph.clear();
  Pipeline.clear();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void  _gl_widget::read_image(string File_name)
{
  // the original image is read
  _image_IO Image_io;

  // hide the widget meanwhile there are changes
  hide();

  Image_io.read_image(File_name,*Original_image.get());

  int Width_aux=Original_image->cols;
  int Height_aux=Original_image->rows;

  adjust_image_sizes(Width_aux,Height_aux);
  if (Width_aux!=Original_image->cols || Height_aux!=Original_image->rows){
      QMessageBox MsgBox;
      MsgBox.setText("The size of the input image is not even. Adjusted to the nearest value (cubic interpolation)");
      MsgBox.exec();
    cv::resize(*Original_image.get(),*Original_image.get(),cv::Size(Width_aux,Height_aux),0,0,cv::INTER_CUBIC);
  }

  // flip the original image to adapt it to OpenGL
  cv::flip(*Original_image.get(),*Original_image.get(),0);

  if (Original_image->channels()==1){
    //  Gray
    *Image_cv_gray.get()=Original_image->clone();
    Image_cv_color->release();
    Image_cv_color->create(Original_image->rows,Original_image->cols,CV_8UC3);
    Frame_buffer->release();
    Frame_buffer->create(Original_image->rows,Original_image->cols,CV_8UC3);

    // The input image is copied to the frame-buffer as an initial value
    cvtColor(*Image_cv_gray.get(),*Image_cv_color.get(),cv::COLOR_GRAY2RGB,3);
    // The input image is copied to the frame-buffer as an initial value
    cvtColor(*Original_image.get(),*Frame_buffer.get(),cv::COLOR_GRAY2BGR,3);
  }
  else{
    // Color
    Image_cv_color->release();
    *Image_cv_color.get()=Original_image->clone();
    Image_cv_gray->release();
    Image_cv_gray->create(Original_image->rows,Original_image->cols,CV_8U);

    // the image is converted to gray escale
    cvtColor(*Image_cv_color.get(),*Image_cv_gray.get(),cv::COLOR_BGR2GRAY,1);
    // The input image is copied to the frame-buffer as an initial value
    *Frame_buffer.get()=Original_image->clone();
    //cvtColor(Original_image,Frame_buffer,CV_BGR2BGRA,4);
  }

  show();

  if (Image_loaded==false){
    Image_loaded=true;
    create_filters_from_blocks();
    refresh_image();
  }
  else{
    update_all_filters();
    refresh_image();
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void  _gl_widget::save_image(string File_name)
{
  _image_IO Image_io;
  cv::Mat Aux_image;

  Aux_image=Images.get_data(Selected_image)->clone();

  cv::flip(Aux_image,Aux_image,0);
  Image_io.write_image(File_name,Aux_image);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::read_dots()
{
  string File_name;
  _image_IO Image_io;
  char Name[200];

  QDir Directory;
  File_name=Directory.currentPath().toStdString();

//  printf("Reading dots files\n");
  for (unsigned int i=_f_gl_widget_ns::RESOLUTION_300PPI;i<_f_gl_widget_ns::LAST_RESOLUTION;i++){
    for (unsigned int j=_f_gl_widget_ns::MIN_DOT_SIZE;j<=_f_gl_widget_ns::MAX_DOT_SIZE;j++){
      // to resolve the problem with absolute paths
      sprintf(Name,"/dots/dots_%d_%d.pgm",j,_f_gl_widget_ns::VEC_RESOLUTION[i]);
      string File_name_aux=File_name+string(Name);
      Image_io.read_image(File_name_aux,*Dots[i][j]);
      cv::flip(*Dots[i][j],*Dots[i][j],0);
      if (Dots[i][j]->channels()!=1){
        cout << "Error: number of channels incorrect in dots files" << endl;
        exit(-1);
      }
    }
  }

  // tab2
  Scale_drawing_mode[2].resize(5);
  Translation[2].resize(5);
  for (int i=0;i<5;i++){
    Scale_drawing_mode[2][i]=1.0;
    Translation[2][i]=_vertex2f(0,0);
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::mousePressEvent(QMouseEvent *Event)
{
  if (Event->buttons() & Qt::LeftButton) {
    Change_pos=true;
    Initial_position_X=Event->position().x();
    Initial_position_Y=Event->position().y();
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::mouseReleaseEvent(QMouseEvent *Event)
{
  Q_UNUSED(Event)
  Change_pos=false;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::mouseMoveEvent(QMouseEvent *Event)
{
  int Last_position_X=Event->position().x();
  int Last_position_Y=Event->position().y();

  if (Change_pos==true){
    Translation[Selected_tab][Selected_index].x+=Last_position_X-Initial_position_X;
    Translation[Selected_tab][Selected_index].y+=Initial_position_Y-Last_position_Y;
    Initial_position_X=Last_position_X;
    Initial_position_Y=Last_position_Y;
    }

  Drawn=false;

  update();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::wheelEvent(QWheelEvent *Event)
{
  QPoint Num_degrees = Event->angleDelta();
  int Step = Num_degrees.y() / 120;

  if (Step<0) Scale_drawing_mode[Selected_tab][Selected_index]/=1.2;
  else Scale_drawing_mode[Selected_tab][Selected_index]*=1.2;

  update();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::mouseDoubleClickEvent(QMouseEvent *Event)
{
  if (Event->buttons() & Qt::LeftButton) {
    Translation[Selected_tab][Selected_index].x=0;
    Translation[Selected_tab][Selected_index].y=0;
    update();
  }
  if (Event->buttons() & Qt::RightButton) {
    Scale_drawing_mode[Selected_tab][Selected_index]=1.0;
    update();
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::initializeGL()
{
  const GLubyte* strm;
  GLenum err;

  strm = glGetString(GL_VENDOR);
  std::cerr << "Vendor: " << strm << "\n";
  strm = glGetString(GL_RENDERER);
  std::cerr << "Renderer: " << strm << "\n";
  strm = glGetString(GL_VERSION);
  std::cerr << "OpenGL Version: " << strm << "\n";
  strm = glGetString(GL_SHADING_LANGUAGE_VERSION);
  std::cerr << "GLSL Version: " << strm << "\n";

  //Glew init:
  err = glewInit();
  if (GLEW_OK != err){
    // Problem: glewInit failed, something is seriously wrong.
    std::cerr << "Error: " << glewGetErrorString(err) << "\n";
    QMessageBox::warning(this, tr("Warning"),tr("glewInit failed"));
    exit (-1);
  }

  std::cerr << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << "\n";
  if (!glewGetExtension("GL_ARB_texture_non_power_of_two")){
    std::cerr << "Warning: ARB_texture_non_power_of_two may not be supported" << "\n";
    QMessageBox::warning(this, tr("Warning"),tr("ARB_texture_non_power_of_two may not be supported"));
    exit (-1);
  }

  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &Max_texture_size);

  Change_pos=false;

  _shaders Shader;

  // initial image: a gray square
  int Width1=512;
  int Height1=512;

  QImage Image1(Width1,Height1,QImage::Format_RGB888);
  Image1.fill(QColor(240,0,0));

  Program1=Shader.load_shaders("src/shaders/basic.vert","src/shaders/basic.frag");
  if (Program1==0){
    exit(-1);
  }
  // adjust the sizes

  glCreateTextures(GL_TEXTURE_2D,1,&Texture1);
  glTextureStorage2D(Texture1,1,GL_RGB8,Width1,Height1);

  glBindTexture(GL_TEXTURE_2D,Texture1);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // fill with data
  glTextureSubImage2D(Texture1,0,0,0,Width1,Height1,GL_RGB,GL_UNSIGNED_BYTE,Image1.constBits());

  Vertices.resize(6);
  Tex_coordinates.resize(6);

  Vertices[0]=_vertex3f(-Width1/2, -Height1/2, 0.);
  Vertices[1]=_vertex3f(Width1/2, -Height1/2, 0.);
  Vertices[2]=_vertex3f(-Width1/2, Height1/2, 0.);
  Vertices[3]=_vertex3f(Width1/2, -Height1/2, 0.);
  Vertices[4]=_vertex3f(Width1/2, Height1/2, 0.);
  Vertices[5]=_vertex3f(-Width1/2, Height1/2, 0.);

  Tex_coordinates[0]=_vertex2f(0.0,0.0);
  Tex_coordinates[1]=_vertex2f(1.0,0.0);
  Tex_coordinates[2]=_vertex2f(0.0,1.0);
  Tex_coordinates[3]=_vertex2f(1.0,0.0);
  Tex_coordinates[4]=_vertex2f(1.0,1.0);
  Tex_coordinates[5]=_vertex2f(0.0,1.0);

  glCreateVertexArrays(1,&VAO1);
  glBindVertexArray(VAO1);

  // vertices coordinates
  // create
  glCreateBuffers(1,&VBO1_1);
  // reserve espace (and initialice; if use NULL there is not initialization)
  glNamedBufferStorage(VBO1_1,Vertices.size()*3*sizeof(float),&Vertices[0],GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
  // bind to VAO
  glVertexArrayVertexBuffer(VAO1,0,VBO1_1,0,3*sizeof(float));
  // state the format of the data
  glVertexArrayAttribFormat(VAO1,1,3,GL_FLOAT,GL_FALSE,0);
  // state what vertex buffer binding ot use for this attribute
  glVertexArrayAttribBinding(VAO1,1,0);
  // enable the attribute
  glEnableVertexAttribArray(1);

  // texture coordinates
  glCreateBuffers(1,&VBO1_2);
  // reserve espace (and initialice; if use NULL there is not initialization)
  glNamedBufferStorage(VBO1_2,Tex_coordinates.size()*2*sizeof(float),&Tex_coordinates[0],GL_DYNAMIC_STORAGE_BIT);
  // bind to VAO
  glVertexArrayVertexBuffer(VAO1,1,VBO1_2,0,2*sizeof(float));
  // state the format of the data
  glVertexArrayAttribFormat(VAO1,2,2,GL_FLOAT,GL_FALSE,0);
  // state what vertex buffer binding ot use for this attribute
  glVertexArrayAttribBinding(VAO1,2,1);
  // enable the attribute
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::paintGL()
{
  glViewport(0,0,width(),height());
  draw_objects();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::resizeGL(int width, int height)
{
  Window_width=width;
  Window_height=height;

  glViewport(0,0,width,height);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::draw_objects()
{

  QMatrix4x4 Model_view;

  Model_view.ortho(-Window_width*Scale_drawing_mode[Selected_tab][Selected_index]/2,Window_width*Scale_drawing_mode[Selected_tab][Selected_index]/2,-Window_height*Scale_drawing_mode[Selected_tab][Selected_index]/2, Window_height*Scale_drawing_mode[Selected_tab][Selected_index]/2,-100,100);
  Model_view.translate(Translation[Selected_tab][Selected_index].x,Translation[Selected_tab][Selected_index].y,0);

  glViewport(0,0,Window_width,Window_height);
  glClearColor(0.8,0.8,0.8,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  //  glPointSize(10);
  glUseProgram(Program1);
  glBindVertexArray(VAO1);

  glBindTexture(GL_TEXTURE_2D,Texture1);

  glUniformMatrix4fv(0,1,GL_FALSE,Model_view.data());

  glDrawArrays(GL_TRIANGLES,0,6);

  glBindVertexArray(0);
  glUseProgram(0);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::refresh_image()
{
  if (Image_loaded){
    if (Images.get_data(Selected_image_index)->channels()==1){
      cv::cvtColor(*Images.get_data(Selected_image_index).get(),*Frame_buffer.get(),cv::COLOR_GRAY2BGR,3);
    }
    else{
      Images.get_data(Selected_image_index)->copyTo(*Frame_buffer.get());
    }

    int Width1=Frame_buffer->cols;
    int Height1=Frame_buffer->rows;

    glDeleteTextures(1,&Texture1);

    glCreateTextures(GL_TEXTURE_2D,1,&Texture1);
    glTextureStorage2D(Texture1,1,GL_RGB8,Width1,Height1);

    glBindTexture(GL_TEXTURE_2D,Texture1);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // fill with data
    glTextureSubImage2D(Texture1,0,0,0,Width1,Height1,GL_BGR,GL_UNSIGNED_BYTE,Frame_buffer->data);

    Vertices[0]=_vertex3f(-Width1/2, -Height1/2, 0.);
    Vertices[1]=_vertex3f(Width1/2, -Height1/2, 0.);
    Vertices[2]=_vertex3f(-Width1/2, Height1/2, 0.);
    Vertices[3]=_vertex3f(Width1/2, -Height1/2, 0.);
    Vertices[4]=_vertex3f(Width1/2, Height1/2, 0.);
    Vertices[5]=_vertex3f(-Width1/2, Height1/2, 0.);

    glBindVertexArray(VAO1);
    // uptdate vertices coordinates
    glNamedBufferSubData(VBO1_1,0,Vertices.size()*3*sizeof(float),&Vertices[0]);

    glBindVertexArray(0);

    update();
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::refresh_image(cv::Mat *Image)
{
  if (Image->channels()==1){
    cv::cvtColor(*Image,*Frame_buffer.get(),cv::COLOR_GRAY2BGR,3);
  }
  else{
    Image->copyTo(*Frame_buffer.get());
  }

  glTexImage2D(GL_TEXTURE_2D,0,3,Frame_buffer->cols,Frame_buffer->rows,0,GL_BGR,GL_UNSIGNED_BYTE,Frame_buffer->data);

  Drawn=false;

  update();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::update_all_filters()
{
  for (unsigned int i=0;i<(*Vec_order).size();i++){
    Filters.get_data((*Vec_order)[i])->update();
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::show_images()
{
  Images_tab->clear();
  for (int i=0;i<Images.size();i++){
    Images_tab->set_item(Images.get_string(i));
  }
  Images_tab->set_selected_item(Selected_image_index);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::show_filters()
{
  Filters_tab->clear();
  for (int i=0;i<Filters.size();i++){
    Filters_tab->set_item(Filters.get_data(i)->name());
    // hide the widget
    Filters.get_data(i)->hide();
  }
  Filters_tab->set_selected_item(Selected_filter_index);
  // show the selected widget
  Filters.get_data(Selected_filter_index)->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::set_image(string Image_name)
{
  Selected_image=Image_name;
  Selected_image_index=Images.get_position(Image_name);
  Selected_index=Selected_image_index;
  // show the selected image
  Images_tab->set_selected_item(Selected_image_index);

  refresh_image();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::set_filter(string Filter_name)
{
  // to hide all the filters of the effect
  for (int i=0;i<Filters.size();i++){
    // hide the widget
    Filters.get_data(i)->hide();
  }

  Selected_filter=Filter_name;
  Selected_filter_index=Filters.get_position(Filter_name);
  // show the selected filter
  Filters.get_data(Selected_filter_index)->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::update_aux(string Name)
{

  Filters.get_data(Name)->update();
  if (Graph.find(Name)!=Graph.end()){
    // the element exist
    for (auto &Iterator: Graph[Name]){
      update_aux(Iterator);
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::update_effect(string Name)
{
  // update all the affected filters
  update_aux(Name);

  refresh_image();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::read_data_effect(string Name)
{
  QJsonObject Filter_object;
  QStringList Key_list;
  map<string,string> Values;

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

void _gl_widget::write_data_effect(string Name)
{
  QJsonObject Filter_object;
  QStringList Key,Parameter;
  map<std::string,std::string> Values;
  string Name_filter;
  _block Block;

  QFile File(Name.c_str());

  if (!File.open(QIODevice::WriteOnly)) {
      qWarning("Couldn't open json file.");
      exit(-1);
  }

  QJsonArray Filter_array;

  // for each filter
  for (unsigned int i=0;i<Pipeline.size();i++) {
    // the data of each filter is put on an json object
    Values.clear();
    Pipeline[i]->write_parameters(Values);

    Name_filter=(*Vec_order)[i];
    Block=(*Map_data_blocks)[Name_filter];

    QJsonObject Json_object;

    Json_object["type_filter"]=QString::fromStdString(Block.Type);
    Json_object["input_image_0"]=QString::fromStdString(Block.In1);
    Json_object["output_image_0"]=QString::fromStdString(Name_filter);
    Json_object["input_image_1"]=QString::fromStdString(Block.In2);
    for (auto &Position: Values){
      Json_object[QString(Position.first.c_str())]=QString(Position.second.c_str());
    }
    Filter_array.append(Json_object);
  }
  Filter_object["effect"]=Filter_array;
  QJsonDocument Document(Filter_object);
  File.write(Document.toJson());
  File.close();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::read_effect(string Name)
{
  if (Effect_ready==false){
    read_data_effect(Name);
    Effect_ready=true;
  }
  else{
    if (Image_loaded==true){ // there is one loaded image but with other effect
      hide();
        // sets initial tab
      Window->set_tab(0);
      // the output images are deleted (they have different size)
      Images.clear();
      Filters.clear();
      Pipeline.clear();
      Filters_json_data.clear();

      Scale_drawing_mode[0].clear();
      Translation[0].clear();

      // the widgets of each filter must be included in the filters tab
      Filters_tab->remove_widgets();

      Images.set("COLOR",Image_cv_color);
      Images.set("GRAY",Image_cv_gray);
      if (Original_image->channels()==1){//  Gray
        cvtColor(*Original_image.get(),*Frame_buffer.get(),cv::COLOR_GRAY2BGR,3);
      }
      else{// Color
        *Frame_buffer.get()=Original_image->clone();
      }

      read_data_effect(Name);
      // scale and tranlation values for each image
      for (int i=0;i<Images.size();i++){
        Scale_drawing_mode[0].push_back(1.0);
        Translation[0].push_back(_vertex2f(0,0));
      }
      // the widgets of each filter must be included in the filters tab
      Filters_tab->insert_widgets();

      Selected_image_index=0;
      Selected_filter_index=0;

      show_images();
      if (Filters.size()>0) show_filters();

      // show the widget
      show();

      glTexImage2D(GL_TEXTURE_2D,0,3,Frame_buffer->cols,Frame_buffer->rows,0,GL_BGR,GL_UNSIGNED_BYTE,Frame_buffer->data);

      Drawn=false;

      update();
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::create_filters_from_blocks()
{
  string Type;
  string Name;
  cv::Mat *Input_image0;
  cv::Mat *Output_image0;
  cv::Mat *Input_image1;
  _block Block;

  // clear all the data
  clear_data();
  if (Image_loaded==true){
    // The input image is copied to the frame-buffer as an initial value
    if (Original_image->channels()==1){
      Image_cv_gray->release();
      *Image_cv_gray.get()=Original_image->clone();
      Image_cv_color->release();
      Image_cv_color->create(Original_image->rows,Original_image->cols,CV_8UC3);
      Frame_buffer->release();
      Frame_buffer->create(Original_image->rows,Original_image->cols,CV_8UC3);

      // The input image is copied to the color image
      cvtColor(*Image_cv_gray.get(),*Image_cv_color.get(),cv::COLOR_GRAY2RGB,3);
      // The input image is copied to the frame-buffer as an initial value
      cvtColor(*Original_image.get(),*Frame_buffer.get(),cv::COLOR_GRAY2BGR,3);
    }
    else{
      Image_cv_color->release();
      *Image_cv_color.get()=Original_image->clone();
      Image_cv_gray->release();
      Image_cv_gray->create(Original_image->rows,Original_image->cols,CV_8U);

      // the image is converted to gray escale
      cvtColor(*Image_cv_color.get(),*Image_cv_gray.get(),cv::COLOR_BGR2GRAY,1);
      // The input image is copied to the frame-buffer as an initial value
      *Frame_buffer.get()=Original_image->clone();
    }
    Images.set("COLOR",Image_cv_color);
    // The gray version is saved as 1
    Images.set("GRAY",Image_cv_gray);
  }
  else{// there is no image loaded
    //  We set a gray initial image
    Image_cv_gray->release();
    Image_cv_gray->create(256,256,CV_8U);
    Image_cv_gray->setTo(255);
    Image_cv_color->release();
    Image_cv_color->create(256,256,CV_8UC3);
    Frame_buffer->release();
    Frame_buffer->create(256,256,CV_8UC3);
    // The input image is copied to the frame-buffer as an initial value
    cvtColor(*Image_cv_gray.get(),*Image_cv_color.get(),cv::COLOR_GRAY2RGB,3);
    // The color version is saved as 0
    Images.set("COLOR",Image_cv_color);
    // The gray version is saved as 1
    Images.set("GRAY",Image_cv_gray);
    // The input image is copied to the frame-buffer as an initial value
    *Frame_buffer.get()=Original_image->clone();
  }

  for (unsigned int i=0;i<Vec_order->size();i++){
    Name=(*Vec_order)[i];
    Block=(*Map_data_blocks)[Name];

    Type=Block.Type;
    Name=Block.Name;
    Input_image0=Images.Data_by_string[Block.In1].get();
    if (Block.In2!="NULL"){
      Input_image1=Images.Data_by_string[Block.In2].get();
    }
    else Input_image1=nullptr;

    // save data for the graph
    // input 0
    if (Graph.find(Block.In1)==Graph.end()){
      vector<string> Vec_aux;
      Vec_aux.push_back(Name);
      Graph[Block.In1]=Vec_aux;
    }
    else{
      Graph[Block.In1].push_back(Name);
    }

    // input 1
    if (Block.In2!="NULL"){
      if (Graph.find(Block.In2)==Graph.end()){
        vector<string> Vec_aux;
        Vec_aux.push_back(Name);
        Graph[Block.In2]=Vec_aux;
      }
      else{
        Graph[Block.In2].push_back(Name);
      }
    }

    int Width_aux=0;
    int Height_aux=0;

    switch((int)Type_filter_name[Type]){
    #ifdef DEFINE_FILTER_BILATERAL
    case FILTER_BILATERAL:Filters.set(Name,make_shared<_filter_bilateral_ui>(this,(*Map_filters_parameters)[Name],Name));break; // Bilateral
    #endif

    #ifdef DEFINE_FILTER_CANNY
    case FILTER_CANNY:Filters.set(Name,make_shared<_filter_canny_ui>(this,(*Map_filters_parameters)[Name],Name));break; // CANNY
    #endif

    #ifdef DEFINE_FILTER_CCVT
    case FILTER_CCVT:Filters.set(Name,make_shared<_filter_ccvt_ui>(this,(*Map_filters_parameters)[Name],Name));break; // BALZER'S CCVT
    #endif

    #ifdef DEFINE_FILTER_COMBINATION
    case FILTER_COMBINATION:{
        shared_ptr<_filter_combination_ui> Filter1=make_shared<_filter_combination_ui>(this,(*Map_filters_parameters)[Name],Name);
        Filters.set(Name,Filter1);
        if (Input_image0->cols!=Original_image->cols || Input_image0->rows!=Original_image->rows){
          Filter1->set_change_output_image_size(true);
          Width_aux=Input_image0->cols;
          Height_aux=Input_image0->rows;
        }
      }
      break; // COMBINATION
    #endif

    #ifdef DEFINE_FILTER_CONTRAST_BRIGHT
    case FILTER_CONTRAST_BRIGHT:Filters.set(Name,make_shared<_filter_contrast_bright_ui>(this,(*Map_filters_parameters)[Name],Name));break; // CONTRAST
    #endif

    #ifdef DEFINE_FILTER_DILATION
    case FILTER_DILATION:Filters.set(Name,make_shared<_filter_dilation_ui>(this,(*Map_filters_parameters)[Name],Name));break; // DILATION
    #endif

    #ifdef DEFINE_FILTER_DISTANCE_FIELD
    case FILTER_DISTANCE_FIELD:Filters.set(Name,make_shared<_filter_distance_field_ui>(this,(*Map_filters_parameters)[Name],Name));break; // FIELD DISTANCE
    #endif

    #ifdef DEFINE_FILTER_DOG
    case FILTER_DOG:Filters.set(Name,make_shared<_filter_dog_ui>(this,(*Map_filters_parameters)[Name],Name));break; // DOG
    #endif

    #ifdef DEFINE_FILTER_DOT_EBG
    case FILTER_DOT_EBG:
    {
      shared_ptr<_filter_dot_ebg_ui> Filter1=make_shared<_filter_dot_ebg_ui>(this,(*Map_filters_parameters)[Name],Name);
      Filters.set(Name,Filter1);
      Filter1->set_dots_images(&Dots);

      float Scaling_factor=Filter1->scaling_factor();
      Width_aux=(int)((float)Input_image0->cols*Scaling_factor);
      Height_aux=(int)(Input_image0->rows*Scaling_factor);

      adjust_image_sizes(Width_aux,Height_aux);

      break; // DOT_EBS
    }
    #endif

    #ifdef DEFINE_FILTER_DOT_SVG
    case FILTER_DOT_SVG:
    {
      shared_ptr<_filter_dot_svg_ui> Filter1=make_shared<_filter_dot_svg_ui>(this,(*Map_filters_parameters)[Name],Name);
      Filters.set(Name,Filter1);
      Filter1->set_dots_images(&Dots);

      float Scaling_factor=Filter1->scaling_factor();
      Width_aux=(int)((float)Input_image0->cols*Scaling_factor);
      Height_aux=(int)(Input_image0->rows*Scaling_factor);

      adjust_image_sizes(Width_aux,Height_aux);

      break; // DOT_EBS
    }
    #endif

    #ifdef DEFINE_FILTER_EROTION
    case FILTER_EROTION:Filters.set(Name,make_shared<_filter_erotion_ui>(this,(*Map_filters_parameters)[Name],Name));break; // DILATION
    #endif
    #ifdef DEFINE_FILTER_EXAMPLE_BASED_GRAYSCALE_STIPPLING
    case FILTER_STIPPLING_EBG:{
        shared_ptr<_filter_stippling_ebg> Filter1=make_shared<_filter_stippling_ebg_ui>(this,(*Map_filters_parameters)[Name],Name);
        Filters.set(Name,Filter1);
        Filter1->set_dots_images(&Dots);

        float Scaling_factor=Filter1->scaling_factor();
        Width_aux=(int)((float)Input_image0->cols*Scaling_factor);
        Height_aux=(int)(Input_image0->rows*Scaling_factor);

        adjust_image_sizes(Width_aux,Height_aux);
        Active_dots_tabs=true;
      }
      break; // STIPPLING_EBS
    #endif

    #ifdef DEFINE_FILTER_GAUSSIAN
    case FILTER_GAUSSIAN:Filters.set(Name,make_shared<_filter_gaussian_ui>(this,(*Map_filters_parameters)[Name],Name));break; // GAUSSIAN
    #endif

    #ifdef DEFINE_FILTER_HALFTONING_ADAPTIVE_CLUSTERING_SELECTIVE_PRECIPITATION
    case FILTER_HALFTONING_ACSP:Filters.set(Name,make_shared<_filter_halftoning_acsp_ui>(this,(*Map_filters_parameters)[Name],Name));break; // ADAPTIVE CLUSTERING SELECTIVE PRECIPITATION
    #endif

    #ifdef DEFINE_FILTER_HALFTONING_CONTRAST_AWARE
    case FILTER_HALFTONING_CAH:Filters.set(Name,make_shared<_filter_halftoning_cah_ui>(this,(*Map_filters_parameters)[Name],Name));break; // HALFTONING_CAH
    #endif

    #ifdef DEFINE_FILTER_HALFTONING_OSTROMOUKHOV
    case FILTER_HALFTONING_OST:Filters.set(Name,make_shared<_filter_halftoning_ost_ui>(this,(*Map_filters_parameters)[Name],Name));break; // HALFTONING
    #endif

    #ifdef DEFINE_FILTER_HALFTONING_SPACE_FILLING_CURVE
    case FILTER_HALFTONING_SFC:Filters.set(Name,make_shared<_filter_halftoning_sfc_ui>(this,(*Map_filters_parameters)[Name],Name));break; // SPACE FILLING CURVE
    #endif

    #ifdef DEFINE_FILTER_INVERSION
    case FILTER_INVERSION:Filters.set(Name,make_shared<_filter_inversion_ui>(this,(*Map_filters_parameters)[Name],Name));break; // INVERSION
    #endif

    #ifdef DEFINE_FILTER_KANG
    case FILTER_KANG:Filters.set(Name,make_shared<_filter_kang_ui>(this,(*Map_filters_parameters)[Name],Name));break; // KANG
    #endif

    #ifdef DEFINE_FILTER_MEASURE_SSIM_PSNR
    case FILTER_MEASURE_SSIM_PSNR:Filters.set(Name,make_shared<_filter_measure_ssim_psnr_ui>(this,Name));break; // MEASURE_SSIM_PSNR
    #endif

    #ifdef DEFINE_FILTER_RETINEX
    case FILTER_RETINEX:Filters.set(Name,make_shared<_filter_retinex_ui>(this,(*Map_filters_parameters)[Name],Name));break; // RETINEX
    #endif

    #ifdef DEFINE_FILTER_RWT
    case FILTER_RWT:
      if (Input_image0->cols!=Input_image0->rows){
        QMessageBox MsgBox;
        MsgBox.setText("Filter Recursive Wang Tiles");
        MsgBox.setInformativeText("Warning: the image must be square");
        MsgBox.setStandardButtons(QMessageBox::Ok);
        MsgBox.exec();
        exit(-1);
      }
      Filters.set(Name,make_shared<_filter_rwt_ui>(this,(*Map_filters_parameters)[Name],Name));
      break; // STIPPLING_RWT
    #endif

    #ifdef DEFINE_FILTER_STIPPLING_BY_EXAMPLE
    case FILTER_STIPPLING_SBE:
      Filters.set(Name,make_shared<_filter_stippling_sbe_ui>(this,(*Map_filters_parameters)[Name],Name));

      if (Input_image0->rows!=4096 || Input_image0->cols!=4096){
        QMessageBox MsgBox;
        MsgBox.setText("Filter Stippling by example");
        if (Input_image0->rows==Input_image0->cols){
          MsgBox.setInformativeText("Warning: the image must have around the optimum: 4096x4096. Do you want to resize it?");
          MsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
          MsgBox.setDefaultButton(QMessageBox::No);
        }
        else{
            MsgBox.setInformativeText("Warning: the image must be square");
            MsgBox.setStandardButtons(QMessageBox::Ok);
            MsgBox.exec();
            exit(-1);
        }
        int Result = MsgBox.exec();
        switch (Result) {
        case QMessageBox::Yes:
          // change the size
          Filters.get_data(Name)->set_change_output_image_size(true);

          float Factor;
          float Rest;

          Factor=4096/Input_image0->rows;

          Height_aux=(int)((float)Input_image0->rows*Factor);
          if (Height_aux%4!=0){
            Rest=(float)Height_aux/4.0;
            Rest=Rest-(float)(Height_aux/4);
            if (Rest<=0.5) Height_aux=(Height_aux/4)*4;
            else Height_aux=((Height_aux/4)+1)*4;
          }

          Width_aux=Height_aux;
        }
      }
      break;
    #endif // STIPPLING BY EXAMPLE

    #ifdef DEFINE_FILTER_STIPPLING_STRUCTURE_AWARE
    case FILTER_STIPPLING_SAS:Filters.set(Name,make_shared<_filter_stippling_sas_ui>(this,(*Map_filters_parameters)[Name],Name));break;
    #endif  // STIPPLING_SAS

    #ifdef DEFINE_FILTER_STIPPLING_CONTRAST_AWARE
    case FILTER_STIPPLING_CAS:Filters.set(Name,make_shared<_filter_stippling_cas_ui>(this,(*Map_filters_parameters)[Name],Name));break; // STIPPLING_CAS
    #endif

    #ifdef DEFINE_FILTER_WCVD
    case FILTER_WCVD:
    {
      Filters.set(Name,make_shared<_filter_wcvd_ui>(this,(*Map_filters_parameters)[Name],Name));break; //WCVD
//      _filter_wcvd_ui *Filter1=new _filter_wcvd_ui(this,(*Map_filters_parameters)[Name],Name);
//      Filters.set(Name,Filter1);
      break;
    }
    #endif

    }// end switch

    if (Filters.get_data(Name)->change_output_image_size()==false){ // not necessary to change
      Images.set(Name,make_shared<cv::Mat>());
      Images.get_data(Name)->create(Input_image0->rows,Input_image0->cols,CV_8U);
    }
    else{ // it is necessary to change the size of the output image
      Images.set(Name,make_shared<cv::Mat>());
      Images.get_data(Name)->create(Height_aux,Width_aux,CV_8U);
    }

    // output_image0
    Output_image0=Images.get_data(Name).get();

    Filters.get_data(Name)->set_images(Input_image0,Output_image0,Input_image1);
    Pipeline.push_back(Filters.get_data(Name));
  }

  if (Image_loaded==true){
    // sets initial tab
    Window->set_tab(0);

    Scale_drawing_mode[0].clear();
    Translation[0].clear();

    // the widgets of each filter must be included in the filters tab
    Filters_tab->remove_widgets();

    // scale and tranlation values for each image
    for (int i=0;i<Images.size();i++){
      Scale_drawing_mode[0].push_back(1.0);
      Translation[0].push_back(_vertex2f(0,0));
    }

    // the widgets of each filter must be included in the filters tab
    Filters_tab->insert_widgets();

    Selected_image_index=0;
    Selected_filter_index=0;

    show_images();
    if (Filters.size()>0) show_filters();

    // show the widget
    show();

    update_all_filters();

    refresh_image();
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::create_filters_from_blocks(map<string,_block> *Map_data_blocks1, vector<string> *Vec_order1, map<string, map<string, string> > *Map_filters_parameters1)
{
  Map_data_blocks=Map_data_blocks1;
  Vec_order=Vec_order1;
  Map_filters_parameters=Map_filters_parameters1;

  if (Image_loaded==true){
    create_filters_from_blocks();
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::update_filters_parameters()
{
  // the values of the Map_filters_parameters must be update with the current values
  // also, the _INI_ filed must be changef to FILE. This allows to load the modified values if the effect is changed
  string Name;

  for (int i=0;i<Filters.size();i++){
    Name=(Filters.get_data(i))->name();
    (Filters.get_data(i))->write_parameters((*Map_filters_parameters)[Name]);
    (*Map_filters_parameters)[Name]["_INI_"]="FILE";
  }
}



/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::change_tab(int Index1)
{
  switch (Index1) {
  case 0:
    Selected_tab=0;
    Selected_index=Selected_image_index;
    refresh_image();
    break;
  case 1:
    Selected_index=Selected_image_index;
    if (Selected_tab!=0){
      Selected_tab=0;
      refresh_image();
    }
    break;
  default:break;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _gl_widget::adjust_image_sizes(int &Width1, int &Height1)
{
  float Rest;

  if (Width1%4!=0){
    Rest=round((float)Width1/4.0);
    Width1=(int)Rest*4;

  }
  if (Height1%4!=0){
    Rest=round((float)Height1/4.0);
    Height1=(int)Rest*4;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void  _gl_widget::check2(string File_name1,string File_name2)
{
  Q_UNUSED(File_name1)
  Q_UNUSED(File_name2)

  #ifdef DEFINE_FILTER_MEASURE_SSIM_PSNR
  string Name="prueba";
  cv::Mat *Output_image0;
  //cv::Mat *Input_image1;

  // the original image is read
  _image_IO Image_io;

  Image_io.read_image(File_name1,*Image_cv_gray.get());
  cv::flip(*Image_cv_gray.get(),*Image_cv_gray.get(),0);
  Images.set("GRAY",Image_cv_gray);

  Image_io.read_image(File_name2,*Image_cv_gray1.get());
  cv::flip(*Image_cv_gray1.get(),*Image_cv_gray1.get(),0);
  Images.set("GRAY1",Image_cv_gray1);


  Filters.set(Name,make_shared<_filter_measure_ssim_psnr_ui>(this,Filters_json_data[0],Name));

  if (Filters.get_data(Name)->change_output_image_size()==false){ // not necessary to change
    Images.set(Name,make_shared<cv::Mat>());
    Images.get_data(Name)->create(Image_cv_gray->rows,Image_cv_gray->cols,CV_8U);
  }

  // output_image0
  Output_image0=Images.get_data(Name).get();

  Filters.get_data(Name)->set_images(Image_cv_gray.get(),Output_image0,Image_cv_gray1.get());
  Filters.get_data(Name)->update();
  #endif
}
