#include "telex.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
static void check(const std::string&keys,const std::u32string&expected){ktlex::Engine e;for(unsigned char c:keys)e.feed(c);if(e.text()!=expected){std::cerr<<"FAIL: "<<keys<<'\n';std::exit(1);}if(e.commit()!=expected||!e.text().empty())std::exit(1);}
int main(){
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
