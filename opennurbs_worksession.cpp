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

// The file format matches CRhWorkSession in RhWorkSession.cpp, which is what Rhino reads and writes.

static const unsigned int ON_WS_HeaderSize = 74;
static const ON__UINT32 ON_WS_V4ReferenceModelTypecode = 0x00020000;
static const ON__UINT32 ON_WS_V4LayerTypecode = 0x00100000;

// Readers reject files with more than this many layer overrides for one model.
static const int ON_WS_MaxLayerOverrideCount = 10000;

static const char* ON_WS_V5HeaderText = "Rhinoceros Worksession File Format Version ";
static const unsigned int ON_WS_V5HeaderTextLength = 43;

// Saved as UTF-16 after a 4 byte length that counts the null terminator.
static const char* ON_WS_V4HeaderText = "Rhino worksession description file";
static const unsigned int ON_WS_V4HeaderTextLength = 34;

static void ON_WS_V4Header(unsigned char buffer[ON_WS_HeaderSize])
{
  memset(buffer, 0, ON_WS_HeaderSize);
  buffer[0] = (unsigned char)(ON_WS_V4HeaderTextLength + 1);
  for (unsigned int i = 0; i < ON_WS_V4HeaderTextLength; i++)
    buffer[4 + 2 * i] = (unsigned char)ON_WS_V4HeaderText[i];
}

static bool ON_WS_WriteHeader(ON_BinaryArchive& archive, unsigned int version)
{
  unsigned char buffer[ON_WS_HeaderSize];

  if (4 == version)
  {
    ON_WS_V4Header(buffer);
  }
  else if (5 == version)
  {
    // The first 4 bytes are zero so Rhino 4 reads them as an empty string and stops.
    memset(buffer, 0, ON_WS_HeaderSize);
    memcpy(&buffer[4], ON_WS_V5HeaderText, ON_WS_V5HeaderTextLength);
    buffer[4 + ON_WS_V5HeaderTextLength] = (unsigned char)('0' + version);
  }
  else
  {
    return false;
  }

  return archive.WriteByte(ON_WS_HeaderSize, buffer);
}

// Returns 0 and leaves the archive where it was when the header is not a worksession header.
static unsigned int ON_WS_ReadHeader(ON_BinaryArchive& archive)
{
  unsigned char buffer[ON_WS_HeaderSize];
  memset(buffer, 0, sizeof(buffer));

  const ON__UINT64 pos0 = archive.CurrentPosition();
  if (!archive.ReadByte(ON_WS_HeaderSize, buffer))
  {
    const ON__UINT64 pos1 = archive.CurrentPosition();
    if (pos0 < pos1 && pos1 <= pos0 + ON_WS_HeaderSize)
      archive.SeekBackward(pos1 - pos0);
    return 0;
  }

  if (0 == buffer[0] && 0 == buffer[1] && 0 == buffer[2] && 0 == buffer[3]
    && 0 == memcmp(&buffer[4], ON_WS_V5HeaderText, ON_WS_V5HeaderTextLength))
  {
    // Same rules as Rhino: optional spaces, the version digits, then zeros to the end.
    unsigned int i = 4 + ON_WS_V5HeaderTextLength;
    while (i < ON_WS_HeaderSize && ' ' == buffer[i])
      i++;
    if (i < ON_WS_HeaderSize && buffer[i] >= '1' && buffer[i] <= '9')
    {
      unsigned int version = 0;
      while (i < ON_WS_HeaderSize && buffer[i] >= '0' && buffer[i] <= '9' && version < 100000000)
        version = 10 * version + (unsigned int)(buffer[i++] - '0');
      if (i < ON_WS_HeaderSize)
      {
        while (i < ON_WS_HeaderSize && 0 == buffer[i])
          i++;
        if (ON_WS_HeaderSize == i)
          return version;
      }
    }
  }
  else
  {
    unsigned char v4[ON_WS_HeaderSize];
    ON_WS_V4Header(v4);
    if (0 == memcmp(buffer, v4, ON_WS_HeaderSize))
      return 4;
  }

  archive.SeekBackward(ON_WS_HeaderSize);
  return 0;
}

// The embedded ON_Layer is the only part of a worksession file at the current 3dm version, with
// 8 byte chunk lengths. There is no component manifest, so its component indices are not mapped.
class ON_WS_EmbeddedLayerArchiveScope
{
public:
  ON_WS_EmbeddedLayerArchiveScope(ON_BinaryArchive& archive)
    : m_archive(archive)
    , m_3dm_version(archive.Archive3dmVersion())
    , m_opennurbs_version(archive.ArchiveOpenNURBSVersion())
    , m_bIndexMapping(archive.ReferencedComponentIndexMapping())
  {
    archive.SetArchive3dmVersion(ON_BinaryArchive::CurrentArchiveVersion());
    archive.SetOpenNURBS3dmVersion(ON::Version());
    archive.SetReferencedComponentIndexMapping(false);
  }

