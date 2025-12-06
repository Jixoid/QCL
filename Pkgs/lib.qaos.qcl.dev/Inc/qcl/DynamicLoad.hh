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
#include <map>

#include "Basis.h"

#include "qcl/Control.hh"
#include "qcl/View.hh"
#include "qcl/Effect.hh"


using namespace std;
using namespace jix;



namespace qcl::dyn
{
  extern map<string, control* (*)()> RegList;
  extern map<string, effect*  (*)()> EffList;

  void Register(const char* Name, control* (*Maker)());
  void Register(const char* Name, effect*  (*Maker)());

  
  using qdl = vector<shared_ptr<control>>;
  
  qdl Load_FormFile(string FPath, unordered_map<string, point> FuncMap);
  
  shared_ptr<control> FindFromName(qdl QDL, string Name);
}
