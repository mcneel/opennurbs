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

#include "opennurbs.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#include "opennurbs_internal_glyph.h"

#if defined(ON_INTERNAL_LINUX_FONT_FILES)

// Linux runs RhinoCore, the headless Rhino behind Rhino.Inside and
// Rhino.Compute. It has no platform font engine opennurbs can ask for the
// installed fonts, and RhinoCore does not depend on libfontconfig. This file
// is the Linux counterpart of opennurbs_win_dwrite.cpp and
// opennurbs_apple_nsfont.cpp: it finds the installed font files in the
// directories fontconfig would search, a Fonts folder next to the Rhino
// libraries and the directories in RHINO_FONT_DIRS, and reads every face with
// FreeType to build the installed ON_Font list. The face a managed font draws
// with is opened later by ON_FreeType::CreateFace() (opennurbs_freetype.cpp)
// from the file recorded here.

#include "opennurbs_freetype_include.h"
#include FT_TRUETYPE_TABLES_H // OS/2 table: weight, width, italic, PANOSE
#include <dlfcn.h> // dladdr() finds the directory holding the Rhino libraries

//////////////////////////////////////////////////////////////////////////
//
// Font file table
//
// ON_Font has no field for a file path and the FreeType face for an installed
// font can only be opened from its file, so the file and face index of every
// installed font are kept here, keyed by ON_Font::FontCharacteristicsHash().
//

class ON_LinuxFontFileInfo
{
public:
  ON_LinuxFontFileInfo() = default;
  ~ON_LinuxFontFileInfo() = default;
  ON_LinuxFontFileInfo(const ON_LinuxFontFileInfo&) = default;
  ON_LinuxFontFileInfo& operator=(const ON_LinuxFontFileInfo&) = default;

  ON_SHA1_Hash m_font_characteristics_hash = ON_SHA1_Hash::ZeroDigest;
  ON_wString m_font_file_path;
  int m_face_index = 0;
};

// Function-local so the table exists whenever the installed font list is first
// built, which is during static initialization of ON_Font::Default
// (opennurbs_statics.cpp), before file scope statics in this file would be.
static ON_ClassArray<ON_LinuxFontFileInfo>& Internal_LinuxFontFileTable()
{
  static ON_ClassArray<ON_LinuxFontFileInfo> table(256);
  return table;
}

static void Internal_LinuxAddFontFile(
  const ON_SHA1_Hash& font_characteristics_hash,
  const wchar_t* font_file_path,
  int face_index
)
{
  ON_LinuxFontFileInfo& info = Internal_LinuxFontFileTable().AppendNew();
  info.m_font_characteristics_hash = font_characteristics_hash;
  info.m_font_file_path = font_file_path;
  info.m_face_index = face_index;
}

//////////////////////////////////////////////////////////////////////////
//
// Font directories
//

static const ON_wString Internal_LinuxEnvironmentVariable(const char* name)
{
  const char* value = (nullptr != name && 0 != name[0]) ? getenv(name) : nullptr;
  ON_wString s(value); // UTF-8
  s.TrimLeftAndRight();
  return s;
}

static const ON_wString Internal_LinuxHomeDirectory()
{
  return Internal_LinuxEnvironmentVariable("HOME");
}

// Expands a leading "~" and removes trailing directory separators.
static const ON_wString Internal_LinuxCleanDirectoryName(const ON_wString& dirty_name)
{
  ON_wString name(dirty_name);
  name.TrimLeftAndRight();
  if (name.IsEmpty())
    return ON_wString::EmptyString;
  if ('~' == name[0] && (1 == name.Length() || ON_FileSystemPath::IsDirectorySeparator(name[1], true)))
  {
    const ON_wString home = Internal_LinuxHomeDirectory();
    if (home.IsEmpty())
      return ON_wString::EmptyString;
    name = home + ON_wString(static_cast<const wchar_t*>(name) + 1);
  }
  int length = name.Length();
  while (length > 1 && ON_FileSystemPath::IsDirectorySeparator(name[length - 1], true))
    --length;
  return ON_wString(static_cast<const wchar_t*>(name), length);
}

