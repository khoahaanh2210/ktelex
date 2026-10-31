#pragma once
#include <string>
#include <vector>
#include <algorithm>
namespace ktlex {
class Engine {
 std::u32string value;
 std::u32string raw;
 bool literal=false;
 const std::vector<std::u32string> rows={U"aáàảãạ",U"ăắằẳẵặ",U"âấầẩẫậ",U"eéèẻẽẹ",U"êếềểễệ",U"iíìỉĩị",U"oóòỏõọ",U"ôốồổỗộ",U"ơớờởỡợ",U"uúùủũụ",U"ưứừửữự",U"yýỳỷỹỵ"};
 const std::vector<std::u32string> caps={U"AÁÀẢÃẠ",U"ĂẮẰẲẴẶ",U"ÂẤẦẨẪẬ",U"EÉÈẺẼẸ",U"ÊẾỀỂỄỆ",U"IÍÌỈĨỊ",U"OÓÒỎÕỌ",U"ÔỐỒỔỖỘ",U"ƠỚỜỞỠỢ",U"UÚÙỦŨỤ",U"ƯỨỪỬỮỰ",U"YÝỲỶỸỴ"};
 int row(char32_t c)const{for(unsigned i=0;i<rows.size();++i)if(rows[i].find(c)!=std::u32string::npos||caps[i].find(c)!=std::u32string::npos)return i;return -1;}
 bool uppercase(char32_t c,int r)const{return caps[r].find(c)!=std::u32string::npos;}
 size_t mark(char32_t c,int r)const{return (uppercase(c,r)?caps[r]:rows[r]).find(c);}
 char32_t shaped(char32_t c,int dest)const{int r=row(c);return (uppercase(c,r)?caps[dest]:rows[dest])[mark(c,r)];}
 void tone(int t){std::vector<size_t> v;for(size_t i=0;i<value.size();++i)if(row(value[i])>=0)v.push_back(i);if(v.empty())return;if(v.size()>1&&v[0]>0&&((value[v[0]-1]==U'q'&&row(value[v[0]])==9)||(value[v[0]-1]==U'g'&&row(value[v[0]])==5)))v.erase(v.begin());size_t pos=v[0];for(auto i:v){int r=row(value[i]);if(r==4||r==8)pos=i;}if(pos==v[0]&&v.size()>1){if(v.size()>2)pos=v[1];else if(v.back()+1<value.size())pos=v.back();}for(auto i:v){int r=row(value[i]);bool upper=uppercase(value[i],r);value[i]=(upper?caps[r]:rows[r])[i==pos?t:0];}}
public:
 const std::u32string& text()const{return value;}
 void reset(){value.clear();raw.clear();literal=false;}
 std::u32string commit(){auto s=value;reset();return s;}
 void backspace(){if(!value.empty())value.pop_back();raw=value;}
 void feed(char32_t c){
 raw+=c;
 if(literal){value+=c;return;}
 // A vowel following a completed consonant ending starts a second
 // syllable: restore the exact keystrokes instead of changing an
 // earlier vowel across that consonant (e.g. con+s+o in console).
 if(row(c)>=0&&!value.empty()&&row(value.back())<0){
  bool prior=false;
  for(size_t i=0;i+1<value.size();++i)if(row(value[i])>=0)prior=true;
  bool initialGi=value==U"gi"||value==U"Gi"||value==U"GI";
  if(prior&&!initialGi){value=raw;literal=true;return;}
 }
 char32_t k=c;if(k>=U'A'&&k<=U'Z')k+=32;auto ts=std::u32string(U"sfrxjz").find(k);if(ts!=std::u32string::npos&&std::any_of(value.begin(),value.end(),[&](auto x){return row(x)>=0;})){int t=ts==5?0:int(ts)+1;bool same=false;for(auto x:value){int r=row(x);if(r>=0&&mark(x,r)==size_t(t)&&t)same=true;}tone(same?0:t);if(same){
 // The repeated tone key escapes the preceding modifier. Keep only
 // one literal key in the restoration buffer, otherwise a later
 // fallback resurrects the escape key (conss+ole -> conssole).
 value+=c;
 raw.pop_back();
 }return;}
 if(!value.empty()){auto &last=value.back();if(k==U'd'&&(last==U'd'||last==U'D')){last=last==U'D'?U'Đ':U'đ';return;}if(k==U'd'&&(last==U'đ'||last==U'Đ')){last=last==U'Đ'?U'D':U'd';value+=c;return;}
 for(size_t n=value.size();n-->0;){int r=row(value[n]),dest=-1;if(k==U'w'){if(r==0)dest=1;if(r==6)dest=8;if(r==9)dest=10;}if(k==U'a'&&r==0)dest=2;if(k==U'e'&&r==3)dest=4;if(k==U'o'&&r==6)dest=7;if(dest>=0){// ua+w forms ưa, preserving a tone entered before w.
 if(k==U'w'&&r==0&&n>0&&row(value[n-1])==9){value[n-1]=shaped(value[n-1],10);return;}
 value[n]=shaped(value[n],dest);
 if(dest==8&&n>0&&row(value[n-1])==9)value[n-1]=shaped(value[n-1],10);
 return;}if(r>=0)break;}}
 value+=c;
 }
};
}
