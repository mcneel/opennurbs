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

  #define ON_RDK_UD_DECALS                            L"decals"
    #define ON_RDK_UD_DECAL                             L"decal"
      #define ON_RDK_UD_DECAL_CUSTOM                      L"custom"
      #define ON_RDK_UD_DECAL_CUSTOM_RENDERER             L"renderer"
      #define ON_RDK_UD_DECAL_CUSTOM_PARAMS                   L"parameters"
      #define ON_RDK_UD_DECAL_INSTANCE_ID                 L"instance-id"

      #define ON_RDK_UD_DECAL_MAPPING                     L"mapping"
      //----- +++++++++++++++++++
      #define ON_RDK_UD_DECAL_MAPPING_PLANAR              L"planar"
      #define ON_RDK_UD_DECAL_MAPPING_CYLINDRICAL         L"cylindrical"
      #define ON_RDK_UD_DECAL_MAPPING_SPHERICAL           L"spherical"
      #define ON_RDK_UD_DECAL_MAPPING_UV                  L"uv"
      #define ON_RDK_UD_DECAL_MAPPING_NONE                L"none"

      #define ON_RDK_UD_DECAL_PROJECTION                  L"projection"
      //----- ++++++++++++++++++++++
      #define ON_RDK_UD_DECAL_PROJECTION_FORWARD          L"forward"
      #define ON_RDK_UD_DECAL_PROJECTION_BACKWARD         L"backward"
      #define ON_RDK_UD_DECAL_PROJECTION_BOTH             L"both"
      #define ON_RDK_UD_DECAL_PROJECTION_NONE             L"none"

      #define ON_RDK_UD_DECAL_MAP_TO_INSIDE_ON            L"map-to-inside-on"
      #define ON_RDK_UD_DECAL_TRANSPARENCY                L"transparency"
      #define ON_RDK_UD_DECAL_TEXTURE_INSTANCE            L"texture-instance"
      #define ON_RDK_UD_DECAL_HEIGHT                      L"height"
      #define ON_RDK_UD_DECAL_ORIGIN                      L"origin"
      #define ON_RDK_UD_DECAL_RADIUS                      L"radius"
      #define ON_RDK_UD_DECAL_HORZ_SWEEP_STA              L"latitude-start"
      #define ON_RDK_UD_DECAL_HORZ_SWEEP_END              L"latitude-stop"
      #define ON_RDK_UD_DECAL_VERT_SWEEP_STA              L"longitude-start"
      #define ON_RDK_UD_DECAL_VERT_SWEEP_END              L"longitude-stop"
      #define ON_RDK_UD_DECAL_VECTOR_UP                   L"vector-up"
      #define ON_RDK_UD_DECAL_VECTOR_ACROSS               L"vector-across"
      #define ON_RDK_UD_DECAL_MIN_U                       L"min-u"
      #define ON_RDK_UD_DECAL_MIN_V                       L"min-v"
      #define ON_RDK_UD_DECAL_MAX_U                       L"max-u"
      #define ON_RDK_UD_DECAL_MAX_V                       L"max-v"
      #define ON_RDK_UD_DECAL_IS_TEMPORARY                L"is-temporary"
      #define ON_RDK_UD_DECAL_IS_VISIBLE                  L"is-visible"

// ON_Decal

class ON_Decal::CImpl : public ON_InternalXMLImpl
{
public:
  CImpl() { ON_CreateUuid(m_decal_id); }
  CImpl(ON_XMLNode& n) { ON_CreateUuid(m_decal_id); Node() = n; }

  ON_UUID m_decal_id;
};

static const wchar_t* XMLPath(void)
{
  return L""; // The node is the actual decal node.
}

ON_Decal::ON_Decal()
{
  m_impl = new CImpl;
}

ON_Decal::ON_Decal(ON_XMLNode& model_node)
{
  m_impl = new CImpl(model_node);
}

ON_Decal::ON_Decal(const ON_Decal& d)
{
  m_impl = new CImpl;
  operator = (d);
}

ON_Decal::~ON_Decal()
{
  delete m_impl;
  m_impl = nullptr;
}

