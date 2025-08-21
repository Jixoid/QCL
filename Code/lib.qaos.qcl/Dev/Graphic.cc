/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <iostream>
#include <memory>

#include "Basis.h"

#include "qcl/Graphic.hh"
#include "qcl/Platform.hh"

using namespace std;
using namespace jix;


namespace jix::qcl
{

  surface::surface(u32 nWidth, u32 nHeight)
  {
    platform::surface::New(&OHID, nWidth, nHeight);
  }

  surface::~surface()
  {
    platform::surface::Dis(OHID);
  }


  void surface::Set_Color(color Val)
  {
    platform::surface::Set_Color(OHID, Val);
  }

  void surface::Set_Source(shared_ptr<surface> Val)
  {
    platform::surface::Set_Source(OHID, std::move(Val));
  }

  void surface::Set_FontSize(f32 Size)
  {
    platform::surface::Set_FontSize(OHID, Size);
  }

  void surface::Set_LineSize(f32 Size)
  {
    platform::surface::Set_LineSize(OHID, Size);
  }


  void surface::Set_Pos(poit_f32 Val)
  {
    platform::surface::Set_Pos(OHID, Val);
  }

  void surface::Set_rPos(poit_f32 Val)
  {
    platform::surface::Set_rPos(OHID, Val);
  }

  void surface::Set_Size(u32 nWidth, u32 nHeight)
  {
    platform::surface::Set_Size(OHID, nWidth, nHeight);
  }


  void surface::Draw_Rect(rect_f32 Area)
  {
    platform::surface::Draw_Rect(OHID, Area);
  }

  void surface::Draw_RectRound(rect_f32 Area, f32 Radius)
  {
    platform::surface::Draw_RectRound(OHID, Area, Radius);
  }

  void surface::Draw_Line(poit_f32 P1, poit_f32 P2)
  {
    platform::surface::Draw_Line(OHID, P1, P2);
  }

  void surface::Draw_Text(const char* Text)
  {
    platform::surface::Draw_Text(OHID, Text);
  }

  size_f32 surface::Calc_Text(const char* Text)
  {
    size_f32 Ret;

    platform::surface::Calc_Text(OHID, Text, &Ret);

    return Ret;
  }


  void surface::Filter_GrayScale(rect_f32 Area)
  {
    platform::surface::Filter_GrayScale(OHID, Area);
  }


  void surface::Connect(qcl::window *Window)
  {
    platform::surface::Connect(OHID, Window->OHID);
  }
  
  void surface::Stroke()
  {
    platform::surface::Stroke(OHID);
  }

  void surface::Fill()
  {
    platform::surface::Fill(OHID);
  }

  void surface::Paint()
  {
    platform::surface::Paint(OHID);
  }


  void surface::Clear()
  {
    platform::surface::Clear(OHID);
  }

  void surface::Flush()
  {
    platform::surface::Flush(OHID);
  }

}