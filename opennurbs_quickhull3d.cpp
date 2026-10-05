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
#include "opennurbs_internal_defines.h" // undefs the Windows.h min/max macros

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

// Merge passes used by ON_QuickHull3DImpl::DoAdjacentMerge.
static const int QH_NONCONVEX_LARGER_FACE = 1;
static const int QH_NONCONVEX_EITHER_FACE = 2;

// ---------------------------------------------------------------------------
// Implementation-detail node types.
//
// These form the half-edge graph built during hull construction. They are
// defined only in this translation unit (never in a header) so they cannot be
// used outside the algorithm. The QH_ prefix keeps them from colliding with
// anything else in the opennurbs build.
// ---------------------------------------------------------------------------

class QH_Vertex;
class QH_HalfEdge;
class QH_Face;
class QH_VertexList;
class QH_FaceList;
class QH_NodePool;

// Represents vertices of the hull, as well as the input points from which it
// is formed. Owned by the node pool; all pointers are non-owning.
class QH_Vertex
{
public:
  QH_Vertex() = default;
  QH_Vertex(double x, double y, double z, int index);

public:
  ON_3dPoint m_point = ON_3dPoint::Origin;
  int m_index = 0;
  QH_Vertex* m_prev = nullptr;
  QH_Vertex* m_next = nullptr;
  QH_Face*   m_face = nullptr;
};

QH_Vertex::QH_Vertex(double x, double y, double z, int index)
  : m_point(x, y, z)
  , m_index(index)
{
}

// Half-edge surrounding a face in counter-clockwise direction. Owned by the
// node pool; all pointers are non-owning.
class QH_HalfEdge
{
public:
  QH_HalfEdge() = default;
  QH_HalfEdge(QH_Vertex* v, QH_Face* f);

  QH_HalfEdge* GetNext() const;
  void SetNext(QH_HalfEdge* edge);

  QH_HalfEdge* GetPrev() const;
  void SetPrev(QH_HalfEdge* edge);

  QH_Face* GetFace() const;
  QH_HalfEdge* GetOpposite() const;
  void SetOpposite(QH_HalfEdge* edge);

  QH_Vertex* Head() const;
  QH_Vertex* Tail() const;
  QH_Face* OppositeFace() const;

  ON_String GetVertexString() const;

  double Length() const;
  double LengthSquared() const;

public:
  QH_Vertex* m_vertex = nullptr;
  QH_Face* m_face = nullptr;
  QH_HalfEdge* m_next = nullptr;
  QH_HalfEdge* m_prev = nullptr;
  QH_HalfEdge* m_opposite = nullptr;
};

// Owns the QH_Face / QH_HalfEdge nodes for the duration of a hull build. All
// cross-references inside the graph are raw pointers; the pool keeps every
// node alive until the next build (or destruction), then frees them together.
class QH_NodePool
{
public:
  QH_NodePool() = default;

public:
  QH_Face* NewFace();
  QH_HalfEdge* NewHalfEdge();
  QH_HalfEdge* NewHalfEdge(QH_Vertex* v, QH_Face* f);
  void Clear();

private:
  std::vector<std::unique_ptr<QH_Face>> m_faces;
  std::vector<std::unique_ptr<QH_HalfEdge>> m_halfEdges;
};

// Basic triangular (or, after merging, polygonal) face used to form the hull.
class QH_Face
{
public:
  QH_Face() = default;

  enum Mark { VISIBLE = 1, NON_CONVEX = 2, DELETED = 3 };

  static QH_Face* CreateTriangle(
    QH_NodePool& pool, 
    QH_Vertex* v0, 
    QH_Vertex* v1, 
    QH_Vertex* v2,
    double minArea = 0.0
  );

  QH_HalfEdge* GetEdge(int i) const;
  QH_HalfEdge* GetFirstEdge() const;
  QH_HalfEdge* FindEdge(const QH_Vertex* vt, const QH_Vertex* vh) const;

  double DistanceToPlane(const ON_3dPoint& p) const;

  const ON_3dVector& GetNormal() const;
  const ON_3dPoint&  GetCentroid() const;

  int NumVertices() const;

  ON_String GetVertexString() const;
  void GetVertexIndices(int* idxs) const;

  void ComputeCentroid(ON_3dPoint& centroid) const;
  void ComputeNormal(ON_3dVector& normal);
  void ComputeNormal(ON_3dVector& normal, double minArea);

  void CheckConsistency() const;

  int MergeAdjacentFace(QH_HalfEdge* hedgeAdj, QH_Face** discarded);
  void Triangulate(QH_NodePool& pool, QH_FaceList& newFaces, double minArea);

public:
  QH_HalfEdge* m_he0 = nullptr;
  double m_area = 0.0;
  double m_planeOffset = 0.0;
  int m_index = 0;
  int m_numVerts = 0;
  QH_Face* m_next = nullptr;
  int m_mark = VISIBLE;
  QH_Vertex* m_outside = nullptr;

private:
  void ComputeNormalAndCentroid();
  void ComputeNormalAndCentroid(double minArea);
  QH_Face* ConnectHalfEdges(QH_HalfEdge* hedgePrev, QH_HalfEdge* hedge);
  double AreaSquared(const QH_HalfEdge* hedge0, const QH_HalfEdge* hedge1) const;

private:
  ON_3dVector m_normal = ON_3dVector::ZeroVector;
  ON_3dPoint m_centroid = ON_3dPoint::Origin;
};

// Doubly-linked intrusive vertex list. Does not own its vertices.
class QH_VertexList
{
public:
  QH_VertexList() = default;

public:
  void Clear();
  void Add(QH_Vertex* vtx);
  void AddAll(QH_Vertex* vtx);
  void Remove(const QH_Vertex* vtx);
  void Remove(const QH_Vertex* vtx1, const QH_Vertex* vtx2);
  void InsertBefore(QH_Vertex* vtx, QH_Vertex* next);

  QH_Vertex* First() const;
  bool IsEmpty() const;

private:
  QH_Vertex* m_head = nullptr;
  QH_Vertex* m_tail = nullptr;
};

// Single-linked face list. Does not own its faces.
class QH_FaceList
{
public:
  QH_FaceList() = default;

public:
  void Clear();
  void Add(QH_Face* f);
  QH_Face* First() const;
  bool IsEmpty() const;

private:
  QH_Face* m_head = nullptr;
  QH_Face* m_tail = nullptr;
};

// ---------------------------------------------------------------------------
// QH_HalfEdge
// ---------------------------------------------------------------------------

QH_HalfEdge::QH_HalfEdge(QH_Vertex* v, QH_Face* f)
  : m_vertex(v)
  , m_face(f)
{
}

void QH_HalfEdge::SetNext(QH_HalfEdge* edge)
{
  m_next = edge;
}

QH_HalfEdge* QH_HalfEdge::GetNext() const
{
  return m_next;
}

void QH_HalfEdge::SetPrev(QH_HalfEdge* edge)
{
  m_prev = edge;
}

QH_HalfEdge* QH_HalfEdge::GetPrev() const
{
  return m_prev;
}

QH_Face* QH_HalfEdge::GetFace() const
{
  return m_face;
}

QH_HalfEdge* QH_HalfEdge::GetOpposite() const
{
  return m_opposite;
}

void QH_HalfEdge::SetOpposite(QH_HalfEdge* edge)
{
  if (nullptr == edge) return;
  m_opposite = edge;
  edge->m_opposite = this;
}

QH_Vertex* QH_HalfEdge::Head() const
{
  return m_vertex;
}

QH_Vertex* QH_HalfEdge::Tail() const
{
  return m_prev != nullptr ? m_prev->m_vertex : nullptr;
}

QH_Face* QH_HalfEdge::OppositeFace() const
{
  return m_opposite != nullptr ? m_opposite->m_face : nullptr;
}

