#include <functional>
#include <memory>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <ctime>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <fmt/core.h>

namespace termed
{

  class editable : public ftxui::ComponentBase
  {
    public:
      editable(std::reference_wrapper<std::vector<std::string>> content) : content_ref {content},
                                                              line_num {0},
                                                              cursor_pos {0},
                                                              max_line_num_length {1} {}

      ftxui::Element Render() override
      {
        std::vector<std::string> temp {content_ref.get()};
        std::string nu_fmt {" {:>} "};
        nu_fmt.insert(4, std::to_string(max_line_num_length));
        ftxui::Elements lines (temp.size()); 
        for (std::size_t i {0}; i < temp.size(); i++)
        {
          if (i == line_num)
          {
            temp[i].insert(temp[i].size(), " ");
            lines[i] = ftxui::hbox(
              ftxui::text(fmt::format(fmt::runtime(nu_fmt), std::to_string(i))) | ftxui::color(ftxui::Color::Red),
              ftxui::text(temp[i].substr(0, cursor_pos)),
              ftxui::text(temp[i].substr(cursor_pos, 1)) | ftxui::inverted | ftxui::focus | ftxui::reflect(cursor_box),
              ftxui::text(temp[i].substr(std::min(temp[i].size(), cursor_pos + 1)))
            );
          }
          else
          {
            lines[i] = ftxui::hbox(
              ftxui::text(fmt::format(fmt::runtime(nu_fmt), std::to_string(i))) | ftxui::color(ftxui::Color::Red),
              ftxui::text(temp[i])
            );
          }
        }
        return ftxui::vbox(ftxui::vbox(lines) | ftxui::reflect(click_box) | ftxui::frame, ftxui::filler()) | ftxui::reflect(scroll_box) | ftxui::border;
      }

      bool OnEvent(ftxui::Event e) override
      {
        if (e.is_mouse())
          return OnMouseEvent(e);

        if (e == ftxui::Event::Backspace)
        {
          if (cursor_pos <= 0)
          {
            if (line_num > 0)
            {
              std::string cur_line {content_ref.get()[line_num]};
              content_ref.get().erase(content_ref.get().begin() + line_num);
              line_num--;
              cursor_pos = content_ref.get()[line_num].size();
              content_ref.get()[line_num] += cur_line;
              max_line_num_length = std::to_string(content_ref.get().size()).size();
              return true;
            }
            else
              return false;
          }
          cursor_pos--;
          content_ref.get()[line_num].erase(cursor_pos, 1);
          return true;
        }
        
        if (e == ftxui::Event::Delete)
        {
          if (cursor_pos > content_ref.get()[line_num].size() - 1)
            return false;
          content_ref.get()[line_num].erase(cursor_pos, 1);
          return true;
        }

        if (e == ftxui::Event::ArrowLeft)
        {
          if (cursor_pos <= 0)
            return false;
          cursor_pos--;
          return true;
        }

        if (e == ftxui::Event::ArrowRight)
        {
          if (cursor_pos > content_ref.get()[line_num].size() - 1)
            return false;
          cursor_pos++;
          return true;
        }

        if (e == ftxui::Event::ArrowUp)
        {
          if (line_num <= 0)
            return false;
          line_num--;
          cursor_pos = std::min(content_ref.get()[line_num].size(), cursor_pos);
          return true;
        }

        if (e == ftxui::Event::ArrowDown)
        {
          if (line_num >= content_ref.get().size() - 1)
            return false;
          line_num++;
          cursor_pos = std::min(content_ref.get()[line_num].size(), cursor_pos);
          return true;
        }

        if (e == ftxui::Event::Return)
        {
          std::string cut {content_ref.get()[line_num].substr(cursor_pos)};
          content_ref.get()[line_num].resize(cursor_pos);
          content_ref.get().insert(content_ref.get().begin() + line_num + 1, cut);
          max_line_num_length = std::to_string(content_ref.get().size()).size();
          cursor_pos = 0;
          line_num++;
          return true;
        }

        if (e.is_character())
        {
          content_ref.get()[line_num].insert(cursor_pos, e.character());
          cursor_pos++;
          return true;
        }

        return false;
      }

      bool OnMouseEvent(ftxui::Event e)
      {
        if (e.mouse().button == ftxui::Mouse::WheelDown || 
            e.mouse().button == ftxui::Mouse::WheelUp)
          return OnWheel(e);
        if ((e.mouse().button == ftxui::Mouse::Left || 
            e.mouse().button == ftxui::Mouse::Right) && 
            e.mouse().motion == ftxui::Mouse::Pressed)
          return OnClick(e);
        return false;
      }

      bool OnWheel(ftxui::Event e)
      {
        if (!scroll_box.Contain(e.mouse().x, e.mouse().y))
          return false;
        if (e.mouse().button == ftxui::Mouse::WheelDown)
        {
          move_cursor_down(1);
          return true;
        }
        if (e.mouse().button == ftxui::Mouse::WheelUp)
        {
          move_cursor_up(1);
          return true;
        }

        return false;
      }

      bool OnClick(ftxui::Event e)
      {
        if (!click_box.Contain(e.mouse().x, e.mouse().y))
          return false;

        TakeFocus();

        if (e.mouse().button == ftxui::Mouse::Left)
        {
          line_num = std::max(static_cast<std::size_t>(0), std::min(content_ref.get().size(), static_cast<std::size_t>(e.mouse().y) - cursor_box.y_min + line_num));
          cursor_pos = std::max(static_cast<std::size_t>(0), std::min(content_ref.get()[line_num].size(), static_cast<std::size_t>(e.mouse().x) - cursor_box.x_min  + cursor_pos));
          return true;
        }

        return false;
      }

    private:

      void move_cursor_left(std::size_t chars) 
      {
        cursor_pos -= std::min(cursor_pos, chars);
      }

      void move_cursor_right(std::size_t chars)
      {
        cursor_pos += std::min(content_ref.get()[line_num].size() - 1 - cursor_pos, chars);
      }

      void move_cursor_up(std::size_t chars)
      {
        line_num -= std::min(line_num, chars);
        cursor_pos = std::min(content_ref.get()[line_num].size(), cursor_pos);
      }
      
      void move_cursor_down(std::size_t chars)
      {
        line_num += std::min(content_ref.get().size() - 1 - line_num, chars);
        cursor_pos = std::min(content_ref.get()[line_num].size(), cursor_pos);
      }

      std::reference_wrapper<std::vector<std::string>> content_ref;
      int max_line_num_length {};
      std::size_t cursor_pos; 
      std::size_t line_num;
      ftxui::Box scroll_box;
      ftxui::Box click_box;
      ftxui::Box cursor_box;
  };
}

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

  
  ftxui::Component edit_pane {std::make_shared<termed::editable>(content)};
  ftxui::Component renderer {ftxui::Renderer(edit_pane, [&]{
    return edit_pane->Render();
  })};

  scr.Loop(renderer);

  for (int i {0}; i < content.size(); i++)
    fio << content[i] << '\n';

  fio.close();

  return 0;
}
