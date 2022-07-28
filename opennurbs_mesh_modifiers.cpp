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

#include "opennurbs.h"
#include "opennurbs_internal_defines.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

class ON_MeshModifier::CImpl : public ON_InternalXMLImpl
{
public:
  CImpl() { }
  CImpl(ON_XMLNode& n) : ON_InternalXMLImpl(&n) { }
};

ON_MeshModifier::ON_MeshModifier()
{
  m_impl = new CImpl;
}

ON_MeshModifier::ON_MeshModifier(ON_XMLNode& model_node)
{
  m_impl = new CImpl(model_node);
}

ON_MeshModifier::~ON_MeshModifier()
{
  delete m_impl;
  m_impl = nullptr;
}

ON_wString ON_MeshModifier::XML(void) const
{
  // The XML does not include the <xml> node. It's the actual mesh modifier node.
  return m_impl->Node().String();
}

////////////////////////////////////////////////////////////////
//
// Displacement
//
////////////////////////////////////////////////////////////////

#define ON_DISPLACEMENT_ROOT                       L"new-displacement-object-data"
  #define ON_DISPLACEMENT_ON                       L"on"
  #define ON_DISPLACEMENT_CHANNEL                  L"channel"
  #define ON_DISPLACEMENT_BLACK_POINT              L"black-point"
  #define ON_DISPLACEMENT_WHITE_POINT              L"white-point"
  #define ON_DISPLACEMENT_SWEEP_PITCH              L"sweep-pitch"
  #define ON_DISPLACEMENT_REFINE_STEPS             L"refine-steps"
  #define ON_DISPLACEMENT_REFINE_SENSITIVITY       L"refine-sensitivity"
  #define ON_DISPLACEMENT_TEXTURE                  L"texture"
  #define ON_DISPLACEMENT_FACE_COUNT_LIMIT_ENABLED L"face-count-limit-enabled"
  #define ON_DISPLACEMENT_FACE_COUNT_LIMIT         L"face-count-limit"
  #define ON_DISPLACEMENT_POST_WELD_ANGLE          L"post-weld-angle"
  #define ON_DISPLACEMENT_MESH_MEMORY_LIMIT        L"mesh-memory-limit"
  #define ON_DISPLACEMENT_FAIRING_ENABLED          L"fairing-enabled"
  #define ON_DISPLACEMENT_FAIRING_AMOUNT           L"fairing-amount"
  #define ON_DISPLACEMENT_SWEEP_RES_FORMULA        L"sweep-res-formula"
  #define ON_DISPLACEMENT_SUB_OBJECT_COUNT         L"sub-object-count"
  #define ON_DISPLACEMENT_SUB                      L"sub"
    #define ON_DISPLACEMENT_SUB_INDEX              L"sub-index"
    #define ON_DISPLACEMENT_SUB_ON                 L"sub-on"
    #define ON_DISPLACEMENT_SUB_TEXTURE            L"sub-texture"
    #define ON_DISPLACEMENT_SUB_CHANNEL            L"sub-channel"
    #define ON_DISPLACEMENT_SUB_BLACK_POINT        L"sub-black-point"
    #define ON_DISPLACEMENT_SUB_WHITE_POINT        L"sub-white-point"

class ON_Displacement::CImplDSP
{
public:
  ON_SimpleArray<SubItem*> m_subs;
};

ON_Displacement::ON_Displacement(ON_XMLNode& model_node)
  :
  ON_MeshModifier(model_node)
{
  m_impl_dsp = new CImplDSP;

  auto it = model_node.GetChildIterator();
  while (auto* child_node = it.GetNextChild())
  {
    if (ON_DISPLACEMENT_SUB == child_node->TagName())
    {
      m_impl_dsp->m_subs.Append(new SubItem(*child_node));
    }
  }
}

ON_Displacement::ON_Displacement(const ON_Displacement& dsp)
{
  m_impl_dsp = new CImplDSP;
  operator = (dsp);
}

ON_Displacement::~ON_Displacement()
{
  DeleteAllSubItems();

  delete m_impl_dsp;
  m_impl_dsp = nullptr;
}

const ON_Displacement& ON_Displacement::operator = (const ON_Displacement& dsp)
{
  if (this != &dsp)
  {
    SetOn                    (dsp.On());
    SetTexture               (dsp.Texture());
    SetMappingChannel        (dsp.MappingChannel());
    SetBlackPoint            (dsp.BlackPoint());
    SetWhitePoint            (dsp.WhitePoint());
    SetInitialQuality        (dsp.InitialQuality());
    SetFinalMaxFacesOn       (dsp.FinalMaxFacesOn());
    SetFinalMaxFaces         (dsp.FinalMaxFaces());
    SetFairingOn             (dsp.FairingOn());
    SetFairing               (dsp.Fairing());
    SetPostWeldAngle         (dsp.PostWeldAngle());
    SetMeshMemoryLimit       (dsp.MeshMemoryLimit());
    SetRefineSteps           (dsp.RefineSteps());
    SetRefineSensitivity     (dsp.RefineSensitivity());
    SetSweepResolutionFormula(dsp.SweepResolutionFormula());
  }

  return *this;
}

bool ON_Displacement::operator == (const ON_Displacement& dsp) const
{
  if (On()                     != dsp.On())                     return false;
  if (Texture()                != dsp.Texture())                return false;
  if (MappingChannel()         != dsp.MappingChannel())         return false;
  if (BlackPoint()             != dsp.BlackPoint())             return false;
  if (WhitePoint()             != dsp.WhitePoint())             return false;
  if (InitialQuality()         != dsp.InitialQuality())         return false;
  if (FinalMaxFacesOn()        != dsp.FinalMaxFacesOn())        return false;
  if (FinalMaxFaces()          != dsp.FinalMaxFaces())          return false;
  if (FairingOn()              != dsp.FairingOn())              return false;
  if (Fairing()                != dsp.Fairing())                return false;
  if (PostWeldAngle()          != dsp.PostWeldAngle())          return false;
  if (MeshMemoryLimit()        != dsp.MeshMemoryLimit())        return false;
  if (RefineSteps()            != dsp.RefineSteps())            return false;
  if (RefineSensitivity()      != dsp.RefineSensitivity())      return false;
  if (SweepResolutionFormula() != dsp.SweepResolutionFormula()) return false;

  return true;
}