ON_String QH_HalfEdge::GetVertexString() const
{
  ON_String s;
  if (Tail() != nullptr)
    s.Format("%d-%d", Tail()->m_index, Head()->m_index);
  else
    s.Format("?-%d", Head()->m_index);
  return s;
}

double QH_HalfEdge::Length() const
{
  if (Tail() != nullptr) return Head()->m_point.DistanceTo(Tail()->m_point);
  return -1.0;
}

double QH_HalfEdge::LengthSquared() const
{
  if (Tail() != nullptr) return Head()->m_point.DistanceToSquared(Tail()->m_point);
  return -1.0;
}

// ---------------------------------------------------------------------------
// QH_NodePool
// ---------------------------------------------------------------------------

QH_Face* QH_NodePool::NewFace()
{
  m_faces.push_back(std::make_unique<QH_Face>());
  return m_faces.back().get();
}

QH_HalfEdge* QH_NodePool::NewHalfEdge()
{
  m_halfEdges.push_back(std::make_unique<QH_HalfEdge>());
  return m_halfEdges.back().get();
}

QH_HalfEdge* QH_NodePool::NewHalfEdge(QH_Vertex* v, QH_Face* f)
{
  m_halfEdges.push_back(std::make_unique<QH_HalfEdge>(v, f));
  return m_halfEdges.back().get();
}

void QH_NodePool::Clear()
{
  m_faces.clear();
  m_halfEdges.clear();
}

// ---------------------------------------------------------------------------
// QH_VertexList
// ---------------------------------------------------------------------------

void QH_VertexList::Clear()
{
  m_head = m_tail = nullptr;
}

void QH_VertexList::Add(QH_Vertex* vtx)
{
  if (nullptr == vtx) return;
  if (m_head == nullptr) m_head = vtx;
  else m_tail->m_next = vtx;
  vtx->m_prev = m_tail;
  vtx->m_next = nullptr;
  m_tail = vtx;
}

void QH_VertexList::AddAll(QH_Vertex* vtx)
{
  if (nullptr == vtx) return;
  if (m_head == nullptr) m_head = vtx;
  else m_tail->m_next = vtx;
  vtx->m_prev = m_tail;
  while (vtx->m_next != nullptr) vtx = vtx->m_next;
  m_tail = vtx;
}

void QH_VertexList::Remove(const QH_Vertex* vtx)
{
  if (nullptr == vtx) return;
  if (vtx->m_prev == nullptr) m_head = vtx->m_next;
  else vtx->m_prev->m_next = vtx->m_next;
  if (vtx->m_next == nullptr) m_tail = vtx->m_prev;
  else vtx->m_next->m_prev = vtx->m_prev;
}

void QH_VertexList::Remove(const QH_Vertex* vtx1, const QH_Vertex* vtx2)
{
  if (nullptr == vtx1 || nullptr == vtx2) return;
  if (vtx1->m_prev == nullptr) m_head = vtx2->m_next;
  else vtx1->m_prev->m_next = vtx2->m_next;
  if (vtx2->m_next == nullptr) m_tail = vtx1->m_prev;
  else vtx2->m_next->m_prev = vtx1->m_prev;
}

void QH_VertexList::InsertBefore(QH_Vertex* vtx, QH_Vertex* next)
{
  if (nullptr == vtx || nullptr == next) return;
  vtx->m_prev = next->m_prev;
  if (next->m_prev == nullptr) m_head = vtx;
  else next->m_prev->m_next = vtx;
  vtx->m_next = next;
  next->m_prev = vtx;
}

QH_Vertex* QH_VertexList::First() const
{
  return m_head;
}

bool QH_VertexList::IsEmpty() const
{
  return m_head == nullptr;
}

// ---------------------------------------------------------------------------
// QH_FaceList
// ---------------------------------------------------------------------------

void QH_FaceList::Clear()
{
  m_head = m_tail = nullptr;
}

void QH_FaceList::Add(QH_Face* f)
{
  if (nullptr == f) return;
  if (m_head == nullptr) m_head = f;
  else m_tail->m_next = f;
  f->m_next = nullptr;
  m_tail = f;
}

QH_Face* QH_FaceList::First() const
{
  return m_head;
}

bool QH_FaceList::IsEmpty() const
{
  return m_head == nullptr;
}

// ---------------------------------------------------------------------------
// QH_Face
// ---------------------------------------------------------------------------

QH_HalfEdge* QH_Face::GetFirstEdge() const
{
  return m_he0;
}

double QH_Face::DistanceToPlane(const ON_3dPoint& p) const
{
  return m_normal.x * p.x + m_normal.y * p.y + m_normal.z * p.z - m_planeOffset;
}

const ON_3dVector& QH_Face::GetNormal() const
{
  return m_normal;
}

const ON_3dPoint& QH_Face::GetCentroid() const
{
  return m_centroid;
}

int QH_Face::NumVertices() const
{
  return m_numVerts;
}

void QH_Face::ComputeCentroid(ON_3dPoint& centroid) const
{
  centroid = ON_3dPoint::Origin;
  QH_HalfEdge* he = m_he0;
  do
  {
    centroid = centroid + he->Head()->m_point;
    he = he->m_next;
  } while (he != m_he0);
  centroid *= (1.0 / static_cast<double>(m_numVerts));
}

void QH_Face::ComputeNormal(ON_3dVector& normal, double minArea)
{
  ComputeNormal(normal);

  if (m_area < minArea)
  {
    // Make the normal more robust by removing components parallel to the
    // longest edge.
    QH_HalfEdge* hedgeMax = nullptr;
    double lenSqrMax = 0.0;
    QH_HalfEdge* hedge = m_he0;
    do
    {
      double lenSqr = hedge->LengthSquared();
      if (lenSqr > lenSqrMax)
      {
        hedgeMax = hedge;
        lenSqrMax = lenSqr;
      }
      hedge = hedge->m_next;
    } while (hedge != m_he0);

    const ON_3dPoint& p2 = hedgeMax->Head()->m_point;
    const ON_3dPoint& p1 = hedgeMax->Tail()->m_point;
    double lenMax = std::sqrt(lenSqrMax);
    double ux = (p2.x - p1.x) / lenMax;
    double uy = (p2.y - p1.y) / lenMax;
    double uz = (p2.z - p1.z) / lenMax;
    double dot = normal.x * ux + normal.y * uy + normal.z * uz;
    normal.x -= dot * ux;
    normal.y -= dot * uy;
    normal.z -= dot * uz;

    normal.Unitize();
  }
}

void QH_Face::ComputeNormal(ON_3dVector& normal)
{
  QH_HalfEdge* he1 = m_he0->m_next;
  QH_HalfEdge* he2 = he1->m_next;

  const ON_3dPoint& p0 = m_he0->Head()->m_point;
  ON_3dPoint p2 = he1->Head()->m_point;

  double d2x = p2.x - p0.x;
  double d2y = p2.y - p0.y;
  double d2z = p2.z - p0.z;

  normal = ON_3dVector::ZeroVector;
  m_numVerts = 2;

  while (he2 != m_he0)
  {
    double d1x = d2x, d1y = d2y, d1z = d2z;

    p2 = he2->Head()->m_point;
    d2x = p2.x - p0.x;
    d2y = p2.y - p0.y;
    d2z = p2.z - p0.z;

    normal.x += d1y * d2z - d1z * d2y;
    normal.y += d1z * d2x - d1x * d2z;
    normal.z += d1x * d2y - d1y * d2x;

    he1 = he2;
    he2 = he2->m_next;
    m_numVerts++;
  }
  m_area = normal.Length();
  normal *= (1.0 / m_area);
}

void QH_Face::ComputeNormalAndCentroid()
{
  ComputeNormal(m_normal);
  ComputeCentroid(m_centroid);
  m_planeOffset = m_normal * ON_3dVector(m_centroid);
  int numv = 0;
  QH_HalfEdge* he = m_he0;
  do
  {
    numv++;
    he = he->m_next;
  } while (he != m_he0);
  if (numv != m_numVerts)
  {
    // Internal invariant; should never happen for valid input.
    ON_ERROR("ON_QuickHull3D - face vertex count mismatch.");
  }
}

