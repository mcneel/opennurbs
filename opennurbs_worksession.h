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

#if !defined(OPENNURBS_WORKSESSION_INC_)
#define OPENNURBS_WORKSESSION_INC_

/*
Description:
  Layer settings saved in a Rhino worksession (.rws) file.
*/
class ON_CLASS ON_WorksessionLayerValues
{
public:
  ON_WorksessionLayerValues();
  ~ON_WorksessionLayerValues();
  ON_WorksessionLayerValues(const ON_WorksessionLayerValues& src);
  ON_WorksessionLayerValues& operator=(const ON_WorksessionLayerValues& src);

  // Defaults: visible, unlocked, black.
  bool Visible() const;
  void SetVisible(bool bVisible);

  bool Locked() const;
  void SetLocked(bool bLocked);

  ON_Color Color() const;
  void SetColor(ON_Color color);

  /*
  Returns:
    The rest of the layer's settings, saved by Rhino 9 and later. Reading an
    older file leaves this set to the default layer.
  Remarks:
    Rhino does not use this layer's id, and a layer saved without one gets a
    new id when it is read, as it does in a .3dm file.
  */
  const ON_Layer& Layer() const;
  void SetLayer(const ON_Layer& layer);

private:
#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4251 )
  std::unique_ptr<class ON_WorksessionLayerValues_Private> m_private;
#pragma ON_PRAGMA_WARNING_POP
  friend class ON_WorksessionFile_Private;
};

/*
Description:
  A layer setting in a reference model that the worksession changes.
  Any value that is different in WorksessionValues() and FileValues()
  is an override. Values that are the same are not.
*/
class ON_CLASS ON_WorksessionLayerOverride
{
public:
  ON_WorksessionLayerOverride();
  ~ON_WorksessionLayerOverride();
  ON_WorksessionLayerOverride(const ON_WorksessionLayerOverride& src);
  ON_WorksessionLayerOverride& operator=(const ON_WorksessionLayerOverride& src);

  // Id of the layer in the reference model file.
  ON_UUID LayerId() const;
  void SetLayerId(const ON_UUID& layer_id);

  // The layer's settings in the reference model when the worksession was saved.
  const ON_WorksessionLayerValues& FileValues() const;
  void SetFileValues(const ON_WorksessionLayerValues& values);

  // The layer's settings in the worksession.
  const ON_WorksessionLayerValues& WorksessionValues() const;
  void SetWorksessionValues(const ON_WorksessionLayerValues& values);

private:
#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4251 )
  std::unique_ptr<class ON_WorksessionLayerOverride_Private> m_private;
#pragma ON_PRAGMA_WARNING_POP
  friend class ON_WorksessionFile_Private;
};

/*
Description:
  A model in a Rhino worksession (.rws) file.
*/
class ON_CLASS ON_WorksessionModel
{
public:
  ON_WorksessionModel();
  ~ON_WorksessionModel();
  ON_WorksessionModel(const ON_WorksessionModel& src);
  ON_WorksessionModel& operator=(const ON_WorksessionModel& src);

  // True for the model that was active when the worksession was saved.
  // Every other model is a reference model.
  bool IsActive() const;
  void SetActive(bool bActive);

  // Full path of the model file, as saved. Nothing checks that it exists.
  const ON_wString FullPath() const;
  void SetFullPath(const wchar_t* full_path);

  // Path of the model file relative to the .rws file, as saved. Rhino uses
  // it when the file is no longer at FullPath(). It can be empty.
  // Version 4 files do not have it.
  const ON_wString RelativePath() const;
  void SetRelativePath(const wchar_t* relative_path);

  // Layer overrides for a reference model. Rhino does not save or use
  // them for the active model. Version 4 files do not have them.
  int LayerOverrideCount() const;

  // Returns nullptr if index is out of range. The pointer is valid only until the
  // list of layer overrides next changes.
  const ON_WorksessionLayerOverride* LayerOverride(int index) const;

  // Returns false if index is out of range.
  bool SetLayerOverride(int index, const ON_WorksessionLayerOverride& layer_override);

  void AppendLayerOverride(const ON_WorksessionLayerOverride& layer_override);

  // Returns false if index is out of range.
  bool RemoveLayerOverride(int index);

  void RemoveAllLayerOverrides();

