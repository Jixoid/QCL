/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <iostream>
#include <memory>
#include <vector>
#include <cmath>

extern "C"
{
  #include <X11/Xlib.h>
  #include <stdlib.h>

  #include <cairo/cairo.h>
  #include <cairo/cairo-xlib.h>
}

#include "Basis.h"

#include "qcl/Control.hh"
#include "qcl/View.hh"
#include "qcl/Window.hh"
#include "qcl/Graphic.hh"
#include "qcl/Platform.hh"

using namespace std;
using namespace jix;


struct x11_handle
{
  Window   window;
  int      screen;
  int      width, height;
};

struct cairo_handle
{
  cairo_surface_t *Sur;
  cairo_t *Car;

  Window   Win;
};

struct app_handle
{
  Display *dpy;
  Window   win;
};


#define XSelect (ExposureMask | ButtonPressMask  | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask)

#define __CurrentApp ((app_handle*)CurrentApp->OHID)


namespace jix::qcl::platform::window
{
  void New(handle *ret)
  {
    x11_handle* h = (x11_handle*)calloc(1, sizeof(x11_handle));
    *ret = (handle)h;

    h->screen = DefaultScreen(__CurrentApp->dpy);
    h->width = 800;
    h->height = 600;

    XSetWindowAttributes attrs;
    attrs.background_pixmap = None;
    attrs.event_mask = XSelect;

    h->window = XCreateWindow(
        __CurrentApp->dpy,
        RootWindow(__CurrentApp->dpy, h->screen),
        10, 10,
        h->width, h->height,
        1,
        CopyFromParent,
        InputOutput,
        CopyFromParent,
        CWBackPixmap | CWEventMask,
        &attrs
    );
  }


  #define Native ((x11_handle*)val)

  void Dis(handle  val)
  {
    XDestroyWindow(__CurrentApp->dpy, Native->window);

    free(Native);
  }


  void Show(handle val)
  {
    XMapWindow(__CurrentApp->dpy, Native->window);
    XFlush(__CurrentApp->dpy);
  }

  void Hide(handle val)
  {
    XUnmapWindow(__CurrentApp->dpy, Native->window);
    XFlush(__CurrentApp->dpy);
  }


  void     SizeSet(handle val, size_i32 Size)
  {
    XResizeWindow(__CurrentApp->dpy, Native->window, Size.W, Size.H);
    XFlush(__CurrentApp->dpy);
    Native->width  = Size.W;
    Native->height = Size.H;
  }

  size_i32 SizeGet(handle val)
  {
    return {
      Native->width,
      Native->height
    };
  }


  #undef Native
}


