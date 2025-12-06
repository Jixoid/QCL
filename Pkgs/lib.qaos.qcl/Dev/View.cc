/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define ef else if
#define el else

#include <memory>
#include <vector>
#include <ranges>

#include "Basis.h"

#include "qcl/Control.hh"
#include "qcl/View.hh"
#include "qcl/Graphic.hh"
#include "qcl/Application.hh"

using namespace std;
using namespace jix;



namespace qcl
{
  view::view(): control()
  {}

  view::~view()
  {}


  void view::HoverControlSet(control* Ctrl)
  {
    if (FHoverControl == Ctrl)
      return;

    if (Ctrl != Nil)
      Ctrl->Handler_StateChanged(
        controlStates::csHover |
        (FFocusControl == Ctrl ? controlStates::csFocus:0)
      );

    if (FHoverControl != Nil)
      FHoverControl->Handler_StateChanged(
        (FFocusControl == FHoverControl ? controlStates::csFocus:0)
      ); 


    FHoverControl = Ctrl;
  }

  void view::FocusControlSet(control* Ctrl)
  {
    if (FFocusControl == Ctrl)
      return;

    if (Ctrl != Nil)
      Ctrl->Handler_StateChanged(
        controlStates::csFocus |
        (FHoverControl == Ctrl ? controlStates::csHover:0)
      );

    if (FFocusControl != Nil)
      FFocusControl->Handler_StateChanged(
        (FHoverControl == FFocusControl ? controlStates::csHover:0)
      ); 


    FFocusControl = Ctrl;
  }


  void view::Child_Add(shared_ptr<control> Ctrl)
  {
    if (Ctrl->Parent != Nil)
      Ctrl->Parent->Child_Rem(Ctrl.get());


    auto it = std::ranges::find_if(Childs.begin(), Childs.end(),
      [&](const shared_ptr<control>& c)
      {
        return c.get() == Ctrl.get();
      }
    );

    if (it != Childs.end())
      return;
    

    Ctrl->Parent = this;
    Childs.push_back(std::move(Ctrl));


    Flag_Add(MustDraw | MustAutoSize | MustTiling);
  }

  void view::Child_Rem(control* Ctrl)
  {
    auto it = std::ranges::find_if(Childs.begin(), Childs.end(),
      [&](const std::shared_ptr<control>& c)
      {
        return c.get() == Ctrl;
      }
    );


    if (it == Childs.end())
      return;
    
    // Parent pointer'ını temizle
    Childs.erase(it);


    if (FHoverControl == Ctrl)
      FHoverControl = Nil;

    if (FFocusControl == Ctrl)
      FFocusControl = Nil;
      
    Do_Resize();
  }

  
  inline bool __ifScrollableVert(view* Self, i16 Delta)
  {
    return (Delta > 0) ? (Self->ClientBound.Y1 +Self->ClientPos.Y < 0):(Self->ClientBound.Y2 +Self->ClientPos.Y -Self->Size.H > 0);
  }

  inline bool __ifScrollableHorz(view* Self, i16 Delta)
  {
    return (Delta > 0) ? (Self->ClientBound.X1 +Self->ClientPos.X < 0):(Self->ClientBound.X2 +Self->ClientPos.X -Self->Size.W > 0);
  }




  size_i32 view::CalcAutoSize()
  {
    rect_i32 Bound = {0,0,0,0};

    for (auto &X: Childs)
    {
      if (!X->Visible)
        continue;

      if (X->Poit.X < Bound.X1) Bound.X1 = X->Poit.X;
      if (X->Poit.Y < Bound.Y1) Bound.Y1 = X->Poit.Y;
      
      i32 Right = X->Poit.X + X->Size.W;
      i32 Bottom = X->Poit.Y + X->Size.H;

      if (Right > Bound.X2) Bound.X2 = Right;
      if (Bottom > Bound.Y2) Bound.Y2 = Bottom;
    }

    return {Bound.X2, Bound.Y2};
  }