static void Internal_LinuxAppendFontDirectory(
  const ON_wString& dirty_name,
  ON_ClassArray<ON_wString>& font_directories
)
{
  const ON_wString name = Internal_LinuxCleanDirectoryName(dirty_name);
  if (name.IsEmpty())
    return;
  for (int i = 0; i < font_directories.Count(); ++i)
  {
    if (font_directories[i].EqualOrdinal(name, false))
      return;
  }
  font_directories.Append(name);
}

// Appends <entry>/<subdirectory> for every entry of a colon separated path list.
static void Internal_LinuxAppendFontDirectoriesFromPathList(
  const ON_wString& path_list,
  const wchar_t* subdirectory,
  ON_ClassArray<ON_wString>& font_directories
)
{
  const wchar_t* s = static_cast<const wchar_t*>(path_list);
  if (nullptr == s)
    return;
  for (;;)
  {
    const wchar_t* e = s;
    while (0 != *e && ':' != *e)
      ++e;
    if (e > s)
    {
      ON_wString directory(s, (int)(e - s));
      if (nullptr != subdirectory && 0 != subdirectory[0])
        directory = ON_FileSystemPath::CombinePaths(directory, false, subdirectory, false, false);
      Internal_LinuxAppendFontDirectory(directory, font_directories);
    }
    if (0 == *e)
      break;
    s = e + 1;
  }
}

// Reads the <dir> elements of fontconfig's configuration so the distribution's
// and administrator's font directories are honored without libfontconfig.
static void Internal_LinuxAppendFontConfigDirectories(
  const char* fonts_conf_path,
  ON_ClassArray<ON_wString>& font_directories
)
{
  FILE* fp = ON::OpenFile(fonts_conf_path, "rb");
  if (nullptr == fp)
    return;
  ON_SimpleArray<char> text(16 * 1024);
  char buffer[4096];
  for (;;)
  {
    const size_t n = fread(buffer, 1, sizeof(buffer), fp);
    if (n <= 0)
      break;
    text.Append((int)n, buffer);
    if (text.Count() > 1024 * 1024)
      break; // not a fonts.conf
  }
  ON::CloseFile(fp);
  const char terminator = 0;
  text.Append(terminator);

  const char* s = text.Array();
  for (;;)
  {
    const char* tag = strstr(s, "<dir");
    if (nullptr == tag)
      break;
    const char* tag_end = strchr(tag, '>');
    if (nullptr == tag_end)
      break;
    const char c = tag[4];
    if ('>' != c && ' ' != c && '\t' != c && '\r' != c && '\n' != c)
    {
      s = tag + 4; // some other element
      continue;
    }
    if ('/' == tag_end[-1])
    {
      s = tag_end + 1; // empty element
      continue;
    }
    const char* text_begin = tag_end + 1;
    const char* text_end = strstr(text_begin, "</dir>");
    if (nullptr == text_end)
      break;

    const ON_String attributes(tag + 4, (int)(tag_end - tag - 4));
    ON_wString directory(text_begin, (int)(text_end - text_begin)); // UTF-8
    directory.TrimLeftAndRight();
    if (directory.IsNotEmpty())
    {
      if (nullptr != strstr(attributes, "prefix=\"xdg\""))
      {
        ON_wString xdg_data_home = Internal_LinuxEnvironmentVariable("XDG_DATA_HOME");
        if (xdg_data_home.IsEmpty())
          xdg_data_home = ON_FileSystemPath::CombinePaths(Internal_LinuxHomeDirectory(), false, L".local/share", false, false);
        directory = ON_FileSystemPath::CombinePaths(xdg_data_home, false, directory, false, false);
      }
      else if (nullptr != strstr(attributes, "prefix=\"default\"") || nullptr != strstr(attributes, "prefix=\"cwd\""))
      {
        directory = ON_wString::EmptyString; // relative to the current directory - meaningless for a server process
      }
      if (directory.IsNotEmpty())
        Internal_LinuxAppendFontDirectory(directory, font_directories);
    }
    s = text_end + 6;
  }
}

