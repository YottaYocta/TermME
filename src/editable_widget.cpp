#include "editable_widget.h"

namespace TermME
{
  
  // public

  editable_widget::editable_widget(std::reference_wrapper<std::vector<std::string>> content) : content_widget {content},
    line_num {0},
    cursor_pos {0},
    line_num_offset {1} {}

  ftxui::Element editable_widget::Render()
  {
    std::vector<std::string> temp {content_ref.get()};
    std::string nu_fmt {" {:>} "};
    nu_fmt.insert(4, std::to_string(line_num_offset));
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
    return ftxui::window(ftxui::text("edit mode"), ftxui::vbox(ftxui::vbox(lines) | ftxui::reflect(box) | ftxui::frame, ftxui::filler()) | ftxui::reflect(box) | ftxui::color(ftxui::Color::GrayLight)) | ftxui::color(ftxui::Color::GrayDark);
  }

  bool editable_widget::OnEvent(ftxui::Event e)
  {
    if (e.is_mouse())
      return OnMouseEvent(e);

    if (e == ftxui::Event::Backspace)
    {
      if (cursor_pos >= 2 && 
          content_ref.get()[line_num][cursor_pos - 1] == ' ' && 
          content_ref.get()[line_num][cursor_pos - 2] == ' ')
      {
        untab();
        return true;
      }
      else
      {
        return delete_before();
      }
    }

    if (e == ftxui::Event::Delete)
    {
      return delete_after();
    }

    if (e == ftxui::Event::ArrowLeft)
    {
      move_cursor_left(1);
      return true;
    }

    if (e == ftxui::Event::ArrowRight)
    {
      move_cursor_right(1);
      return true;
    }

    if (e == ftxui::Event::ArrowUp)
    {
      move_cursor_up(1);
      return true;
    }

    if (e == ftxui::Event::ArrowDown)
    {
      move_cursor_down(1);
      return true;
    }

    if (e == ftxui::Event::Return)
    {
      std::string cut {content_ref.get()[line_num].substr(cursor_pos)};
      content_ref.get()[line_num].resize(cursor_pos);
      content_ref.get().insert(content_ref.get().begin() + line_num + 1, cut);
      line_num_offset = std::to_string(content_ref.get().size() - 1).size();
      cursor_pos = 0;
      line_num++;
      std::size_t spaces {content_ref.get()[line_num - 1].find_first_not_of(' ')}; 
      if (spaces == std::string::npos)
        spaces = 0;
      for (int i {0}; i < spaces / 2; i++)
        tab();
      return true;
    }

    if (e == ftxui::Event::Tab)
    {
      if (cursor_pos % 2 == 0)
      {
        tab();
      }
      else
      {
        write_char(' ');
      }
      return true;
    }

    if (e.is_character())
    {
      write_char(e.character());
      return true;
    }

    return false;
  }

  bool editable_widget::OnMouseEvent(ftxui::Event e)
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

  bool editable_widget::OnWheel(ftxui::Event e)
  {
    if (!box.Contain(e.mouse().x, e.mouse().y))
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

  bool editable_widget::OnClick(ftxui::Event e)
  {
    if (!box.Contain(e.mouse().x, e.mouse().y))
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

  // private

  void editable_widget::move_cursor_left(std::size_t chars) 
  {
    cursor_pos -= std::min(cursor_pos, chars);
  }

  void editable_widget::move_cursor_right(std::size_t chars)
  {
    cursor_pos += std::min(content_ref.get()[line_num].size() - cursor_pos, chars);
  }

  void editable_widget::move_cursor_up(std::size_t chars)
  {
    line_num -= std::min(line_num, chars);
    cursor_pos = std::min(content_ref.get()[line_num].size(), cursor_pos);
  }

  void editable_widget::move_cursor_down(std::size_t chars)
  {
    line_num += std::min(content_ref.get().size() - 1 - line_num, chars);
    cursor_pos = std::min(content_ref.get()[line_num].size(), cursor_pos);
  }

  void editable_widget::write_char(char c)
  {
    content_ref.get()[line_num].insert(cursor_pos, std::string {c});
    cursor_pos++;
  }

  void editable_widget::write_char(std::string c)
  {
    content_ref.get()[line_num].insert(cursor_pos, c);
    cursor_pos++;
  }

  bool editable_widget::delete_before()
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
        line_num_offset = std::to_string(content_ref.get().size() - 1).size();
        return true;
      }
      else
        return false;
    }
    
    cursor_pos--;
    content_ref.get()[line_num].erase(cursor_pos, 1);

    return true;
  }

  bool editable_widget::delete_after()
  {
    if (cursor_pos > content_ref.get()[line_num].size() - 1)
      return false;
    content_ref.get()[line_num].erase(cursor_pos, 1);
    return true;
  }

  void editable_widget::tab()
  {
    write_char(' ');
    write_char(' ');
  }

  void editable_widget::untab()
  {
    delete_before(); 
    delete_before(); 
  }
}
