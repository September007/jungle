#pragma once
#include "cmake.h"
#include "codeblocks.h"
#include <pugixml.hpp>
#include <filesystem>
#include <functional>

inline codeblocks::Project ParseCodeBlocksProject(string cbp_file_path){
    pugi::xml_document cbp;
    codeblocks::Project ret;
    if(auto load_result=cbp.load_file(cbp_file_path.c_str());!load_result)
        return {};

    auto p=cbp.child("CodeBlocks_project_file").child("Project");
    ret.title=p.find_child([](pugi:: xml_node &child){return !child.attribute("title").empty();}).attribute("title").as_string();
    ret.pch_mode=codeblocks::E_PCH_Mode(p.find_child([](pugi:: xml_node &child){return !child.attribute("title").empty();}).attribute("title").as_int());
    ret.compiler=p.find_child([](pugi:: xml_node &child){return !child.attribute("title").empty();}).attribute("title").as_string();
    
    for (pugi::xml_node child = p.first_child(); child; child = child.next_sibling())
    { 
        string pname=child.name();
        if(pname=="Option")
        {
            if(!child.attribute("title").empty())
                ret.title=child.attribute("title").as_string();
            if(!child.attribute("pch_mode").empty())
                ret.pch_mode=codeblocks::E_PCH_Mode(child.attribute("pch_mode").as_int());
            if(!child.attribute("compiler").empty())
                ret.compiler=child.attribute("compiler").as_string();
            
        }else if(pname=="Build"){
            for(pugi::xml_node target=child.first_child();target;target=target.next_sibling()){

            }
        }
    }
    return ret;
}