void QH_Face::ComputeNormalAndCentroid(double minArea)
{
  ComputeNormal(m_normal, minArea);
  ComputeCentroid(m_centroid);
  m_planeOffset = m_normal * ON_3dVector(m_centroid);
}

QH_Face* QH_Face::CreateTriangle(QH_NodePool& pool, QH_Vertex* v0, QH_Vertex* v1, QH_Vertex* v2,
                                 double minArea)
{
  if (nullptr == v0 || nullptr == v1 || nullptr == v2) return nullptr;
  QH_Face* face = pool.NewFace();
  QH_HalfEdge* he0 = pool.NewHalfEdge(v0, face);
  QH_HalfEdge* he1 = pool.NewHalfEdge(v1, face);
  QH_HalfEdge* he2 = pool.NewHalfEdge(v2, face);

  he0->m_prev = he2; he0->m_next = he1;
  he1->m_prev = he0; he1->m_next = he2;
  he2->m_prev = he1; he2->m_next = he0;

  face->m_he0 = he0;
  face->ComputeNormalAndCentroid(minArea);
  return face;
}

QH_HalfEdge* QH_Face::GetEdge(int i) const
{
  QH_HalfEdge* he = m_he0;
  while (i > 0)
  {
    he = he->m_next;
    --i;
  }
  while (i < 0)
  {
    he = he->m_prev;
    ++i;
  }
  return he;
}

QH_HalfEdge* QH_Face::FindEdge(const QH_Vertex* vt, const QH_Vertex* vh) const
{
  QH_HalfEdge* he = m_he0;
  do
  {
    if (he->Head() == vh && he->Tail() == vt) return he;
    he = he->m_next;
  } while (he != m_he0);
  return nullptr;
}

ON_String QH_Face::GetVertexString() const
{
  ON_String s;
  QH_HalfEdge* he = m_he0;
  do
  {
    if (s.IsEmpty()) s = ON_String::FormatToString("%d", he->Head()->m_index);
    else             s += ON_String::FormatToString(" %d", he->Head()->m_index);
    he = he->m_next;
  } while (he != m_he0);
  return s;
}

void QH_Face::GetVertexIndices(int* idxs) const
{
  if (nullptr == idxs) return;
  QH_HalfEdge* he = m_he0;
  int i = 0;
  do
  {
    idxs[i++] = he->Head()->m_index;
    he = he->m_next;
  } while (he != m_he0);
}

QH_Face* QH_Face::ConnectHalfEdges(QH_HalfEdge* hedgePrev, QH_HalfEdge* hedge)
{
  if (nullptr == hedgePrev || nullptr == hedge) return nullptr;

  QH_Face* discardedFace = nullptr;

  if (hedgePrev->OppositeFace() == hedge->OppositeFace())
  {
    // Redundant edge: remove it.
    QH_Face* oppFace = hedge->OppositeFace();
    QH_HalfEdge* hedgeOpp;

    if (hedgePrev == m_he0) m_he0 = hedge;

    if (oppFace->NumVertices() == 3)
    {
      // Can get rid of the opposite face altogether.
      hedgeOpp = hedge->GetOpposite()->m_prev->GetOpposite();
      oppFace->m_mark = DELETED;
      discardedFace = oppFace;
    }
    else
    {
      hedgeOpp = hedge->GetOpposite()->m_next;
      if (oppFace->m_he0 == hedgeOpp->m_prev) oppFace->m_he0 = hedgeOpp;
      hedgeOpp->m_prev = hedgeOpp->m_prev->m_prev;
      hedgeOpp->m_prev->m_next = hedgeOpp;
    }
    hedge->m_prev = hedgePrev->m_prev;
    hedge->m_prev->m_next = hedge;

    hedge->m_opposite = hedgeOpp;
    hedgeOpp->m_opposite = hedge;

    // oppFace was modified, so need to recompute.
    oppFace->ComputeNormalAndCentroid();
  }
  else
  {
    hedgePrev->m_next = hedge;
    hedge->m_prev = hedgePrev;
  }
  return discardedFace;
}

void QH_Face::CheckConsistency() const
{
  QH_HalfEdge* hedge = m_he0;
  double maxd = 0.0;
  int numv = 0;

  if (m_numVerts < 3)
  {
    ON_ERROR("ON_QuickHull3D - degenerate face.");
    return;
  }
  do
  {
    QH_HalfEdge* hedgeOpp = hedge->GetOpposite();
    if (hedgeOpp == nullptr)
    {
      ON_ERROR("ON_QuickHull3D - unreflected half edge.");
      return;
    }
    else if (hedgeOpp->GetOpposite() != hedge)
    {
      ON_ERROR("ON_QuickHull3D - opposite half edge has wrong opposite.");
      return;
    }
    if (hedgeOpp->Head() != hedge->Tail() || hedge->Head() != hedgeOpp->Tail())
    {
      ON_ERROR("ON_QuickHull3D - half edge incorrectly reflected.");
      return;
    }
    QH_Face* oppFace = hedgeOpp->m_face;
    if (oppFace == nullptr)
    {
      ON_ERROR("ON_QuickHull3D - no face on half edge.");
      return;
    }
    else if (oppFace->m_mark == DELETED)
    {
      ON_ERROR("ON_QuickHull3D - opposite face not on hull.");
      return;
    }
    double d = std::abs(DistanceToPlane(hedge->Head()->m_point));
    if (d > maxd) maxd = d;
    numv++;
    hedge = hedge->m_next;
  } while (hedge != m_he0);

  if (numv != m_numVerts)
  {
    ON_ERROR("ON_QuickHull3D - face vertex count mismatch.");
  }
}

int QH_Face::MergeAdjacentFace(QH_HalfEdge* hedgeAdj, QH_Face** discarded)
{
  if (nullptr == hedgeAdj || nullptr == discarded) return 0;

  QH_Face* oppFace = hedgeAdj->OppositeFace();
  int numDiscarded = 0;

  discarded[numDiscarded++] = oppFace;
  oppFace->m_mark = DELETED;

  QH_HalfEdge* hedgeOpp = hedgeAdj->GetOpposite();

  QH_HalfEdge* hedgeAdjPrev = hedgeAdj->m_prev;
  QH_HalfEdge* hedgeAdjNext = hedgeAdj->m_next;
  QH_HalfEdge* hedgeOppPrev = hedgeOpp->m_prev;
  QH_HalfEdge* hedgeOppNext = hedgeOpp->m_next;

  while (hedgeAdjPrev->OppositeFace() == oppFace)
  {
    hedgeAdjPrev = hedgeAdjPrev->m_prev;
    hedgeOppNext = hedgeOppNext->m_next;
  }
  while (hedgeAdjNext->OppositeFace() == oppFace)
  {
    hedgeOppPrev = hedgeOppPrev->m_prev;
    hedgeAdjNext = hedgeAdjNext->m_next;
  }

  for (QH_HalfEdge* h = hedgeOppNext; h != hedgeOppPrev->m_next; h = h->m_next)
  {
    h->m_face = this;
  }

  if (hedgeAdj == m_he0) m_he0 = hedgeAdjNext;

  QH_Face* discardedFace;

  discardedFace = ConnectHalfEdges(hedgeOppPrev, hedgeAdjNext);
  if (discardedFace != nullptr) discarded[numDiscarded++] = discardedFace;

  discardedFace = ConnectHalfEdges(hedgeAdjPrev, hedgeOppNext);
  if (discardedFace != nullptr) discarded[numDiscarded++] = discardedFace;

  ComputeNormalAndCentroid();
  CheckConsistency();

  return numDiscarded;
}

