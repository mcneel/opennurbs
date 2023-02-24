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

#if !defined(ON_LINEAR_WORKFLOW_INC_)
#define ON_LINEAR_WORKFLOW_INC_

class ON_CLASS ON_LinearWorkflow final
{
public:
  ON_LinearWorkflow();
  ON_LinearWorkflow(ON_XMLNode& model_node);
  ON_LinearWorkflow(const ON_LinearWorkflow& lw);
  ~ON_LinearWorkflow();

  const ON_LinearWorkflow& operator = (const ON_LinearWorkflow& lw);

  bool operator == (const ON_LinearWorkflow& lw);
  bool operator != (const ON_LinearWorkflow& lw);

  // Returns the linear workflow active state for textures.
  bool PreProcessTextures(void) const;

  // Sets the linear workflow (pre-process) active state for textures.
  void SetPreProcessTextures(bool b);

  // Returns the linear workflow active state for individual colors.
  bool PreProcessColors(void) const;

  // Sets the linear workflow (pre-process) active state for individual colors.
  void SetPreProcessColors(bool b);

  // Returns the pre-process gamma for input textures and colors.
  float PreProcessGamma(void) const;

  // Set pre-process gamma. This will generally be >= 1.0 (usually 2.2).
  // This is the actual value applied in pre-process.
  void SetPreProcessGamma(float gamma);

  // Get post-process gamma enabled state.
  bool PostProcessGammaOn(void) const;

  // Set post-process gamma enabled state.
  void SetPostProcessGammaOn(bool on);

  // Get post-process frame buffer enabled state.
  bool PostProcessFrameBuffer(void) const;

  // Set post-process frame buffer enabled state.
  void SetPostProcessFrameBuffer(bool on);

  // Returns the post-process gamma for frame buffer. This is not the value applied; it's the value that
  // appears in the UI. See PostProcessGammaReciprocal().
  float PostProcessGamma(void) const;

  // Set post-process gamma. This will generally be >= 1.0 (usually 2.2).
  // The actual value to be applied in post-process is the reciprocal.
  void SetPostProcessGamma(float gamma);

  // Returns: Gamma reciprocal for frame buffer. This is the value that is applied to each color channel
  // for post-process and is supplied here as an optimization.
  float PostProcessGammaReciprocal(void) const;

  // Applies pre-process gamma correction to a color if linear workflow is active.
  // for_texture is true if the color is part of a texture. */
  void ApplyPreProcessGamma(ON_4fColor& col, bool for_texture) const;

  // Returns the CRC of gamma and linear workflow active state.
  ON__UINT32 DataCRC(ON__UINT32 current_remainder) const;

private:
  class CImpl;
  CImpl* m_impl;
};

#endif