  ~ON_WS_EmbeddedLayerArchiveScope()
  {
    m_archive.SetReferencedComponentIndexMapping(m_bIndexMapping);
    m_archive.SetArchive3dmVersion(m_3dm_version);
    m_archive.SetOpenNURBS3dmVersion(m_opennurbs_version);
  }

  ON_WS_EmbeddedLayerArchiveScope(const ON_WS_EmbeddedLayerArchiveScope&) = delete;
  ON_WS_EmbeddedLayerArchiveScope& operator=(const ON_WS_EmbeddedLayerArchiveScope&) = delete;

private:
  ON_BinaryArchive& m_archive;
  const int m_3dm_version;
  const unsigned int m_opennurbs_version;
  const bool m_bIndexMapping;
};

class ON_WorksessionLayerValues_Private
{
public:
  bool m_bVisible = true;
  bool m_bLocked = false;
  ON_Color m_color = ON_Color::Black;
  ON_Layer m_layer;
};

class ON_WorksessionLayerOverride_Private
{
public:
  ON_UUID m_layer_id = ON_nil_uuid;
  ON_WorksessionLayerValues m_file_values;
  ON_WorksessionLayerValues m_worksession_values;
};

class ON_WorksessionModel_Private
{
public:
  bool m_bActive = false;
  ON_wString m_full_path;
  ON_wString m_relative_path;
  ON_ClassArray<ON_WorksessionLayerOverride> m_layer_overrides;
  ON_WorksessionLayerValues m_parent_layer_values;
};

class ON_WorksessionFile_Private
{
public:
  unsigned int m_version = 0;
  ON_ClassArray<ON_WorksessionModel> m_models;

  // Readers fill the private members directly. Copying a layer in afterwards would change the
  // copy count saved with its per-viewport settings, so an unchanged file would not write the same bytes.
  static bool WriteLayerValues(ON_BinaryArchive& archive, const ON_WorksessionLayerValues& values);
  static bool ReadLayerValues(ON_BinaryArchive& archive, ON_WorksessionLayerValues& values);
  static bool WriteLayerOverride(ON_BinaryArchive& archive, const ON_WorksessionLayerOverride& layer_override);
  static bool ReadLayerOverride(ON_BinaryArchive& archive, ON_WorksessionLayerOverride& layer_override);
  static bool WriteV5Model(ON_BinaryArchive& archive, const ON_WorksessionModel& model);
  static bool ReadV5Model(ON_BinaryArchive& archive, ON_WorksessionModel& model);
  static bool WriteV5(ON_BinaryArchive& archive, const ON_ClassArray<ON_WorksessionModel>& models);
  static bool ReadV5(ON_BinaryArchive& archive, ON_ClassArray<ON_WorksessionModel>& models);
  static bool WriteV4Model(ON_BinaryArchive& archive, const ON_WorksessionModel& model);
  static bool ReadV4Model(ON_BinaryArchive& archive, ON_WorksessionModel& model);
  static bool WriteV4(ON_BinaryArchive& archive, const ON_ClassArray<ON_WorksessionModel>& models);
  static bool CanWrite(const ON_ClassArray<ON_WorksessionModel>& models, unsigned int version);
  static bool ReadV4(ON_BinaryArchive& archive, ON_ClassArray<ON_WorksessionModel>& models);
};


ON_WorksessionLayerValues::ON_WorksessionLayerValues() : m_private(new ON_WorksessionLayerValues_Private) {}
ON_WorksessionLayerValues::~ON_WorksessionLayerValues() = default;
ON_WorksessionLayerValues::ON_WorksessionLayerValues(const ON_WorksessionLayerValues& src)
  : m_private(new ON_WorksessionLayerValues_Private(*src.m_private)) {}

ON_WorksessionLayerValues& ON_WorksessionLayerValues::operator=(const ON_WorksessionLayerValues& src)
{
  if (this != &src)
    *m_private = *src.m_private;
  return *this;
}