  void view::Draw_after()
  {
    for (auto &X: Childs)
    {
      if (!X->Visible)
        continue;

      Surface->Set_Pos({ (f32)(ClientPos.X + X->Poit.X), (f32)(ClientPos.Y + X->Poit.Y) });
      Surface->Set_Source(X->Surface);
      Surface->Paint();


      continue;
      l_Debug: {
        Surface->Set_Color(color(0,0,1));
        Surface->Set_LineSize(1);
        
        Surface->Set_Pos({
          (f32)(ClientPos.X + X->Poit.X) +5,
          (f32)(ClientPos.Y + X->Poit.Y),
        });
        Surface->Draw_ToLine({
          (f32)(ClientPos.X + X->Poit.X),
          (f32)(ClientPos.Y + X->Poit.Y),
        });
        Surface->Draw_ToLine({
          (f32)(ClientPos.X + X->Poit.X),
          (f32)(ClientPos.Y + X->Poit.Y) +5,
        });
        Surface->Stroke();


        Surface->Set_Pos({
          (f32)(ClientPos.X + X->Poit.X) +5,
          (f32)(ClientPos.Y + X->Poit.Y) +X->Size.H,
        });
        Surface->Draw_ToLine({
          (f32)(ClientPos.X + X->Poit.X),
          (f32)(ClientPos.Y + X->Poit.Y) +X->Size.H,
        });
        Surface->Draw_ToLine({
          (f32)(ClientPos.X + X->Poit.X),
          (f32)(ClientPos.Y + X->Poit.Y) +X->Size.H -5,
        });
        Surface->Stroke();


        Surface->Set_Pos({
          (f32)(ClientPos.X + X->Poit.X) +X->Size.W -5,
          (f32)(ClientPos.Y + X->Poit.Y) +X->Size.H,
        });
        Surface->Draw_ToLine({
          (f32)(ClientPos.X + X->Poit.X) +X->Size.W,
          (f32)(ClientPos.Y + X->Poit.Y) +X->Size.H,
        });
        Surface->Draw_ToLine({
          (f32)(ClientPos.X + X->Poit.X) +X->Size.W,
          (f32)(ClientPos.Y + X->Poit.Y) +X->Size.H -5,
        });
        Surface->Stroke();


        Surface->Set_Pos({
          (f32)(ClientPos.X + X->Poit.X) +X->Size.W -5,
          (f32)(ClientPos.Y + X->Poit.Y),
        });
        Surface->Draw_ToLine({
          (f32)(ClientPos.X + X->Poit.X) +X->Size.W,
          (f32)(ClientPos.Y + X->Poit.Y),
        });
        Surface->Draw_ToLine({
          (f32)(ClientPos.X + X->Poit.X) +X->Size.W,
          (f32)(ClientPos.Y + X->Poit.Y) +5,
        });
        Surface->Stroke();
      }
    }


    if (__ifScrollableVert(this, +1) || __ifScrollableVert(this, -1))
      Draw_ScrollVert();

    if (__ifScrollableHorz(this, +1) || __ifScrollableHorz(this, -1))
      Draw_ScrollHorz();

    control::Draw_after();
  }


  void view::Draw_ScrollVert()
  {
    if (!ScrollVertVisible)
      return;

    Surface->Set_LineSize(4);

    Surface->Set_Color(color(1,1,1, 0.08));
    Surface->Draw_Line({(f32)Size.W -5,5}, {(f32)Size.W -5, (f32)Size.H -5});
    Surface->Stroke();

    f32
      MaxSize = (ClientBound.Y2-ClientBound.Y1),
      Pos = -ClientPos.Y,
      Height = Size.H -10;

    Surface->Set_Color(color(0.50, 0.35, 0.90));
    Surface->Draw_Line({(f32)Size.W -5,5 +((Pos /MaxSize) *Height)}, {(f32)Size.W -5, 5 +((Pos /MaxSize) *Height) +((Height /MaxSize) *Height)});
    Surface->Stroke();
  }

  void view::Draw_ScrollHorz()
  {
    if (!ScrollHorzVisible)
      return;

    Surface->Set_LineSize(4);

    Surface->Set_Color(color(1,1,1, 0.08));
    Surface->Draw_Line({5, (f32)Size.H -5}, {(f32)Size.W -5, (f32)Size.H -5});
    Surface->Stroke();

    f32
      MaxSize = (ClientBound.X2-ClientBound.X1),
      Pos = -ClientPos.X,
      Width = Size.W -10;

    Surface->Set_Color(color(0.50, 0.35, 0.90));
    Surface->Draw_Line({5 +((Pos /MaxSize) *Width), (f32)Size.H -5}, {5 +((Pos /MaxSize) *Width) +((Width /MaxSize) *Width), (f32)Size.H -5});
    Surface->Stroke();
  }



  bool view::LoadProp(string Name, const jconf::Value& Prop)
  {
    if (control::LoadProp(Name, Prop))
      return true;


    jconf::Value Buf;
    string Cac;

    if (Name == "ScrollVertVisible")
    {
      bool Nat = this->ScrollVertVisible;

      if (!Prop.isBool())
        return false;

      Nat = (bool)Prop;

      ScrollVertVisible = (Nat);
      return true;
    }

    ef (Name == "ScrollHorzVisible")
    {
      bool Nat = this->ScrollHorzVisible;

      if (!Prop.isBool())
        return false;

      Nat = (bool)Prop;

      ScrollHorzVisible = (Nat);
      return true;
    }

    else
      return false;
  }



