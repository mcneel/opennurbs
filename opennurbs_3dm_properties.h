//
// Copyright (c) 1993-2022 Robert McNeel & Associates. All rights reserved.
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

#if !defined(OPENNURBS_3DM_PROPERTIES_INC_)
#define OPENNURBS_3DM_PROPERTIES_INC_

//////////////////////////////////////////////////////////////////////////////////////////

class ON_CLASS ON_3dmRevisionHistory
{
public:
  /*
    Default construction sets this = ON_3dmRevisionHistory::Empty
  */
  ON_3dmRevisionHistory();

  ~ON_3dmRevisionHistory() = default;

  ON_3dmRevisionHistory(const ON_3dmRevisionHistory&) = default;
  ON_3dmRevisionHistory& operator=(const ON_3dmRevisionHistory&) = default;

  /*
  Description:
    The Empty revision has a revision number zero,
    all time values set to zero and all string
    values empty.
  */
  static const ON_3dmRevisionHistory Empty;

  /*
  Returns:
    A revision history with
      m_revision_count = 1
      m_create_time = now
      m_last_edit_time = now
      m_sCreatedBy = current user
      m_sLastEditedBy = current user
  */
  static ON_3dmRevisionHistory FirstRevision();

  int NewRevision(); // returns updated revision count

  bool IsValid() const;
  bool IsEmpty() const;
  bool Read( ON_BinaryArchive& );
  bool Write( ON_BinaryArchive& ) const;
  void Dump( ON_TextLog& ) const;

  /*
  Returns:
    true 
      if m_create_time is >= January 1, 1970
  */
  bool CreateTimeIsSet() const;

  /*
  Returns:
    true 
      if m_last_edit_time is >= January 1, 1970
  */
  bool LastEditedTimeIsSet() const;

  ON_wString m_sCreatedBy;
  ON_wString m_sLastEditedBy;
  struct tm  m_create_time;        // UCT create time
  struct tm  m_last_edit_time;     // UCT las edited time
  int        m_revision_count = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////

class ON_CLASS ON_3dmNotes
{
public:
  ON_3dmNotes();
  ~ON_3dmNotes();

  static const ON_3dmNotes Empty;

  bool IsValid() const;
  bool IsEmpty() const;

  bool Read( ON_BinaryArchive& );
  bool Write( ON_BinaryArchive& ) const;

  void Dump(ON_TextLog&) const;

  ////////////////////////////////////////////////////////////////
  //
  // Interface - this information is serialized.  Applications
  // may want to derive a runtime class that has additional
  // window and font information.
  ON_wString m_notes;

  bool m_bVisible;    // true if notes window is showing
  bool m_bHTML;       // true if notes are in HTML

  // last window position
  int m_window_left;
  int m_window_top;
  int m_window_right;
  int m_window_bottom;
};

//////////////////////////////////////////////////////////////////////////////////////////

class ON_CLASS ON_3dmApplication
{
  // application that created the 3dm file
public:
  ON_3dmApplication();
  ~ON_3dmApplication();

  static const ON_3dmApplication Empty;

  bool IsValid() const;

  bool IsEmpty() const;

  bool Read( ON_BinaryArchive& );
  bool Write( ON_BinaryArchive& ) const;

  void Dump( ON_TextLog& ) const;

  ON_wString m_application_name;    // short name like "Rhino 2.0"
  ON_wString m_application_URL;     // URL
  ON_wString m_application_details; // whatever you want
};

//////////////////////////////////////////////////////////////////////////////////////////

class ON_CLASS ON_3dmProperties
{
public:
  ON_3dmProperties() = default;
  ~ON_3dmProperties() = default;
  ON_3dmProperties(const ON_3dmProperties&) = default;
  ON_3dmProperties& operator=(const ON_3dmProperties&) = default;

  static const ON_3dmProperties Empty;

  bool IsEmpty() const;

  bool Read(
    ON_BinaryArchive& archive
    );

  /*
  Remarks:
    If archive.ArchiveFileName() is not empty, that value is 
    written in place of m_3dmArchiveFullPathName in the 3dm archive.
    If archive.ArchiveFileName() is empty, then m_3dmArchiveFullPathName
    is written in the 3dm archive.
  */
  bool Write(
    ON_BinaryArchive& archive
    ) const;

  void Dump( ON_TextLog& ) const;

  ON_3dmRevisionHistory  m_RevisionHistory;
  ON_3dmNotes            m_Notes;
  ON_WindowsBitmap       m_PreviewImage;     // preview image of model
  ON_3dmApplication      m_Application;      // application that created 3DM file
  
  // name of .3dm archive when it was written.  Used to find referenced files
  // when the archive is moved or copied and then read.
  ON_wString             m_3dmArchiveFullPathName; 
};

//////////////////////////////////////////////////////////////////////////////////////////

#endif
