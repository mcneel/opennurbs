/*
//
// Copyright (c) 1993-2026 Robert McNeel & Associates. All rights reserved.
// OpenNURBS, Rhinoceros, and Rhino3D are registered trademarks of Robert
// McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//				
// For complete openNURBS copyright information see <http://www.opennurbs.org>.
//
////////////////////////////////////////////////////////////////
*/

#if !defined(OPENNURBS_CPP_BASE_INC_)
#define OPENNURBS_CPP_BASE_INC_

// basic C++ declarations

#include <bitset>
#include <ostream>

#if !defined(UUID_DEFINED) && !defined(GUID_DEFINED)
// basic C++ declarations
bool operator==(const struct ON_UUID_struct& a, const struct ON_UUID_struct& b);
bool operator!=(const struct ON_UUID_struct& a, const struct ON_UUID_struct& b);
#endif

class ON_CLASS ON_StopWatch
{
public:
  ON_StopWatch() = default;
  ~ON_StopWatch() = default;
  ON_StopWatch(const ON_StopWatch&) = default;
  ON_StopWatch& operator=(const ON_StopWatch&) = default;

public:
  enum class State : unsigned char
  {
    ///<summary>
    /// The stopwatch is off.
    ///</summary>
    Off = 0, 

    ///<summary>
    /// The stopwatch is started and running. 
    ///</summary>
    Running = 1,

    ///<summary>
    /// The stopwatch has been started and stopped.
    ///</summary>
    Stopped = 2
  };

  /*
  Description:
    If the stopwatch is off or stopped, it is started. Otherwise nothing happens. 
  */
  void Start();

  /*
  Description:
    If the stopwatch is running, then it is stopped. Otherwise nothing happens. 
  Returns:
    If the stopwatch was running, the elapsed time from the most recent Start().
    Otherwise, 0.0 is returned.
  */
  double Stop();

  /*
  Description:
    The stopwatch is reset and turned off. Any previously set times are lost.
  */
  void Reset();

  /*
  Returns:
    Current state of the stopwatch.
  */
  ON_StopWatch::State CurrentState() const;


  /*
  Returns:
    The elapsed time in seconds.
  Remarks:
    If the stopwatch is running, the elapsed time is the duration from the most recent Start() to now.
    If the stopwatch is stopped, the elapsed time is the duration between the most recent Start() and Stop().
    If the stopwatch is off, the elapsed time is zero.
  */
  double ElapsedTime() const;

private:
  // current state
  ON_StopWatch::State m_state = ON_StopWatch::State::Off;
#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4251 ) 
  // C4251: ... : class 'std::...' 
  //        needs to have dll-interface to be used by clients ...
  // m_start and m_stop are private and all code that manages them is explicitly implemented in the DLL.
  std::chrono::high_resolution_clock::time_point m_start; // most recent Start() time.
  std::chrono::high_resolution_clock::time_point m_stop; // most recent Stop() time.
#pragma ON_PRAGMA_WARNING_POP
};

// templated handler for scoped enums that represent bit flags
template <typename T>
class ON_BitFlags
{
  
  // BitFlags taken from https://web.archive.org/web/20250118231723/https://voithos.io/articles/type-safe-enum-class-bit-flags/
  // this code has MIT license. Renamed to ON_BitFlags and extended to return ::All() and ::None()
  using UnderlyingT = std::underlying_type_t<T>;

public:
  constexpr ON_BitFlags() : flags_(static_cast<UnderlyingT>(0)) {}
  constexpr explicit ON_BitFlags(T v) : flags_(ToUnderlying(v)) {}
  constexpr ON_BitFlags(std::initializer_list<T> vs) : ON_BitFlags()
  {
    for (T v : vs)
    {
      flags_ |= ToUnderlying(v);
    }
  }

  static constexpr ON_BitFlags None()
  {
    return ON_BitFlags(static_cast<UnderlyingT>(0));
  }

  static constexpr ON_BitFlags All()
  {
    return ~None();
  }

  constexpr bool Any() const
  {
    return flags_ != None();
  }

