#define main ktlex_program_main
#include "../ibus/main.cpp"
#undef main
#include <cstdlib>
#include <iostream>
#include <vector>
static std::vector<std::string> commits;
extern "C" void __wrap_ibus_engine_commit_text(IBusEngine*, IBusText*t){commits.emplace_back(t->text);g_object_unref(t);}
extern "C" void __wrap_ibus_engine_update_preedit_text(IBusEngine*, IBusText*t,guint cursor,gboolean visible){
 if(visible){
  if(cursor!=g_utf8_strlen(t->text,-1)){std::cerr<<"preedit cursor must count Unicode characters\n";std::exit(1);}
  bool noUnderline=false;
  if(t->attrs)for(guint i=0;;++i){
   auto a=ibus_attr_list_get(t->attrs,i);if(!a)break;
   if(a->type==IBUS_ATTR_TYPE_UNDERLINE&&a->value==IBUS_ATTR_UNDERLINE_NONE&&a->start_index==0&&a->end_index==cursor)noUnderline=true;
  }
  if(!noUnderline){std::cerr<<"preedit must explicitly disable underline over the entire word\n";std::exit(1);}
 }
 g_object_unref(t);
}
extern "C" void __wrap_ibus_engine_update_preedit_text_with_mode(IBusEngine*base,IBusText*t,guint cursor,gboolean visible,guint mode){
 if(visible&&mode!=IBUS_ENGINE_PREEDIT_COMMIT){std::cerr<<"visible preedit must survive focus changes\n";std::exit(1);}
 __wrap_ibus_engine_update_preedit_text(base,t,cursor,visible);
}
static void require(bool ok){if(!ok){std::cerr<<"space must be committed exactly once with the pending word\n";std::exit(1);}}
int main(){auto base=IBUS_ENGINE(g_object_new(kt_engine_get_type(), "engine-name", "test", "object-path", "/org/ktlex/test", nullptr));auto &core=*reinterpret_cast<KtEngine*>(base)->core;
 for(char c:std::string("xin"))key(base,c,0,0);
 require(key(base,IBUS_KEY_space,0,0));require(commits==std::vector<std::string>{"xin "});require(core.text().empty());
 for(char c:std::string("chao"))key(base,c,0,0);
 require(key(base,IBUS_KEY_space,0,0));require(commits==std::vector<std::string>({"xin ","chao "}));
 require(!key(base,IBUS_KEY_space,0,0));require(commits.size()==2);
 require(!key(base,IBUS_KEY_space,0,IBUS_RELEASE_MASK));require(commits.size()==2);
 for(auto pair:{std::pair{"console","console "},{"conssole","console "},{"CONSSOLE","CONSOLE "},
  {"dooom","doom "},{"doooooom","dooooom "},{"zoom","zoom "},{"dienej","diện "},{"dienje","diện "},{"tienges","tiếng "}}){
  commits.clear();
  for(char c:std::string(pair.first))require(key(base,c,0,0));
  require(key(base,IBUS_KEY_space,0,0));
  require(commits==std::vector<std::string>{pair.second});
 }
 g_object_unref(base);
}
