// RUN: %clang_cc1 -std=c++20 -triple x86_64-unknown-linux-gnu -emit-llvm -o - %s | FileCheck %s

using Size = decltype(sizeof(0));
void *operator new(Size);
void operator delete(void *) noexcept;
void operator delete(void *, Size) noexcept;

struct Noisy {
  Noisy();
  Noisy(const Noisy &);
  ~Noisy();
};

class Function {
public:
  template <typename F>
  explicit Function(F &&f) : callable_{new Callable<F>{static_cast<F &&>(f)}} {}

  ~Function() { delete callable_; }

private:
  struct CallableBase {
    virtual ~CallableBase() = default;
  };

  template <typename F> struct Callable final : CallableBase {
    explicit Callable(F f) : function{static_cast<F &&>(f)} {}

    F function;
  };

  CallableBase *callable_;
};

struct Options {
  Function function{[noisy = Noisy{}] {}};
};

Options kOptions{};

// CHECK-LABEL: define internal void @__cxx_global_var_init
// CHECK: call void @_ZN5NoisyC1Ev
// CHECK: call void @_ZN8FunctionC1IN7Options8functionMUlvE_EEEOT_
// CHECK: call void @_ZN7Options8functionMUlvE_D1Ev

// CHECK-LABEL: define {{.*}} @_ZN7Options8functionMUlvE_D1Ev
// CHECK: call void @_ZN7Options8functionMUlvE_D2Ev

// CHECK-LABEL: define {{.*}} @_ZN7Options8functionMUlvE_D2Ev
// CHECK: call void @_ZN5NoisyD1Ev