static bool Internal_LinuxIsFontFile(const ON_wString& path)
{
  // FreeType reads all of these. Bitmap formats are deliberately left out:
  // opennurbs needs outlines.
  const ON_wString ext = ON_FileSystemPath::FileNameExtensionFromPath(path); // includes the leading period
  return
    ext.EqualOrdinal(L".ttf", true)
    || ext.EqualOrdinal(L".otf", true)
    || ext.EqualOrdinal(L".ttc", true)
    || ext.EqualOrdinal(L".otc", true)
    || ext.EqualOrdinal(L".pfb", true)
    || ext.EqualOrdinal(L".pfa", true);
}

// Appends every font file below directory to font_files.
static void Internal_LinuxScanFontDirectory(
  const ON_wString& directory,
  int depth,
  ON_ClassArray<ON_wString>& visited_directories,
  ON_ClassArray<ON_wString>& font_files
)
{
  if (depth > 8 || directory.IsEmpty())
    return; // symbolic link cycle or a pathological tree
  if (false == ON_FileSystem::IsDirectory(static_cast<const wchar_t*>(directory)))
    return;
  for (int i = 0; i < visited_directories.Count(); ++i)
  {
    if (visited_directories[i].EqualOrdinal(directory, false))
      return;
  }
  visited_directories.Append(directory);

  ON_ClassArray<ON_wString> subdirectories;
  ON_FileIterator fit;
  if (false == fit.Initialize(static_cast<const wchar_t*>(directory)))
    return;
  for (bool bHaveItem = fit.FirstItem(); bHaveItem; bHaveItem = fit.NextItem())
  {
    if (fit.CurrentItemIsHidden())
      continue;
    const ON_wString path = fit.CurrentItemFullPathName();
    if (path.IsEmpty())
      continue;
    if (fit.CurrentItemIsDirectory())
    {
      subdirectories.Append(path);
      continue;
    }
    if (false == fit.CurrentItemIsFile())
      continue;
    if (Internal_LinuxIsFontFile(path))
      font_files.Append(path);
  }

  // Recurse after this directory's iterator is done so only one directory is open at a time.
  for (int i = 0; i < subdirectories.Count(); ++i)
    Internal_LinuxScanFontDirectory(subdirectories[i], depth + 1, visited_directories, font_files);
}

// Directory holding the opennurbs library, or the executable when opennurbs
// is linked statically. A RhinoCore deployment is flat, so this is also the
// libRhinoCore directory.
static const ON_wString Internal_LinuxOpenNurbsDirectory()
{
  Dl_info info;
  memset(&info, 0, sizeof(info));
  if (0 == dladdr((void*)&ON_ManagedFonts::InstalledFonts, &info) || nullptr == info.dli_fname || 0 == info.dli_fname[0])
    return ON_wString::EmptyString;
  return ON_FileSystemPath::VolumeAndDirectoryFromPath(ON_wString(info.dli_fname));
}