bool ON_WorksessionLayerValues::Visible() const { return m_private->m_bVisible; }
void ON_WorksessionLayerValues::SetVisible(bool bVisible) { m_private->m_bVisible = bVisible; }
bool ON_WorksessionLayerValues::Locked() const { return m_private->m_bLocked; }
void ON_WorksessionLayerValues::SetLocked(bool bLocked) { m_private->m_bLocked = bLocked; }
ON_Color ON_WorksessionLayerValues::Color() const { return m_private->m_color; }
void ON_WorksessionLayerValues::SetColor(ON_Color color) { m_private->m_color = color; }
const ON_Layer& ON_WorksessionLayerValues::Layer() const { return m_private->m_layer; }
void ON_WorksessionLayerValues::SetLayer(const ON_Layer& layer) { m_private->m_layer = layer; }

ON_WorksessionLayerOverride::ON_WorksessionLayerOverride() : m_private(new ON_WorksessionLayerOverride_Private) {}
ON_WorksessionLayerOverride::~ON_WorksessionLayerOverride() = default;
ON_WorksessionLayerOverride::ON_WorksessionLayerOverride(const ON_WorksessionLayerOverride& src)
  : m_private(new ON_WorksessionLayerOverride_Private(*src.m_private)) {}

ON_WorksessionLayerOverride& ON_WorksessionLayerOverride::operator=(const ON_WorksessionLayerOverride& src)
{
  if (this != &src)
    *m_private = *src.m_private;
  return *this;
}

ON_UUID ON_WorksessionLayerOverride::LayerId() const { return m_private->m_layer_id; }
void ON_WorksessionLayerOverride::SetLayerId(const ON_UUID& layer_id) { m_private->m_layer_id = layer_id; }
const ON_WorksessionLayerValues& ON_WorksessionLayerOverride::FileValues() const { return m_private->m_file_values; }
void ON_WorksessionLayerOverride::SetFileValues(const ON_WorksessionLayerValues& values) { m_private->m_file_values = values; }
const ON_WorksessionLayerValues& ON_WorksessionLayerOverride::WorksessionValues() const { return m_private->m_worksession_values; }
void ON_WorksessionLayerOverride::SetWorksessionValues(const ON_WorksessionLayerValues& values) { m_private->m_worksession_values = values; }

ON_WorksessionModel::ON_WorksessionModel() : m_private(new ON_WorksessionModel_Private) {}
ON_WorksessionModel::~ON_WorksessionModel() = default;
ON_WorksessionModel::ON_WorksessionModel(const ON_WorksessionModel& src)
  : m_private(new ON_WorksessionModel_Private(*src.m_private)) {}

ON_WorksessionModel& ON_WorksessionModel::operator=(const ON_WorksessionModel& src)
{
  if (this != &src)
    *m_private = *src.m_private;
  return *this;
}

bool ON_WorksessionModel::IsActive() const { return m_private->m_bActive; }
void ON_WorksessionModel::SetActive(bool bActive) { m_private->m_bActive = bActive; }
const ON_wString ON_WorksessionModel::FullPath() const { return m_private->m_full_path; }
void ON_WorksessionModel::SetFullPath(const wchar_t* full_path) { m_private->m_full_path = full_path; }
const ON_wString ON_WorksessionModel::RelativePath() const { return m_private->m_relative_path; }
void ON_WorksessionModel::SetRelativePath(const wchar_t* relative_path) { m_private->m_relative_path = relative_path; }
int ON_WorksessionModel::LayerOverrideCount() const { return m_private->m_layer_overrides.Count(); }

const ON_WorksessionLayerOverride* ON_WorksessionModel::LayerOverride(int index) const
{
  return (index >= 0 && index < m_private->m_layer_overrides.Count()) ? &m_private->m_layer_overrides[index] : nullptr;
}

bool ON_WorksessionModel::SetLayerOverride(int index, const ON_WorksessionLayerOverride& layer_override)
{
  if (index < 0 || index >= m_private->m_layer_overrides.Count())
    return false;
  m_private->m_layer_overrides[index] = layer_override;
  return true;
}

void ON_WorksessionModel::AppendLayerOverride(const ON_WorksessionLayerOverride& layer_override)
{
  m_private->m_layer_overrides.Append(layer_override);
}

bool ON_WorksessionModel::RemoveLayerOverride(int index)
{
  if (index < 0 || index >= m_private->m_layer_overrides.Count())
    return false;
  m_private->m_layer_overrides.Remove(index);
  return true;
}

void ON_WorksessionModel::RemoveAllLayerOverrides() { m_private->m_layer_overrides.Destroy(); }
const ON_WorksessionLayerValues& ON_WorksessionModel::ParentLayerValues() const { return m_private->m_parent_layer_values; }
void ON_WorksessionModel::SetParentLayerValues(const ON_WorksessionLayerValues& values) { m_private->m_parent_layer_values = values; }

