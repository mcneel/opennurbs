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
  CImpl() { ON_CreateUuid(m_uuid); }
  CImpl(ON_XMLNode& n) : ON_InternalXMLImpl(&n) { ON_CreateUuid(m_uuid); }

  ON_UUID m_uuid;
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
  const auto s = m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_MAPPING, ON_RDK_UD_DECAL_MAPPING_UV).AsString();

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
  const auto s = m_impl->GetParameter(XMLPath(), ON_RDK_UD_DECAL_PROJECTION, ON_RDK_UD_DECAL_PROJECTION_FORWARD).AsString();

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
  return m_impl->m_uuid;
}

// ON_DecalCollection

ON_DecalCollection::~ON_DecalCollection()
{
  for (int i = 0; i < m_items.Count(); i++)
  {
    delete m_items[i];
  }
}

void ON_DecalCollection::CreateDecalsFromXML(const ONX_Model& model, int archive_3dm_version)
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
    GetRDKObjectInformation(*attr, xml, archive_3dm_version);
    if (xml.IsEmpty())
      continue; // No XML found on the component's attributes.

    // Create a new item for this component.
    auto* item = new Item(component->Id());

    // Set the XML to the item. The XML is the <xml> node including (among other possible things)
    // all the decals on this component.
    item->SetXML(xml.Array());

    // Append the item to the collection.
    m_items.Append(item);
  }
}

void ON_DecalCollection::CreateXMLFromDecals(const ONX_Model& model, int archive_3dm_version)
{
  for (int i = 0; i < m_items.Count(); i++)
  {
    auto* item = m_items[i];

    const auto& mgc = model.ModelGeometryComponentFromId(item->ComponentId());
    ON_ASSERT(mgc.ComponentType() == ON_ModelComponent::Type::ModelGeometry);

    // Decals are stored on the user data of object attributes.
    auto* attr = const_cast<ON_3dmObjectAttributes*>(mgc.Attributes(nullptr));
    if (nullptr == attr)
      continue; // Should never happen because we have an item for the object.

    // Get the new decals XML as a string.
    const auto decals_xml = item->DecalsXML();

    // Get the entire XML off of the attributes user data.
    ON_XMLRootNode root;

    ON_wString xml;
    GetRDKObjectInformation(*attr, xml, archive_3dm_version);
    if (xml.IsNotEmpty())
    {
      if (ON_XMLNode::ReadError == root.ReadFromStream(xml))
        continue; // Invalid XML. Should never happen.
    }

    // Find any existing decals node, creating it if not found.
    // This also ensures that the necessary parent nodes exist.
    const auto* path = ON_RDK_UD_ROOT  ON_RDK_SLASH  ON_RDK_UD_DECALS;
    auto* old_decals_node = root.CreateNodeAtPath(path); // Never null.

    // Get the parent node of the decals node.
    auto* parent_node = old_decals_node->GetParent();

    // Delete the existing decals node.
    delete parent_node->DetachChild(*old_decals_node);

    if (decals_xml.IsNotEmpty())
    {
      // Attach a new decals node to the parent node and read it from the decals XML.
      auto* new_decals_node = parent_node->AttachChildNode(new ON_XMLNode(L""));
      new_decals_node->ReadFromStream(decals_xml);

      // Set the item's XML to the attributes user data.
      SetRDKObjectInformation(*attr, root.String(), archive_3dm_version);
    }
  }
}

int ON_DecalCollection::FindItemIndex(const ON_UUID& component_id) const
{
  for (int i = 0; i < m_items.Count(); i++)
  {
    if (m_items[i]->ComponentId() == component_id)
      return i;
  }

  return -1;
}

ON_DecalCollection::Item* ON_DecalCollection::FindItem(const ON_UUID& component_id) const
{
  const auto index = FindItemIndex(component_id);
  if (index >= 0)
    return m_items[index];

  return nullptr;
}

ON_DecalCollection::Item* ON_DecalCollection::ItemAt(int index) const
{
  if ((index < 0) || (index >= m_items.Count()))
    return nullptr;

  return m_items[index];
}

int ON_DecalCollection::ItemCount(void) const
{
  return m_items.Count();
}

ON_Decal* ON_DecalCollection::AddDecal(const ON_ModelComponent& component)
{
  ON_XMLNode decals_node(ON_RDK_UD_DECALS);

  // See if there's already an item for this component.
  auto* item = FindItem(component.Id());
  if (nullptr != item)
  {
    // Yes, so load the item's current XML into 'decals_node'.
    decals_node.ReadFromStream(item->DecalsXML());
  }
  else
  {
    // Decals are stored on the user data of object attributes.
    if (nullptr == GetComponentAttributes(component))
      return nullptr; // No attributes on component.

    // Create a new item for this component.
    item = new Item(component.Id());

    // Append the item to the collection.
    m_items.Append(item);
  }

  // Count the number of existing decal nodes. This will be the index of the new decal.
  int index = 0;
  auto it = decals_node.GetChildIterator();
  ON_XMLNode* child_node = nullptr;
  while (nullptr != (child_node = it.GetNextChild()))
  {
    index++;
  }

  // Attach a child node for the new decal.
  decals_node.AttachChildNode(new ON_XMLNode(ON_RDK_UD_DECAL));

  // Set the XML to the item.
  item->SetDecalsXML(decals_node);

  // Return the decal just added.
  return item->GetDecal(index);
}