// The font files in every directory the Linux scan covers.
static void Internal_LinuxCollectFontFiles(ON_ClassArray<ON_wString>& font_files)
{
  ON_ClassArray<ON_wString> font_directories(16);

  // InstalledFonts() keeps the first of two fonts with identical
  // characteristics, so the directories the user and the Rhino installation
  // control come before the system directories.

  // RHINO_FONT_DIRS: colon separated list of extra font directories. This is
  // how a container or a Rhino.Compute server points Rhino at fonts that are
  // not installed system wide.
  Internal_LinuxAppendFontDirectoriesFromPathList(Internal_LinuxEnvironmentVariable("RHINO_FONT_DIRS"), nullptr, font_directories);

  // The fonts Rhino ships (SLF-RHN Architect, MecSoft_Font-1, NotoEmoji-Regular,
  // Y14.5-2018) are installed as Fonts/ next to the Rhino libraries
  // (src4/rhino4/CMakeLists.txt). The Windows installer registers the same
  // files as system fonts.
  const ON_wString rhino_directory = Internal_LinuxOpenNurbsDirectory();
  if (rhino_directory.IsNotEmpty())
    Internal_LinuxAppendFontDirectory(ON_FileSystemPath::CombinePaths(rhino_directory, false, L"Fonts", false, false), font_directories);

  // The directories fontconfig searches by default.
  Internal_LinuxAppendFontDirectory(L"/usr/share/fonts", font_directories);
  Internal_LinuxAppendFontDirectory(L"/usr/local/share/fonts", font_directories);
  ON_wString xdg_data_home = Internal_LinuxEnvironmentVariable("XDG_DATA_HOME");
  if (xdg_data_home.IsEmpty())
    xdg_data_home = L"~/.local/share";
  Internal_LinuxAppendFontDirectory(ON_FileSystemPath::CombinePaths(xdg_data_home, false, L"fonts", false, false), font_directories);
  Internal_LinuxAppendFontDirectory(L"~/.fonts", font_directories);
  ON_wString xdg_data_dirs = Internal_LinuxEnvironmentVariable("XDG_DATA_DIRS");
  if (xdg_data_dirs.IsEmpty())
    xdg_data_dirs = L"/usr/local/share:/usr/share";
  Internal_LinuxAppendFontDirectoriesFromPathList(xdg_data_dirs, L"fonts", font_directories);

  // Distribution and administrator customizations.
  Internal_LinuxAppendFontConfigDirectories("/etc/fonts/fonts.conf", font_directories);

  ON_ClassArray<ON_wString> visited_directories(64);
  for (int i = 0; i < font_directories.Count(); ++i)
    Internal_LinuxScanFontDirectory(font_directories[i], 0, visited_directories, font_files);
}

//////////////////////////////////////////////////////////////////////////
//
// Font faces
//

static ON_Font::Stretch Internal_LinuxStretchFromOS2WidthClass(unsigned int os2_usWidthClass)
{
  // OS/2 usWidthClass 1 (ultra-condensed) ... 5 (medium) ... 9 (ultra-expanded)
  // is the same scale as ON_Font::Stretch.
  if (os2_usWidthClass >= 1 && os2_usWidthClass <= 9)
    return static_cast<ON_Font::Stretch>(static_cast<unsigned char>(os2_usWidthClass));
  return ON_Font::Stretch::Medium;
}

