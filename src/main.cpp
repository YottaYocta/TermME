#include <functional>
#include <memory>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <ctime>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <fmt/core.h>

#include "editable_widget.h"

int main(int argc, char* argv[])
{
  ftxui::ScreenInteractive scr {ftxui::ScreenInteractive::Fullscreen()};
  
  std::vector<std::string> content {};
  std::string temp {};

  std::ifstream fin;
  std::ofstream fout;
  
  // opening file

  if (argc > 1)
  {
    // read
    fin.open(argv[1]);
    content.push_back("");

    while (std::getline(fin, temp))
      content.push_back(temp);

    fin.close();
    fout.open(argv[1]);
  }
  else
  {
    time_t now {std::time(0)};
    tm* localtm {std::localtime(&now)};
    std::string timestr {std::string(asctime(localtm))};
    timestr.pop_back();
    content.push_back("");
    fout.open(timestr + ".md");
  }

  
  ftxui::Component edit_pane {std::make_shared<TermME::editable_widget>(content)};
  ftxui::Component renderer {ftxui::Renderer(edit_pane, [&]{
    return edit_pane->Render();
  })};

  scr.Loop(renderer);

  for (int i {0}; i < content.size(); i++)
    fout << content[i] << '\n';

  fout.close();

  return 0;
}