const ON_Decal& ON_Decal::operator = (const ON_Decal& d)
{
  if (this != &d)
  {
    SetTextureInstanceId(d.TextureInstanceId());
    SetMapping(d.Mapping());
    SetProjection(d.Projection());
    SetMapToInside(d.MapToInside());
    SetTransparency(d.Transparency());
    SetOrigin(d.Origin());
    SetVectorUp(d.VectorUp());
    SetVectorAcross(d.VectorAcross());
    SetHeight(d.Height());
    SetRadius(d.Radius());

    double sta = 0.0, end = 0.0;
    d.GetHorzSweep(sta, end);
    SetHorzSweep(sta, end);
    d.GetVertSweep(sta, end);
    SetVertSweep(sta, end);

    double min_u = 0.0, min_v = 0.0, max_u = 0.0, max_v = 0.0;
    d.UVBounds(min_u, min_v, max_u, max_v);
    SetUVBounds(min_u, min_v, max_u, max_v);
  }

  return *this;
}

bool ON_Decal::operator == (const ON_Decal& d)
{
  if (TextureInstanceId() != d.TextureInstanceId()) return false;
  if (Mapping()           != d.Mapping())           return false;
  if (Projection()        != d.Projection())        return false;
  if (MapToInside()       != d.MapToInside())       return false;
  if (Transparency()      != d.Transparency())      return false;
  if (Origin()            != d.Origin())            return false;
  if (VectorUp()          != d.VectorUp())          return false;
  if (VectorAcross()      != d.VectorAcross())      return false;
  if (Height()            != d.Height())            return false;
  if (Radius()            != d.Radius())            return false;

  double sta1 = 0.0, end1 = 0.0, sta2 = 0.0, end2 = 0.0;
    GetHorzSweep(sta1, end1);
  d.GetHorzSweep(sta2, end2);
  if (!IsDoubleEqual(sta1, sta2)) return false;
  if (!IsDoubleEqual(end1, end2)) return false;

    GetVertSweep(sta1, end1);
  d.GetVertSweep(sta2, end2);
  if (!IsDoubleEqual(sta1, sta2)) return false;
  if (!IsDoubleEqual(end1, end2)) return false;

  double min_u1 = 0.0, min_v1 = 0.0, max_u1 = 0.0, max_v1 = 0.0;
  double min_u2 = 0.0, min_v2 = 0.0, max_u2 = 0.0, max_v2 = 0.0;
    UVBounds(min_u1, min_v1, max_u1, max_v1);
  d.UVBounds(min_u2, min_v2, max_u2, max_v2);
  if (!IsDoubleEqual(min_u1, min_u2)) return false;
  if (!IsDoubleEqual(min_v1, min_v2)) return false;
  if (!IsDoubleEqual(max_u1, max_u2)) return false;
  if (!IsDoubleEqual(max_v1, max_v2)) return false;

  return true;
}

bool ON_Decal::operator != (const ON_Decal& d)
{
  return !(operator == (d));
}

ON_UUID ON_Decal::TextureInstanceId(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_TEXTURE_INSTANCE, ON_nil_uuid).AsUuid();
}

void ON_Decal::SetTextureInstanceId(const ON_UUID& id)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_TEXTURE_INSTANCE, id);
}

ON_Decal::Mappings ON_Decal::Mapping(void) const
{
  const ON_wString s = m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_MAPPING, ON_RDK_UD_DECAL_MAPPING_UV).AsString();

  if (ON_RDK_UD_DECAL_MAPPING_UV == s)
    return ON_Decal::Mappings::UV;

  if (ON_RDK_UD_DECAL_MAPPING_PLANAR == s)
    return ON_Decal::Mappings::Planar;

  if (ON_RDK_UD_DECAL_MAPPING_SPHERICAL == s)
    return ON_Decal::Mappings::Spherical;

  if (ON_RDK_UD_DECAL_MAPPING_CYLINDRICAL == s)
    return ON_Decal::Mappings::Cylindrical;

  return ON_Decal::Mappings::None;
}

void ON_Decal::SetMapping(Mappings m)
{
  const wchar_t* s = L"";
  switch (m)
  {
  case ON_Decal::Mappings::UV:          s = ON_RDK_UD_DECAL_MAPPING_UV;          break;
  case ON_Decal::Mappings::Planar:      s = ON_RDK_UD_DECAL_MAPPING_PLANAR;      break;
  case ON_Decal::Mappings::Spherical:   s = ON_RDK_UD_DECAL_MAPPING_SPHERICAL;   break;
  case ON_Decal::Mappings::Cylindrical: s = ON_RDK_UD_DECAL_MAPPING_CYLINDRICAL; break;
  case ON_Decal::Mappings::None: break;
  }

  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_MAPPING, s);
}