// Reads face face_index of a font file into font_characteristics. face_count
// is set from the file (.ttc collections have several faces).
// Returns false when the face is missing or has no outlines.
static bool Internal_LinuxReadFontFace(
  FT_Library freetype_library,
  const ON_String& utf8_path,
  FT_Long face_index,
  FT_Long& face_count,
  ON_Font& font_characteristics
)
{
  FT_Face face = nullptr;
  if (FT_Err_Ok != FT_New_Face(freetype_library, utf8_path, face_index, &face) || nullptr == face)
    return false;

  bool rc = false;
  for (;;)
  {
    face_count = face->num_faces;
    if (false == FT_IS_SCALABLE(face))
      break; // bitmap strike - no outlines
    if (nullptr == face->family_name || 0 == face->family_name[0])
      break;

    // FreeType prefers the typographic (name id 16/17) names, so
    // "DejaVu Sans Condensed Bold" arrives as family "DejaVu Sans" and
    // style "Condensed Bold" the way Core Text reports it on macOS.
    const ON_wString family_name(face->family_name);
    ON_wString face_name(face->style_name);
    face_name.TrimLeftAndRight();
    if (face_name.IsEmpty())
      face_name = L"Regular";
    ON_wString postscript_name(FT_Get_Postscript_Name(face));
    postscript_name.TrimLeftAndRight();
    if (postscript_name.IsEmpty())
      postscript_name = family_name; // ON_Font needs at least one of the names

    ON_Font::Weight weight = ON_Font::Weight::Normal;
    ON_Font::Stretch stretch = ON_Font::Stretch::Medium;
    ON_Font::Style style = ON_Font::Style::Upright;
    int windows_logfont_weight = 400;
    ON_PANOSE1 panose1 = ON_PANOSE1::Zero;

    const TT_OS2* os2 = (const TT_OS2*)FT_Get_Sfnt_Table(face, ft_sfnt_os2);
    if (nullptr != os2 && 0xFFFF != os2->version)
    {
      if (os2->usWeightClass > 0 && os2->usWeightClass <= 1000)
      {
        windows_logfont_weight = (int)os2->usWeightClass;
        weight = ON_Font::WeightFromWindowsLogfontWeight(windows_logfont_weight);
      }
      stretch = Internal_LinuxStretchFromOS2WidthClass(os2->usWidthClass);
      if (0 != (0x0200 & os2->fsSelection))
        style = ON_Font::Style::Oblique; // fsSelection bit 9 OBLIQUE
      else if (0 != (0x0001 & os2->fsSelection))
        style = ON_Font::Style::Italic; // fsSelection bit 0 ITALIC
      if (os2->panose[0] <= 5) // valid PANOSE 1.0 family kind
        panose1.SetTenBytes(os2->panose);
    }
    else if (0 != (FT_STYLE_FLAG_BOLD & face->style_flags))
    {
      weight = ON_Font::Weight::Bold;
      windows_logfont_weight = 700;
    }
    if (ON_Font::Style::Upright == style && 0 != (FT_STYLE_FLAG_ITALIC & face->style_flags))
      style = ON_Font::Style::Italic;

    // Windows LOGFONT name so this font can be found by the name RTF runs and
    // Rhino 6/7 files use. Internal_SetFakeWindowsLogfontNames() refines it
    // for families with more than four faces.
    ON_wString windows_logfont_name = ON_Font::FakeWindowsLogfontNameFromFamilyAndPostScriptNames(family_name, postscript_name);
    if (windows_logfont_name.IsEmpty())
      windows_logfont_name = family_name;

    rc = font_characteristics.SetFontCharacteristicsForExperts(
      0.0, // point size: annotation size (units per em)
      postscript_name,
      windows_logfont_name,
      ON_FontFaceQuartet::Member::Unset,
      family_name,
      face_name,
      weight,
      style,
      stretch,
      false,
      false,
      ON_Font::WindowsConstants::logfont_default_charset,
      windows_logfont_weight,
      ON_UNSET_VALUE, // no Apple weight trait
      panose1
    );
    break;
  }

  FT_Done_Face(face);
  return rc;
}

//////////////////////////////////////////////////////////////////////////
//
// ON_ManagedFonts
//

void ON_ManagedFonts::Internal_GetLinuxInstalledFonts(
  ON_SimpleArray<const ON_Font*>& device_list
)
{
  ON_ClassArray<ON_wString> font_files(256);
  Internal_LinuxCollectFontFiles(font_files);
  if (font_files.Count() <= 0)
    return;

  // FreeType library used only to read names and properties while scanning.
  // Faces are closed as soon as they are read and the library is released at
  // the end of the scan; glyph faces come from ON_FreeType::Library().
  // (Keeping this in a static left the FT_Init_FreeType blocks reachable at
  // exit, which the valgrind memcheck runs report as leaks.)
  FT_Library freetype_library = nullptr;
  if (FT_Err_Ok != FT_Init_FreeType(&freetype_library) || nullptr == freetype_library)
    return;

  for (int i = 0; i < font_files.Count(); ++i)
  {
    const ON_wString& font_file_path = font_files[i];
    const ON_String utf8_path(font_file_path);
    FT_Long face_count = 1;
    for (FT_Long face_index = 0; face_index < face_count; ++face_index)
    {
      ON_Font font_characteristics;
      if (false == Internal_LinuxReadFontFace(freetype_library, utf8_path, face_index, face_count, font_characteristics))
        continue;

      // It is critical that m_font_type = ON_Font::FontType::InstalledFont (same as the Apple provider).
      ON_Font* installed_font = new ON_Font(ON_Font::FontType::InstalledFont, font_characteristics);

      // The hash is cached on the font now, before Internal_SetFakeWindowsLogfontNames()
      // renames multi-face families, so it stays the key Internal_LinuxFontFile() looks up.
      Internal_LinuxAddFontFile(installed_font->FontCharacteristicsHash(), font_file_path, (int)face_index);
      device_list.Append(installed_font);
    }
  }

  FT_Done_FreeType(freetype_library);
}

