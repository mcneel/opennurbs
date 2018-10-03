/*
//
// Copyright (c) 1993-2018 Robert McNeel & Associates. All rights reserved.
// OpenNURBS, Rhinoceros, and Rhino3D are registered trademarks of Robert
// McNeel & Assoicates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//				
// For complete openNURBS copyright information see <http://www.opennurbs.org>.
//
////////////////////////////////////////////////////////////////
*/		

////////////////////////////////////////////////////////////////
//
//  example_test.cpp  
// 
//  Example program using the Rhino file IO toolkit. The program reads  
//  Rhino 3dm model files and verifies they can be saved and reread with
//  no changes to core model content. The program is a console application
//  that takes options, file names, and directory names as command line
//   arguments.
//
////////////////////////////////////////////////////////////////////////

#include "../opennurbs_public_examples.h"

class Internal_CTestContext
{
public:
  enum : unsigned int
  {
    sizeof_a = 4088
  };

  Internal_CTestContext()
  {
    m_a = onmalloc(sizeof_a);
  }

  ~Internal_CTestContext()
  {
    onfree(m_a);
  }

  void SetInitialDirecory(
    const wchar_t* initial_directory,
    unsigned int counter
  )
  {
    m_initial_directory = Internal_CleanPath(initial_directory);
    m_initial_directory_length = m_initial_directory.Length();
    m_initial_directory_counter = counter;
  }

  void* m_a = nullptr;
  ON__INT64 m_min_delta_time = 0;
  ON__INT64 m_max_delta_time = 0;

  unsigned int m_file_count = 0;
  unsigned int m_directory_count = 0;
  unsigned int m_file_open_error_count = 0;
  unsigned int m_max_directory_tree_depth = 0;
  unsigned int m_max_file_count = 0;

  const ON_wString TextLogPathFromFullPath(
    const wchar_t* full_path
  ) const;

private:
  ON_wString m_initial_directory;
  unsigned int m_initial_directory_counter = 0;
  int m_initial_directory_length = 0;
  Internal_CTestContext(const Internal_CTestContext&) = delete;
  Internal_CTestContext operator=(const Internal_CTestContext&) = delete;
  const ON_wString Internal_CleanPath(const wchar_t* dirty_path) const
  {
    const wchar_t* volume = 0;
    const wchar_t* path = 0;

    // Use local path in case drive, dir, file_name_stem or ext are being reused.
    on_wsplitpath(dirty_path, &volume, &path, nullptr, nullptr);
    ON_wString clean_path(path);
    if (clean_path.IsEmpty())
      return ON_wString(dirty_path);
    clean_path.Replace(ON_wString::Backslash, ON_wString::Slash);
    
    if (nullptr != volume && volume < path)
    {
      ON_wString clean_volume(volume, (int)(path - volume));
      return (clean_volume + clean_path);
    }

    return clean_path;
  }

};

const ON_wString Internal_CTestContext::TextLogPathFromFullPath(const wchar_t* full_path) const
{
  // replace initial directory with <Initial Folder> and use / for
  // the directory separator so that output files are standardized.
  ON_wString text_log_path;
  ON_wString path1 = Internal_CleanPath(full_path);
  if (m_initial_directory_length > 0 &&
    ON_wString::EqualOrdinal(m_initial_directory, m_initial_directory_length, path1, m_initial_directory_length, true)
    )
  {
    text_log_path 
      = (m_initial_directory_counter>0)
      ? ON_wString::FormatToString(L"<initial folder %u>", m_initial_directory_counter)
      : ON_wString(L"<initial folder>");
    text_log_path += static_cast<const wchar_t*>(path1) + m_initial_directory_length;
  }
  else
  {
    text_log_path = path1;
  }
  return text_log_path;
}


