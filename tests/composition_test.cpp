#define main ktlex_program_main
#include "../ibus/main.cpp"
#undef main
#include <cstdlib>
#include <iostream>
#include <vector>
static std::u32string document,preedit;
static size_t cursor=0,anchor=0;
static int commits=0,deletions=0,cases=0;
static guint preeditMode=IBUS_ENGINE_PREEDIT_CLEAR;
static void require(bool ok,const std::string&message){if(!ok){std::cerr<<message<<'\n';std::exit(1);}}
static std::u32string decode(const char*s){return std::wstring_convert<std::codecvt_utf8<char32_t>,char32_t>{}.from_bytes(s);}
extern "C" void __wrap_ibus_engine_commit_text(IBusEngine*,IBusText*t){
 auto insertion=decode(t->text);auto start=std::min(cursor,anchor),end=std::max(cursor,anchor);
 document.replace(start,end-start,insertion);cursor=anchor=start+insertion.size();++commits;g_object_unref(t);
}
extern "C" void __wrap_ibus_engine_delete_surrounding_text(IBusEngine*,gint,guint){
 // GNOME advertises this capability even when the editor rejects deletion.
 // A lost delete must never leave both the old vowel and its replacement.
 ++deletions;
}
extern "C" void __wrap_ibus_engine_update_preedit_text(IBusEngine*,IBusText*t,guint position,gboolean visible){
 preedit=visible?decode(t->text):U"";
 require(position==decode(t->text).size(),"preedit cursor must count characters, not UTF-8 bytes");
 g_object_unref(t);
}
extern "C" void __wrap_ibus_engine_update_preedit_text_with_mode(IBusEngine*base,IBusText*t,guint position,gboolean visible,guint mode){
 preeditMode=mode;__wrap_ibus_engine_update_preedit_text(base,t,position,visible);
}
static void surrounding(IBusEngine*base,const std::u32string&text,guint pos){
 auto t=ibus_text_new_from_string(utf8(text).c_str());g_object_ref_sink(t);
 g_signal_emit_by_name(base,"set-surrounding-text",t,pos,pos);g_object_unref(t);
}
static void press(IBusEngine*base,guint k){require(key(base,k,0,0),"key must be handled");}
static void fresh(IBusEngine*base){
 reset(base);document=U"prefix🙂 ";cursor=anchor=document.size();preedit.clear();commits=deletions=0;
 surrounding(base,document,cursor);
}
static void check(IBusEngine*base,const std::string&keys,const std::u32string&expected){
 fresh(base);auto prefix=document;
 // No per-key surrounding feedback: real clients can delay/coalesce it.
 for(char c:keys)press(base,c);
 require(document+preedit==prefix+expected,"wrong visible text for "+keys+": "+utf8(document+preedit));
 require(commits==0&&deletions==0,"composition must not depend on destructive direct edits: "+keys);
 press(base,IBUS_KEY_space);
 require(document==prefix+expected+U" "&&preedit.empty()&&commits==1,"word/space must commit exactly once: "+keys);
 require(!key(base,IBUS_KEY_space,0,0),"a second space must pass through");
 require(commits==1,"empty word must not recommit");++cases;
}
int main(){
 auto base=IBUS_ENGINE(g_object_new(kt_engine_get_type(),"engine-name","test","object-path","/org/ktlex/composition",nullptr));
 // Reproduces haf -> haà in the previous direct implementation.
 base->client_capabilities=IBUS_CAP_SURROUNDING_TEXT|IBUS_CAP_PREEDIT_TEXT;
 check(base,"haf",U"hà");
 const std::string vowels[]={"a","aw","aa","e","ee","i","o","oo","ow","u","uw","y"};
 const std::u32string lower[]={U"aáàảãạ",U"ăắằẳẵặ",U"âấầẩẫậ",U"eéèẻẽẹ",U"êếềểễệ",U"iíìỉĩị",U"oóòỏõọ",U"ôốồổỗộ",U"ơớờởỡợ",U"uúùủũụ",U"ưứừửữự",U"yýỳỷỹỵ"};
 const std::u32string upper[]={U"AÁÀẢÃẠ",U"ĂẮẰẲẴẶ",U"ÂẤẦẨẪẬ",U"EÉÈẺẼẸ",U"ÊẾỀỂỄỆ",U"IÍÌỈĨỊ",U"OÓÒỎÕỌ",U"ÔỐỒỔỖỘ",U"ƠỚỜỞỠỢ",U"UÚÙỦŨỤ",U"ƯỨỪỬỮỰ",U"YÝỲỶỸỴ"};
 const std::string tones[]={"","s","f","r","x","j"};
 for(guint capability:{guint(0),guint(IBUS_CAP_SURROUNDING_TEXT)}){
  base->client_capabilities=IBUS_CAP_PREEDIT_TEXT|capability;
  for(int v=0;v<12;++v)for(int t=0;t<6;++t){
   check(base,"h"+vowels[v]+tones[t],U"h"+std::u32string(1,lower[v][t]));
   auto keys="h"+vowels[v]+tones[t];for(auto &c:keys)c-=32;
   check(base,keys,U"H"+std::u32string(1,upper[v][t]));
  }
  for(auto pair:{std::pair{"giayas",U"giấy"},{"quayas",U"quấy"},{"ddaya",U"đây"},{"DDAYA",U"ĐÂY"},{"ddaysa",U"đấy"},{"cauaf",U"cầu"},{"toio",U"tôi"},{"tieues",U"tiếu"},{"yeue",U"yêu"},{"tuoiws",U"tưới"},{"guiwr",U"gửi"},{"cuuws",U"cứu"},{"huouw",U"hươu"},{"nhapaj",U"nhập"},{"nhapja",U"nhập"},{"NHAPAJ",U"NHẬP"},{"ddapaj",U"đập"},{"bepes",U"bếp"},{"lenhej",U"lệnh"},{"hueches",U"huếch"},{"nguechej",U"nguệch"},{"hopoj",U"hộp"},{"luataj",U"luật"},{"data",U"data"},{"coco",U"coco"},{"dienej",U"diện"},{"dienje",U"diện"},{"tienges",U"tiếng"},{"QUAS",U"QUÁ"},{"Gias",U"Giá"},{"QUAWS",U"QUẮ"},{"quawms",U"quắm"},{"quowr",U"quở"},{"qusa",U"quá"},{"hoasn",U"hoán"},{"thuowngf",U"thường"},{"ddawng",U"đăng"},{"dooom",U"doom"},{"zoom",U"zoom"},{"conssole",U"console"}})check(base,pair.first,pair.second);
 }
 // Delimiters and navigation commit once, then remain the client's key.
 for(guint boundary:{guint('.'),guint(','),guint('1'),guint(IBUS_KEY_Return),guint(IBUS_KEY_Tab),guint(IBUS_KEY_Left),guint(IBUS_KEY_Delete)}){
  fresh(base);for(char c:std::string("haf"))press(base,c);
  require(!key(base,boundary,0,0),"delimiter/navigation must pass through");
  require(document==U"prefix🙂 hà"&&preedit.empty()&&commits==1,"delimiter/navigation must preserve exactly one word");
 }
 // The eventual commit replaces a selection and keeps surrounding text.
 fresh(base);document=U"before OLD after";cursor=7;anchor=10;
 for(char c:std::string("haf"))press(base,c);press(base,IBUS_KEY_space);
 require(document==U"before hà  after"&&commits==1,"commit must replace only the selection");
 fresh(base);for(char c:std::string("haf"))press(base,c);
 press(base,IBUS_KEY_BackSpace);require(preedit==U"h","backspace must remove one visible Unicode character");
 press(base,'a');press(base,'f');require(preedit==U"hà","composition must continue after backspace");
 press(base,IBUS_KEY_Escape);require(preedit.empty()&&document==U"prefix🙂 ","escape must cancel pending text only");
 for(char c:std::string("haf"))press(base,c);
 require(preeditMode==IBUS_ENGINE_PREEDIT_COMMIT,"focus changes must preserve preedit via COMMIT mode");
 // Mutter/IBus commits COMMIT-mode preedit before notifying the engine.
 __wrap_ibus_engine_commit_text(base,ibus_text_new_from_string(utf8(preedit).c_str()));
 reset(base);IBUS_ENGINE_GET_CLASS(base)->focus_out(base);
 require(document==U"prefix🙂 hà"&&commits==1,"focus out must preserve exactly one client-owned commit");
 IBUS_ENGINE_GET_CLASS(base)->disable(base);require(commits==1,"disable after focus out must not duplicate a word");
 fresh(base);for(char c:std::string("haf"))press(base,c);reset(base);
 require(preedit.empty()&&document==U"prefix🙂 "&&commits==0,"reset must discard uncommitted composition");
 fresh(base);press(base,'h');press(base,'a');
 for(guint modifier:{guint(IBUS_KEY_Shift_L),guint(IBUS_KEY_Shift_R),guint(IBUS_KEY_Control_L),guint(IBUS_KEY_Alt_R),guint(IBUS_KEY_Super_L),guint(IBUS_KEY_Caps_Lock),guint(IBUS_KEY_ISO_Level3_Shift)}){
  require(!key(base,modifier,0,0),"modifier key must pass through");
  require(preedit==U"ha"&&commits==0,"pressing a modifier alone must not split the pending word");
 }
 press(base,'F');require(preedit==U"hà","Shift plus uppercase tone key must still compose the same word");
 require(!key(base,'c',0,IBUS_CONTROL_MASK),"Ctrl shortcut must pass through");
 require(document==U"prefix🙂 hà"&&commits==1,"shortcut must commit current word once");
 require(!key(base,'a',0,IBUS_RELEASE_MASK)&&commits==1,"key release must not produce text");
 std::cout<<cases<<" composition cases passed; rejected deletion, delayed feedback and lifecycle checked\n";
 g_object_unref(base);
}
