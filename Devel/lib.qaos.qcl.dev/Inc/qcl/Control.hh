/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include <iostream>
#include <memory>
#include <chrono>
#include <vector>

#include <sys/time.h>

#include "Basis.h"

#include "JConf.h"

#include "qcl/Graphic.hh"
#include "qcl/Input.hh"

using namespace std;
using namespace jix;



namespace jix::qcl
{
  class view;  // Forward


  enum controlFlags: u32
  {
    MustDraw = (u32)1 << 0,
  };
  using controlFlagSet = u32;


  enum controlStates: u16
  {
    csNormal = (u16)1 << 0,
    csHover  = (u16)1 << 1,
    csOver   = (u16)1 << 2,
    csFocus  = (u16)2 << 3,
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

      string Name = "";
      vector<string> Style;

      controlFlagSet  ControlFlags = (MustDraw);
      controlStateSet ControlState = controlStates::csNormal;

      rect_i32 Margins = {0,0,0,0};
      controlAnchors Anchors = {{false, Nil},{false, Nil},{false, Nil},{false, Nil}};

      bool Visible = true;
      bool Enabled = true;

      uPtr Tag = 0;
      jc_stc QDL = Nil;

      u64 LastClick = 0;


    public:
      virtual void Poit_Set(poit_i32 Val);
      virtual void Size_Set(size_i32 Val);

      virtual void Draw_before();
      virtual void Draw_after();
      virtual void Draw();


      control* GetRoot();
      void DyeToRoot();

      virtual bool LoadProp(string Name, jc_obj Prop);
      virtual bool LoadFunc(string Name, point Func);


    public:
      void (*OnPaint)     (qcl::control*) = Nil;
      void (*OnResize)    (qcl::control*) = Nil;
      
      void (*OnClick)     (qcl::control*) = Nil;
      void (*OnDblClick)     (qcl::control*) = Nil;
      
      void (*OnMouseDown) (qcl::control*, poit_i32 Pos, mouseButtonSet Button, mouseButtonSet State) = Nil;
      void (*OnMouseUp)   (qcl::control*, poit_i32 Pos, mouseButtonSet Button, mouseButtonSet State) = Nil;
      void (*OnMouseMove) (qcl::control*, poit_i32 Pos, mouseButtonSet State) = Nil;
      
      void (*OnScrollVert)(qcl::control*, poit_i32 Pos, i16 Delta, mouseButtonSet State) = Nil;
      void (*OnScrollHorz)(qcl::control*, poit_i32 Pos, i16 Delta, mouseButtonSet State) = Nil;
      
      void (*OnStateChanged)(qcl::control*, controlStateSet State) = Nil;


      virtual void Handler_Message(controlMessages Msg);
      virtual void Handler_Paint(); 
      virtual void Handler_Resize();
      virtual void Handler_MouseDown (poit_i32 Pos, mouseButtonSet Button, mouseButtonSet State);
      virtual void Handler_MouseUp   (poit_i32 Pos, mouseButtonSet Button, mouseButtonSet State);
      virtual void Handler_MouseMove (poit_i32 Pos, mouseButtonSet State);
      virtual void Handler_ScrollVert(poit_i32 Pos, i16 Delta, mouseButtonSet State);
      virtual void Handler_ScrollHorz(poit_i32 Pos, i16 Delta, mouseButtonSet State);
      virtual void Handler_StateChanged(controlStateSet State);


      virtual void Do_Paint();
      virtual void Do_Resize();
      virtual void Do_MouseDown (poit_i32 Pos, mouseButtonSet Button, mouseButtonSet State);
      virtual void Do_MouseUp   (poit_i32 Pos, mouseButtonSet Button, mouseButtonSet State);
      virtual void Do_MouseMove (poit_i32 Pos, mouseButtonSet State);
      virtual void Do_ScrollVert(poit_i32 Pos, i16 Delta, mouseButtonSet State);
      virtual void Do_ScrollHorz(poit_i32 Pos, i16 Delta, mouseButtonSet State);
      virtual void Do_StateChanged(controlStateSet State);
  };

}