static const ONX_ErrorCounter Internal_TestModelRead(
  ON_TextLog& text_log,
  const ON_wString text_log_3dm_file_name,
  ON_BinaryArchive& source_archive,
  bool bVerbose
  )
{
  ONX_ModelTest::Type test_type = ONX_ModelTest::Type::ReadWriteReadCompare;
  
  /////
  //
  // Read the orginal file
  //
  text_log.PushIndent();
  ONX_ModelTest test;
  test.ReadTest(source_archive, test_type, true, text_log_3dm_file_name, &text_log);
  text_log.PrintNewLine();
  text_log.Print("Test Results:\n");
  text_log.PushIndent();
  test.Dump(text_log);
  text_log.PopIndent();

  ONX_ErrorCounter error_counter = test.ErrorCounter();

  const ONX_Model* source_model = test.SourceModel().get();
  if (nullptr == source_model)
  {
    text_log.PopIndent();
    return error_counter;
  }

  const bool bCompareTestFailed = ONX_ModelTest::Result::Fail == test.TestResult(ONX_ModelTest::Type::ReadWriteReadCompare);

  if ( bVerbose || bCompareTestFailed )
  {
    for (int i = 0; i < 2; i++)
    {
      if (0 == i)
        test.DumpSourceModel();
      else
        test.DumpReadWriteReadModel();
      if (false == bCompareTestFailed)
        break;
    }
  }
  
  text_log.PrintNewLine();

  const ON_ModelComponent::Type component_types[] =
  {
    // TODO uncomment types as components are derived from ON_ModelComponent
    ON_ModelComponent::Type::Image,
    //ON_ModelComponent::Type::TextureMapping,
    ON_ModelComponent::Type::RenderMaterial,
    ON_ModelComponent::Type::LinePattern,
    ON_ModelComponent::Type::Layer,
    //ON_ModelComponent::Type::Group,
    ON_ModelComponent::Type::TextStyle,
    ON_ModelComponent::Type::DimStyle,
    ON_ModelComponent::Type::RenderLight,
    ON_ModelComponent::Type::HatchPattern,
    ON_ModelComponent::Type::InstanceDefinition,
    ON_ModelComponent::Type::ModelGeometry,
    //ON_ModelComponent::Type::HistoryRecord
  };

  const unsigned int component_type_count = (unsigned int)(sizeof(component_types) / sizeof(component_types[0]));

  const ONX_Model& model = *source_model;

  error_counter.ClearLibraryErrorsAndWarnings();

  const ON_ComponentManifest& manifest = model.Manifest();
  ON_SerialNumberMap it_map;

  for (unsigned int i = 0; i < component_type_count; i++)
  {
    const ON_ModelComponent::Type component_type = component_types[i];
    const bool bUniqueNameRequired = ON_ModelComponent::UniqueNameRequired(component_type);
    const bool bEmbeddedFileComponent = (ON_ModelComponent::Type::Image == component_type);
    ONX_ModelComponentIterator it(model,component_type);
    unsigned int it_count = 0;
    for (ON_ModelComponentReference mcr = it.FirstComponentReference(); false == mcr.IsEmpty(); mcr = it.NextComponentReference())
    {
      const ON_ModelComponent* model_component = mcr.ModelComponent();
      if (nullptr == model_component)
      {
        ON_ERROR("Iterator returned nullptr mcr.ModelComponent()");
        continue; 
      }
      if (model_component->ComponentType() != component_type)
      {
        ON_ERROR("Iterator returned wrong mcr.ModelComponent()->ComponentType()");
        continue; 
      }
      const ON__UINT64 model_component_sn = model_component->RuntimeSerialNumber();
      const ON_UUID model_component_id = model_component->Id();
      if (0 == model_component_sn)
      {
        ON_ERROR("Iterator mcr.ModelComponent()->RuntimeSerialNumber() is zero.");
        continue; 
      }
      if (ON_nil_uuid == model_component_id)
      {
        ON_ERROR("Iterator mcr.ModelComponent()->Id() is nil.");
        continue; 
      }
      const ON_SerialNumberMap::SN_ELEMENT* e_sn = it_map.FindSerialNumber(model_component_sn);
      if (nullptr != e_sn)
      {
        ON_ERROR("Iterator returned component serial number twice.");
        continue; 
      }
      const ON_SerialNumberMap::SN_ELEMENT* e_id = it_map.FindId(model_component_id);
      if (nullptr != e_id)
      {
        ON_ERROR("Iterator returned component id twice.");
        continue; 
      }
      ON_SerialNumberMap::SN_ELEMENT* e = it_map.AddSerialNumberAndId(model_component_sn,model_component_id);
      if (nullptr == e)
      {
        ON_ERROR("ON_SerialNumberMap failed to add sn and id.");
        continue; 
      }
      if (e->m_value.m_u_type != 0)
      {
        ON_ERROR("ON_SerialNumberMap error.");
        continue; 
      }
      e->m_value.m_u_type = 1;
      e->m_value.m_u.ptr = (void*)model_component;

      const ON_ComponentManifestItem& item = manifest.ItemFromComponentRuntimeSerialNumber(model_component_sn);
      if (item.IsUnset())
      {
        ON_ERROR("Iterator returned item not in manifest.");
        continue; 
      }
      if (model_component_sn != item.ComponentRuntimeSerialNumber())
      {
        ON_ERROR("manifest.ItemFromComponentRuntimeSerialNumber() error.");
        continue; 
      }
      if (model_component_id != item.Id())
      {
        ON_ERROR("item has wrong id.");
        continue; 
      }
 
      const ON_ComponentManifestItem& item_id = manifest.ItemFromId(model_component_id);
      if (&item != &item_id)
      {
        ON_ERROR("manifest.ItemFromId() failed.");
        continue; 
      }

      if (bUniqueNameRequired || bEmbeddedFileComponent)
      {
        const ON_Bitmap* embedded_file_reference = bEmbeddedFileComponent ? ON_Bitmap::Cast(model_component) : nullptr;
        const ON_NameHash full_path_hash 
          = (nullptr != embedded_file_reference) 
          ? ON_NameHash::CreateFilePathHash(embedded_file_reference->FileReference()) 
          : ON_NameHash::EmptyNameHash;
        const ON_NameHash name_hash 
          =bEmbeddedFileComponent
          ? full_path_hash
          : model_component->NameHash();
        if (false == name_hash.IsValidAndNotEmpty())
        {
          ON_ERROR("model compoent name is not valid.");
          continue; 
        }
        if (name_hash != item.NameHash())
        {
          ON_ERROR("item has wrong name hash.");
          continue; 
        }
        const ON_ComponentManifestItem& item_name = manifest.ItemFromNameHash(component_type,name_hash);
        if (&item != &item_name)
        {
          ON_ERROR("manifest.ItemFromNameHash() failed.");
          continue; 
        }
      }
      it_count++;
    }

    if (it_count != manifest.ActiveAndDeletedComponentCount(component_type))
    {
      ON_ERROR("iterator count != manifest.ActiveAndDeletedComponentCount(component_type)");
      continue; 
    }
  }

  text_log.PopIndent();
  error_counter.AddLibraryErrorsAndWarnings();
  return error_counter;
}

