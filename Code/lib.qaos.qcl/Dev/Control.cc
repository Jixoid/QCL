/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define ef else if

#include <iostream>
#include <memory>

#include "Basis.h"

#include "JConf.h"

#include "qcl/Control.hh"
#include "qcl/Graphic.hh"
#include "qcl/View.hh"

using namespace std;
using namespace jix;



u64 GetTickCount()
{
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  
  return static_cast<u64>(tv.tv_sec) *1000 +tv.tv_usec /1000;
}


namespace jix::qcl
{
  control::control()
  {
    Surface = make_shared<qcl::surface>(Size.W, Size.H);
  }

  control::~control()
  {}



  inline void __recalc_bound(control* Self)
  {
    Self->EndPoit.X = Self->Poit.X +Self->Size.W;
    Self->EndPoit.Y = Self->Poit.Y +Self->Size.H;
  }


  void control::Poit_Set(poit_i32 Val)
  {
    Poit = Val;
    __recalc_bound(this);

    if (Parent != Nil)
      Parent->ControlFlags |= controlFlags::MustDraw;
  }
  
  void control::Size_Set(size_i32 Val)
  {
    Size = Val;
    __recalc_bound(this);

    Do_Resize();

    if (Parent != Nil)
      Parent->ControlFlags |= controlFlags::MustDraw;

  }


  void control::Draw_before()
  {
    Surface->Clear();
  }

  void control::Draw_after()
  {}

  void control::Draw()
  {}


  control* control::GetRoot()
  {
    control* This = this;

    while (This->Parent != Nil)
      This = This->Parent;

    return This;
  }

  void control::DyeToRoot()
  {
    control* This = this;
    This->ControlFlags |= controlFlags::MustDraw;

    while (This->Parent != Nil)
    {
      This = This->Parent;

      This->ControlFlags |= controlFlags::MustDraw;
    }
  }


  string __jc_ValGet(jc_val Val)
  {
    if (Val == Nil)
      return "";

    if (!jc_IsVal(Val))
      return "";

    
    string Ret;
    char *CStr;
    CStr = jc_ValGet(Val);
    Ret = string(CStr);
    jc_DisStr(CStr);

    return Ret;
  }



  control* __findInScope(control* Self, string Name)
  {
    if (Self->Parent == Nil)
      return Nil;


    if (Self->Parent->Name == Name)
      return Self->Parent;

    
    for (auto &X: Self->Parent->Childs)
      if (X.get() != Self && X->Name == Name)
        return X.get();

    return Nil;
  }


