#include "codeblocks_to_cmake.h"
#include <fstream>

using namespace std;

int main(int argc,const char **argv)
{
    system("pwd");
    cout<<format("argc: {}",argc)<<endl;
    pugi::xml_document cbp;
    const char*default_argv[]={argv[0]
    ,"../../../tools/codeblocks_to_cmake/test/app/app.cbp"
    ,"/home/ws/src/proj/TCCodec_VCU_Linux/Src/Source/Plugins/wp_mxf/wp_mxf.cbp"};
    if(argc==1)
        {
            constexpr int l=sizeof(default_argv)/sizeof(*default_argv);
            argc=l;
            argv=default_argv;
        }
    auto cwd=fs::current_path();
    for (int i = 1; i < argc; ++i)
    try{
        scope_modify_Current_Path __(cwd);
        std::string cbp_file = argv[i];
        // 转换cbp_file 到绝对路径可以避免以后的某些转换错误
        cbp_file = fs::absolute(cbp_file).string();
        string target_cmake_dir = fs::path(cbp_file).parent_path().string();
        
        pugi::xml_parse_result result = cbp.load_file(cbp_file.c_str());

        codeblocks::Project proj;
        codeblocks::Context cbCx;
        trans::Context cx;
        cx.cmCtx = {target_cmake_dir, target_cmake_dir};
        auto cmake_output_path=cx.cmCtx.cmake_current_dir + "/CMakeLists.txt";
        ofstream out(cmake_output_path);

        cout<<format("\n-- start {}\n-- outputfile {}\n",cbp_file,cmake_output_path);
        auto q=fs::is_regular_file(cbp_file);
        {
            std::ifstream oo(cbp_file);
            string s;
            oo >> s;
            oo>>s;
        }
        std::tie(proj, cbCx) = ParseCodeBlocksProject(cbp_file);
        cx.cbCtxs = {{fs::canonical(cbp_file).parent_path().string(), cbCx}};

        auto cmake_target = cbProj_to_cmakTarget(proj, cx);
        auto cmake_proj = cmakTargets_to_cmakeProj({cmake_target}, target_cmake_dir);

        string cmake_str = cmake::Helper::dump_Project(cmake_proj);
        out << cmake_str << endl;

        cout<<format("-- end {}\n",cbp_file);
    }catch(std::exception &e){
        std::cerr<<format("-- catch error when parsing [{}]\n",argv[i]);
        std::cerr<<format("-- error: [{}]",e.what())<<endl;
        
    }
    // cout << proj.title << endl;
}