static const ONX_ErrorCounter Internal_TestFileRead(
  ON_TextLog& text_log,
  const ON_wString fullpath,
  const ON_wString text_log_path,
  bool bVerbose
  )
{
  FILE* fp = nullptr;

  fp = ON_FileStream::Open3dmToRead(fullpath);

  ONX_ErrorCounter error_counter;
  error_counter.ClearLibraryErrorsAndWarnings();

  const ON_wString path_to_print
    = (text_log_path.IsNotEmpty())
    ? text_log_path
    : fullpath;
  
  for (;;)
  {
    if (nullptr == fp)
    {
      text_log.Print(
        L"Skipped file: %ls\n",
        static_cast<const wchar_t*>(path_to_print)
      );
      error_counter.IncrementFailureCount();
      break;
    }

    text_log.Print(
      L"File name: %ls\n",
      static_cast<const wchar_t*>(path_to_print)
    );

    ON_BinaryFile source_archive(ON::archive_mode::read3dm, fp);
    source_archive.SetArchiveFullPath(fullpath);

    error_counter += Internal_TestModelRead(text_log, path_to_print, source_archive, bVerbose);
    break;
  }

  if ( nullptr != fp )
  {
    ON_FileStream::Close(fp);
    fp = nullptr;
  }

  return error_counter;
}

