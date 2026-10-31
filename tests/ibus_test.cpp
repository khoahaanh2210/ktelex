#define main ktlex_program_main
#include "../ibus/main.cpp"
#undef main
#include <cstdlib>
#include <iostream>
#include <vector>
static std::vector<std::string> commits;
extern "C" void __wrap_ibus_engine_commit_text(IBusEngine*, IBusText*t){commits.emplace_back(t->text);g_object_unref(t);}
extern "C" void __wrap_ibus_engine_update_preedit_text(IBusEngine*, IBusText*t,guint,gboolean){g_object_unref(t);}
static void require(bool ok){if(!ok){std::cerr<<"space must be committed exactly once with the pending word\n";std::exit(1);}}
int main(){auto base=IBUS_ENGINE(g_object_new(kt_engine_get_type(), "engine-name", "test", "object-path", "/org/ktlex/test", nullptr));auto &core=*reinterpret_cast<KtEngine*>(base)->core;
 for(char c:std::string("xin"))key(base,c,0,0);
 require(key(base,IBUS_KEY_space,0,0));require(commits==std::vector<std::string>{"xin "});require(core.text().empty());
 for(char c:std::string("chao"))key(base,c,0,0);
 require(key(base,IBUS_KEY_space,0,0));require(commits==std::vector<std::string>({"xin ","chao "}));
 require(!key(base,IBUS_KEY_space,0,0));require(commits.size()==2);
 require(!key(base,IBUS_KEY_space,0,IBUS_RELEASE_MASK));require(commits.size()==2);
 for(auto input:{"console","conssole","CONSSOLE"}){
  commits.clear();
  for(char c:std::string(input))require(key(base,c,0,0));
  require(key(base,IBUS_KEY_space,0,0));
  require(commits==std::vector<std::string>{std::string(input)=="CONSSOLE"?"CONSOLE ":"console "});
 }
 g_object_unref(base);
}
