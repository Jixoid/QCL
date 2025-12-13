/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define el else
#define ef else if

#include <memory>
#include <vector>
#include <random>

#include "Basis.h"

#include "qcl/Control.hh"
#include "qcl/Graphic.hh"
#include "qcl/Application.hh"
#include "qcl/DynamicLoad.hh"
#include "qcl/Effect.hh"
#include "qstd/Standard.hh"

using namespace std;
using namespace jix;
using namespace qcl;



namespace qstd
{
  monet Monet;


  tuple<f64,f64,f64> rgb2hsl(color C)
  {
    f64 maxc = std::max({C.R, C.G, C.B});
    f64 minc = std::min({C.R, C.G, C.B});
    f64 delta = maxc -minc;

    // Lightness
    f64 L = (maxc +minc) /2.0;

    f64 H = 0.0;
    f64 S = 0.0;


    if (delta == 0.0)
    {
      // achromatic
      H = 0.0;
      S = 0.0;
    }

    el {
      // Saturation
      if (L < 0.5)
        S = delta /(maxc +minc);
      el
        S = delta /(2.0 -maxc -minc);

      // Hue
      if (maxc == C.R)
      {
        H = (C.G -C.B) /delta;
        
        if (C.G < C.B)
          H += 6.0;
      }
      
      ef (maxc == C.G)
      {
        H = ((C.B -C.R) /delta) +2.0;
      }
      
      el { // maxc == B
        H = ((C.R -C.G) /delta) +4.0;
      }

      H *= 60.0; // convert to degrees
      
      // ensure H in [0,360)
      if (H >= 360.0)
        H -= 360.0;
      
      if (H < 0.0)
        H += 360.0;
    }

    // convert S,L to percentages
    return {H, S *100.0, L *100.0};
  }

  f64 hue2rgb(f64 p, f64 q, f64 t)
  {
    if (t < 0.0) t += 1.0;
    if (t > 1.0) t -= 1.0;
    if (t < 1.0 /6.0) return p +(q -p) *6.0 *t;
    if (t < 1.0 /2.0) return q;
    if (t < 2.0 /3.0) return p +(q -p) *(2.0 /3.0 -t) *6.0;
    
    return p;
  }

  color hsl2rgb(f64 H, f64 S_percent, f64 L_percent)
  {
    f64 S = S_percent /100.0;
    f64 L = L_percent /100.0;

    color Ret(0,0,0);

    if (S == 0.0)
    {
      // achromatic
      Ret.R = Ret.G = Ret.B = L;
    }
    
    el {
      f64 h = H /360.0; // normalize hue to [0,1)
      f64 q = (L < 0.5) ? (L *(1.0 +S)) : (L +S -L *S);
      f64 p = 2.0 * L -q;

      Ret.R = hue2rgb(p, q, h +1.0 /3.0);
      Ret.G = hue2rgb(p, q, h);
      Ret.B = hue2rgb(p, q, h -1.0 /3.0);
    }

    return Ret;
  }


  monet::monet()
    : Main(0,0,0)
    , MainDark(0,0,0)
    , MainDarkS(0,0,0)
    , MainLight(0,0,0)
    , Text(0,0,0)
    , TextDis(0,0,0)
    , TextDark(0,0,0)
    , TextHighlight(0,0,0)
    , Gray(0,0,0)
    , GrayDark(0,0,0)
    , GrayLight(0,0,0)
    , Back(0,0,0)
    , BackDark(0,0,0)
  {}

  void monet::Update(color Color, bool Dark)
  {
    auto [H,S,L] = rgb2hsl(Color);

    if (Dark)
    {
      this->Main          = hsl2rgb(H, 55, 55);
      this->MainDark      = hsl2rgb(H, 55, 20);
      this->MainDarkS     = hsl2rgb(H, 55, 30);
      this->MainLight     = hsl2rgb(H, 55, 70);
      this->Text          = hsl2rgb(H, 55, 92);
      this->TextDis       = hsl2rgb(H, 55, 80);
      this->TextDark      = hsl2rgb(H, 55, 5);
      this->TextHighlight = hsl2rgb(H, 55, 80);
      this->Gray          = hsl2rgb(H, 8, 28);
      this->GrayDark      = hsl2rgb(H, 8, 18);
      this->GrayLight     = hsl2rgb(H, 8, 42);
      this->Back          = hsl2rgb(H, 3, 13);
      this->BackDark      = hsl2rgb(H, 3, 10);
    }

    el {
      this->Main          = hsl2rgb(H, 55, 45);
      this->MainDark      = hsl2rgb(H, 55, 75);
      this->MainDarkS     = hsl2rgb(H, 55, 60);
      this->MainLight     = hsl2rgb(H, 55, 30);
      this->Text          = hsl2rgb(H, 55, 8);
      this->TextDis       = hsl2rgb(H, 55, 22);
      this->TextDark      = hsl2rgb(H, 55, 95);
      this->TextHighlight = hsl2rgb(H, 55, 20);
      this->Gray          = hsl2rgb(H, 8, 72);
      this->GrayDark      = hsl2rgb(H, 8, 82);
      this->GrayLight     = hsl2rgb(H, 8, 58);
      this->Back          = hsl2rgb(H, 3, 86);
      this->BackDark      = hsl2rgb(H, 3, 83);
    }
  }


  color ParseColor(string Value)
  {
    #define Reg(X) (Value == #X) return X;

    if Reg(Monet.Main)
    ef Reg(Monet.MainLight)
    ef Reg(Monet.MainDark)
    ef Reg(Monet.MainDarkS)
    ef Reg(Monet.Text)
    ef Reg(Monet.TextDis)
    ef Reg(Monet.TextHighlight)
    ef Reg(Monet.TextDark)
    ef Reg(Monet.Gray)
    ef Reg(Monet.GrayLight)
    ef Reg(Monet.GrayDark)
    ef Reg(Monet.Back)
    ef Reg(Monet.BackDark)

    el return color(0,0,0,0);

    #undef Reg
  }



  #pragma region noise

  noise::noise()
    : effect()
  {
    Reset();
  }

  noise::~noise()
  {
    if (From != Nil)
      delete From;
    
    if (Noise != Nil)
      delete Noise;

    if (Data != Nil)
      delete Data;
  }


  void noise::Reset()
  {
    if (Data != Nil)
      delete Data;

    if (Noise != Nil)
      delete Noise;


    // RNG
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(0, (int)VNoise);

    // Önce grayscale noise üret
    vector<u8> gray(WIDTH * HEIGHT);
    for (int i = 0; i < WIDTH * HEIGHT; ++i)
      gray[i] = static_cast<u8>(dist(mt));


    Data = new vector<u32>(WIDTH * HEIGHT, 0);
    for (int y = 0; y < HEIGHT; ++y)
      for (int x = 0; x < WIDTH; ++x)
      {
        u8 v = gray[y *WIDTH + x];
        
        // grayscale -> RGB
        //u32 pixel = (u32(v) << 24) | (0 << 16) | (0 << 8) | 0;
        u32 pixel = (0 << 24) | (u32(v) << 16) | (u32(v) << 8) | u32(v);
        
        (*Data)[y * WIDTH + x] = pixel;
      }


    Noise = new surface(WIDTH, HEIGHT, Data->data());
  }

  void noise::Update(size_i32 Size)
  {
    if (From == Nil)
      From = new surface(Size.W, Size.H);

    if (LastSize == Size)
      return;


    From->Set_Size(Size.W, Size.H);

    u32 AW = (Size.W /512) +1;
    u32 AH = (Size.H /512) +1;


    for (u32 X = 0; X < AW; X++)
      for (u32 Y = 0; Y < AH; Y++)
      {
        From->Set_Pos({(f32)X*512, (f32)Y*512});
        From->Set_SourceP(Noise);
        From->Paint();
      }
  
  }

  void noise::Draw(surface *Surface, poit_f32 Pos)
  {
    Surface->Set_Pos(Pos);
    Surface->Set_SourceP(From);
    Surface->Paint();
  }


  bool noise::LoadProp(string Name, const jconf::Value& Prop)
  {
    if (effect::LoadProp(Name, Prop))
      return true;


    if (Name == "Noise")
    {
      if (!Prop.isInt())
        return false;

      this->VNoise = (i64)Prop;

      Reset();
      return true;
    }

    else
      return false;
  }

