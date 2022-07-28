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

#if !defined(ON_MESH_MODIFIERS_INC_)
#define ON_MESH_MODIFIERS_INC_

class ON_CLASS ON_MeshModifier
{
public:
  ON_MeshModifier();
  ON_MeshModifier(ON_XMLNode& model_node);
  ON_MeshModifier(const ON_MeshModifier& mm) = delete;
  virtual ~ON_MeshModifier();

  const ON_MeshModifier& operator = (const ON_MeshModifier&) = delete;

  ON_wString XML(void) const;

protected:
  class CImpl;
  CImpl* m_impl;
};

class ON_CLASS ON_Displacement : public ON_MeshModifier
{
public:
  ON_Displacement();
  ON_Displacement(ON_XMLNode& model_node);
  ON_Displacement(const ON_Displacement& dsp);
  ~ON_Displacement();

  const ON_Displacement& operator = (const ON_Displacement& dsp);

  bool operator == (const ON_Displacement& dsp) const;
  bool operator != (const ON_Displacement& dsp) const;

  // Specifies whether the feature is enabled or not.
  bool On(void) const;
  void SetOn(bool b);

  ON_UUID Texture(void) const;
  void SetTexture(const ON_UUID& id);

  int MappingChannel(void) const;
  void SetMappingChannel(int c);

  double BlackPoint(void) const;
  void SetBlackPoint(double b);

  double WhitePoint(void) const;
  void SetWhitePoint(double w);

  int InitialQuality(void) const;
  void SetInitialQuality(int q);

  bool FinalMaxFacesOn(void) const;
  void SetFinalMaxFacesOn(bool b);

  int FinalMaxFaces(void) const;
  void SetFinalMaxFaces(int f);

  bool FairingOn(void) const;
  void SetFairingOn(bool b);

  int Fairing(void) const;
  void SetFairing(int f);

  double PostWeldAngle(void) const;
  void SetPostWeldAngle(double a);

  int MeshMemoryLimit(void) const;
  void SetMeshMemoryLimit(int l);

  int RefineSteps(void) const;
  void SetRefineSteps(int s);

  double RefineSensitivity(void) const;
  void SetRefineSensitivity(double s);

  enum class SweepResolutionFormulas : int { Default = 0, AbsoluteToleranceDependent = 1 };
  SweepResolutionFormulas SweepResolutionFormula(void) const;
  void SetSweepResolutionFormula(SweepResolutionFormulas f);

  // Sub-items can exist to override the top-level parameters for polysurface/SubD faces.
  class ON_CLASS SubItem final
  {
  public:
    SubItem(ON_XMLNode& sub_node);
    SubItem(const SubItem&) = delete;
    ~SubItem();

    const SubItem& operator = (const SubItem& sub);

    bool operator == (const SubItem& sub) const;
    bool operator != (const SubItem& sub) const;

    // Returns the index of this sub-item.
    // When this is >= 0 it is the component index of the face.
    int Index(void) const;
    void SetIndex(int i);

    // Overrides displacement 'on'.
    bool On(void) const;
    void SetOn(bool b);

    // Overrides displacement 'texture'.
    ON_UUID Texture(void) const;
    void SetTexture(const ON_UUID& id);

    // Overrides displacement 'mapping channel'.
    int MappingChannel(void) const;
    void SetMappingChannel(int c);

    // Overrides displacement 'black-point'.
    double BlackPoint(void) const;
    void SetBlackPoint(double b);

    // Overrides displacement 'white-point'.
    double WhitePoint(void) const;
    void SetWhitePoint(double w);

  private:
    class CImpl;
    CImpl* m_impl;
  };

  class ON_CLASS SubItemIterator final
  {
  public:
    SubItemIterator(const ON_Displacement& dsp);
    ~SubItemIterator();

    SubItem* Next(void);

  private:
    class CImpl;
    CImpl* m_impl;
  };

  SubItem& AddSubItem(void);
  void DeleteAllSubItems(void);
  SubItem* FindSubItem(const int index) const;
  SubItemIterator GetSubItemIterator(void) const;

private:
  class CImplDSP;
  CImplDSP* m_impl_dsp;
};

class ON_CLASS ON_EdgeSoftening : public ON_MeshModifier
{
public:
  ON_EdgeSoftening();
  ON_EdgeSoftening(ON_XMLNode& model_node);
  ON_EdgeSoftening(const ON_EdgeSoftening& es);

  const ON_EdgeSoftening& operator = (const ON_EdgeSoftening& es);

  bool operator == (const ON_EdgeSoftening& es) const;
  bool operator != (const ON_EdgeSoftening& es) const;

  // Specifies whether the feature is enabled or not.
  bool On(void) const;
  void SetOn(bool b);

  // The softening radius.
  double Softening(void) const;
  void SetSoftening(double d);

  // Specifies whether to chamfer the edges.
  bool Chamfer(void) const;
  void SetChamfer(bool b);

  // Specifies whether the edges are faceted.
  bool Faceted(void) const;
  void SetFaceted(bool b);

  // Threshold angle (in degrees) which controls whether an edge is softened or not.
  // The angle refers to the angles between the adjacent faces of an edge.
  double EdgeAngleThreshold(void) const;
  void SetEdgeAngleThreshold(double d);

  // Specifies whether to soften edges despite too large a radius.
  bool ForceSoftening(void) const;
  void SetForceSoftening(bool b);
};

class ON_CLASS ON_Thickening : public ON_MeshModifier
{
public:
  ON_Thickening();
  ON_Thickening(ON_XMLNode& model_node);
  ON_Thickening(const ON_Thickening& t);