double QH_Face::AreaSquared(const QH_HalfEdge* hedge0, const QH_HalfEdge* hedge1) const
{
  if (nullptr == hedge0 || nullptr == hedge1) return 0.0;

  const ON_3dPoint& p0 = hedge0->Tail()->m_point;
  const ON_3dPoint& p1 = hedge0->Head()->m_point;
  const ON_3dPoint& p2 = hedge1->Head()->m_point;

  double dx1 = p1.x - p0.x, dy1 = p1.y - p0.y, dz1 = p1.z - p0.z;
  double dx2 = p2.x - p0.x, dy2 = p2.y - p0.y, dz2 = p2.z - p0.z;

  double x = dy1 * dz2 - dz1 * dy2;
  double y = dz1 * dx2 - dx1 * dz2;
  double z = dx1 * dy2 - dy1 * dx2;

  return x * x + y * y + z * z;
}

void QH_Face::Triangulate(QH_NodePool& pool, QH_FaceList& newFaces, double minArea)
{
  if (NumVertices() < 4) return;

  QH_Vertex* v0 = m_he0->Head();

  QH_HalfEdge* hedge = m_he0->m_next;
  QH_HalfEdge* oppPrev = hedge->m_opposite;
  QH_Face* face0 = nullptr;

  for (hedge = hedge->m_next; hedge != m_he0->m_prev; hedge = hedge->m_next)
  {
    QH_Face* face = CreateTriangle(pool, v0, hedge->m_prev->Head(), hedge->Head(), minArea);
    face->m_he0->m_next->SetOpposite(oppPrev);
    face->m_he0->m_prev->SetOpposite(hedge->m_opposite);
    oppPrev = face->m_he0;
    newFaces.Add(face);
    if (face0 == nullptr) face0 = face;
  }
  hedge = pool.NewHalfEdge(m_he0->m_prev->m_prev->Head(), this);
  hedge->SetOpposite(oppPrev);

  hedge->m_prev = m_he0;
  hedge->m_prev->m_next = hedge;

  hedge->m_next = m_he0->m_prev;
  hedge->m_next->m_prev = hedge;

  ComputeNormalAndCentroid(minArea);
  CheckConsistency();

  for (QH_Face* f = face0; f != nullptr; f = f->m_next) f->CheckConsistency();
}

// ---------------------------------------------------------------------------
// ON_QuickHull3DImpl - the hull builder state and algorithm.
// ---------------------------------------------------------------------------

class ON_QuickHull3DImpl
{
public:
  ON_QuickHull3DImpl() = default;

  bool Build(const ON_3dPoint* points, int nump);
  bool Build(const double* coords, int nump);
  bool BuildFromPointCloud(const ON_PointCloud& pc);

  bool HasVertexColors() const;
  void Triangulate();

  int GetNumVertices() const;
  int GetNumFaces() const;

  int GetVertices(ON_SimpleArray<ON_3dPoint>& vertices) const;
  int GetVertices(double* coords) const;
  int GetVertexPointIndices(ON_SimpleArray<int>& indices) const;
  int GetFaces(ON_ClassArray<ON_SimpleArray<int>>& faces, ON_QuickHull3D::FaceIndexFlags indexFlags) const;

  bool ToMesh(ON_Mesh& mesh) const;

  double GetDistanceTolerance() const;
  void   SetExplicitDistanceTolerance(double tol);
  double GetExplicitDistanceTolerance() const;

  bool IsValid(ON_TextLog* text_log, double tol) const;

private:
  double m_charLength = 0.0;

  // unique_ptr ownership of the input vertices.
  std::vector<std::unique_ptr<QH_Vertex>> m_pointBuffer;

  std::vector<int> m_vertexPointIndices;
  ON_SimpleArray<ON_Color> m_inputColors; // mirrors ON_PointCloud::m_C
  QH_Face* m_discardedFaces[3] = { nullptr, nullptr, nullptr };

  QH_Vertex* m_maxVtxs[3] = { nullptr, nullptr, nullptr };
  QH_Vertex* m_minVtxs[3] = { nullptr, nullptr, nullptr };

  std::vector<QH_Face*> m_faces;
  std::vector<QH_HalfEdge*> m_horizon;

  QH_FaceList m_newFaces;
  QH_VertexList m_unclaimed;
  QH_VertexList m_claimed;

  int m_numVertices = 0;
  int m_numFaces = 0;
  int m_numPoints = 0;

  double m_explicitTolerance = ON_UNSET_VALUE;
  double m_tolerance = 0.0;

  QH_NodePool m_pool;

  void AddPointToFace(QH_Vertex* vtx, QH_Face* face);
  void RemovePointFromFace(const QH_Vertex* vtx, QH_Face* face);
  QH_Vertex* RemoveAllPointsFromFace(const QH_Face* face);

  void InitBuffers(int nump);
  void SetPoints(const double* coords, int nump);
  void SetPoints(const ON_3dPoint* pnts, int nump);
  void ComputeMaxAndMin();
  bool CreateInitialSimplex();

  bool BuildHull();
  void ReindexFacesAndVertices();
  void MarkFaceVertices(QH_Face* face, int mark);

  void ResolveUnclaimedPoints(QH_FaceList& newFaces);
  void DeleteFacePoints(QH_Face* face, QH_Face* absorbingFace);

  double OppFaceDistance(const QH_HalfEdge* he) const;
  bool DoAdjacentMerge(QH_Face* face, int mergeType);
  void CalculateHorizon(const ON_3dPoint& eyePnt, QH_HalfEdge* edge0, QH_Face* face, std::vector<QH_HalfEdge*>& horizon);
  QH_HalfEdge* AddAdjoiningFace(QH_Vertex* eyeVtx, QH_HalfEdge* he);
  void AddNewFaces(QH_FaceList& newFaces, QH_Vertex* eyeVtx, const std::vector<QH_HalfEdge*>& horizon);
  QH_Vertex* NextPointToAdd();
  void AddPointToHull(QH_Vertex* eyeVtx);

  void GetFaceIndices(int* indices, QH_Face* face, ON_QuickHull3D::FaceIndexFlags flags) const;

  bool CheckFaceConvexity(QH_Face* face, double tol, ON_TextLog* text_log) const;
  bool CheckFaces(double tol, ON_TextLog* text_log) const;
};

void ON_QuickHull3DImpl::AddPointToFace(QH_Vertex* vtx, QH_Face* face)
{
  if (nullptr == vtx || nullptr == face) return;
  vtx->m_face = face;
  if (face->m_outside == nullptr) m_claimed.Add(vtx);
  else m_claimed.InsertBefore(vtx, face->m_outside);
  face->m_outside = vtx;
}

void ON_QuickHull3DImpl::RemovePointFromFace(const QH_Vertex* vtx, QH_Face* face)
{
  if (nullptr == vtx || nullptr == face) return;
  if (vtx == face->m_outside)
  {
    if (vtx->m_next != nullptr && vtx->m_next->m_face == face) face->m_outside = vtx->m_next;
    else face->m_outside = nullptr;
  }
  m_claimed.Remove(vtx);
}

QH_Vertex* ON_QuickHull3DImpl::RemoveAllPointsFromFace(const QH_Face* face)
{
  if (nullptr == face) return nullptr;
  if (face->m_outside != nullptr)
  {
    QH_Vertex* end = face->m_outside;
    while (end->m_next != nullptr && end->m_next->m_face == face) end = end->m_next;
    m_claimed.Remove(face->m_outside, end);
    end->m_next = nullptr;
    return face->m_outside;
  }
  return nullptr;
}

void ON_QuickHull3DImpl::InitBuffers(int nump)
{
  if (static_cast<int>(m_pointBuffer.size()) < nump)
  {
    m_pointBuffer.reserve(nump);
    while (static_cast<int>(m_pointBuffer.size()) < nump)
      m_pointBuffer.push_back(std::make_unique<QH_Vertex>());
  }
  m_vertexPointIndices.assign(nump, 0);
  m_inputColors.SetCount(0); // cleared on every build; repopulated on success
  m_pool.Clear();
  m_faces.clear();
  m_claimed.Clear();
  m_numFaces = 0;
  m_numPoints = nump;
}