  // Checks if a specific flag is set.
  constexpr bool IsSet(T v) const
  {
    return (flags_ & ToUnderlying(v)) == ToUnderlying(v);
  }
  // Sets a single flag value.
  constexpr void Set(T v) { flags_ |= ToUnderlying(v); }
  // Unsets a single flag value.
  constexpr void Unset(T v) { flags_ &= ~ToUnderlying(v); }
  // Clears all flag values.
  constexpr void Clear() { flags_ = None(); }

  constexpr operator bool() const
  {
    return flags_ != static_cast<UnderlyingT>(0);
  }

  friend constexpr ON_BitFlags operator|(ON_BitFlags lhs, T rhs)
  {
    return ON_BitFlags(lhs.flags_ | ToUnderlying(rhs));
  }
  friend constexpr ON_BitFlags operator|(ON_BitFlags lhs, ON_BitFlags rhs)
  {
    return ON_BitFlags(lhs.flags_ | rhs.flags_);
  }
  friend constexpr ON_BitFlags operator&(ON_BitFlags lhs, T rhs)
  {
    return ON_BitFlags(lhs.flags_ & ToUnderlying(rhs));
  }
  friend constexpr ON_BitFlags operator&(ON_BitFlags lhs, ON_BitFlags rhs)
  {
    return ON_BitFlags(lhs.flags_ & rhs.flags_);
  }
  friend constexpr ON_BitFlags operator^(ON_BitFlags lhs, T rhs)
  {
    return ON_BitFlags(lhs.flags_ ^ ToUnderlying(rhs));
  }
  friend constexpr ON_BitFlags operator^(ON_BitFlags lhs, ON_BitFlags rhs)
  {
    return ON_BitFlags(lhs.flags_ ^ rhs.flags_);
  }

  friend constexpr ON_BitFlags& operator|=(ON_BitFlags& lhs, T rhs)
  {
    lhs.flags_ |= ToUnderlying(rhs);
    return lhs;
  }
  friend constexpr ON_BitFlags& operator|=(ON_BitFlags& lhs, ON_BitFlags rhs)
  {
    lhs.flags_ |= rhs.flags_;
    return lhs;
  }
  friend constexpr ON_BitFlags& operator&=(ON_BitFlags& lhs, T rhs)
  {
    lhs.flags_ &= ToUnderlying(rhs);
    return lhs;
  }
  friend constexpr ON_BitFlags& operator&=(ON_BitFlags& lhs, ON_BitFlags rhs)
  {
    lhs.flags_ &= rhs.flags_;
    return lhs;
  }
  friend constexpr ON_BitFlags& operator^=(ON_BitFlags& lhs, T rhs)
  {
    lhs.flags_ ^= ToUnderlying(rhs);
    return lhs;
  }
  friend constexpr ON_BitFlags& operator^=(ON_BitFlags& lhs, ON_BitFlags rhs)
  {
    lhs.flags_ ^= rhs.flags_;
    return lhs;
  }

  friend constexpr ON_BitFlags operator~(const ON_BitFlags& bf)
  {
    return ON_BitFlags(~bf.flags_);
  }

  friend constexpr bool operator==(const ON_BitFlags& lhs, const ON_BitFlags& rhs)
  {
    return lhs.flags_ == rhs.flags_;
  }
  friend constexpr bool operator!=(const ON_BitFlags& lhs, const ON_BitFlags& rhs)
  {
    return lhs.flags_ != rhs.flags_;
  }

  // Stream output operator for debugging.
  friend std::ostream& operator<<(std::ostream& os, const ON_BitFlags& bf)
  {
    // Write out a bitset representation.
    os << std::bitset<sizeof(UnderlyingT) * 8>(bf.flags_);
    return os;
  }

  // Construct BitFlags from raw values.
  static constexpr ON_BitFlags FromRaw(UnderlyingT flags)
  {
    return ON_BitFlags(flags);
  }
  // Retrieve the raw underlying flags.
  constexpr UnderlyingT ToRaw() const { return flags_; }

private:
  constexpr explicit ON_BitFlags(UnderlyingT flags) : flags_(flags) {}
  static constexpr UnderlyingT ToUnderlying(T v) { return static_cast<UnderlyingT>(v); }
  UnderlyingT flags_;
};


#endif
