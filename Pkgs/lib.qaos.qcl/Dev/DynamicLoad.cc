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
#include <unordered_map>
#include <map>

#include "Basis.h"

#include "JConf.h"
#include "JConf.hh"

#include "qcl/Control.hh"
#include "qcl/View.hh"
#include "qcl/Effect.hh"
#include "qcl/Popup.hh"
#include "qcl/DynamicLoad.hh"

using namespace std;
using namespace jix;



namespace qcl::dyn
{
  map<string, control* (*)()> RegList;
  map<string, effect*  (*)()> EffList;

  void Register(const char* Name, control* (*Maker)())
  {
    RegList[Name] = Maker;
  }

  void Register(const char* Name, effect* (*Maker)())
  {
    EffList[Name] = Maker;
  }



  qdl LoadQDL_Arr(const jconf::Value& Arr, string Scope);

  control* LoadQDL_Stc(const jconf::Value& Stc, string Scope, u32 Index)
  {
    jconf::Value Cac;



    // Name
    Cac = Stc["&name"];
    if (!Cac.isString())
      throw runtime_error("Wrong type, value was expected: " +Scope+ "["+ std::to_string(Index) +"] -> &name");

    string Name = (string)Cac;
       


    // Type
    Cac = Stc["&type"];
    if (!Cac.isString())
      throw runtime_error("Wrong type, value was expected: " +Scope+ "/"+ Name +" -> &type");

    string Type = (string)Cac;
       


    // Create New
    auto it = RegList.find(Type);
    if (it == RegList.end())
      throw runtime_error("Species name not found: " +Scope+ "/"+ Name +" -> &type = \""+Type+"\"");


    control *Self = it->second();

    Self->QDL = jc_nref(Stc.getHandle());
    Self->Name = Name;


    // Style
    Cac = Stc["&style"];
    if (Cac.isNull())
    {
      // if (not found) then continue
    }

    ef (Cac.isString())
    {
      string Style = (string)Cac;
      Self->Style.push_back(Style);
    }
    
    ef (Cac.isArray())
    {
      for (u32 i = 0; i < Cac.size(); i++)
      {
        jconf::Value Obj = Cac[i];
        if (!Obj.isString())
          throw runtime_error("Wrong type, value was expected: " +Scope+ "/"+ Name +" -> &style["+to_string(i)+"]");


        string Style = (string)Obj;
        Self->Style.push_back(Style);
      }
    }

    else
      throw runtime_error("Wrong type, value or array was expected: " +Scope+ "/"+ Name +" -> &style");
    
      


    // Childs
    Cac = Stc["&subs"];
    if (!Cac.isNull())
    {
      if (!Cac.isArray())
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

  qdl LoadQDL_Arr(const jconf::Value& Arr, string Scope)
  {
    vector<shared_ptr<control>> Ret;

    for (u32 i = 0; i < Arr.size(); i++)
    {
      jconf::Value Obj = Arr[i];
      if (!Obj.isStruct())
        throw runtime_error("Wrong type, struct was expected: " +Scope +"["+ std::to_string(i) +"]");

      
      Ret.push_back(shared_ptr<control>(LoadQDL_Stc(Obj, Scope, i)));
    }

    return Ret;
  }



  pair<bool, shared_ptr<popup_item>> __popup_load_items(const jconf::Value& Node, string Scope, unordered_map<string, point> &FuncMap)
  {
    if (!Node.isArray())
      return {false, {}};


    shared_ptr<popup_item> Ret = make_shared<popup_item>();


    for (u32 i = 0; i < Node.size(); i++)
    {
      jconf::Value Sub = Node[i];

      if (Sub.isString())
      {
        string Cac = (string)Sub;

        shared_ptr<popup_item> Tmp = make_shared<popup_item>();
        Tmp->Name = Cac;

        Ret->Items.push_back(Tmp);
      }

      ef (Sub.isStruct())
      {
        auto VNa = Sub["&name"];
        if (VNa.isNull())
          return {true, {}};

        string Cac = (string)VNa;
        shared_ptr<popup_item> Tmp = make_shared<popup_item>();
        Tmp->Name = Cac;


        auto VIt = Sub["&subs"];
        if (!VIt.isNull())
        {
          auto SThe = __popup_load_items(VIt, Scope +"/"+ Cac, FuncMap);

          if (!SThe.first)
            return {false, {}};

          Tmp = SThe.second;
          Tmp->Name = Cac;

          
          for (auto &X: Tmp->Items)
            X->Parent = Ret.get();
        }



        auto VOnCl = Sub["^OnClick"];
        if (!VOnCl.isNull())
        {
          string Prop_Func = (string)VOnCl;


          auto it = FuncMap.find(Prop_Func);

          if (it == FuncMap.end())
            throw runtime_error("Func not found in code: " +Scope+" -> "+"OnClick" +" = "+ Prop_Func);


          Tmp->OnClick = (void (*)(qcl::control*))it->second;
        }


        Ret->Items.push_back(std::move(Tmp));
      }

      else
        return {false, {}};
    }

    return {true, Ret};
  }


  void LoadQDL_Popup(popup *Ctrl, string Scope, unordered_map<string, point> &FuncMap)
  {
    jconf::Value Node = jconf::Value(Ctrl->QDL, false)["Items"];
    // Assuming Ctrl->QDL is jc_obj/stc handle, we wrap it.
    
    if (Node.isNull())
      return;

    ef (!Node.isArray())
      throw runtime_error("Invalid prop: " +Scope+"/"+Ctrl->Name+" -> "+"Items");
    


    auto Temp = __popup_load_items(Node, Scope+"/"+Ctrl->Name +" -> Items", FuncMap);
    if (!Temp.first)
      throw runtime_error("Invalid prop: " +Scope+"/"+Ctrl->Name+" -> "+"Items");


    Ctrl->Items = Temp.second;
    Ctrl->VItems = Ctrl->Items.get();
  }

  void LoadQDL_Prop(control* Ctrl, string Scope, const jconf::Value& Styles, unordered_map<string, point> &FuncMap)
  {
    // Set Styles
    for (u32 s = 0; s < Ctrl->Style.size(); s++)
    {
      jconf::Value Style = Styles[Ctrl->Style[s]];
      if (Style.isNull())
        throw runtime_error("struct was expected: root/&styles -> " +Ctrl->Style[s]);

      else if (!Style.isStruct())
        throw runtime_error("Wrong type, struct was expected: root/&styles -> " +Ctrl->Style[s]);


      jc_obj H = Style.getHandle();
      for (u32 i = 0; i < jc_stc_count(H); i++)
      {
        char *CStr;
        string Prop_Name;

        jconf::Value Prop(jc_stc_index(H, i, &CStr));
        Prop_Name = string(CStr);
        jc_dis_str(CStr);


        if (Prop_Name[0] == '&')
          continue;

        
        if (Prop_Name[0] == '^')
        {
          string Prop_Func;

          Prop_Func = (string)Prop;


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
    if (Ctrl->QDL != Nil) 
    {
      jc_obj H = Ctrl->QDL;
      for (u32 i = 0; i < jc_stc_count(H); i++)
      {
        char *CStr;
        string Prop_Name;

        jconf::Value Prop(jc_stc_index(H, i, &CStr));
        Prop_Name = string(CStr);
        jc_dis_str(CStr);
        

        if (Prop_Name[0] == '&')
          continue;


      
      if (Prop_Name[0] == '^')
      {
        string Prop_Func;

        Prop_Func = (string)Prop;


        auto it = FuncMap.find(Prop_Func);

        if (it == FuncMap.end())
          throw runtime_error("Func not found in code: " +Scope+"/"+Ctrl->Name+" -> "+Prop_Name +" = "+ Prop_Func);


        if (!Ctrl->LoadFunc(Prop_Name.substr(1), it->second))
          throw runtime_error("Invalid prop: " +Scope+"/"+Ctrl->Name+" -> "+Prop_Name);
      }

      ef (auto C = dynamic_cast<qcl::popup*>(Ctrl); C != Nil && Prop_Name == "Items")
      {}

      else
      {
        if (!Ctrl->LoadProp(Prop_Name, Prop))
          throw runtime_error("Invalid prop: " +Scope+"/"+Ctrl->Name+" -> "+Prop_Name);
      }
    }


    // Popup
    if (auto C = dynamic_cast<qcl::popup*>(Ctrl); C != Nil)
      LoadQDL_Popup(C, Scope, FuncMap);


    if (auto C = dynamic_cast<qcl::view*>(Ctrl); C != Nil)
      for (auto &X: C->Childs)
        LoadQDL_Prop(X.get(), Scope+"/"+Ctrl->Name, Styles, FuncMap);
    }
  }

  qdl Load_FormFile(string FPath, unordered_map<string, point> FuncMap)
  {
    // Open File
    jconf::Value DL = jconf::Value::Parse(FPath);
   
    if (DL.isNull())
      throw runtime_error("Corrupt qdl file: " +FPath);


    DL.saveBin(FPath+'b'); // jconf::Value::saveBin expects handle to be set.


    // Load Comps
    jconf::Value Comps = DL["&subs"];
    if (!Comps.isArray())
      throw runtime_error("array was expected: root -> &subs: " +FPath);
    
    auto Ret = LoadQDL_Arr(Comps, "root");


    // Load Design
    jconf::Value Styles = DL["&styles"];
    if (!Styles.isStruct())
      throw runtime_error("struct was expected: root -> &styles: " +FPath);

    for (auto &X: Ret)
      LoadQDL_Prop(X.get(), "root", Styles, FuncMap);

    return Ret;
  }



  shared_ptr<control> FindFromName(qdl QDL, string Name)
  {
    for (auto &X: QDL)
    {
      if (X->Name == Name)
        return X;

      ef (auto C = dynamic_pointer_cast<view>(X); C != Nil)
      {
        if (auto B = FindFromName(C->Childs, Name); B != Nil)
          return B;
      }
    }

    return Nil;
  }

}
