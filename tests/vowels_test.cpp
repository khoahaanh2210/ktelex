#include "telex.hpp"
#include <iostream>
#include <string>
#include <cstdlib>
static void check(std::string keys,std::u32string expected){ktlex::Engine e;for(unsigned char c:keys)e.feed(c);if(e.text()!=expected){std::cerr<<"FAIL keys="<<keys<<"\n";std::exit(1);}}
int main(){
 const std::string keys[]={"a","aw","aa","e","ee","i","o","oo","ow","u","uw","y"};
 const std::u32string lower[]={U"aáàảãạ",U"ăắằẳẵặ",U"âấầẩẫậ",U"eéèẻẽẹ",U"êếềểễệ",U"iíìỉĩị",U"oóòỏõọ",U"ôốồổỗộ",U"ơớờởỡợ",U"uúùủũụ",U"ưứừửữự",U"yýỳỷỹỵ"};
 const std::u32string upper[]={U"AÁÀẢÃẠ",U"ĂẮẰẲẴẶ",U"ÂẤẦẨẪẬ",U"EÉÈẺẼẸ",U"ÊẾỀỂỄỆ",U"IÍÌỈĨỊ",U"OÓÒỎÕỌ",U"ÔỐỒỔỖỘ",U"ƠỚỜỞỠỢ",U"UÚÙỦŨỤ",U"ƯỨỪỬỮỰ",U"YÝỲỶỸỴ"};
 const std::string tones[]={"","s","f","r","x","j"};
 for(int i=0;i<12;++i)for(int t=0;t<6;++t){check(keys[i]+tones[t],{lower[i][t]});auto caps=keys[i];for(auto &c:caps)c-=32;check(caps+tones[t],{upper[i][t]});if(t){check(keys[i]+tones[t]+"z",{lower[i][0]});check(caps+tones[t]+"Z",{upper[i][0]});}}
 for(int t=0;t<6;++t){check("suaw"+tones[t],U"s"+std::u32string(1,lower[10][t])+U"a");check("SUAW"+tones[t],U"S"+std::u32string(1,upper[10][t])+U"A");}
 for(auto k:{"suwar","suwra","suawr","suarw"})check(k,U"sửa");
}