void ON_QuickHull3DImpl::SetPoints(const double* coords, int nump)
{
  if (nullptr == coords) return;
  for (int i = 0; i < nump; i++)
  {
    QH_Vertex* vtx = m_pointBuffer[i].get();
    vtx->m_point.Set(coords[i * 3 + 0], coords[i * 3 + 1], coords[i * 3 + 2]);
    vtx->m_index = i;
  }
}

void ON_QuickHull3DImpl::SetPoints(const ON_3dPoint* pnts, int nump)
{
  if (nullptr == pnts) return;
  for (int i = 0; i < nump; i++)
  {
    QH_Vertex* vtx = m_pointBuffer[i].get();
    vtx->m_point = pnts[i];
    vtx->m_index = i;
  }
}

void ON_QuickHull3DImpl::ComputeMaxAndMin()
{
  ON_3dPoint max = ON_3dPoint::Origin;
  ON_3dPoint min = ON_3dPoint::Origin;

  for (int i = 0; i < 3; i++)
  {
    m_maxVtxs[i] = m_minVtxs[i] = m_pointBuffer[0].get();
  }
  max = m_pointBuffer[0]->m_point;
  min = m_pointBuffer[0]->m_point;

  for (int i = 1; i < m_numPoints; i++)
  {
    const ON_3dPoint& pnt = m_pointBuffer[i]->m_point;
    if (pnt.x > max.x)
    {
      max.x = pnt.x;
      m_maxVtxs[0] = m_pointBuffer[i].get();
    }
    else if (pnt.x < min.x)
    {
      min.x = pnt.x;
      m_minVtxs[0] = m_pointBuffer[i].get();
    }
    if (pnt.y > max.y)
    {
      max.y = pnt.y;
      m_maxVtxs[1] = m_pointBuffer[i].get();
    }
    else if (pnt.y < min.y)
    {
      min.y = pnt.y;
      m_minVtxs[1] = m_pointBuffer[i].get();
    }
    if (pnt.z > max.z)
    {
      max.z = pnt.z;
      m_maxVtxs[2] = m_pointBuffer[i].get();
    }
    else if (pnt.z < min.z)
    {
      min.z = pnt.z;
      m_minVtxs[2] = m_pointBuffer[i].get();
    }
  }

  m_charLength = std::max(max.x - min.x, max.y - min.y);
  m_charLength = std::max(max.z - min.z, m_charLength);
  if (m_explicitTolerance == ON_UNSET_VALUE)
  {
    m_tolerance =
      3.0 * ON_EPSILON *
      (std::max(std::abs(max.x), std::abs(min.x)) +
       std::max(std::abs(max.y), std::abs(min.y)) +
       std::max(std::abs(max.z), std::abs(min.z)));
  }
  else
  {
    m_tolerance = m_explicitTolerance;
  }
}

bool ON_QuickHull3DImpl::CreateInitialSimplex()
{
  double max = 0.0;
  int imax = 0;
  for (int i = 0; i < 3; i++)
  {
    double diff = m_maxVtxs[i]->m_point[i] - m_minVtxs[i]->m_point[i];
    if (diff > max)
    {
      max = diff;
      imax = i;
    }
  }

  if (max <= m_tolerance)
  {
    ON_ERROR("ON_QuickHull3D::Build - input points appear to be coincident.");
    return false;
  }

  QH_Vertex* vtx[4] = { nullptr, nullptr, nullptr, nullptr };
  vtx[0] = m_maxVtxs[imax];
  vtx[1] = m_minVtxs[imax];

  ON_3dVector nrml = ON_3dVector::ZeroVector;
  double maxSqr = 0.0;
  ON_3dVector u01 = vtx[1]->m_point - vtx[0]->m_point;
  u01.Unitize();
  for (int i = 0; i < m_numPoints; i++)
  {
    ON_3dVector diff02 = m_pointBuffer[i]->m_point - vtx[0]->m_point;
    ON_3dVector xprod = ON_CrossProduct(u01, diff02);
    double lenSqr = xprod.LengthSquared();
    if (lenSqr > maxSqr &&
        m_pointBuffer[i].get() != vtx[0] &&
        m_pointBuffer[i].get() != vtx[1])
    {
      maxSqr = lenSqr;
      vtx[2] = m_pointBuffer[i].get();
      nrml = xprod;
    }
  }
  if (std::sqrt(maxSqr) <= 100 * m_tolerance)
  {
    ON_ERROR("ON_QuickHull3D::Build - input points appear to be colinear.");
    return false;
  }
  nrml.Unitize();

  // Recompute nrml to make sure it is normal to u01.
  ON_3dVector res = (nrml * u01) * u01;
  nrml -= res;
  nrml.Unitize();

  double maxDist = 0.0;
  double d0 = vtx[2]->m_point * nrml;
  for (int i = 0; i < m_numPoints; i++)
  {
    double dist = std::abs(m_pointBuffer[i]->m_point * nrml - d0);
    if (dist > maxDist &&
        m_pointBuffer[i].get() != vtx[0] &&
        m_pointBuffer[i].get() != vtx[1] &&
        m_pointBuffer[i].get() != vtx[2])
    {
      maxDist = dist;
      vtx[3] = m_pointBuffer[i].get();
    }
  }
  if (std::abs(maxDist) <= 100 * m_tolerance)
  {
    ON_ERROR("ON_QuickHull3D::Build - input points appear to be coplanar.");
    return false;
  }

  QH_Face* tris[4];
  if (vtx[3]->m_point * nrml - d0 < 0)
  {
    tris[0] = QH_Face::CreateTriangle(m_pool, vtx[0], vtx[1], vtx[2]);
    tris[1] = QH_Face::CreateTriangle(m_pool, vtx[3], vtx[1], vtx[0]);
    tris[2] = QH_Face::CreateTriangle(m_pool, vtx[3], vtx[2], vtx[1]);
    tris[3] = QH_Face::CreateTriangle(m_pool, vtx[3], vtx[0], vtx[2]);

    for (int i = 0; i < 3; i++)
    {
      int k = (i + 1) % 3;
      tris[i + 1]->GetEdge(1)->SetOpposite(tris[k + 1]->GetEdge(0));
      tris[i + 1]->GetEdge(2)->SetOpposite(tris[0]->GetEdge(k));
    }
  }
  else
  {
    tris[0] = QH_Face::CreateTriangle(m_pool, vtx[0], vtx[2], vtx[1]);
    tris[1] = QH_Face::CreateTriangle(m_pool, vtx[3], vtx[0], vtx[1]);
    tris[2] = QH_Face::CreateTriangle(m_pool, vtx[3], vtx[1], vtx[2]);
    tris[3] = QH_Face::CreateTriangle(m_pool, vtx[3], vtx[2], vtx[0]);

    for (int i = 0; i < 3; i++)
    {
      int k = (i + 1) % 3;
      tris[i + 1]->GetEdge(0)->SetOpposite(tris[k + 1]->GetEdge(1));
      tris[i + 1]->GetEdge(2)->SetOpposite(tris[0]->GetEdge((3 - i) % 3));
    }
  }

  for (int i = 0; i < 4; i++) m_faces.push_back(tris[i]);

  for (int i = 0; i < m_numPoints; i++)
  {
    QH_Vertex* v = m_pointBuffer[i].get();
    if (v == vtx[0] || v == vtx[1] || v == vtx[2] || v == vtx[3]) continue;

    maxDist = m_tolerance;
    QH_Face* maxFace = nullptr;
    for (int k = 0; k < 4; k++)
    {
      double dist = tris[k]->DistanceToPlane(v->m_point);
      if (dist > maxDist)
      {
        maxFace = tris[k];
        maxDist = dist;
      }
    }
    if (maxFace != nullptr) AddPointToFace(v, maxFace);
  }
  return true;
}

