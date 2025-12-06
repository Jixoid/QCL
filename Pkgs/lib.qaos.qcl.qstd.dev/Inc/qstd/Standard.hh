/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include <memory>
#include <vector>

#include "Basis.h"

#include "qcl/Control.hh"
#include "qcl/Graphic.hh"
#include "qcl/Window.hh"
#include "qcl/Popup.hh"
#include "qcl/Effect.hh"

using namespace std;
using namespace jix;
using namespace qcl;



namespace qstd
{
  void Register();

  class monet
  {
    public:
      monet();

    public:
      color Main;
      color MainDark;
      color MainDarkS;
      color MainLight;
      color Text;
      color TextDis;
      color TextDark;
      color TextHighlight;
      color Gray;
      color GrayDark;
      color GrayLight;
      color Back;
      color BackDark;

    public:
      void Update(color Color, bool Dark = true);
  };

  extern monet Monet;



  class noise: public effect
  {
    public:
      noise();
      ~noise() override;

    public:
      const u32 WIDTH  = 512;
      const u32 HEIGHT = 512;

    public:
      u8 VNoise = 20;

    public:
      vector<u32> *Data = Nil;
      surface *Noise = Nil;

      surface *From = Nil;

      size_i32 LastSize = {0,0};

    public:
      void Reset() override;
      void Update(size_i32 Size) override;
      void Draw(surface *Surface, poit_f32 Pos) override;

      bool LoadProp(string Name, const jconf::Value& Prop) override;
  };



  class form: public qcl::window
  {
    public:
      form();
      ~form() override;

      
    public:
      void Draw() override;

  };

  class layout: public qcl::view
  {
    public:
      layout();
      ~layout() override;

    public:
      color Color;
      u16 BorderRadius = 12;

    public:
      void Draw() override;

      void Draw_ScrollVert() override;
      void Draw_ScrollHorz() override;

      bool LoadProp(string Name, const jconf::Value& Prop) override;
  };

  class layout_vert: public layout
  {
    public:
      layout_vert();
      ~layout_vert() override;


    public:
      void Do_Tiling() override;

      size_i32 CalcAutoSize() override;
  };

  class layout_horz: public layout
  {
    public:
      layout_horz();
      ~layout_horz() override;


    public:
      void Do_Tiling() override;

      size_i32 CalcAutoSize() override;
  };

  class layout_flow: public layout
  {
    public:
      layout_flow();
      ~layout_flow() override;


    public:
      void Do_Tiling() override;

      size_i32 CalcAutoSize() override;
  };


  enum text_align: u8
  {
    taLeft = 1,
    taTop  = 1,

    taCenter = 2,

    taRight  = 3,
    taBottom = 3,
  };

  class text: public qcl::control
  {
    public:
      text();
      ~text() override;


    public:
      string Text = "text";

      color Color;

      u16 FontSize = 14;

      text_align AlignVert = taCenter;
      text_align AlignHorz = taCenter;

    public:
      void Draw() override;

      size_i32 CalcAutoSize() override;

      bool LoadProp(string Name, const jconf::Value& Prop) override;
  };

  class icon: public qcl::control
  {
    public:
      icon();
      ~icon() override;


    public:
      shared_ptr<surface> Icon;

      string Path = "";


    public:
      void Draw() override;

      bool LoadIcon();

      bool LoadProp(string Name, const jconf::Value& Prop) override;
  };

  class edit: public qcl::control
  {
    public:
      edit();
      ~edit() override;


    public:
      string Text = "";
      string Title = "";
      u16 BorderRadius = 6;

      u0 Pos = 0;

    public:
      void Draw() override;

      void Handler_StateChanged(qcl::controlStateSet State) override;

      void Do_KeyDown(char *Key, u32 KeyCode, qcl::shiftStateSet State) override;

      bool LoadProp(string Name, const jconf::Value& Prop) override;
  };

  class radio: public qcl::control
  {
    public:
      radio();
      ~radio() override;


    public:
      string Text = "radio";

      bool Checked = false;

      void (*OnChanged) (qcl::control*, bool Status) = Nil;

    public:
      void Draw() override;

      void Handler_StateChanged(qcl::controlStateSet State) override;

      void Do_Click() override;
      virtual void Do_Changed(bool Status);

