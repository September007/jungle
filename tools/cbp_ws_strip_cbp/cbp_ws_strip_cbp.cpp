#include <codeblocks_to_cmake.h>
#include <vector>
using namespace std;
int main(int argc, char **argv) {

  vector<string> wss;
  for (int i = 1; i < argc; ++i)
    wss.push_back(argv[i]);
  vector<string> cbps;
  for (auto ws : wss)
    try {

      // to absolute path
      ws = fs::absolute(ws).string();
      scope_modify_Current_Path _change_path(fs::path(ws).parent_path());
      pugi::xml_document doc;
      auto result = doc.load_file(ws.c_str());
      if (!result)
        continue;
      auto ws_node = doc.child("CodeBlocks_workspace_file").child("Workspace");
      for (auto p = ws_node.first_child(); p; p = p.next_sibling())
        if (auto cbp = p.attribute("filename"); !cbp.empty()) {
          auto cbp_abs = fs::canonical(cbp.as_string());
          cbps.push_back(cbp_abs);
        }
    } catch (std::exception &e) {
      std::cerr << format("got error when handle {}\n{}", ws, e.what());
    }
  for (auto cbp : cbps)
  std:
    cout << cbp << endl;

  return 0;
}