bool ON_Displacement::operator != (const ON_Displacement& dsp) const
{
  return !(operator == (dsp));
}

bool ON_Displacement::On(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_ON, false).AsBool();
}

void ON_Displacement::SetOn(bool b)
{
  m_impl->SetParameter(ON_DISPLACEMENT_ON, b);
}

ON_UUID ON_Displacement::Texture(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_TEXTURE, ON_nil_uuid).AsUuid();
}

void ON_Displacement::SetTexture(const ON_UUID& id)
{
  m_impl->SetParameter(ON_DISPLACEMENT_TEXTURE, id);
}

int ON_Displacement::MappingChannel(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_CHANNEL, 0).AsInteger();
}

void ON_Displacement::SetMappingChannel(int c)
{
  m_impl->SetParameter(ON_DISPLACEMENT_CHANNEL, c);
}

double ON_Displacement::BlackPoint(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_BLACK_POINT, 0.0).AsDouble();
}

void ON_Displacement::SetBlackPoint(double b)
{
  m_impl->SetParameter(ON_DISPLACEMENT_BLACK_POINT, b);
}

double ON_Displacement::WhitePoint(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_WHITE_POINT, 1.0).AsDouble();
}

void ON_Displacement::SetWhitePoint(double w)
{
  m_impl->SetParameter(ON_DISPLACEMENT_WHITE_POINT, w);
}

int ON_Displacement::InitialQuality(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_SWEEP_PITCH, 1000).AsInteger();
}

void ON_Displacement::SetInitialQuality(int q)
{
  m_impl->SetParameter(ON_DISPLACEMENT_SWEEP_PITCH, q);
}

bool ON_Displacement::FinalMaxFacesOn(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_FACE_COUNT_LIMIT_ENABLED, false).AsBool();
}

void ON_Displacement::SetFinalMaxFacesOn(bool b)
{
  m_impl->SetParameter(ON_DISPLACEMENT_FACE_COUNT_LIMIT_ENABLED, b);
}

int ON_Displacement::FinalMaxFaces(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_FACE_COUNT_LIMIT, 10000).AsInteger();
}

void ON_Displacement::SetFinalMaxFaces(int f)
{
  m_impl->SetParameter(ON_DISPLACEMENT_FACE_COUNT_LIMIT, f);
}

bool ON_Displacement::FairingOn(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_FAIRING_ENABLED, false).AsBool();
}

void ON_Displacement::SetFairingOn(bool b)
{
  m_impl->SetParameter(ON_DISPLACEMENT_FAIRING_ENABLED, b);
}

int ON_Displacement::Fairing(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_FAIRING_AMOUNT, 4).AsInteger();
}

void ON_Displacement::SetFairing(int f)
{
  m_impl->SetParameter(ON_DISPLACEMENT_FAIRING_AMOUNT, f);
}

double ON_Displacement::PostWeldAngle(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_POST_WELD_ANGLE, 40.0).AsDouble();
}

void ON_Displacement::SetPostWeldAngle(double a)
{
  m_impl->SetParameter(ON_DISPLACEMENT_POST_WELD_ANGLE, a);
}

int ON_Displacement::MeshMemoryLimit(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_MESH_MEMORY_LIMIT, 512).AsInteger();
}

void ON_Displacement::SetMeshMemoryLimit(int l)
{
  m_impl->SetParameter(ON_DISPLACEMENT_MESH_MEMORY_LIMIT, l);
}

int ON_Displacement::RefineSteps(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_REFINE_STEPS, 1).AsInteger();
}

void ON_Displacement::SetRefineSteps(int s)
{
  m_impl->SetParameter(ON_DISPLACEMENT_REFINE_STEPS, s);
}

double ON_Displacement::RefineSensitivity(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_REFINE_SENSITIVITY, 0.5).AsDouble();
}

void ON_Displacement::SetRefineSensitivity(double s)
{
  m_impl->SetParameter(ON_DISPLACEMENT_REFINE_SENSITIVITY, s);
}

ON_Displacement::SweepResolutionFormulas ON_Displacement::SweepResolutionFormula(void) const
{
  const auto def = SweepResolutionFormulas::Default;
  const auto v = m_impl->GetParameter(ON_DISPLACEMENT_SWEEP_RES_FORMULA, int(def)).AsInteger();
  return SweepResolutionFormulas(v);
}

void ON_Displacement::SetSweepResolutionFormula(SweepResolutionFormulas f)
{
  m_impl->SetParameter(ON_DISPLACEMENT_SWEEP_RES_FORMULA, int(f));
}

ON_Displacement::SubItemIterator ON_Displacement::GetSubItemIterator(void) const
{
  return SubItemIterator(*this);
}

ON_Displacement::SubItem& ON_Displacement::AddSubItem(void)
{
  auto* sub_node = m_impl->Node().AttachChildNode(new ON_XMLNode(ON_DISPLACEMENT_SUB));
  auto* sub = new SubItem(*sub_node);
  m_impl_dsp->m_subs.Append(sub);

  return *sub;
}

void ON_Displacement::DeleteAllSubItems(void)
{
  for (int i = 0; i < m_impl_dsp->m_subs.Count(); i++)
  {
    delete m_impl_dsp->m_subs[i];
  }

  m_impl_dsp->m_subs.Destroy();
}