ON_Decal* ON_DecalCollection::GetDecal(const ON_UUID& id)
{
  for (int i = 0; i < m_items.Count(); i++)
  {
    auto* decal = m_items[i]->GetDecal(id);
    if (nullptr != decal)
      return decal;
  }

  return nullptr;
}

// ON_DecalCollection::Item

ON_DecalCollection::Item::Item(const ON_UUID& cid)
  :
  m_decals_node(L""),
  m_component_id(cid)
{
}

ON_DecalCollection::Item::~Item()
{
  DeleteAllDecals();
}

void ON_DecalCollection::Item::DeleteAllDecals(void)
{
  for (int i = 0; i < m_decals.Count(); i++)
  {
    delete m_decals[i];
  }

  m_decals.Destroy();
}

ON_wString ON_DecalCollection::Item::DecalsXML(void) const
{
  return m_decals_node.String();
}

bool ON_DecalCollection::Item::SetXML(const wchar_t* xml)
{
  ON_XMLRootNode root;
  if (!root.ReadFromStream(xml))
    return false;

  const auto* path = ON_RDK_UD_ROOT  ON_RDK_SLASH  ON_RDK_UD_DECALS;
  auto* decals_node = root.GetNodeAtPath(path);
  if (nullptr == decals_node)
    return false; // No decals in the item.

  return SetDecalsXML(*decals_node);
}

bool ON_DecalCollection::Item::SetDecalsXML(const ON_XMLNode& decals_node)
{
  DeleteAllDecals();

  m_decals_node = decals_node;

  // Iterate over the decals under the decals node adding a new decal for each one.
  // The decals store the pointer to their node inside the decals node in the collection.
  // This is safe because the decals have the same lifetime as the collection.
  auto it = m_decals_node.GetChildIterator();
  ON_XMLNode* child_node = nullptr;
  while (nullptr != (child_node = it.GetNextChild()))
  {
    m_decals.Append(new ON_Decal(*child_node));
  }

  return true;
}

ON_Decal* ON_DecalCollection::Item::GetDecal(int index) const
{
  if (index >= m_decals.Count())
    return nullptr;

  return m_decals[index];
}

ON_Decal* ON_DecalCollection::Item::GetDecal(const ON_UUID& id) const
{
  for (int i = 0; i < m_decals.Count(); i++)
  {
    auto* decal = m_decals[i];
    if (decal->Id() == id)
      return decal;
  }

  return nullptr;
}

// ON_DecalIterator

class ON_DecalIterator::CImpl
{
public:
  CImpl(ONX_Model& model) : m_model(model) { }
  virtual ~CImpl() { }

  virtual ON_Decal* Next(void) = 0;

protected:
  ONX_Model& m_model;
  int m_decal_index = -1;
};

class CImpl_PerComponent : public ON_DecalIterator::CImpl
{
public:
  CImpl_PerComponent(ONX_Model& model, const ON_ModelComponent& component)
    :
    CImpl(model)
  {
    m_component_id = component.Id();
  }

  virtual ON_Decal* Next(void) override
  {
    if (nullptr == m_item)
    {
      const auto& collection = GetDecalCollection(m_model);

      m_item = collection.FindItem(m_component_id);
      if (nullptr == m_item)
        return nullptr;

      m_decal_index = 0;
    }

    return m_item->GetDecal(m_decal_index++);
  }

public:
  ON_UUID m_component_id;
  ON_DecalCollection::Item* m_item = nullptr;
};

class CImpl_All : public ON_DecalIterator::CImpl
{
public:
  CImpl_All(ONX_Model& model) : CImpl(model) { }

  virtual ON_Decal* Next(void) override
  {
    if (!m_populated)
    {
      const auto& collection = GetDecalCollection(m_model);

      for (int i = 0; i < collection.ItemCount(); i++)
      {
        const auto* item = collection.ItemAt(i);

        int d = 0;
        ON_Decal* decal = nullptr;
        while (nullptr != (decal = item->GetDecal(d++)))
        {
          m_decals.Append(decal);
        }
      }

      m_decal_index = 0;
      m_populated = true;
    }

    if (m_decal_index >= m_decals.Count())
      return nullptr;

    return m_decals[m_decal_index++];
  }

private:
  ON_SimpleArray<ON_Decal*> m_decals;
  bool m_populated = false;
};

ON_DecalIterator::ON_DecalIterator(ONX_Model& model, const ON_ModelComponent* component)
{
  if (nullptr != component)
  {
    m_impl = new CImpl_PerComponent(model, *component);
  }
  else
  {
    m_impl = new CImpl_All(model);
  }
}

ON_DecalIterator::~ON_DecalIterator()
{
  delete m_impl;
  m_impl = nullptr;
}

ON_Decal* ON_DecalIterator::Next(void) const
{
  return m_impl->Next();
}
