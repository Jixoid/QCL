/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/


#include <iostream>
#include <vector>
#include <memory>

#include "Basis.h"

#include "qcl/Control.hh"
#include "qcl/Application.hh"
#include "qcl/DynamicLoad.hh"
#include "qstd/Standard.hh"


using namespace std;
using namespace jix;
using namespace qcl;



void But1_OnClick(qcl::control* Self)
{
  cout << "Click: " << Self->Tag << endl;
}


void Nav_Menu(qcl::control* Self)
{
  cout << "Menu" << endl;
}

void Nav_Add(qcl::control* Self)
{
  cout << "Add" << endl;
}


#define RegFunc(X) {#X, (point)&X},

extern "C" int qcl_entry2(shared_ptr<qcl::application> app)
{
  qstd::Register();


  qstd::Monet
    .Update(color(203.0/255.0, 151.0/255.0, 77.0/255.0));
    //.Update(color(49.0/255.0, 211.0/255.0, 76.0/255.0));
    //.Update(color(0.45, 0.30, 0.80));


  auto QDL = qcl::dyn::Load_FormFile(
    "Des/Main.qdl", {

    RegFunc(But1_OnClick)
    RegFunc(Nav_Menu)
    RegFunc(Nav_Add)
  });

  dynamic_cast<qstd::form*>(QDL[0].get())->Show();


  app->Run();
  return 0;
}

extern "C" void qcl_error(const char *Msg)
{
  cerr << Msg << endl;
}

