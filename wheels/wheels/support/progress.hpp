#pragma once

#include <wheels/core/assert.hpp>

#include <string>
#include <cstdlib>
#include <sstream>
#include <iostream>

namespace wheels {

class ProgressBar {
 public:
  struct Options {
    bool bar;
    char fill_char;
    size_t width;
    bool redraw;
  };

 public:
  ProgressBar(const std::string& name, Options options = {true, '#', 50, true})
      : name_(name), options_(options) {
  }

  void Start(size_t total) {
    total_ = total;
    progress_ = 0;
    Draw();
  }

  void MakeProgress() {
    WHEELS_VERIFY(progress_ < total_, "Overflow");

    ++progress_;
    if (Percents(progress_ - 1, total_) < Percents(progress_, total_)) {
      Draw();
    }
  }

  void Complete() {
    std::cout << std::endl;
  }

 private:
  static size_t Points(size_t part, size_t total, size_t width) {
    return (part * width) / total;
  }

  static size_t Percents(size_t progress, size_t total) {
    return progress * 100 / total;
  }

  void Draw() {
    if (options_.bar) {
      DrawProgressBar();
    } else {
      WriteProgress();
    }
  }

  void WriteProgress() {
    size_t percents = Percents(progress_, total_);

    if (options_.redraw) {
      std::cout << '\r';
    }

    std::cout << name_ << ": " << percents << '%' << " (" << progress_
              << ")";

    if (!options_.redraw) {
      std::cout << std::endl;
    }

    std::cout.flush();
  }

  void DrawProgressBar() {
    size_t percents = Percents(progress_, total_);

    size_t fill = Points(progress_, total_, options_.width);
    size_t left = options_.width - fill;

    std::stringstream out;

    if (options_.redraw) {
      out << '\r';
    }

    out << name_ << ": |" << std::string(fill, options_.fill_char)
        << std::string(left, '-') << "| " << percents << "% Complete";

    std::cout << out.str();
  }

 private:
  const std::string name_;
  const Options options_;

  size_t progress_{0};
  size_t total_;
};

}  // namespace wheels
