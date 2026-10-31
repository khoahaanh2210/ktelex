#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
namespace ktlex {
class Engine {
 std::u32string value;
 std::u32string raw;
 bool literal=false;
 std::u32string beforeShape;
 std::u32string deferredShape;
 char32_t shapeKey=0,shapeLetter=0;
 const std::vector<std::u32string> rows={U"aáàảãạ",U"ăắằẳẵặ",U"âấầẩẫậ",U"eéèẻẽẹ",U"êếềểễệ",U"iíìỉĩị",U"oóòỏõọ",U"ôốồổỗộ",U"ơớờởỡợ",U"uúùủũụ",U"ưứừửữự",U"yýỳỷỹỵ"};
 const std::vector<std::u32string> caps={U"AÁÀẢÃẠ",U"ĂẮẰẲẴẶ",U"ÂẤẦẨẪẬ",U"EÉÈẺẼẸ",U"ÊẾỀỂỄỆ",U"IÍÌỈĨỊ",U"OÓÒỎÕỌ",U"ÔỐỒỔỖỘ",U"ƠỚỜỞỠỢ",U"UÚÙỦŨỤ",U"ƯỨỪỬỮỰ",U"YÝỲỶỸỴ"};
 int row(char32_t c)const{for(unsigned i=0;i<rows.size();++i)if(rows[i].find(c)!=std::u32string::npos||caps[i].find(c)!=std::u32string::npos)return i;return -1;}
 bool uppercase(char32_t c,int r)const{return caps[r].find(c)!=std::u32string::npos;}
 size_t mark(char32_t c,int r)const{return (uppercase(c,r)?caps[r]:rows[r]).find(c);}
 char32_t shaped(char32_t c,int dest)const{int r=row(c);return (uppercase(c,r)?caps[dest]:rows[dest])[mark(c,r)];}
 int currentTone()const{int t=0;for(auto x:value){int r=row(x);if(r>=0&&mark(x,r))t=int(mark(x,r));}return t;}
 void tone(int t){std::vector<size_t> v;for(size_t i=0;i<value.size();++i)if(row(value[i])>=0)v.push_back(i);if(v.empty())return;if(v.size()>1&&v[0]>0&&(((value[v[0]-1]==U'q'||value[v[0]-1]==U'Q')&&row(value[v[0]])==9)||((value[v[0]-1]==U'g'||value[v[0]-1]==U'G')&&row(value[v[0]])==5))){
 // A tone entered before the following vowel must leave the gi/qu
 // semivowel when the syllable gains its real tone-bearing vowel.
 int r=row(value[v[0]]);bool upper=uppercase(value[v[0]],r);
 value[v[0]]=(upper?caps[r]:rows[r])[0];
 v.erase(v.begin());
 }size_t pos=v[0];for(auto i:v){int r=row(value[i]);if(r==4||r==8)pos=i;}if(pos==v[0]&&v.size()>1){if(v.size()>2)pos=v[1];else if(v.back()+1<value.size())pos=v.back();}for(auto i:v){int r=row(value[i]);bool upper=uppercase(value[i],r);value[i]=(upper?caps[r]:rows[r])[i==pos?t:0];}}
public:
 const std::u32string& text()const{return value;}
 void reset(){value.clear();raw.clear();literal=false;shapeKey=0;beforeShape.clear();deferredShape.clear();}
 std::u32string commit(){auto s=value;reset();return s;}
 void backspace(){
  shapeKey=0;beforeShape.clear();deferredShape.clear();
  if(!value.empty())value.pop_back();
  if(!literal){int t=currentTone();if(t)tone(t);}
  raw=value;if(value.empty())literal=false;
 }
 void feed(char32_t c){
 raw+=c;
 auto lower=[](char32_t x){return x>=U'A'&&x<=U'Z'?x+32:x;};
 if(!deferredShape.empty()){
  auto k=lower(c);
  if(std::u32string(U"sfrxj").find(k)!=std::u32string::npos||k==shapeKey){
   value=std::move(deferredShape);deferredShape.clear();
  }else{
   deferredShape.clear();shapeKey=0;beforeShape.clear();
   value=raw;literal=true;return;
  }
 }
 if(literal){value+=c;return;}
 // Repeating a shape modifier escapes only that transformation. Keep
 // earlier composition (such as đ or an existing tone) intact.
 if(shapeKey&&lower(c)==shapeKey){
  value=beforeShape;value+=shapeLetter;raw=value;literal=true;
  shapeKey=0;beforeShape.clear();return;
 }
 shapeKey=0;beforeShape.clear();
 // Foreign consonant clusters cannot begin a Vietnamese syllable.
 // Decide before consuming any Telex modifiers so the raw spelling survives.
 if(row(c)>=0&&std::none_of(value.begin(),value.end(),[&](auto x){return row(x)>=0;})){
  auto onset=value;
  for(auto &x:onset)if(x>=U'A'&&x<=U'Z')x+=32;else if(x==U'Đ')x=U'đ';
  const std::vector<std::u32string> initials={U"",U"b",U"c",U"ch",U"d",U"đ",U"g",U"gh",U"h",U"k",U"kh",U"l",U"m",U"n",U"ng",U"ngh",U"nh",U"p",U"ph",U"q",U"r",U"s",U"t",U"th",U"tr",U"v",U"x"};
  if(std::find(initials.begin(),initials.end(),onset)==initials.end()){value=raw;literal=true;return;}
 }
 // Circumflex keys may follow the coda, including a single vowel:
 // nhap+a+j -> nhập, ben+e -> bên, ton+o -> tôn.
 bool lateShape=false,lateStop=false;
 size_t glidePosition=value.size();
 for(size_t n=value.size();n-->0;){
  int r=row(value[n]);if(r<0)continue;
  auto ending=value.substr(n+1);
  for(auto &x:ending)x=lower(x);
  bool coda=ending==U"c"||ending==U"ch"||ending==U"m"||ending==U"n"||ending==U"ng"||ending==U"nh"||ending==U"p"||ending==U"t";
  lateStop=ending==U"c"||ending==U"ch"||ending==U"p"||ending==U"t";
  size_t start=n;while(start>0&&row(value[start-1])>=0)--start;
  // gi/qu contribute a consonantal glide before the actual nucleus.
  if(start<n&&start>0&&((lower(value[start-1])==U'g'&&row(value[start])==5)||(lower(value[start-1])==U'q'&&row(value[start])==9)))++start;
  std::u32string nucleus;
  for(size_t i=start;i<=n;++i)nucleus+=rows[row(value[i])][0];
  auto k=lower(c);
  // In these vowel groups the final i/y/u is a glide, not the vowel
  // modified by a/e/o/w: day+a -> dây, toi+o -> tôi, gui+w+r -> gửi.
  bool throughGlide=(k==U'a'&&(nucleus==U"ay"||nucleus==U"au"||nucleus==U"uay"))||
                    (k==U'e'&&(nucleus==U"eu"||nucleus==U"ieu"||nucleus==U"yeu"||nucleus==U"ueu"))||
                    (k==U'o'&&(nucleus==U"oi"||nucleus==U"uoi"))||
                    (k==U'w'&&(nucleus==U"oi"||nucleus==U"uoi"||nucleus==U"ưoi"||nucleus==U"ui"||nucleus==U"uu"||nucleus==U"uou"||nucleus==U"ưou"));
  if(ending.empty()&&throughGlide)glidePosition=n;
  lateShape=coda&&((k==U'a'&&(nucleus==U"a"||nucleus==U"ua"))||
                  (k==U'e'&&(nucleus==U"e"||nucleus==U"ue"||nucleus==U"ie"||nucleus==U"ye"||nucleus==U"uye"))||
                  (k==U'o'&&(nucleus==U"o"||nucleus==U"uo")));
  break;
 }
 // A vowel following a completed consonant ending starts a second
 // syllable: restore the exact keystrokes instead of changing an
 // earlier vowel across that consonant (e.g. con+s+o in console).
 if(row(c)>=0&&!value.empty()&&row(value.back())<0){
  bool prior=false;
  for(size_t i=0;i+1<value.size();++i)if(row(value[i])>=0)prior=true;
  bool initialGi=value==U"gi"||value==U"Gi"||value==U"GI";
  if(prior&&!initialGi&&!lateShape){value=raw;literal=true;return;}
 }
 char32_t k=c;if(k>=U'A'&&k<=U'Z')k+=32;auto ts=std::u32string(U"sfrxjz").find(k);if(ts!=std::u32string::npos&&std::any_of(value.begin(),value.end(),[&](auto x){return row(x)>=0;})){int t=ts==5?0:int(ts)+1;bool same=false;for(auto x:value){int r=row(x);if(r>=0&&mark(x,r)==size_t(t)&&t)same=true;}tone(same?0:t);if(same){
 // The repeated tone key escapes the preceding modifier. Keep only
 // one literal key in the restoration buffer, otherwise a later
 // fallback resurrects the escape key (conss+ole -> conssole).
 value+=c;
 raw.pop_back();
 }return;}
 if(!value.empty()){auto &last=value.back();if(k==U'd'&&(last==U'd'||last==U'D')){last=last==U'D'?U'Đ':U'đ';return;}if(k==U'd'&&(last==U'đ'||last==U'Đ')){last=last==U'Đ'?U'D':U'd';value+=c;raw.pop_back();return;}
 for(size_t n=value.size();n-->0;){if(n==glidePosition)continue;int r=row(value[n]),dest=-1;if(k==U'w'){if(r==0)dest=1;if(r==6)dest=8;if(r==9)dest=10;}if(k==U'a'&&r==0)dest=2;if(k==U'e'&&r==3)dest=4;if(k==U'o'&&r==6)dest=7;if(dest>=0){
 beforeShape=value;shapeKey=k;shapeLetter=c;
 // ua+w forms ưa, preserving a tone entered before w.
 bool precedingQu=n>1&&(value[n-2]==U'q'||value[n-2]==U'Q');
 if(k==U'w'&&r==0&&n>0&&row(value[n-1])==9&&!precedingQu){value[n-1]=shaped(value[n-1],10);return;}
 int existingTone=currentTone();
 value[n]=shaped(value[n],dest);
 // The u in qu is a consonantal glide, not part of the ươ pair.
 if(dest==8&&n>0&&row(value[n-1])==9&&!precedingQu)value[n-1]=shaped(value[n-1],10);
 if(existingTone)tone(existingTone);
 // An unmarked stop-coda syllable is ambiguous (data/coco). Keep its
 // literal spelling until a tone key confirms the late transformation.
 if(lateShape&&lateStop&&!existingTone){deferredShape=value;value=beforeShape;value+=c;}
 return;}if(r>=0)break;}}
 // A new vowel or final consonant can change the tone-bearing vowel.
 int existingTone=currentTone();
 value+=c;
 if(existingTone)tone(existingTone);
 }
};
}
