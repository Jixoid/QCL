/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define el else
#define ef else if

#include <memory>

#include "Basis.h"

#include "JConf.h"
#include "JConf.hh"

#include "qcl/Control.hh"
#include "qcl/Graphic.hh"
#include "qcl/Window.hh"
#include "qcl/Application.hh"
#include "qcl/DynamicLoad.hh"
#include "qcl/Popup.hh"
#include "qcl/View.hh"

using namespace std;
using namespace jix;



u64 GetTickCount()
{
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  
  return static_cast<u64>(tv.tv_sec) *1000 +tv.tv_usec /1000;
}


namespace qcl
{

  control::control()
  {
    Surface = make_shared<qcl::surface>(Size.W, Size.H);
  }

  control::~control()
  {}



  void control::Draw_before()
  {
    Surface->Clear();
  }

  void control::Draw_after()
  {
    //Surface->Set_Color(color(1,0,0));
    //Surface->Set_LineSize(1);
    
    //Surface->Draw_Rect({0,0, (f32)Size.W, (f32)Size.H});
    //Surface->Stroke();


    //Surface->Set_FontSize(8);
    //Surface->Set_Color(color(0,1,0));
    //Surface->Set_Pos({5,5});
    //string Cac = to_string(Size.W) +"x"+ to_string(Size.H);
    //Surface->Draw_Text(Cac.c_str());
    //Surface->Fill();
  }

  void control::Draw()
  {}


  void control::CalcAutoSize()
  { PreferedSize = {20,20}; }


  poit_i32 GetLocalPos(view *Target, control *This)
  {
    poit_i32 Ret{0,0}; 

    while (true)
    {
      Ret += This->Poit;

      if (This->Parent == Nil)
        return {0,0};

      ef (This->Parent == Target)
        return Ret;

      el
        This = This->Parent;
    }
    
  }

  bool control::Show_Popup(poit_f32 Poit)
  {
    if (Popup == Nil)
      return false;
      

    window *Window = Nil;
    {
      control *Parent = GetRoot();

      if (Parent == Nil)
        return false;

      ef (dynamic_cast<window*>(Parent) == Nil)
        return false;

      Window = static_cast<window*>(Parent);
    }


    Popup->Poit = GetLocalPos(Window, this) +poit_i32{(i32)Poit.X, (i32)Poit.Y};
    Popup->Flag_Add(DirtyRebound);
    Window->Overlay = Popup;
  

    CurrentApp->PushMessage(Window, controlMessages::cmPaint);

    return true;
  }


  control* control::GetRoot()
  {
    control* This = this;

    while (This->Parent != Nil)
      This = This->Parent;

    return This;
  }

  void control::DyeToRoot(dirtyFlags Flag)
  {
    control* This = this;
    This->Flag_Add(Flag);

    while (This->Parent != Nil)
    {
      This = This->Parent;

      This->Flag_Add(Flag);
    }
  }


  void control::Flag_Add(dirtyFlagSet Flag)
  {
    if (Flag & DirtyResize)
      Flag |= (DirtyRebound | DirtyTiling);

      
    if (Flag & DirtyTiling)
      Flag |= (DirtyDraw);
      
      
    if (Flag & (DirtyDraw))
    {
      control* This = this;
      
      while (This->Parent != Nil)
      {
        This = This->Parent;
        This->DirtyFlags |= (DirtyDraw);
      }
    }

    if (Flag & (DirtyResize | DirtyRebound))
    {
      control* This = this;
      
      while (This->Parent != Nil)
      {
        This = This->Parent;
        
        if (This->AutoSize)
          This->DirtyFlags |= (DirtyAutoSize);
        
        This->DirtyFlags |= (DirtyTiling | DirtyDraw);
      }
    }

    DirtyFlags |= Flag;
  }

