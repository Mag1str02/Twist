#include "thread.hpp"
#include "mutex.hpp"
#include "shared_mutex.hpp"
#include "timed_mutex.hpp"
#include "condition_variable.hpp"

namespace twist::rt::sim {

namespace user::library::std_like {

namespace domain {

// Runtime

// Visibility
static const int kTwistRtVisibility = -128;

// Tag
static const char* kTwistRtTagName = "twist.rt";

// Components

// thread

static trace::attr::Unit thread_impl_rt_tag{kTwistRtTagName};
static trace::Domain thread_impl{"ThreadState"};

static trace::attr::Unit thread_rt_tag{kTwistRtTagName};
static trace::Domain thread{"thread"};

// mutex

static trace::attr::Unit mutex_impl_rt_tag{kTwistRtTagName};
static trace::Domain mutex_impl{"MutexImpl"};

static trace::attr::Unit mutex_rt_tag{kTwistRtTagName};
static trace::Domain mutex{"mutex"};

// timed_mutex

static trace::attr::Unit timed_mutex_impl_rt_tag{kTwistRtTagName};
static trace::Domain timed_mutex_impl{"TimedMutexImpl"};

static trace::attr::Unit timed_mutex_rt_tag{kTwistRtTagName};
static trace::Domain timed_mutex{"timed_mutex"};

// shared_mutex

static trace::attr::Unit shared_mutex_impl_rt_tag{kTwistRtTagName};
static trace::Domain shared_mutex_impl{"SharedMutexImpl"};

static trace::attr::Unit shared_mutex_rt_tag{kTwistRtTagName};
static trace::Domain shared_mutex{"shared_mutex"};

// condition_variable

static trace::attr::Unit cv_impl_rt_tag{kTwistRtTagName};
static trace::Domain cv_impl{"CondVarImpl"};

static trace::attr::Unit cv_rt_tag{kTwistRtTagName};
static trace::Domain cv{"condition_variable"};

//

static struct Init {
  Init() {
    thread_impl.LinkAttr(thread_impl_rt_tag);
    thread_impl.SetVisibility(kTwistRtVisibility);
    thread.LinkAttr(thread_rt_tag);

    mutex_impl.LinkAttr(mutex_impl_rt_tag);
    mutex_impl.SetVisibility(kTwistRtVisibility);
    mutex.LinkAttr(mutex_rt_tag);

    timed_mutex_impl.LinkAttr(timed_mutex_impl_rt_tag);
    timed_mutex_impl.SetVisibility(kTwistRtVisibility);
    timed_mutex.LinkAttr(timed_mutex_rt_tag);

    shared_mutex_impl.LinkAttr(shared_mutex_impl_rt_tag);
    shared_mutex_impl.SetVisibility(kTwistRtVisibility);
    shared_mutex.LinkAttr(shared_mutex_rt_tag);

    cv_impl.LinkAttr(cv_impl_rt_tag);
    cv_impl.SetVisibility(kTwistRtVisibility);
    cv.LinkAttr(cv_rt_tag);
  }
} init;

// Access

// thread

trace::Domain& Thread() {
  return thread;
}

trace::Domain& ThreadState() {
  return thread_impl;
}

// mutex

trace::Domain& Mutex() {
  return mutex;
}

trace::Domain& MutexImpl() {
  return mutex_impl;
}

// timed_mutex

trace::Domain& TimedMutex() {
  return timed_mutex;
}

trace::Domain& TimedMutexImpl() {
  return timed_mutex_impl;
}

// shared_mutex

trace::Domain& SharedMutex() {
  return shared_mutex;
}

trace::Domain& SharedMutexImpl() {
  return shared_mutex_impl;
}

// condition_variable

trace::Domain& CondVar() {
  return cv;
}

trace::Domain& CondVarImpl() {
  return cv_impl;
}

}  // namespace domain

}  // namespace user::library::std_like

}  // namespace twist::rt::sim