  // Settings of the layer Rhino adds as the parent of all of a reference
  // model's layers. Version 4 files do not have them.
  const ON_WorksessionLayerValues& ParentLayerValues() const;
  void SetParentLayerValues(const ON_WorksessionLayerValues& values);

private:
#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4251 )
  std::unique_ptr<class ON_WorksessionModel_Private> m_private;
#pragma ON_PRAGMA_WARNING_POP
  friend class ON_WorksessionFile_Private;
};

/*
Description:
  Reads and writes Rhino worksession (.rws) files.
  A worksession file lists models and a worksession's layer overrides.
  It does not contain any model content.
Remarks:
  Rhino 4 wrote version 4 files. Rhino 5 and later write version 5.
  Both versions can be read and written.
*/
class ON_CLASS ON_WorksessionFile
{
public:
  ON_WorksessionFile();
  ~ON_WorksessionFile();
  ON_WorksessionFile(const ON_WorksessionFile& src);
  ON_WorksessionFile& operator=(const ON_WorksessionFile& src);

  // The version Rhino 5 and later write.
  static unsigned int CurrentVersion();

  /*
  Returns:
    The version of the worksession file, or 0 if the file cannot be read
    or is not a worksession file.
  */
  static unsigned int FileVersion(
    const wchar_t* file_path
    );

  /*
  Description:
    Reads a worksession file.
  Returns:
    True if the file was read. If false is returned, there are no models.
    Version() is set to the file's version whenever the file is a
    worksession file, including one from a newer version of Rhino that
    this code cannot read.
  */
  bool Read(
    const wchar_t* file_path
    );

  /*
  Description:
    Reads a worksession from the current position in archive.
  Parameters:
    archive - [in]
      An archive in read mode, such as an ON_BinaryFile.
      A worksession file is not a .3dm archive and the chunks in it have
      4 byte lengths. The archive's 3dm version is set to 0 while reading
      and restored afterwards.
  */
  bool Read(
    ON_BinaryArchive& archive
    );

  /*
  Description:
    Writes a worksession file.
  Parameters:
    file_path - [in]
    version - [in]
      4 or 5. Version 4 needs exactly one active model and saves only
      the model file paths.
  Returns:
    True if the file was written. Nothing is written, and an existing file is
    left as it was, if the worksession cannot be written in this version.
  Remarks:
    The first active model is written first and the other models follow in
    order. Models with an empty full path are not written. Paths are written
    as they are, and are not checked or changed.
  */
  bool Write(
    const wchar_t* file_path,
    unsigned int version = ON_WorksessionFile::CurrentVersion()
    ) const;

  /*
  Description:
    Writes a worksession at the current position in archive.
  Parameters:
    archive - [in]
      An archive in write mode, such as an ON_BinaryFile. The archive's 3dm
      version is set to 0 while writing and restored afterwards.
    version - [in]
      4 or 5.
  */
  bool Write(
    ON_BinaryArchive& archive,
    unsigned int version = ON_WorksessionFile::CurrentVersion()
    ) const;

  // Version of the file set by Read(), or 0. Write() does not use it.
  unsigned int Version() const;
  void SetVersion(unsigned int version);

  int ModelCount() const;

  // Returns nullptr if index is out of range. The pointer is valid only until the
  // list of models next changes.
  const ON_WorksessionModel* Model(int index) const;

  // Returns false if index is out of range.
  bool SetModel(int index, const ON_WorksessionModel& model);

  void AppendModel(const ON_WorksessionModel& model);

  // Returns false if index is out of range.
  bool RemoveModel(int index);

  void RemoveAllModels();

  // Index of the first active model, or -1 if there is none.
  int ActiveModelIndex() const;

  /*
  Description:
    Prints the file's contents. For each saved layer it prints its name and
    the settings Rhino uses: visibility, locking, color, print color and
    width, model visibility, visibility in new details, whether it is
    expanded, and a CRC of the per-viewport settings.
  */
  void Dump(
    class ON_TextLog& text_log
    ) const;

private:
#pragma ON_PRAGMA_WARNING_PUSH
#pragma ON_PRAGMA_WARNING_DISABLE_MSC( 4251 )
  std::unique_ptr<class ON_WorksessionFile_Private> m_private;
#pragma ON_PRAGMA_WARNING_POP
};

#endif