static int Internal_ComparePath(const ON_wString* lhs, const ON_wString* rhs)
{
  // sort nullptr to end of list.
  if (lhs == rhs)
    return 0;
  if (nullptr == lhs)
    return 1;
  if (nullptr == rhs)
    return -1;
  int rc = ON_wString::CompareOrdinal(static_cast<const wchar_t*>(*lhs), static_cast<const wchar_t*>(*rhs), true);
  if ( 0 == rc )
    rc = ON_wString::CompareOrdinal(static_cast<const wchar_t*>(*lhs), static_cast<const wchar_t*>(*rhs), false);
  return rc;
}

static const ONX_ErrorCounter Internal_TestReadFolder(
  ON_TextLog& text_log,
  const wchar_t* directory_path,
  unsigned int directory_tree_depth,
  bool bVerbose,
  Internal_CTestContext& test_context
  )
{
  ONX_ErrorCounter error_counter;
  error_counter.ClearLibraryErrors();

  if (nullptr == directory_path || 0 == directory_path[0])
  {
    text_log.Print(L"Empty directory name.\n");
  }

  ON_FileIterator fit;
  if (false == fit.Initialize(directory_path))
  {
    text_log.Print(
      L"Invalid directory name: %ls\n",
      directory_path
      );
    error_counter.IncrementFailureCount();
    return error_counter;
  }

  const ON_wString text_log_directory_name
    = (directory_tree_depth <= 1)
    ? ON_wString(directory_path)
    : test_context.TextLogPathFromFullPath(directory_path);
  text_log.Print(
    L"Directory name: %ls\n",
    static_cast<const wchar_t*>(text_log_directory_name)
    );
  text_log.PushIndent();

  ON_ClassArray< ON_wString > sub_directories(32);
  ON_ClassArray< ON_wString > skipped_files(32);
  ON_ClassArray< ON_wString > tested_files(32);

  for ( bool bHaveItem = fit.FirstItem(); bHaveItem; bHaveItem = fit.NextItem() )
  {
    if ( test_context.m_max_file_count > 0 && test_context.m_file_count >= test_context.m_max_file_count)
      break;

    if (fit.CurrentItemIsDirectory())
    {
      if (directory_tree_depth < test_context.m_max_directory_tree_depth)
      {
        sub_directories.Append(fit.CurrentItemFullPathName());
      }
      continue;
    }

    ON_wString fullpath = fit.CurrentItemFullPathName();

    if (ON_FileStream::Is3dmFile(fullpath, false))
      tested_files.Append(fullpath);
    else
      skipped_files.Append(fullpath);
  }

  // sort file and folder names so test order depends only on content.
  // This is required so results from different computers can be compared.
  sub_directories.QuickSort(Internal_ComparePath);
  skipped_files.QuickSort(Internal_ComparePath);
  tested_files.QuickSort(Internal_ComparePath);

  if (skipped_files.Count() > 0)
  {
    text_log.PrintNewLine();
    for (int i = 0; i < skipped_files.Count(); i++)
    {
      const ON_wString path_to_print = test_context.TextLogPathFromFullPath(skipped_files[i]);
      text_log.Print(
        L"Skipped file: %ls\n",
        static_cast<const wchar_t*>(path_to_print)
      );
    }
    text_log.PrintNewLine();
  }
     
  for ( int i = 0; i < tested_files.Count(); i++ )
  {
    const ON_wString full_path = tested_files[i];
    const ON_wString path_to_print = test_context.TextLogPathFromFullPath(full_path);
    test_context.m_file_count++;
    const ONX_ErrorCounter file_error_counter = Internal_TestFileRead(text_log, full_path, path_to_print, bVerbose);
    error_counter += file_error_counter;
  }

  for (int i = 0; i < sub_directories.Count(); i++)
  {
    if (test_context.m_max_file_count > 0 && test_context.m_file_count >= test_context.m_max_file_count)
      break;
    const ON_wString sub_directory_path = sub_directories[i];
    test_context.m_directory_count++;
    error_counter += Internal_TestReadFolder(text_log, sub_directory_path, directory_tree_depth + 1, bVerbose, test_context);
  }

  text_log.PopIndent();

  return error_counter;
}

