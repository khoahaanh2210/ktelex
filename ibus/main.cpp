#include <ibus.h>
#include "telex.hpp"
#include <codecvt>
#include <locale>
struct KtEngine { IBusEngine parent; ktlex::Engine *core; };
struct KtEngineClass { IBusEngineClass parent; };
G_DEFINE_TYPE(KtEngine, kt_engine, IBUS_TYPE_ENGINE)
static std::string utf8(const std::u32string&s){return std::wstring_convert<std::codecvt_utf8<char32_t>,char32_t>{}.to_bytes(s);}
static void update(KtEngine*e){auto s=utf8(e->core->text());ibus_engine_update_preedit_text(IBUS_ENGINE(e),ibus_text_new_from_string(s.c_str()),e->core->text().size(),!s.empty());}
static void commit(IBusEngine*base){auto e=(KtEngine*)base;auto s=utf8(e->core->commit());if(!s.empty())ibus_engine_commit_text(base,ibus_text_new_from_string(s.c_str()));update(e);}
static gboolean key(IBusEngine*base,guint k,guint,guint state){auto e=(KtEngine*)base;if(state&IBUS_RELEASE_MASK)return FALSE;if(state&(IBUS_CONTROL_MASK|IBUS_MOD1_MASK|IBUS_SUPER_MASK)){commit(base);return FALSE;}if(k==IBUS_KEY_Escape){e->core->reset();update(e);return TRUE;}if(k==IBUS_KEY_BackSpace){if(e->core->text().empty())return FALSE;e->core->backspace();update(e);return TRUE;}if((k>='a'&&k<='z')||(k>='A'&&k<='Z')){e->core->feed(k);update(e);return TRUE;}if(k==IBUS_KEY_space&&!e->core->text().empty()){
 auto s=utf8(e->core->commit());s+=' ';
 update(e);
 ibus_engine_commit_text(base,ibus_text_new_from_string(s.c_str()));
 return TRUE;
 }commit(base);return FALSE;}
static void reset(IBusEngine*b){auto e=(KtEngine*)b;e->core->reset();update(e);}
static void finalize(GObject*o){delete ((KtEngine*)o)->core;G_OBJECT_CLASS(kt_engine_parent_class)->finalize(o);}
static void kt_engine_init(KtEngine*e){e->core=new ktlex::Engine;}
static void kt_engine_class_init(KtEngineClass*c){auto b=IBUS_ENGINE_CLASS(c);b->process_key_event=key;b->reset=reset;b->focus_out=commit;b->disable=commit;G_OBJECT_CLASS(c)->finalize=finalize;}
int main(){ibus_init();auto bus=ibus_bus_new();if(!ibus_bus_is_connected(bus))return 1;auto factory=ibus_factory_new(ibus_bus_get_connection(bus));ibus_factory_add_engine(factory,"ktlex",kt_engine_get_type());ibus_bus_request_name(bus,"org.freedesktop.IBus.Ktlex",0);ibus_main();g_object_unref(factory);g_object_unref(bus);return 0;}