namespace jix::qcl::platform::surface
{
  void New(handle *ret, u32 nWidth, u32 nHeight)
  {
    cairo_handle* h = (cairo_handle*)calloc(1, sizeof(cairo_handle));
    *ret = (handle)h;


    h->Sur = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, nWidth, nHeight);
    h->Car = cairo_create(h->Sur);
  }

  #define Native ((cairo_handle*)val)

  void Dis(handle  val)
  {
    cairo_destroy(Native->Car);
    cairo_surface_destroy(Native->Sur);

    free(Native);
  }


  void Set_Color(handle  val, color Val)
  {
    cairo_set_source_rgba(Native->Car, Val.R, Val.G, Val.B, Val.A);
  }

  void Set_Source(handle  val, shared_ptr<qcl::surface> Val)
  {
    f64 X,Y;
    cairo_get_current_point(Native->Car, &X, &Y);

    cairo_set_source_surface(Native->Car, ((cairo_handle*)Val->OHID)->Sur, X,Y);
  }

  void Set_FontSize(handle  val, f32 Size)
  {
    cairo_set_font_size(Native->Car, Size);
  }

  void Set_LineSize(handle  val, f32 Size)
  {
    cairo_set_line_width(Native->Car, Size);
  }


  void Set_Pos(handle  val, poit_f32 Val)
  {
    cairo_move_to(Native->Car, Val.X, Val.Y);
  }

  void Set_rPos(handle  val, poit_f32 Val)
  {
    cairo_rel_move_to(Native->Car, Val.X, Val.Y);
  }

  void Set_Size(handle  val, u32 nWidth, u32 nHeight)
  {
    cairo_destroy(Native->Car);
    cairo_surface_destroy(Native->Sur);


    if (Native->Win == 0)
    {
      Native->Sur = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, nWidth, nHeight);
      Native->Car = cairo_create(Native->Sur);
    }

    else
    {
      XWindowAttributes Attrs;
      XGetWindowAttributes(__CurrentApp->dpy, Native->Win, &Attrs);

      Native->Sur = cairo_xlib_surface_create(
        __CurrentApp->dpy,
        Native->Win,
        DefaultVisual(
          __CurrentApp->dpy,
          DefaultScreen(__CurrentApp->dpy)
        ),
        Attrs.width,
        Attrs.height
      );

      Native->Car = cairo_create(Native->Sur);
    }
  }


  void Draw_Rect(handle  val, rect_f32 Area)
  {
    cairo_rectangle(Native->Car, Area.X1, Area.Y1, Area.X2 -Area.X1, Area.Y2 -Area.Y1);
  }

  void Draw_RectRound(handle  val, rect_f32 Area, f32 Radius)
  {
    if (Radius > (Area.Y2-Area.Y1) /2)
      Radius = (Area.Y2-Area.Y1) /2;

    if (Radius > (Area.X2-Area.X1) /2)
      Radius = (Area.X2-Area.X1) /2;


    cairo_new_path(Native->Car);

    cairo_arc(Native->Car, Area.X1 +Radius, Area.Y1 +Radius, Radius, M_PI, 3 * M_PI / 2);
    cairo_line_to(Native->Car, Area.X2 -Radius, Area.Y1);
    cairo_arc(Native->Car, Area.X2 -Radius, Area.Y1 +Radius, Radius, 3 * M_PI / 2, 0);
    cairo_line_to(Native->Car, Area.X2, Area.Y2 -Radius);
    cairo_arc(Native->Car, Area.X2 -Radius, Area.Y2 -Radius, Radius, 0, M_PI / 2);
    cairo_line_to(Native->Car, Area.X1 +Radius, Area.Y2);
    cairo_arc(Native->Car, Area.X1 +Radius, Area.Y2 -Radius, Radius, M_PI / 2, M_PI);
    cairo_line_to(Native->Car, Area.X1, Area.Y1 +Radius);

    cairo_close_path(Native->Car);
  }

  void Draw_Line(handle  val, poit_f32 P1, poit_f32 P2)
  {
    cairo_move_to(Native->Car, P1.X, P1.Y);

    cairo_line_to(Native->Car, P2.X, P2.Y);
  }

  void Draw_Text(handle  val, const char* Text)
  {
    cairo_text_extents_t Ex;

    cairo_text_extents(Native->Car, Text, &Ex);

    cairo_rel_move_to(Native->Car, 0, -Ex.y_bearing);

    cairo_show_text(Native->Car, Text);
  }


  void Calc_Text(handle  val, const char* Text, size_f32* Size)
  {
    cairo_text_extents_t Ex;

    cairo_text_extents(Native->Car, Text, &Ex);


    *Size = {(f32)Ex.width, (f32)-Ex.y_bearing};
  }


  void Filter_GrayScale(handle  val, rect_f32 Area)
  {
    int width = cairo_image_surface_get_width(Native->Sur);
    int height = cairo_image_surface_get_height(Native->Sur);
    unsigned char* data = cairo_image_surface_get_data(Native->Sur);
    int stride = cairo_image_surface_get_stride(Native->Sur);
    cairo_format_t fmt = cairo_image_surface_get_format(Native->Sur);

    cairo_surface_flush(Native->Sur);

    i32 x1 = std::max(0, (i32)Area.X1);
    i32 y1 = std::max(0, (i32)Area.Y1);
    i32 x2 = std::min(width, (i32)Area.X2);
    i32 y2 = std::min(height, (i32)Area.Y2);

    for (int y = y1; y < y2; ++y)
      for (int x = x1; x < x2; ++x) {
        unsigned char* src = data + y * stride + x * 4;

        // ARGB: [B, G, R, A] in Cairo image surfaces (little endian)
        uint8_t Bc = src[0];
        uint8_t Gc = src[1];
        uint8_t Rc = src[2];
        uint8_t Ac = src[3];

        uint8_t gray = (uint8_t)(0.299 * Rc + 0.587 * Gc + 0.114 * Bc);
        src[0] = gray;
        src[1] = gray;
        src[2] = gray;
        src[3] = Ac;  // alpha'yı bozmadan taşı
      }

    cairo_surface_mark_dirty(Native->Sur);
  }
  

  void Connect(handle  val, handle Window)
  {
    cairo_destroy(Native->Car);
    cairo_surface_destroy(Native->Sur);



    Native->Win = ((x11_handle*)Window)->window;

    XWindowAttributes Attrs;
    XGetWindowAttributes(__CurrentApp->dpy, Native->Win, &Attrs);

    Native->Sur = cairo_xlib_surface_create(
      __CurrentApp->dpy,
      Native->Win,
      DefaultVisual(
        __CurrentApp->dpy,
        DefaultScreen(__CurrentApp->dpy)
      ),
      Attrs.width,
      Attrs.height
    );

    Native->Car = cairo_create(Native->Sur);
  }
  

  void Stroke(handle  val)
  {
    cairo_stroke(Native->Car);
  }

  void Fill(handle  val)
  {
    cairo_fill(Native->Car);
  }

  void Paint(handle  val)
  {
    cairo_paint(Native->Car);
  }


  void Clear(handle  val)
  {
    auto bak = cairo_get_operator(Native->Car);

    int width  = cairo_image_surface_get_width(Native->Sur);
    int height = cairo_image_surface_get_height(Native->Sur);


    cairo_set_operator(Native->Car, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba(Native->Car, 0,0,0,0);
    cairo_rectangle(Native->Car, 0,0, width, height);
    cairo_fill(Native->Car);


    cairo_set_operator(Native->Car, bak);
  }

  void Flush(handle  val)
  {
    cairo_surface_flush(Native->Sur);

    if (Native->Win != 0)
      XFlush(__CurrentApp->dpy);
  }

  #undef Native
}


