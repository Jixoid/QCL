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

#include "Basis.h"

using namespace std;
using namespace jix;



namespace jix::qcl
{

  enum mouseButtons: u16
  {
    btnLeft     = (u32)1 << 0,
    btnRight    = (u32)1 << 1,
    btnMiddle   = (u32)1 << 2,
  };

  using mouseButtonSet = u16;

}
