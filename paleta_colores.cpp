#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_show_colormap.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Image.H>
#include <FL/x.H>
#include <FL/fl_draw.H>

#include <stdlib.h>
#include <stdio.h>

int width = 75;
int height = 75;
uchar *image;

void make_image() {
  image = new uchar[3*width*height];
  uchar *p = image;
  for (int y = 0; y < height; y++) {
    double Y = double(y)/(height-1);
    for (int x = 0; x < width; x++) {
      double X = double(x)/(width-1);
      *p++ = uchar(255*((1-X)*(1-Y))); // red in upper-left
      *p++ = uchar(255*((1-X)*Y));	// green in lower-left
      *p++ = uchar(255*(X*Y));	// blue in lower-right
    }
  }
}

class Pens : public Fl_Box
{
  void draw();
public:
  Pens(int X, int Y, int W, int H, const char* L)
  : Fl_Box(X,Y,W,H,L) {}
};

void Pens::draw() {
  // use every color in the gray ramp:
  for (int i = 0; i < 3*8; i++) {
    fl_color((Fl_Color)(FL_GRAY_RAMP+i));
    fl_line(x()+i, y(), x()+i, y()+h());
  }
}

Fl_Color color_global = FL_GRAY;
#define fullcolor_cell (FL_FREE_COLOR)

void cb1(Fl_Widget *, void *v) {
  color_global = fl_show_colormap(color_global);
  Fl_Box* b = (Fl_Box*)v;
  b->color(color_global);
  b->parent()->redraw();
}

void cb2(Fl_Widget *, void *v) {
  uchar r,g,b;
  Fl::get_color(color_global,r,g,b);
  if (!fl_color_chooser("New color:",r,g,b,3))
    return;
  color_global = fullcolor_cell;
  Fl::set_color(fullcolor_cell,r,g,b);
  Fl_Box* bx = (Fl_Box*)v;
  bx->color(fullcolor_cell);
  bx->parent()->redraw();
}

int paleta_colores() {
  Fl::set_color(fullcolor_cell,145,159,170);
  Fl_Window* window = new Fl_Window(400,400,"Color para el fondo del diagrama:");
  Fl_Box* box = new Fl_Box(50,50,300,300);
  box->box(FL_THIN_DOWN_BOX);
  color_global = fullcolor_cell;
  box->color(color_global);
  Fl_Button* b1 = new Fl_Button(140,120,120,30,"fl_show_colormap");
  b1->callback(cb1,&box);
  Fl_Button* b2 = new Fl_Button(140,160,120,30,"fl_choose_color");
  b2->callback(cb2,&box);
  Fl_Box* image_box = new Fl_Box(140,200,120,120,0);
  make_image();
  (new Fl_RGB_Image(image, width, height))->label(image_box);
  Pens* p = new Pens(80,200,3*8,120,"lineas");
  p->align(FL_ALIGN_TOP);
  window->show();
  return 0;
}
