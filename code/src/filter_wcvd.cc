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


#include "filter_wcvd.h"
#include "glwidget.h"

using namespace _f_wcvd_ns;


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_wcvd::_filter_wcvd()
{
  Percent_of_dots=WCVD_PERCENT_OF_DOTS_DEFAULT;
  Number_of_dark_dots=WCVD_NUMBER_OF_DARK_DOTS_DEFAULT;
  Number_of_dots=(int)((float)Number_of_dark_dots*DOTS_FACTOR);
  Number_of_good_dots=Number_of_dots*Percent_of_dots/100;

  Local_change=false;
  Moved_points=1e8;

  Num_channels_input_image_0=1;
  Num_channels_output_image_0=1;

  Type_filter=_f_filter_ns::FILTER_WCVD;

  Scaling_factor=1;
  Change_output_image_size=false;
  Use_dots=false;
  Dark_points_counted=false;

  Save_intermediate_images=false;

  Percent_fixed_centroidals=WCVD_PERCENT_FIXED_CENTROIDALS;
  Number_of_iteractions=WCVD_NUM_ITERACTIONS_DEFAULT;

  C1.init(0,1);
  C1.set_seed(1000);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::reset_data()
{
  Percent_of_dots=WCVD_PERCENT_OF_DOTS_DEFAULT;
  Number_of_dark_dots=WCVD_NUMBER_OF_DARK_DOTS_DEFAULT;
  Number_of_dots=(int)((float)Number_of_dark_dots*DOTS_FACTOR);
  Number_of_good_dots=Number_of_dots*Percent_of_dots/100;

  Percent_fixed_centroidals=WCVD_PERCENT_FIXED_CENTROIDALS;
  Number_of_iteractions=WCVD_NUM_ITERACTIONS_DEFAULT;

  C1.init(0,1);
  C1.set_seed(1000);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::clear_window()
{
  glClearColor(1,1,1,1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::change_mvp()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,Window_width,0,Window_height,Front_plane,Back_plane);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glViewport(0,0,Window_width,Window_height);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::count_dark_pixels(cv::Mat *Input_image)
{
  unsigned int Count=0;

  Window_width=Input_image->cols;
  Window_height=Input_image->rows;

  // count the number of dark pixels
  for (int Row=0;Row<Window_height;Row++){
    for (int Col=0;Col<Window_width;Col++){
      if (Input_image->at<unsigned char>(Row,Col)<255) Count++;
    }
  }

  // max number of WCVD dots
  Number_of_dark_dots=Count;
  // apply the percentage of dots to not fill the full image
  Number_of_dots=(int)((float)Number_of_dark_dots*DOTS_FACTOR);

  if (Percent_of_dots>0) Number_of_good_dots=(int) ((float)Number_of_dots*(float)Percent_of_dots/100.0f);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::initialize()
{
  float Angle;

  Pixels.clear();
  Points.clear();
  New_positions.clear();
  Pixels_count.clear();
  Vertices.clear();
  Triangles.clear();
  Colors.clear();

  Points.resize(Number_of_good_dots);
  New_positions.resize(Number_of_good_dots);
  Colors_cones.resize(Number_of_good_dots);
  Pixels.resize(Window_width*Window_height);
  Pixels_count.resize(Number_of_good_dots);

  if (Window_width>Window_height) Radius=Window_width;
  else Radius=Window_height;

  Radius=sqrt(2)*Radius; // the radius must cover the rectangle

  // formula to compute the angle with an error (See article)
  int Error=1; // 1 pixel error
  Angle=2*acos((float)((int)Radius-Error)/Radius);
  Num_divisions=(int)(2*M_PI/Angle)+1;

  Num_vertices=Number_of_good_dots*(Num_divisions+1);
  Num_faces=Number_of_good_dots*Num_divisions;

  // normal data
  Vertices.resize(Num_vertices);
  Triangles.resize(Num_faces);
  Colors.resize(Num_vertices);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::create_random_points(cv::Mat *Input_image)
{
  _vertex3f Vertex_aux(0,0,Point_height);
  _random_uniform_int Position_x(2,Window_width-2);
  _random_uniform_int Position_y(2,Window_height-2);
  unsigned int Count, Count_errors=0;
  cv::Mat Selected_positions;

  // this image allows to remove duplicates
  Selected_positions.create(Window_height,Window_width,CV_8U);
  Selected_positions.setTo(255);
  Position_x.seed();
  Count=0;
  Position_y.seed();

  while (Count<Number_of_good_dots){
    Vertex_aux.x=roundf(Position_x.value());
    Vertex_aux.y=roundf(Position_y.value());
    Count_errors++;
    // controls that no point of a white zone is selected (probably it will produce a weight=0)
    if (Input_image->at<unsigned char>(Vertex_aux.y,Vertex_aux.x)<250 && Selected_positions.at<unsigned char>(Vertex_aux.y,Vertex_aux.x)==255){
      New_positions[Count]=Vertex_aux;
      Count++;
      // this postions has been selected
      Selected_positions.at<unsigned char>(Vertex_aux.y,Vertex_aux.x)=0;
      // plus the 8 neighbors
      Selected_positions.at<unsigned char>(Vertex_aux.y,Vertex_aux.x+1)=0;
      Selected_positions.at<unsigned char>(Vertex_aux.y+1,Vertex_aux.x+1)=0;
      Selected_positions.at<unsigned char>(Vertex_aux.y+1,Vertex_aux.x)=0;
      Selected_positions.at<unsigned char>(Vertex_aux.y+1,Vertex_aux.x-1)=0;
      Selected_positions.at<unsigned char>(Vertex_aux.y,Vertex_aux.x-1)=0;
      Selected_positions.at<unsigned char>(Vertex_aux.y-1,Vertex_aux.x-1)=0;
      Selected_positions.at<unsigned char>(Vertex_aux.y-1,Vertex_aux.x)=0;
      Selected_positions.at<unsigned char>(Vertex_aux.y-1,Vertex_aux.x+1)=0;
      //
      Count_errors=0;
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::copy_points()
{
  for (unsigned int i=0;i<Number_of_good_dots;i++) Points[i]=New_positions[i];
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::create_primitive(unsigned int Index)
{
  float x,y;
  float Angle;
  _vertex3f Vertex(0,0,0);
  _vertex3i Triangle;
  int Offset,Offset1;
  int Pos_apex;
  _vertex3uc Color;
  _vertex3f Center=Points[Index];

  Offset=Index*(Num_divisions+1); // for vertices
  Offset1=Index*Num_divisions; // for triangles

  // Compute one color
  Color.r= (Index & 0x00FF0000) >> 16;
  Color.g= (Index & 0x0000FF00) >> 8;
  Color.b= Index & 0x000000FF;

  // asign the color
  Colors_cones[Index]=Color;

  // create the vertices for a cone and assign the color
  // the base is at z=0
  for (int i=0;i<Num_divisions;i++){
    Angle=2*M_PI*(float)i/(float)Num_divisions;
    x=Radius*cos(Angle);
    y=Radius*sin(Angle);
    Vertex.x=(float)Center.x+x;
    Vertex.y=(float)Center.y+y;
    Vertices[Offset+i]=Vertex;
    Colors[Offset+i]=Color;
  }
  // the apex is include
  Pos_apex=Offset+Num_divisions;
  Vertices[Pos_apex]=_vertex3f(Center.x,Center.y,Center.z);
  Colors[Pos_apex]=Color;

  // create the triangles
  for (int i=0;i<Num_divisions;i++){
    Triangle._0=Offset+i;
    Triangle._1=Offset+((i+1)%(Num_divisions));
    Triangle._2=Pos_apex;
    Triangles[Offset1+i]=Triangle;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::create()
{
  // create the data
  for (unsigned int i=0;i<Number_of_good_dots;i++){
    create_primitive(i);
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::hsv_to_rbg(float h,float s, float v,unsigned char &r,unsigned char &g,unsigned char &b)
{
  int h_i;
  float f,p,q,t;

  h_i =(int)(h*6);
  f = h*6 - h_i;
  p = v * (1 - s);
  q = v * (1 - f*s);
  t = v * (1 - (1 - f) * s);
  switch(h_i){
  case 0:
    r=(int)(v*256);
    g=(int)(t*256);
    b=(int)(p*256);
    break;
  case 1:
    r=(int)(q*256);
    g=(int)(v*256);
    b=(int)(p*256);
    break;
  case 2:
    r=(int)(p*256);
    g=(int)(v*256);
    b=(int)(t*256);
    break;
  case 3:
    r=(int)(p*256);
    g=(int)(q*256);
    b=(int)(v*256);
    break;
  case 4:
    r=(int)(t*256);
    g=(int)(p*256);
    b=(int)(v*256);
    break;
  case 5:
    r=(int)(v*256);
    g=(int)(p*256);
    b=(int)(q*256);
    break;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::create_primitive_drawing(unsigned int Index)
{
  float x,y;
  float Angle;
  _vertex3f Vertex(0,0,0);
  _vertex3i Triangle;
  int Offset,Offset1;
  int Pos_apex;
  _vertex3uc Color;
  _vertex3f Center=Points[Index];

  Offset=Index*(Num_divisions+1); // for vertices
  Offset1=Index*Num_divisions; // for triangles

  // Compute one color
  hsv_to_rbg(C1.value(),0.75,0.75,Color.r,Color.g,Color.b);
  // asign the color
  Colors_cones[Index]=Color;
  // create the vertices for a cone and asign the color
  // the base is at z=0
  for (int i=0;i<Num_divisions;i++){
    Angle=2*M_PI*(float)i/(float)Num_divisions;
    x=Radius*cos(Angle);
    y=Radius*sin(Angle);
    Vertex.x=(float)Center.x+x;
    Vertex.y=(float)Center.y+y;
    Vertices[Offset+i]=Vertex;
    Colors[Offset+i]=Color;
  }
  // the apex is included
  Pos_apex=Offset+Num_divisions;
  Vertices[Pos_apex]=_vertex3f(Center.x,Center.y,Apex_height);
  Colors[Pos_apex]=Color;

  // create the triangles
  for (int i=0;i<Num_divisions;i++){
    Triangle._0=Offset+i;
    Triangle._1=Offset+((i+1)%(Num_divisions));
    Triangle._2=Pos_apex;
    Triangles[Offset1+i]=Triangle;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::create_drawing()
{
  C1.set_seed(1000);
  // create the data
  for (unsigned int i=0;i<Number_of_good_dots;i++){
    create_primitive_drawing(i);
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::compute_centroids(cv::Mat *Input_image)
{
  unsigned int Index;
  _vertex3uc Pixel;
  float Weight;
  _vertex2f Difference;
  unsigned int Count;

  glDisable(GL_DEPTH_TEST);
  // read the data form the framebuffer. It is saved in Pixels
  glReadBuffer(GL_FRONT);
  glPixelStorei(GL_PACK_ALIGNMENT,1);
  glReadPixels(0,0,Window_width,Window_height,GL_RGB,GL_UNSIGNED_BYTE,&Pixels[0]);

  // As each zone has a unique color, we can obtain the centroid by adding the positions with the same id and then dividing by the number
  // of pixels with the same ID.
  // This is the case with a constan weight (the input image has the same tone). As we want that the points to be more compacted in black zones
  // the pixels that correspond to darker zones has more weight that those of clearer zones
  // The solution is that the weight for each pixel is not 1, but the 255-pixel_intensity -> black pixels has more importance
  // this also explain why no white pixels are selected as seed points

  // the values are initialized
  for (unsigned int i=0;i<Number_of_good_dots;i++) New_positions[i]=_vertex3f(0,0,Point_height);
  for (unsigned int i=0;i<Number_of_good_dots;i++) Pixels_count[i]=0;

  // the vector is walked as a matrix
  for (int i=0;i<Window_height;i++){
    for (int j=0;j<Window_width;j++){
      // the pixel i,j is obtained
      Pixel=Pixels[i*Window_width+j];
      // the RGB value is converted to a ID
      //Index=(unsigned int)Pixel.Red+(unsigned int)(Pixel.Green << 8)+(unsigned int)(Pixel.Blue << 16);
      Index=(unsigned int)(Pixel.r << 16)+(unsigned int)(Pixel.g << 8)+(unsigned int)Pixel.b;
      if (Index!=16777215){ // it is not a white pixel ¡¡¡SOMETHING TO RESOLVE. ALL PIXELS MUST BE DIFFERENT TO WHITE!!!
        // the weight of the pixel is computed depending on the tone of the image
        Weight=(255.0-(float) Input_image->at<unsigned char>(i,j))/255.0;
//        cout << "Tratando el pixel: " << i << " " << j << " index: " << Index << " Weight: " << Weight << endl;
        // the new position modulated by weight is added to the Index position
        New_positions[Index]+=_vertex3f(j*Weight,i*Weight,0);
        // the weight is accumulated
        Pixels_count[Index]+=Weight;
      }
    }
  }

  // the centroids are computed as the median of the positions
  // Moved points is used to detect when to stop
  // the minimum change is 1 pixel difference

  Moved_points=0;
  Count=0;
  for (unsigned int i=0;i<Number_of_good_dots;i++){
    if (Pixels_count[i]>0){
      // obtain the new position
      New_positions[i].x/=Pixels_count[i];
      New_positions[i].y/=Pixels_count[i];
      Difference=Points[i]-New_positions[i];
      // Only if the difference between the old and the new position is > 1 it is changed
      if (Difference.module()<MIN_DOTS_DISTANCE){
        // recover the original position
        New_positions[Count].x=Points[i].x;
        New_positions[Count].y=Points[i].y;
      }
      else{
        // save the new position
        New_positions[Count].x=New_positions[i].x;
        New_positions[Count].y=New_positions[i].y;
        Moved_points++;
      }
      Count++;
    }
  }

  Number_of_good_dots=Count;

  glEnable(GL_DEPTH_TEST);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::draw_new_points(float R,float G,float B)
{
  unsigned int i;

  glPointSize(3);
  glColor3f(R,G,B);
  glBegin(GL_POINTS);
  for (i=0;i<Number_of_good_dots;i++){
    glVertex3fv((GLfloat *) &Points[i]);
  }
  glEnd();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::draw_points(cv::Mat *Input_image)
{
  for (unsigned int i=0;i<Number_of_good_dots;i++){
    Input_image->at<cv::Vec3b>((int) Points[i].y,(int) Points[i].x)=cv::Vec3b(0,0,255);
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::draw_triangles()
{
  int Position;
  _vertex3f Aux1;

  glEnable(GL_DEPTH_TEST);

  for (unsigned int i=0;i<Number_of_good_dots;i++){
    glColor3ubv((GLubyte *) &Colors_cones[i]);
    glBegin(GL_TRIANGLES);
    Position=i*Num_divisions;
    for (int j=0;j<Num_divisions;j++){
      glVertex3fv((GLfloat *) &Vertices[Triangles[Position]._0]);
      glVertex3fv((GLfloat *) &Vertices[Triangles[Position]._1]);
      glVertex3fv((GLfloat *) &Vertices[Triangles[Position]._2]);
      Position++;
    }
    glEnd();
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::draw_dots(cv::Mat *Output_image)
{
  for (unsigned int i=0;i<Number_of_good_dots;i++){
    Output_image->at<unsigned char>((int) Points[i].y,(int) Points[i].x)=0;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::percent_of_dots(unsigned int Percent_of_dots1)
{
  Percent_of_dots=Percent_of_dots1;
  // apply the percentage
  if (Percent_of_dots>0) Number_of_good_dots=Number_of_dots*Percent_of_dots/100;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::number_of_dots(unsigned int Number_of_dots1)
{
  if (Number_of_dots1>Number_of_dots) Number_of_dots1=Number_of_dots;

  Percent_of_dots=((float)Number_of_dots1/(float)Number_of_dots)*100.0;
  // apply the percentage
  if (Percent_of_dots>0) Number_of_good_dots=Number_of_dots*Percent_of_dots/100;
  else Number_of_good_dots=Number_of_dots1;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::wcvd(cv::Mat *Input_image0,cv::Mat *Output_image0)
{
  std::string File_name;
  int Count_pass=0;
  char Name_aux[200];
  int Num_iteractions=0;

  if (Local_change==false){
    //Save_intermediate_images=true;

    count_dark_pixels(Input_image0);
    initialize();

    if (Number_of_good_dots>0){
      create_random_points(Input_image0);

       GL_widget->makeCurrent();

      //renderbuffers
      glGenRenderbuffers(Num_renderbuffers, Renderbuffers);
      // color
      glBindRenderbuffer(GL_RENDERBUFFER, Renderbuffers[0]);
      glRenderbufferStorage(GL_RENDERBUFFER,GL_RGB8,Window_width,Window_height);
      // depth
      glBindRenderbuffer(GL_RENDERBUFFER, Renderbuffers[1]);
      glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24 ,Window_width,Window_height);

      // This is for the FBO
      glGenFramebuffers(1,&Framebuffer);
      glBindFramebuffer(GL_FRAMEBUFFER,Framebuffer);
      // attach the color
      glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER, Renderbuffers[0]);
      // attach the depth
      glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, Renderbuffers[1]);

      //
      change_mvp();

      unsigned int Initial_limit=Number_of_good_dots;

//      int Count_progress=0;
//      QProgressDialog Progress("Computing WCVD...", "Abort",0,Initial_limit);
//      Progress.setWindowModality(Qt::WindowModal);
//      Progress.setMinimumDuration(0);
//      Progress.setCancelButton(0);

      Moved_points=Initial_limit;

      int Limit=(int)((float)Initial_limit*((100.-(float)Percent_fixed_centroidals)/100.0));

      while (Moved_points>Limit && Num_iteractions++<Number_of_iteractions){
        save_GL_state();
        // update progress      
//        Count_progress=Initial_limit-Moved_points;
//        Progress.setValue(Count_progress);

        restore_GL_state();

        clear_window();
        // copy the new centroids to Points
        copy_points();

        // create the new cones
        create();
        // daw the cones
        draw_triangles();

        // compute the new centroids
        compute_centroids(Input_image0);

        // save the intermediante images
        if (Save_intermediate_images){
          clear_window();
          create_drawing();
          draw_triangles();
          draw_new_points(0,0,0);
          sprintf(Name_aux,FILE_NAME,Count_pass);
          File_name=Name_aux;
          save_wcvd_image_to_file(File_name);
          Count_pass++;
        }
      }

//      Progress.setValue(Initial_limit);
 
      // the WCVD is computed. Now the obtained points must be drawn
      copy_points();

      if (Save_intermediate_images){
        clear_window();
        create_drawing();
        draw_triangles();
        draw_new_points(0,0,0);
        sprintf(Name_aux,FILE_NAME,Count_pass);
        File_name=Name_aux;
        save_wcvd_image_to_file(File_name);
        Save_intermediate_images=false;
      }

      // delete the render buffers and frame buffers
      glDeleteRenderbuffers(Num_renderbuffers, Renderbuffers);
      glDeleteFramebuffers(1,&Framebuffer);

      // the normal framebuffer take the control of drawing
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER,GL_widget->defaultFramebufferObject());

      draw_dots(Output_image0);
    }
  }
  else{
    // only to change the drawing of dots
    if (Number_of_good_dots>0) draw_dots(Output_image0);
    Local_change=false;
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::update()
{
  cv::Mat *Aux_input_image=nullptr;
  cv::Mat *Aux_output_image=nullptr;

  // check the output size
  if (Input_image_0->cols!=Output_image_0->cols || Input_image_0->rows!=Output_image_0->rows){
    Output_image_0->release();
    Output_image_0->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
  }

  // Check the number of input channels
  if (Input_image_0->channels()!=Num_channels_input_image_0){// Different number of channels
    if (Input_image_0->channels()==3){
      // conversion
      Aux_input_image=new cv::Mat;
      Aux_input_image->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
      cvtColor(*Input_image_0,*Aux_input_image,cv::COLOR_BGR2GRAY,1);
    }
    else std::cout << "Error in the number of channels in the input image " << __LINE__ << " " << __FILE__ << std::endl;
  }
  else{// the same number of channels
    Aux_input_image=Input_image_0;
  }

  // Check the number of output channels
  if (Output_image_0->channels()!=Num_channels_output_image_0){// Different number of channels
    if (Output_image_0->channels()==3){
      // conversion
      Aux_output_image=new cv::Mat;
      Aux_output_image->create(Input_image_0->rows,Input_image_0->cols,CV_8UC1);
      Aux_output_image->setTo(255);
      wcvd(Aux_input_image,Aux_output_image);
      cvtColor(*Aux_input_image,*Output_image_0,cv::COLOR_GRAY2RGB,3);
    }
    else std::cout << "Error in the number of channels in the output image " << __LINE__ << " " << __FILE__ << std::endl;
  }
  else{// the same number of channels
    Output_image_0->setTo(255);
    wcvd(Aux_input_image,Output_image_0);
  }

  if (Aux_input_image!=nullptr && Aux_input_image!=Input_image_0) delete Aux_input_image;
  if (Aux_output_image!=nullptr && Aux_output_image!=Output_image_0) delete Aux_output_image;

  update_percent_of_dots();
  update_number_of_dots();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::save_wcvd_image_to_file(std::string File_name)
{
  if (Aux_image.cols!=Window_height){
    Aux_image.release();
    Aux_image.create(Window_height,Window_width,CV_8UC3);
  }

  glReadBuffer(GL_FRONT);
  glPixelStorei(GL_PACK_ALIGNMENT,1);
  glReadPixels(0,0,Window_width,Window_height,GL_RGB,GL_UNSIGNED_BYTE,&Aux_image.data[0]);

  // #ifdef WINDOWS
 //    std::string Aux_string;
  // for (int i = 0; i<File_name.size(); i++) {
  // 	if (File_name[i] == '/') {
  // 		Aux_string.push_back('\\');
  // 	}
  // 	else Aux_string.push_back(File_name[i]);
  // }
  // File_name = Aux_string;
  // #endif

  cv::flip(Aux_image,Aux_image,0);

  imwrite(File_name,Aux_image);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::save_wcvd_image()
{
  if (Aux_image.cols!=Window_height){
    Aux_image.release();
    Aux_image.create(Window_height,Window_width,CV_8UC3);
  }

  glReadBuffer(GL_FRONT);
  glPixelStorei(GL_PACK_ALIGNMENT,1);
  glReadPixels(0,0,Window_width,Window_height,GL_RGB,GL_UNSIGNED_BYTE,&Aux_image.data[0]);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::save_GL_state()
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd::restore_GL_state()
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopAttrib();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_wcvd_ui::_filter_wcvd_ui(_gl_widget *GL_widget1,std::string Name1)
{
  GL_widget=GL_widget1;
  Name=Name1;
  Qtw_filter_wcvd=new _qtw_filter_wcvd(GL_widget1,this,Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_wcvd_ui::_filter_wcvd_ui(_gl_widget *GL_widget1, std::map<std::string, std::string> &Parameters, std::string Box_name)
{
  read_parameters(Parameters);
  GL_widget=GL_widget1;
  Name=Box_name;
  Qtw_filter_wcvd=new _qtw_filter_wcvd(GL_widget1,this,Box_name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_filter_wcvd_ui::~_filter_wcvd_ui()
{
  delete Qtw_filter_wcvd;
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd_ui::reset_data()
{
  _filter_wcvd::reset_data();
  Qtw_filter_wcvd->set_parameter1(parameter1());
  hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd_ui::show()
{
  Qtw_filter_wcvd->show();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd_ui::hide()
{
  Qtw_filter_wcvd->hide();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void *_filter_wcvd_ui::get_link()
{
  return Qtw_filter_wcvd->get_link();
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd_ui::read_parameters(std::map<std::string,std::string> &Parameters)
{
  if (Parameters["_INI_"]=="EDITOR"){// default parameters
    // ¡¡¡ it is necessary to change the order of reading to guarantee that percentage has priority over number of points
    parameter2(WCVD_NUMBER_OF_DOTS_DEFAULT);
    parameter1(WCVD_PERCENT_OF_DOTS_DEFAULT);
    parameter3(WCWD_SAVE_INTERMEDIATE_IMAGES_DEFAULT);
  }
  else{// Parameters from file or from initialised filter
    try{
      // ¡¡¡ it is necessary to change the order of reading to guarantee that percentage has priority over number of points
      if (Parameters["number_of_dots"]=="default") parameter2(WCVD_NUMBER_OF_DOTS_DEFAULT);
      else parameter2(atoi(Parameters["number_of_dots"].c_str()));

      if (Parameters["percent_of_dots"]=="default") parameter1(WCVD_PERCENT_OF_DOTS_DEFAULT);
      else parameter1(atoi(Parameters["percent_of_dots"].c_str()));

      if (Parameters["save_intermediate_images"]=="default") parameter3(WCWD_SAVE_INTERMEDIATE_IMAGES_DEFAULT);
      else{
        if (Parameters["save_intermediate_images"]=="true") parameter3(true);
        else parameter3(false);
      }
    }
    catch (const std::out_of_range& oor) {
      QMessageBox MsgBox;
      MsgBox.setText("Error in the data of WCVD filter");
      MsgBox.exec();
      exit(-1);
    }
  }
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd_ui::write_parameters(std::map<std::string, std::string> &Parameters)
{
  char Aux[100];
  
  sprintf(Aux,"%d",parameter1());
  Parameters["percent_of_dots"]=std::string(Aux);
  sprintf(Aux,"%d",parameter2());
  Parameters["number_of_dots"]=std::string(Aux);

  if (parameter3()) sprintf(Aux,"%s","true");
  else sprintf(Aux,"%s","false");
  Parameters["save_intermediate_images"]=std::string(Aux);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd_ui::update_percent_of_dots()
{
  Qtw_filter_wcvd->set_parameter1(parameter1());
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _filter_wcvd_ui::update_number_of_dots()
{
  Qtw_filter_wcvd->set_parameter2(parameter2());
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

_qtw_filter_wcvd::_qtw_filter_wcvd(_gl_widget *GL_widget1,_filter_wcvd_ui *Filter1,std::string Box_name)
{
  QString Str;

  GL_widget=GL_widget1;
  Filter=Filter1;

  Group_box_main=new QGroupBox(tr(Box_name.c_str()));
  Group_box_main->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_main=new QVBoxLayout;

  // Parameter1
  // percent of dots
  QGroupBox *Group_box_parameter1=new QGroupBox(tr(String_group_box_parameter1.c_str()));
  Group_box_parameter1->setAlignment(Qt::AlignCenter);

  QGridLayout *Grid_parameter1 = new QGridLayout;

  QLabel *Label_parameter1_min= new QLabel(tr(String_label_parameter1_min.c_str()));
  QLabel *Label_parameter1_max= new QLabel(tr(String_label_parameter1_max.c_str()));


  Line_edit_parameter1=new QLineEdit();
  Line_edit_parameter1->setAlignment(Qt::AlignRight);
  Line_edit_parameter1->setReadOnly(true);
  Line_edit_parameter1->setEnabled(false);
  Line_edit_parameter1->setToolTip(tr(String_label_parameter1_tooltip.c_str()));
  Str=Str.number(Filter->parameter1());
  Line_edit_parameter1->setText(Str);

  Slider_parameter1 = new QSlider(Qt::Horizontal);
  Slider_parameter1->setRange(Parameter1_min_value,Parameter1_max_value);
  Slider_parameter1->setSingleStep(Parameter1_single_step);
  Slider_parameter1->setPageStep(Parameter1_page_step);
  Slider_parameter1->setTickInterval(Parameter1_tick_interval);
  Slider_parameter1->setTickPosition(QSlider::TicksRight);
  Slider_parameter1->setTracking(Parameter1_set_tracking);
  Slider_parameter1->setValue(Filter->parameter1());
  Slider_parameter1->setToolTip(tr(String_parameter1_tooltip.c_str()));

  Grid_parameter1->addWidget(Line_edit_parameter1,0,1,Qt::AlignCenter);
  Grid_parameter1->addWidget(Label_parameter1_min,1,0,Qt::AlignRight);
  Grid_parameter1->addWidget(Slider_parameter1,1,1);
  Grid_parameter1->addWidget(Label_parameter1_max,1,2,Qt::AlignLeft);

  Group_box_parameter1->setLayout(Grid_parameter1);

  connect(Slider_parameter1, SIGNAL(valueChanged(int)),this,SLOT(set_parameter1_slot(int)));


  // Parameter2
  // Number of points
  QGroupBox *Group_box_parameter2=new QGroupBox(tr(String_group_box_parameter2.c_str()));
  Group_box_parameter2->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_parameter2 = new QVBoxLayout;

  Spinbox_parameter2=new QSpinBox;
  Spinbox_parameter2->setRange(10,100000);
  Spinbox_parameter2->setValue(Filter->parameter2());
  Spinbox_parameter2->setToolTip(tr(String_parameter2_tooltip.c_str()));
  Spinbox_parameter2->setKeyboardTracking(false);
  Spinbox_parameter2->setAlignment(Qt::AlignRight);

  connect(Spinbox_parameter2, SIGNAL(valueChanged(int)),this,SLOT(set_parameter2_slot(int)));

  Vertical_box_parameter2->addWidget(Spinbox_parameter2);

  Group_box_parameter2->setLayout(Vertical_box_parameter2);

  // Save intermediate images
  // Parameter 3
  Group_box_parameter3=new QGroupBox(tr(String_group_box_parameter3.c_str()));
  Group_box_parameter3->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Vertical_box_parameter3 = new QVBoxLayout;

  Checkbox_parameter3=new QCheckBox(tr(String_checkbox_parameter3.c_str()));
  if (Filter->parameter3()) Checkbox_parameter3->setChecked(true);
  else Checkbox_parameter3->setChecked(false);
  Checkbox_parameter3->setToolTip(tr(String_parameter3_tooltip.c_str()));

  connect(Checkbox_parameter3,SIGNAL(stateChanged(int)),this,SLOT(set_parameter3_slot(int)));

  Vertical_box_parameter3->addWidget(Checkbox_parameter3);

  Group_box_parameter3->setLayout(Vertical_box_parameter3);

  // Parameter4
  // percent of fixed centroidals
  Group_box_parameter4=new QGroupBox(tr(String_group_box_parameter4.c_str()));
  Group_box_parameter4->setAlignment(Qt::AlignCenter);

  QGridLayout *Grid_parameter4 = new QGridLayout;

  QLabel *Label_parameter4_min= new QLabel(tr(String_label_parameter4_min.c_str()));
  QLabel *Label_parameter4_max= new QLabel(tr(String_label_parameter4_max.c_str()));


  Line_edit_parameter4=new QLineEdit();
  Line_edit_parameter4->setAlignment(Qt::AlignRight);
  Line_edit_parameter4->setReadOnly(true);
  Line_edit_parameter4->setEnabled(false);
  Line_edit_parameter4->setToolTip(tr(String_label_parameter4_tooltip.c_str()));
  Str=Str.number(Filter->parameter4());
  Line_edit_parameter4->setText(Str);

  Slider_parameter4 = new QSlider(Qt::Horizontal);
  Slider_parameter4->setRange(Parameter4_min_value,Parameter4_max_value);
  Slider_parameter4->setSingleStep(Parameter4_single_step);
  Slider_parameter4->setPageStep(Parameter4_page_step);
  Slider_parameter4->setTickInterval(Parameter4_tick_interval);
  Slider_parameter4->setTickPosition(QSlider::TicksRight);
  Slider_parameter4->setTracking(Parameter4_set_tracking);
  Slider_parameter4->setValue(Filter->parameter4());
  Slider_parameter4->setToolTip(tr(String_parameter4_tooltip.c_str()));

  Grid_parameter4->addWidget(Line_edit_parameter4,0,1,Qt::AlignCenter);
  Grid_parameter4->addWidget(Label_parameter4_min,1,0,Qt::AlignRight);
  Grid_parameter4->addWidget(Slider_parameter4,1,1);
  Grid_parameter4->addWidget(Label_parameter4_max,1,2,Qt::AlignLeft);

  Group_box_parameter4->setLayout(Grid_parameter4);

  connect(Slider_parameter4, SIGNAL(valueChanged(int)),this,SLOT(set_parameter4_slot(int)));


  // Parameter5
  // percent of fixed centroidals
  Group_box_parameter5=new QGroupBox(tr(String_group_box_parameter5.c_str()));
  Group_box_parameter5->setAlignment(Qt::AlignCenter);

  QVBoxLayout *Verticalbox_parameter5 = new QVBoxLayout;

  Spinbox_parameter5=new QSpinBox;

  Spinbox_parameter5 = new QSpinBox;
  Spinbox_parameter5->setMaximum(Filter->parameter5());
  Spinbox_parameter5->setAlignment(Qt::AlignRight);
  Spinbox_parameter5->setKeyboardTracking(false);
  Spinbox_parameter5->setValue(Filter->parameter5());
  Spinbox_parameter5->setToolTip(tr(String_parameter5_tooltip.c_str()));

  Verticalbox_parameter5->addWidget(Spinbox_parameter5);

  Group_box_parameter5->setLayout(Verticalbox_parameter5);

  connect(Spinbox_parameter5, SIGNAL(valueChanged(int)),this,SLOT(set_parameter5_slot(int)));

  //
  Vertical_box_main->addWidget(Group_box_parameter4);
  Vertical_box_main->addWidget(Group_box_parameter5);
  Vertical_box_main->addWidget(Group_box_parameter1);
  Vertical_box_main->addWidget(Group_box_parameter2);
  Vertical_box_main->addWidget(Group_box_parameter3);

  Group_box_main->setLayout(Vertical_box_main);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_wcvd::set_parameter1(int Value)
{
  QString Str;

  Str=Str.number(Value);
  Line_edit_parameter1->setText(Str);
  Slider_parameter1->blockSignals(true);
  Slider_parameter1->setValue(Value);
  Slider_parameter1->blockSignals(false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_wcvd::set_parameter2(int Value)
{
  Spinbox_parameter2->blockSignals(true);
  Spinbox_parameter2->setValue(Value);
  Spinbox_parameter2->blockSignals(false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_wcvd::set_parameter3(bool Value)
{
  Checkbox_parameter3->blockSignals(true);
  if (Value) Checkbox_parameter3->setChecked(true);
  else Checkbox_parameter3->setChecked(false);
  Checkbox_parameter3->blockSignals(false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_wcvd::set_parameter4(int Value)
{
  QString Str;

  Str=Str.number(Value);
  Line_edit_parameter4->setText(Str);
  Slider_parameter4->blockSignals(true);
  Slider_parameter4->setValue(Value);
  Slider_parameter4->blockSignals(false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_wcvd::set_parameter5(int Value)
{
  Spinbox_parameter5->blockSignals(true);
  Spinbox_parameter5->setValue(Value);
  Spinbox_parameter5->blockSignals(false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_wcvd::set_parameter1_slot(int Size)
{
  QString Str;
  char Aux[100];

  sprintf(Aux,"%2d",Size);
  Str=Aux;
  Line_edit_parameter1->setText(Str);
  Filter->parameter1(Size);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_wcvd::set_parameter2_slot(int Size)
{
  Filter->parameter2(Size);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_wcvd::set_parameter3_slot(int Value)
{
  if (Value==Qt::Checked) Filter->parameter3(true);
  else Filter->parameter3(false);

  GL_widget->update_effect(Filter->Name);
  Checkbox_parameter3->blockSignals(true);
  Checkbox_parameter3->setChecked(false);
  Checkbox_parameter3->blockSignals(false);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_wcvd::set_parameter4_slot(int Size)
{
  QString Str;
  char Aux[100];

  sprintf(Aux,"%2d",Size);
  Str=Aux;
  Line_edit_parameter4->setText(Str);
  Filter->parameter4(Size);
  GL_widget->update_effect(Filter->Name);
}


/*****************************************************************************//**
 *
 *
 *
 *****************************************************************************/

void _qtw_filter_wcvd::set_parameter5_slot(int Size)
{
  Filter->parameter5(Size);
  GL_widget->update_effect(Filter->Name);
}
