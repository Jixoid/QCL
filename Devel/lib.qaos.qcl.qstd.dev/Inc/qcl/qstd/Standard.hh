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
#include "qcl/Window.hh"

using namespace std;
using namespace jix;



namespace jix::qcl::qstd
{
  void Register();


  class form: public window
  {
    public:
      form();
      ~form() override;

      
    public:
      void Draw() override;

  };

  class layout: public view
  {
    public:
      layout();
      ~layout() override;


    public:
      color BackColor = color(0.16, 0.16, 0.16);
      u16 BorderRadius = 12;


    public:
      void Draw() override;

      bool LoadProp(string Name, jc_obj Prop) override;
  };

  class layout_vert: public layout
  {
    public:
      layout_vert();
      ~layout_vert() override;


    public:
      void Do_Resize() override;
  };

  class layout_horz: public layout
  {
    public:
      layout_horz();
      ~layout_horz() override;


    public:
      void Do_Resize() override;
  };

  class button: public control
  {
    public:
      button();
      ~button() override;


    public:
      string Text = "button";
      u16 BorderRadius = 12;


    public:
      void Draw() override;

      void Handler_StateChanged(controlStateSet State) override;

      bool LoadProp(string Name, jc_obj Prop) override;
  };

  class label: public control
  {
    public:
      label();
      ~label() override;


    public:
      string Text = "button";


    public:
      void Draw() override;

      bool LoadProp(string Name, jc_obj Prop) override;
  };

}
