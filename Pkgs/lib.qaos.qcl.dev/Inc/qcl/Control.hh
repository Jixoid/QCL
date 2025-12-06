/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include <memory>
#include <vector>

#include <sys/time.h>

#include "Basis.h"

#include "JConf.h"
#include "JConf.hh"

#include "qcl/Graphic.hh"
#include "qcl/Effect.hh"
#include "qcl/Input.hh"

using namespace std;
using namespace jix;



namespace qcl
{
  class view;  // Forward
  class popup; // Forward


  enum controlFlags: u32
  {
    MustTiling   = (u32)1 << 0,
    MustAutoSize = (u32)1 << 1,
    MustResize   = (u32)1 << 2,
    MustRebound  = (u32)1 << 3,
    MustDraw     = (u32)1 << 4,
  };
  using controlFlagSet = u32;


  enum controlStates: u16
  {
    csHover  = (u16)1 << 0,
    csFocus  = (u16)1 << 1,
  };
  using controlStateSet = u16;


  class control;

  enum controlAnchorSide
  {
    casBegin = 1,
    casEnd   = 2,
  };

  struct controlAnchor
  {
    bool Active;
    control *Control;
    controlAnchorSide Side = controlAnchorSide::casBegin;
  };

  struct controlAnchors
  {
    controlAnchor Left, Top, Righ, Bot;
  };


  enum controlMessages: u32
  {
    cmPaint = 1,

    cmMouseDown = 2,  // for Check
    cmMouseUp   = 3,  // for Check
    cmMouseMove = 4,  // for Check
  };


  class control
  {
    public:
      control();
      virtual ~control();

    public:
      shared_ptr<surface> Surface;
      poit_i32 Poit = {0, 0};
      size_i32 Size = {20, 10};
      poit_i32 EndPoit = {20,10};

      view *Parent = Nil;
      
      shared_ptr<popup> Popup;

      bool AutoSize = false;
      bool Transparent = false;

      string Name = "";
      vector<string> Style;
      vector<shared_ptr<effect>> Effects;

      controlFlagSet  ControlFlags = (MustDraw | MustAutoSize | MustTiling);
      controlStateSet ControlState = 0;

      rect_i32 Margins = {0,0,0,0};
      controlAnchors Anchors = {{false, Nil},{false, Nil},{false, Nil},{false, Nil}};

      bool Visible = true;
      bool Enabled = true;

      u0 Tag = 0;
      jc_stc QDL = Nil;

      u64 LastClick = 0;


    public:
      virtual void Draw_before();
      virtual void Draw_after();
      virtual void Draw();

      virtual size_i32 CalcAutoSize();

      bool Show_Popup(poit_f32 Poit);

      control* GetRoot();
      void DyeToRoot(controlFlags Flag = MustDraw);

      void Flag_Add(controlFlagSet Flag);
      void Flag_Rem(controlFlagSet Flag);


      virtual bool LoadProp(string Name, const jconf::Value& Prop);
      virtual bool LoadFunc(string Name, point Func);
      

    public:
      void (*OnPaint)       (qcl::control*) = Nil;
      void (*OnPaint_before)(qcl::control*) = Nil;
      void (*OnPaint_after) (qcl::control*) = Nil;
      void (*OnResize)      (qcl::control*) = Nil;
      
      void (*OnClick)       (qcl::control*) = Nil;
      void (*OnClickEx)     (qcl::control*, poit_i32 Pos) = Nil;
      void (*OnDblClick)    (qcl::control*) = Nil;
      
      void (*OnMouseDown)   (qcl::control*, poit_i32 Pos, shiftStateSet Button, shiftStateSet State) = Nil;
      void (*OnMouseUp)     (qcl::control*, poit_i32 Pos, shiftStateSet Button, shiftStateSet State) = Nil;
      void (*OnMouseMove)   (qcl::control*, poit_i32 Pos, shiftStateSet State) = Nil;

      void (*OnKeyDown)     (qcl::control*, char *Key, u32 KeyCode, shiftStateSet State) = Nil;
      void (*OnKeyUp)       (qcl::control*, char *Key, u32 KeyCode, shiftStateSet State) = Nil;
      
      void (*OnScrollVert)  (qcl::control*, poit_i32 Pos, i16 Delta, shiftStateSet State) = Nil;
      void (*OnScrollHorz)  (qcl::control*, poit_i32 Pos, i16 Delta, shiftStateSet State) = Nil;
      
      void (*OnStateChanged)(qcl::control*, controlStateSet State) = Nil;


      virtual void Handler_Message     (controlMessages Msg);
      virtual void Handler_Paint       (); 
      virtual void Handler_Resize      ();
      virtual void Handler_MouseDown   (poit_i32 Pos, shiftStateSet Button, shiftStateSet State);
      virtual void Handler_MouseUp     (poit_i32 Pos, shiftStateSet Button, shiftStateSet State);
      virtual void Handler_MouseMove   (poit_i32 Pos, shiftStateSet State);
      virtual void Handler_KeyDown     (char *Key, u32 KeyCode, shiftStateSet State);
      virtual void Handler_KeyUp       (char *Key, u32 KeyCode, shiftStateSet State);
      virtual void Handler_ScrollVert  (poit_i32 Pos, i16 Delta, shiftStateSet State);
      virtual void Handler_ScrollHorz  (poit_i32 Pos, i16 Delta, shiftStateSet State);
      virtual void Handler_StateChanged(controlStateSet State);


      virtual void Do_Paint_prepare();
      virtual void Do_Paint        ();
      virtual void Do_Resize       ();
      virtual void Do_Click        ();
      virtual void Do_ClickEx      (poit_i32 Pos);
      virtual void Do_DblClick     ();
      virtual void Do_MouseDown    (poit_i32 Pos, shiftStateSet Button, shiftStateSet State);
      virtual void Do_MouseUp      (poit_i32 Pos, shiftStateSet Button, shiftStateSet State);
      virtual void Do_MouseMove    (poit_i32 Pos, shiftStateSet State);
      virtual void Do_KeyDown      (char *Key, u32 KeyCode, shiftStateSet State);
      virtual void Do_KeyUp        (char *Key, u32 KeyCode, shiftStateSet State);
      virtual void Do_ScrollVert   (poit_i32 Pos, i16 Delta, shiftStateSet State);
      virtual void Do_ScrollHorz   (poit_i32 Pos, i16 Delta, shiftStateSet State);
      virtual void Do_StateChanged (controlStateSet State);
  };

}
