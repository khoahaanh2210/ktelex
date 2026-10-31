#include "telex.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
static void check(const std::string&keys,const std::u32string&expected){
 ktlex::Engine engine;for(char c:keys)engine.feed(c);
 if(engine.text()!=expected){std::cerr<<"FAIL: "<<keys<<'\n';std::exit(1);}
 if(engine.commit()!=expected||!engine.text().empty())std::exit(1);
}
int main(){
 const std::string tones="sfrxj";
 struct Family {const char*before;const char*after;const char32_t*expected;};
 // Each expected string lists the five marked vowels in tone-key order.
 const Family families[]={
  {"qua","",U"áàảãạ"},{"qu","a",U"áàảãạ"},
  {"gia","",U"áàảãạ"},{"gi","a",U"áàảãạ"},
  {"hoa","n",U"áàảãạ"},{"hoan","",U"áàảãạ"},
  {"tu","aan",U"ấầẩẫậ"},{"tua","an",U"ấầẩẫậ"},{"tuaa","n",U"ấầẩẫậ"},{"tuaan","",U"ấầẩẫậ"},
  {"ti","een",U"ếềểễệ"},{"tie","en",U"ếềểễệ"},{"tiee","n",U"ếềểễệ"},{"tieen","",U"ếềểễệ"},
  {"cu","oon",U"ốồổỗộ"},{"cuo","on",U"ốồổỗộ"},{"cuoo","n",U"ốồổỗộ"},{"cuoon","",U"ốồổỗộ"},
  {"thu","owng",U"ớờởỡợ"},{"thuo","wng",U"ớờởỡợ"},{"thuow","ng",U"ớờởỡợ"},{"thuowng","",U"ớờởỡợ"}
 };
 for(const auto&f:families)for(size_t i=0;i<tones.size();++i){
  std::string before=f.before,keys=before+tones[i]+f.after;
  std::u32string expected;
  if(before.rfind("qu",0)==0)expected=U"qu"+std::u32string(1,f.expected[i]);
  else if(before.rfind("gi",0)==0)expected=U"gi"+std::u32string(1,f.expected[i]);
  else if(before.rfind("hoa",0)==0)expected=U"ho"+std::u32string(1,f.expected[i])+U"n";
  else if(before.rfind("tu",0)==0)expected=U"tu"+std::u32string(1,f.expected[i])+U"n";
  else if(before.rfind("ti",0)==0)expected=U"ti"+std::u32string(1,f.expected[i])+U"n";
  else if(before.rfind("cu",0)==0)expected=U"cu"+std::u32string(1,f.expected[i])+U"n";
  else expected=U"thư"+std::u32string(1,f.expected[i])+U"ng";
  check(keys,expected);
 }
 for(auto pair:{std::pair{"uww",U"uw"},{"oww",U"ow"},{"aww",U"aw"},
  {"uoww",U"uow"},{"uaww",U"uaw"},{"UOWW",U"UOW"},
  {"ddda",U"dda"},{"dddda",U"ddda"},{"ddaaan",U"đaan"},
  {"ddeeen",U"đeen"},{"ddooon",U"đoon"},{"hafooo",U"hàoo"},
  {"doOom",U"doOm"},{"thuwoww",U"thưow"}})check(pair.first,pair.second);
 const std::u32string quBreve=U"ắằẳẵặ",quHorn=U"ớờởỡợ";
 for(size_t i=0;i<tones.size();++i){
  check(std::string("quaw")+tones[i],U"qu"+std::u32string(1,quBreve[i]));
  check(std::string("qua")+tones[i]+"w",U"qu"+std::u32string(1,quBreve[i]));
  check(std::string("quawm")+tones[i],U"qu"+std::u32string(1,quBreve[i])+U"m");
  check(std::string("quow")+tones[i],U"qu"+std::u32string(1,quHorn[i]));
  check(std::string("quo")+tones[i]+"w",U"qu"+std::u32string(1,quHorn[i]));
 }
 check("QUAWS",U"QUẮ");check("Quowr",U"Quở");check("quaww",U"quaw");
 const std::u32string lower=U"áàảãạ",upper=U"ÁÀẢÃẠ";
 for(auto onset:{"qu","Qu","QU","qU","gi","Gi","GI","gI"})for(size_t i=0;i<tones.size();++i){
  std::string prefix=onset;
  std::u32string expected(prefix.begin(),prefix.end());
  bool caps=prefix=="QU"||prefix=="GI";
  expected+=caps?upper[i]:lower[i];
  check(prefix+(caps?"A":"a")+tones[i],expected);
  check(prefix+tones[i]+(caps?"A":"a"),expected);
 }
 for(auto pair:{std::pair{"QUAS",U"QUÁ"},{"Quas",U"Quá"},{"qUaS",U"qUá"},{"GIAS",U"GIÁ"},{"Gias",U"Giá"},{"gIaS",U"gIá"},{"tusaan",U"tuấn"},{"haf",U"hà"},{"zoom",U"zoom"},{"dienej",U"diện"},{"dooom",U"doom"}})check(pair.first,pair.second);
 const std::u32string openO=U"óòỏõọ";
 for(size_t i=0;i<tones.size();++i){
  ktlex::Engine engine;for(char c:std::string("hoan")+tones[i])engine.feed(c);
  engine.backspace();
  if(engine.text()!=U"h"+std::u32string(1,openO[i])+U"a"){std::cerr<<"FAIL: hoan tone backspace\n";return 1;}
 }
 for(auto keys:{"zoom","dooo","ddawng"}){
  ktlex::Engine engine;for(char c:std::string(keys))engine.feed(c);
  while(!engine.text().empty())engine.backspace();
  for(char c:std::string("haf"))engine.feed(c);
  if(engine.text()!=U"hà"){std::cerr<<"FAIL: backspace "<<keys<<'\n';return 1;}
  engine.reset();for(char c:std::string("zoom"))engine.feed(c);
  engine.reset();for(char c:std::string("qusa"))engine.feed(c);
  if(engine.text()!=U"quá")return 1;
 }
}
