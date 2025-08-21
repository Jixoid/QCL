/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include <unordered_map>

#include "Basis.h"

#include "JConf.h"

#include "qcl/Control.hh"
#include "qcl/View.hh"
#include "qcl/DynamicLoad.hh"

using namespace std;
using namespace jix;



namespace jix::qcl::dyn
{
  unordered_map<string, control* (*)()> RegList;

  void Register(const char* Name, control* (*Maker)())
  {
    //cout << "QCL.reg: " << Name << endl;

    RegList[Name] = Maker;
  }



  vector<control*> LoadQDL_Arr(jc_arr Arr, string Scope);

  control* LoadQDL_Stc(jc_stc Stc, string Scope, u32 Index)
  {
    jc_obj Cac;


    #pragma region Name

      Cac = jc_StcGet(Stc, "&name");
      if (!jc_IsVal(Cac))
        throw runtime_error("Wrong type, value was expected: " +Scope+ "["+ std::to_string(Index) +"] -> &name");

      string Name;
      {
        char *CStr = jc_ValGet(Cac);

        Name = string(CStr);

        jc_DisStr(CStr);
      }
      
    #pragma endregion


    #pragma region  Type

      Cac = jc_StcGet(Stc, "&type");
      if (!jc_IsVal(Cac))
        throw runtime_error("Wrong type, value was expected: " +Scope+ "/"+ Name +" -> &type");

      string Type;
      {
        char *CStr = jc_ValGet(Cac);

        Type = string(CStr);

        jc_DisStr(CStr);
      }
      
    #pragma endregion


    // Create New
    auto it = RegList.find(Type);
    if (it == RegList.end())
      throw runtime_error("Species name not found: " +Scope+ "/"+ Name +" -> &type = \""+Type+"\"");


    control *Self = it->second();

    Self->QDL = Stc;
    Self->Name = Name;


    #pragma region  Style

      Cac = jc_StcGet(Stc, "&style");
      if (Cac == Nil)
      {
        // if (not found) then continue
      }
      else if (jc_IsVal(Cac))
      {
        string Style;
        {
          char *CStr = jc_ValGet(Cac);

          Style = string(CStr);

          jc_DisStr(CStr);
        }

        Self->Style.push_back(Style);
      }
      else if (jc_IsArr(Cac))
      {
        for (u32 i = 0; i < jc_ArrC(Cac); i++)
        {
          jc_val Obj = jc_ArrGet(Cac, i);
          if (!jc_IsVal(Obj))
            throw runtime_error("Wrong type, value was expected: " +Scope+ "/"+ Name +" -> &style["+to_string(i)+"]");


          string Style;
          {
            char *CStr = jc_ValGet(Obj);

            Style = string(CStr);

            jc_DisStr(CStr);
          }

          Self->Style.push_back(Style);
        }
      }
      else
        throw runtime_error("Wrong type, value or array was expected: " +Scope+ "/"+ Name +" -> &style");
      
    #pragma endregion


    // Childs
    Cac = jc_StcGet(Stc, "&subs");
    if (Cac != Nil)
    {
      if (!jc_IsArr(Cac))
        throw runtime_error("Wrong type, array was expected: " +Scope+ "/"+ Name +" -> &subs");
      
      else
      {
        if (dynamic_cast<qcl::view*>(Self) == Nil)
          throw runtime_error("This component is not of type qcl::view: " +Scope+ "/"+ Name);


        auto Childs = LoadQDL_Arr(Cac, Scope+"/"+Name);

        for (auto &X: Childs)
          dynamic_cast<qcl::view*>(Self)->Child_Add(shared_ptr<qcl::control>(X));

      }
    }


    return Self;
  }

  vector<control*> LoadQDL_Arr(jc_arr Arr, string Scope)
  {
    vector <control*> Ret;

    for (u32 i = 0; i < jc_ArrC(Arr); i++)
    {
      jc_stc Obj = jc_ArrGet(Arr, i);
      if (Obj == Nil || !jc_IsStc(Obj))
        throw runtime_error("Wrong type, struct was expected: " +Scope +"["+ std::to_string(i) +"]");

      
      Ret.push_back(LoadQDL_Stc(Obj, Scope, i));
    }

    return Ret;
  }



