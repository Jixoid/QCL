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
#include <ranges>

#include "Basis.h"

#include "qcl/Control.hh"
#include "qcl/Graphic.hh"
#include "qcl/Application.hh"
#include "qcl/DynamicLoad.hh"
#include "qcl/qstd/Standard.hh"

using namespace std;
using namespace jix;


inline string __jc_ValGet(jc_val Val)
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


namespace jix::qcl::qstd
{

  #pragma region form

  form::form(): window()
  {}

  form::~form()
  {}


  void form::Draw()
  {
    Surface->Set_Color(color(0.12, 0.12, 0.12));

    Surface->Draw_Rect({0,0, (f32)Size.W, (f32)Size.H});
    Surface->Fill();
  }

  #pragma endregion


  #pragma region layout

  layout::layout(): view()
  {}

  layout::~layout()
  {}


  void layout::Draw()
  {
    Surface->Set_Color(BackColor);

    Surface->Draw_RectRound({0,0, (f32)Size.W, (f32)Size.H}, BorderRadius);
    Surface->Fill();
  }

  bool layout::LoadProp(string Name, jc_obj Prop)
  {
    if (control::LoadProp(Name, Prop))
      return true;


    jc_obj Buf;
    string Cac;

    if (Name == "BorderRadius")
    {
      u16 Nat = this->BorderRadius;

      Cac = __jc_ValGet(Prop);
      if (Cac.empty())
        return false;

      auto eret = from_chars(Cac.data(), Cac.data() +Cac.size(), Nat);
      if (eret.ec != std::errc())
        return false;


      BorderRadius = (Nat);
      return true;
    }

    else if (Name == "BackColor")
    {
      color Nat = this->BackColor;

      if (!jc_IsStc(Prop))
        return false;


      // R
      Cac = __jc_ValGet(jc_StcGet(Prop, "R"));
      if (!Cac.empty())
      {
        auto eret = from_chars(Cac.data(), Cac.data() +Cac.size(), Nat.R);
        if (eret.ec != std::errc())
          return false;

        Nat.R /= 255;
      }


      // G
      Cac = __jc_ValGet(jc_StcGet(Prop, "G"));
      if (!Cac.empty())
      {
        auto eret = from_chars(Cac.data(), Cac.data() +Cac.size(), Nat.G);
        if (eret.ec != std::errc())
          return false;

        Nat.G /= 255;
      }


      // B
      Cac = __jc_ValGet(jc_StcGet(Prop, "B"));
      if (!Cac.empty())
      {
        auto eret = from_chars(Cac.data(), Cac.data() +Cac.size(), Nat.B);
        if (eret.ec != std::errc())
          return false;

        Nat.B /= 255;
      }


      // A
      Cac = __jc_ValGet(jc_StcGet(Prop, "A"));
      if (!Cac.empty())
      {
        auto eret = from_chars(Cac.data(), Cac.data() +Cac.size(), Nat.A);
        if (eret.ec != std::errc())
          return false;

        Nat.A /= 255;
      }
      

      BackColor = (Nat);
      return true;
    }

    else
      return false;
  }


  #pragma endregion


  #pragma region layout_vert

  layout_vert::layout_vert(): layout()
  {}

  layout_vert::~layout_vert()
  {}


  void layout_vert::Do_Resize()
  {
    // Set vert layout
    i32 StartPos = 0;
    i32 LastMargin = 0;

    for (auto &X: Childs)
    {
      StartPos += (X->Margins.Y1 > LastMargin) ? (X->Margins.Y1):(LastMargin);

      X->Poit_Set({X->Margins.X1, StartPos});

      LastMargin = X->Margins.Y2;
      StartPos += X->Size.H;
    }

    view::Do_Resize();
  }

  #pragma endregion


  #pragma region layout_horz

  layout_horz::layout_horz(): layout()
  {}

  layout_horz::~layout_horz()
  {}


  void layout_horz::Do_Resize()
  {
    // Set vert layout
    i32 StartPos = 0;
    i32 LastMargin = 0;

    for (auto &X: Childs)
    {
      StartPos += (X->Margins.X1 > LastMargin) ? (X->Margins.X1):(LastMargin);

      X->Poit_Set({StartPos, X->Margins.Y1});

      LastMargin = X->Margins.X2;
      StartPos += X->Size.W;
    }

    view::Do_Resize();
  }

