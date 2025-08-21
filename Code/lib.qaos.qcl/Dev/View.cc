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
#include "qcl/View.hh"
#include "qcl/Graphic.hh"
#include "qcl/Application.hh"

using namespace std;
using namespace jix;



namespace jix::qcl
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
      Ctrl->Handler_StateChanged(controlStates::csHover);

    if (FHoverControl != Nil)
      FHoverControl->Handler_StateChanged(controlStates::csNormal); 


    FHoverControl = Ctrl;
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
  }

  
  inline bool __ifScrollableVert(view* Self, i16 Delta)
  {
    return (Delta > 0) ? (Self->ClientBound.Y1 +Self->ClientPos.Y < 0):(Self->ClientBound.Y2 +Self->ClientPos.Y -Self->Size.H > 0);
  }

  inline bool __ifScrollableHorz(view* Self, i16 Delta)
  {
    return (Delta > 0) ? (Self->ClientBound.X1 +Self->ClientPos.X < 0):(Self->ClientBound.X2 +Self->ClientPos.X -Self->Size.W > 0);
  }



  void view::Draw_after()
  {
    for (auto &X: Childs)
    {
      if (!X->Visible)
        continue;

      if (X->ControlFlags & controlFlags::MustDraw)
        X->Do_Paint();

      Surface->Set_Pos({ (f32)(ClientPos.X + X->Poit.X), (f32)(ClientPos.Y + X->Poit.Y) });
      Surface->Set_Source(X->Surface);
      Surface->Paint();
    }


    if (__ifScrollableVert(this, +1) || __ifScrollableVert(this, -1))
    {
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

    if (__ifScrollableHorz(this, +1) || __ifScrollableHorz(this, -1))
    {
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
  
  }



  control* view::FindInput(poit_i32 Pos)
  {
    for (auto &X: views::reverse(Childs))
      if (
        (X->Visible && X->Enabled) &&

        Pos.X >= ClientPos.X + X->Poit.X &&
        Pos.Y >= ClientPos.Y + X->Poit.Y &&
        
        Pos.X <= ClientPos.X + X->EndPoit.X &&
        Pos.Y <= ClientPos.Y + X->EndPoit.Y
      )
        return X.get();

    return Nil;
  }


  void view::Do_Resize()
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
              TempP.X = (X->Anchors.Righ.Control->Visible) ? (X->Anchors.Righ.Control -> Poit.X):(X->Anchors.Righ.Control -> EndPoit.X);
              break;

            case controlAnchorSide::casEnd:
              TempP.X = X->Anchors.Righ.Control -> EndPoit.X;
              break;
          }
      }

      if (X->Anchors.Bot.Active)
      {
        if (X->Anchors.Bot.Control == this)
          TempE.Y = Size.H;

        else
          switch (X->Anchors.Bot.Side)
          {
            case controlAnchorSide::casBegin:
              TempP.Y = (X->Anchors.Bot.Control->Visible) ? (X->Anchors.Bot.Control -> Poit.Y):(X->Anchors.Bot.Control -> EndPoit.Y);
              break;

            case controlAnchorSide::casEnd:
              TempP.Y = X->Anchors.Bot.Control -> EndPoit.Y;
              break;
          }
      }



      if (X->Poit.X != TempP.X || X->Poit.Y != TempP.Y)
        X->Poit_Set(TempP);

      if (X->Size.W != TempE.X-TempP.X || X->Size.H != TempE.Y-TempP.Y)
        X->Size_Set({TempE.X-TempP.X, TempE.Y-TempP.Y});
    }


    // Calc Limit
    ClientBound = {0,0, Size.W, Size.H};
    for (auto &X: Childs)
    {
      if (X->Poit.X < ClientBound.X1)
        ClientBound.X1 = X->Poit.X;

      if (X->Poit.Y < ClientBound.Y1)
        ClientBound.Y1 = X->Poit.Y;

      if (X->EndPoit.X > ClientBound.X2)
        ClientBound.X2 = X->EndPoit.X;

      if (X->EndPoit.Y > ClientBound.Y2)
        ClientBound.Y2 = X->EndPoit.Y;
    }

    control::Do_Resize();
  }

  void view::Do_MouseDown(poit_i32 Pos, mouseButtonSet Button, mouseButtonSet State)
  {
    if (State == 0)
      HoverControlSet(FindInput(Pos));


    if (FHoverControl == Nil)
    {
      control::Do_MouseDown(Pos, Button, State);
      return;
    }

    FHoverControl->Handler_MouseDown({Pos.X -ClientPos.X -FHoverControl->Poit.X, Pos.Y  -ClientPos.Y -FHoverControl->Poit.Y}, Button, State);
  }

  void view::Do_MouseUp(poit_i32 Pos, mouseButtonSet Button, mouseButtonSet State)
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

  void view::Do_MouseMove (poit_i32 Pos, mouseButtonSet State)
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

  void view::Do_ScrollVert(poit_i32 Pos, i16 Delta, mouseButtonSet State)
  {
    control *Active = FindInput(Pos);

    if (Active == Nil || (dynamic_cast<qcl::view*>(Active) == Nil && __ifScrollableVert(this, Delta)))
    {
      control::Do_ScrollVert(Pos, Delta, State);


      if (__ifScrollableVert(this, Delta))
      {
        ClientPos.Y += Delta;
        DyeToRoot();
        CurrentApp->SendMessage(GetRoot(), controlMessages::cmPaint);

        if (State == 0)
          HoverControlSet(FindInput(Pos));
      }

      return;
    }

    Active->Handler_ScrollVert({Pos.X  -ClientPos.X -Active->Poit.X, Pos.Y  -ClientPos.Y -Active->Poit.Y}, Delta, State);
  }

  void view::Do_ScrollHorz(poit_i32 Pos, i16 Delta, mouseButtonSet State)
  {
    control *Active = FindInput(Pos);

    if (Active == Nil || (dynamic_cast<qcl::view*>(Active) == Nil && __ifScrollableHorz(this, Delta)))
    {
      control::Do_ScrollHorz(Pos, Delta, State);


      if (__ifScrollableHorz(this, Delta))
      {
        ClientPos.X += Delta;
        DyeToRoot();
        CurrentApp->SendMessage(GetRoot(), controlMessages::cmPaint);

        if (State == 0)
          HoverControlSet(FindInput(Pos));
      }

      return;
    }

    Active->Handler_ScrollHorz({Pos.X  -ClientPos.X -Active->Poit.X, Pos.Y  -ClientPos.Y -Active->Poit.Y}, Delta, State);
  }

}