bool ON_QuickHull3DImpl::Build(const ON_3dPoint* points, int nump)
{
  if (nullptr == points)
  {
    ON_ERROR("ON_QuickHull3D::Build - points is nullptr.");
    return false;
  }
  if (nump < 4)
  {
    ON_ERROR("ON_QuickHull3D::Build - fewer than four input points specified.");
    return false;
  }
  InitBuffers(nump);
  SetPoints(points, nump);
  return BuildHull();
}

bool ON_QuickHull3DImpl::Build(const double* coords, int nump)
{
  if (nullptr == coords)
  {
    ON_ERROR("ON_QuickHull3D::Build - coords is nullptr.");
    return false;
  }
  if (nump < 4)
  {
    ON_ERROR("ON_QuickHull3D::Build - fewer than four input points specified.");
    return false;
  }
  InitBuffers(nump);
  SetPoints(coords, nump);
  return BuildHull();
}

bool ON_QuickHull3DImpl::BuildFromPointCloud(const ON_PointCloud& pc)
{
  if (!Build(pc.m_P.Array(), pc.m_P.Count()))
    return false;
  if (pc.HasPointColors() && pc.m_C.Count() == pc.m_P.Count())
    m_inputColors = pc.m_C;
  return true;
}

bool ON_QuickHull3DImpl::HasVertexColors() const
{
  return m_inputColors.Count() > 0 && m_inputColors.Count() == m_numPoints;
}

void ON_QuickHull3DImpl::Triangulate()
{
  double minArea = 1000.0 * m_charLength * ON_EPSILON;
  m_newFaces.Clear();
  for (QH_Face* face : m_faces)
  {
    if (face->m_mark == QH_Face::VISIBLE) face->Triangulate(m_pool, m_newFaces, minArea);
  }
  for (QH_Face* face = m_newFaces.First(); face != nullptr; face = face->m_next)
  {
    m_faces.push_back(face);
  }
}

double ON_QuickHull3DImpl::OppFaceDistance(const QH_HalfEdge* he) const
{
  if (nullptr == he) return 0.0;
  return he->m_face->DistanceToPlane(he->m_opposite->m_face->GetCentroid());
}

bool ON_QuickHull3DImpl::DoAdjacentMerge(QH_Face* face, int mergeType)
{
  if (nullptr == face) return false;
  QH_HalfEdge* hedge = face->m_he0;
  bool convex = true;
  do
  {
    QH_Face* oppFace = hedge->OppositeFace();
    bool merge = false;

    if (mergeType == QH_NONCONVEX_EITHER_FACE)
    {
      if (OppFaceDistance(hedge) > -m_tolerance ||
          OppFaceDistance(hedge->m_opposite) > -m_tolerance)
      {
        merge = true;
      }
    }
    else // QH_NONCONVEX_WRT_LARGER_FACE
    {
      if (face->m_area > oppFace->m_area)
      {
        if (OppFaceDistance(hedge) > -m_tolerance) merge = true;
        else if (OppFaceDistance(hedge->m_opposite) > -m_tolerance) convex = false;
      }
      else
      {
        if (OppFaceDistance(hedge->m_opposite) > -m_tolerance) merge = true;
        else if (OppFaceDistance(hedge) > -m_tolerance) convex = false;
      }
    }

    if (merge)
    {
      int numd = face->MergeAdjacentFace(hedge, m_discardedFaces);
      for (int i = 0; i < numd; i++) DeleteFacePoints(m_discardedFaces[i], face);
      return true;
    }
    hedge = hedge->m_next;
  } while (hedge != face->m_he0);

  if (!convex) face->m_mark = QH_Face::NON_CONVEX;
  return false;
}

void ON_QuickHull3DImpl::CalculateHorizon(const ON_3dPoint& eyePnt, QH_HalfEdge* edge0,
                                        QH_Face* face, std::vector<QH_HalfEdge*>& horizon)
{
  if (nullptr == face) return; // edge0 may be nullptr; handled below
  DeleteFacePoints(face, nullptr);
  face->m_mark = QH_Face::DELETED;

  QH_HalfEdge* edge;
  if (edge0 == nullptr)
  {
    edge0 = face->GetEdge(0);
    edge = edge0;
  }
  else
  {
    edge = edge0->GetNext();
  }

  do
  {
    QH_Face* oppFace = edge->OppositeFace();
    if (oppFace->m_mark == QH_Face::VISIBLE)
    {
      if (oppFace->DistanceToPlane(eyePnt) > m_tolerance)
      {
        CalculateHorizon(eyePnt, edge->GetOpposite(), oppFace, horizon);
      }
      else
      {
        horizon.push_back(edge);
      }
    }
    edge = edge->GetNext();
  } while (edge != edge0);
}

QH_HalfEdge* ON_QuickHull3DImpl::AddAdjoiningFace(QH_Vertex* eyeVtx, QH_HalfEdge* he)
{
  if (nullptr == eyeVtx || nullptr == he) return nullptr;
  QH_Face* face = QH_Face::CreateTriangle(m_pool, eyeVtx, he->Tail(), he->Head());
  m_faces.push_back(face);
  face->GetEdge(-1)->SetOpposite(he->GetOpposite());
  return face->GetEdge(0);
}

void ON_QuickHull3DImpl::AddNewFaces(QH_FaceList& newFaces, QH_Vertex* eyeVtx,
                                   const std::vector<QH_HalfEdge*>& horizon)
{
  if (nullptr == eyeVtx) return;
  newFaces.Clear();

  QH_HalfEdge* hedgeSidePrev = nullptr;
  QH_HalfEdge* hedgeSideBegin = nullptr;

  for (QH_HalfEdge* horizonHe : horizon)
  {
    QH_HalfEdge* hedgeSide = AddAdjoiningFace(eyeVtx, horizonHe);
    if (hedgeSidePrev != nullptr) hedgeSide->m_next->SetOpposite(hedgeSidePrev);
    else hedgeSideBegin = hedgeSide;
    newFaces.Add(hedgeSide->GetFace());
    hedgeSidePrev = hedgeSide;
  }
  hedgeSideBegin->m_next->SetOpposite(hedgeSidePrev);
}

QH_Vertex* ON_QuickHull3DImpl::NextPointToAdd()
{
  if (!m_claimed.IsEmpty())
  {
    QH_Face* eyeFace = m_claimed.First()->m_face;
    QH_Vertex* eyeVtx = nullptr;
    double  maxDist = 0.0;
    for (QH_Vertex* vtx = eyeFace->m_outside;
         vtx != nullptr && vtx->m_face == eyeFace;
         vtx = vtx->m_next)
    {
      double dist = eyeFace->DistanceToPlane(vtx->m_point);
      if (dist > maxDist)
      {
        maxDist = dist;
        eyeVtx = vtx;
      }
    }
    return eyeVtx;
  }
  return nullptr;
}

void ON_QuickHull3DImpl::AddPointToHull(QH_Vertex* eyeVtx)
{
  if (nullptr == eyeVtx) return;
  m_horizon.clear();
  m_unclaimed.Clear();

  RemovePointFromFace(eyeVtx, eyeVtx->m_face);
  CalculateHorizon(eyeVtx->m_point, nullptr, eyeVtx->m_face, m_horizon);
  m_newFaces.Clear();
  AddNewFaces(m_newFaces, eyeVtx, m_horizon);

  // First merge pass: merge faces which are non-convex as determined by the
  // larger face.
  for (QH_Face* face = m_newFaces.First(); face != nullptr; face = face->m_next)
  {
    if (face->m_mark == QH_Face::VISIBLE)
    {
      while (DoAdjacentMerge(face, QH_NONCONVEX_LARGER_FACE)) {}
    }
  }
  // Second merge pass: merge faces which are non-convex wrt either face.
  for (QH_Face* face = m_newFaces.First(); face != nullptr; face = face->m_next)
  {
    if (face->m_mark == QH_Face::NON_CONVEX)
    {
      face->m_mark = QH_Face::VISIBLE;
      while (DoAdjacentMerge(face, QH_NONCONVEX_EITHER_FACE)) {}
    }
  }
  ResolveUnclaimedPoints(m_newFaces);
}

