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
#include "qcl/Graphic.hh"

using namespace std;
using namespace jix;



namespace jix::qcl
{

  class view: public control
  {
    public:
      view();
      ~view() override;


    public:
      vector<shared_ptr<control>> Childs;

      control* FHoverControl = Nil;
      void HoverControlSet(control* Ctrl);

      void Child_Add(shared_ptr<control> Ctrl);
      void Child_Rem(control* Ctrl);

    public:
      rect_i32 ClientBound = {0,0,0,0};
      poit_i32 ClientPos = {0,0};

      bool ScrollableVert = false;
      bool ScrollableHorz = false;


    public:
      void Draw_after() override;


    public:
      virtual control* FindInput(poit_i32 Pos);

      void Do_Resize    () override;
      void Do_MouseDown (poit_i32 Pos, mouseButtonSet Button, mouseButtonSet State) override;
      void Do_MouseUp   (poit_i32 Pos, mouseButtonSet Button, mouseButtonSet State) override;
      void Do_MouseMove (poit_i32 Pos, mouseButtonSet State) override;
      void Do_ScrollVert(poit_i32 Pos, i16 Delta, mouseButtonSet State) override;
      void Do_ScrollHorz(poit_i32 Pos, i16 Delta, mouseButtonSet State) override;
  };

}
