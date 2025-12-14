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

#include "qcl/Control.hh"
#include "qcl/View.hh"
#include "qcl/Window.hh"
#include "qcl/Graphic.hh"
#include "qcl/Platform.hh"

using namespace std;
using namespace jix;



namespace qcl
{
  window::window(): view()
  {
    platform::window::New(&OHID);

    CurrentApp->Window_Reg(this);


    Surface->Connect(this);
  }

  window::window(handle nOHID): view()
  {
    OHID = nOHID;

    CurrentApp->Window_Reg(this);


    Surface->Connect(this);
  }

  window::~window()
  {
    CurrentApp->Window_Dis(this);

    platform::window::Dis(OHID);
  }

  

  void window::Show()
  {
    platform::window::Show(OHID);
  }

  void window::Hide()
  {
    platform::window::Hide(OHID);
  }


  control* window::FindInput(poit_i32 Pos)
  {
    if (Overlay != Nil)
    {
      if (
        Pos.X >= ClientPos.X + Overlay->Poit.X &&
        Pos.Y >= ClientPos.Y + Overlay->Poit.Y &&
        
        Pos.X <= ClientPos.X + Overlay->EndPoit.X &&
        Pos.Y <= ClientPos.Y + Overlay->EndPoit.Y
      )
        return Overlay.get();

      return Nil;
    }

    el
      return view::FindInput(Pos);
  }

  void window::Do_MouseDown(poit_i32 Pos, shiftStateSet Button, shiftStateSet State)
  {
    view::Do_MouseDown(Pos, Button, State);

    if (Button == shiftStates::ssLeft && FHoverControl == Nil)
    {
      Overlay = Nil;

      CurrentApp->PushMessage(this, controlMessages::cmPaint);
    }
  }

  void window::Draw_after()
  {
    view::Draw_after();
    
    if (Overlay != Nil)
    {
      //if (Overlay->ControlFlags & controlFlags::MustDraw)
      //{
        Overlay->Do_Paint_prepare();
        Overlay->Do_Paint();
      //}


      Surface->Set_Pos({ (f32)(ClientPos.X + Overlay->Poit.X), (f32)(ClientPos.Y + Overlay->Poit.Y) });
      Surface->Set_Source(Overlay->Surface);
      Surface->Paint();
    }
  }



  void window::Handler_WindowStateChanged(windowStates State)
  {
    WindowState = State;

    Do_WindowStateChanged(State);
  }

  void window::Do_WindowStateChanged(windowStates State)
  {
    if (OnWindowStateChanged != Nil)
      OnWindowStateChanged(this, State);
  }

}
