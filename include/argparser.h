#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>

#include "util.h"

namespace pl0 {

template<class TargetType>
class ArgumentParser {
  class Handler {
    int arg_count_;

   public:
    explicit Handler(int arg_count) : arg_count_{arg_count} {}
    virtual ~Handler() = default;

    virtual void apply(
        const std::vector<std::string> &args, TargetType &target) = 0;

    [[nodiscard]] int arg_count() const { return arg_count_; }
  };

  template<typename PropertyType, typename Formatter>
  class StoreHandler : public Handler {
    PropertyType TargetType::*setter_;

    Formatter formatter_;

   public:
    StoreHandler(PropertyType TargetType::*setter, Formatter formatter)
        : Handler(1), setter_{setter}, formatter_{std::move(formatter)} {}
    virtual ~StoreHandler() = default;

    void apply(const std::vector<std::string> &args, TargetType &target) final {
      target.*setter_ = formatter_(args[0]);
    }
  };

  template<typename PropertyType>
  class StoreHandler<PropertyType, void> : public Handler {
    PropertyType TargetType::*setter_;

   public:
    explicit StoreHandler(PropertyType TargetType::*setter)
        : Handler(1), setter_{setter} {}
    virtual ~StoreHandler() = default;

    void apply(const std::vector<std::string> &args, TargetType &target) final {
      target.*setter_ = args[0];
    }
  };

  class FlagHandler : public Handler {
    bool TargetType::*setter_;

    bool store_value_;

   public:
    explicit FlagHandler(bool TargetType::*setter, bool store_value)
        : Handler(0), setter_{setter}, store_value_{store_value} {}
    virtual ~FlagHandler() = default;

    void apply(
        const std::vector<std::string> & /*args*/, TargetType &target) final {
      target.*setter_ = store_value_;
    }
  };

  template<typename Callable>
  class CustomHandler : public Handler {
    Callable callable_;

   public:
    explicit CustomHandler(Callable callable)
        : Handler(0), callable_{std::move(callable)} {}
    virtual ~CustomHandler() = default;

    void apply(
        const std::vector<std::string> & /*args*/,
        TargetType & /*target*/) final {
      callable_();
    }
  };

  class Option {
    std::vector<std::string> arg_names_;
    std::string description_;
    Handler *the_handler_;

   public:
    template<typename IterableStringList>
    Option(
        IterableStringList arg_names,
        std::string description,
        Handler *the_handler)
        : description_{std::move(description)}, the_handler_{the_handler} {
      for (const auto &arg_name : arg_names) {
        arg_names_.push_back(std::string{arg_name});
      }
    }

    void ShowHelp(std::ostream &out) {
      out << "    ";
      for (const auto &arg_name : arg_names_) { out << arg_name << ' '; }
      out.put('\n');
      out << "        " << description_ << '\n';
    }
  };

  std::unordered_map<std::string, Handler *> handlers_;
  std::vector<Option *> all_options_;
  std::string description_;

  template<typename IterableStringList>
  inline void AddHandler(
      IterableStringList list, std::string description, Handler *the_handler) {
    for (auto name : list) { handlers_[name] = the_handler; }
    all_options_.push_back(
        new Option{list, std::move(description), the_handler});
  }

  static bool IsFlag(const std::string &flag) {
    if (flag.length() > 1 && flag[0] == '-') {
      size_t i = flag[1] == '-' ? 1 : 0;
      do {
        i++;
        while (i < flag.length() && isalnum(flag[i])) { i++; }
      } while (i < flag.length() && (flag[i] == '-' || flag[i] == '_'));
      return i == flag.length();
    }
    return false;
  }

 public:
  explicit ArgumentParser(std::string description)
      : description_{std::move(description)} {}

  ~ArgumentParser() {
    //        for (auto pair : handlers_)
    //            delete pair.second;
    for (auto opt : all_options_) { delete opt; }
  }

  [[noreturn]] void ShowHelp() {
    std::cout << description_ << '\n';
    std::cout << "Options:\n";
    for (auto opt : all_options_) { opt->ShowHelp(std::cout); }
    exit(EXIT_SUCCESS);
  }

  template<typename IterableStringList>
  void Store(
      IterableStringList flags,
      std::string description,
      std::string TargetType::*setter) {
    AddHandler(
        flags, std::move(description),
        new StoreHandler<std::string, void>(setter));
  }

  template<
      typename IterableStringList,
      typename PropertyType,
      typename Formatter>
  void Store(
      IterableStringList flags,
      std::string description,
      PropertyType TargetType::*setter,
      Formatter formatter) {
    AddHandler(
        flags, std::move(description),
        new StoreHandler<PropertyType, Formatter>(setter, formatter));
  };

  void Flags(
      const std::vector<std::string> &flags,
      std::string description,
      bool TargetType::*setter,
      bool default_value = true) {
    AddHandler(
        flags, std::move(description), new FlagHandler(setter, default_value));
  }

  void Parse(
      int argc,
      const char *argv[],
      TargetType &target,
      std::vector<std::string> &rest) {
    if (argc < 2) { ShowHelp(); }

    rest.clear();

    std::queue<std::string> args;
    for (int i = 1; i < argc; i++) { args.emplace(argv[i]); }

    while (!args.empty()) {
      std::string arg = args.front();
      args.pop();

      if (IsFlag(arg)) {
        auto result = handlers_.find(arg);
        if (result == handlers_.end()) {
          throw BasicError("unsupported option '" + arg + '\'');
        }
        Handler *the_handler = result->second;

        std::vector<std::string> params;
        for (int i = 0; i < the_handler->arg_count(); i++) {
          if (args.empty()) {
            throw BasicError(
                arg + " requires " + std::to_string(the_handler->arg_count()));
          }
          params.push_back(args.front());
          args.pop();
        }
        the_handler->apply(params, target);
      } else {
        rest.push_back(arg);
      }
    }
  }
};

} // namespace pl0

#endif