bool ON_WorksessionFile_Private::WriteLayerValues(ON_BinaryArchive& archive, const ON_WorksessionLayerValues& values)
{
  if (!archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK, 1, 1))
    return false;

  bool rc = false;
  for (;;)
  {
    if (!archive.WriteBool(values.Visible()))
      break;
    if (!archive.WriteBool(values.Locked()))
      break;
    if (!archive.WriteColor(values.Color()))
      break;

    bool layer_rc = false;
    {
      const ON_WS_EmbeddedLayerArchiveScope scope(archive);
      layer_rc = archive.WriteObject(values.Layer());
    }
    if (!layer_rc)
      break;

    rc = true;
    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool ON_WorksessionFile_Private::ReadLayerValues(ON_BinaryArchive& archive, ON_WorksessionLayerValues& values)
{
  ON_WorksessionLayerValues_Private& v = *values.m_private;
  v = ON_WorksessionLayerValues_Private();

  int major_version = 0;
  int minor_version = 0;
  if (!archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (1 != major_version)
      break;
    if (!archive.ReadBool(&v.m_bVisible))
      break;
    if (!archive.ReadBool(&v.m_bLocked))
      break;
    if (!archive.ReadColor(v.m_color))
      break;

    if (minor_version >= 1)
    {
      bool layer_rc = false;
      {
        const ON_WS_EmbeddedLayerArchiveScope scope(archive);
        layer_rc = (1 == archive.ReadObject(v.m_layer));
      }
      if (!layer_rc)
        break;
    }

    rc = true;
    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;

  return rc;
}

bool ON_WorksessionFile_Private::WriteLayerOverride(ON_BinaryArchive& archive, const ON_WorksessionLayerOverride& layer_override)
{
  if (!archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK, 1, 0))
    return false;

  const bool rc
    = archive.WriteUuid(layer_override.LayerId())
    && WriteLayerValues(archive, layer_override.FileValues())
    && WriteLayerValues(archive, layer_override.WorksessionValues());

  if (!archive.EndWrite3dmChunk())
    return false;

  return rc;
}

bool ON_WorksessionFile_Private::ReadLayerOverride(ON_BinaryArchive& archive, ON_WorksessionLayerOverride& layer_override)
{
  ON_WorksessionLayerOverride_Private& o = *layer_override.m_private;

  int major_version = 0;
  int minor_version = 0;
  if (!archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version))
    return false;

  const bool rc
    = 1 == major_version
    && archive.ReadUuid(o.m_layer_id)
    && ReadLayerValues(archive, o.m_file_values)
    && ReadLayerValues(archive, o.m_worksession_values);

  if (!archive.EndRead3dmChunk())
    return false;

  return rc;
}