  #pragma endregion




  #pragma region form

  form::form()
    : window()
  {}

  form::~form()
  {}


  void form::Draw()
  {
    Surface->Set_Color(qcl::color(0.12, 0.12, 0.12));

    Surface->Draw_Rect({0,0, (f32)Size.W, (f32)Size.H});
    Surface->Fill();
  }

  #pragma endregion


  #pragma region layout

  layout::layout()
    : view()
    , Color(Monet.Back)
  {}

  layout::~layout()
  {}


  void layout::Draw()
  {
    Surface->Set_Color(color(1,1,1,1));
    Surface->Draw_RectRound({0,0, (f32)Size.W, (f32)Size.H}, BorderRadius);
    Surface->Clip();

    Surface->Set_Color(Color);
    Surface->Draw_Rect({0,0, (f32)Size.W, (f32)Size.H});
    Surface->Fill();

    for (auto &X: Effects)
    {
      if (!X->Enabled) continue;

      X->Update({Size.W, Size.H});
      X->Draw(Surface.get(), {0,0});
    }

    Surface->Clip_Reset();
  }

  void layout::Draw_ScrollVert()
  {
    if (!ScrollVertVisible)
      return;

    Surface->Set_Color(color(1,1,1,1));
    Surface->Draw_RectRound({0,0, (f32)Size.W, (f32)Size.H}, BorderRadius);
    Surface->Clip();


    Surface->Set_LineSize(6);

    Surface->Set_Color(Monet.GrayDark._A(0.4));
    Surface->Draw_Line({(f32)Size.W -3,0}, {(f32)Size.W -3, (f32)Size.H});
    Surface->Stroke();

    f32
      MaxSize = (ClientBound.Y2-ClientBound.Y1),
      Pos = -ClientPos.Y,
      Height = Size.H;

    Surface->Set_Color(Monet.Main);
    Surface->Draw_Line(
      {(f32)Size.W -3, ((Pos /MaxSize) *Height)},
      {(f32)Size.W -3, ((Pos /MaxSize) *Height) +((Height /MaxSize) *Height)}
    );
    Surface->Stroke();


    Surface->Clip_Reset();
  }

  void layout::Draw_ScrollHorz()
  {
    if (!ScrollHorzVisible)
      return;

    Surface->Set_Color(color(1,1,1,1));
    Surface->Draw_RectRound({0,0, (f32)Size.W, (f32)Size.H}, BorderRadius);
    Surface->Clip();


    Surface->Set_LineSize(6);

    Surface->Set_Color(Monet.GrayDark._A(0.4));
    Surface->Draw_Line({0, (f32)Size.H -3}, {(f32)Size.W, (f32)Size.H -3});
    Surface->Stroke();

    f32
      MaxSize = (ClientBound.X2-ClientBound.X1),
      Pos = -ClientPos.X,
      Width = Size.W;

    Surface->Set_Color(Monet.Main);
    Surface->Draw_Line(
      {((Pos /MaxSize) *Width), (f32)Size.H -3},
      {((Pos /MaxSize) *Width) +((Width /MaxSize) *Width), (f32)Size.H -3}
    );
    Surface->Stroke();


    Surface->Clip_Reset();
  }

  bool layout::LoadProp(string Name, const jconf::Value& Prop)
  {
    if (view::LoadProp(Name, Prop))
      return true;


    if (Name == "BorderRadius")
    {
      if (!Prop.isInt())
        return false;

      this->BorderRadius = (i64)Prop;

      return true;
    }

    ef (Name == "Color")
    {
      if (!Prop.isString())
        return false;

      string Cac = (string)Prop;

      this->Color = ParseColor(Cac);
      if (this->Color == color(0,0,0,0))
        return false;
      
      return true;
    }

    else
      return false;
  }

  #pragma endregion


  #pragma region layout_vert

  layout_vert::layout_vert()
    : layout()
  {}

  layout_vert::~layout_vert()
  {}


  void layout_vert::Do_Tiling()
  {
    // Apply Anchors
    for (auto &X: Childs)
    {
      poit_i32 TempP = X->Poit;

      if (X->Anchors.Left.Active)
      {
        if (X->Anchors.Left.Control == this)
          TempP.X = 0;

        else
          switch (X->Anchors.Left.Side)
          {
            case controlAnchorSide::casBegin:
              TempP.X = X->Anchors.Left.Control -> Poit.X;
              break;

            case controlAnchorSide::casEnd:
              TempP.X = (X->Anchors.Left.Control->Visible) ? (X->Anchors.Left.Control -> EndPoit.X):(X->Anchors.Left.Control -> Poit.X);
              break;
          }

        TempP.X += X->Margins.X1;
      }

      if (X->Anchors.Top.Active)
      {
        if (X->Anchors.Top.Control == this)
          TempP.Y = 0;

        else
          switch (X->Anchors.Top.Side)
          {
            case controlAnchorSide::casBegin:
              TempP.Y = X->Anchors.Top.Control -> Poit.Y;
              break;

            case controlAnchorSide::casEnd:
              TempP.Y = (X->Anchors.Top.Control->Visible) ? (X->Anchors.Top.Control -> EndPoit.Y):(X->Anchors.Top.Control -> Poit.Y);
              break;
          }

        TempP.Y += X->Margins.Y1;
      }


      
      poit_i32 TempE = {TempP.X +X->Size.W, TempP.Y +X->Size.H};

      if (X->Anchors.Righ.Active)
      {
        if (X->Anchors.Righ.Control == this)
          TempE.X = Size.W;

        else
          switch (X->Anchors.Righ.Side)
          {
            case controlAnchorSide::casBegin:
              TempE.X = (X->Anchors.Righ.Control->Visible) ? (X->Anchors.Righ.Control -> Poit.X):(X->Anchors.Righ.Control -> EndPoit.X);
              break;

            case controlAnchorSide::casEnd:
              TempE.X = X->Anchors.Righ.Control -> EndPoit.X;
              break;
          }

        TempE.X -= X->Margins.X2;
      }

      if (X->Anchors.Bot.Active)
      {
        if (X->Anchors.Bot.Control == this)
          TempE.Y = Size.H;

        else
          switch (X->Anchors.Bot.Side)
          {
            case controlAnchorSide::casBegin:
              TempE.Y = (X->Anchors.Bot.Control->Visible) ? (X->Anchors.Bot.Control -> Poit.Y):(X->Anchors.Bot.Control -> EndPoit.Y);
              break;

            case controlAnchorSide::casEnd:
              TempE.Y = X->Anchors.Bot.Control -> EndPoit.Y;
              break;
          }

        TempE.Y -= X->Margins.Y2;
      }


      if (!X->Anchors.Top.Active  && X->Anchors.Bot.Active)  TempP.Y = TempE.Y -X->Size.H;
      if (!X->Anchors.Left.Active && X->Anchors.Righ.Active) TempP.X = TempE.X -X->Size.W;


      if (X->Poit.X != TempP.X || X->Poit.Y != TempP.Y)
      {
        X->Poit = TempP;
        X->Flag_Add(DirtyRebound);
      }

      if (X->Size.W != TempE.X-TempP.X || X->Size.H != TempE.Y-TempP.Y)
      {
        X->Size = {TempE.X-TempP.X, TempE.Y-TempP.Y};
        X->Flag_Add(DirtyResize | DirtyRebound);
      }

    }


    // Set vert layout
    i32 StartPos = 0;
    i32 LastMargin = 0;

    for (auto &X: Childs)
    {
      StartPos += (X->Margins.Y1 > LastMargin) ? (X->Margins.Y1):(LastMargin);

      if (X->Anchors.Bot.Active)
      {
        X->Poit = {X->Margins.X1, X->Poit.Y};
        X->Flag_Add(DirtyRebound);
        continue;
      }
      
      X->Poit = {X->Margins.X1, StartPos};
      X->Flag_Add(DirtyRebound);

      LastMargin = X->Margins.Y2;
      StartPos += X->Size.H;
    }

  }