namespace jix::qcl::platform::application
{
  void New(handle *ret)
  {
    app_handle* h = (app_handle*)calloc(1, sizeof(app_handle));
    *ret = (handle)h;

    h->dpy = XOpenDisplay(NULL);

    h->win = XCreateSimpleWindow(
        h->dpy,
        RootWindow(h->dpy, DefaultScreen(h->dpy)),
        10, 10, 10, 10,
        1,
        BlackPixel(h->dpy, DefaultScreen(h->dpy)),
        WhitePixel(h->dpy, DefaultScreen(h->dpy))
    );

    XSelectInput(h->dpy, h->win, SubstructureNotifyMask | StructureNotifyMask );
  }

  #define Native ((app_handle*)val)

  void Dis(handle  val)
  {
    XDestroyWindow(Native->dpy, Native->win);

    XCloseDisplay(Native->dpy);

    free(Native);
  }



  void SendMessage(qcl::control* Ctrl, controlMessages Msg)
  { 
    XEvent ev = {};
    
    ev.type = ClientMessage;
    ev.xclient.display = __CurrentApp->dpy;
    ev.xclient.window = DefaultRootWindow(__CurrentApp->dpy);
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = *((i32*)&Msg);
    *(uPtr*)(&ev.xclient.data.l[2]) = (uPtr)Ctrl; 


    XSendEvent(__CurrentApp->dpy, __CurrentApp->win, False, NoEventMask, &ev);
    XFlush(__CurrentApp->dpy);
  }