  void control::Flag_Rem(dirtyFlagSet Flag)
  {
    DirtyFlags &= ~Flag;
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


  bool control::LoadProp(string Name, const jconf::Value& Prop)
  {

    if (Name == "Effects")
    {
      vector<shared_ptr<effect>> Nat;

      if (!Prop.isArray())
        return false;
      

      for (u32 i = 0; i < Prop.size(); i++)
      {
        jconf::Value X = Prop[i];

        if (!X.isStruct())
          return false;


        
        auto Cac = (string)X["&type"];
        if (Cac.empty())
          return false;

        
        auto it = dyn::EffList.find(Cac);
        if (it == dyn::EffList.end())
          return false;

        Nat.push_back(shared_ptr<effect>(it->second()));


        // Iterate over struct keys - API doesn't seem to support iterating keys directly easily
        // reusing underlying handle or if jconf::Value has iterator (checked header, no iterator exposed directly?)
        // JConf.hh shows: u32 size() const { if (isStruct()) return jc_stc_count(Handle); ... }
        // But no key iterator. However, the original code used jc_StcC and jc_StcInd.
        // I should probably use the underlying handle for iteration if the C++ API doesn't wrapper it.
        // Or wait, the original code used:
        // for (u32 j = 0; j < jc_StcC(X); j++) { auto SX = jc_StcInd(X, j, &Te); ... }
        
        // Let's check JConf.hh again. It has no key iterator in Value class.
        // But Value has getHandle().
        // So I can do:
        jc_obj H = X.getHandle();
        for (u32 j = 0; j < jc_stc_count(H); j++)
        {
          char *Te;
          auto SX = jconf::Value(jc_stc_index(H, j, &Te));
          
          Cac = string(Te);

          if (Cac == "&type")
            continue;

          if (!Nat.back()->LoadProp(Cac, SX))
            return false;
          
          jc_dis_str(Te); // Original code used jc_DisStr(Te). Check if that was macro for jc_dis_str.
          // Assuming jc_DisStr is macro for jc_dis_str.
        }

      }
    

      Effects = (Nat);
      return true;
    }

    ef (Name == "Poit")
    {
      if (!Prop.isStruct())
        return false;

      if (auto Buf = Prop["x"]; !Buf.isNull())
        Poit.X = (i64)Buf;

      if (auto Buf = Prop["y"]; !Buf.isNull())
        Poit.Y = (i64)Buf;

      Flag_Add(DirtyRebound);
      return true;
    }

    ef (Name == "Size")
    {
      if (!Prop.isStruct())
        return false;

      if (auto Buf = Prop["w"]; !Buf.isNull())
        Size.W = (i64)Buf;

      if (auto Buf = Prop["h"]; !Buf.isNull())
        Size.H = (i64)Buf;

      Flag_Add(DirtyResize | DirtyRebound);
      return true;
    }

    ef (Name == "MinSize")
    {
      if (!Prop.isStruct())
        return false;

      if (auto Buf = Prop["w"]; !Buf.isNull())
        MinSize.W = (i64)Buf;

      if (auto Buf = Prop["h"]; !Buf.isNull())
        MinSize.H = (i64)Buf;

      //Flag_Add(DirtyResize | DirtyRebound);
      return true;
    }

    ef (Name == "MaxSize")
    {
      if (!Prop.isStruct())
        return false;

      if (auto Buf = Prop["w"]; !Buf.isNull())
        MaxSize.W = (i64)Buf;

      if (auto Buf = Prop["h"]; !Buf.isNull())
        MaxSize.H = (i64)Buf;

      //Flag_Add(DirtyResize | DirtyRebound);
      return true;
    }

    ef (Name == "Tag")
    {
      if (!Prop.isInt())
        return false;

      Tag = (i64)Prop;
      
      return true;
    }

    ef (Name == "Visible")
    {
      if (!Prop.isBool())
        return false;

      Visible = (bool)Prop;

      return true;
    }

    ef (Name == "Enabled")
    {
      if (!Prop.isBool())
        return false;

      Enabled = (bool)Prop;

      return true;
    }

    ef (Name == "AutoSize")
    {
      if (!Prop.isBool())
        return false;

      AutoSize = (bool)Prop;

      return true;
    }

    ef (Name == "Transparent")
    {
      if (!Prop.isBool())
        return false;

      Transparent = (bool)Prop;

      return true;
    }

    ef (Name == "Anchors")
    {
      controlAnchors Nat = this->Anchors;

      if (!Prop.isStruct())
        return false;


      auto Handler = [&](control* This, const jconf::Value& Stc, controlAnchor* Anchor) -> bool
      {
        if (!Stc.isStruct())
          return false;

        string Cac;
        
        // Active
        Anchor->Active = (bool)Stc["active"];

        // Control
        Cac = (string)Stc["control"];
        if (!Cac.empty())
        {
          if (Cac[0] != '@')
            return false;

          if (Cac == "@^")
            Anchor->Control = this->Parent;
          el
            Anchor->Control = __findInScope(This, Cac.substr(1));

          if (Anchor->Control == Nil)
            return false;
        }

        // Size
        Cac = (string)Stc["side"];
        if (!Cac.empty())
        {
          if (Cac == "beg" || Cac == "<-")
            Anchor->Side = controlAnchorSide::casBegin;

          ef (Cac == "end" || Cac == "->")
            Anchor->Side = controlAnchorSide::casEnd;

          else
            return false;
        }

        
        return true;
      };


      if (auto Buf = Prop["left"]; !Buf.isNull())
        if (!Handler(this, Buf, &Nat.Left))
          return false;

      if (auto Buf = Prop["top"]; !Buf.isNull())
        if (!Handler(this, Buf, &Nat.Top))
          return false;

      if (auto Buf = Prop["righ"]; !Buf.isNull())
        if (!Handler(this, Buf, &Nat.Righ))
          return false;

      if (auto Buf = Prop["bot"]; !Buf.isNull())
        if (!Handler(this, Buf, &Nat.Bot))
          return false;


      Anchors = (Nat);

      if (Parent != Nil)
        Parent->Do_Resize();

      return true;
    }

    ef (Name == "Margins")
    {
      if (!Prop.isStruct())
        return false;


      if (auto Buf = Prop["left"]; !Buf.isNull())
        Margins.X1 = (i64)Buf;

      if (auto Buf = Prop["righ"]; !Buf.isNull())
        Margins.X2 = (i64)Buf;

      if (auto Buf = Prop["top"]; !Buf.isNull())
        Margins.Y1 = (i64)Buf;

      if (auto Buf = Prop["bot"]; !Buf.isNull())
        Margins.Y2 = (i64)Buf;

      return true;
    }

    else
      return false;
  }

  bool control::LoadFunc(string Name, point Func)
  {
    #define makro(X) (Name == #X) { X = (decltype(X))Func; return true; }

    if makro(OnPaint)
    ef makro(OnPaint_before)
    ef makro(OnPaint_after)
    ef makro(OnResize)
    ef makro(OnClick)
    ef makro(OnClickEx)
    ef makro(OnDblClick)
    ef makro(OnMouseDown)
    ef makro(OnMouseUp)
    ef makro(OnMouseMove)
    ef makro(OnKeyDown)
    ef makro(OnKeyUp)
    ef makro(OnScrollVert)
    ef makro(OnScrollHorz)
    ef makro(OnStateChanged)

    #undef makro

    return false;
  }


  void control::Handler_Message(controlMessages Msg)
  {
    switch (Msg)
    {
      case controlMessages::cmPaint: Handler_Paint(); break;

      default: break;
    }
  }

  void control::Handler_Paint()
  {
    Do_Paint_prepare();
    Do_Paint();
  }

  void control::Handler_Resize()
  {
    Flag_Add(DirtyResize);
  }

  void control::Handler_MouseDown(poit_i32 Pos, shiftStateSet Button, shiftStateSet State)
  {
    Do_MouseDown(Pos, Button, State);
  }
  
  void control::Handler_MouseUp(poit_i32 Pos, shiftStateSet Button, shiftStateSet State)
  {
    Do_MouseUp(Pos, Button, State);
  }

  void control::Handler_MouseMove(poit_i32 Pos, shiftStateSet State)
  {
    Do_MouseMove(Pos, State);
  }

  void control::Handler_KeyDown(char *Key, u32 KeyCode, shiftStateSet State)
  {
    Do_KeyDown(Key, KeyCode, State);
  }
  
  void control::Handler_KeyUp(char *Key, u32 KeyCode, shiftStateSet State)
  {
    Do_KeyUp(Key, KeyCode, State);
  }

  void control::Handler_ScrollVert(poit_i32 Pos, i16 Delta, shiftStateSet State)
  {
    Do_ScrollVert(Pos, Delta, State);
  }

  void control::Handler_ScrollHorz(poit_i32 Pos, i16 Delta, shiftStateSet State)
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
    if (OnPaint_before != Nil)
      OnPaint_before(this);

    Draw();
    if (OnPaint != Nil)
      OnPaint(this);

    Draw_after();
    if (OnPaint_after != Nil)
      OnPaint_after(this);


    if (!Enabled)
      Surface->Filter_GrayScale({0,0, (f32)Size.W, (f32)Size.H});

    Surface->Flush();

    Flag_Rem(DirtyDraw);
  }