  void layout_vert::CalcAutoSize()
  {
    // Set vert layout
    i32 StartPos = 0;
    i32 LastMargin = 0;

    for (auto &X: Childs)
    {
      StartPos += (X->Margins.Y1 > LastMargin) ? (X->Margins.Y1):(LastMargin);

      LastMargin = X->Margins.Y2;
      StartPos += X->Size.H;
    }
    StartPos += LastMargin;


    i32 AWidth = 0;
    for (auto &X: Childs)
      AWidth = max(AWidth, X->Margins.X1 +X->Margins.X2 +X->Size.W);


    PreferedSize = {AWidth, StartPos};
  }

  #pragma endregion


  #pragma region layout_horz

  layout_horz::layout_horz()
    : layout()
  {}

  layout_horz::~layout_horz()
  {}


  void layout_horz::Do_Tiling()
  {
    // Set vert layout
    i32 StartPos = 0;
    i32 LastMargin = 0;

    for (auto &X: Childs)
    {
      StartPos += (X->Margins.X1 > LastMargin) ? (X->Margins.X1):(LastMargin);

      if (X->Anchors.Righ.Active)
      {
        X->Poit = {X->Poit.X, X->Margins.Y1};
        X->Flag_Add(DirtyRebound);
        continue;
      }

      X->Poit = {StartPos, X->Margins.Y1};
      X->Flag_Add(DirtyRebound);

      LastMargin = X->Margins.X2;
      StartPos += X->Size.W;
    }

    view::Do_Tiling();
  }

  void layout_horz::CalcAutoSize()
  {
    // Set horz layout
    i32 StartPos = 0;
    i32 LastMargin = 0;

    for (auto &X: Childs)
    {
      StartPos += (X->Margins.X1 > LastMargin) ? (X->Margins.X1):(LastMargin);

      LastMargin = X->Margins.X2;
      StartPos += X->Size.W;
    }
    StartPos += LastMargin;


    i32 AHeight = 0;
    for (auto &X: Childs)
      AHeight = max(AHeight, X->Margins.Y1 +X->Margins.Y2 +X->Size.H);


    PreferedSize = {StartPos, AHeight};
  }

  #pragma endregion


  #pragma region layout_flow

  layout_flow::layout_flow()
    : layout()
  {}

  layout_flow::~layout_flow()
  {}


  void layout_flow::Do_Tiling()
  {
    poit_i32 CPos = {0, 0};
    poit_i32 LPad = {0, 0}; // R,B
    i32 LineHeight = 0;

    for (auto &X: Childs)
    {
      if (!X->Visible)
          continue;

      // Satırda sığacak mı?
      if (CPos.X + X->Size.W +X->Margins.X1 +LPad.X > Size.W)
      {
        // Yeni satıra geç
        CPos.X = 0;
        CPos.Y += LineHeight +LPad.Y +X->Margins.Y1;
        LineHeight = 0;
      }

      if (CPos.X == 0)
        CPos.X = X->Margins.X1;

      if (CPos.Y == 0)
        CPos.Y = X->Margins.Y1;


      // Konum ata
      X->Poit.X = CPos.X;
      X->Poit.Y = CPos.Y;

      // Bir sonraki eleman için ilerle
      CPos.X += X->Size.W +X->Margins.X1 +LPad.X;

      // Satır yüksekliğini güncelle
      if (X->Size.H > LineHeight)
        LineHeight = X->Size.H;
    }

    view::Do_Tiling();
  }

  void layout_flow::CalcAutoSize()
  {
    poit_i32 CPos = {0, 0};
    poit_i32 LPad = {0, 0}; // R,B
    i32 LineHeight = 0;

    i32 MaxW = 0;
    i32 MaxH = 0;

    for (auto &X: Childs)
    {
      if (!X->Visible)
          continue;

      // Satırda sığacak mı?
      if (CPos.X + X->Size.W +X->Margins.X1 +LPad.X > Size.W)
      {
        // Yeni satıra geç
        MaxW = max(MaxW, CPos.X);

        CPos.X = 0;
        CPos.Y += LineHeight +LPad.Y +X->Margins.Y1;
        LineHeight = 0;
      }

      if (CPos.X == 0)
        CPos.X = X->Margins.X1;

      if (CPos.Y == 0)
        CPos.Y = X->Margins.Y1;


      // Konum ata
      //X->Poit.X = CPos.X;
      //X->Poit.Y = CPos.Y;

      // Bir sonraki eleman için ilerle
      CPos.X += X->Size.W +X->Margins.X1 +LPad.X;

      // Satır yüksekliğini güncelle
      if (X->Size.H > LineHeight)
        LineHeight = X->Size.H;
    }

    MaxW = max(MaxW, CPos.X);
    MaxH = CPos.Y + LineHeight;

    PreferedSize = {MaxW, MaxH};
  }

  #pragma endregion


  #pragma region text

  text::text()
    : control()
    , Color(Monet.Text)
  {
    AutoSize = true;
  }

  text::~text()
  {}


  void text::Draw()
  {
    Surface->Set_FontSize(FontSize);

    size_f32 TSize = Surface->Calc_Text(Text.c_str());


    poit_f32 Pos;

    switch (AlignVert)
    {

      case taTop:
      {
        Pos.Y = 2;
        break;
      }

      case taCenter:
      {
        Pos.Y = (Size.H -TSize.H)/2;
        break;
      }

      case taBottom:
      {
        Pos.Y = Size.H -TSize.H -2;
        break;
      }

    }

    switch (AlignHorz)
    {

      case taLeft:
      {
        Pos.X = 2;
        break;
      }

      case taCenter:
      {
        Pos.X = (Size.W -TSize.W)/2;
        break;
      }

      case taRight:
      {
        Pos.X = Size.W -TSize.W -2;
        break;
      }

    }


    Surface->Set_Color(Color);
    Surface->Set_Pos(Pos);
    Surface->Draw_Text(Text.c_str());
    Surface->Fill();
  }

  void text::CalcAutoSize()
  {
    Surface->Set_FontSize(FontSize);
    size_f32 TSize = Surface->Calc_Text(Text.c_str());

    TSize.W += 8;
    TSize.H += 8;
    
    PreferedSize = {(i32)TSize.W, (i32)TSize.H};
  }

  bool text::LoadProp(string Name, const jconf::Value& Prop)
  {
    if (control::LoadProp(Name, Prop))
      return true;


    if (Name == "Text")
    {
      if (!Prop.isString())
        return false;


      Text = (string)Prop;
      
      Flag_Add(DirtyAutoSize);
      return true;
    }

    ef (Name == "Color")
    {
      if (!Prop.isString())
        return false;

      string Cac = (string)Prop;

      Color = ParseColor(Cac);
      if (Color == color(0,0,0,0))
        return false;

      return true;
    }

    ef (Name == "FontSize")
    {
      if (!Prop.isInt())
        return false;

      
      FontSize = (i64)Prop;

      Flag_Add(DirtyAutoSize);
      return true;
    }

    ef (Name == "AlignVert")
    {
      text_align Nat;

      if (!Prop.isString())
        return false;


      // Val
      string Cac = (string)Prop;      

      if (Cac == "top")
        Nat = text_align::taTop;

      ef (Cac == "center")
        Nat = text_align::taCenter;
      
      ef (Cac == "bottom")
        Nat = text_align::taBottom;

      else
        return false;


      AlignVert = (Nat);
      return true;
    }

    ef (Name == "AlignHorz")
    {
      text_align Nat;

      if (!Prop.isString())
        return false;


      // Val
      string Cac = (string)Prop;

      if (Cac == "left")
        Nat = text_align::taLeft;

      ef (Cac == "center")
        Nat = text_align::taCenter;
      
      ef (Cac == "right")
        Nat = text_align::taRight;

      else
        return false;


      AlignHorz = (Nat);
      return true;
    }

    else
      return false;
  }

  #pragma endregion
  

  #pragma region icon

  icon::icon()
    : control()
  {}

  icon::~icon()
  {}


  void icon::Draw()
  {
    if (Icon == Nil)
      return;

    Surface->Set_Pos({0,0});
    Surface->Set_Source(Icon);
    Surface->Paint();
  }


  bool icon::LoadIcon()
  {
    auto Sur = surface::FromFile_SVG(Size.W, Size.H, Path.c_str());

    Icon = shared_ptr<surface>(Sur);

    return true;
  }


  bool icon::LoadProp(string Name, const jconf::Value& Prop)
  {
    if (control::LoadProp(Name, Prop))
      return true;


    if (Name == "Path")
    {
      if (!Prop.isString())
        return false;


      Path = (string)Prop;

      LoadIcon();
      return true;
    }

    else
      return false;
  }

