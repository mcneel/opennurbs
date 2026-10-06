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

#if !defined(OPENNURBS_PARALLEL_SORT_INC_)
#define OPENNURBS_PARALLEL_SORT_INC_

////////////////////////////////////////////////////////////////
//
// ON_ParallelSort - a multi-threaded std::sort.
//
// Sorting a multi-million element array is one of the few places in openNURBS
// where a single core is the bottleneck. Building the topology of a dense scan
// mesh is the motivating case: it sorts one ON_MeshFaceSide per face side, and
// on a ten million face mesh that sort dominates, which is felt as a pause
// every time something needs topology - snapping, picking, selection, the
// wireframe display.
//
// The fallback implementation is deliberately built on std::thread and nothing
// else. openNURBS sits below the Rhino core and also ships standalone, as
// public openNURBS and inside rhino3dm, so it cannot reach up to the core's
// parallel sort header and should not acquire a third party dependency to get
// one. std::sort with a parallel execution policy is not a portable answer
// either: it is unimplemented in Apple's libc++, and on libstdc++ it needs TBB.
// That is the whole reason the call sites this replaces were written as a
// Windows branch with a serial fallback for everyone else.
//
// Where the standard library does ship a working parallel sort, though, using
// it beats reimplementing one. The MSVC STL does, with no extra dependency, so
// on MSVC this forwards to std::sort(std::execution::par_unseq, ...) and the
// chunk-and-merge below is not used. That is not a portability hedge, it is a
// measured decision: on a 32 thread Windows machine, sorting 40M
// ON_MeshFaceSide - the ten million face scan mesh this whole exercise is about
// - the chunk-and-merge took 1.41x the time of par_unseq (799 ms against 566
// ms), and it lost by a similar margin at 1M and 10M. par_unseq reached 7.5x
// over a serial sort where the chunk-and-merge reached 5.3x.
//
// Two things about the fallback explain the gap, and neither is cheap to fix:
// std::inplace_merge asks for a temporary buffer and silently degrades to a
// genuinely in-place merge when it cannot have one, which at 640 MB live is the
// likely case; and the last merge round is a single thread merging two
// half-sized runs while every other core idles.
//
// Note that this is where Windows was before RH-97746 - opennurbs_mesh.cpp and
// opennurbs_mesh_topology.cpp already included <execution> and already called
// par_unseq. So MSVC consumers, rhino3dm included, gain no dependency they did
// not already have. What they gain is the Mac and Linux builds finally getting
// a parallel sort too, and the point_stride fix in RH-97757, both of which are
// independent of which sort routine runs underneath.
//
// This header is intentionally NOT included by opennurbs.h. It pulls in <thread>
// and <vector>, and openNURBS keeps the STL out of its public headers and its
// precompiled header. Include it directly from the .cpp that needs it.
//
// Usage:
//
//   #include "opennurbs_parallel_sort.h"
//   ...
//   ON_ParallelSort(a, a + count, [](const T& x, const T& y) { return x < y; });
//
// The comparison predicate must be a strict weak ordering, exactly as std::sort
// requires, and it must additionally be safe to call from several threads at
// once - so it may read whatever it likes, but it must not write to shared
// state. A copy of the predicate is handed to each worker, so a predicate that
// carries mutable per-call scratch state in its captures is a data race. Every
// caller in openNURBS passes a stateless comparison over const data.
//
// The sort is NOT stable, matching std::sort. Where a stable result matters,
// the comparison has to break its own ties - as the vertex-id sort in
// opennurbs_mesh.cpp does, to keep vertex ids from moving when a mesh is
// transformed.
//
////////////////////////////////////////////////////////////////

// Threads are assumed everywhere openNURBS builds except WebAssembly, where a
// std::thread needs a pthreads-enabled toolchain that our build does not use.
// Without this define ON_ParallelSort still compiles and still sorts - it is
// simply std::sort on the calling thread.
#if !defined(ON_RUNTIME_WASM)
#define ON_PARALLEL_SORT_SUPPORTED
#endif

