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

#include "Basis.h"

#include "qcl/Control.hh"
#include "qcl/View.hh"
#include "qcl/Graphic.hh"

using namespace std;
using namespace jix;



namespace jix::qcl
{

  class window: public view
  {
    public:
      window();
      explicit window(handle nOHID);
      ~window() override;


    public:
      handle OHID;

      size_i32 LastSize;

      
    public:
      void Show();
      void Hide();

  };

}
