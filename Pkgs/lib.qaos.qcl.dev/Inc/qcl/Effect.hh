/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include "Basis.h"

#include "qcl/Graphic.hh"
#include "JConf.hh"

using namespace std;
using namespace jix;



namespace qcl
{

  class effect
  {
    public:
      effect();
      virtual ~effect();

    public:
      bool Enabled = true;

    public:
      virtual void Reset() = 0;
      virtual void Update(size_i32 Size) = 0;
      virtual void Draw(surface *Surface, poit_f32 Pos) = 0;

      virtual bool LoadProp(string Name, const jconf::Value& Prop);
  };

}

