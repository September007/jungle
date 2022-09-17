#include "codeblocks_to_cmake.h"
#include <pugixml.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <fmt/format.h>

using namespace std;

int main() try{
    pugi::xml_document cbp;
    std::string cbp_file="/home/ws/src/proj/TCCodec_VCU_Linux/Src/Source/SDK/OGG/lib_ogg-1.1.3/win32/ogg_static.cbp";
    std:string out_file=cbp_file+".xml";
    ofstream out(out_file);
    pugi::xml_parse_result result = cbp.load_file(cbp_file.c_str());

//     auto name=cbp.attributes();
//     if (!result)
//         return -1;
//     auto xml=cbp.child("xml");
//     out<<fmt::format(R"(<CodeBlocks_project_file>
// 	<FileVersion major="{}" minor="{}" />
// <CodeBlocks_project_file/>)",
//         cbp.child("CodeBlocks_project_file").child("FileVersion").attribute("major").as_int(),
//         cbp.child("CodeBlocks_project_file").child("FileVersion").attribute("minor").as_int()
//         );
    auto p=ParseCodeBlocksProject(cbp_file);
    
}
catch(std::exception &e){
    cout<<e.what()<<endl;
}