ON_Decal::Projections ON_Decal::Projection(void) const
{
  const ON_wString s = m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_PROJECTION, ON_RDK_UD_DECAL_PROJECTION_FORWARD).AsString();

  if (ON_RDK_UD_DECAL_PROJECTION_FORWARD == s)
    return ON_Decal::Projections::Forward;

  if (ON_RDK_UD_DECAL_PROJECTION_BACKWARD == s)
    return ON_Decal::Projections::Backward;

  if (ON_RDK_UD_DECAL_PROJECTION_BOTH == s)
    return ON_Decal::Projections::Both;

  return ON_Decal::Projections::None;
}

void ON_Decal::SetProjection(Projections p)
{
  const wchar_t* s = L"";
  switch (p)
  {
  case ON_Decal::Projections::Forward:  s = ON_RDK_UD_DECAL_PROJECTION_FORWARD;  break;
  case ON_Decal::Projections::Backward: s = ON_RDK_UD_DECAL_PROJECTION_BACKWARD; break;
  case ON_Decal::Projections::Both:     s = ON_RDK_UD_DECAL_PROJECTION_BOTH;     break;
  case ON_Decal::Projections::None: break;
  }

  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_PROJECTION, s);
}

bool ON_Decal::MapToInside(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_MAP_TO_INSIDE_ON, false).AsBool();
}

void ON_Decal::SetMapToInside(bool b)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_MAP_TO_INSIDE_ON, b);
}

double ON_Decal::Transparency(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_TRANSPARENCY, 0.0).AsDouble();
}

void ON_Decal::SetTransparency(double d)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_TRANSPARENCY, d);
}

ON_3dPoint ON_Decal::Origin(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_ORIGIN, ON_3dPoint::Origin).As3dPoint();
}

void ON_Decal::SetOrigin(const ON_3dPoint& pt)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_ORIGIN, pt);
}

ON_3dVector ON_Decal::VectorUp(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_VECTOR_UP, ON_3dPoint::Origin).As3dPoint();
}

void ON_Decal::SetVectorUp(const ON_3dVector& vec)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_VECTOR_UP, ON_3dPoint(vec));
}

ON_3dVector ON_Decal::VectorAcross(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_VECTOR_ACROSS, ON_3dPoint::Origin).As3dPoint();
}

void ON_Decal::SetVectorAcross(const ON_3dVector& vec)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_VECTOR_ACROSS, ON_3dPoint(vec));
}

double ON_Decal::Height(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_HEIGHT, 1.0).AsDouble();
}

void ON_Decal::SetHeight(double d)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_HEIGHT, d);
}

double ON_Decal::Radius(void) const
{
  return m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_RADIUS, 1.0).AsDouble();
}

void ON_Decal::SetRadius(double d)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_RADIUS, d);
}

void ON_Decal::GetHorzSweep(double& sta, double& end) const
{
  sta = m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_HORZ_SWEEP_STA, 0.0).AsDouble();
  end = m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_HORZ_SWEEP_END, 0.0).AsDouble();
}

void ON_Decal::SetHorzSweep(double sta, double end)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_HORZ_SWEEP_STA, sta);
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_HORZ_SWEEP_END, end);
}

void ON_Decal::GetVertSweep(double& sta, double& end) const
{
  sta = m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_VERT_SWEEP_STA, 0.0).AsDouble();
  end = m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_VERT_SWEEP_END, 0.0).AsDouble();
}

void ON_Decal::SetVertSweep(double sta, double end)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_VERT_SWEEP_STA, sta);
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_VERT_SWEEP_END, end);
}

void ON_Decal::UVBounds(double& min_u, double& min_v, double& max_u, double& max_v) const
{
  min_u = m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_MIN_U, 0.0).AsDouble();
  min_v = m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_MIN_V, 0.0).AsDouble();
  max_u = m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_MAX_U, 1.0).AsDouble();
  max_v = m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_MAX_V, 1.0).AsDouble();
}

void ON_Decal::SetUVBounds(double min_u, double min_v, double max_u, double max_v)
{
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_MIN_U, min_u);
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_MIN_V, min_v);
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_MAX_U, max_u);
  m_impl->SetParameter(XMLPath(), ON_RDK_UD_DECAL_MAX_V, max_v);
}

ON__UINT32 ON_Decal::DataCRC(ON__UINT32 crc) const
{
  return m_impl->NodeAt(XMLPath()).DataCRC(crc);
}

ON_UUID ON_Decal::Id(void) const
{
  return m_impl->m_decal_id;
}

const ON_XMLNode& ON_Decal::XML(void) const
{
  return m_impl->Node();
}

// ON_DecalCollection