ON_Displacement::SubItem* ON_Displacement::FindSubItem(const int index) const
{
  auto it = GetSubItemIterator();
  while (auto* sub = it.Next())
  {
    if (sub->Index() == index)
      return sub;
  }

  return nullptr;
}

class ON_Displacement::SubItem::CImpl : public ON_InternalXMLImpl
{
public:
  CImpl(ON_XMLNode& sub_node) : ON_InternalXMLImpl(&sub_node) { }
};

ON_Displacement::SubItem::SubItem(ON_XMLNode& sub_node)
{
  m_impl = new CImpl(sub_node);
}

ON_Displacement::SubItem::~SubItem()
{
  delete m_impl;
  m_impl = nullptr;
}

const ON_Displacement::SubItem& ON_Displacement::SubItem::operator = (const SubItem& sub)
{
  if (this != &sub)
  {
    SetIndex          (sub.Index());
    SetOn             (sub.On());
    SetTexture        (sub.Texture());
    SetMappingChannel (sub.MappingChannel());
    SetBlackPoint     (sub.BlackPoint());
    SetWhitePoint     (sub.WhitePoint());
  }

  return *this;
}

bool ON_Displacement::SubItem::operator == (const SubItem& sub) const
{
  if (Index()          != sub.Index())          return false;
  if (On()             != sub.On())             return false;
  if (Texture()        != sub.Texture())        return false;
  if (MappingChannel() != sub.MappingChannel()) return false;
  if (BlackPoint()     != sub.BlackPoint())     return false;
  if (WhitePoint()     != sub.WhitePoint())     return false;

  return true;
}

bool ON_Displacement::SubItem::operator != (const SubItem& c) const
{
  return !(operator == (c));
}

int ON_Displacement::SubItem::Index(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_SUB_INDEX, -1).AsInteger();
}

void ON_Displacement::SubItem::SetIndex(int i)
{
  m_impl->SetParameter(ON_DISPLACEMENT_SUB_INDEX, i);
}

bool ON_Displacement::SubItem::On(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_SUB_ON, false).AsBool();
}

void ON_Displacement::SubItem::SetOn(bool b)
{
  m_impl->SetParameter(ON_DISPLACEMENT_SUB_ON, b);
}

ON_UUID ON_Displacement::SubItem::Texture(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_SUB_TEXTURE, false).AsUuid();
}

void ON_Displacement::SubItem::SetTexture(const ON_UUID& id)
{
  m_impl->SetParameter(ON_DISPLACEMENT_SUB_TEXTURE, id);
}

int ON_Displacement::SubItem::MappingChannel(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_SUB_CHANNEL, 0).AsInteger();
}

void ON_Displacement::SubItem::SetMappingChannel(int c)
{
  m_impl->SetParameter(ON_DISPLACEMENT_SUB_CHANNEL, c);
}

double ON_Displacement::SubItem::BlackPoint(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_SUB_BLACK_POINT, 0.0).AsDouble();
}

void ON_Displacement::SubItem::SetBlackPoint(double b)
{
  m_impl->SetParameter(ON_DISPLACEMENT_SUB_BLACK_POINT, b);
}

double ON_Displacement::SubItem::WhitePoint(void) const
{
  return m_impl->GetParameter(ON_DISPLACEMENT_SUB_WHITE_POINT, 1.0).AsDouble();
}

void ON_Displacement::SubItem::SetWhitePoint(double w)
{
  m_impl->SetParameter(ON_DISPLACEMENT_SUB_WHITE_POINT, w);
}

class ON_Displacement::SubItemIterator::CImpl
{
public:
  CImpl(const ON_Displacement& dsp) : m_dsp(dsp) { }

  SubItem* Next(void)
  {
    if (m_index >= m_dsp.m_impl_dsp->m_subs.Count())
      return nullptr;

    return m_dsp.m_impl_dsp->m_subs[m_index++];
  }

  const ON_Displacement& m_dsp;
  int m_index = 0;
};

ON_Displacement::SubItemIterator::SubItemIterator(const ON_Displacement& dsp)
{
  m_impl = new CImpl(dsp);
}

ON_Displacement::SubItemIterator::~SubItemIterator()
{
  delete m_impl;
  m_impl = nullptr;
}

ON_Displacement::SubItem* ON_Displacement::SubItemIterator::Next(void)
{
  return m_impl->Next();
}

////////////////////////////////////////////////////////////////
//
// Edge Softening
//
////////////////////////////////////////////////////////////////

#define ON_EDGE_SOFTENING_ROOT              L"edge-softening-object-data"
  #define ON_EDGE_SOFTENING_ON              L"on"
  #define ON_EDGE_SOFTENING_SOFTENING       L"softening"
  #define ON_EDGE_SOFTENING_CHAMFER         L"chamfer"
  #define ON_EDGE_SOFTENING_UNWELD          L"unweld"
  #define ON_EDGE_SOFTENING_FORCE_SOFTENING L"force-softening"
  #define ON_EDGE_SOFTENING_EDGE_THRESHOLD  L"edge-threshold"

ON_EdgeSoftening::ON_EdgeSoftening()
{
}

ON_EdgeSoftening::ON_EdgeSoftening(ON_XMLNode& model_node)
  :
  ON_MeshModifier(model_node)
{
}

ON_EdgeSoftening::ON_EdgeSoftening(const ON_EdgeSoftening& es)
{
  operator = (es);
}

const ON_EdgeSoftening& ON_EdgeSoftening::operator = (const ON_EdgeSoftening& es)
{
  if (this != &es)
  {
    SetOn                (es.On());
    SetSoftening         (es.Softening());
    SetChamfer           (es.Chamfer());
    SetFaceted           (es.Faceted());
    SetForceSoftening    (es.ForceSoftening());
    SetEdgeAngleThreshold(es.EdgeAngleThreshold());
  }

  return *this;
}