static ONX_ErrorCounter Internal_Test( 
  unsigned int max_directory_tree_depth,
  ON_wString full_path,
  bool bVerbose,
  ON_TextLog& text_log,
  unsigned int& directory_counter,
  unsigned int& file_count,
  unsigned int& folder_count
)
{

  ONX_ErrorCounter err = ONX_ErrorCounter::Zero;

  if (ON_FileSystem::IsFile(full_path))
  {
    if (ON_FileStream::Is3dmFile(full_path, false))
    {
      text_log.Print(L"Testing 3dm file: %ls\n", static_cast<const wchar_t*>(full_path));
      err = Internal_TestFileRead(text_log, full_path, ON_wString::EmptyString, bVerbose);
      file_count++;
    }
  }
  else if ( max_directory_tree_depth > 0 )
  {
    if (full_path.Length() != 1 || false == ON_FileSystemPath::IsDirectorySeparator(full_path[0], true))
    {
      const wchar_t dir_seps[3] = { ON_FileSystemPath::DirectorySeparator,ON_FileSystemPath::AlternateDirectorySeparator,0 };
      full_path.TrimRight(dir_seps);
    }
    if (ON_FileSystem::IsDirectory(full_path))
    {
      text_log.Print(L"Testing 3dm files in folder: %ls\n", static_cast<const wchar_t*>(full_path));
      Internal_CTestContext test_context;
      directory_counter++;
      test_context.SetInitialDirecory(full_path,directory_counter);
      test_context.m_max_directory_tree_depth = max_directory_tree_depth;

      err = Internal_TestReadFolder(text_log, full_path, 1, bVerbose, test_context);
      file_count += test_context.m_file_count;
      folder_count += test_context.m_directory_count;
    }
  }

  return err;
}

static void Internal_PrintHelp(
  const char* example_test_exe_name,
  ON_TextLog& text_log
)
{
  if ( 0 == example_test_exe_name || 0 == example_test_exe_name[0])
    example_test_exe_name = "example_test";

  text_log.Print("\n");
  text_log.Print("SYNOPSIS:\n");
  text_log.Print("  %s [-out:outputfilename.txt] [-r[:N]] <file or directory names>\n",example_test_exe_name );
  text_log.Print("\n");
  text_log.Print("DESCRIPTION:\n");
  text_log.Print("  If a file is listed, it is read as an opennurbs model file.\n");
  text_log.Print("  If a directory is listed, all .3dm files in that directory\n");
  text_log.Print("  are read as opennurbs model files.\n");
  text_log.Print("\n");
  text_log.Print("  Available options:\n");
  text_log.Print("    -out:outputfilename.txt\n");
  text_log.Print("      The output is written to the named file.\n");
  text_log.Print("    -recursive\n");
  text_log.Print("      Recursivly reads files in subdirectories.\n");
  text_log.Print("      If a :N is appended to the option, N limits the recursion depth.\n");
  text_log.Print("      -r:1 reads the directory and does not recurse.\n");
  text_log.Print("    -verbose\n");
  text_log.Print("      Include a full listing of 3dm file contents.\n");
  text_log.Print("\n");
  text_log.Print("RETURNS:\n");
  text_log.Print("  0: All tested files passed.\n");
  text_log.Print("  >0: Number of failures, errors, or warnings. See output for details.\n");
  text_log.Print("\n");
  text_log.Print("EXAMPLE:\n");
  text_log.Print("  %s -out:list.txt -resursive:2 .../example_files\n",example_test_exe_name);
  text_log.Print("  with read all the opennurbs .3dm files in the\n"); 
  text_log.Print("  example_files/ directory and immediate subdirectories of example_files/.\n");
}

static const char* Internal_ParseOptionHead(
  const char* s,
  const char* option_name0,
  const char* option_name1 = nullptr,
  const char* option_name2 = nullptr
)
{
  if (nullptr == s || 0 == s[0])
    return nullptr;
  if ('-' != s[0] )
  {
#if defined(ON_OS_WINDOWS)
    if ( '/' != s[0] )
#endif
    return nullptr;
  }

  s++;
  if (0 == s[0])
    return nullptr;
  
  const char* option_names[] = { option_name0, option_name1, option_name2 };
  const size_t option_name_count = sizeof(option_names) / sizeof(option_names[0]);
  for (size_t i = 0; i < option_name_count; i++)
  {
    const char* option_name = option_names[i];
    const int option_name_length = ON_String::Length(option_name);
    if (option_name_length < 1)
      continue;
    if (ON_String::EqualOrdinal(option_name, option_name_length, s, option_name_length, true))
      return s + option_name_length;
  }
  return nullptr;
}