  #pragma endregion


  #pragma region button

  button::button(): control()
  {}

  button::~button()
  {}


  void button::Draw()
  {
    switch (ControlState)
    {
      case controlStates::csHover:
        Surface->Set_Color(color(0.45, 0.30, 0.80));
        break;
      
      default:
        Surface->Set_Color(color(0.28, 0.28, 0.28));
        break;
    }
    
    Surface->Draw_RectRound(rect_f32{0,0, (f32)Size.W, (f32)Size.H}, BorderRadius);
    Surface->Fill();


    Surface->Set_FontSize(15);

    size_f32 TSize = Surface->Calc_Text(Text.c_str());

    Surface->Set_Color(color(1,1,1));
    Surface->Set_Pos({ trunc((Size.W -TSize.W)/2), trunc((Size.H -TSize.H)/2)});
    Surface->Draw_Text(Text.c_str());
  }


  void button::Handler_StateChanged(controlStateSet State)
  {
    control::Handler_StateChanged(State);


    DyeToRoot();
    CurrentApp->SendMessage(GetRoot(), controlMessages::cmPaint);
  }


  bool button::LoadProp(string Name, jc_obj Prop)
  {
    if (control::LoadProp(Name, Prop))
      return true;


    jc_obj Buf;
    string Cac;

    if (Name == "Text")
    {
      string Nat;

      if (!jc_IsVal(Prop))
        return false;


      // Val
      Cac = __jc_ValGet(Prop);
      if (Cac.empty())
        return false;
      Nat = Cac;
      

      Text = (Nat);
      return true;
    }

    if (Name == "BorderRadius")
    {
      u16 Nat = this->BorderRadius;

      Cac = __jc_ValGet(Prop);
      if (Cac.empty())
        return false;

      auto eret = from_chars(Cac.data(), Cac.data() +Cac.size(), Nat);
      if (eret.ec != std::errc())
        return false;


      BorderRadius = (Nat);
      return true;
    }

    else
      return false;
  }

  #pragma endregion


  #pragma region label

  label::label(): control()
  {}

  label::~label()
  {}


  void label::Draw()
  {
    Surface->Set_FontSize(13);

    size_f32 TSize = Surface->Calc_Text(Text.c_str());

    Surface->Set_Color(color(1,1,1));
    Surface->Set_Pos({(Size.W -TSize.W)/2, (Size.H -TSize.H)/2});
    Surface->Draw_Text(Text.c_str());
  }


  bool label::LoadProp(string Name, jc_obj Prop)
  {
    if (control::LoadProp(Name, Prop))
      return true;


    jc_obj Buf;
    string Cac;

    if (Name == "Text")
    {
      string Nat;

      if (!jc_IsVal(Prop))
        return false;


      // Val
      Cac = __jc_ValGet(Prop);
      if (Cac.empty())
        return false;
      Nat = Cac;
      

      Text = (Nat);
      return true;
    }

    else
      return false;
  }

  #pragma endregion
  


  void Register()
  {
    qcl::dyn::Register(
      "qstd::form",
      []() -> qcl::control*
      {
        return new qcl::qstd::form();
      }
    );

    qcl::dyn::Register(
      "qstd::layout",
      []() -> qcl::control*
      {
        return new qcl::qstd::layout();
      }
    );

    qcl::dyn::Register(
      "qstd::layout_vert",
      []() -> qcl::control*
      {
        return new qcl::qstd::layout_vert();
      }
    );

    qcl::dyn::Register(
      "qstd::layout_horz",
      []() -> qcl::control*
      {
        return new qcl::qstd::layout_horz();
      }
    );

    qcl::dyn::Register(
      "qstd::button",
      []() -> qcl::control*
      {
        return new qcl::qstd::button();
      }
    );

    qcl::dyn::Register(
      "qstd::label",
      []() -> qcl::control*
      {
        return new qcl::qstd::label();
      }
    );
  }

}