bool ON_WorksessionFile_Private::WriteV5Model(ON_BinaryArchive& archive, const ON_WorksessionModel& model)
{
  if (!archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK, 1, 1))
    return false;

  bool rc = false;
  for (;;)
  {
    if (!archive.WriteBool(model.IsActive()))
      break;
    if (!archive.WriteString(model.FullPath()))
      break;
    if (!archive.WriteString(model.RelativePath()))
      break;

    if (!archive.BeginWrite3dmBigChunk(TCODE_ANONYMOUS_CHUNK, 0))
      break;
    const int override_count = model.IsActive() ? 0 : model.LayerOverrideCount();
    bool overrides_rc = archive.WriteInt(override_count);
    for (int i = 0; i < override_count && overrides_rc; i++)
      overrides_rc = WriteLayerOverride(archive, *model.LayerOverride(i));
    if (!archive.EndWrite3dmChunk())
      break;
    if (!overrides_rc)
      break;

    if (!WriteLayerValues(archive, model.ParentLayerValues()))
      break;

    rc = true;
    break;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool ON_WorksessionFile_Private::ReadV5Model(ON_BinaryArchive& archive, ON_WorksessionModel& model)
{
  ON_WorksessionModel_Private& m = *model.m_private;

  int major_version = 0;
  int minor_version = 0;
  if (!archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (1 != major_version)
      break;
    if (!archive.ReadBool(&m.m_bActive))
      break;
    if (!archive.ReadString(m.m_full_path))
      break;
    if (!archive.ReadString(m.m_relative_path))
      break;

    ON__UINT32 typecode = 0;
    ON__INT64 value = 0;
    if (!archive.BeginRead3dmBigChunk(&typecode, &value))
      break;
    int override_count = 0;
    bool overrides_rc
      = TCODE_ANONYMOUS_CHUNK == typecode
      && archive.ReadInt(&override_count)
      && override_count >= 0
      && override_count <= ON_WS_MaxLayerOverrideCount;
    if (overrides_rc)
    {
      m.m_layer_overrides.Reserve((size_t)override_count);
      for (int i = 0; i < override_count && overrides_rc; i++)
        overrides_rc = ReadLayerOverride(archive, m.m_layer_overrides.AppendNew());
    }
    if (!archive.EndRead3dmChunk())
      break;
    if (!overrides_rc)
      break;

    if (minor_version >= 1 && !ReadLayerValues(archive, m.m_parent_layer_values))
      break;

    rc = true;
    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;

  return rc;
}

bool ON_WorksessionFile_Private::WriteV5(ON_BinaryArchive& archive, const ON_ClassArray<ON_WorksessionModel>& models)
{
  if (!archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK, 1, 0))
    return false;

  bool rc = false;
  if (archive.BeginWrite3dmBigChunk(TCODE_ANONYMOUS_CHUNK, 0))
  {
    // Same order as Rhino: the first active model, then the rest in array order.
    int first_active_index = -1;
    for (int i = 0; i < models.Count() && first_active_index < 0; i++)
    {
      if (models[i].IsActive() && models[i].FullPath().IsNotEmpty())
        first_active_index = i;
    }

    rc = true;
    if (first_active_index >= 0)
      rc = WriteV5Model(archive, models[first_active_index]);
    for (int i = 0; i < models.Count() && rc; i++)
    {
      if (i != first_active_index && models[i].FullPath().IsNotEmpty())
        rc = WriteV5Model(archive, models[i]);
    }

    if (rc)
      rc = archive.BeginWrite3dmBigChunk(TCODE_ENDOFTABLE, 0) && archive.EndWrite3dmChunk();

    if (!archive.EndWrite3dmChunk())
      rc = false;
  }

  if (!archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool ON_WorksessionFile_Private::ReadV5(ON_BinaryArchive& archive, ON_ClassArray<ON_WorksessionModel>& models)
{
  int major_version = 0;
  int minor_version = 0;
  if (!archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version))
    return false;

  bool rc = false;
  for (;;)
  {
    if (1 != major_version)
      break;

    ON__UINT32 typecode = 0;
    ON__INT64 value = 0;
    if (!archive.BeginRead3dmBigChunk(&typecode, &value))
      break;

    while (TCODE_ANONYMOUS_CHUNK == typecode)
    {
      ON__UINT32 next_typecode = 0;
      ON__INT64 next_value = 0;
      if (!archive.PeekAt3dmBigChunkType(&next_typecode, &next_value))
        break;
      if (TCODE_ENDOFTABLE == next_typecode)
      {
        rc = archive.BeginRead3dmBigChunk(&next_typecode, &next_value)
          && archive.EndRead3dmChunk()
          && TCODE_ENDOFTABLE == next_typecode;
        break;
      }
      if (!ReadV5Model(archive, models.AppendNew()))
        break;
    }

    if (!archive.EndRead3dmChunk())
      rc = false;
    break;
  }

  if (!archive.EndRead3dmChunk())
    rc = false;

  return rc;
}

bool ON_WorksessionFile_Private::WriteV4Model(ON_BinaryArchive& archive, const ON_WorksessionModel& model)
{
  // Rhino 4 wrote layer settings here and later versions skip them, so none are written.
  return archive.Write3dmChunkVersion(1, 1)
    && archive.WriteString(model.FullPath())
    && archive.WriteInt(2) // obsolete scale mode
    && archive.BeginWrite3dmChunk(TCODE_ENDOFTABLE, 0)
    && archive.EndWrite3dmChunk();
}

bool ON_WorksessionFile_Private::ReadV4Model(ON_BinaryArchive& archive, ON_WorksessionModel& model)
{
  int major_version = 0;
  int minor_version = 0;
  if (!archive.Read3dmChunkVersion(&major_version, &minor_version))
    return false;
  if (1 != major_version || minor_version < 0 || minor_version > 1)
    return false;
  if (!archive.ReadString(model.m_private->m_full_path))
    return false;
  if (minor_version > 0)
  {
    int obsolete_scale_mode = 0;
    if (!archive.ReadInt(&obsolete_scale_mode))
      return false;
  }

  // Rhino reads the Rhino 4 layer settings and does not use them, so they are read and dropped.
  for (;;)
  {
    ON__UINT32 typecode = 0;
    ON__INT64 value = 0;
    if (!archive.BeginRead3dmBigChunk(&typecode, &value))
      return false;

    bool rc = true;
    if (ON_WS_V4LayerTypecode == typecode)
    {
      ON_wString name;
      int mode = 0;
      int iges_level = 0;
      int material_index = 0;
      ON_Color color;
      unsigned char obsolete_line_style[20];
      rc = archive.ReadString(name)
        && archive.ReadInt(&mode)
        && archive.ReadInt(&iges_level)
        && archive.ReadInt(&material_index)
        && archive.ReadColor(color)
        && archive.ReadByte(20, obsolete_line_style);
    }

    if (!archive.EndRead3dmChunk())
      rc = false;

    if (TCODE_ENDOFFILE == typecode || TCODE_ENDOFTABLE == typecode)
      return rc;
    if (!rc || ON_WS_V4LayerTypecode != typecode)
      return false;
  }
}

// ON_BinaryArchive::Write3dmEndMark() needs a .3dm archive, so this writes the end mark Rhino 4 wrote.
static bool ON_WS_WriteV4EndMark(ON_BinaryArchive& archive)
{
  // Typecode, chunk length and this value are 4 bytes each.
  const int file_size = (int)(archive.CurrentPosition() + 12);
  if (!archive.BeginWrite3dmChunk(TCODE_ENDOFFILE, 0))
    return false;
  const bool rc = archive.WriteInt(file_size);
  if (!archive.EndWrite3dmChunk())
    return false;
  return rc;
}

bool ON_WorksessionFile_Private::CanWrite(const ON_ClassArray<ON_WorksessionModel>& models, unsigned int version)
{
  if (4 != version && 5 != version)
    return false;

  int named_active_count = 0;
  for (int i = 0; i < models.Count(); i++)
  {
    const ON_WorksessionModel& model = models[i];
    if (!model.IsActive() && model.LayerOverrideCount() > ON_WS_MaxLayerOverrideCount)
      return false;
    if (model.IsActive() && model.FullPath().IsNotEmpty())
      named_active_count++;
  }

  // Version 4 marks the active model only by putting it first, so it needs exactly one.
  return 5 == version || 1 == named_active_count;
}

bool ON_WorksessionFile_Private::WriteV4(ON_BinaryArchive& archive, const ON_ClassArray<ON_WorksessionModel>& models)
{
  // Version 4 marks the active model by putting it first, outside any chunk.
  int active_index = -1;
  for (int i = 0; i < models.Count(); i++)
  {
    if (models[i].IsActive() && models[i].FullPath().IsNotEmpty())
    {
      if (active_index >= 0)
        return false;
      active_index = i;
    }
  }
  if (active_index < 0)
    return false;

  // Rhino writes this version outside a chunk, so it can never change.
  if (!archive.Write3dmChunkVersion(1, 0))
    return false;
  if (!WriteV4Model(archive, models[active_index]))
    return false;

  for (int i = 0; i < models.Count(); i++)
  {
    const ON_WorksessionModel& model = models[i];
    if (i == active_index || model.FullPath().IsEmpty())
      continue;
    if (!archive.BeginWrite3dmChunk(ON_WS_V4ReferenceModelTypecode, 0))
      return false;
    const bool rc = WriteV4Model(archive, model);
    if (!archive.EndWrite3dmChunk() || !rc)
      return false;
  }

  return ON_WS_WriteV4EndMark(archive);
}

bool ON_WorksessionFile_Private::ReadV4(ON_BinaryArchive& archive, ON_ClassArray<ON_WorksessionModel>& models)
{
  int major_version = 0;
  int minor_version = 0;
  if (!archive.Read3dmChunkVersion(&major_version, &minor_version))
    return false;
  if (1 != major_version || 0 != minor_version)
    return false;

  ON_WorksessionModel& active_model = models.AppendNew();
  active_model.SetActive(true);
  if (!ReadV4Model(archive, active_model))
    return false;

  for (;;)
  {
    ON__UINT32 typecode = 0;
    ON__INT64 value = 0;
    if (!archive.BeginRead3dmBigChunk(&typecode, &value))
      return false;

    bool rc = true;
    if (ON_WS_V4ReferenceModelTypecode == typecode)
      rc = ReadV4Model(archive, models.AppendNew());
    else if (TCODE_ENDOFFILE != typecode)
      rc = false;

    if (!archive.EndRead3dmChunk())
      rc = false;

    if (!rc)
      return false;
    if (ON_WS_V4ReferenceModelTypecode != typecode)
      return true;
  }
}

ON_WorksessionFile::ON_WorksessionFile() : m_private(new ON_WorksessionFile_Private) {}
ON_WorksessionFile::~ON_WorksessionFile() = default;
ON_WorksessionFile::ON_WorksessionFile(const ON_WorksessionFile& src)
  : m_private(new ON_WorksessionFile_Private(*src.m_private)) {}

ON_WorksessionFile& ON_WorksessionFile::operator=(const ON_WorksessionFile& src)
{
  if (this != &src)
    *m_private = *src.m_private;
  return *this;
}

unsigned int ON_WorksessionFile::CurrentVersion()
{
  return 5;
}

unsigned int ON_WorksessionFile::FileVersion(const wchar_t* file_path)
{
  if (nullptr == file_path || 0 == file_path[0])
    return 0;

  FILE* fp = ON::OpenFile(file_path, L"rb");
  if (nullptr == fp)
    return 0;

  unsigned int version = 0;
  {
    ON_BinaryFile archive(ON::archive_mode::read, fp);
    version = ON_WS_ReadHeader(archive);
  }
  ON::CloseFile(fp);

  return version;
}

bool ON_WorksessionFile::Read(const wchar_t* file_path)
{
  m_private->m_version = 0;
  m_private->m_models.Destroy();

  if (nullptr == file_path || 0 == file_path[0])
    return false;

  FILE* fp = ON::OpenFile(file_path, L"rb");
  if (nullptr == fp)
    return false;

  bool rc = false;
  {
    ON_BinaryFile archive(ON::archive_mode::read, fp);
    rc = Read(archive);
  }
  ON::CloseFile(fp);

  return rc;
}

bool ON_WorksessionFile::Read(ON_BinaryArchive& archive)
{
  m_private->m_version = 0;
  m_private->m_models.Destroy();

  if (!archive.ReadMode())
    return false;

  const int saved_3dm_version = archive.Archive3dmVersion();
  archive.SetArchive3dmVersion(0);

  bool rc = false;
  m_private->m_version = ON_WS_ReadHeader(archive);
  if (4 == m_private->m_version)
    rc = ON_WorksessionFile_Private::ReadV4(archive, m_private->m_models);
  else if (5 == m_private->m_version)
    rc = ON_WorksessionFile_Private::ReadV5(archive, m_private->m_models);

  archive.SetArchive3dmVersion(saved_3dm_version);

  if (!rc)
    m_private->m_models.Destroy();

  return rc;
}

bool ON_WorksessionFile::Write(const wchar_t* file_path, unsigned int version) const
{
  if (nullptr == file_path || 0 == file_path[0])
    return false;

  // Written to memory first, so a worksession that cannot be written leaves an existing file alone.
  ON_Buffer buffer;
  {
    ON_BinaryArchiveBuffer archive(ON::archive_mode::write, &buffer);
    if (!Write(archive, version))
      return false;
  }

  FILE* fp = ON::OpenFile(file_path, L"wb");
  if (nullptr == fp)
    return false;

  bool rc = buffer.SeekFromStart(0);
  unsigned char bytes[65536];
  for (ON__UINT64 remaining = buffer.Size(); rc && remaining > 0;)
  {
    const size_t count = (size_t)((remaining < sizeof(bytes)) ? remaining : sizeof(bytes));
    rc = (count == buffer.Read(count, bytes)) && (count == fwrite(bytes, 1, count, fp));
    remaining -= count;
  }
  if (0 != ON::CloseFile(fp))
    rc = false;

  return rc;
}

bool ON_WorksessionFile::Write(ON_BinaryArchive& archive, unsigned int version) const
{
  if (!archive.WriteMode())
    return false;
  const ON_ClassArray<ON_WorksessionModel>& models = m_private->m_models;
  if (!ON_WorksessionFile_Private::CanWrite(models, version))
    return false;

  const int saved_3dm_version = archive.Archive3dmVersion();
  archive.SetArchive3dmVersion(0);

  bool rc = ON_WS_WriteHeader(archive, version);
  if (rc)
    rc = (4 == version) ? ON_WorksessionFile_Private::WriteV4(archive, models) : ON_WorksessionFile_Private::WriteV5(archive, models);

  archive.SetArchive3dmVersion(saved_3dm_version);

  return rc;
}

unsigned int ON_WorksessionFile::Version() const { return m_private->m_version; }
void ON_WorksessionFile::SetVersion(unsigned int version) { m_private->m_version = version; }
int ON_WorksessionFile::ModelCount() const { return m_private->m_models.Count(); }

const ON_WorksessionModel* ON_WorksessionFile::Model(int index) const
{
  return (index >= 0 && index < m_private->m_models.Count()) ? &m_private->m_models[index] : nullptr;
}

bool ON_WorksessionFile::SetModel(int index, const ON_WorksessionModel& model)
{
  if (index < 0 || index >= m_private->m_models.Count())
    return false;
  m_private->m_models[index] = model;
  return true;
}

void ON_WorksessionFile::AppendModel(const ON_WorksessionModel& model)
{
  m_private->m_models.Append(model);
}

bool ON_WorksessionFile::RemoveModel(int index)
{
  if (index < 0 || index >= m_private->m_models.Count())
    return false;
  m_private->m_models.Remove(index);
  return true;
}

void ON_WorksessionFile::RemoveAllModels() { m_private->m_models.Destroy(); }

int ON_WorksessionFile::ActiveModelIndex() const
{
  for (int i = 0; i < m_private->m_models.Count(); i++)
  {
    if (m_private->m_models[i].IsActive())
      return i;
  }
  return -1;
}

// Paths and names are printed without formatting, which drops text a process's locale cannot convert.
static void ON_WS_DumpQuoted(ON_TextLog& text_log, const wchar_t* label, const ON_wString& text)
{
  text_log.Print(L"%ls: \"", label);
  text_log.PrintString(text);
  text_log.Print(L"\"\n");
}

static void ON_WS_DumpColor(ON_TextLog& text_log, const wchar_t* label, ON_Color color)
{
  text_log.Print(L"%ls: %d,%d,%d,%d\n", label, color.Red(), color.Green(), color.Blue(), color.Alpha());
}

static void ON_WS_DumpLayerValues(ON_TextLog& text_log, const wchar_t* label, const ON_WorksessionLayerValues& values)
{
  text_log.Print(L"%ls:\n", label);
  const ON_TextLogIndent indent1(text_log);
  text_log.Print(L"Visible: %ls\n", values.Visible() ? L"yes" : L"no");
  text_log.Print(L"Locked: %ls\n", values.Locked() ? L"yes" : L"no");
  ON_WS_DumpColor(text_log, L"Color", values.Color());

  const ON_Layer& layer = values.Layer();
  text_log.Print(L"Layer:\n");
  const ON_TextLogIndent indent2(text_log);
  ON_WS_DumpQuoted(text_log, L"Name", layer.Name());
  ON_WS_DumpColor(text_log, L"Print color", layer.PlotColor());
  text_log.Print(L"Print width: %g\n", layer.PlotWeight());
  text_log.Print(L"Model visible: %ls\n", layer.ModelIsVisible() ? L"yes" : L"no");
  text_log.Print(L"Model persistent visibility: %ls\n", layer.ModelPersistentVisibility() ? L"yes" : L"no");
  text_log.Print(L"Visible in new details: %ls\n", layer.PerViewportIsVisibleInNewDetails() ? L"yes" : L"no");
  text_log.Print(L"Expanded: %ls\n", layer.m_bExpanded ? L"yes" : L"no");
  text_log.Print(L"Per-viewport settings CRC: %08x\n", layer.PerViewportSettingsCRC());
}

void ON_WorksessionFile::Dump(ON_TextLog& text_log) const
{
  const ON_ClassArray<ON_WorksessionModel>& models = m_private->m_models;
  text_log.Print(L"Worksession file version %u\n", m_private->m_version);
  text_log.Print(L"%d models\n", models.Count());
  for (int i = 0; i < models.Count(); i++)
  {
    const ON_WorksessionModel& model = models[i];
    text_log.Print(L"Model %d (%ls):\n", i, model.IsActive() ? L"active" : L"reference");
    const ON_TextLogIndent indent1(text_log);
    ON_WS_DumpQuoted(text_log, L"Full path", model.FullPath());
    ON_WS_DumpQuoted(text_log, L"Relative path", model.RelativePath());
    ON_WS_DumpLayerValues(text_log, L"Parent layer", model.ParentLayerValues());
    text_log.Print(L"%d layer overrides\n", model.LayerOverrideCount());
    for (int j = 0; j < model.LayerOverrideCount(); j++)
    {
      const ON_WorksessionLayerOverride& layer_override = *model.LayerOverride(j);
      text_log.Print(L"Layer override %d:\n", j);
      const ON_TextLogIndent indent2(text_log);
      text_log.Print(L"Layer id: ");
      text_log.Print(layer_override.LayerId());
      text_log.Print(L"\n");
      ON_WS_DumpLayerValues(text_log, L"File values", layer_override.FileValues());
      ON_WS_DumpLayerValues(text_log, L"Worksession values", layer_override.WorksessionValues());
    }
  }
}