  #pragma endregion
  

  #pragma region edit

  edit::edit()
    : control()
  {}

  edit::~edit()
  {}


  void edit::Draw()
  {
    color Theme(0,0,0,0);

    if ((ControlState & (csFocus)) != 0)
      Theme = Monet.Main;

    ef ((ControlState & (csHover)) != 0)
      Theme = Monet.MainLight;

    else
      Theme = Monet.Gray;
      
    
    Surface->Set_Color(Theme);
    Surface->Set_LineSize(2);
    Surface->Draw_RectRound(rect_f32{0+1,4+1, (f32)Size.W-1, (f32)Size.H-1}, BorderRadius);
    Surface->Stroke();


    if (!Title.empty())
    {
      Surface->Set_FontSize(10);

      size_f32 TSize = Surface->Calc_Text(Title.c_str());

      
      auto OpBak = Surface->Get_OP();
      Surface->Set_OP(surface::operators::soClear);

      Surface->Set_Color(color(0,0,0));
      Surface->Draw_Rect(rect_f32{11,4, 12+6+TSize.W, 6});
      Surface->Fill();

      Surface->Set_OP(OpBak);


      Surface->Set_Color(color(1,1,1));
      Surface->Set_Pos({14, 0});
      Surface->Draw_Text(Title.c_str());
      Surface->Fill();
    }


    Surface->Set_FontSize(15);

    size_f32 TSize = Surface->Calc_Text(Text.c_str());

    Surface->Set_Color(color(1,1,1));
    Surface->Set_Pos({12, ((Size.H -5 -TSize.H)/2) +5});
    Surface->Draw_Text(Text.c_str());
    Surface->Fill();


    if ((ControlState & (csFocus)) != 0)
    {
      size_f32 PosSize = Surface->Calc_Text((Text.substr(0, Pos)+".").c_str());

      Surface->Set_Color(Monet.Text);
      
      Surface->Draw_Line(
        {12 +PosSize.W -4, (((f32)Size.H -5)/2) +5 -(15/2)},
        {12 +PosSize.W -4, (((f32)Size.H -5)/2) +5 +(15/2)}
      );
      Surface->Stroke();
    }
  }


  void utf8_backspace_at(string &s, u0 &pos)
  {
    if (s.empty() || pos == 0)
      return;

    u0 i = pos - 1;
    
    // Continuation byte (10xxxxxx) yakaladıkça geri git
    while (i > 0 && ((s[i] & 0xC0) == 0x80))
      --i;

    s.erase(i, pos - i);
    pos = i;
  }


  u32 utf8_length(string s)
  {
    u0 count = 0;
    for (u8 c: s)
      if ((c & 0xC0) != 0x80) // UTF-8 continuation bytes'i atla
        ++count;

    return count;
  }

  u32 utf8_length(const char *s)
  {
    u0 count = 0;

    u0 i = 0;
    while (s[i] != 0)
    {
      if ((s[i] & 0xC0) != 0x80) // UTF-8 continuation bytes'i atla
        ++count;

      ++i;
    }


    return count;
  }

  i8 utf8_char_length(char C)
  {
    if ((C & 0x80) == 0x00) return 1;  // 0xxxxxxx
    ef ((C & 0xE0) == 0xC0) return 2;  // 110xxxxx
    ef ((C & 0xF0) == 0xE0) return 3;  // 1110xxxx
    ef ((C & 0xF8) == 0xF0) return 4;  // 11110xxx
    
    el
      return -1; // geçersiz UTF-8 başlangıcı
  }

  i8 utf8_char_length_from_end(const char* str, u0 len)
  {
    if (len == 0)
      return 0;


    i8 i = 1;
    while (true)
    {
      if ((*(str -i) & 0xC0) != 0x80) // UTF-8 continuation bytes'i atla
        break;

      ++i;
    }

    return i;
  }



  void edit::Handler_StateChanged(controlStateSet State)
  {
    control::Handler_StateChanged(State);

    DyeToRoot();
    CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
  }

  void edit::Do_KeyDown(char *Key, u32 KeyCode, shiftStateSet State)
  {
    if (KeyCode == 113)
      Pos = (Pos == 0 ? 0:Pos -utf8_char_length_from_end((Text.c_str()+Pos), Pos));

    ef (KeyCode == 114)
      Pos = min<u32>(Text.size(), Pos +utf8_char_length(*(Text.c_str()+Pos)));

    ef (KeyCode == 111)
      Pos = 0;

    ef (KeyCode == 116)
      Pos = Text.size();


    if (KeyCode != 0)
    {
      DyeToRoot();
      CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
      return;
    }
    


    if (Key[0] == 0x08)
    {
      utf8_backspace_at(Text, Pos);
    }

    else
    {
      Text.insert(Pos, string(Key));
      Pos += utf8_char_length(Key[0]);
    }


    DyeToRoot();
    CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
  }


  bool edit::LoadProp(string Name, const jconf::Value& Prop)
  {
    if (control::LoadProp(Name, Prop))
      return true;


    if (Name == "Text")
    {
      if (!Prop.isString())
        return false;

      Text = (string)Prop;

      return true;
    }

    ef (Name == "Title")
    {
      if (!Prop.isString())
        return false;

      Title = (string)Prop;

      return true;
    }

    ef (Name == "BorderRadius")
    {
      if (!Prop.isInt())
        return false;

      BorderRadius = (i64)Prop;

      return true;
    }

    else
      return false;
  }

  #pragma endregion


  #pragma region radio

  radio::radio()
    : control()
  {}

  radio::~radio()
  {}


  void radio::Draw()
  {
    color Theme(0,0,0,0);

    if ((ControlState & (csHover)) != 0)
      Theme = Monet.MainLight;
    else
      Theme = Monet.Gray;

    
    Surface->Set_Color(Theme);
    Surface->Set_LineSize(2);

    Surface->Draw_Arc({11, 11}, 10, 0, M_PI*2);
    Surface->Stroke();

    if (Checked)
    {
      Surface->Set_Color(color(1,1,1,1));
      Surface->Draw_Arc({11,11}, 5, 0, M_PI*2);
      Surface->Clip();

      Surface->Set_Color(Monet.Main);
      Surface->Draw_Rect({11-5,11-5, 11+5,11+5});
      Surface->Fill();

      for (auto &X: Effects)
      {
        if (!X->Enabled) continue;

        X->Update({10,10});
        X->Draw(Surface.get(), {11-5,11-5});
      }

      Surface->Clip_Reset();
    }


    Surface->Set_FontSize(15);

    size_f32 TSize = Surface->Calc_Text(Text.c_str());

    Surface->Set_Color(Monet.Text);
    Surface->Set_Pos({27, ((Size.H -TSize.H)/2)});
    Surface->Draw_Text(Text.c_str());
    Surface->Fill();
  }

  void radio::Handler_StateChanged(controlStateSet State)
  {
    control::Handler_StateChanged(State);

    DyeToRoot();
    CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
  }

  void radio::Do_Click()
  {
    if (Parent != Nil)
      for (auto &X: Parent->Childs)
        if (auto C = dynamic_cast<qstd::radio*>(X.get()); C != Nil)
          if (C->Checked && C != this)
            C->Do_Changed(false);
    
    if (!Checked)
      Do_Changed(true);
    

    control::Do_Click();
  }

  void radio::Do_Changed(bool Status)
  {
    Checked = Status;

    if (OnChanged != Nil)
      OnChanged(this, Status);


    DyeToRoot();
    CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
  }

  bool radio::LoadProp(string Name, const jconf::Value& Prop)
  {
    if (control::LoadProp(Name, Prop))
      return true;


    if (Name == "Text")
    {
      if (!Prop.isString())
        return false;

      Text = (string)Prop;
      return true;
    }

    ef (Name == "Checked")
    {
      if (!Prop.isBool())
        return false;
      
      Checked = (bool)Prop;

      return true;
    }

    else
      return false;
  }

  bool radio::LoadFunc(string Name, point Func)
  {
    if (control::LoadFunc(Name, Func))
      return true;


    #define makro(X)  if (Name == #X) { X = (decltype(X))Func; return true; }

    makro(OnChanged)

    #undef makro

    return false;
  }

  #pragma endregion


  #pragma region check

