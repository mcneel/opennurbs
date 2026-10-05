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
//
// ON_QuickHull3D computes the convex hull of a set of 3d points using the
// Quickhull algorithm described in Barber, Dobkin, and Huhdanpaa, "The
// Quickhull Algorithm for Convex Hulls" (ACM Transactions on Mathematical
// Software, Vol. 22, No. 4, December 1996).
//
// Faces whose shared edges are not clearly convex are merged, so the hull
// faces returned by ON_QuickHull3D may be convex polygons rather than
// triangles. Call Triangulate() to reduce every face to triangles.
//

#if !defined(OPENNURBS_QUICKHULL3D_INC_)
#define OPENNURBS_QUICKHULL3D_INC_

class ON_QuickHull3DImpl;

class ON_CLASS ON_QuickHull3D
{
public:
  ON_QuickHull3D();
  ~ON_QuickHull3D();

public:
  // Bit flags controlling the index format returned by GetFaces() and Print().
  // Combine them with operator|. The default (NONE) is counter-clockwise,
  // hull-relative, and zero-based.
  enum class FaceIndexFlags : unsigned int
  {
    None = 0,              // ccw, hull-relative, zero-based (the default)
    Clockwise = 0x1,       // list face vertices clockwise rather than ccw
    IndexedFromOne = 0x2,  // number vertices starting at 1
    IndexedFromZero = 0x4, // number vertices starting at 0 (the default)
    PointRelative = 0x8    // number relative to the original input points
  };

  // Description:
  //   Build the convex hull of a set of 3d points.
  // Parameters:
  //   points - [in] input points (point_count of them).
  //   point_count - [in] number of input points; must be >= 4.
  //   coords - [in] flat array of 3*point_count doubles (x0,y0,z0,x1,...).
  //   point_cloud - [in] input points; if it carries per-point colors
  //     (HasPointColors()) the colors of the hull vertices are captured and
  //     emitted by ToMesh().
  // Returns:
  //   True if a hull was built. If the input is null, has fewer than four
  //   points, or is degenerate (coincident, colinear, or coplanar within the
  //   distance tolerance) ON_ERROR is called and false is returned.
  bool Build(const ON_3dPoint* points, int point_count);
  bool Build(const double* coords, int point_count);
  bool Build(const ON_SimpleArray<ON_3dPoint>& points);
  bool Build(const ON_PointCloud& point_cloud);

  // Description:
  //   Build the convex hull of a set of 3d points, also returning the vertex
  //   indices of each hull face.
  // Parameters:
  //   points - [in] input points (point_count of them).
  //   point_count - [in] number of input points; must be >= 4.
  //   facets - [out] one entry per hull face, each containing the indices,
  //     into the points array, of that face's vertices - equivalent to
  //     GetFaces(facets, FaceIndexFlags::PointRelative), but returned directly from
  //     Build() without a separate call. Emptied if the hull could not be
  //     built.
  // Returns:
  //   True if a hull was built. See Build(const ON_3dPoint*, int) for the
  //   conditions under which this fails.
  bool Build(const ON_3dPoint* points, int point_count, ON_ClassArray<ON_SimpleArray<int>>& facets);

  // Description:
  //   Build the convex hull of a point cloud, also returning the vertex
  //   indices of each hull face.
  // Parameters:
  //   point_cloud - [in] input points; if it carries per-point colors
  //     (HasPointColors()) the colors of the hull vertices are captured and
  //     emitted by ToMesh().
  //   facets - [out] one entry per hull face, each containing the indices,
  //     into point_cloud, of that face's vertices - equivalent to
  //     GetFaces(facets, FaceIndexFlags::PointRelative), but returned directly from
  //     Build() without a separate call. Emptied if the hull could not be
  //     built.
  // Returns:
  //   True if a hull was built. See Build(const ON_PointCloud&) for the
  //   conditions under which this fails.
  bool Build(const ON_PointCloud& point_cloud, ON_ClassArray<ON_SimpleArray<int>>& facets);

  // Returns true if the most recent Build() captured per-point colors that
  // ToMesh() will emit as per-vertex colors on the output mesh.
  bool HasVertexColors() const;

  // Triangulates any non-triangular hull faces in place.
  void Triangulate();

  // Number of vertices / faces in the most recently built hull.
  int GetNumVertices() const;
  int GetNumFaces() const;

