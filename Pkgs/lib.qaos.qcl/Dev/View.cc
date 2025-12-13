/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <format>
#define ef else if
#define el else

#include <memory>
#include <vector>
#include <ranges>
#include <algorithm>

#include "Basis.h"

#include "qcl/Control.hh"
#include "qcl/View.hh"
#include "qcl/Graphic.hh"
#include "qcl/Application.hh"
#include "qcl/Platform.hh"

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


    Flag_Add(DirtyDraw | DirtyAutoSize | DirtyTiling);
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




  void view::CalcAutoSize()
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

    PreferedSize = {Bound.X2, Bound.Y2};
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



  void view::TilingScanH(vector<control*> &HList, control *This)
  {
    if (This->_TC_Visiting)
    {
      string Msg = "a circular depency is caught";
      platform::qcl_error(Msg.c_str()); 
    }


    This->_TC_Visiting = true;

    if (This->Anchors.Left.Active && This->Anchors.Left.Control != Nil && This->Anchors.Left.Control != this)
      TilingScanH(HList, This->Anchors.Left.Control);

    if (This->Anchors.Righ.Active && This->Anchors.Righ.Control != Nil && This->Anchors.Righ.Control != this)
      TilingScanH(HList, This->Anchors.Righ.Control);

    This->_TC_Visiting = false;


    if (auto fnd = std::find(HList.begin(), HList.end(), This); fnd == HList.end())
      HList.push_back(This);
  }

  void view::TilingScanV(vector<control*> &VList, control *This)
  {
    if (This->_TC_Visiting)
    {
      string Msg = "a circular depency is caught";
      platform::qcl_error(Msg.c_str()); 
    }


    This->_TC_Visiting = true;
    
    if (This->Anchors.Top.Active && This->Anchors.Top.Control != Nil && This->Anchors.Top.Control != this)
      TilingScanV(VList, This->Anchors.Top.Control);

    if (This->Anchors.Bot.Active && This->Anchors.Bot.Control != Nil && This->Anchors.Bot.Control != this)
      TilingScanV(VList, This->Anchors.Bot.Control);

    This->_TC_Visiting = false;


    if (auto fnd = std::find(VList.begin(), VList.end(), This); fnd == VList.end())
      VList.push_back(This);
  }


  void view::Do_Tiling()
  {
    // Pass 1: Horz & Vert Scan
    vector<control*> CList;

    for (auto &X: Childs)
    {
      X->_TC_Visiting = false;
      X->_TC_Visited = false;
    }
    for (auto &X: Childs) TilingScanH(CList, X.get());

    for (auto &X: Childs)
    {
      X->_TC_Visiting = false;
      X->_TC_Visited = false;
    }
    for (auto &X: Childs) TilingScanV(CList, X.get());

    
    // Pass 2: Horz & Vert Relocation
    for (auto &X: CList)
    {
      // Start Pos
      poit_i32 SPos = X->Poit;


      if (X->Anchors.Left.Active)
      {
        const auto Ctrl = X->Anchors.Left.Control;


        if (Ctrl == this)
          SPos.X = 0;

        ef (Ctrl == Nil);

        el switch (X->Anchors.Left.Side)
        {
          case controlAnchorSide::casBegin:
            SPos.X = Ctrl->Poit.X;
            break;

          case controlAnchorSide::casEnd:
            SPos.X = (Ctrl->Visible) ? (Ctrl->EndPoit.X):(Ctrl->Poit.X);
            break;
        }


        if (Ctrl != Nil)
          SPos.X += X->Margins.X1;
      }

      if (X->Anchors.Top.Active)
      {
        const auto Ctrl = X->Anchors.Top.Control;


        if (Ctrl == this)
          SPos.Y = 0;

        ef (Ctrl == Nil);

        el switch (X->Anchors.Top.Side)
        {
          case controlAnchorSide::casBegin:
            SPos.Y = Ctrl->Poit.Y;
            break;

          case controlAnchorSide::casEnd:
            SPos.Y = (Ctrl->Visible) ? (Ctrl->EndPoit.Y):(Ctrl->Poit.Y);
            break;
        }


        if (Ctrl != Nil)
          SPos.Y += X->Margins.Y1;
      }



      // End Pos
      size_i32 NSize = (X->AutoSize ? X->PreferedSize : X->Size);

      NSize.W = max(X->MinSize.W, NSize.W);
      NSize.H = max(X->MinSize.H, NSize.H);

      if (X->MaxSize.W != 0) NSize.W = min(X->MaxSize.W, NSize.W);
      if (X->MaxSize.H != 0) NSize.H = min(X->MaxSize.H, NSize.H);

      poit_i32 EPos = {
        .X = SPos.X +NSize.W,
        .Y = SPos.Y +NSize.H,
      };


      if (X->Anchors.Righ.Active)
      {
        const auto Ctrl = X->Anchors.Righ.Control;


        if (Ctrl == this)
          EPos.X = Size.W;

        ef (Ctrl == Nil);

        el switch (X->Anchors.Righ.Side)
        {
          case controlAnchorSide::casBegin:
            EPos.X = (Ctrl->Visible) ? (Ctrl->Poit.X):(Ctrl->EndPoit.X);
            break;

          case controlAnchorSide::casEnd:
            EPos.X = Ctrl->EndPoit.X;
            break;
        }

        if (Ctrl != Nil)
          EPos.X -= X->Margins.X2;
      }

      if (X->Anchors.Bot.Active)
      {
        const auto Ctrl = X->Anchors.Bot.Control;


        if (Ctrl == this)
          EPos.Y = Size.H;

        ef (Ctrl == Nil);

        el switch (X->Anchors.Bot.Side)
        {
          case controlAnchorSide::casBegin:
            EPos.Y = (Ctrl->Visible) ? (Ctrl->Poit.Y):(Ctrl->EndPoit.Y);
            break;

          case controlAnchorSide::casEnd:
            EPos.Y = Ctrl->EndPoit.Y;
            break;
        }

        if (Ctrl != Nil)
          EPos.Y -= X->Margins.Y2;
      }


      // End Fixed
      if (!X->Anchors.Left.Active && X->Anchors.Righ.Active) SPos.X = EPos.X -X->Size.W;
      if (!X->Anchors.Top.Active  && X->Anchors.Bot.Active)  SPos.Y = EPos.Y -X->Size.H;


      // Set Attrs
      if (X->Poit.X != SPos.X || X->Poit.Y != SPos.Y)
      {
        X->Poit = SPos;
        X->Flag_Add(DirtyRebound);
      }

      if (X->Size.W != (EPos.X -SPos.X) || X->Size.H != (EPos.Y -SPos.Y))
      {
        X->Size = {
          .W = (EPos.X -SPos.X),
          .H = (EPos.Y -SPos.Y),
        };

        X->Size.W = max(X->MinSize.W, X->Size.W);
        X->Size.H = max(X->MinSize.H, X->Size.H);

        if (X->MaxSize.W != 0) X->Size.W = min(X->MaxSize.W, X->Size.W);
        if (X->MaxSize.H != 0) X->Size.H = min(X->MaxSize.H, X->Size.H);

        X->Flag_Add(DirtyResize);
      }

      if (X->Flag_HasR(DirtyResize))
        X->Do_Resize();

      if (X->Flag_HasR(DirtyRebound))
        X->EndPoit = {
          .X = X->Poit.X +X->Size.W,
          .Y = X->Poit.Y +X->Size.H,
        };

      X->Do_Paint_prepare();
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
    // Pass 1: Prepare & Calc Prefered Size
    for (auto &X: Childs)
    {
      if (!X->Visible)
        continue;

      X->Do_Paint_prepare();

      if (X->AutoSize && X->Flag_HasR(DirtyAutoSize))
      {
        X->CalcAutoSize();
        X->Flag_Rem(DirtyAutoSize);
      }
    }


    // Pass 2: Tiling
    if (Flag_HasR(DirtyTiling))
      Do_Tiling();


    // Pass 3: Final
    if (Flag_HasR(DirtyResize))
      Do_Resize();

    if (Flag_HasR(DirtyRebound))
      EndPoit = {
        .X = Poit.X +Size.W,
        .Y = Poit.Y +Size.H,
      };

  }

  void view::Do_Paint()
  {
    for (auto &X: Childs)
    {
      if (!X->Visible)
        continue;

      if (X->DirtyFlags & DirtyDraw)
        X->Do_Paint();
    }


    control::Do_Paint();
  }

  void view::Do_Resize()
  {
    Flag_Add(DirtyTiling);

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