  control* view::FindInput(poit_i32 Pos)
  {
    for (auto &X: views::reverse(Childs))
      if (
        (X->Visible && X->Enabled && !X->Transparent) &&

        Pos.X >= ClientPos.X + X->Poit.X &&
        Pos.Y >= ClientPos.Y + X->Poit.Y &&
        
        Pos.X <= ClientPos.X + X->EndPoit.X &&
        Pos.Y <= ClientPos.Y + X->EndPoit.Y
      )
        return X.get();

    return Nil;
  }



  void view::Do_Tiling()
  {
    // Apply Anchors
    for (auto &X: Childs)
    {
      poit_i32 TempP = X->Poit;

      if (X->Anchors.Left.Active)
      {
        if (X->Anchors.Left.Control == this)
          TempP.X = 0;

        else
          switch (X->Anchors.Left.Side)
          {
            case controlAnchorSide::casBegin:
              TempP.X = X->Anchors.Left.Control -> Poit.X;
              break;

            case controlAnchorSide::casEnd:
              TempP.X = (X->Anchors.Left.Control->Visible) ? (X->Anchors.Left.Control -> EndPoit.X):(X->Anchors.Left.Control -> Poit.X);
              break;
          }

        TempP.X += X->Margins.X1;
      }

      if (X->Anchors.Top.Active)
      {
        if (X->Anchors.Top.Control == this)
          TempP.Y = 0;

        else
          switch (X->Anchors.Top.Side)
          {
            case controlAnchorSide::casBegin:
              TempP.Y = X->Anchors.Top.Control -> Poit.Y;
              break;

            case controlAnchorSide::casEnd:
              TempP.Y = (X->Anchors.Top.Control->Visible) ? (X->Anchors.Top.Control -> EndPoit.Y):(X->Anchors.Top.Control -> Poit.Y);
              break;
          }

        TempP.Y += X->Margins.Y1;
      }


      
      poit_i32 TempE = {TempP.X +X->Size.W, TempP.Y +X->Size.H};

      if (X->Anchors.Righ.Active)
      {
        if (X->Anchors.Righ.Control == this)
          TempE.X = Size.W;

        else
          switch (X->Anchors.Righ.Side)
          {
            case controlAnchorSide::casBegin:
              TempE.X = (X->Anchors.Righ.Control->Visible) ? (X->Anchors.Righ.Control -> Poit.X):(X->Anchors.Righ.Control -> EndPoit.X);
              break;

            case controlAnchorSide::casEnd:
              TempE.X = X->Anchors.Righ.Control -> EndPoit.X;
              break;
          }

        TempE.X -= X->Margins.X2;
      }

      if (X->Anchors.Bot.Active)
      {
        if (X->Anchors.Bot.Control == this)
          TempE.Y = Size.H;

        else
          switch (X->Anchors.Bot.Side)
          {
            case controlAnchorSide::casBegin:
              TempE.Y = (X->Anchors.Bot.Control->Visible) ? (X->Anchors.Bot.Control -> Poit.Y):(X->Anchors.Bot.Control -> EndPoit.Y);
              break;

            case controlAnchorSide::casEnd:
              TempE.Y = X->Anchors.Bot.Control -> EndPoit.Y;
              break;
          }

        TempE.Y -= X->Margins.Y2;
      }


      if (!X->Anchors.Top.Active  && X->Anchors.Bot.Active)  TempP.Y = TempE.Y -X->Size.H;
      if (!X->Anchors.Left.Active && X->Anchors.Righ.Active) TempP.X = TempE.X -X->Size.W;


      if (X->Poit.X != TempP.X || X->Poit.Y != TempP.Y)
      {
        X->Poit = TempP;
        X->Flag_Add(MustRebound);
      }

      if (X->Size.W != TempE.X-TempP.X || X->Size.H != TempE.Y-TempP.Y)
      {
        X->Size = {TempE.X-TempP.X, TempE.Y-TempP.Y};
        X->Flag_Add(MustResize);
      }

    }

    
    // Calc Limit
    ClientBound = {0,0, Size.W, Size.H};
    for (auto &X: Childs)
    {
      if (!X->Visible)
        continue;


      if (X->Poit.X < ClientBound.X1)
        ClientBound.X1 = X->Poit.X;

      if (X->Poit.Y < ClientBound.Y1)
        ClientBound.Y1 = X->Poit.Y;

      if (X->EndPoit.X > ClientBound.X2)
        ClientBound.X2 = X->EndPoit.X;

      if (X->EndPoit.Y > ClientBound.Y2)
        ClientBound.Y2 = X->EndPoit.Y;
    }

  }

