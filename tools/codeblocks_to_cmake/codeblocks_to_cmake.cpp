#include "codeblocks_to_cmake.h"
#include <pugixml.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <fmt/format.h>

using namespace std;

int main() {
    pugi::xml_document cbp;
    std::string cbp_file="/home/lull/src/cb/test/app/app.cbp";
    std:string out_file=cbp_file;
    out_file.append(".xml");
    ofstream out(out_file);
    pugi::xml_parse_result result = cbp.load_file(cbp_file.c_str());

    auto proj=ParseCodeBlocksProject(cbp_file);
    
    cout<<proj.title<<endl;
    
}