  check::check()
    : control()
  {}

  check::~check()
  {}


  void check::Draw()
  {
    color Theme(0,0,0,0);

    if ((ControlState & (csHover)) != 0)
      Theme = Monet.MainLight;
    else
      Theme = Monet.Gray;
      
    
    Surface->Set_Color(Theme);
    Surface->Set_LineSize(2);

    Surface->Draw_RectRound({1,1, 21, 21}, 7);
    Surface->Stroke();

    if (Checked)
    {
      Surface->Set_Color(color(1,1,1,1));
      Surface->Draw_RectRound({11-5,11-5, 11+5,11+5}, 3);
      Surface->Clip();
      
      Surface->Set_Color(Monet.Main);
      Surface->Draw_Rect({11-5,11-5, 11+5,11+5});
      Surface->Fill();

      for (auto &X: Effects)
      {
        if (!X->Enabled) continue;

        X->Update({10,10});
        X->Draw(Surface.get(), {11-5,11-5});
      }

      Surface->Clip_Reset();
    }


    Surface->Set_FontSize(15);

    size_f32 TSize = Surface->Calc_Text(Text.c_str());

    Surface->Set_Color(Monet.Text);
    Surface->Set_Pos({27, ((Size.H -TSize.H)/2)});
    Surface->Draw_Text(Text.c_str());
    Surface->Fill();
  }

  void check::Handler_StateChanged(controlStateSet State)
  {
    control::Handler_StateChanged(State);

    DyeToRoot();
    CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
  }

  void check::Do_Click()
  {
    Do_Changed(!Checked);

    control::Do_Click();
  }

  void check::Do_Changed(bool Status)
  {
    Checked = Status;

    if (OnChanged != Nil)
      OnChanged(this, Status);


    DyeToRoot();
    CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
  }

  bool check::LoadProp(string Name, const jconf::Value& Prop)
  {
    if (control::LoadProp(Name, Prop))
      return true;


    if (Name == "Text")
    {
      if (!Prop.isString())
        return false;

      Text = (string)Prop;
      return true;
    }

    ef (Name == "Checked")
    {
      if (!Prop.isBool())
        return false;

      Checked = (bool)Prop;

      return true;
    }

    else
      return false;
  }

  bool check::LoadFunc(string Name, point Func)
  {
    if (control::LoadFunc(Name, Func))
      return true;


    #define makro(X)  if (Name == #X) { X = (decltype(X))Func; return true; }

    makro(OnChanged)

    #undef makro

    return false;
  }

  #pragma endregion


  #pragma region toggle

  toggle::toggle()
    : control()
  {}

  toggle::~toggle()
  {}


  void toggle::Draw()
  {
    color Theme(0,0,0,0);
    color Theme2(0,0,0,0);

    if ((ControlState & (csHover)) != 0)
    {
      Theme = Monet.MainLight;
      Theme2 = Monet.Main;
    }
    else
    {
      Theme = Monet.Gray;
      Theme2 = Monet.GrayLight;
    }


    Surface->Set_LineSize(2);

    if (!Checked)
    {
      Surface->Set_Color(Theme);
      Surface->Draw_RectRound({(f32)Size.W -50, 1, (f32)Size.W -1, 25}, 12);
      Surface->Stroke();

      Surface->Set_Color(Theme2);
      Surface->Draw_Arc({(f32)Size.W -38, 13}, 7, 0, M_PI*2);
      Surface->Fill();
    }

    el {
      Surface->Set_Color(color(1,1,1,1));
      Surface->Draw_RectRound({(f32)Size.W -51, 0, (f32)Size.W, 26}, 14);
      Surface->Clip();

      Surface->Set_Color(Monet.Main);
      Surface->Draw_Rect({(f32)Size.W -51, 0, (f32)Size.W, 26});
      Surface->Fill();

      for (auto &X: Effects)
      {
        if (!X->Enabled) continue;

        X->Update({52,26});
        X->Draw(Surface.get(), {(f32)Size.W -51,0});
      }

      Surface->Set_Color(Monet.MainDark._A(0.9));
      Surface->Draw_Arc({(f32)Size.W -13, 13}, 9, 0, M_PI*2);
      Surface->Fill();

      Surface->Clip_Reset();
    }


    Surface->Set_FontSize(15);

    size_f32 TSize = Surface->Calc_Text(Text.c_str());

    Surface->Set_Color(Monet.Text);
    Surface->Set_Pos({(Size.H -TSize.H)/2, (Size.H -TSize.H)/2});
    Surface->Draw_Text(Text.c_str());
    Surface->Fill();
  }

  void toggle::Handler_StateChanged(controlStateSet State)
  {
    control::Handler_StateChanged(State);

    DyeToRoot();
    CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
  }

  void toggle::Do_Click()
  {
    Do_Changed(!Checked);

    control::Do_Click();
  }

  void toggle::Do_Changed(bool Status)
  {
    Checked = Status;

    if (OnChanged != Nil)
      OnChanged(this, Status);


    DyeToRoot();
    CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
  }

  bool toggle::LoadProp(string Name, const jconf::Value& Prop)
  {
    if (control::LoadProp(Name, Prop))
      return true;


    if (Name == "Text")
    {
      if (!Prop.isString())
        return false;

      Text = (string)Prop;
      return true;
    }

    ef (Name == "Checked")
    {
      if (!Prop.isBool())
        return false;
      
      Checked = (bool)Prop;

      
      return true;
    }

    else
      return false;
  }

  bool toggle::LoadFunc(string Name, point Func)
  {
    if (control::LoadFunc(Name, Func))
      return true;


    #define makro(X)  if (Name == #X) { X = (decltype(X))Func; return true; }

    makro(OnChanged)

    #undef makro

    return false;
  }

  #pragma endregion


  #pragma region progbar

  progbar::progbar()
    : control()
  {}

  progbar::~progbar()
  {}


  void progbar::Draw()
  {
    Surface->Set_LineSize(2);

    Surface->Set_Color(color(1,1,1,1));
    Surface->Draw_RectRound({1, 2, (f32)Size.W -1, (f32)Size.H -2}, 14);
    Surface->Clip();

    
    Surface->Set_Color(Monet.Gray._A(0.5));
    Surface->Draw_Rect({1, 2, (f32)Size.W -1, (f32)Size.H -2});
    Surface->Fill();


    Surface->Set_Color(color(1,1,1,1));
    Surface->Draw_RectRound({1, 2, ((f32)Size.W /Max *Value), (f32)Size.H -2}, 14);
    Surface->Clip();

    Surface->Set_Color(Monet.Main);
    Surface->Draw_Rect({1, 2, ((f32)Size.W /Max *Value), (f32)Size.H -2});
    Surface->Fill();

    for (auto &X: Effects)
    {
      if (!X->Enabled) continue;

      X->Update({i32((f32)Size.W /Max *Value), Size.H});
      X->Draw(Surface.get(), {0,0});
    }

    Surface->Clip_Reset();



    Surface->Set_FontSize(15);

    size_f32 TSize = Surface->Calc_Text(Text.c_str());

    Surface->Set_Color(Monet.Text);
    Surface->Set_Pos({((Size.W -TSize.W)/2), ((Size.H -TSize.H)/2)});
    Surface->Draw_Text(Text.c_str());
    Surface->Fill();

    

    Surface->Set_Color(color(1,1,1,1));
    Surface->Draw_RectRound({1, 2, ((f32)Size.W /Max *Value), (f32)Size.H -2}, 14);
    Surface->Clip();

    Surface->Set_Color(Monet.TextDark);
    Surface->Set_Pos({((Size.W -TSize.W)/2), ((Size.H -TSize.H)/2)});
    Surface->Draw_Text(Text.c_str());
    Surface->Fill();

    Surface->Clip_Reset();
  }

  bool progbar::LoadProp(string Name, const jconf::Value& Prop)
  {
    if (control::LoadProp(Name, Prop))
      return true;


    if (Name == "Text")
    {
      if (!Prop.isString())
        return false;

      Text = (string)Prop;

      return true;
    }

    ef (Name == "Max")
    {
      if (!Prop.isInt())
        return false;

      Max = (i64)Prop;

      return true;
    }

    ef (Name == "Value")
    {
      if (!Prop.isInt())
        return false;

      Value = (i64)Prop;

      return true;
    }

    else
      return false;
  }