bool ON_QuickHull3DImpl::BuildHull()
{
  ComputeMaxAndMin();
  if (!CreateInitialSimplex())
    return false;
  QH_Vertex* eyeVtx;
  while ((eyeVtx = NextPointToAdd()) != nullptr)
  {
    AddPointToHull(eyeVtx);
  }
  ReindexFacesAndVertices();
  return true;
}

void ON_QuickHull3DImpl::ResolveUnclaimedPoints(QH_FaceList& newFaces)
{
  QH_Vertex* vtxNext = m_unclaimed.First();
  for (QH_Vertex* vtx = vtxNext; vtx != nullptr; vtx = vtxNext)
  {
    vtxNext = vtx->m_next;

    double maxDist = m_tolerance;
    QH_Face* maxFace = nullptr;
    for (QH_Face* newFace = newFaces.First(); newFace != nullptr; newFace = newFace->m_next)
    {
      if (newFace->m_mark == QH_Face::VISIBLE)
      {
        double dist = newFace->DistanceToPlane(vtx->m_point);
        if (dist > maxDist)
        {
          maxDist = dist;
          maxFace = newFace;
        }
        if (maxDist > 1000 * m_tolerance) break;
      }
    }
    if (maxFace != nullptr) AddPointToFace(vtx, maxFace);
  }
}

void ON_QuickHull3DImpl::DeleteFacePoints(QH_Face* face, QH_Face* absorbingFace)
{
  if (nullptr == face) return; // absorbingFace may be nullptr; handled below
  QH_Vertex* faceVtxs = RemoveAllPointsFromFace(face);
  if (faceVtxs == nullptr) return;

  if (absorbingFace == nullptr)
  {
    m_unclaimed.AddAll(faceVtxs);
    return;
  }
  QH_Vertex* vtxNext = faceVtxs;
  for (QH_Vertex* vtx = vtxNext; vtx != nullptr; vtx = vtxNext)
  {
    vtxNext = vtx->m_next;
    double dist = absorbingFace->DistanceToPlane(vtx->m_point);
    if (dist > m_tolerance) AddPointToFace(vtx, absorbingFace);
    else                    m_unclaimed.Add(vtx);
  }
}

void ON_QuickHull3DImpl::MarkFaceVertices(QH_Face* face, int mark)
{
  if (nullptr == face) return;
  QH_HalfEdge* he0 = face->GetFirstEdge();
  QH_HalfEdge* he = he0;
  do
  {
    he->Head()->m_index = mark;
    he = he->m_next;
  } while (he != he0);
}

void ON_QuickHull3DImpl::ReindexFacesAndVertices()
{
  for (int i = 0; i < m_numPoints; i++) m_pointBuffer[i]->m_index = -1;

  m_numFaces = 0;
  std::vector<QH_Face*> kept;
  kept.reserve(m_faces.size());
  for (QH_Face* face : m_faces)
  {
    if (face->m_mark != QH_Face::VISIBLE) continue;
    MarkFaceVertices(face, 0);
    ++m_numFaces;
    kept.push_back(face);
  }
  m_faces = std::move(kept);

  m_numVertices = 0;
  for (int i = 0; i < m_numPoints; i++)
  {
    QH_Vertex* vtx = m_pointBuffer[i].get();
    if (vtx->m_index == 0)
    {
      m_vertexPointIndices[m_numVertices] = i;
      vtx->m_index = m_numVertices++;
    }
  }
}

int ON_QuickHull3DImpl::GetNumVertices() const
{
  return m_numVertices;
}

int ON_QuickHull3DImpl::GetNumFaces() const
{
  return static_cast<int>(m_faces.size());
}

int ON_QuickHull3DImpl::GetVertices(ON_SimpleArray<ON_3dPoint>& vertices) const
{
  vertices.Empty();
  vertices.Reserve(m_numVertices);
  for (int i = 0; i < m_numVertices; i++)
  {
    vertices.Append(m_pointBuffer[m_vertexPointIndices[i]]->m_point);
  }
  return m_numVertices;
}

int ON_QuickHull3DImpl::GetVertices(double* coords) const
{
  if (nullptr == coords) return 0;
  for (int i = 0; i < m_numVertices; i++)
  {
    const ON_3dPoint& pnt = m_pointBuffer[m_vertexPointIndices[i]]->m_point;
    coords[i * 3 + 0] = pnt.x;
    coords[i * 3 + 1] = pnt.y;
    coords[i * 3 + 2] = pnt.z;
  }
  return m_numVertices;
}

int ON_QuickHull3DImpl::GetVertexPointIndices(ON_SimpleArray<int>& indices) const
{
  indices.Empty();
  indices.Reserve(m_numVertices);
  for (int i = 0; i < m_numVertices; i++) indices.Append(m_vertexPointIndices[i]);
  return m_numVertices;
}

int ON_QuickHull3DImpl::GetFaces(ON_ClassArray<ON_SimpleArray<int>>& faces, ON_QuickHull3D::FaceIndexFlags indexFlags) const
{
  faces.Empty();
  faces.Reserve(m_faces.size());
  for (QH_Face* face : m_faces)
  {
    const int count = face->NumVertices();
    ON_SimpleArray<int>& row = faces.AppendNew();
    row.Reserve(count);
    row.SetCount(count);
    GetFaceIndices(row.Array(), face, indexFlags);
  }
  return faces.Count();
}

bool ON_QuickHull3DImpl::ToMesh(ON_Mesh& mesh) const
{
  if (m_numVertices < 4) return false;
  mesh.Destroy();

  ON_SimpleArray<ON_3dPoint> vertices;
  GetVertices(vertices);
  mesh.m_V.SetCapacity(m_numVertices);
  for (int i = 0; i < m_numVertices; i++)
  {
    mesh.SetVertex(i, vertices[i]);
  }

  // If a Build captured per-point colors, emit the subset for hull vertices.
  if (HasVertexColors())
  {
    mesh.m_C.SetCapacity(m_numVertices);
    mesh.m_C.SetCount(m_numVertices);
    for (int i = 0; i < m_numVertices; i++)
    {
      mesh.m_C[i] = m_inputColors[m_vertexPointIndices[i]];
    }
  }

  ON_ClassArray<ON_SimpleArray<int>> faceIndices;
  GetFaces(faceIndices, ON_QuickHull3D::FaceIndexFlags::None);
  int faceUpperBound = 0;
  for (const ON_SimpleArray<int>& f : faceIndices)
  {
    const int n = f.Count();
    faceUpperBound += (n <= 4) ? 1 : (n - 2);
  }
  mesh.m_F.SetCapacity(faceUpperBound);

  for (const ON_SimpleArray<int>& f : faceIndices)
  {
    const int n = f.Count();
    if (n < 3) continue;
    if (n == 3)
    {
      ON_MeshFace& face = mesh.m_F.AppendNew();
      face.vi[0] = f[0]; face.vi[1] = f[1]; face.vi[2] = f[2]; face.vi[3] = f[2];
    }
    else if (n == 4)
    {
      ON_MeshFace& face = mesh.m_F.AppendNew();
      face.vi[0] = f[0]; face.vi[1] = f[1]; face.vi[2] = f[2]; face.vi[3] = f[3];
    }
    else
    {
      // Planar n-gon (n >= 5): fan-triangulate from vertex 0, then register the
      // tessellation triangles as a single ON_Mesh n-gon so the original merged
      // hull face is preserved.
      std::vector<unsigned int> ngonFaces;
      ngonFaces.reserve(n - 2);
      for (int i = 1; i + 1 < n; i++)
      {
        ON_MeshFace& face = mesh.m_F.AppendNew();
        face.vi[0] = f[0];
        face.vi[1] = f[i];
        face.vi[2] = f[i + 1];
        face.vi[3] = f[i + 1];
        ngonFaces.push_back(static_cast<unsigned int>(mesh.m_F.Count() - 1));
      }
      mesh.AddNgon(static_cast<unsigned int>(ngonFaces.size()), ngonFaces.data());
    }
  }

  mesh.ComputeFaceNormals();
  mesh.ComputeVertexNormals();
  return true;
}

