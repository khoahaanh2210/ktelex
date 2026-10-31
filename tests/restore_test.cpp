#include "telex.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
static void check(const std::string&keys,const std::u32string&expected){ktlex::Engine e;for(unsigned char c:keys)e.feed(c);if(e.text()!=expected){std::cerr<<"FAIL: "<<keys<<'\n';std::exit(1);}if(e.commit()!=expected||!e.text().empty())std::exit(1);}
int main(int argc,char**argv){
 if(argc>1&&std::string(argv[1])=="repeated-vowels"){
  for(auto vowel:{'a','e','o','A','E','O'})for(int count=3;count<=12;++count){
   std::string keys="d"+std::string(count,vowel)+"m";
   auto expected="d"+std::string(count-1,vowel)+"m";
   check(keys,std::u32string(expected.begin(),expected.end()));
   keys=std::string(count,vowel);
   expected=std::string(count-1,vowel);
   check(keys,std::u32string(expected.begin(),expected.end()));
  }
  check("doOom",U"doOm");
  check("dooom",U"doom");
  check("toom",U"tôm");check("coom",U"côm");
  check("dieenj",U"diện");
  return 0;
 }

 if(argc>1&&std::string(argv[1])=="foreign"){
  for(auto word:{"zoom","Zoom","ZOOM","zoo","zees","food","FOOD","jazz","free","wrong","data","coco"}){
   std::u32string expected;for(char c:std::string(word))expected+=c;
   check(word,expected);
  }
  return 0;
 }
 if(argc>1&&std::string(argv[1])=="late-shape"){
  for(auto pair:{std::pair{"dienej",U"diện"},{"dieenj",U"diện"},{"dienje",U"diện"},
   {"DIENEJ",U"DIỆN"},{"tienges",U"tiếng"},{"biener",U"biển"},
   {"yenes",U"yến"},{"toocs",U"tốc"},{"nghienge",U"nghiêng"},{"khuono",U"khuôn"},{"gisenge",U"giếng"}})check(pair.first,pair.second);
  return 0;
 }

 check("conss",U"cons");
 check("conssole",U"console");
 check("ConssOle",U"ConsOle");
 check("CONSSOLE",U"CONSOLE");
 check("conssoles",U"consoles");
 check("conssolidate",U"consolidate");
 check("console",U"console");check("Console",U"Console");check("consoles",U"consoles");
 for(auto pair:{std::pair{"asso",U"aso"},{"affo",U"afo"},{"arro",U"aro"},{"axxo",U"axo"},{"ajjo",U"ajo"}})check(pair.first,pair.second);
 check("tieengs",U"tiếng");check("suarw",U"sửa");
}
