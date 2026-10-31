#include <ibus.h>
#include "telex.hpp"
#include <codecvt>
#include <locale>

struct KtEngine { IBusEngine parent; ktlex::Engine *core; };
struct KtEngineClass { IBusEngineClass parent; };
G_DEFINE_TYPE(KtEngine, kt_engine, IBUS_TYPE_ENGINE)

static std::string utf8(const std::u32string &text) {
 return std::wstring_convert<std::codecvt_utf8<char32_t>,char32_t>{}.to_bytes(text);
}

static void update(KtEngine *engine) {
 auto bytes=utf8(engine->core->text());
 auto text=ibus_text_new_from_string(bytes.c_str());
 auto length=engine->core->text().size();
 ibus_text_append_attribute(text,IBUS_ATTR_TYPE_UNDERLINE,IBUS_ATTR_UNDERLINE_NONE,0,length);
 // Keep the word in preedit until it is committed. Surrounding-text
 // capability does not guarantee that the editor will apply deletions.
 ibus_engine_update_preedit_text_with_mode(IBUS_ENGINE(engine),text,length,!bytes.empty(),IBUS_ENGINE_PREEDIT_COMMIT);
}

static void commit(IBusEngine *base) {
 auto engine=reinterpret_cast<KtEngine*>(base);
 auto bytes=utf8(engine->core->commit());
 update(engine);
 if(!bytes.empty())ibus_engine_commit_text(base,ibus_text_new_from_string(bytes.c_str()));
}

static gboolean key(IBusEngine *base,guint keyval,guint,guint state) {
 auto engine=reinterpret_cast<KtEngine*>(base);
 if(state&IBUS_RELEASE_MASK)return FALSE;
 // Modifier presses are not word boundaries. The following printable key
 // carries the modifier state (for example Shift+F is still a tone key).
 switch(keyval) {
 case IBUS_KEY_Shift_L: case IBUS_KEY_Shift_R:
 case IBUS_KEY_Control_L: case IBUS_KEY_Control_R:
 case IBUS_KEY_Alt_L: case IBUS_KEY_Alt_R:
 case IBUS_KEY_Meta_L: case IBUS_KEY_Meta_R:
 case IBUS_KEY_Super_L: case IBUS_KEY_Super_R:
 case IBUS_KEY_Hyper_L: case IBUS_KEY_Hyper_R:
 case IBUS_KEY_Caps_Lock: case IBUS_KEY_Shift_Lock:
 case IBUS_KEY_Num_Lock: case IBUS_KEY_ISO_Level3_Shift:
  return FALSE;
 default: break;
 }
 if(state&(IBUS_CONTROL_MASK|IBUS_MOD1_MASK|IBUS_SUPER_MASK)) {
  commit(base);
  return FALSE;
 }
 if(keyval==IBUS_KEY_Escape) {
  engine->core->reset();
  update(engine);
  return TRUE;
 }
 if(keyval==IBUS_KEY_BackSpace) {
  if(engine->core->text().empty())return FALSE;
  engine->core->backspace();
  update(engine);
  return TRUE;
 }
 if((keyval>='a'&&keyval<='z')||(keyval>='A'&&keyval<='Z')) {
  engine->core->feed(keyval);
  update(engine);
  return TRUE;
 }
 if(keyval==IBUS_KEY_space&&!engine->core->text().empty()) {
  auto bytes=utf8(engine->core->commit());
  bytes+=' ';
  update(engine);
  ibus_engine_commit_text(base,ibus_text_new_from_string(bytes.c_str()));
  return TRUE;
 }
 commit(base);
 return FALSE;
}

static void reset(IBusEngine *base) {
 auto engine=reinterpret_cast<KtEngine*>(base);
 engine->core->reset();
 update(engine);
}
static void finalize(GObject *object) {
 delete reinterpret_cast<KtEngine*>(object)->core;
 G_OBJECT_CLASS(kt_engine_parent_class)->finalize(object);
}
static void kt_engine_init(KtEngine *engine) { engine->core=new ktlex::Engine; }
static void kt_engine_class_init(KtEngineClass *klass) {
 auto engine=IBUS_ENGINE_CLASS(klass);
 engine->process_key_event=key;
 engine->reset=reset;
 // IBus/the client commits COMMIT-mode preedit before these callbacks.
 // Clear the local copy so a later key cannot commit the same word twice.
 engine->focus_out=reset;
 engine->disable=reset;
 G_OBJECT_CLASS(klass)->finalize=finalize;
}
int main() {
 ibus_init();
 auto bus=ibus_bus_new();
 if(!ibus_bus_is_connected(bus))return 1;
 auto factory=ibus_factory_new(ibus_bus_get_connection(bus));
 ibus_factory_add_engine(factory,"ktlex",kt_engine_get_type());
 ibus_bus_request_name(bus,"org.freedesktop.IBus.Ktlex",0);
 ibus_main();
 g_object_unref(factory);
 g_object_unref(bus);
 return 0;
}