// Use the standard library's own parallel sort where there is one worth using.
// The test is the standard library, not the operating system, because that is
// what actually decides whether <execution> works: the MSVC STL implements the
// parallel algorithms against the Windows thread pool with nothing else to
// install, Apple's libc++ does not implement them at all, and libstdc++ needs
// TBB linked in. clang-cl defines _MSC_VER and uses the MSVC STL, so it lands
// here correctly.
//
// Defining ON_PARALLEL_SORT_NO_STD_EXECUTION forces the chunk-and-merge even on
// MSVC. That exists so the implementation the Mac and Linux builds actually use
// can be exercised on Windows hardware - without it nothing running on Windows
// ever executes the code below, so a change that broke it would only be caught
// by someone building on a Mac. Nothing in the shipping build defines it; it is
// for tests and soaks.
#if defined(ON_PARALLEL_SORT_SUPPORTED) && defined(_MSC_VER) && !defined(ON_PARALLEL_SORT_NO_STD_EXECUTION)
#define ON_PARALLEL_SORT_USE_STD_EXECUTION
#endif

#include <algorithm>
#include <iterator>

#if defined(ON_PARALLEL_SORT_SUPPORTED)
#include <thread>
#include <vector>
#endif

#if defined(ON_PARALLEL_SORT_USE_STD_EXECUTION)
#include <execution>
#endif

/*
Description:
  Number of elements below which ON_ParallelSort sorts on the calling thread.

Remarks:
  Starting a thread costs tens of microseconds, which is the same order as
  std::sort on a few tens of thousands of small elements, so below this count
  threading is a pessimisation rather than a speedup. The value is a round
  number in the middle of the range where the two are level, not a measured
  optimum - the sorts that matter here are two to three orders of magnitude
  bigger, and the sorts that do not are far below it.
*/
const size_t ON_ParallelSort_MinimumCount = 32768;

/*
Description:
  Largest number of worker threads ON_ParallelSort will use.

Remarks:
  openNURBS is frequently called from code that is already parallel - meshing,
  the render pipeline, per-object display cache builds. An unbounded fan-out
  inside a sort would multiply against that. This cap keeps the oversubscription
  bounded while still covering the machines Rhino runs on.
*/
const unsigned int ON_ParallelSort_MaximumThreadCount = 32;

/*
Description:
  Number of worker threads the chunk-and-merge implementation would use for a
  given element count.
Parameters:
  count - [in] number of elements to be sorted.
Returns:
  1 when the sort will run on the calling thread, otherwise the number of
  threads the work will be split across. Always a power of two, so that the
  merge phase pairs up exactly.
Remarks:
  This describes the chunk-and-merge implementation only. Where
  ON_PARALLEL_SORT_USE_STD_EXECUTION is defined - MSVC - ON_ParallelSort
  forwards to the standard library instead, which picks its own thread count,
  so a return value above 1 means "large enough that ON_ParallelSort will try to
  use threads" and nothing more precise than that.
*/
inline unsigned int ON_ParallelSortThreadCount(size_t count)
{
#if defined(ON_PARALLEL_SORT_SUPPORTED)
  if (count < ON_ParallelSort_MinimumCount)
    return 1;

  // hardware_concurrency() is permitted to return 0 when it cannot tell.
  unsigned int available = std::thread::hardware_concurrency();
  if (available < 2)
    return 1;

  if (available > ON_ParallelSort_MaximumThreadCount)
    available = ON_ParallelSort_MaximumThreadCount;

  // Round down to a power of two, and do not hand any thread less than the
  // serial threshold - a 40000 element sort on a 24 core machine would
  // otherwise be 24 threads sorting 1600 elements each, which is all overhead.
  unsigned int threads = 1;
  while (threads * 2 <= available && (count / (threads * 2)) >= ON_ParallelSort_MinimumCount)
    threads *= 2;

  return threads;
#else
  (void)count;
  return 1;
#endif
}

