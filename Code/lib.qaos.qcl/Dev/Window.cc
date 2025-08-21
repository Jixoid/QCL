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
#include "qcl/Platform.hh"

using namespace std;
using namespace jix;



namespace jix::qcl
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

}