  #pragma endregion


  #pragma region slider

  slider::slider()
    : control()
  {}

  slider::~slider()
  {}


  void slider::Draw()
  {
    Surface->Set_LineSize(2);

    if ((ControlState & (csHover)) != 0)
    {
      Surface->Set_Color(color(1,1,1,1));
      Surface->Draw_RectRound({1, 4, ((f32)(Size.W-4) /Max *Value)+2-7, (f32)Size.H -4}, 14, 14, 4, 4);
      Surface->Clip();

      Surface->Set_Color(Monet.Main);
      Surface->Draw_Rect({1, 4, ((f32)(Size.W-4) /Max *Value)+2-7, (f32)Size.H -4});
      Surface->Fill();

      for (auto &X: Effects)
      {
        if (!X->Enabled) continue;

        X->Update({Size.W, Size.H-8});
        X->Draw(Surface.get(), {0,4});
      }

      Surface->Clip_Reset();


      Surface->Set_Color(Monet.MainLight);
      Surface->Draw_RectRound({((f32)(Size.W-4) /Max *Value)+2-3, 1, ((f32)(Size.W-4) /Max *Value)+2+3, (f32)Size.H -1}, 4);
      Surface->Fill();

      Surface->Set_Color(Monet.Gray._A(0.5));
      Surface->Draw_RectRound({((f32)(Size.W-4) /Max *Value)+2+7, 4, (f32)Size.W -1, (f32)Size.H -4}, 4, 4, 14, 14);
      Surface->Fill();
    }
      
    else
    {
      /*
      Surface->Set_Color(color(0.45, 0.30, 0.80));
      Surface->Draw_RectRound({1, ((f32)Size.H /2) -2, ((f32)(Size.W-14) /Max *Value)+7-10, ((f32)Size.H /2) +2}, 14);
      Surface->Fill();

      Surface->Set_Color(color(0.45, 0.30, 0.80));
      Surface->Draw_Arc({((f32)(Size.W-14) /Max *Value)+7, (f32)Size.H /2}, 7, 0, M_PI *2);
      Surface->Fill();

      Surface->Set_Color(color(0.28, 0.28, 0.28));
      Surface->Draw_RectRound({((f32)(Size.W-14) /Max *Value)+7+10, ((f32)Size.H /2) -2, (f32)Size.W -1, ((f32)Size.H /2) +2}, 14);
      Surface->Fill();
      */

      Surface->Set_Color(color(1,1,1,1));
      Surface->Draw_RectRound({1, 4, ((f32)(Size.W-4) /Max *Value)+2-6, (f32)Size.H -4}, 14, 14, 4, 4);
      Surface->Clip();

      Surface->Set_Color(Monet.Main);
      Surface->Draw_Rect({1, 4, ((f32)(Size.W-4) /Max *Value)+2-6, (f32)Size.H -4});
      Surface->Fill();

      for (auto &X: Effects)
      {
        if (!X->Enabled) continue;

        X->Update({Size.W, Size.H-8});
        X->Draw(Surface.get(), {0,4});
      }

      Surface->Clip_Reset();


      Surface->Set_Color(Monet.MainLight);
      Surface->Draw_RectRound({((f32)(Size.W-4) /Max *Value)+2-2, 1, ((f32)(Size.W-4) /Max *Value)+2+2, (f32)Size.H -1}, 4);
      Surface->Fill();

      Surface->Set_Color(Monet.Gray._A(0.5));
      Surface->Draw_RectRound({((f32)(Size.W-4) /Max *Value)+2+6, 4, (f32)Size.W -1, (f32)Size.H -4}, 4, 4, 14, 14);
      Surface->Fill();
    }
  }

  void slider::Handler_StateChanged(controlStateSet State)
  {
    control::Handler_StateChanged(State);

    DyeToRoot();
    CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
  }

  void slider::Do_MouseDown (poit_i32 Pos, shiftStateSet Button, shiftStateSet State)
  {
    if (Button == shiftStates::ssLeft)
    {
      i32 Step = min<i32>(Size.W, max<i32>(0, Pos.X));
      
      Do_Changed((f32)Step /(f32)Size.W *Max);
    }

    control::Do_MouseDown(Pos, Button, State);
  }

  void slider::Do_MouseMove(poit_i32 Pos, shiftStateSet State)
  {
    if ((State & shiftStates::ssLeft) != 0)
    {
      i32 Step = min<i32>(Size.W, max<i32>(0, Pos.X));

      Do_Changed((f32)Step /(f32)Size.W *Max);
    }

    control::Do_MouseMove(Pos, State);
  }

  void slider::Do_Changed(u32 nValue)
  {
    Value = nValue;

    if (OnChanged != Nil)
      OnChanged(this, nValue);


    DyeToRoot();
    CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
  }

  bool slider::LoadProp(string Name, const jconf::Value& Prop)
  {
    if (control::LoadProp(Name, Prop))
      return true;


    if (Name == "Max")
    {
      if (!Prop.isInt())
        return false;

      Max = (i64)Prop;

      return true;
    }

    ef (Name == "Value")
    {
      if (!Prop.isInt())
        return false;


      Value = (i64)Prop;

      return true;
    }

    else
      return false;
  }

  bool slider::LoadFunc(string Name, point Func)
  {
    if (control::LoadFunc(Name, Func))
      return true;


    #define makro(X)  if (Name == #X) { X = (decltype(X))Func; return true; }

    makro(OnChanged)

    #undef makro

    return false;
  }

  #pragma endregion


  #pragma region tabs

  tabs::tabs()
    : control()
  {}

  tabs::~tabs()
  {}


  void tabs::Draw()
  {
    Surface->Set_LineSize(2);
    Surface->Set_FontSize(14);


    Surface->Set_Color(color(1,1,1,1));
    Surface->Draw_RectRound({0,0, (f32)Size.W, (f32)Size.H}, 18);
    Surface->Clip();


    Surface->Set_Color(Monet.Gray._A(0.4));
    Surface->Draw_RectRound({0,0, (f32)Size.W, (f32)Size.H}, 18);
    Surface->Fill();


    u32 XOff = 0;

    for (u32 i = 0; i < Tabs.size(); i++)
    {
      auto &X = Tabs[i];

      XOff += 16;


      if (TabID == i)
      {
        Surface->Sets_Push();

        Surface->Set_Color(color(1,1,1,1));
        Surface->Draw_RectRound({(f32)XOff -6, (Size.H/2) -(OPT_Tabs[i].H/2) -6, XOff +OPT_Tabs[i].W +6, (Size.H/2) +(OPT_Tabs[i].H/2) +6}, 18);
        Surface->Clip();

        Surface->Set_Color(Monet.Main);
        Surface->Draw_Rect({(f32)XOff -6, (Size.H/2) -(OPT_Tabs[i].H/2) -6, XOff +OPT_Tabs[i].W +6, (Size.H/2) +(OPT_Tabs[i].H/2) +6});
        Surface->Fill();

        Surface->Set_Color(Monet.TextDark);
        Surface->Set_Pos({(f32)XOff, (Size.H -OPT_Tabs[i].H)/2});
        Surface->Draw_Text(X.c_str());
        Surface->Fill();

        for (auto &X: Effects)
        {
          if (!X->Enabled) continue;

          X->Update({(i32)OPT_Tabs[i].W +12, (i32)OPT_Tabs[i].H +12});
          X->Draw(Surface.get(), {(f32)XOff -6, (Size.H/2) -(OPT_Tabs[i].H/2) -6});
        }

        Surface->Sets_Pop();
      }

      el {
        Surface->Set_Color(Monet.Text);
        Surface->Set_Pos({(f32)XOff, (Size.H -OPT_Tabs[i].H)/2});
        Surface->Draw_Text(X.c_str());
        Surface->Fill();
      }

      
      if (HTabID == i)
      {
        Surface->Set_Color(Monet.GrayLight);
        Surface->Draw_RectRound({(f32)XOff -4, (f32)Size.H -4, XOff +OPT_Tabs[i].W +4, (f32)Size.H}, 6,0,6,0);
        Surface->Fill();
      }


      XOff += OPT_Tabs[i].W;
    }


    Surface->Clip_Reset();
  }