  void LoadQDL_Prop(control* Ctrl, string Scope, jc_stc Styles, unordered_map<string, point> &FuncMap)
  {
    // Set Styles
    for (u32 s = 0; s < Ctrl->Style.size(); s++)
    {
      jc_stc Style = jc_StcGet(Styles, Ctrl->Style[s].c_str());
      if (Style == Nil)
        throw runtime_error("struct was expected: root/&styles -> " +Ctrl->Style[s]);

      else if (!jc_IsStc(Style))
        throw runtime_error("Wrong type, struct was expected: root/&styles -> " +Ctrl->Style[s]);


      for (u32 i = 0; i < jc_StcC(Style); i++)
      {
        char *CStr;
        string Prop_Name;

        jc_obj Prop = jc_StcInd(Style, i, &CStr);
        Prop_Name = string(CStr);
        jc_DisStr(CStr);

        if (Prop_Name[0] == '&')
          continue;


        
        if (Prop_Name[0] == '^')
        {
          char *CStr;
          string Prop_Func;

          CStr = jc_ValGet(Prop);
          Prop_Func = string(CStr);
          jc_DisStr(CStr);


          auto it = FuncMap.find(Prop_Func);

          if (it == FuncMap.end())
            throw runtime_error("Func not found in code: " +Scope+"/"+Ctrl->Name+"("+Ctrl->Style[s]+")"+" -> "+Prop_Name +" = "+ Prop_Func);


          if (!Ctrl->LoadFunc(Prop_Name.substr(1), it->second))
            throw runtime_error("Invalid prop: " +Scope+"/"+Ctrl->Name+"("+Ctrl->Style[s]+")"+" -> "+Prop_Name);
        }

        else
        {
          if (!Ctrl->LoadProp(Prop_Name, Prop))
            throw runtime_error("Invalid prop: " +Scope+"/"+Ctrl->Name+"("+Ctrl->Style[s]+")"+" -> "+Prop_Name);
        }
      }

    }


    // Set Prop
    for (u32 i = 0; i < jc_StcC(Ctrl->QDL); i++)
    {
      char *CStr;
      string Prop_Name;

      jc_obj Prop = jc_StcInd(Ctrl->QDL, i, &CStr);
      Prop_Name = string(CStr);
      jc_DisStr(CStr);

      if (Prop_Name[0] == '&')
        continue;


      
      if (Prop_Name[0] == '^')
      {
        char *CStr;
        string Prop_Func;

        CStr = jc_ValGet(Prop);
        Prop_Func = string(CStr);
        jc_DisStr(CStr);


        auto it = FuncMap.find(Prop_Func);

        if (it == FuncMap.end())
          throw runtime_error("Func not found in code: " +Scope+"/"+Ctrl->Name+" -> "+Prop_Name +" = "+ Prop_Func);


        if (!Ctrl->LoadFunc(Prop_Name.substr(1), it->second))
          throw runtime_error("Invalid prop: " +Scope+"/"+Ctrl->Name+" -> "+Prop_Name);
      }

      else
      {
        if (!Ctrl->LoadProp(Prop_Name, Prop))
          throw runtime_error("Invalid prop: " +Scope+"/"+Ctrl->Name+" -> "+Prop_Name);
      }
    }


    if (auto C = dynamic_cast<qcl::view*>(Ctrl); C != Nil)
      for (auto &X: C->Childs)
        LoadQDL_Prop(X.get(), Scope+"/"+Ctrl->Name, Styles, FuncMap);
  }


  vector<control*> Load_formFile(string FPath, unordered_map<string, point> FuncMap)
  {
    // Open File
    jc_stc DL = jc_Parse(FPath.c_str());
   
    if (DL == Nil)
      throw runtime_error("Corrupt qdl file: " +FPath);


    jc_WriteBin((FPath+'b').c_str(), DL);


    // Load Comps
    jc_obj Comps = jc_StcGet(DL, "&subs");
    if (!jc_IsArr(Comps))
      throw runtime_error("array was expected: root -> &subs: " +FPath);
    
    auto Ret = LoadQDL_Arr(Comps, "root");


    // Load Design
    jc_obj Styles = jc_StcGet(DL, "&styles");
    if (!jc_IsStc(Styles))
      throw runtime_error("struct was expected: root -> &styles: " +FPath);

    for (auto &X: Ret)
      LoadQDL_Prop(X, "root", Styles, FuncMap);

    return Ret;
  }

}