/*
Description:
  Sort a range in place using multiple threads.
Parameters:
  first - [in] beginning of the range.
  last - [in] end of the range.
  comp - [in] strict weak ordering, called concurrently. See the notes at the
              top of this file.
Remarks:
  Behaves as std::sort(first, last, comp) in every respect a caller can observe,
  apart from being faster on large ranges. Falls back to std::sort on the
  calling thread for small ranges, on a single core machine, and where threads
  are unavailable.

  On MSVC this forwards to std::sort(std::execution::par_unseq, ...), which is
  faster than the implementation below - see the note at the top of this file.
  Small ranges still go to a plain std::sort rather than through the parallel
  machinery, so the threshold means the same thing on every platform.

  Everywhere else: sorts each of N equal chunks on its own thread, then merges
  the chunks pairwise, halving the number of runs each round and using every
  core for the early rounds. std::inplace_merge does the merging: it takes
  whatever temporary buffer it can get and degrades to an in-place merge rather
  than failing when it cannot have one, which matters here because the arrays
  being sorted are already hundreds of megabytes.

  A predicate that throws will terminate rather than propagate, because it
  throws on a worker thread. std::sort makes no guarantee about the state of the
  range after a throwing predicate either, so nothing is lost that a caller
  could have relied on.
*/
template <class RandomIt, class Compare>
void ON_ParallelSort(RandomIt first, RandomIt last, Compare comp)
{
  if (last <= first)
    return;

#if defined(ON_PARALLEL_SORT_USE_STD_EXECUTION)

  // The standard library has a parallel sort worth using. Keep the small-range
  // threshold so that behaviour matches the other platforms and so that the
  // tens of thousands of tiny sorts openNURBS does are not handed to the
  // parallel machinery just to be handed straight back.
  if ((size_t)std::distance(first, last) < ON_ParallelSort_MinimumCount)
    std::sort(first, last, comp);
  else
    std::sort(std::execution::par_unseq, first, last, comp);

#elif defined(ON_PARALLEL_SORT_SUPPORTED)
  const size_t count = (size_t)std::distance(first, last);
  const unsigned int thread_count = ON_ParallelSortThreadCount(count);

  if (thread_count < 2)
  {
    std::sort(first, last, comp);
    return;
  }

  // Chunk boundaries. bounds[i] .. bounds[i+1] is chunk i, and bounds has one
  // extra entry so the last chunk gets the remainder.
  std::vector<size_t> bounds(thread_count + 1);
  const size_t chunk = count / thread_count;
  for (unsigned int i = 0; i < thread_count; i++)
    bounds[i] = i * chunk;
  bounds[thread_count] = count;

  std::vector<std::thread> workers;
  workers.reserve(thread_count - 1);

  // Sort the chunks. The calling thread takes the first one rather than
  // starting a thread and then waiting on it.
  for (unsigned int i = 1; i < thread_count; i++)
  {
    const size_t lo = bounds[i];
    const size_t hi = bounds[i + 1];
    workers.emplace_back([first, lo, hi, comp]() { std::sort(first + lo, first + hi, comp); });
  }
  std::sort(first + bounds[0], first + bounds[1], comp);

  for (auto& worker : workers)
    worker.join();

  // Merge rounds. Each round merges chunk i with chunk i+width, so the number
  // of sorted runs halves and width doubles until one run is left. The merges
  // within a round touch disjoint ranges, so they run concurrently.
  //
  // thread_count is a power of two, so every chunk has a partner in every round
  // and the pairs come out even - there is no leftover run to carry forward.
  for (unsigned int width = 1; width < thread_count; width *= 2)
  {
    workers.clear();

    // Pair 0 is left for the calling thread, below.
    for (unsigned int i = 2 * width; i + width < thread_count; i += 2 * width)
    {
      const size_t lo = bounds[i];
      const size_t mid = bounds[i + width];
      const size_t hi = bounds[i + 2 * width];

      workers.emplace_back([first, lo, mid, hi, comp]()
        { std::inplace_merge(first + lo, first + mid, first + hi, comp); });
    }

    // Pair 0 always exists: width < thread_count is the loop condition.
    std::inplace_merge(first + bounds[0], first + bounds[width], first + bounds[2 * width], comp);

    for (auto& worker : workers)
      worker.join();
  }
#else
  std::sort(first, last, comp);
#endif
}

/*
Description:
  Sort a range in place using multiple threads and operator<.
Parameters:
  first - [in] beginning of the range.
  last - [in] end of the range.
*/
template <class RandomIt>
void ON_ParallelSort(RandomIt first, RandomIt last)
{
  typedef typename std::iterator_traits<RandomIt>::value_type ON_ParallelSort_value_type;
  ON_ParallelSort(first, last,
    [](const ON_ParallelSort_value_type& a, const ON_ParallelSort_value_type& b) { return a < b; });
}

#endif // OPENNURBS_PARALLEL_SORT_INC_