  void tabs::ReCalc_TabsSize()
  {
    Surface->Set_FontSize(14);


    OPT_Tabs.clear();
    OPT_Tabs.reserve(Tabs.size());

    for (u32 i = 0; i < Tabs.size(); i++)
      OPT_Tabs.push_back(Surface->Calc_Text(Tabs[i].c_str()));

  }

  void tabs::Do_MouseMove(poit_i32 Pos, shiftStateSet State)
  {
    u32 XOff = 0;

    i32 NHTabID = -1;

    for (u32 i = 0; i < OPT_Tabs.size(); i++)
    {
      XOff += 16;

      if (
        Pos.X > XOff -3 &&
        Pos.X < XOff +OPT_Tabs[i].W +3
      )
      {
        NHTabID = i;
        break;
      }

      XOff += OPT_Tabs[i].W;
    }


    if (NHTabID != HTabID)
    {
      HTabID = NHTabID;

      DyeToRoot();
      CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
    }


    control::Do_MouseMove(Pos, State);
  }

  void tabs::Do_ClickEx(poit_i32 Pos)
  {
    u32 XOff = 0;

    i32 NTabID = -1;

    for (u32 i = 0; i < OPT_Tabs.size(); i++)
    {
      XOff += 16;

      if (
        Pos.X > XOff -3 &&
        Pos.X < XOff +OPT_Tabs[i].W +3
      )
      {
        NTabID = i;
        break;
      }

      XOff += OPT_Tabs[i].W;
    }


    if (NTabID != -1 && NTabID != TabID)
    {
      TabID = NTabID;

      DyeToRoot();
      CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
    }


    control::Do_ClickEx(Pos);
  }

  void tabs::Do_Changed(i32 nTabID)
  {
    TabID = nTabID;

    if (OnChanged != Nil)
      OnChanged(this, nTabID);


    DyeToRoot();
    CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
  }

  bool tabs::LoadProp(string Name, const jconf::Value& Prop)
  {
    if (control::LoadProp(Name, Prop))
      return true;


    if (Name == "Tabs")
    {
      vector<string> Nat;

      if (!Prop.isArray())
        return false;

      
      for (u32 i = 0; i < Prop.size(); i++)
      {
        auto It = Prop[i];

        if (!It.isString())
          return false;

        Nat.push_back((string)It);
      }
      

      Tabs = (Nat);
      ReCalc_TabsSize();
      return true;
    }

    ef (Name == "TabID")
    {
      if (!Prop.isInt())
        return false;

      
      TabID = (i64)Prop;

      return true;
    }

    else
      return false;
  }

  bool tabs::LoadFunc(string Name, point Func)
  {
    if (control::LoadFunc(Name, Func))
      return true;


    #define makro(X)  if (Name == #X) { X = (decltype(X))Func; return true; }

    makro(OnChanged)

    #undef makro

    return false;
  }

  #pragma endregion


  #pragma region choice

  choice::choice()
    : tabs()
  {}

  choice::~choice()
  {}


  void choice::Draw()
  {
    Surface->Set_LineSize(2);
    Surface->Set_FontSize(14);


    Surface->Set_Color(Monet.Gray._A(0.4));
    Surface->Draw_RectRound({0, (f32)Threshold, (f32)Size.W, (f32)Size.H -Threshold}, 18);
    Surface->Fill();


    i32 XOff = -10;

    for (u32 i = 0; i < Tabs.size(); i++)
    {
      auto &X = Tabs[i];

      XOff += 16;


      if (TabID == i)
      {
        Surface->Set_Color(color(1,1,1,1));
        Surface->Draw_RectRound({(f32)XOff -6, 0, XOff +OPT_Tabs[i].W +6, (f32)Size.H}, 18);
        Surface->Clip();

        Surface->Set_Color(Monet.Main);
        Surface->Draw_Rect({(f32)XOff -6, 0, XOff +OPT_Tabs[i].W +6, (f32)Size.H});
        Surface->Fill();

        Surface->Set_Color(Monet.TextDark);
        Surface->Set_Pos({(f32)XOff, (Size.H -OPT_Tabs[i].H)/2});
        Surface->Draw_Text(X.c_str());
        Surface->Fill();

        for (auto &X: Effects)
        {
          if (!X->Enabled) continue;

          X->Update({i32(OPT_Tabs[i].W +12), Size.H});
          X->Draw(Surface.get(), {(f32)XOff-6, 0});
        }

        Surface->Clip_Reset();
      }

      el {
        Surface->Set_Color(Monet.Text);
        Surface->Set_Pos({(f32)XOff, (Size.H -OPT_Tabs[i].H)/2});
        Surface->Draw_Text(X.c_str());
        Surface->Fill();
      }

      
      if (HTabID == i)
      {
        //Surface->Set_Color(Monet.GrayLight);
        //Surface->Draw_RectRound({(f32)XOff -4, (f32)Size.H -4, XOff +OPT_Tabs[i].W +4, (f32)Size.H}, 6,0,6,0);
        //Surface->Fill();
      }


      XOff += OPT_Tabs[i].W;
    }

  }

  void choice::Do_MouseMove(poit_i32 Pos, shiftStateSet State)
  {
    i32 XOff = -10;

    i32 NHTabID = -1;

    for (u32 i = 0; i < OPT_Tabs.size(); i++)
    {
      XOff += 16;

      if (
        Pos.X > XOff -3 &&
        Pos.X < XOff +OPT_Tabs[i].W +3
      )
      {
        NHTabID = i;
        break;
      }

      XOff += OPT_Tabs[i].W;
    }


    if (NHTabID != HTabID)
    {
      HTabID = NHTabID;

      DyeToRoot();
      CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
    }


    control::Do_MouseMove(Pos, State);
  }

  void choice::Do_ClickEx(poit_i32 Pos)
  {
    i32 XOff = -10;

    i32 NTabID = -1;

    for (u32 i = 0; i < OPT_Tabs.size(); i++)
    {
      XOff += 16;

      if (
        Pos.X > XOff -3 &&
        Pos.X < XOff +OPT_Tabs[i].W +3
      )
      {
        NTabID = i;
        break;
      }

      XOff += OPT_Tabs[i].W;
    }


    if (NTabID != -1 && NTabID != TabID)
    {
      TabID = NTabID;

      DyeToRoot();
      CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
    }


    control::Do_ClickEx(Pos);
  }

  bool choice::LoadProp(string Name, const jconf::Value& Prop)
  {
    if (tabs::LoadProp(Name, Prop))
      return true;


    if (Name == "Threshold")
    {
      if (!Prop.isInt())
        return false;
        
      Threshold = (i64)Prop;
      
      return true;
    }

    else
      return false;
  }

  #pragma endregion



  #pragma region popup

  popup::popup()
    : qcl::popup()
  {}

  popup::~popup()
  {}


