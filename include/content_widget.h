#ifndef CONTENT_WIDGET
#define CONTENT_WIDGET

#include <functional>
#include <string>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>

namespace TermME
{
  class content_widget : public ftxui::ComponentBase
  {
    public:
      content_widget(std::reference_wrapper<std::vector<std::string>> content) : content_ref {content} {}
    protected:
      std::reference_wrapper<std::vector<std::string>> content_ref;
  };
}

#endif