static const ON_String Internal_ParseOptionTail(
  const char* s
)
{
  for (;;)
  {
    if (nullptr == s || ':' != s[0] || s[1] <= ON_String::Space)
      break;
    ON_String tail(s+1);
    tail.TrimRight();
    int len = tail.Length();
    if (len < 1)
      break;
    if ('"' == tail[0] && '"' == tail[len - 1])
      tail.TrimLeftAndRight("\"");
    else if ('\'' == tail[0] && '\'' == tail[len - 1])
      tail.TrimLeftAndRight("'");
    if (tail.Length() < 1)
      break;
    return tail;
  }
  return ON_String::EmptyString;
}


static bool Internal_ParseOption_OUT(const char* s, ON_String& output_file_name)
{
  output_file_name = ON_wString::EmptyString;
  const char* tail = Internal_ParseOptionHead(s, "out" );
  if (nullptr == tail)
    return false;
  output_file_name = Internal_ParseOptionTail(tail);
  return true;
}

static bool Internal_Parse_ParsOption_RECURSE(const char* s, unsigned int& N)
{
  const char* tail = Internal_ParseOptionHead(s, "r", "recurse", "recursive" );
  if (nullptr == tail)
    return false;

  if (0 == tail[0])
  {
    N = 16; // sanity limit of default directory recursion depth
    return true;
  }

  N = 0;
  const ON_String num = Internal_ParseOptionTail(tail);
  if (num.IsNotEmpty())
  {
    unsigned int u = 0;
    const char* s1 = ON_String::ToNumber(num, u, &u);
    if (nullptr != s1 && s1 > static_cast<const char*>(num) && u >= 1 && 0 == s1[0])
      N = u;
  }
  return true;
}

static bool Internal_Parse_ParsOption_VERBOSE(const char* s)
{
  const char* tail = Internal_ParseOptionHead(s, "v", "verbose");
  return (nullptr != tail && 0 == tail[0]);
}

static void Internal_PrintIntroduction(
  const ON_wString& exe_name,
  ON_TextLog& text_log
)
{
  text_log.Print(L"Executable: %ls\n", static_cast<const wchar_t*>(exe_name));
  text_log.Print("Compiled: " __DATE__ " " __TIME__ "\n");
  const ON_wString platform_name =
#if defined(ON_RUNTIME_WIN)
    L"Windows"
#elif defined(ON_RUNTIME_APPLE_IOS)
    L"Apple iOS"
#elif defined(ON_RUNTIME_APPLE_MACOS)
    L"Apple Mac OS"
#elif defined(ON_RUNTIME_APPLE)
    L"Apple"
#elif defined(ON_RUNTIME_ANDROID)
    L"Android"
#elif defined(ON_RUNTIME_LINUX)
    L"Linux"
#else
    L"Unknown"
#endif
    ;
  ON::Version();
  text_log.Print(L"Platform: %ls\n", static_cast<const wchar_t*>(platform_name));
  text_log.Print(
    "Opennurbs version: %u.%u %04u-%02u-%02u %02u:%02u (%u)\n",
    ON::VersionMajor(),
    ON::VersionMinor(),
    ON::VersionYear(),
    ON::VersionMonth(),
    ON::VersionDayOfMonth(),
    ON::VersionHour(),
    ON::VersionMinute(),
    ON::VersionBranch()
  );
  text_log.Print("Current time: ");
  text_log.PrintCurrentTime();
  text_log.Print(" UCT\n");
}