  void popup::Draw()
  {
    Surface->Set_FontSize(14);

    Surface->Set_Color(Monet.GrayDark);
    Surface->Draw_RectRound({0,0, (f32)Size.W, (f32)Size.H}, 13);
    Surface->Fill();


    const i32 k = 26;
    u32 s = 0;

    if (VItems != Items.get())
    {
      Surface->Set_Color(color(1,1,1,1));

      Surface->Set_Pos    ({3 +12 +4, (f32)(s)+3 +12 -4});
      Surface->Draw_ToLine({3 +12,    (f32)(s)+3 +12});
      Surface->Draw_ToLine({3 +12 +4, (f32)(s)+3 +12 +4});
      Surface->Stroke();


      auto TSize = Surface->Calc_Text(VItems->Name.c_str());

      Surface->Set_Pos({((f32)Size.W -TSize.W)/2, (s) +((30 -TSize.H)/2)});

      Surface->Draw_Text(VItems->Name.c_str());
      Surface->Fill();

      s += 30;
    }


    for (u32 i = 0; i < VItems->Items.size(); i++)
    {
      if (VItems->Items[i]->Name == "-")
      {
        Surface->Set_Color(Monet.Gray);
        Surface->Draw_Line(
          {0, (f32)(s)+6},
          {(f32)Size.W,    (f32)(s)+6}
        );
        Surface->Stroke();

        s += 8;
        continue;
      }


      if (i == HSel)
      {
        Surface->Set_Color(color(1,1,1,1));
        Surface->Draw_RectRound({3, (f32)(s)+3, (f32)Size.W-3, (f32)(s) -3 +30}, 30);
        Surface->Clip();
        
        Surface->Set_Color(Monet.Main);
        Surface->Draw_Rect({3, (f32)(s)+3, (f32)Size.W-3, (f32)(s) -3 +30});
        Surface->Fill();

        for (auto &X: Effects)
        {
          if (!X->Enabled) continue;

          X->Update({Size.W-6, k});
          X->Draw(Surface.get(), {3, (f32)(s)+3});
        }

        Surface->Clip_Reset();
      }

      auto TSize = Surface->Calc_Text(VItems->Items[i]->Name.c_str());

      Surface->Set_Pos({((30 -TSize.H)/2) +1, (s) +((30 -TSize.H)/2)});

      if (i == HSel)
      {
        Surface->Set_Color(Monet.TextDark);
        Surface->Set_rPos({1,0});
      }
      el {
        Surface->Set_Color(Monet.Text);
      }

      Surface->Draw_Text(VItems->Items[i]->Name.c_str());
      Surface->Fill();

      if (!VItems->Items[i]->Items.empty())
      {
        Surface->Set_Pos    ({(f32)Size.W -3 -12 -4, (f32)(s)+3 +12 -4});
        Surface->Draw_ToLine({(f32)Size.W -3 -12,    (f32)(s)+3 +12});
        Surface->Draw_ToLine({(f32)Size.W -3 -12 -4, (f32)(s)+3 +12 +4});
        Surface->Stroke();
      }

      s += k;
    }


    if (s+4 != Size.H)
    {
      pair<u32, popup*> *Data = new pair<u32, popup*>;

      Data->first = s+4;
      Data->second = this;


      CurrentApp->PushTask(
        [](u0 Data)
        {
          pair<u32, popup*> *Pair = (pair<u32, popup*>*)Data;

          Pair->second->Size = {Pair->second->Size.W, (i32)Pair->first};
          Pair->second->Flag_Add(DirtyResize | DirtyRebound);

          CurrentApp->PushMessage(Pair->second->GetRoot(), controlMessages::cmPaint);

          delete Pair;
        },
        (u0)Data
      );
    }

  }

  void popup::Do_MouseUp(poit_i32 Pos, shiftStateSet Button, shiftStateSet State)
  {
    const i32 k = 26;
    u32 s = 3;
    i32 NHSel = 0;

    if (VItems != Items.get())
    {
      s += 30;
      
      if (Pos.Y <= s)
      {
        NHSel = -2;
        goto _l_Sec;
      }
    }


    for (u32 i = 0; i < VItems->Items.size(); i++)
    {
      if (VItems->Items[i]->Name == "-")
        s += 8;
      else
        s += k;

      if (s >= Pos.Y)
        break;

      NHSel += 1;
    }


    _l_Sec:
    if (NHSel == -2)
    {
      if (VItems->Items[0]->Parent == Nil)
        return;

      VItems = VItems->Items[0]->Parent;
      Flag_Add(DirtyDraw);

      Do_MouseMove(Pos, 0);

      CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
      return;
    }

    if (Pos.X < 0 || Pos.X > Size.W)
      goto _l_Close;

    if (VItems->Items[NHSel]->Name == "-")
      return;



    if (VItems->Items[NHSel]->Items.empty())
    {
      if (VItems->Items[NHSel]->OnClick != Nil)
        VItems->Items[NHSel]->OnClick(this);

      goto _l_Close;
    }
    else
    {
      VItems = VItems->Items[NHSel].get();
      Flag_Add(DirtyDraw);

      Do_MouseMove(Pos, 0);

      CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
    }


    return;

    _l_Close:
    if (auto C = dynamic_cast<window*>(GetRoot()); C != Nil)
    if (C->Overlay.get() == this)
      C->Overlay = Nil;
  }

  void popup::Do_MouseMove(poit_i32 Pos, shiftStateSet State)
  {
    const i32 k = 26;
    u32 s = 3;
    i32 NHSel = 0;

    if (VItems != Items.get())
    {
      s += 30;
      
      if (Pos.Y <= s)
      {
        NHSel = -2;
        goto _l_Sec;
      }
    }

    for (u32 i = 0; i < VItems->Items.size(); i++)
    {
      if (VItems->Items[i]->Name == "-")
        s += 8;
      else
        s += k;

      if (s >= Pos.Y)
        break;

      NHSel += 1;
    }
    

    _l_Sec:
    if (NHSel != HSel)
    {
      HSel = NHSel;

      DyeToRoot();
      CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
    }

    control::Do_MouseMove(Pos, State);
  }


  void popup::Handler_StateChanged(controlStateSet State)
  {
    control::Handler_StateChanged(State);

    if ((State & controlStates::csHover) == 0)
      HSel = -1;


    DyeToRoot();
    CurrentApp->PushMessage(GetRoot(), controlMessages::cmPaint);
  }

  #pragma endregion



  #pragma region chip

  chip::chip()
    : view()
    , Color(Monet.MainLight)
  {}

  chip::~chip()
  {}


  void chip::Draw()
  {
    Surface->Set_LineSize(2);
    
    Surface->Set_Color(Color);
    
    if (BorderRadius < 0)
      Surface->Draw_RectRound({1,1, (f32)Size.W-1, (f32)Size.H-1}, (Size.W > Size.H ? Size.W:Size.H));
    el
      Surface->Draw_RectRound({1,1, (f32)Size.W-1, (f32)Size.H-1}, BorderRadius);

    Surface->Stroke();
  }

  bool chip::LoadProp(string Name, const jconf::Value& Prop)
  {
    if (view::LoadProp(Name, Prop))
      return true;


    if (Name == "Color")
    {
      if (!Prop.isString())
        return false;

      string Cac = (string)Prop;


      Color = ParseColor(Cac);
      if (Color == color(0,0,0,0))
        return false;

      return true;
    }

    ef (Name == "BorderRadius")
    {
      if (!Prop.isInt())
        return false;

      BorderRadius = (i64)Prop;

      return true;
    }

    else
      return false;
  }

  #pragma endregion


  #pragma region card

  card::card()
    : view()
    , Color(Monet.Main)
  {}

  card::~card()
  {}


  void card::Draw()
  {
    Surface->Set_Color(color(1,1,1,1));

    if (BorderRadius < 0)
      Surface->Draw_RectRound({1,1, (f32)Size.W-1, (f32)Size.H-1}, (Size.W > Size.H ? Size.W:Size.H));
    el
      Surface->Draw_RectRound({1,1, (f32)Size.W-1, (f32)Size.H-1}, BorderRadius);

    Surface->Clip();


    Surface->Set_Color(Color);
    Surface->Draw_Rect({0,0, (f32)Size.W, (f32)Size.H});
    Surface->Fill();

    for (auto &X: Effects)
    {
      if (!X->Enabled) continue;
      
      X->Update({Size.W, Size.H});
      X->Draw(Surface.get(), {0,0});
    }

    Surface->Clip_Reset();
  }

  bool card::LoadProp(string Name, const jconf::Value& Prop)
  {
    if (view::LoadProp(Name, Prop))
      return true;


    if (Name == "Color")
    {
      if (!Prop.isString())
        return false;

      string Cac = (string)Prop;

      Color = ParseColor(Cac);
      if (Color == color(0,0,0,0))
        return false;

      return true;
    }

    ef (Name == "BorderRadius")
    {
      if (!Prop.isInt())
        return false;

      BorderRadius = (i64)Prop;

      return true;
    }

    else
      return false;
  }

  #pragma endregion




  void Register()
  {
    #define Reg(X)  dyn::Register(#X, []() -> control* { return new X(); })
    #define RegE(X) dyn::Register(#X, []() -> effect*  { return new X(); })

    Reg(qstd::form);
    Reg(qstd::layout);
    Reg(qstd::layout_vert);
    Reg(qstd::layout_horz);
    Reg(qstd::layout_flow);
    Reg(qstd::text);
    Reg(qstd::icon);
    Reg(qstd::edit);
    Reg(qstd::radio);
    Reg(qstd::check);
    Reg(qstd::toggle);
    Reg(qstd::progbar);
    Reg(qstd::slider);
    Reg(qstd::tabs);
    Reg(qstd::choice);

    Reg(qstd::popup);

    Reg(qstd::chip);
    Reg(qstd::card);

    RegE(qstd::noise);

    #undef Reg
  }

}
