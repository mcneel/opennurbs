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

#if !defined(ON_RENDER_CHANNELS_INC_)
#define ON_RENDER_CHANNELS_INC_

class ON_CLASS ON_RenderChannels final
{
public:
  ON_RenderChannels();
  ON_RenderChannels(ON_XMLNode& model_node);
  ON_RenderChannels(const ON_RenderChannels& rch);
  ~ON_RenderChannels();

  const ON_RenderChannels& operator = (const ON_RenderChannels& rch);

  bool operator == (const ON_RenderChannels& rch);
  bool operator != (const ON_RenderChannels& rch);

  enum class Modes : unsigned int
  {
    Automatic,
    Custom,
  };

  // Get the mode.
  Modes Mode(void) const;

  // Set the mode.
  void SetMode(Modes m);

  // Get the list of channels to render when in 'custom' mode.
  // - 'chan' accepts the channel ids. */
  void GetCustomList(ON_SimpleArray<ON_UUID>& chan) const;

  // Set the list of channels to render when in 'custom' mode.
  // - 'chan' contains the channel ids.
  void SetCustomList(const ON_SimpleArray<ON_UUID>& chan);

private:
  class CImpl;
  CImpl* m_impl;
};

#endif
