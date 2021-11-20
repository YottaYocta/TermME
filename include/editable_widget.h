#ifndef EDITABLE_WIDGET_H
#define EDITABLE_WIDGET_H

#include <algorithm>
#include <cstddef>
#include <functional>
#include <string>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <fmt/core.h>

#include "content_widget.h"

namespace TermME
{
  class editable_widget : public content_widget
  {
    public: 
      editable_widget(std::reference_wrapper<std::vector<std::string>> content);
      ftxui::Element Render() override;
      bool OnEvent(ftxui::Event e) override;
      bool OnMouseEvent(ftxui::Event e);
      bool OnWheel(ftxui::Event e);
      bool OnClick(ftxui::Event e);
    private:
      void move_cursor_left(std::size_t chars);
      void move_cursor_right(std::size_t chars);
      void move_cursor_up(std::size_t chars);
      void move_cursor_down(std::size_t chars);
      void write_char(char c);
      void write_char(std::string c);
      bool delete_before();
      bool delete_after();
      void tab();
      void untab();
      
      int line_num_offset;
      std::size_t cursor_pos; 
      std::size_t line_num;
      ftxui::Box box;
      ftxui::Box cursor_box;
  };
}

#endif