  void Run(handle val) { while (true)
  {
    XEvent Ev;
    XNextEvent(Native->dpy, &Ev);



    if (Ev.type == ClientMessage)
    {
      qcl::control *Ctrl = (qcl::control*)((void*)(*(uPtr*)(&Ev.xclient.data.l[2])));
      controlMessages Msg = *(controlMessages*)&Ev.xclient.data.l[0];

      Ctrl->Handler_Message(Msg);
      continue;
    }
    


    qcl::window* CWin;
    for (auto &[Key, Win]: CurrentApp->WinList)
      if (((x11_handle*)Key)->window == Ev.xany.window)
      {
        CWin = Win;

        goto _l_Event;
      }

    continue;


    _l_Event:

    switch (Ev.type)
    {

      case Expose: // Paint
      {
        CWin->Handler_Paint();
        
        break;
      }

      case ButtonPress: // Mouse Down
      {
        mouseButtonSet LSta = 0;

        if (Ev.xbutton.state & Button1Mask)
          LSta |= mouseButtons::btnLeft;

        if (Ev.xbutton.state & Button2Mask)
          LSta |= mouseButtons::btnMiddle;

        if (Ev.xbutton.state & Button3Mask)
          LSta |= mouseButtons::btnRight;



        mouseButtonSet LBut = 0;

        switch (Ev.xbutton.button)
        {
          case 1: LBut = mouseButtons::btnLeft;     break;
          case 2: LBut = mouseButtons::btnMiddle;   break;
          case 3: LBut = mouseButtons::btnRight;    break;

          case 4:
          case 5: {

            CWin->Handler_ScrollVert(poit_i32{Ev.xbutton.x, Ev.xbutton.y}, (Ev.xbutton.button == 4) ? (10):(-10), LSta);
            continue;
          };

          case 6:
          case 7: {

            CWin->Handler_ScrollHorz(poit_i32{Ev.xbutton.x, Ev.xbutton.y}, (Ev.xbutton.button == 6) ? (10):(-10), LSta);
            continue;
          };

          
          default: continue;
        }
        
        LSta &= ~LBut;

        CWin->Handler_MouseDown(poit_i32{Ev.xbutton.x, Ev.xbutton.y}, LBut, LSta);
        break;
      }

      case ButtonRelease: // Mouse Up
      {
        mouseButtonSet LSta = 0;

        if (Ev.xbutton.state & Button1Mask)
          LSta |= mouseButtons::btnLeft;

        if (Ev.xbutton.state & Button2Mask)
          LSta |= mouseButtons::btnMiddle;

        if (Ev.xbutton.state & Button3Mask)
          LSta |= mouseButtons::btnRight;



        mouseButtonSet LBut = 0;

        switch (Ev.xbutton.button)
        {
          case 1: LBut = mouseButtons::btnLeft;     break;
          case 2: LBut = mouseButtons::btnMiddle;   break;
          case 3: LBut = mouseButtons::btnRight;    break;

          default: continue;
        }

        LSta &= ~LBut;

        CWin->Handler_MouseUp(poit_i32{Ev.xbutton.x, Ev.xbutton.y}, LBut, LSta);
        break;
      }

      case MotionNotify: // Mouse Move
      {
        mouseButtonSet LSta = 0;

        if (Ev.xmotion.state & Button1Mask)
          LSta |= mouseButtons::btnLeft;

        if (Ev.xmotion.state & Button2Mask)
          LSta |= mouseButtons::btnMiddle;

        if (Ev.xmotion.state & Button3Mask)
          LSta |= mouseButtons::btnRight;


        CWin->Handler_MouseMove(poit_i32{Ev.xmotion.x, Ev.xmotion.y}, LSta);
        break;
      }  

      case ConfigureNotify: // Configure: Resize (change bounds)
      {

        if ((Ev.xconfigure.width != CWin->LastSize.W) || (Ev.xconfigure.height != CWin->LastSize.H))
        {
          CWin->LastSize = {Ev.xconfigure.width, Ev.xconfigure.height};
          CWin->Size = CWin->LastSize;

          CWin->Handler_Resize();
        }

        break;
      }

    }
  }}

  #undef Native
}


#undef __CurrentApp