bool ON_EdgeSoftening::operator == (const ON_EdgeSoftening& es) const
{
  if (On()                 != es.On())                 return false;
  if (Softening()          != es.Softening())          return false;
  if (Chamfer()            != es.Chamfer())            return false;
  if (Faceted()            != es.Faceted())            return false;
  if (ForceSoftening()     != es.ForceSoftening())     return false;
  if (EdgeAngleThreshold() != es.EdgeAngleThreshold()) return false;

  return true;
}

bool ON_EdgeSoftening::operator != (const ON_EdgeSoftening& es) const
{
  return !(operator == (es));
}

bool ON_EdgeSoftening::On(void) const
{
  return m_impl->GetParameter(ON_EDGE_SOFTENING_ON, false).AsBool();
}

void ON_EdgeSoftening::SetOn(bool b)
{
  m_impl->SetParameter(ON_EDGE_SOFTENING_ON, b);
}

double ON_EdgeSoftening::Softening(void) const
{
  return m_impl->GetParameter(ON_EDGE_SOFTENING_SOFTENING, 0.1).AsDouble();
}

void ON_EdgeSoftening::SetSoftening(double d)
{
  m_impl->SetParameter(ON_EDGE_SOFTENING_SOFTENING, d);
}

bool ON_EdgeSoftening::Chamfer(void) const
{
  return m_impl->GetParameter(ON_EDGE_SOFTENING_CHAMFER, false).AsBool();
}

void ON_EdgeSoftening::SetChamfer(bool b)
{
  m_impl->SetParameter(ON_EDGE_SOFTENING_CHAMFER, b);
}

bool ON_EdgeSoftening::Faceted(void) const
{
  return m_impl->GetParameter(ON_EDGE_SOFTENING_UNWELD, false).AsBool();
}

void ON_EdgeSoftening::SetFaceted(bool b)
{
  m_impl->SetParameter(ON_EDGE_SOFTENING_UNWELD, b);
}

double ON_EdgeSoftening::EdgeAngleThreshold(void) const
{
  return m_impl->GetParameter(ON_EDGE_SOFTENING_EDGE_THRESHOLD, 5.0).AsDouble();
}

void ON_EdgeSoftening::SetEdgeAngleThreshold(double d)
{
  m_impl->SetParameter(ON_EDGE_SOFTENING_EDGE_THRESHOLD, d);
}

bool ON_EdgeSoftening::ForceSoftening(void) const
{
  return m_impl->GetParameter(ON_EDGE_SOFTENING_FORCE_SOFTENING, false).AsBool();
}

void ON_EdgeSoftening::SetForceSoftening(bool b)
{
  m_impl->SetParameter(ON_EDGE_SOFTENING_FORCE_SOFTENING, b);
}

////////////////////////////////////////////////////////////////
//
// Thickening
//
////////////////////////////////////////////////////////////////

#define ON_THICKNESS_ROOT           L"thickening-object-data"
  #define ON_THICKNESS_ON           L"on"
  #define ON_THICKNESS_DISTANCE     L"distance"
  #define ON_THICKNESS_SOLID        L"solid"
  #define ON_THICKNESS_BOTH_SIDES   L"both-sides"
  #define ON_THICKNESS_OFFSET_ONLY  L"offset-only"

ON_Thickening::ON_Thickening()
{
}

ON_Thickening::ON_Thickening(ON_XMLNode& model_node)
  :
  ON_MeshModifier(model_node)
{
}

ON_Thickening::ON_Thickening(const ON_Thickening& t)
{
  operator = (t);
}

const ON_Thickening& ON_Thickening::operator = (const ON_Thickening& t)
{
  if (this != &t)
  {
    SetOn        (t.On());
    SetDistance  (t.Distance());
    SetSolid     (t.Solid());
    SetOffsetOnly(t.OffsetOnly());
    SetBothSides (t.BothSides());
  }

  return *this;
}

bool ON_Thickening::operator == (const ON_Thickening& t) const
{
  if (On()         != t.On())         return false;
  if (Distance()   != t.Distance())   return false;
  if (Solid()      != t.Solid())      return false;
  if (OffsetOnly() != t.OffsetOnly()) return false;
  if (BothSides()  != t.BothSides())  return false;

  return true;
}

bool ON_Thickening::operator != (const ON_Thickening& t) const
{
  return !(operator == (t));
}

bool ON_Thickening::On(void) const
{
  return m_impl->GetParameter(ON_THICKNESS_ON, false).AsBool();
}

void ON_Thickening::SetOn(bool b)
{
  m_impl->SetParameter(ON_THICKNESS_ON, b);
}

double ON_Thickening::Distance(void) const
{
  return m_impl->GetParameter(ON_THICKNESS_DISTANCE, 0.1).AsDouble();
}

void ON_Thickening::SetDistance(double d)
{
  m_impl->SetParameter(ON_THICKNESS_DISTANCE, d);
}

bool ON_Thickening::Solid(void) const
{
  return m_impl->GetParameter(ON_THICKNESS_SOLID, true).AsBool();
}

void ON_Thickening::SetSolid(bool b)
{
  m_impl->SetParameter(ON_THICKNESS_SOLID, b);
}

bool ON_Thickening::OffsetOnly(void) const
{
  return m_impl->GetParameter(ON_THICKNESS_OFFSET_ONLY, false).AsBool();
}

void ON_Thickening::SetOffsetOnly(bool b)
{
  m_impl->SetParameter(ON_THICKNESS_OFFSET_ONLY, b);
}

bool ON_Thickening::BothSides(void) const
{
  return m_impl->GetParameter(ON_THICKNESS_BOTH_SIDES, false).AsBool();
}

