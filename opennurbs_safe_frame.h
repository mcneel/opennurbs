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

#if !defined(ON_SAFE_FRAME_INC_)
#define ON_SAFE_FRAME_INC_

class ON_CLASS ON_SafeFrame final
{
public:
  ON_SafeFrame();
  ON_SafeFrame(ON_XMLNode& model_node);
  ON_SafeFrame(const ON_SafeFrame&);
  ~ON_SafeFrame();

  const ON_SafeFrame& operator = (const ON_SafeFrame& sf);

  bool operator == (const ON_SafeFrame& sf);
  bool operator != (const ON_SafeFrame& sf);

  // Returns true if the safe frame is on.
  bool On(void) const;

  // Sets the safe frame on or off.
  void SetOn(bool b);

  // Returns true if the safe frame is only displayed in perspective views.
  bool PerspectiveOnly(void) const;

  // Sets whether or not the safe frame is only displayed in perspective views.
  void SetPerspectiveOnly(bool b);

  // Returns true if the 4x3 field grid is on.
  bool FieldGridOn(void) const;

  // Sets whether or not the 4x3 field grid is on.
  void SetFieldGridOn(bool b);

  // Returns true if the live frame is on.
  bool LiveFrameOn(void) const;

  // Sets whether or not the live frame is on.
  void SetLiveFrameOn(bool b);

  // Returns true if the action frame is on.
  bool ActionFrameOn(void) const;

  // Sets whether or not the action frame is on.
  void SetActionFrameOn(bool b);

  // Returns true if the action frame X and Y scales are linked.
  bool ActionFrameLinked(void) const;

  // Sets whether or not the action frame X and Y scales are linked.
  void SetActionFrameLinked(bool b);

  // Returns the action frame X scale.
  double ActionFrameXScale(void) const;

  // Sets the action frame X scale.
  void SetActionFrameXScale(double d);

  // Returns the action frame Y scale.
  double ActionFrameYScale(void) const;

  // Sets the action frame Y scale.
  void SetActionFrameYScale(double d);

  // Returns true if the title frame is on.
  bool TitleFrameOn(void) const;

  // Sets whether or not the title frame is on.
  void SetTitleFrameOn(bool b);

  // Returns true if the title frame X and Y scales are linked.
  bool TitleFrameLinked(void) const;

  // Sets whether or not the title frame X and Y scales are linked.
  void SetTitleFrameLinked(bool b);

  // Returns the title frame X scale.
  double TitleFrameXScale(void) const;

  // Sets the title frame X scale.
  void SetTitleFrameXScale(double d);

  // Returns the title frame Y scale.
  double TitleFrameYScale(void) const;

  // Sets the title frame Y scale.
  void SetTitleFrameYScale(double d);

private:
  class CImpl;
  CImpl* m_impl;
};

#endif
