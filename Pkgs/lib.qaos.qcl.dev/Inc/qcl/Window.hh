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



namespace qcl
{

  class window: public view
  {
    public:
      window();
      explicit window(handle nOHID);
      ~window() override;

    public:
      handle OHID;

      shared_ptr<control> Overlay;
      
    public:
      void Show();
      void Hide();

      void Draw_after() override;

      control* FindInput(poit_i32 Pos) override;

      void Do_MouseDown(poit_i32 Pos, shiftStateSet Button, shiftStateSet State) override;
  };

}