void ON_Thickening::SetBothSides(bool b)
{
  m_impl->SetParameter(ON_THICKNESS_BOTH_SIDES, b);
}

////////////////////////////////////////////////////////////////
//
// CurvePiping
//
////////////////////////////////////////////////////////////////

#define ON_CURVE_PIPING_ROOT        L"curve-piping-object-data"
  #define ON_CURVE_PIPING_ON        L"on"
  #define ON_CURVE_PIPING_SEGMENTS  L"segments"
  #define ON_CURVE_PIPING_RADIUS    L"radius"
  #define ON_CURVE_PIPING_ACCURACY  L"accuracy"
  #define ON_CURVE_PIPING_WELD      L"weld"
  #define ON_CURVE_PIPING_CAP_TYPE  L"cap-type"
    #define ON_CURVE_PIPING_NONE    L"none"
    #define ON_CURVE_PIPING_FLAT    L"flat"
    #define ON_CURVE_PIPING_BOX     L"box"
    #define ON_CURVE_PIPING_DOME    L"dome"

ON_CurvePiping::ON_CurvePiping()
{
}

ON_CurvePiping::ON_CurvePiping(ON_XMLNode& model_node)
  :
  ON_MeshModifier(model_node)
{
}

ON_CurvePiping::ON_CurvePiping(const ON_CurvePiping& cp)
{
  operator = (cp);
}

const ON_CurvePiping& ON_CurvePiping::operator = (const ON_CurvePiping& cp)
{
  if (this != &cp)
  {
    SetRadius  (cp.Radius());
    SetSegments(cp.Segments());
    SetFaceted (cp.Faceted());
    SetAccuracy(cp.Accuracy());
    SetCapType (cp.CapType());
  }

  return *this;
}

bool ON_CurvePiping::operator == (const ON_CurvePiping& cp) const
{
  if (Radius()   != cp.Radius())   return false;
  if (Segments() != cp.Segments()) return false;
  if (Faceted()  != cp.Faceted())  return false;
  if (Accuracy() != cp.Accuracy()) return false;
  if (CapType()  != cp.CapType())  return false;

  return true;
}

bool ON_CurvePiping::operator != (const ON_CurvePiping& cp) const
{
  return !(operator == (cp));
}

bool ON_CurvePiping::On(void) const
{
  return m_impl->GetParameter(ON_CURVE_PIPING_ON, false).AsBool();
}

void ON_CurvePiping::SetOn(bool b)
{
  m_impl->SetParameter(ON_CURVE_PIPING_ON, b);
}

double ON_CurvePiping::Radius(void) const
{
  return m_impl->GetParameter(ON_CURVE_PIPING_RADIUS, 1.0).AsDouble();
}

void ON_CurvePiping::SetRadius(double d)
{
  m_impl->SetParameter(ON_CURVE_PIPING_RADIUS, d);
}

int ON_CurvePiping::Segments(void) const
{
  return m_impl->GetParameter(ON_CURVE_PIPING_SEGMENTS, 16).AsInteger();
}

void ON_CurvePiping::SetSegments(int s)
{
  m_impl->SetParameter(ON_CURVE_PIPING_SEGMENTS, s);
}

bool ON_CurvePiping::Faceted(void) const
{
  return m_impl->GetParameter(ON_CURVE_PIPING_WELD, false).AsBool();
}

void ON_CurvePiping::SetFaceted(bool b)
{
  m_impl->SetParameter(ON_CURVE_PIPING_WELD, b);
}

int ON_CurvePiping::Accuracy(void) const
{
  return m_impl->GetParameter(ON_CURVE_PIPING_ACCURACY, 50).AsInteger();
}

void ON_CurvePiping::SetAccuracy(int a)
{
  m_impl->SetParameter(ON_CURVE_PIPING_ACCURACY, std::min(100, std::max(0, a)));
}

static const wchar_t* CapTypeToString(ON_CurvePiping::CapTypes ct)
{
  switch (ct)
  {
  case ON_CurvePiping::CapTypes::Flat: return ON_CURVE_PIPING_FLAT;
  case ON_CurvePiping::CapTypes::Box:  return ON_CURVE_PIPING_BOX;
  case ON_CurvePiping::CapTypes::Dome: return ON_CURVE_PIPING_DOME;
  }

  return ON_CURVE_PIPING_NONE;
}

ON_CurvePiping::CapTypes ON_CurvePiping::CapType(void) const
{
  const auto s = m_impl->GetParameter(ON_CURVE_PIPING_CAP_TYPE, L"").AsString();

  if (s == ON_CURVE_PIPING_FLAT) return ON_CurvePiping::CapTypes::Flat;
  if (s == ON_CURVE_PIPING_BOX)  return ON_CurvePiping::CapTypes::Box;
  if (s == ON_CURVE_PIPING_DOME) return ON_CurvePiping::CapTypes::Dome;

  return ON_CurvePiping::CapTypes::None;
}

void ON_CurvePiping::SetCapType(CapTypes ct)
{
  const auto* s = CapTypeToString(ct);
  m_impl->SetParameter(ON_CURVE_PIPING_CAP_TYPE, s);
}

////////////////////////////////////////////////////////////////
//
// Shutlining
//
////////////////////////////////////////////////////////////////

#define ON_SHUTLINING_ROOT               L"shut-lining-object-data"
  #define ON_SHUTLINING_ON               L"on"
  #define ON_SHUTLINING_CURVE            L"curve"
    #define ON_SHUTLINING_CURVE_UUID     L"uuid"
    #define ON_SHUTLINING_CURVE_ENABLED  L"enabled"
    #define ON_SHUTLINING_CURVE_RADIUS   L"radius"
    #define ON_SHUTLINING_CURVE_PROFILE  L"profile"
    #define ON_SHUTLINING_CURVE_PULL     L"pull"
    #define ON_SHUTLINING_CURVE_IS_BUMP  L"is-bump"
  #define ON_SHUTLINING_FACETED          L"faceted"
  #define ON_SHUTLINING_AUTO_UPDATE      L"auto-update"
  #define ON_SHUTLINING_FORCE_UPDATE     L"force-update"