  bool control::LoadProp(string Name, jc_obj Prop)
  {
    jc_obj Buf;
    string Cac;

    if (Name == "Poit")
    {
      poit_i32 Nat = this->Poit;

      if (!jc_IsStc(Prop))
        return false;


      // X
      Cac = __jc_ValGet(jc_StcGet(Prop, "X"));
      if (!Cac.empty())
      {
        auto eret = from_chars(Cac.data(), Cac.data() +Cac.size(), Nat.X);
        if (eret.ec != std::errc())
          return false;
      }

      // Y
      Cac = __jc_ValGet(jc_StcGet(Prop, "Y"));
      if (!Cac.empty())
      {
        auto eret = from_chars(Cac.data(), Cac.data() +Cac.size(), Nat.Y);
        if (eret.ec != std::errc())
          return false;
      }
      

      Poit_Set(Nat);
      return true;
    }

    ef (Name == "Size")
    {
      size_i32 Nat = this->Size;
      
      if (!jc_IsStc(Prop))
        return false;


      // W
      Cac = __jc_ValGet(jc_StcGet(Prop, "W"));
      if (!Cac.empty())
      {
        auto eret = from_chars(Cac.data(), Cac.data() +Cac.size(), Nat.W);
        if (eret.ec != std::errc())
          return false;
      }


      // H
      Cac = __jc_ValGet(jc_StcGet(Prop, "H"));
      if (!Cac.empty())
      {
        auto eret = from_chars(Cac.data(), Cac.data() +Cac.size(), Nat.H);
        if (eret.ec != std::errc())
          return false;
      }
      

      Size_Set(Nat);
      return true;
    }

    ef (Name == "Tag")
    {
      uPtr Nat = this->Tag;
      
      if (!jc_IsVal(Prop))
        return false;


      // 
      Cac = __jc_ValGet(Prop);
      if (Cac.empty())
        return false;

      auto eret = from_chars(Cac.data(), Cac.data() +Cac.size(), Nat);
      if (eret.ec != std::errc())
        return false;


      Tag = (Nat);
      return true;
    }

    ef (Name == "Visible")
    {
      bool Nat = this->Visible;
      
      if (!jc_IsVal(Prop))
        return false;


      // 
      Cac = __jc_ValGet(Prop);
      if (Cac.empty())
        return false;

      else
      {
        if (Cac == "1" || Cac == "true")
          Nat = true;

        ef (Cac == "0" || Cac == "false")
          Nat = false;

        else
          return false;
      }

      Visible = (Nat);
      return true;
    }

    ef (Name == "Enabled")
    {
      bool Nat = this->Enabled;
      
      if (!jc_IsVal(Prop))
        return false;


      // 
      Cac = __jc_ValGet(Prop);
      if (Cac.empty())
        return false;

      else
      {
        if (Cac == "1" || Cac == "true")
          Nat = true;

        ef (Cac == "0" || Cac == "false")
          Nat = false;

        else
          return false;
      }


      Enabled = (Nat);
      return true;
    }

    ef (Name == "Anchors")
    {
      controlAnchors Nat = this->Anchors;

      if (!jc_IsStc(Prop))
        return false;


      auto Handler = [](control* This, jc_obj Stc, controlAnchor* Anchor) -> bool
      {
        if (!jc_IsStc(Stc))
          return false;

        string Cac;
        
        // Active
        Cac = __jc_ValGet(jc_StcGet(Stc, "Active"));
        if (!Cac.empty())
        {
          if (Cac == "1" || Cac == "true")
            Anchor->Active = true;

          ef (Cac == "0" || Cac == "false")
            Anchor->Active = false;

          else
            return false;
        }


        // Control
        Cac = __jc_ValGet(jc_StcGet(Stc, "Control"));
        if (!Cac.empty())
        {
          if (Cac[0] != '@')
            return false;

          Anchor->Control = __findInScope(This, Cac.substr(1));

          if (Anchor->Control == Nil)
            return false;
        }


        // Size
        Cac = __jc_ValGet(jc_StcGet(Stc, "Side"));
        if (!Cac.empty())
        {
          if (Cac == "begin")
            Anchor->Side = controlAnchorSide::casBegin;

          ef (Cac == "end")
            Anchor->Side = controlAnchorSide::casEnd;

          else
            return false;
        }

        
        return true;
      };



      Buf = jc_StcGet(Prop, "Left");
      if (Buf != Nil)
        if (!Handler(this, Buf, &Nat.Left))
          return false;

      Buf = jc_StcGet(Prop, "Top");
      if (Buf != Nil)
        if (!Handler(this, Buf, &Nat.Top))
          return false;

      Buf = jc_StcGet(Prop, "Righ");
      if (Buf != Nil)
        if (!Handler(this, Buf, &Nat.Righ))
          return false;

      Buf = jc_StcGet(Prop, "Bot");
      if (Buf != Nil)
        if (!Handler(this, Buf, &Nat.Bot))
          return false;


      Anchors = (Nat);
      return true;
    }

    ef (Name == "Margins")
    {
      rect_i32 Nat = this->Margins;
      
      if (!jc_IsStc(Prop))
        return false;


      // Left
      Cac = __jc_ValGet(jc_StcGet(Prop, "Left"));
      if (!Cac.empty())
      {
        auto eret = from_chars(Cac.data(), Cac.data() +Cac.size(), Nat.X1);
        if (eret.ec != std::errc())
          return false;
      }


      // Top
      Cac = __jc_ValGet(jc_StcGet(Prop, "Top"));
      if (!Cac.empty())
      {
        auto eret = from_chars(Cac.data(), Cac.data() +Cac.size(), Nat.Y1);
        if (eret.ec != std::errc())
          return false;
      }


      // Bot
      Cac = __jc_ValGet(jc_StcGet(Prop, "Bot"));
      if (!Cac.empty())
      {
        auto eret = from_chars(Cac.data(), Cac.data() +Cac.size(), Nat.Y2);
        if (eret.ec != std::errc())
          return false;
      }


      // Right
      Cac = __jc_ValGet(jc_StcGet(Prop, "Righ"));
      if (!Cac.empty())
      {
        auto eret = from_chars(Cac.data(), Cac.data() +Cac.size(), Nat.X2);
        if (eret.ec != std::errc())
          return false;
      }


      Margins = (Nat);
      return true;
    }

    else
      return false;
  }