  const ON_Thickening& operator = (const ON_Thickening& t);

  bool operator == (const ON_Thickening& t) const;
  bool operator != (const ON_Thickening& t) const;

  // Specifies whether the feature is enabled or not.
  bool On(void) const;
  void SetOn(bool b);

  double Distance(void) const;
  void SetDistance(double d);

  bool Solid(void) const;
  void SetSolid(bool b);

  bool OffsetOnly(void) const;
  void SetOffsetOnly(bool b);

  bool BothSides(void) const;
  void SetBothSides(bool b);
};

class ON_CLASS ON_CurvePiping : public ON_MeshModifier
{
public:
  ON_CurvePiping();
  ON_CurvePiping(ON_XMLNode& model_node);
  ON_CurvePiping(const ON_CurvePiping& cp);

  const ON_CurvePiping& operator = (const ON_CurvePiping& cp);

  bool operator == (const ON_CurvePiping& cp) const;
  bool operator != (const ON_CurvePiping& cp) const;

  // Specifies whether the feature is enabled or not.
  bool On(void) const;
  void SetOn(bool b);

  // Specifies the radius of the pipe (minimum value 0.0001).
  double Radius(void) const;
  void SetRadius(double d);

  // Specifies the number of segments in the pipe (minimum value 2).
  int Segments(void) const;
  void SetSegments(int s);

  // Specifies whether the pipe is faceted or not.
  bool Faceted(void) const;
  void SetFaceted(bool b);

  // Specifies the accuracy of the pipe in the range 0 to 100.
  int Accuracy(void) const;
  void SetAccuracy(int a);

  enum class CapTypes : int
  {
    None = 0, // No capping.
    Flat = 1, // A flat surface will cap the pipe.
    Box  = 2, // A simple construction will cap the pipe.
    Dome = 3, // A meridians-and-parallels hemisphere construction will cap the pipe.
  };

  // The type of cap to be created at the ends of the pipe.
  CapTypes CapType(void) const;
  void SetCapType(CapTypes ct);
};

class ON_CLASS ON_Shutlining : public ON_MeshModifier
{
public:
  ON_Shutlining();
  ON_Shutlining(ON_XMLNode& model_node);
  ON_Shutlining(const ON_Shutlining& sl);
  virtual ~ON_Shutlining();

  const ON_Shutlining& operator = (const ON_Shutlining& sl);

  bool operator == (const ON_Shutlining& sl) const;
  bool operator != (const ON_Shutlining& sl) const;

  // Specifies whether the feature is enabled or not.
  bool On(void) const;
  void SetOn(bool b);

  // Specifies whether the shutlining is faceted or not.
  bool Faceted(void) const;
  void SetFaceted(bool b);

  // Specifies whether the shutlining automatically updates or not.
  bool AutoUpdate(void) const;
  void SetAutoUpdate(bool b);

  // Specifies whether updating is forced or not.
  bool ForceUpdate(void) const;
  void SetForceUpdate(bool b);

  class ON_CLASS Curve final
  {
  public:
    Curve(ON_XMLNode& curve_node);
    Curve(const Curve&) = delete;
    ~Curve();

    const Curve& operator = (const Curve& c);

    bool operator == (const Curve& c) const;
    bool operator != (const Curve& c) const;

    ON_UUID Id(void) const;
    void SetId(const ON_UUID& id);

    double Radius(void) const;
    void SetRadius(double d);

    int Profile(void) const;
    void SetProfile(int p);

    bool Enabled(void) const;
    void SetEnabled(bool b);

    bool Pull(void) const;
    void SetPull(bool b);

    bool IsBump(void) const;
    void SetIsBump(bool b);

  private:
    class CImpl;
    CImpl* m_impl;
  };

  class ON_CLASS CurveIterator final
  {
  public:
    CurveIterator(const ON_Shutlining& sl);
    ~CurveIterator();

    Curve* Next(void);

  private:
    class CImpl;
    CImpl* m_impl;
  };

  // Adds a new curve to the shutlining. The curve will have an id of ON_nil_uuid.
  // After adding a curve, you should set the id to that of a curve in the model
  // that will be used to calculate the shutlining.
  Curve& AddCurve(void);

  // Deletes all the curves from the shutlining.
  void DeleteAllCurves(void);

  // Finds a curve on the shutlining by its id. Returns null if not found.
  Curve* FindCurve(const ON_UUID& id) const;

  // Gets an iterator for iterating over all the curves on the shutlining.
  CurveIterator GetCurveIterator(void) const;

private:
  class CImplSL;
  CImplSL* m_impl_sl;
};

class ON_CLASS ON_MeshModifiers final
{
public:
  ON_MeshModifiers(const ON_ModelComponent& component, const wchar_t* xml);
  ON_MeshModifiers(const ON_MeshModifiers&) = delete;
  ~ON_MeshModifiers();

  const ON_MeshModifiers& operator = (const ON_MeshModifiers&) = delete;

  bool operator == (const ON_MeshModifiers&) const = delete;
  bool operator != (const ON_MeshModifiers&) const = delete;

  // Get an object that provides access to displacement information.
  ON_Displacement& Displacement(void);

  // Get an object that provides access to edge softening information.
  ON_EdgeSoftening& EdgeSoftening(void);

  // Get an object that provides access to thickening information.
  ON_Thickening& Thickening(void);

  // Get an object that provides access to curve piping information.
  ON_CurvePiping& CurvePiping(void);

  // Get an object that provides access to shutlining information.
  ON_Shutlining& Shutlining(void);

private:
  class CImpl;
  CImpl* m_impl;
};

#endif
