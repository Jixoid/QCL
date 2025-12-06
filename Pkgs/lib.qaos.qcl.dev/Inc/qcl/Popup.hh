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



namespace qcl
{

  struct popup_item
  {
    string Name;

    void (*OnClick)(qcl::control*) = Nil;


    popup_item *Parent = Nil;
    vector<shared_ptr<popup_item>> Items;
  };


  class popup: public control
  {
    public:
      popup();
      ~popup() override;


    public:
      shared_ptr<popup_item> Items;
      popup_item *VItems;

  };

}
