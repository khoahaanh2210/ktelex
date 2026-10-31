#include "telex.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
static int cases=0;
static void check(const std::string&keys,const std::u32string&expected){
 ktlex::Engine engine;for(char c:keys)engine.feed(c);
 if(engine.text()!=expected||engine.commit()!=expected||!engine.text().empty()){
  std::cerr<<"late modifier failed: "<<keys<<'\n';std::exit(1);
 }
 ++cases;
}
int main(){
 check("nhapaj",U"nhập");
 check("ddaya",U"đây");
 struct Glide {const char*keys;char modifier;const char32_t*prefix;const char32_t*forms;char32_t last;};
 const Glide glides[]={
  {"dday",'a',U"đ",U"âấầẩẫậ",U'y'},{"cau",'a',U"c",U"âấầẩẫậ",U'u'},
  {"khuay",'a',U"khu",U"âấầẩẫậ",U'y'},{"toi",'o',U"t",U"ôốồổỗộ",U'i'},
  {"cuoi",'o',U"cu",U"ôốồổỗộ",U'i'},{"theu",'e',U"th",U"êếềểễệ",U'u'},
  {"tieu",'e',U"ti",U"êếềểễệ",U'u'},{"yeu",'e',U"y",U"êếềểễệ",U'u'},
  {"ngueu",'e',U"ngu",U"êếềểễệ",U'u'},{"boi",'w',U"b",U"ơớờởỡợ",U'i'},
  {"tuoi",'w',U"tư",U"ơớờởỡợ",U'i'},{"gui",'w',U"g",U"ưứừửữự",U'i'},
  {"cuu",'w',U"c",U"ưứừửữự",U'u'},{"huou",'w',U"hư",U"ơớờởỡợ",U'u'}
 };
 const std::string glideTones[]={"","s","f","r","x","j"};
 for(const auto&family:glides)for(int t=0;t<6;++t){
  std::string source=family.keys,modifier(1,family.modifier);
  auto expected=std::u32string(family.prefix)+family.forms[t]+family.last;
  check(source+modifier+glideTones[t],expected);
  check(source+glideTones[t]+modifier,expected);
  check(source.substr(0,source.size()-1)+glideTones[t]+source.back()+modifier,expected);
 }
 check("giaya",U"giây");check("giayas",U"giấy");check("quayas",U"quấy");
 check("DDAYA",U"ĐÂY");check("Ddaya",U"Đây");check("ddayaaf",U"đayaf");
 check("GUIWR",U"GỬI");check("tuwoiws",U"tưới");
 check("ddaay",U"đây");check("caauf",U"cầu");check("tooif",U"tồi");
 check("theeus",U"thếu");check("tieeur",U"tiểu");check("yeeuf",U"yều");
 check("bowi",U"bơi");check("tuwowis",U"tưới");check("guwri",U"gửi");
 check("cuwsu",U"cứu");check("huwowu",U"hươu");

 // Every tone, with the circumflex before/after the final consonant and
 // the tone before/after either. Expected Unicode rows are independent.
 struct Nucleus {const char*head;char modifier;const char32_t*prefix;const char32_t*forms;};
 const Nucleus nuclei[]={
  {"nha",'a',U"nh",U"âấầẩẫậ"},{"be",'e',U"b",U"êếềểễệ"},{"to",'o',U"t",U"ôốồổỗộ"},
  {"tua",'a',U"tu",U"âấầẩẫậ"},{"tie",'e',U"ti",U"êếềểễệ"},
  {"ye",'e',U"y",U"êếềểễệ"},{"huye",'e',U"huy",U"êếềểễệ"},
  {"hue",'e',U"hu",U"êếềểễệ"},{"quye",'e',U"quy",U"êếềểễệ"},
  {"cuo",'o',U"cu",U"ôốồổỗộ"},{"qua",'a',U"qu",U"âấầẩẫậ"},{"que",'e',U"qu",U"êếềểễệ"}
 };
 const std::string tones[]={"","s","f","r","x","j"};
 for(const auto&family:nuclei)for(auto coda:{"m","n","ng"})for(int t=0;t<6;++t){
  std::string head=family.head,shape(1,family.modifier);
  std::u32string end;for(char c:std::string(coda))end+=c;
  auto expected=std::u32string(family.prefix)+family.forms[t]+end;
  check(head+coda+shape+tones[t],expected);
  check(head+coda+tones[t]+shape,expected);
  check(head+tones[t]+coda+shape,expected);
  check(head+shape+coda+tones[t],expected);
 }
 // Stop codas exercise the deferred branch; only sắc/nặng form normal
 // Vietnamese stop-coda syllables. An unmarked late candidate stays literal.
 for(const auto&family:nuclei)for(auto coda:{"c","ch","p","t"})for(int t:{0,1,5}){
  std::string head=family.head,shape(1,family.modifier);
  std::u32string end;for(char c:std::string(coda))end+=c;
  auto shaped=std::u32string(family.prefix)+family.forms[t]+end;
  auto late=head+coda+shape;
  auto expected=t?shaped:std::u32string(late.begin(),late.end());
  check(late+tones[t],expected);
  check(head+coda+tones[t]+shape,expected);
  check(head+tones[t]+coda+shape,expected);
  check(head+shape+coda+tones[t],shaped);
 }
 for(auto pair:{std::pair{"nhapja",U"nhập"},{"nhajpa",U"nhập"},{"nhaapj",U"nhập"},
  {"NHAPAJ",U"NHẬP"},{"Nhapaj",U"Nhập"},{"nhApaJ",U"nhẬp"},
  {"bacas",U"bấc"},{"batas",U"bất"}}){
  check(pair.first,pair.second);
 }
 for(auto pair:{std::pair{"bepes",U"bếp"},{"bepse",U"bếp"},{"betej",U"bệt"},
  {"lenhej",U"lệnh"},{"lechej",U"lệch"},
  {"hueches",U"huếch"},{"nguechej",U"nguệch"},{"huenhe",U"huênh"},
  {"tocos",U"tốc"},{"hopoj",U"hộp"},{"luataj",U"luật"},
  {"quanaf",U"quần"},{"tiepes",U"tiếp"},
  {"viecej",U"việc"},{"yetes",U"yết"},{"huyetes",U"huyết"},{"thuocos",U"thuốc"}}){
  check(pair.first,pair.second);
 }
 // Unmarked stop-coda candidates remain literal until an explicit tone
 // disambiguates them. This preserves the existing data/coco behavior.
 check("data",U"data");check("coco",U"coco");check("nhapa",U"nhapa");
 check("ddapa",U"đapa");check("ddapaj",U"đập");
 check("nhapal",U"nhapal");check("nhapaa",U"nhapa");
 check("hoana",U"hoana");check("boene",U"boene");
 ktlex::Engine edited;for(char c:std::string("nhapa"))edited.feed(c);
 edited.backspace();for(char c:std::string("ja"))edited.feed(c);
 if(edited.text()!=U"nhập"){std::cerr<<"late modifier after backspace failed\n";return 1;}
 edited.reset();for(char c:std::string("ddapa"))edited.feed(c);
 edited.backspace();for(char c:std::string("ja"))edited.feed(c);
 if(edited.text()!=U"đập"){std::cerr<<"late modifier must preserve đ when editing\n";return 1;}
 edited.reset();for(char c:std::string("data"))edited.feed(c);
 if(edited.commit()!=U"data")return 1;
 for(char c:std::string("haf"))edited.feed(c);
 if(edited.text()!=U"hà")return 1;
 std::cout<<cases<<" late modifier cases passed\n";
}