void ON_QuickHull3DImpl::GetFaceIndices(int* indices, QH_Face* face, ON_QuickHull3D::FaceIndexFlags flags) const
{
  if (nullptr == indices || nullptr == face) return;
  bool ccw = ((flags & ON_QuickHull3D::FaceIndexFlags::Clockwise) == ON_QuickHull3D::FaceIndexFlags::None);
  bool indexedFromOne = ((flags & ON_QuickHull3D::FaceIndexFlags::IndexedFromOne) != ON_QuickHull3D::FaceIndexFlags::None);
  bool pointRelative = ((flags & ON_QuickHull3D::FaceIndexFlags::PointRelative) != ON_QuickHull3D::FaceIndexFlags::None);

  QH_HalfEdge* hedge = face->m_he0;
  int k = 0;
  do
  {
    int idx = hedge->Head()->m_index;
    if (pointRelative)  idx = m_vertexPointIndices[idx];
    if (indexedFromOne) ++idx;
    indices[k++] = idx;
    hedge = ccw ? hedge->m_next : hedge->m_prev;
  } while (hedge != face->m_he0);
}

double ON_QuickHull3DImpl::GetDistanceTolerance() const
{
  return m_tolerance;
}

void ON_QuickHull3DImpl::SetExplicitDistanceTolerance(double tol)
{
  // Anything but a valid positive number means "compute it from the points".
  m_explicitTolerance = (ON_IsValid(tol) && tol > 0.0) ? tol : ON_UNSET_VALUE;
}

double ON_QuickHull3DImpl::GetExplicitDistanceTolerance() const
{
  return m_explicitTolerance;
}

bool ON_QuickHull3DImpl::CheckFaceConvexity(QH_Face* face, double tol, ON_TextLog* text_log) const
{
  if (nullptr == face) return false; // text_log may be nullptr by design
  QH_HalfEdge* he = face->m_he0;
  do
  {
    face->CheckConsistency();

    double dist = OppFaceDistance(he);
    if (dist > tol)
    {
      if (text_log) text_log->Print("Edge %s non-convex by %.17g\n",
                                     static_cast<const char*>(he->GetVertexString()), dist);
      return false;
    }
    dist = OppFaceDistance(he->m_opposite);
    if (dist > tol)
    {
      if (text_log) text_log->Print("Opposite edge %s non-convex by %.17g\n",
                                     static_cast<const char*>(he->m_opposite->GetVertexString()), dist);
      return false;
    }
    if (he->m_next->OppositeFace() == he->OppositeFace())
    {
      if (text_log) text_log->Print("Redundant vertex %d in face %s\n",
                                     he->Head()->m_index,
                                     static_cast<const char*>(face->GetVertexString()));
      return false;
    }
    he = he->m_next;
  } while (he != face->m_he0);
  return true;
}

bool ON_QuickHull3DImpl::CheckFaces(double tol, ON_TextLog* text_log) const
{
  bool convex = true;
  for (QH_Face* face : m_faces)
  {
    if (face->m_mark == QH_Face::VISIBLE)
    {
      if (!CheckFaceConvexity(face, tol, text_log)) convex = false;
    }
  }
  return convex;
}

bool ON_QuickHull3DImpl::IsValid(ON_TextLog* text_log, double tol) const
{
  double pointTol = 10.0 * tol;

  if (!CheckFaces(m_tolerance, text_log)) return false;

  for (int i = 0; i < m_numPoints; i++)
  {
    const ON_3dPoint& pnt = m_pointBuffer[i]->m_point;
    for (QH_Face* face : m_faces)
    {
      if (face->m_mark != QH_Face::VISIBLE) continue;
      double dist = face->DistanceToPlane(pnt);
      if (dist > pointTol)
      {
        if (text_log) text_log->Print("Point %d %.17g above face %s\n",
                                       i, dist, static_cast<const char*>(face->GetVertexString()));
        return false;
      }
    }
  }
  return true;
}

// ---------------------------------------------------------------------------
// ON_QuickHull3D - thin public wrapper over ON_QuickHull3DImpl.
// ---------------------------------------------------------------------------

ON_QuickHull3D::ON_QuickHull3D()
  : m_impl(new ON_QuickHull3DImpl())
{
}

ON_QuickHull3D::~ON_QuickHull3D()
{
  delete m_impl;
  m_impl = nullptr;
}

bool ON_QuickHull3D::Build(const ON_3dPoint* points, int point_count)
{
  return m_impl->Build(points, point_count);
}

bool ON_QuickHull3D::Build(const ON_3dPoint* points, int point_count, ON_ClassArray<ON_SimpleArray<int>>& facets)
{
  facets.Empty();
  if (!Build(points, point_count))
    return false;

  GetFaces(facets, FaceIndexFlags::PointRelative);
  return true;
}

bool ON_QuickHull3D::Build(const ON_SimpleArray<ON_3dPoint>& points)
{
  return m_impl->Build(points.Array(), points.Count());
}

bool ON_QuickHull3D::Build(const double* coords, int point_count)
{
  return m_impl->Build(coords, point_count);
}

bool ON_QuickHull3D::Build(const ON_PointCloud& point_cloud)
{
  return m_impl->BuildFromPointCloud(point_cloud);
}

bool ON_QuickHull3D::Build(const ON_PointCloud& point_cloud, ON_ClassArray<ON_SimpleArray<int>>& facets)
{
  facets.Empty();
  if (!Build(point_cloud))
    return false;

  GetFaces(facets, FaceIndexFlags::PointRelative);
  return true;
}

bool ON_QuickHull3D::HasVertexColors() const
{
  return m_impl->HasVertexColors();
}

void ON_QuickHull3D::Triangulate()
{
  m_impl->Triangulate();
}

int ON_QuickHull3D::GetNumVertices() const
{
  return m_impl->GetNumVertices();
}

int ON_QuickHull3D::GetNumFaces() const
{
  return m_impl->GetNumFaces();
}

int ON_QuickHull3D::GetVertices(ON_SimpleArray<ON_3dPoint>& vertices) const
{
  return m_impl->GetVertices(vertices);
}

int ON_QuickHull3D::GetVertices(double* coords) const
{
  return m_impl->GetVertices(coords);
}

int ON_QuickHull3D::GetVertexPointIndices(ON_SimpleArray<int>& indices) const
{
  return m_impl->GetVertexPointIndices(indices);
}

int ON_QuickHull3D::GetFaces(ON_ClassArray<ON_SimpleArray<int>>& faces, FaceIndexFlags index_flags) const
{
  return m_impl->GetFaces(faces, index_flags);
}

ON_Mesh* ON_QuickHull3D::ToMesh(ON_Mesh* mesh) const
{
  if (mesh)
    mesh->Destroy();

  ON_Mesh* newmesh = mesh ? mesh : new ON_Mesh();
  if (!m_impl->ToMesh(*newmesh))
  {
    if (newmesh != mesh)
      delete newmesh;
    return nullptr;
  }
  return newmesh;
}

double ON_QuickHull3D::GetDistanceTolerance() const
{
  return m_impl->GetDistanceTolerance();
}

void ON_QuickHull3D::SetExplicitDistanceTolerance(double tol)
{
  m_impl->SetExplicitDistanceTolerance(tol);
}

double ON_QuickHull3D::GetExplicitDistanceTolerance() const
{
  return m_impl->GetExplicitDistanceTolerance();
}

bool ON_QuickHull3D::IsValid(ON_TextLog* text_log) const
{
  return m_impl->IsValid(text_log, GetDistanceTolerance());
}

bool ON_QuickHull3D::IsValid(ON_TextLog* text_log, double tolerance) const
{
  return m_impl->IsValid(text_log, tolerance);
}