  // Description:
  //   Get the hull vertex points.
  // Parameters:
  //   vertices - [out] emptied, then filled with the hull vertex points.
  // Returns:
  //   The number of hull vertices.
  int GetVertices(ON_SimpleArray<ON_3dPoint>& vertices) const;

  // Copies the hull vertex coordinates into coords (3*GetNumVertices()
  // doubles) and returns the number of vertices, or 0 if coords is null.
  int GetVertices(double* coords) const;

  // Description:
  //   Get, for each hull vertex, the index of the input point it came from.
  // Parameters:
  //   indices - [out] emptied, then filled with one input point index per
  //     hull vertex.
  // Returns:
  //   The number of hull vertices.
  int GetVertexPointIndices(ON_SimpleArray<int>& indices) const;

  // Description:
  //   Get the vertex indices of each hull face.
  // Parameters:
  //   faces - [out] emptied, then filled with one entry per hull face, each
  //     containing the indices of that face's vertices.
  //   index_flags - [in] any combination of FaceIndexFlags values.
  // Returns:
  //   The number of hull faces.
  int GetFaces(ON_ClassArray<ON_SimpleArray<int>>& faces, FaceIndexFlags index_flags = FaceIndexFlags::None) const;

  // Description:
  //   Populate an ON_Mesh from the hull. Triangular faces become triangles
  //   (ON_MeshFace with vi[2]==vi[3]); merged quads become quads; merged
  //   faces of five or more sides are fan-triangulated and the tessellation
  //   triangles are grouped as a single ON_MeshNgon so the original hull
  //   face is preserved. Face and vertex normals are computed. Per-vertex
  //   colors are set when HasVertexColors() is true.
  // Parameters:
  //   mesh - [in] if not nullptr, this mesh is destroyed and reused to
  //     return the result; otherwise a new ON_Mesh is allocated.
  // Returns:
  //   A pointer to the populated mesh (mesh, or a newly allocated ON_Mesh
  //   when mesh is nullptr), or nullptr if no hull has been built, in which
  //   case a caller-supplied mesh is left untouched.
  class ON_Mesh* ToMesh(ON_Mesh* mesh) const;

  // Distance tolerance used to decide when faces are unambiguously convex and
  // when points are unambiguously above or below a face plane. By default it
  // is computed automatically from the input point data. Setting an explicit
  // tolerance that is not a valid positive number restores that default, and
  // GetExplicitDistanceTolerance() then returns ON_UNSET_VALUE.
  double GetDistanceTolerance() const;
  void SetExplicitDistanceTolerance(double tol);
  double GetExplicitDistanceTolerance() const;

  // Description:
  //   Verify the hull: no face is non-convex and no input point lies outside
  //   any face, both measured against the distance tolerance.
  // Parameters:
  //   text_log - [in] destination for diagnostic messages; may be null.
  //   tol - [in] distance tolerance to test against.
  // Returns:
  //   True if the hull is valid.
  bool IsValid(ON_TextLog* text_log) const;
  bool IsValid(ON_TextLog* text_log, double tolerance) const;

private:
  ON_QuickHull3D(const ON_QuickHull3D&) = delete;
  ON_QuickHull3D& operator=(const ON_QuickHull3D&) = delete;

private:
  ON_QuickHull3DImpl* m_impl = nullptr;
};

// Bitwise combination operators for ON_QuickHull3D::FaceIndexFlags, so callers
// can write e.g. ON_QuickHull3D::FaceIndexFlags::CLOCKWISE |
// ON_QuickHull3D::FaceIndexFlags::INDEXED_FROM_ONE.
inline ON_QuickHull3D::FaceIndexFlags operator|(
  ON_QuickHull3D::FaceIndexFlags lhs, ON_QuickHull3D::FaceIndexFlags rhs)
{
  return static_cast<ON_QuickHull3D::FaceIndexFlags>(
    static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

inline ON_QuickHull3D::FaceIndexFlags operator&(
  ON_QuickHull3D::FaceIndexFlags lhs, ON_QuickHull3D::FaceIndexFlags rhs)
{
  return static_cast<ON_QuickHull3D::FaceIndexFlags>(
    static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

inline ON_QuickHull3D::FaceIndexFlags& operator|=(
  ON_QuickHull3D::FaceIndexFlags& lhs, ON_QuickHull3D::FaceIndexFlags rhs)
{
  lhs = lhs | rhs;
  return lhs;
}

#endif
