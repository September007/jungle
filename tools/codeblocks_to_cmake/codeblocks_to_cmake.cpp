#include "codeblocks_to_cmake.h"
#include <fstream>

using namespace std;

int main()
{
    pugi::xml_document cbp;
    std::string cbp_file = "../../../tools/codeblocks_to_cmake/test/app/app.cbp";
    string cmake_file = fs::path(cbp_file).parent_path().string();
    pugi::xml_parse_result result = cbp.load_file(cbp_file.c_str());

    codeblocks::Project proj;
    codeblocks::Context cbCx;
    std::tie(proj, cbCx) = ParseCodeBlocksProject(cbp_file);
    trans::Context cx;
    cx.cbCtxs = {{fs::canonical(cbp_file).parent_path().string(), cbCx}};
    cx.cmCtx = {cmake_file, cmake_file};
    auto cmake_target = cbProj_to_cmakTarget(proj, cx);
    auto cmake_proj = cmakTargets_to_cmakeProj({cmake_target}, cmake_file);

    string cmake_str = cmake::Helper::dump_Project(cmake_proj);
    ofstream out(cx.cmCtx.cmake_current_dir+"/CMakeLists.txt");
    out << cmake_str << endl;
    cout << proj.title << endl;
}