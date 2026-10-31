#include "telex.hpp"
#include <cstdlib>
#include <iostream>
static void require(bool ok){if(!ok){std::cerr<<"core composition/reset regression\n";std::exit(1);}}
int main(){for(auto p:{std::pair{"console",U"console"},{"Console",U"Console"},{"consoles",U"consoles"},{"hoaf",U"hòa"},{"quas",U"quá"},{"gias",U"giá"},{"tieengs",U"tiếng"},{"Vieetj",U"Việt"},{"ddawng",U"đăng"},{"dduwowngf",U"đường"}}){ktlex::Engine e;for(char c:std::string(p.first))e.feed(c);require(e.text()==p.second);require(e.commit()==p.second);require(e.text().empty());for(char c:std::string("ddawng"))e.feed(c);require(e.text()==U"đăng");} }
