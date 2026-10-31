#include "telex.hpp"
#include <cassert>
int main(){for(auto p:{std::pair{"console",U"console"},{"Console",U"Console"},{"consoles",U"consoles"},{"hoaf",U"hòa"},{"quas",U"quá"},{"gias",U"giá"},{"tieengs",U"tiếng"},{"Vieetj",U"Việt"},{"ddawng",U"đăng"},{"dduwowngf",U"đường"}}){ktlex::Engine e;for(char c:std::string(p.first))e.feed(c);assert(e.text()==p.second);assert(e.commit()==p.second);assert(e.text().empty());for(char c:std::string("ddawng"))e.feed(c);assert(e.text()==U"đăng");} }