  bool control::LoadFunc(string Name, point Func)
  {
    #define makro(X)  if (Name == #X) { X = (decltype(X))Func; return true; }

         makro(OnPaint)
    else makro(OnResize)
    else makro(OnClick)
    else makro(OnDblClick)
    else makro(OnMouseDown)
    else makro(OnMouseUp)
    else makro(OnMouseMove)
    else makro(OnScrollVert)
    else makro(OnScrollHorz)
    else makro(OnStateChanged)

    #undef makro

    return false;
  }


  void control::Handler_Message(controlMessages Msg)
  {
    switch (Msg)
    {
      case controlMessages::cmPaint: Handler_Paint(); break;
    }
  }

  void control::Handler_Paint()
  {
    Do_Paint();
  }

  void control::Handler_Resize()
  {
    Size_Set(size_i32{Size.W, Size.H});

    Do_Paint();
  }

  void control::Handler_MouseDown(poit_i32 Pos, mouseButtonSet Button, mouseButtonSet State)
  {
    Do_MouseDown(Pos, Button, State);
  }
  
  void control::Handler_MouseUp(poit_i32 Pos, mouseButtonSet Button, mouseButtonSet State)
  {
    Do_MouseUp(Pos, Button, State);
  }

  void control::Handler_MouseMove(poit_i32 Pos, mouseButtonSet State)
  {
    Do_MouseMove(Pos, State);
  }

  void control::Handler_ScrollVert(poit_i32 Pos, i16 Delta, mouseButtonSet State)
  {
    Do_ScrollVert(Pos, Delta, State);
  }

  void control::Handler_ScrollHorz(poit_i32 Pos, i16 Delta, mouseButtonSet State)
  {
    Do_ScrollHorz(Pos, Delta, State);
  }

  void control::Handler_StateChanged(controlStateSet State)
  {
    ControlState = State;

    Do_StateChanged(State);
  }



  void control::Do_Paint()
  {
    Draw_before();

    Draw();

    if (OnPaint != Nil)
      OnPaint(this);

    Draw_after();

    if (!Enabled)
      Surface->Filter_GrayScale({0,0, (f32)Size.W, (f32)Size.H});

    Surface->Flush();

    ControlFlags &= ~controlFlags::MustDraw;
  }

  void control::Do_Resize()
  {
    Surface->Set_Size(Size.W, Size.H);

    ControlFlags |= controlFlags::MustDraw;

    if (OnResize != Nil)
      OnResize(this);
  }

  void control::Do_MouseDown(poit_i32 Pos, mouseButtonSet Button, mouseButtonSet State)
  {
    if (OnMouseDown != Nil)
      OnMouseDown(this, Pos, Button, State);
  }

  void control::Do_MouseUp(poit_i32 Pos, mouseButtonSet Button, mouseButtonSet State)
  {
    if (OnMouseUp != Nil)
      OnMouseUp(this, Pos, Button, State);


    // Click
    if (State != 0 || Button != mouseButtons::btnLeft)
      return;

    if (
      Pos.X < 0 ||
      Pos.Y < 0 ||
      Pos.X > Size.W ||
      Pos.Y > Size.H
    )
      return;


    if (OnDblClick == Nil || GetTickCount() -LastClick > 300)
    {
      if (OnClick != Nil)
        OnClick(this);

      LastClick = GetTickCount();
    }
    else
    {
      if (OnDblClick != Nil)
        OnDblClick(this);
    }

  }

  void control::Do_MouseMove (poit_i32 Pos, mouseButtonSet State)
  {
    if (OnMouseMove != Nil)
      OnMouseMove(this, Pos, State);
  }

  void control::Do_ScrollVert(poit_i32 Pos, i16 Delta, mouseButtonSet State)
  {
    if (OnScrollVert != Nil)
      OnScrollVert(this, Pos, Delta, State);
  }

  void control::Do_ScrollHorz(poit_i32 Pos, i16 Delta, mouseButtonSet State)
  {
    if (OnScrollHorz != Nil)
      OnScrollHorz(this, Pos, Delta, State);
  }

  void control::Do_StateChanged(controlStateSet State)
  {
    if (OnStateChanged != Nil)
      OnStateChanged(this, State);
  }

}