class ON_Shutlining::CImplSL
{
public:
  ON_SimpleArray<Curve*> m_curves;
};

ON_Shutlining::ON_Shutlining()
{
  m_impl_sl = new CImplSL;
}

ON_Shutlining::ON_Shutlining(ON_XMLNode& model_node)
  :
  ON_MeshModifier(model_node)
{
  m_impl_sl = new CImplSL;

  auto it = model_node.GetChildIterator();
  while (auto* child_node = it.GetNextChild())
  {
    if (ON_SHUTLINING_CURVE == child_node->TagName())
    {
      m_impl_sl->m_curves.Append(new Curve(*child_node));
    }
  }
}

ON_Shutlining::ON_Shutlining(const ON_Shutlining& sl)
{
  m_impl_sl = new CImplSL;
  operator = (sl);
}

ON_Shutlining::~ON_Shutlining()
{
  DeleteAllCurves();

  delete m_impl_sl;
  m_impl_sl = nullptr;
}

const ON_Shutlining& ON_Shutlining::operator = (const ON_Shutlining& sl)
{
  if (this != &sl)
  {
    // Copy basic parameters.
    SetOn(sl.On());
    SetFaceted(sl.Faceted());
    SetAutoUpdate(sl.AutoUpdate());
    SetForceUpdate(sl.ForceUpdate());

    // Copy curves.
    DeleteAllCurves();
    auto it = sl.GetCurveIterator();
    while (auto* sl_curve = it.Next())
    {
      auto& new_curve = AddCurve();
      new_curve = *sl_curve;
    }
  }

  return *this;
}

ON_Shutlining::Curve* ON_Shutlining::FindCurve(const ON_UUID& id) const
{
  auto it = GetCurveIterator();
  while (auto* curve = it.Next())
  {
    if (curve->Id() == id)
      return curve;
  }

  return nullptr;
}

bool ON_Shutlining::operator == (const ON_Shutlining& sl) const
{
  if (On()          != sl.On())          return false;
  if (Faceted()     != sl.Faceted())     return false;
  if (AutoUpdate()  != sl.AutoUpdate())  return false;
  if (ForceUpdate() != sl.ForceUpdate()) return false;

  auto it1 = GetCurveIterator();
  while (auto* curve = it1.Next())
  {
    const auto* sl_curve = sl.FindCurve(curve->Id());
    if (nullptr == sl_curve)
      return false;
  }

  auto it2 = sl.GetCurveIterator();
  while (auto* sl_curve = it2.Next())
  {
    const auto* curve = FindCurve(sl_curve->Id());
    if ((nullptr == curve) || (*curve != *sl_curve))
      return false;
  }

  return true;
}

bool ON_Shutlining::operator != (const ON_Shutlining& sl) const
{
  return !(operator == (sl));
}

bool ON_Shutlining::On(void) const
{
  return m_impl->GetParameter(ON_SHUTLINING_ON, false).AsBool();
}

void ON_Shutlining::SetOn(bool b)
{
  m_impl->SetParameter(ON_SHUTLINING_ON, b);
}

bool ON_Shutlining::Faceted(void) const
{
  return m_impl->GetParameter(ON_SHUTLINING_FACETED, false).AsBool();
}

void ON_Shutlining::SetFaceted(bool b)
{
  m_impl->SetParameter(ON_SHUTLINING_FACETED, b);
}

bool ON_Shutlining::AutoUpdate(void) const
{
  return m_impl->GetParameter(ON_SHUTLINING_AUTO_UPDATE, false).AsBool();
}

void ON_Shutlining::SetAutoUpdate(bool b)
{
  m_impl->SetParameter(ON_SHUTLINING_AUTO_UPDATE, b);
}

bool ON_Shutlining::ForceUpdate(void) const
{
  return m_impl->GetParameter(ON_SHUTLINING_FORCE_UPDATE, false).AsBool();
}

void ON_Shutlining::SetForceUpdate(bool b)
{
  m_impl->SetParameter(ON_SHUTLINING_FORCE_UPDATE, b);
}

ON_Shutlining::CurveIterator ON_Shutlining::GetCurveIterator(void) const
{
  return CurveIterator(*this);
}

ON_Shutlining::Curve& ON_Shutlining::AddCurve(void)
{
  auto* curve_node = m_impl->Node().AttachChildNode(new ON_XMLNode(ON_SHUTLINING_CURVE));
  auto* curve = new Curve(*curve_node);
  m_impl_sl->m_curves.Append(curve);

  return *curve;
}

void ON_Shutlining::DeleteAllCurves(void)
{
  for (int i = 0; i < m_impl_sl->m_curves.Count(); i++)
  {
    delete m_impl_sl->m_curves[i];
  }

  m_impl_sl->m_curves.Destroy();
}

class ON_Shutlining::Curve::CImpl final
{
public:
  CImpl(ON_XMLNode& n) : m_node(n) { }

  ON_XMLVariant GetParam(const wchar_t* param_name, const ON_XMLVariant& def) const
  {
    const auto* child_node = m_node.GetNamedChild(param_name);
    if (nullptr != child_node)
      return child_node->GetDefaultProperty().GetValue();

    return def;
  }

  void SetParam(const wchar_t* param_name, const ON_XMLVariant& v)
  {
    const auto* child_node = m_node.CreateNodeAtPath(param_name);
    child_node->GetDefaultProperty().SetValue(v);
  }

  ON_XMLNode& m_node;
};

