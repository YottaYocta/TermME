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

  std::fstream fio;
  if (argc > 1)
  {
    fio.open(argv[1], std::ios::in);
    if (!fio.is_open()) 
    {
      fio.open(argv[1], std::ios::out);
      content.push_back("");
    }
    else
    {
      while (std::getline(fio, temp))
        content.push_back(temp);
      fio.open(argv[1], std::ios::out);
    }
  }
  else
  {
    time_t now {std::time(0)};
    tm* localtm {std::localtime(&now)};
    std::string timestr {std::string(asctime(localtm))};
    timestr.pop_back();
    fio.open(timestr + ".md", std::ios::out);
    content.push_back("");
  }

  
  ftxui::Component edit_pane {std::make_shared<TermME::editable_widget>(content)};
  ftxui::Component renderer {ftxui::Renderer(edit_pane, [&]{
    return edit_pane->Render();
  })};

  scr.Loop(renderer);

  for (int i {0}; i < content.size(); i++)
    fio << content[i] << '\n';

  fio.close();

  return 0;
}