bool ON_ManagedFonts::Internal_LinuxFontFile(
  const ON_Font& font,
  const ON_Font*& installed_font,
  ON_wString& font_file_path,
  int& face_index
)
{
  installed_font = nullptr;
  font_file_path = ON_wString::EmptyString;
  face_index = 0;

  const ON_Font* f = nullptr;
  if (ON_Font::FontType::InstalledFont == font.m_font_type)
    f = &font;
  else if (font.IsManagedInstalledFont() || font.IsManagedSubstitutedFont())
    f = font.Internal_ManagedFontToInstalledFont();
  if (nullptr != f && ON_Font::FontType::InstalledFont != f->m_font_type)
    f = nullptr; // a managed font standing in for a missing font has no file
  if (nullptr == f)
    f = InstalledFonts().FromFontProperties(&font, false, false);
  if (nullptr == f)
    return false; // not installed and no substitute was assigned (Internal_AddManagedFont)

  const ON_SHA1_Hash hash = f->FontCharacteristicsHash();
  const ON_ClassArray<ON_LinuxFontFileInfo>& table = Internal_LinuxFontFileTable();
  const int count = table.Count();
  for (int i = 0; i < count; ++i)
  {
    const ON_LinuxFontFileInfo& info = table[i];
    if (hash == info.m_font_characteristics_hash)
    {
      installed_font = f;
      font_file_path = info.m_font_file_path;
      face_index = info.m_face_index;
      return true;
    }
  }
  return false;
}