ON_Shutlining::Curve::Curve(ON_XMLNode& curve_node)
{
  m_impl = new CImpl(curve_node);
}

ON_Shutlining::Curve::~Curve()
{
  delete m_impl;
  m_impl = nullptr;
}

const ON_Shutlining::Curve& ON_Shutlining::Curve::operator = (const ON_Shutlining::Curve& c)
{
  if (this != &c)
  {
    SetId     (c.Id());
    SetEnabled(c.Enabled());
    SetRadius (c.Radius());
    SetProfile(c.Profile());
    SetPull   (c.Pull());
    SetIsBump (c.IsBump());
  }

  return *this;
}

bool ON_Shutlining::Curve::operator == (const Curve& c) const
{
  if (Id()      != c.Id())      return false;
  if (Enabled() != c.Enabled()) return false;
  if (Radius()  != c.Radius())  return false;
  if (Profile() != c.Profile()) return false;
  if (Pull()    != c.Pull())    return false;
  if (IsBump()  != c.IsBump())  return false;

  return true;
}

bool ON_Shutlining::Curve::operator != (const Curve& c) const
{
  return !(operator == (c));
}

ON_UUID ON_Shutlining::Curve::Id(void) const
{
  return m_impl->GetParam(ON_SHUTLINING_CURVE_UUID, ON_nil_uuid);
}

void ON_Shutlining::Curve::SetId(const ON_UUID& id)
{
  m_impl->SetParam(ON_SHUTLINING_CURVE_UUID, id);
}

double ON_Shutlining::Curve::Radius(void) const
{
  return m_impl->GetParam(ON_SHUTLINING_CURVE_RADIUS, 1.0);
}

void ON_Shutlining::Curve::SetRadius(double d)
{
  m_impl->SetParam(ON_SHUTLINING_CURVE_RADIUS, d);
}

int ON_Shutlining::Curve::Profile(void) const
{
  return m_impl->GetParam(ON_SHUTLINING_CURVE_PROFILE, 0);
}

void ON_Shutlining::Curve::SetProfile(int p)
{
  m_impl->SetParam(ON_SHUTLINING_CURVE_PROFILE, p);
}

bool ON_Shutlining::Curve::Enabled(void) const
{
  return m_impl->GetParam(ON_SHUTLINING_CURVE_ENABLED, false);
}

void ON_Shutlining::Curve::SetEnabled(bool b)
{
  m_impl->SetParam(ON_SHUTLINING_CURVE_ENABLED, b);
}

bool ON_Shutlining::Curve::Pull(void) const
{
  return m_impl->GetParam(ON_SHUTLINING_CURVE_PULL, false);
}

void ON_Shutlining::Curve::SetPull(bool b)
{
  m_impl->SetParam(ON_SHUTLINING_CURVE_PULL, b);
}

bool ON_Shutlining::Curve::IsBump(void) const
{
  return m_impl->GetParam(ON_SHUTLINING_CURVE_IS_BUMP, false);
}

void ON_Shutlining::Curve::SetIsBump(bool b)
{
  m_impl->SetParam(ON_SHUTLINING_CURVE_IS_BUMP, b);
}

class ON_Shutlining::CurveIterator::CImpl
{
public:
  CImpl(const ON_Shutlining& sl) : m_sl(sl) { }

  Curve* Next(void)
  {
    if (m_index >= m_sl.m_impl_sl->m_curves.Count())
      return nullptr;

    return m_sl.m_impl_sl->m_curves[m_index++];
  }

  const ON_Shutlining& m_sl;
  int m_index = 0;
};

ON_Shutlining::CurveIterator::CurveIterator(const ON_Shutlining& sl)
{
  m_impl = new CImpl(sl);
}

ON_Shutlining::CurveIterator::~CurveIterator()
{
  delete m_impl;
  m_impl = nullptr;
}

ON_Shutlining::Curve* ON_Shutlining::CurveIterator::Next(void)
{
  return m_impl->Next();
}

// ON_MeshModifiers

class ON_MeshModifiers::CImpl
{
public:
  CImpl(ON_XMLNode& node);
  ~CImpl();

  ON_XMLNode m_displacement_node;
  ON_XMLNode m_edge_softening_node;
  ON_XMLNode m_thickening_node;
  ON_XMLNode m_curve_piping_node;
  ON_XMLNode m_shutlining_node;

  ON_Displacement * m_displacement   = nullptr;
  ON_EdgeSoftening* m_edge_softening = nullptr;
  ON_Thickening   * m_thickening     = nullptr;
  ON_CurvePiping  * m_curve_piping   = nullptr;
  ON_Shutlining   * m_shutlining     = nullptr;
};

ON_MeshModifiers::CImpl::CImpl(ON_XMLNode& model_node)
  :
  m_displacement_node  (ON_DISPLACEMENT_ROOT),
  m_edge_softening_node(ON_EDGE_SOFTENING_ROOT),
  m_thickening_node    (ON_THICKNESS_ROOT),
  m_curve_piping_node  (ON_CURVE_PIPING_ROOT),
  m_shutlining_node    (ON_SHUTLINING_ROOT)
{
  // Iterate over the child nodes of the incoming model node and copy them to the relevent mesh modifier nodes.
  auto it = model_node.GetChildIterator();
  while (auto* child_node = it.GetNextChild())
  {
    const auto& s = child_node->TagName();

    if (ON_DISPLACEMENT_ROOT == s)
    {
      m_displacement_node = *child_node;
    }
    else
    if (ON_EDGE_SOFTENING_ROOT == s)
    {
      m_edge_softening_node = *child_node;
    }
    else
    if (ON_THICKNESS_ROOT == s)
    {
      m_thickening_node = *child_node;
    }
    else
    if (ON_CURVE_PIPING_ROOT == s)
    {
      m_curve_piping_node = *child_node;
    }
    else
    if (ON_SHUTLINING_ROOT == s)
    {
      m_shutlining_node = *child_node;
    }
  }

  // Create the necessary mesh modifiers using the above-initialized nodes. The mesh modifiers store pointers
  // to these nodes. This is safe because the nodes have the same lifetime as the mesh modifiers.
  m_displacement   = new ON_Displacement (m_displacement_node);
  m_edge_softening = new ON_EdgeSoftening(m_edge_softening_node);
  m_thickening     = new ON_Thickening   (m_thickening_node);
  m_curve_piping   = new ON_CurvePiping  (m_curve_piping_node);
  m_shutlining     = new ON_Shutlining   (m_shutlining_node);
}

