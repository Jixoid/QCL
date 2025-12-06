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



namespace qcl
{
  class window;


  // Color
  struct color
  {
    f32
      A,R,G,B;

    inline color(f32 nR, f32 nG, f32 nB, f32 nA = 1.0f)
      : R(nR)
      , G(nG)
      , B(nB)
      , A(nA)
    {}

    inline color _A(f32 nA)
    {
      color Ret = *this;
      Ret.A = nA;

      return Ret;
    }

    inline color _R(f32 nR)
    {
      color Ret = *this;
      Ret.R = nR;

      return Ret;
    }
    
    inline color _G(f32 nG)
    {
      color Ret = *this;
      Ret.G = nG;

      return Ret;
    }

    inline color _B(f32 nB)
    {
      color Ret = *this;
      Ret.B = nB;

      return Ret;
    }


    inline bool operator==(const color &It) const
    {
      return (A == It.A) && (R == It.R) && (G == It.G) && (B == It.B);
    }

  };


  // Rect
  template <typename _T>
  struct rect
  {
    _T
      X1,Y1,
      X2,Y2;

    inline bool operator==(const rect &It) const
    {
      return (X1 == It.X1) && (X2 == It.X2) && (Y1 == It.Y1) && (Y2 == It.Y2);
    }
  };

  using rect_f32 = rect<f32>;
  using rect_i32 = rect<i32>;


  // Poit
  template <typename _T>
  struct poit
  {
    _T X,Y;

    inline bool operator==(const poit &It) const
    {
      return (X == It.X) && (Y == It.Y);
    }

    inline poit operator+(const poit &It) const
    {
      return {X +It.X, Y +It.Y};
    }

    inline poit& operator+=(const poit &It)
    {
      X += It.X;
      Y += It.Y;

      return *this;
    }
  };

  using poit_f32 = poit<f32>;
  using poit_i32 = poit<i32>;


  // Size
  template <typename _T>
  struct size
  {
    _T W,H;

    inline bool operator==(const size &It) const
    {
      return (W == It.W) && (H == It.H);
    }
  };

  using size_f32 = size<f32>;
  using size_i32 = size<i32>;


  // Surface
  class surface
  {
    public:

    enum operators: u16
    {
      soNormal = 0,
      soClear  = 1,
      soIn     = 2,
    };


    public:
      surface(u32 nWidth, u32 nHeight);
      surface(u32 nWidth, u32 nHeight, u32 *Data);
      surface(handle nOHID);
      
      static surface* FromFile_SVG(u32 nWidth, u32 nHeight, const char* Path);
      static surface* FromData_SVG(u32 nWidth, u32 nHeight, const u8* Data, u0 DSize);
      
      ~surface();

    public:
      handle OHID;


    public:
      u32 Width, Height;


    public:
      void Set_Color(color Val);
      void Set_Source(shared_ptr<surface> Val);
      void Set_SourceP(surface *Val);
      void Set_FontSize(f32 Size);
      void Set_LineSize(f32 Size);

      void      Set_OP(operators Op);
      operators Get_OP();

      void Set_Pos(poit_f32 Val);
      void Set_rPos(poit_f32 Val);
      void Set_Size(u32 nWidth, u32 nHeight);

      void Set_Dirty();
      u32* Get_Data();

    public:
      void Draw_Rect(rect_f32 Area);
      void Draw_RectRound(rect_f32 Area, f32 Radius);
      void Draw_RectRound(rect_f32 Area, f32 R_LT, f32 R_LB, f32 R_RT, f32 R_RB);
      void Draw_Line(poit_f32 P1, poit_f32 P2);
      void Draw_ToLine(poit_f32 P);
      void Draw_Arc(poit_f32 O, f32 Radius, f32 Ang1, f32 Ang2);
      void Draw_Text(const char* Text);

      size_f32 Calc_Text(const char* Text);

    public:
      void Filter_GrayScale(rect_f32 Area);
      
    
    public:
      void Connect(qcl::window *PlatformWindow);

      void Stroke();
      void Fill();
      void Paint();
      void Clip();
      void Clip_Reset();

      void Sets_Push();
      void Sets_Pop();

      void Clear();
      void Flush();
  };

}