  void view::Do_Paint_prepare()
  {
    for (auto &X: Childs)
    {
      if (!X->Visible)
        continue;

      X->Do_Paint_prepare();
    }

    
    if (ControlFlags & MustTiling)
    {
      Do_Tiling();
      Flag_Rem(MustTiling);
    }


    for (auto &X: Childs)
    {
      if (!X->Visible)
        continue;

      X->Do_Paint_prepare();
    }

    
    control::Do_Paint_prepare();
  }

  void view::Do_Paint()
  {
    for (auto &X: Childs)
    {
      if (!X->Visible)
        continue;

      if (X->ControlFlags & MustDraw)
        X->Do_Paint();
    }


    control::Do_Paint();
  }

  void view::Do_Resize()
  {
    Flag_Add(MustTiling);

    control::Do_Resize();
  }

  void view::Do_MouseDown(poit_i32 Pos, shiftStateSet Button, shiftStateSet State)
  {
    if (State == 0)
      HoverControlSet(FindInput(Pos));

    if (Button == shiftStates::ssLeft)
      FocusControlSet(FHoverControl);


    if (FHoverControl == Nil)
    {
      control::Do_MouseDown(Pos, Button, State);
      return;
    }

    FHoverControl->Handler_MouseDown({Pos.X -ClientPos.X -FHoverControl->Poit.X, Pos.Y  -ClientPos.Y -FHoverControl->Poit.Y}, Button, State);
  }

  void view::Do_MouseUp(poit_i32 Pos, shiftStateSet Button, shiftStateSet State)
  {
    if (FHoverControl == Nil)
    {
      control::Do_MouseUp(Pos, Button, State);
      return;
    }

    FHoverControl->Handler_MouseUp({Pos.X  -ClientPos.X -FHoverControl->Poit.X, Pos.Y  -ClientPos.Y -FHoverControl->Poit.Y}, Button, State);


    if (State == 0)
      HoverControlSet(FindInput(Pos));
  }

  void view::Do_MouseMove (poit_i32 Pos, shiftStateSet State)
  {
    if (State == 0)
      HoverControlSet(FindInput(Pos));


    if (FHoverControl == Nil)
    {
      control::Do_MouseMove(Pos, State);
      return;
    }


    FHoverControl->Handler_MouseMove({Pos.X  -ClientPos.X -FHoverControl->Poit.X, Pos.Y  -ClientPos.Y -FHoverControl->Poit.Y}, State);
  }

  void view::Do_KeyDown   (char *Key, u32 KeyCode, shiftStateSet State)
  {
    if (FFocusControl != Nil)
      FFocusControl->Handler_KeyDown(Key, KeyCode, State);

    ef (FHoverControl != Nil)
      FHoverControl->Handler_KeyDown(Key, KeyCode, State);

    el
    {
      control::Do_KeyDown(Key, KeyCode, State);
      return;
    }

  }

  void view::Do_KeyUp     (char *Key, u32 KeyCode, shiftStateSet State)
  {
    if (FFocusControl != Nil)
      FFocusControl->Handler_KeyUp(Key, KeyCode, State);

    ef (FHoverControl != Nil)
      FHoverControl->Handler_KeyUp(Key, KeyCode, State);

    {
      control::Do_KeyUp(Key, KeyCode, State);
      return;
    }

  }

  void view::Do_ScrollVert(poit_i32 Pos, i16 Delta, shiftStateSet State)
  {
    control *Active = FindInput(Pos);
    Delta *= 2;

    if (Active == Nil || (dynamic_cast<qcl::view*>(Active) == Nil && __ifScrollableVert(this, Delta)))
    {
      control::Do_ScrollVert(Pos, Delta, State);


      if (__ifScrollableVert(this, Delta))
      {
        ClientPos.Y += Delta;
        DyeToRoot();
        CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);

        if (State == 0)
          HoverControlSet(FindInput(Pos));
      }

      return;
    }

    Active->Handler_ScrollVert({Pos.X  -ClientPos.X -Active->Poit.X, Pos.Y  -ClientPos.Y -Active->Poit.Y}, Delta, State);
  }

  void view::Do_ScrollHorz(poit_i32 Pos, i16 Delta, shiftStateSet State)
  {
    control *Active = FindInput(Pos);
    Delta *= 2;

    if (Active == Nil || (dynamic_cast<qcl::view*>(Active) == Nil && __ifScrollableHorz(this, Delta)))
    {
      control::Do_ScrollHorz(Pos, Delta, State);


      if (__ifScrollableHorz(this, Delta))
      {
        ClientPos.X += Delta;
        DyeToRoot();
        CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);

        if (State == 0)
          HoverControlSet(FindInput(Pos));
      }

      return;
    }

    Active->Handler_ScrollHorz({Pos.X  -ClientPos.X -Active->Poit.X, Pos.Y  -ClientPos.Y -Active->Poit.Y}, Delta, State);
  }

}