int main( int argc, const char *argv[] )
{
  // If you are using OpenNURBS as a Windows DLL, then you MUST use
  // ON::OpenFile() to open the file.  If you are not using OpenNURBS
  // as a Windows DLL, then you may use either ON::OpenFile() or fopen()
  // to open the file.

  const char* example_test_exe_name = 0;
  if ( argc >= 1 && 0 != argv && 0 != argv[0] && 0 != argv[0][0] )
  {
    on_splitpath(
      argv[0],
      nullptr, // volume
      nullptr, // directory,
      &example_test_exe_name,
      nullptr // extension
      );
  }

  if ( 0 == example_test_exe_name || 0 == example_test_exe_name[0] )
  {
#if defined(ON_OS_WINDOWS)
    example_test_exe_name = "example_test.exe";
#else
    example_test_exe_name = "example_test";
#endif
  }

  const ON_wString exe_name((nullptr != argv && argc > 0) ? argv[0] : ((const char*)nullptr));

  ON_TextLog print_to_stdout;
  print_to_stdout.SetIndentSize(2);

  int argi;
  if ( argc < 2 ) 
  {
    Internal_PrintHelp(example_test_exe_name, print_to_stdout);
    return 0;
  }

  // Call once in your application to initialze opennurbs library
  ON::Begin();

  // default text_log is to stdout
  ON_TextLog* text_log = &print_to_stdout;
  FILE* text_log_fp = nullptr;
  
  unsigned int maximum_directory_depth = 0;

  bool bVerbose = false; // true = output will include a full listing of all 3dm file contents.

  ONX_ErrorCounter err;
  unsigned int file_count = 0;
  unsigned int folder_count = 0;
  unsigned int directory_arg_counter = 0;

  bool bPrintIntroduction = true;

  for ( argi = 1; argi < argc; argi++ ) 
  {
    const char* arg = argv[argi];

    // check for -out:<file name> option
    ON_String output_file_name;
    if ( Internal_ParseOption_OUT(arg,output_file_name) )
    {
      // need a new introduction when output destination changes.
      bPrintIntroduction = true;

      if (output_file_name.IsEmpty())
        continue; // invalid option systax

      // change destination of text_log file
      if ( text_log != &print_to_stdout )
      {
        delete text_log;
        text_log = nullptr;
      }
      if ( text_log_fp )
      {
        ON::CloseFile(text_log_fp);
      }

      FILE* text_fp = ON::OpenFile(output_file_name,"w");
      if ( text_fp )
      {
        text_log_fp = text_fp;
        text_log = new ON_TextLog(text_log_fp);
        text_log->SetIndentSize(2);
      }

      continue;
    }

    // check for -recursive:<directory depth> option
    if (Internal_Parse_ParsOption_RECURSE(arg, maximum_directory_depth))
    {
      continue;
    }

    if (Internal_Parse_ParsOption_VERBOSE(arg))
    {
      bVerbose = true;
      continue;
    }

    ON_wString full_path(arg);

    if ( nullptr == text_log )
      text_log = &print_to_stdout;

    if (bPrintIntroduction)
    {
      bPrintIntroduction = false;
      Internal_PrintIntroduction(exe_name, *text_log);
    }

    err += Internal_Test(
      maximum_directory_depth,
      full_path,
      bVerbose,
      *text_log,
      directory_arg_counter,
      file_count,
      folder_count
    );
  }

  if (bPrintIntroduction)
  {
    bPrintIntroduction = false;
    Internal_PrintIntroduction(exe_name, *text_log);
  }

  if (folder_count > 0)
  {
    text_log->Print(
      L"Tested %u 3dm files in %u folders.",
      file_count,
      folder_count
    );
  }
  else
  {
    text_log->Print(
      L"Tested %u 3dm files.",
      file_count
    );
  }

  if (err.FailureCount() > 0)
    text_log->Print(" Failures. ");
  else if (err.ErrorCount() > 0)
    text_log->Print(" Errors. ");
  else if (err.FailureCount() > 0)
    text_log->Print(" Warnings. ");
  else
    text_log->Print(" All tests passed. ");
  err.Dump(*text_log);
  text_log->PrintNewLine();

  if ( text_log != &print_to_stdout )
  {
    delete text_log;
    text_log = nullptr;
  }

  if ( text_log_fp )
  {
    // close the text text_log file
    ON::CloseFile( text_log_fp );
    text_log_fp = 0;
  }
  
  // OPTIONAL: Call just before your application exits to clean
  //           up opennurbs class definition information.
  //           Opennurbs will not work correctly after ON::End()
  //           is called.
  ON::End();

  return err.TotalCount();
}
