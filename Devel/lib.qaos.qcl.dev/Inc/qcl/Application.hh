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
#include <vector>
#include <cmath>
#include <unordered_map>

#include "Basis.h"

#include "qcl/Control.hh"
#include "qcl/View.hh"
#include "qcl/Graphic.hh"

using namespace std;
using namespace jix;



namespace jix::qcl
{

  class application
  {
    public:
      application();
      ~application();
  

    public:
      handle OHID;

      unordered_map<handle, qcl::window*> WinList;

      
    public:
      void MakeCurrent();

      void Window_Reg(qcl::window* Win);
      void Window_Dis(qcl::window* Win);
      
      void SendMessage(qcl::control* Ctrl, controlMessages Msg);

      void Run();
  };


  extern application *CurrentApp;
}
