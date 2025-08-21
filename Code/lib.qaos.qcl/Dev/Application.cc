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

#include "Basis.h"

#include "qcl/Control.hh"
#include "qcl/View.hh"
#include "qcl/Window.hh"
#include "qcl/Graphic.hh"
#include "qcl/Application.hh"
#include "qcl/Platform.hh"

using namespace std;
using namespace jix;



namespace jix::qcl
{
  application::application()
  {
    platform::application::New(&OHID);
  }

  application::~application()
  {
    platform::application::Dis(OHID);

    if (CurrentApp == this)
      CurrentApp = Nil;
  }

  

  void application::MakeCurrent()
  {
    CurrentApp = this;
  }


  void application::Window_Reg(window* Win)
  {
    WinList[Win->OHID] = Win;
  }

  void application::Window_Dis(window* Win)
  {
    WinList.erase(Win->OHID);
  }


  void application::SendMessage(qcl::control* Ctrl, controlMessages Msg)
  {
    platform::application::SendMessage(Ctrl, Msg);
  }


  void application::Run()
  {
    platform::application::Run(OHID);
  }


  application *CurrentApp = Nil;
}
