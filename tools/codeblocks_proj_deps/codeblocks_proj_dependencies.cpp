#include <codeblocks_proj_dependencies.h>
#include <string_util.h>
int main(int argc, char **argv)
{
    vector<string> ps;
    string outputPath="./deps.log";
    if(argc==1){
        std::cout<<format(R"(
        input: codeblocks workspace file
        output: depencies of targets in mermaid-graph
        
        )")<<std::endl;
        return 0;
    }
    {
        // input is workspace
        for(int i=1;i<argc;++i)try{
            string ws=argv[i];
            // to absolute path
            ws= fs::absolute(ws).string();
            scope_modify_Current_Path _change_path(fs::path(ws).parent_path().generic_string());
            pugi::xml_document doc;
            auto result=doc.load_file(ws.c_str());
            if(!result)continue;
            auto ws_node=doc.child("CodeBlocks_workspace_file").child("Workspace");
            for(auto p=ws_node.first_child();p;p=p.next_sibling())
                if(auto cbp=p.attribute("filename");!cbp.empty())
                    {
                        auto cbp_abs=fs::canonical(cbp.as_string());
                        ps.push_back(cbp_abs.generic_string());
                    }
        }catch(std::exception&e){
            SLOG(fmt::format("got error when handle {}\n{}",argv[i],e.what()),&std::cerr);
        }
    }
    // set log output
    std::ofstream log_out("./log_out.log");
    default_out()=&log_out;

    vector<DepsNode> nodes;
    for (auto &p : ps)
        nodes.push_back(fromCBPFile(p));
    std::stringstream ss;

    std::set<string> visit;
    std::map<string,DepsNode> m;
    auto isFirst = [&](string const &s)
    {
        return visit.insert(s).second;
    };
    auto GetTargetDescription = [&](string name)
    {
        auto &node=m[name];
        if(node.name=="")
            node.name=name;
        switch (node.node_type + (isFirst(node.name) ? 0 : 1000))
        {
        case codeblocks::E_Build_Type::GUI_APP:
        case codeblocks::E_Build_Type::CONSOLE_APP:
        case codeblocks::E_Build_Type::COMMANDS_ONLY:
        case codeblocks::E_Build_Type::NATIVE:
            return format("{}({})", node.name, node.name);
        case codeblocks::E_Build_Type::STATIC_APP:
            return format("{}[{}]", node.name, node.name);
        case codeblocks::E_Build_Type::DYNAMIC_APP:
            return format("{}{{{}}}", node.name, node.name);

        default:
            return format("{}", node.name);
        }
    };
    // register 
    for(auto &node:nodes)
    {
        // project target always have one time to setup info
        m[node.name] = node;
        // initial project target or 3rd
        for (auto &dep : node.deps_names)
            m[dep];
    };

    // exclude unvalid target
    {
        auto pm = m;
        for (auto &node : pm)
        {
            auto tname=node.first;
            for(auto & it:ignoredTarget)
            {
                std::regex regex_(it);
                if(auto match_begin=std::sregex_iterator(tname.begin(),tname.end(),regex_);
                    match_begin!=std::sregex_iterator())
                {
                    m.erase(tname);
                    break;
                }
            }
        }
    }
    auto isValidTarget=[&](std::string name){
        return m.find(name)!=m.end();
    };
    ss<<format(R"(graph TD
    方角是静态库
    菱形是动态库
)");
    for (auto &node : nodes)
        if (isValidTarget(node.name))
        {
            for (auto &dep : node.deps_names)
                if (isValidTarget(dep))
                    ss << format("    {}-->{}\n", GetTargetDescription(node.name), GetTargetDescription(dep));
            if (node.deps_names.size())
                ss << std::endl;
        }
    std::cout << ss.rdbuf() << std::endl;
    return 0;
}