const ON_Font* ON_ManagedFonts::Internal_LinuxSubstituteFont(const ON_Font* missing_font)
{
  const ON_FontList& installed_fonts = InstalledFonts();
  if (installed_fonts.Count() <= 0)
    return nullptr;

  ON_Font::Weight weight = ON_Font::Weight::Normal;
  ON_Font::Stretch stretch = ON_Font::Stretch::Medium;
  ON_Font::Style style = ON_Font::Style::Upright;
  ON_wString names[3];
  if (nullptr != missing_font)
  {
    if (ON_Font::Weight::Unset != missing_font->FontWeight())
      weight = missing_font->FontWeight();
    if (ON_Font::Stretch::Unset != missing_font->FontStretch())
      stretch = missing_font->FontStretch();
    if (ON_Font::Style::Unset != missing_font->FontStyle())
      style = missing_font->FontStyle();
    names[0] = missing_font->FamilyName();
    names[1] = missing_font->WindowsLogfontName();
    names[2] = missing_font->PostScriptName();
  }

  // Families files made on Windows and macOS commonly reference, and the
  // family found on stock Linux images that stands in for each. Liberation
  // Sans/Serif/Mono are metric compatible with Arial/Times New Roman/Courier
  // New (fonts-liberation on Debian/Ubuntu, liberation-fonts on Fedora/Amazon
  // Linux, both dependencies of the rhino3d package).
  struct LinuxSubstitute
  {
    const wchar_t* m_missing_family;
    const wchar_t* m_substitute_family;
  };
  static const LinuxSubstitute substitute_table[] =
  {
    { L"Arial", L"Liberation Sans" },
    { L"Arial Narrow", L"Liberation Sans Narrow" },
    { L"Helvetica", L"Liberation Sans" },
    { L"Helvetica Neue", L"Liberation Sans" },
    { L"Times New Roman", L"Liberation Serif" },
    { L"Times", L"Liberation Serif" },
    { L"Courier New", L"Liberation Mono" },
    { L"Courier", L"Liberation Mono" },
    { L"Segoe UI", L"DejaVu Sans" },
    { L"Tahoma", L"DejaVu Sans" },
    { L"Verdana", L"DejaVu Sans" },
    { L"Calibri", L"Carlito" },
    { L"Cambria", L"Caladea" },
    { L"Consolas", L"DejaVu Sans Mono" },
    { L"Menlo", L"DejaVu Sans Mono" },
    { L"Monaco", L"DejaVu Sans Mono" },
  };
  const size_t substitute_count = sizeof(substitute_table) / sizeof(substitute_table[0]);

  const ON_Font* f = nullptr;

  // Family name, then LOGFONT name, then the family the PostScript name implies ("ArialMT" -> "Arial").
  for (int pass = 0; pass < 3 && nullptr == f; ++pass)
  {
    ON_wString name = names[pass];
    name.TrimLeftAndRight();
    if (name.IsEmpty())
      continue;
    if (2 == pass)
      name = ON_Font::FamilyNameFromDirtyName(name);
    for (size_t i = 0; i < substitute_count && nullptr == f; ++i)
    {
      if (name.EqualOrdinal(substitute_table[i].m_missing_family, true))
        f = installed_fonts.FamilyMemberWithWeightStretchStyle(substitute_table[i].m_substitute_family, weight, stretch, style);
    }
  }

  // The family the default font resolved to.
  if (nullptr == f && missing_font != &ON_Font::Default)
  {
    const ON_Font* default_installed_font
      = (ON_Font::Default.IsManagedInstalledFont() || ON_Font::Default.IsManagedSubstitutedFont())
      ? ON_Font::Default.Internal_ManagedFontToInstalledFont()
      : nullptr;
    if (nullptr != default_installed_font && ON_Font::FontType::InstalledFont == default_installed_font->m_font_type)
      f = installed_fonts.FamilyMemberWithWeightStretchStyle(default_installed_font->FamilyName(), weight, stretch, style);
  }

  // Sans-serif families in order of preference.
  static const wchar_t* preferred_sans_serif_families[] =
  {
    L"Liberation Sans", L"DejaVu Sans", L"Noto Sans", L"Open Sans", L"FreeSans",
    L"Nimbus Sans", L"Cantarell", L"Ubuntu", L"Roboto"
  };
  const size_t preferred_count = sizeof(preferred_sans_serif_families) / sizeof(preferred_sans_serif_families[0]);
  for (size_t i = 0; i < preferred_count && nullptr == f; ++i)
    f = installed_fonts.FamilyMemberWithWeightStretchStyle(preferred_sans_serif_families[i], weight, stretch, style);

  // The first sans-serif family found: PANOSE says Latin text with a sans
  // serif style, then a family name containing "Sans", then anything at all.
  if (nullptr == f)
  {
    const ON_SimpleArray<const ON_Font*>& by_family_name = installed_fonts.ByFamilyName();
    for (int pass = 0; pass < 3 && nullptr == f; ++pass)
    {
      for (int i = 0; i < by_family_name.Count() && nullptr == f; ++i)
      {
        const ON_Font* candidate = by_family_name[i];
        if (nullptr == candidate)
          continue;
        if (0 == pass)
        {
          if (ON_PANOSE1::FamilyKind::LatinText != candidate->m_panose1.PANOSE1FamilyKind())
            continue;
          const ON__UINT8 serif_style = candidate->m_panose1.TenBytes()[1];
          if (serif_style < 11 || serif_style > 15)
            continue; // PANOSE 1.0 Latin text serif styles 11-15 are the sans serifs
        }
        else if (1 == pass)
        {
          if (candidate->FamilyName().Find(L"Sans") < 0)
            continue;
        }
        f = installed_fonts.FamilyMemberWithWeightStretchStyle(candidate->FamilyName(), weight, stretch, style);
      }
    }
  }

  return f;
}

#endif // ON_INTERNAL_LINUX_FONT_FILES
