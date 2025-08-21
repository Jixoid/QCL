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
#include "qcl/Window.hh"
#include "qcl/Graphic.hh"
#include "qcl/Application.hh"

using namespace std;
using namespace jix;



namespace jix::qcl::platform
{
  extern "C" void qcl_entry (shared_ptr<qcl::application> app, shared_ptr<qcl::window> win);
  extern "C" void qcl_entry2(shared_ptr<qcl::application> app);
  
  
  namespace window
  {
    void New(handle *ret);
    void Dis(handle  val);

    void Show(handle val);
    void Hide(handle val);

    void     SizeSet(handle val, size_i32 Size);
    size_i32 SizeGet(handle val);
  }

  namespace surface
  {
    void New(handle *ret, u32 nWidth, u32 nHeight);
    void Dis(handle  val);


    void Set_Color(handle  val, color Val);
    void Set_Source(handle  val, shared_ptr<qcl::surface> Val);
    void Set_FontSize(handle  val, f32 Size);
    void Set_LineSize(handle  val, f32 Size);

    void Set_Pos(handle  val, poit_f32 Val);
    void Set_rPos(handle  val, poit_f32 Val);
    void Set_Size(handle  val, u32 nWidth, u32 nHeight);

    void Draw_Rect(handle  val, rect_f32 Area);
    void Draw_RectRound(handle  val, rect_f32 Area, f32 Radius);
    void Draw_Line(handle  val, poit_f32 P1, poit_f32 P2);
    void Draw_Text(handle  val, const char* Text);

    void Calc_Text(handle  val, const char* Text, size_f32* Size);

    void Filter_GrayScale(handle  val, rect_f32 Area);

    void Connect(handle  val, handle Window);

    void Stroke(handle  val);
    void Fill(handle  val);
    void Paint(handle  val);

    void Clear(handle  val);
    void Flush(handle  val);
  }

  namespace application
  {
    void New(handle *ret);
    void Dis(handle  val);


    void SendMessage(qcl::control* Ctrl, controlMessages Msg);

    void Run(handle val);
  }

}