      bool LoadProp(string Name, const jconf::Value& Prop) override;
      bool LoadFunc(string Name, point Func) override;
  };

  class check: public qcl::control
  {
    public:
      check();
      ~check() override;


    public:
      string Text = "check";

      bool Checked = false;

      void (*OnChanged) (qcl::control*, bool Status) = Nil;

    public:
      void Draw() override;

      void Handler_StateChanged(qcl::controlStateSet State) override;

      void Do_Click() override;
      virtual void Do_Changed(bool Status);

      bool LoadProp(string Name, const jconf::Value& Prop) override;
      bool LoadFunc(string Name, point Func) override;
  };

  class toggle: public qcl::control
  {
    public:
      toggle();
      ~toggle() override;


    public:
      string Text = "toggle";

      bool Checked = false;

      void (*OnChanged) (qcl::control*, bool Status) = Nil;

    public:
      void Draw() override;

      void Handler_StateChanged(qcl::controlStateSet State) override;

      void Do_Click() override;
      virtual void Do_Changed(bool Status);

      bool LoadProp(string Name, const jconf::Value& Prop) override;
      bool LoadFunc(string Name, point Func) override;
  };

  class progbar: public qcl::control
  {
    public:
      progbar();
      ~progbar() override;


    public:
      string Text = "progbar";

      u32 Max = 100;
      u32 Value = 30;

    public:
      void Draw() override;

      bool LoadProp(string Name, const jconf::Value& Prop) override;
  };

  class slider: public qcl::control
  {
    public:
      slider();
      ~slider() override;


    public:
      u32 Max = 100;
      u32 Value = 20;

      void (*OnChanged) (qcl::control*, u32 Value) = Nil;

    public:
      void Draw() override;

      void Handler_StateChanged(qcl::controlStateSet State) override;
      void Do_MouseDown(poit_i32 Pos, shiftStateSet Button, shiftStateSet State) override;
      void Do_MouseMove(poit_i32 Pos, shiftStateSet State) override;
      virtual void Do_Changed(u32 Value);

      bool LoadProp(string Name, const jconf::Value& Prop) override;
      bool LoadFunc(string Name, point Func) override;
  };

  class tabs: public qcl::control
  {
    public:
      tabs();
      ~tabs() override;

    public:
      vector<string> Tabs;
      vector<size_f32> OPT_Tabs;

      i32 TabID = -1;
      i32 HTabID = -1;

      void (*OnChanged)(qcl::control*, i32 TabID) = Nil;

    public:
      void Draw() override;

      void ReCalc_TabsSize();

      void Do_MouseMove(poit_i32 Pos, shiftStateSet State) override;
      void Do_ClickEx(poit_i32 Pos) override;
      virtual void Do_Changed(i32 nTabID);

      bool LoadProp(string Name, const jconf::Value& Prop) override;
      bool LoadFunc(string Name, point Func) override;
  };

  class choice: public tabs
  {
    public:
      choice();
      ~choice() override;

    public:
      u32 Threshold = 0;

    public:
      void Draw() override;

      void Do_MouseMove(poit_i32 Pos, shiftStateSet State) override;
      void Do_ClickEx(poit_i32 Pos) override;

      bool LoadProp(string Name, const jconf::Value& Prop) override;
  };



  class popup: public qcl::popup
  {
    public:
      popup();
      ~popup() override;

    public:
      i32 HSel = -1;

    public:
      void Draw() override;

      void Do_MouseUp(poit_i32 Pos, shiftStateSet Button, shiftStateSet State) override;
      void Do_MouseMove(poit_i32 Pos, shiftStateSet State) override;
    
      void Handler_StateChanged(qcl::controlStateSet State) override;
  };



  class chip: public qcl::view
  {
    public:
      chip();
      ~chip() override;

    public:
      color Color;
      i32 BorderRadius = -1;

    public:
      void Draw() override;
      bool LoadProp(string Name, const jconf::Value& Prop) override;
  };

  class card: public qcl::view
  {
    public:
      card();
      ~card() override;

    public:
      color Color;
      i32 BorderRadius = -1;

    public:
      void Draw() override;
      bool LoadProp(string Name, const jconf::Value& Prop) override;
  };
  
}
