#include "opennurbs.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#if defined(ON_COMPILER_MSC)
#include <intrin.h>
#endif

// Mesh booleans and hidden line removal report progress from several worker threads
// through one reporter, so m_fraction_complete and m_previous_callback_fraction_complete
// are read and updated with compare and swap. Did not change to std::atomic<double>
// because it would delete the implicit copy constructor
static_assert(sizeof(double) == sizeof(ON__UINT64), "ON_ProgressReporter progress values are swapped as 64 bit patterns");

static double ON_ProgressReporter_AtomicRead(double* location)
{
  ON__UINT64 bits;
#if defined(ON_COMPILER_MSC)
  bits = (ON__UINT64)_InterlockedCompareExchange64((volatile __int64*)location, 0, 0);
#else
  bits = __atomic_load_n((const ON__UINT64*)location, __ATOMIC_SEQ_CST);
#endif
  double value;
  memcpy(&value, &bits, sizeof(value));
  return value;
}

/*
Description:
  Sets *location to desired when *location is bit for bit equal to *expected.
Returns:
  True when the swap happened.
  False otherwise, and *expected is set to the value *location currently holds.
*/
static bool ON_ProgressReporter_AtomicCompareAndSwap(double* location, double* expected, double desired)
{
  ON__UINT64 expected_bits, desired_bits;
  memcpy(&expected_bits, expected, sizeof(expected_bits));
  memcpy(&desired_bits, &desired, sizeof(desired_bits));

#if defined(ON_COMPILER_MSC)
  const ON__UINT64 prior_bits = (ON__UINT64)_InterlockedCompareExchange64(
    (volatile __int64*)location, (__int64)desired_bits, (__int64)expected_bits);
  if (prior_bits == expected_bits)
    return true;
  memcpy(expected, &prior_bits, sizeof(prior_bits));
  return false;
#else
  if (__atomic_compare_exchange_n((ON__UINT64*)location, &expected_bits, desired_bits, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
    return true;
  memcpy(expected, &expected_bits, sizeof(expected_bits));
  return false;
#endif
}

ON_ProgressReporter::ON_ProgressReporter()
: m_callback_function(0)
, m_callback_context(0)
, m_fraction_complete(0.0)
, m_previous_callback_fraction_complete(-1.0) // any value <= -1.0 will work
{
  m_reserved[0] = 0;
  m_reserved[1] = 0;
  m_reserved[2] = 0;
  m_reserved[3] = 0;
}


ON_ProgressReporter::~ON_ProgressReporter()
{
  m_callback_function = 0;
  m_callback_context = 0;
}

void ON_ProgressReporter::SetSynchronousProgressCallbackFunction(
  void (*callback_function)(ON__UINT_PTR context,double fraction_complete),
  ON__UINT_PTR callback_context
  )
{
  if (    0 == callback_function
       || m_callback_function != callback_function
       || m_callback_context != callback_context
     )
  {
    m_callback_context = callback_context;
    m_callback_function = callback_function;
    m_previous_callback_fraction_complete = -1.0; // any value <= -1.0 will work
  }
}

void ON_ProgressReporter::ReportProgress( 
  ON_ProgressReporter* progress_reporter,
  double fraction_complete
  )
{
  if ( nullptr == progress_reporter )
    return;

  double fraction = ON_ProgressReporter_AtomicRead(&progress_reporter->m_fraction_complete);
  double previous = ON_ProgressReporter_AtomicRead(&progress_reporter->m_previous_callback_fraction_complete);

  if ( !(fraction_complete > fraction) && previous > -1.0 )
    return;

  // Advance the latch. It never moves backwards, so a thread that loses the swap
  // retests against the value the winner stored.
  if ( fraction_complete > fraction )
  {
    const double advanced = (fraction_complete < 1.0) ? fraction_complete : 1.0;
    while ( advanced > fraction )
    {
      if ( ON_ProgressReporter_AtomicCompareAndSwap(&progress_reporter->m_fraction_complete, &fraction, advanced) )
      {
        fraction = advanced;
        break;
      }
    }
  }

  // Read the callback once. Testing the member and then calling through it would let a
  // concurrent SetSynchronousProgressCallbackFunction() or destructor clear it in between.
  void (*callback_function)(ON__UINT_PTR,double) = progress_reporter->m_callback_function;
  if ( nullptr == callback_function )
    return;

  // callback at most 1026 times. The swap claims the step, so exactly one thread calls
  // back for it. Claiming before the call also stops a callback that reports progress
  // itself from recursing.
  const double max_callback_count = 1024.0;
  for (;;)
  {
    if ( !(    fraction >= previous + 1.0/max_callback_count
            || (1.0 == fraction && previous < 1.0)
          )
       )
      return;

    if ( ON_ProgressReporter_AtomicCompareAndSwap(&progress_reporter->m_previous_callback_fraction_complete, &previous, fraction) )
      break;
  }

  callback_function(progress_reporter->m_callback_context,fraction);
}

void ON_ProgressReporter::ReportProgress(
  ON_ProgressReporter* progress_reporter,
  int i,
  int max_i
  )
{
  if ( i <= 0 )
    ON_ProgressReporter::ReportProgress( progress_reporter, 0.0 );
  else
    ON_ProgressReporter::ReportProgress( progress_reporter, i >= max_i ? 1.0 : ((double)i)/((double)max_i) );
}

void ON_ProgressReporter::ReportProgress(
  ON_ProgressReporter* progress_reporter,
  unsigned int i,
  unsigned int max_i
  )
{
  ON_ProgressReporter::ReportProgress( progress_reporter, i >= max_i ? 1.0 : ((double)i)/((double)max_i) );
} 

double ON_ProgressReporter::FractionComplete(
  ON_ProgressReporter* progress_reporter
  )
{
  return ( nullptr != progress_reporter ) ? ON_ProgressReporter_AtomicRead(&progress_reporter->m_fraction_complete) : ON_UNSET_VALUE;
}

ON_ProgressStepCounter ON_ProgressStepCounter::Create(
  ON_ProgressReporter* progress_reporter,
  unsigned int step_count
  )
{
  return ON_ProgressStepCounter::Create(
    progress_reporter,
    step_count,
    0.0, 1.0,
    step_count < 100 ? step_count : 100
    );
}

ON_ProgressStepCounter ON_ProgressStepCounter::Create(
  ON_ProgressReporter* progress_reporter,
  unsigned int step_count,
  double progress_interval_start,
  double progress_interval_finish,
  unsigned int maximum_progress_reports
  )
{
  if (nullptr != progress_reporter
    && step_count > 0
    && 0.0 <= progress_interval_start
    && progress_interval_start < progress_interval_finish
    && progress_interval_finish <= 1.0
    && maximum_progress_reports > 0
    )
  {
    ON_ProgressStepCounter counter;
    counter.m_step_index = 0;
    counter.m_step_count = step_count;
    counter.m_step_interval = (step_count <= maximum_progress_reports) ? step_count : (step_count/maximum_progress_reports);
    if ( counter.m_step_interval <= 0 )
      counter.m_step_interval = 1;
    counter.m_progress_interval_start = progress_interval_start;
    counter.m_progress_interval_finish = progress_interval_finish;
    counter.m_progress_reporter = progress_reporter;
    ON_ProgressReporter::ReportProgress(progress_reporter,counter.Progress());
    return counter;
  }

  return ON_ProgressStepCounter::Empty;
}


void ON_ProgressStepCounter::IncrementStep()
{
  if (m_step_index < m_step_count)
  {
    m_step_index++;
    if (0 == m_step_index % m_step_interval || m_step_index == m_step_count)
      ON_ProgressReporter::ReportProgress(m_progress_reporter,Progress());
  }
}

void ON_ProgressStepCounter::Finished()
{
  if (m_step_index < m_step_count)
  {
    m_step_index = m_step_count-1;
    IncrementStep();
  }
}


double ON_ProgressStepCounter::Progress() const
{
  if (m_step_index <= 0)
    return m_progress_interval_start;
  if (m_step_index >= m_step_count)
    return m_progress_interval_finish;
  const double s = ((double)m_step_index)/((double)m_step_count);
  return (1.0 - s)*m_progress_interval_start + s*m_progress_interval_finish;
}

unsigned int ON_ProgressStepCounter::Step() const
{
  return m_step_index;
}


unsigned int ON_ProgressStepCounter::StepCount() const
{
  return m_step_count;
}