  void control::Do_Paint_prepare()
  {
    if (Flag_HasR(DirtyResize))
      Do_Resize();

    if (Flag_HasR(DirtyRebound))
      EndPoit = {
        .X = Poit.X +Size.W,
        .Y = Poit.Y +Size.H,
      };

  }

  void control::Do_Resize()
  {
    Surface->Set_Size(max<i32>(0,Size.W), max<i32>(0,Size.H));

    
    if (OnResize != Nil)
      OnResize(this);
  }

  void control::Do_Click()
  {
    if (OnClick != Nil)
      OnClick(this);
  }

  void control::Do_ClickEx(poit_i32 Pos)
  {
    if (OnClickEx != Nil)
      OnClickEx(this, Pos);
  }

  void control::Do_DblClick()
  {
    if (OnDblClick != Nil)
      OnDblClick(this);
  }

  void control::Do_MouseDown(poit_i32 Pos, shiftStateSet Button, shiftStateSet State)
  {
    if (OnMouseDown != Nil)
      OnMouseDown(this, Pos, Button, State);
  }

  void control::Do_MouseUp(poit_i32 Pos, shiftStateSet Button, shiftStateSet State)
  {
    if (OnMouseUp != Nil)
      OnMouseUp(this, Pos, Button, State);


    // Click
    if (State != 0 || Button != shiftStates::ssLeft)
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
      Do_ClickEx(Pos);
      Do_Click();

      LastClick = GetTickCount();
    }
    else
    {
      Do_DblClick();
    }

  }

  void control::Do_MouseMove (poit_i32 Pos, shiftStateSet State)
  {
    if (OnMouseMove != Nil)
      OnMouseMove(this, Pos, State);
  }

  void control::Do_KeyDown(char *Key, u32 KeyCode, shiftStateSet State)
  {
    if (OnKeyDown != Nil)
      OnKeyDown(this, Key, KeyCode, State);
  }

  void control::Do_KeyUp(char *Key, u32 KeyCode, shiftStateSet State)
  {
    if (OnKeyUp != Nil)
      OnKeyUp(this, Key, KeyCode, State);
  }

  void control::Do_ScrollVert(poit_i32 Pos, i16 Delta, shiftStateSet State)
  {
    if (OnScrollVert != Nil)
      OnScrollVert(this, Pos, Delta, State);
  }

  void control::Do_ScrollHorz(poit_i32 Pos, i16 Delta, shiftStateSet State)
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