ON_MeshModifiers::CImpl::~CImpl()
{
  delete m_displacement;
  delete m_edge_softening;
  delete m_thickening;
  delete m_curve_piping;
  delete m_shutlining;
}

ON_MeshModifiers::ON_MeshModifiers(const ON_ModelComponent& component, const wchar_t* xml)
{
  ON_XMLRootNode node;
  node.ReadFromStream(xml);
  m_impl = new CImpl(node);
}

ON_MeshModifiers::~ON_MeshModifiers()
{
  delete m_impl;
  m_impl = nullptr;
}

ON_Displacement& ON_MeshModifiers::Displacement(void)
{
  return *m_impl->m_displacement;
}

ON_EdgeSoftening& ON_MeshModifiers::EdgeSoftening(void)
{
  return *m_impl->m_edge_softening;
}

ON_Thickening& ON_MeshModifiers::Thickening(void)
{
  return *m_impl->m_thickening;
}

ON_CurvePiping& ON_MeshModifiers::CurvePiping(void)
{
  return *m_impl->m_curve_piping;
}

ON_Shutlining& ON_MeshModifiers::Shutlining(void)
{
  return *m_impl->m_shutlining;
}

// ON_MeshModifierCollection

ON_MeshModifierCollection::~ON_MeshModifierCollection()
{
  for (const auto& elem : m_map)
  {
    delete elem.second;
  }

  m_map.clear();
}

ON_MeshModifiers* ON_MeshModifierCollection::Find(const ON_ModelComponent& component)
{
  const auto& elem = m_map.find(component.Id());
  if (elem != m_map.end())
    return elem->second;

  return nullptr;
}

bool ON_MeshModifierCollection::CreateMeshModifiersFromXML(const ONX_Model& model)
{
  ONX_ModelComponentIterator cit(model, ON_ModelComponent::Type::ModelGeometry);
  const auto* component = cit.FirstComponent();
  for (; nullptr != component; component = cit.NextComponent())
  {
    auto* attr = GetComponentAttributes(*component);
    if (nullptr == attr)
      continue; // No attributes on component.

    // Get the entire XML off of the attributes user data.
    ON_wString xml;
    ONX_Model::GetMeshModifierObjectInformation(*attr, xml);
    if (xml.IsEmpty())
      continue; // No XML found on the component's attributes.

    // Create a new mesh modifiers object for this component and add it to the map.
    std::pair<ON_UUID, ON_MeshModifiers*> pair(component->Id(), new ON_MeshModifiers(*component, xml));
    m_map.insert(pair);
  }

  return true;
}

static const ON_UUID uuid_displacement_user_data
{
  0x8224a7c4, 0x5590, 0x4ac4, { 0xa3, 0x2c, 0xde, 0x85, 0xdc, 0x2f, 0xfd, 0xae }
};

static const ON_UUID uuid_edge_softening_user_data =
{
  0x8cbe6160, 0x5cbd, 0x4b4d, { 0x8c, 0xd2, 0x7c, 0xe0, 0xa7, 0xc8, 0xc2, 0xd8 }
};

static const ON_UUID uuid_shutlining_user_data =
{
  0x07506ebe, 0x1d69, 0x4345, { 0x9f, 0x0d, 0x2b, 0x9a, 0xa1, 0x90, 0x6e, 0xef }
};

static const ON_UUID uuid_curve_piping_user_data =
{
  0x2b1a758e, 0x7cb1, 0x45ab, { 0xa5, 0xbf, 0xdf, 0xcd, 0x6d, 0x3d, 0x13, 0x6d }
};

static const ON_UUID uuid_thickening_user_data =
{
  0x6aa7ccc3, 0x2721, 0x410f, { 0xaa, 0x56, 0xe8, 0xab, 0x4f, 0x3e, 0xce, 0x67 }
};

bool ON_MeshModifierCollection::CreateXMLFromMeshModifiers(const ONX_Model& model)
{
  for (const auto& elem : m_map)
  {
    auto* mesh_modifiers = elem.second;
    if (nullptr == mesh_modifiers)
      continue;

    const auto& mgc = model.ModelGeometryComponentFromId(elem.first);
    ON_ASSERT(mgc.ComponentType() == ON_ModelComponent::Type::ModelGeometry);

    // Mesh modifiers are stored on the user data of object attributes.
    auto* attr = const_cast<ON_3dmObjectAttributes*>(mgc.Attributes(nullptr));
    if (nullptr == attr)
      continue; // Should never happen because we have an item for the object.

    SetMeshModifierObjectInformation(*attr, uuid_displacement_user_data,   mesh_modifiers->Displacement());
    SetMeshModifierObjectInformation(*attr, uuid_edge_softening_user_data, mesh_modifiers->EdgeSoftening());
    SetMeshModifierObjectInformation(*attr, uuid_thickening_user_data,     mesh_modifiers->Thickening());
    SetMeshModifierObjectInformation(*attr, uuid_curve_piping_user_data,   mesh_modifiers->CurvePiping());
    SetMeshModifierObjectInformation(*attr, uuid_shutlining_user_data,     mesh_modifiers->Shutlining());
  }

  return true;
}