ON_DecalCollection::~ON_DecalCollection()
{
  DeleteAllDecals();
}

void ON_DecalCollection::DeleteAllDecals(void)
{
  for (int i = 0; i < m_decals.Count(); i++)
  {
    delete m_decals[i];
  }

  m_decals.Destroy();
}

const ON_DecalCollection& ON_DecalCollection::operator = (const ON_DecalCollection& dc)
{
  DeleteAllDecals();

  for (int i = 0; i < dc.m_decals.Count(); i++)
  {
    ON_Decal* decal = dc.m_decals[i];
    if (nullptr != decal)
    {
      m_decals.Append(new ON_Decal(*decal));
    }
  }

  return *this;
}

bool ON_DecalCollection::NodeContainsDecals(const ON_XMLRootNode& node) // Static.
{
  const wchar_t* path = ON_RDK_UD_ROOT  ON_RDK_SLASH  ON_RDK_UD_DECALS;
  const ON_XMLNode* decals_node = node.GetNodeAtPath(path);
  if (nullptr == decals_node)
    return false;

  if (nullptr == decals_node->GetNamedChild(ON_RDK_UD_DECAL))
    return false;

  return true;
}

void ON_DecalCollection::Populate(const ON_XMLRootNode& node)
{
  DeleteAllDecals();

  const wchar_t* path = ON_RDK_UD_ROOT  ON_RDK_SLASH  ON_RDK_UD_DECALS;
  ON_XMLNode* decals_node = node.GetNodeAtPath(path);
  if (nullptr == decals_node)
    return; // No decals.

  // Iterate over the decals under the decals node adding a new decal for each one.
  // The decals copy the node into a local node which holds the XML for each decal
  // in that decal. This XML will be modified by the decal setters and then repacked
  // into the attribute user data later during the save process.
  auto it = decals_node->GetChildIterator();
  ON_XMLNode* child_node = nullptr;
  while (nullptr != (child_node = it.GetNextChild()))
  {
    m_decals.Append(new ON_Decal(*child_node));
  }
}

ON_Decal* ON_DecalCollection::AddDecal(void)
{
  ON_XMLNode node(ON_RDK_UD_DECAL);
  auto* decal = new ON_Decal(node);
  m_decals.Append(decal);

  return decal;
}

void CreateDecalsFromXML(const ONX_Model& model, int archive_3dm_version)
{
  ONX_ModelComponentIterator cit(model, ON_ModelComponent::Type::ModelGeometry);
  
  for (const ON_ModelComponent* component = cit.FirstComponent(); nullptr != component; component = cit.NextComponent())
  {
    const ON_3dmObjectAttributes* attr = GetComponentAttributes(*component);
    if (nullptr == attr)
      continue; // No attributes on component.

    // Get the entire XML off of the attributes user data.
    ON_wString xml;
    GetRDKObjectInformation(*attr, xml, archive_3dm_version);
    if (xml.IsEmpty())
      continue; // No XML found on the component's attributes.

    ON_XMLRootNode node;
    if (ON_XMLNode::ReadError != node.ReadFromStream(xml))
    {
      attr->Internal_PopulateDecals(node);
    }
    else
    {
      ON_ERROR("Failed to read decal XML");
    }
  }
}

void CreateXMLFromDecals(const ONX_Model& model, int archive_3dm_version)
{
  ONX_ModelComponentIterator cit(model, ON_ModelComponent::Type::ModelGeometry);

  for (const ON_ModelComponent* component = cit.FirstComponent(); nullptr != component; component = cit.NextComponent())
  {
    ON_3dmObjectAttributes* attr = GetComponentAttributes(*component);
    if (nullptr == attr)
      continue; // No attributes on component.

    const ON_SimpleArray<ON_Decal*>& decals = attr->GetDecalArray();
    if (decals.Count() == 0)
      continue; // No decals on attributes.

    ON_XMLRootNode root;
    ON_XMLNode* decals_node = root.CreateNodeAtPath(ON_RDK_UD_ROOT  ON_RDK_SLASH  ON_RDK_UD_DECALS);
    if (nullptr == decals_node)
      continue; // Failed to create node -- not likely to happen.

    for (int i = 0; i < decals.Count(); i++)
    {
      const ON_Decal* decal = decals[i];
      if (nullptr != decal)
      {
        decals_node->AttachChildNode(new ON_XMLNode(decal->XML()));
      }
    }

    SetRDKObjectInformation(*attr, root.String(), archive_3dm_version);
  }
}
