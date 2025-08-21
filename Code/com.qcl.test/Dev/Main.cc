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
#include "qcl/Graphic.hh"
#include "qcl/View.hh"
#include "qcl/Window.hh"
#include "qcl/Application.hh"
#include "qcl/DynamicLoad.hh"
#include "qcl/qstd/Standard.hh"


using namespace std;
using namespace jix;
using namespace jix::qcl;



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

int main()
{
  qstd::Register();


  auto Application = make_shared<qcl::application>();

  Application->MakeCurrent();



  auto QDL = qcl::dyn::Load_formFile(
    "Code/com.qcl.test/Des/Main.qdl", {

    RegFunc(But1_OnClick)
    RegFunc(Nav_Menu)
    RegFunc(Nav_Add)
  });

  dynamic_cast<qcl::qstd::form*>(QDL[0])->Show();


  Application->Run();
}
