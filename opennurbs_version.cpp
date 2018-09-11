#include "opennurbs.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#define OPENNURBS_VERSION_DEFINITION
#include "opennurbs_version.h"
#undef OPENNURBS_VERSION_DEFINITION

unsigned int ON::Version()
{
  static unsigned int version_number = 0;
  if ( 0 == version_number )
  {
    version_number = ON_VersionNumberConstruct(
      OPENNURBS_VERSION_MAJOR,
      OPENNURBS_VERSION_MINOR,
      OPENNURBS_VERSION_YEAR,
      OPENNURBS_VERSION_MONTH,
      OPENNURBS_VERSION_DAY_OF_MONTH,
      OPENNURBS_VERSION_BRANCH
      );
    const unsigned int macro_version_number = OPENNURBS_VERSION_NUMBER;
    if ( macro_version_number != version_number )
    {
      ON_ERROR("Fix ON_VERSION_... defines in openurbs_version.h");
    }
  }
  return version_number;
}

unsigned int ON::VersionMajor()
{
  return OPENNURBS_VERSION_MAJOR;
}

unsigned int ON::VersionMinor()
{
  return OPENNURBS_VERSION_MINOR;
}

unsigned int ON::VersionYear()
{
  return OPENNURBS_VERSION_YEAR;
}

unsigned int ON::VersionMonth()
{
  return OPENNURBS_VERSION_MONTH;
}

unsigned int ON::VersionDayOfMonth()
{
  return OPENNURBS_VERSION_DAY_OF_MONTH;
}
  
unsigned int ON::VersionHour()
{
  return OPENNURBS_VERSION_HOUR;
}
  
unsigned int ON::VersionMinute()
{
  return OPENNURBS_VERSION_MINUTE;
}

unsigned int ON::VersionBranch()
{
  return OPENNURBS_VERSION_BRANCH;
}

const char* ON::SourceGitRevisionHash()
{
#if defined(OPENNURBS_GIT_REVISION_HASH)
  const char* hash = OPENNURBS_GIT_REVISION_HASH;
  if (nullptr != hash && '0' == hash[0] && 0 == hash[1])
    hash = nullptr;
  if ( nullptr != hash && 0 != hash[0])
    return OPENNURBS_GIT_REVISION_HASH;
#endif
  return "";
}

const char* ON::SourceGitBranchName()
{
#if defined(OPENNURBS_GIT_BRANCH_NAME)
  const char* name = OPENNURBS_GIT_BRANCH_NAME;
  if (nullptr != name && '0' == name[0] && 0 == name[1])
    name = nullptr;
  if (nullptr != name && 0 != name[0])
    return OPENNURBS_GIT_BRANCH_NAME;
#endif
  return "";
}

const char* ON::SourceIdentification()
{
#if defined(OPENNURBS_GIT_BRANCH_NAME) && defined(OPENNURBS_GIT_BRANCH_NAME)
  // As version control systems change, this
  // function will be updated to return some appropriate string value.
  // Developer builds currently return "".
  const char* name = OPENNURBS_GIT_BRANCH_NAME;
  if (nullptr != name && '0' == name[0] && 0 == name[1])
    name = nullptr;
  const char* hash = OPENNURBS_GIT_REVISION_HASH;
  if (nullptr != hash && '0' == hash[0] && 0 == hash[1])
    hash = nullptr;
  if (nullptr != hash && 0 != hash[0])
  {
    return
      (nullptr != name && 0 != name[0])
      ? (OPENNURBS_GIT_BRANCH_NAME " @ " OPENNURBS_GIT_REVISION_HASH)
      : "master @ " OPENNURBS_GIT_REVISION_HASH;
  }
  if (nullptr != name && 0 != name[0])
  {
    return OPENNURBS_GIT_BRANCH_NAME;
  }
#endif
  return "";
}

unsigned int ON::VersionGetQuartet(
  unsigned int opennurbs_version_quartet[4]
  )
{
  const unsigned int v[4] = {OPENNURBS_VERSION_QUARTET_WITH_COMMAS};
  if ( opennurbs_version_quartet )
  {
    opennurbs_version_quartet[0] = v[0];
    opennurbs_version_quartet[1] = v[1];
    opennurbs_version_quartet[2] = v[2];
    opennurbs_version_quartet[3] = v[3];
  }
  return OPENNURBS_VERSION_NUMBER;
}

const char* ON::VersionQuartetAsString()
{
  return OPENNURBS_VERSION_QUARTET_STRING;
}

const wchar_t* ON::VersionQuartetAsWideString()
{
  return OPENNURBS_VERSION_QUARTET_WSTRING;
}
