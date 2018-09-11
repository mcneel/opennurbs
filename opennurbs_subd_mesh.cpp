#include "opennurbs.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

#include "opennurbs_subd_data.h"

/* $NoKeywords: $ */
/*
//
// Copyright (c) 1993-2014 Robert McNeel & Associates. All rights reserved.
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
*/



bool ON_SubDQuadFaceSubdivisionCounter::BreakpointTest()
{
  if ( nullptr == m_level0_face )
    return false;
  if ( 11 != m_level0_face->m_id )
    return false;
  if ( m_subdivision_count < 1 )
    return false;
  if ( 1 != m_corner_index[0])
    return false;
  if ( m_subdivision_count == 1 && 1 != m_corner_index[1])
    return false;

  bool breakpoint_here = true;  // <- breakpoint here

  return breakpoint_here;
}


static ON_ProgressStepCounter CreateFragmentProgressStepCounter(
  ON_SubDFaceIterator& fit,
  const ON_SubDDisplayParameters& limit_mesh_parameters
  )
{
  unsigned int progress_step_count = 0;

  if (nullptr != limit_mesh_parameters.m_progress_reporter)
  {
    for (const ON_SubDFace* face = fit.FirstFace(); nullptr != face; face = fit.NextFace())
    {
      if ( 4 == face->m_edge_count)
        progress_step_count++;
      else
        progress_step_count += face->m_edge_count;
    }
  }

  ON_ProgressStepCounter counter = ON_ProgressStepCounter::Create(
    limit_mesh_parameters.m_progress_reporter,
    progress_step_count,
    limit_mesh_parameters.m_progress_reporter_interval[0],
    limit_mesh_parameters.m_progress_reporter_interval[1],
    100
    );

  return counter;

}

static unsigned int GetQuadLimitSurfaceMeshFragmentsHelper(
  ON_SubDFaceIterator& fit,
  const ON_SubDDisplayParameters& limit_mesh_parameters,
  ON__UINT_PTR fragment_callback_context,
  bool(*mesh_fragment_callback_function)(ON__UINT_PTR, const class ON_SubDLimitMeshFragment*)
  )
{
  ON_ProgressStepCounter counter = CreateFragmentProgressStepCounter(fit,limit_mesh_parameters);
  
  if (nullptr == fit.FirstFace() )
    return ON_SUBD_RETURN_ERROR(0);

  if ( nullptr == mesh_fragment_callback_function )
    return ON_SUBD_RETURN_ERROR(0);

  unsigned int display_density = limit_mesh_parameters.m_display_density;
  //const bool bUseMultipleThreads = limit_mesh_parameters.m_bUseMultipleThreads;


  ON_SubDManagedLimitMeshFragment fragment;
  ON_SubDManagedLimitMeshFragment sub_fragment;
  ON_SubDQuadFaceMesher qfm;
  ON_SubDQuadFaceMesher sub_qfm;
  ON_SubDFaceNeighborhood fnbd;
  ON_SubDQuadFaceSubdivisionCounter quad_face_subdivsion_counter;


  qfm.m_output = ON_SubDQuadFaceMesher::Output::mesh;

  ON_SubDLimitMeshFragment* callback_fragment = nullptr;
  const ON_SubDFace** quad_faces = nullptr;
  unsigned int quad_face_count = 0;

  unsigned int fragment_count = 0;

  if (0 == display_density)
  {
    // make sure all faces are quads.  If not, increase density to 1
    for (const ON_SubDFace* face = fit.FirstFace(); nullptr != face; face = fit.NextFace())
    {
      if (4 == face->m_edge_count)
        continue;
      display_density = 1;
      break;
    }
  }

  // TODO 
  //
  //   Support multiple threads by adding more fragment, sub_fragment qfm, sub_qfm and fnbd
  //   resources and managing them.  
  //
  const unsigned int subquad_display_density = (display_density > 1) ? (display_density - 1) : 0;
  const unsigned short unset_face_edge_index = 0xFFFFU;
  for (const ON_SubDFace* face = fit.FirstFace(); nullptr != face; face = fit.NextFace())
  {
    quad_face_subdivsion_counter.SetLevel0Face(face);

    //const unsigned int initial_subd_level = static_cast<unsigned int>(face->m_level);

    if (4 == face->m_edge_count)
    {
      // face is a quad
      quad_faces = &face;
      quad_face_count = 1;
      if (callback_fragment != &fragment)
      {
        if (0 == fragment.m_P_capacity)
        {
          fragment.ReserveCapacity(ON_SubD::FacetType::Quad, display_density);
          fragment.m_P_count = (unsigned short)ON_SubDLimitMeshFragment::PointCountFromDisplayDensity(ON_SubD::FacetType::Quad,display_density);
          if ( fragment.m_P_count > fragment.m_P_capacity)
            return ON_SUBD_RETURN_ERROR(0);
          fragment.m_face_vertex_index[0] = 0;
          fragment.m_face_vertex_index[1] = 1;
          fragment.m_face_vertex_index[2] = 2;
          fragment.m_face_vertex_index[3] = 3;
        }
        callback_fragment = &fragment;
        qfm.SetDestinationToMeshFragment(display_density, fragment);
      }
    }
    else
    {
      // face is not a quad.  It will be subdivided into quads and each
      // of those quads is meshed a a level of display_density-1.
      if (false == fnbd.Subdivide(ON_SubD::SubDType::QuadCatmullClark, face))
        continue;
      quad_faces = fnbd.m_center_vertex1->m_faces;
      quad_face_count = fnbd.m_center_vertex1->m_face_count;
      if (callback_fragment != &sub_fragment)
      {
        if (0 == sub_fragment.m_P_capacity)
        {
          sub_fragment.ReserveCapacity(ON_SubD::FacetType::Quad, subquad_display_density);
          sub_fragment.m_P_count = (unsigned short)ON_SubDLimitMeshFragment::PointCountFromDisplayDensity(ON_SubD::FacetType::Quad,subquad_display_density);
          if ( sub_fragment.m_P_count > sub_fragment.m_P_capacity)
            return ON_SUBD_RETURN_ERROR(0);
          sub_fragment.m_face_vertex_index[0] = unset_face_edge_index;
          sub_fragment.m_face_vertex_index[1] = unset_face_edge_index;
          sub_fragment.m_face_vertex_index[2] = unset_face_edge_index;
          sub_fragment.m_face_vertex_index[3] = unset_face_edge_index;
        }
        callback_fragment = &sub_fragment;
        qfm.SetDestinationToMeshFragment(subquad_display_density, sub_fragment);
      }
    }

    callback_fragment->m_face = face;
    callback_fragment->m_face_fragment_count = (unsigned short)quad_face_count;

    for (unsigned int qi = 0; qi < quad_face_count; qi++)
    {
      if (quad_face_count > 1)
      {
        if ( qi > 0 )
          quad_face_subdivsion_counter.Pop();
        quad_face_subdivsion_counter.Push(qi);
      }

      const ON_SubDFace* f = quad_faces[qi];
      if (unset_face_edge_index == callback_fragment->m_face_vertex_index[0])
        callback_fragment->m_face_vertex_index[2] = (unsigned short)((qi+1)%quad_face_count);

      callback_fragment->m_face_fragment_index = (unsigned short)qi;

      if (false == qfm.GetLimitMesh(quad_face_subdivsion_counter, f))
        continue;
      
      fragment_count++;
      callback_fragment->m_bbox = ON_PointListBoundingBox(3,0,callback_fragment->m_P_count,(int)callback_fragment->m_P_stride,callback_fragment->m_P);
      if (false == mesh_fragment_callback_function(fragment_callback_context, callback_fragment))
        return true;

      if (0 == (fragment_count % 16))
      {
        if (ON_Terminator::TerminationRequested(limit_mesh_parameters.m_terminator))
          return 0; // not an error
      }
      counter.IncrementStep();
    }
  }

  counter.Finished();

  return fragment_count;
}


static unsigned int GetQuadLimitSurfacePatchFragmentsHelper(
  ON_SubDFaceIterator& fit,
  const ON_SubDDisplayParameters& limit_mesh_parameters,
  ON__UINT_PTR fragment_callback_context,
  bool(*patch_fragment_callback_function)(ON__UINT_PTR, const class ON_SubDLimitPatchFragment*)
  )
{
  ON_ProgressStepCounter counter = CreateFragmentProgressStepCounter(fit,limit_mesh_parameters);
  
  if (nullptr == fit.FirstFace() )
    return ON_SUBD_RETURN_ERROR(0);

  if ( nullptr == patch_fragment_callback_function )
    return ON_SUBD_RETURN_ERROR(0);

  unsigned int display_density = limit_mesh_parameters.m_display_density;
  //const bool bUseMultipleThreads = limit_mesh_parameters.m_bUseMultipleThreads;
  
  ON_SubDQuadFacePatcher patcher;
  ON_SubDQuadFaceMesher qfm;
  ON_SubDQuadFaceMesher sub_qfm;
  ON_SubDFaceNeighborhood fnbd;
  ON_SubDQuadFaceSubdivisionCounter quad_face_subdivsion_counter;

  const ON_SubDFace** quad_faces = nullptr;
  unsigned int quad_face_count = 0;

  unsigned int fragment_count = 0;

  if (0 == display_density)
  {
    // make sure all faces are quads.  If not, increase density to 1
    for (const ON_SubDFace* face = fit.FirstFace(); nullptr != face; face = fit.NextFace())
    {
      if (4 == face->m_edge_count)
      {
        display_density = 1;
        break;
      }
    }
  }

  patcher.m_fragment_callback_context = fragment_callback_context;
  patcher.m_fragment_callback_function = patch_fragment_callback_function;

  // TODO 
  //
  //   Support multiple threads by adding more fragment, sub_fragment qfm, sub_qfm and fnbd
  //   resources and managing them.  
  //
  const unsigned int subquad_display_density = (display_density > 1) ? (display_density - 1) : 0;

  for (const ON_SubDFace* face = fit.FirstFace(); nullptr != face; face = fit.NextFace())
  {
    quad_face_subdivsion_counter.SetLevel0Face(face);

    if (4 == face->m_edge_count)
    {
      // face is a quad
      quad_faces = &face;
      quad_face_count = 1;
      patcher.m_display_density = display_density;

    }
    else
    {
      // face is not a quad.  It will be subdivided into quads and each
      // of those quads is meshed a a level of display_density-1.
      if (false == fnbd.Subdivide(ON_SubD::SubDType::QuadCatmullClark, face))
        continue;
      quad_faces = fnbd.m_center_vertex1->m_faces;
      quad_face_count = fnbd.m_center_vertex1->m_face_count;
      patcher.m_display_density = subquad_display_density;
    }

    patcher.m_patch_fragment = ON_SubDLimitPatchFragment::Unset;
    patcher.m_patch_fragment.m_level0_face = face;
    patcher.m_patch_fragment.m_level0_face_region_count = (unsigned short)quad_face_count;

    qfm.SetDestinationToPatchFragment(patcher);

    for (unsigned int qi = 0; qi < quad_face_count; qi++)
    {
      const ON_SubDFace* f = quad_faces[qi];

      patcher.m_patch_fragment.m_level0_face_region_index = (unsigned short)qi;

      if (quad_face_count > 1)
      {
        if ( qi > 0 )
          quad_face_subdivsion_counter.Pop();
        quad_face_subdivsion_counter.Push(qi);
      }

      if (false == qfm.GetLimitPatches(quad_face_subdivsion_counter, f))
        continue;
      
      fragment_count++;

      if (0 == (fragment_count % 16))
      {
        if (ON_Terminator::TerminationRequested(limit_mesh_parameters.m_terminator))
          return 0; // not an error
      }
      counter.IncrementStep();
    }
  }

  counter.Finished();

  return fragment_count;
}

class GetLimitSurfaceMesh_context_FragmentMark
{
public:
  class GetLimitSurfaceMesh_context* m_context = nullptr;

  size_t m_point_count0 = 0;
  size_t m_quad_count0 = 0;
  size_t m_fragment_count0 = 0;

  size_t m_point_count1 = 0;
  size_t m_quad_count1 = 0;
  size_t m_fragment_count1 = 0;

  unsigned int m_mark_index = 0;
  unsigned int m_face_id = 0;
  unsigned int m_zero_face_id = 0;
  unsigned int m_parent_face_id = 0;
  unsigned int m_face_fragment_count = 0;
  unsigned int m_face_fragment_index = 0;

  ON__UINT_PTR m_fragment_group_id = 0;

  // points to memory in the m_context that is under construction.
  ON_SubDLimitMeshFragment m_fragment = ON_SubDLimitMeshFragment::Empty;

  static int CompareFaceIdAndFragmentIndex(
    const GetLimitSurfaceMesh_context_FragmentMark* a,
    const GetLimitSurfaceMesh_context_FragmentMark* b
    );
};

int GetLimitSurfaceMesh_context_FragmentMark::CompareFaceIdAndFragmentIndex(
    const GetLimitSurfaceMesh_context_FragmentMark* a,
    const GetLimitSurfaceMesh_context_FragmentMark* b
    )
{
  if ( a == b )
    return 0;
  if ( nullptr == a )
    return -1;
  if ( nullptr == b )
    return 1;
  if ( a->m_face_id < b->m_face_id )
    return -1;
  if ( a->m_face_id > b->m_face_id )
    return 1;
  if ( a->m_face_fragment_index < b->m_face_fragment_index )
    return -1;
  if ( a->m_face_fragment_index > b->m_face_fragment_index )
    return 1;
  return 0;
}

class GetLimitSurfaceMesh_context
{
public:
  GetLimitSurfaceMesh_context() = default;

#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
  bool m_bUseMultipleThreads = false; // If true, callback uses the lock
  ON_SleepLock m_lock;
#endif

  size_t m_point_capacity = 0;
  double* m_points = nullptr;
  size_t m_point_stride = 0;

  size_t m_normal_capacity = 0;
  double* m_normals = nullptr;
  size_t m_normal_stride = 0;

  size_t m_quad_capacity = 0;
  
  unsigned int* m_quads = nullptr;
  size_t m_quad_stride = 0; // >= 4

  ON__UINT_PTR* m_quad_group_ids = nullptr;
  size_t m_quad_group_id_stride = 0;

  // counts are updated as meshing progresses
  size_t m_point_count = 0;
  size_t m_quad_count = 0;
  size_t m_fragment_count = 0;

  ON_SimpleArray< GetLimitSurfaceMesh_context_FragmentMark > m_marks;
};

static bool CoincidentPointTest(
  unsigned int i,
  unsigned int j,
  const double* points,
  const size_t point_stride,
  const double* normals,
  const size_t normal_stride
  )
{
  if ( i == j)
    return true;
  
  const double* a = points + i*point_stride;
  const double* b = points + j*point_stride;
  double d = fabs(a[0]-b[0]) + fabs(a[1]-b[1]) + fabs(a[2]-b[2]);
  if (!(d <= 1e-8))
    return ON_SUBD_RETURN_ERROR(false);
  
  a = normals + i*normal_stride;
  b = normals + j*normal_stride;
  d = fabs(a[0]-b[0]) + fabs(a[1]-b[1]) + fabs(a[2]-b[2]);
  if (!(d <= 0.01))
    return ON_SUBD_RETURN_ERROR(false);
  
  return true;
}

static bool GetLimitSurfaceMesh_callback(ON__UINT_PTR void_context, const class ON_SubDLimitMeshFragment* fragment)
{
  GetLimitSurfaceMesh_context* context = (GetLimitSurfaceMesh_context*)void_context;

  unsigned int mark_count = 0;
  ON_SimpleArray< GetLimitSurfaceMesh_context_FragmentMark > face_fragment_marks;

  // When using multiple threads, 
  // get the lock,
  // quickly reserve the section of the desitinataion arrays that will be used by this fragment, 
  // and then return the lock.
  // SInce the space is now reserved, the actual copying can be done after returning the lock.
  GetLimitSurfaceMesh_context_FragmentMark mark;
  mark.m_context = context;
  mark.m_fragment = *fragment;
  if (nullptr != fragment->m_face)
  {
    mark.m_face_id = fragment->m_face->m_id;
    mark.m_zero_face_id = fragment->m_face->m_zero_face_id;
    mark.m_parent_face_id = fragment->m_face->m_parent_face_id;
  }
  mark.m_face_fragment_count = fragment->m_face_fragment_count;
  mark.m_face_fragment_index = fragment->m_face_fragment_index;

#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
  bool bReleaseLock = false;
  if (context->m_bUseMultipleThreads)
  {
    if (false == context->m_lock.GetLock(0, ON_SleepLock::OneSecond))
    {
      // return tru to keep going, but something is really hogging the lock
      return ON_SUBD_RETURN_ERROR(true);
    }
  }
#endif

  mark.m_point_count0 = context->m_point_count;
  mark.m_quad_count0 = context->m_quad_count;
  mark.m_point_count1 = mark.m_point_count0 + fragment->m_P_count;
  mark.m_quad_count1 = mark.m_quad_count0 + fragment->m_grid.m_F_count;
  if (mark.m_point_count1 > context->m_point_capacity || mark.m_quad_count1 > context->m_quad_capacity)
  {
#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
    if (bReleaseLock)
      context->m_lock.ReturnLock();
#endif
    return ON_SUBD_RETURN_ERROR(false);
  }
  mark.m_mark_index = context->m_marks.UnsignedCount();

  mark.m_fragment_group_id = (ON__UINT_PTR)(context->m_fragment_count+1);

  if (mark.m_face_fragment_count <= 1 || 0 == mark.m_face_id || ON_UNSET_UINT_INDEX == mark.m_face_id)
  {
    mark_count = 1;
  }
  else if ( context->m_marks.UnsignedCount() + 1 >= mark.m_face_fragment_count )
  { 
    GetLimitSurfaceMesh_context_FragmentMark* context_fragment_marks = context->m_marks.Array();
    const unsigned int context_mark_count0 = context->m_marks.UnsignedCount();

    // See if we have all the subfragments for this face
    for (unsigned int i = 0; i < context_mark_count0; i++)
    {
      if ( context_fragment_marks[i].m_face_id == mark.m_face_id )
        mark_count++;
    }

    if (mark_count + 1 >= mark.m_face_fragment_count)
    {
      // move all the marks for this face from context->m_marks[] (possibly used by multiple threads)
      // to the local context_fragment_marks[] array for processing below.
      unsigned int context_mark_count1 = 0;
      face_fragment_marks.Reserve(mark.m_face_fragment_count);
      if (context_mark_count0 + 1 >= mark.m_face_fragment_count)
      {
        face_fragment_marks.Append((int)context_mark_count0, context_fragment_marks);
      }
      else
      {
        for (unsigned int i = 0; i < context_mark_count0; i++)
        {
          if (context_fragment_marks[i].m_face_id == mark.m_face_id)
            face_fragment_marks.Append(context_fragment_marks[i]);
          else
          {
            if (i < context_mark_count1)
              context_fragment_marks[context_mark_count1] = context_fragment_marks[i];
            context_mark_count1++;
          }
        }
      }
      context->m_marks.SetCount(context_mark_count1);
      face_fragment_marks.Append(mark);
      mark_count++;
    }
    else
    {
      // This face will have more subfragments delivered in the future.
      mark_count = 0;
      context->m_marks.AppendNew() = mark;
    }
  }
  else
  {
    // This face will have more subfragments delivered in the future.
    context->m_marks.AppendNew() = mark;
  }

  const size_t P_stride = context->m_point_stride;
  const size_t N_stride = context->m_normal_stride;
  const size_t F_stride = context->m_quad_stride;
  const size_t GID_stride = context->m_quad_group_id_stride;

  double* P 
    = (nullptr != context->m_points) 
    ? (context->m_points + mark.m_point_count0*P_stride)
    : nullptr;
  double* N 
    = (nullptr != context->m_normals) 
    ? (context->m_normals + mark.m_point_count0*N_stride)
    : nullptr;
  unsigned int* F 
    = (nullptr != context->m_quads)
    ? (context->m_quads + mark.m_quad_count0*F_stride)
    : nullptr;
  ON__UINT_PTR* GID 
    = (nullptr != context->m_quad_group_ids)
    ? (context->m_quad_group_ids + mark.m_quad_count0*GID_stride)
    : nullptr;

  context->m_point_count = mark.m_point_count1;
  context->m_quad_count = mark.m_quad_count1;
  context->m_fragment_count++;

#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
  if (bReleaseLock)
    context->m_lock.ReturnLock();
#endif

  // Copy the mesh from the fragment to the destination arrays
  //
  // All the code below must be thread safe.  
  //
  // Basically, space context->m_point_stride[] and the other arrays
  // is reserved above and time consuming calculations take place below so
  // any other threads working to create this mesh can continue.
  if ( nullptr != P )
  {
    const double* srcP1 = fragment->m_P + fragment->m_P_count*fragment->m_P_stride;
    for (const double* srcP = fragment->m_P; srcP < srcP1; srcP += fragment->m_P_stride)
    {
      P[0] = srcP[0];
      P[1] = srcP[1];
      P[2] = srcP[2];
      P += P_stride;
    }  
  }

  if ( nullptr != N )
  {
    const double* srcN1 = fragment->m_N + fragment->m_P_count*fragment->m_N_stride;
    for (const double* srcN = fragment->m_N; srcN < srcN1; srcN += fragment->m_N_stride)
    {
      N[0] = srcN[0];
      N[1] = srcN[1];
      N[2] = srcN[2];
      N += N_stride;
    }  
  }

  if ( nullptr != F )
  {
    const ON_SubDLimitMeshFragmentGrid& quads = fragment->m_grid;
    const unsigned int fvi0 = (unsigned int)mark.m_point_count0;
    const unsigned int* srcF1 = quads.m_F + quads.m_F_count*quads.m_F_stride;
    for (const unsigned int* srcF = quads.m_F; srcF < srcF1; srcF += quads.m_F_stride)
    {
      F[0] = srcF[0] + fvi0;
      F[1] = srcF[1] + fvi0;
      F[2] = srcF[2] + fvi0;
      F[3] = srcF[3] + fvi0;
      F += F_stride;
    }  
  }

  if ( nullptr != GID )
  {
    const ON__UINT_PTR* group_id1 = GID + fragment->m_grid.m_F_count*GID_stride;
    while(GID < group_id1)
    {
      *GID = mark.m_fragment_group_id;
      GID += GID_stride;
    }  
  }

  while (mark_count >= 2 && face_fragment_marks.UnsignedCount() == mark_count )
  {
    // combine subfragments into a single fragment.
    face_fragment_marks.QuickSort( GetLimitSurfaceMesh_context_FragmentMark::CompareFaceIdAndFragmentIndex );
    if (face_fragment_marks[0].m_face_id != face_fragment_marks[mark_count - 1].m_face_id )
      break;
    if (0 != face_fragment_marks[0].m_face_fragment_index)
      break;
    if (mark_count - 1 != face_fragment_marks[mark_count - 1].m_face_fragment_index)
      break;

    mark = face_fragment_marks[0];
    unsigned int* quads = mark.m_context->m_quads;
    const size_t quad_stride = mark.m_context->m_quad_stride;
    const double* points = mark.m_context->m_points;
    const size_t point_stride = mark.m_context->m_point_stride;
    const double* normals = mark.m_context->m_normals;
    const size_t normal_stride = mark.m_context->m_normal_stride;
    ON__UINT_PTR* group_ids = mark.m_context->m_quad_group_ids;
    const size_t group_id_stride = mark.m_context->m_quad_group_id_stride;

    if ( nullptr == quads || nullptr == points || nullptr == normals)
      break;
    if ( quad_stride < 4 || point_stride < 3 || normal_stride < 3 )
      break;

    const unsigned int grid_F_count = mark.m_fragment.m_grid.m_F_count;
    const unsigned int grid_side_count = mark.m_fragment.m_grid.SideSegmentCount();
    const size_t magic_stride = grid_side_count*quad_stride;

    const ON__UINT_PTR group_id =
      (nullptr != group_ids && group_id_stride > 0)
      ? group_ids[mark.m_quad_count0*group_id_stride]
      : 0;

    const unsigned int apex_point_index = (unsigned int)mark.m_point_count0;
    const ON_3dPoint apex_point(points+apex_point_index*point_stride);
    const ON_3dVector apex_normal(normals+apex_point_index*normal_stride);
          
    unsigned int* subfragment_quads = quads + face_fragment_marks[mark_count-1].m_quad_count0*quad_stride;
    unsigned int i;
    for ( i = 0; i < mark_count; i++ )
    {
      mark = face_fragment_marks[i];
      if ( grid_F_count != mark.m_fragment.m_grid.m_F_count)
        break;
      unsigned int* prev_subfragment_quads = subfragment_quads;
      subfragment_quads = quads + mark.m_quad_count0*quad_stride;

      if (false == CoincidentPointTest(apex_point_index,subfragment_quads[0],points,point_stride,normals,normal_stride))
        break;
      subfragment_quads[0] = apex_point_index;

      unsigned int n;
      for (n = 0; n < grid_side_count; n++)
      {
        if (false == CoincidentPointTest(subfragment_quads[0],prev_subfragment_quads[0],points,point_stride,normals,normal_stride))
          break;
        prev_subfragment_quads[0] = subfragment_quads[0];
        
        if (false == CoincidentPointTest(subfragment_quads[1],prev_subfragment_quads[3],points,point_stride,normals,normal_stride))
          break;
        prev_subfragment_quads[3] = subfragment_quads[1];

        subfragment_quads += quad_stride;
        prev_subfragment_quads += magic_stride;
      }

      if (n < grid_side_count)
        break;

      subfragment_quads -= magic_stride;

      if (0 != group_id)
      {
        ON__UINT_PTR* p0 = group_ids + mark.m_quad_count0*group_id_stride;
        if (group_id != p0[0])
        {
          for ( ON__UINT_PTR* p1 = p0 + grid_F_count; p0 < p1; p0 += group_id_stride)
            *p0 = group_id;
        }
      }

    }
    if ( mark_count != i )
      break;

    break;
  }

  return true;
}

static int CompareQuadGroupId(const void* a, const void* b)
{
  ON__UINT_PTR ai = *(const ON__UINT_PTR*)a;
  ON__UINT_PTR bi = *(const ON__UINT_PTR*)b;
  if (ai < bi)
    return -1;
  if (ai > bi)
    return 1;
  return 0;
}

ON_Mesh* ON_SubD::GetLimitSurfaceMesh(
  const class ON_SubDDisplayParameters& limit_mesh_parameters,
  ON_Mesh* destination_mesh
  ) const
{
  ON_SubDDisplayParameters local_limit_mesh_parameters = limit_mesh_parameters;
  ON_ProgressReporter::ReportProgress( local_limit_mesh_parameters.m_progress_reporter, 0.0 );

  if (destination_mesh)
    destination_mesh->Destroy();

  const ON_SubDLevel& active_level = ActiveLevel();
  if (active_level.IsEmpty())
    return ON_SUBD_RETURN_ERROR(nullptr);

  if (ON_SubD::SubDType::Unset == active_level.m_subdivision_type )
    const_cast<ON_SubD*>(this)->SetSubDType(ON_SubD::SubDType::QuadCatmullClark);

  const unsigned int fragment_count = LimitSurfaceMeshFragmentCount();
  if ( fragment_count <= 0 )
    return ON_SUBD_RETURN_ERROR(nullptr);

  const ON_SubD::SubDType subd_type = active_level.m_subdivision_type;

  if (ON_SubD::SubDType::QuadCatmullClark != subd_type)
  {
    // TODO - support tri subd after quad stuff is finished.
    return ON_SUBD_RETURN_ERROR(nullptr);
  }

  if (0 == local_limit_mesh_parameters.m_display_density && fragment_count > FaceCount())
    local_limit_mesh_parameters.m_display_density = 1;

  //////////////////////////////////////////////////////////////////
  //
  // Set:
  //  vertex_count = number of points needed to calculate the mesh
  //  face_count = number of quad faces needed to calculate the mesh
  //

  unsigned int vertex_count = 0;
  unsigned int face_count = 0;

  unsigned int tri_count = 0; // tri count
  unsigned int quad_count = 0; // quad count
  unsigned int ngon_edge_sum = 0; // sum of edge counts for faces with m_edge_count >= 5.
  ON_SubDFaceIterator fit(*this);
  for (const ON_SubDFace* f = fit.FirstFace(); nullptr != f; f = fit.NextFace())
  {
    if (3 == f->m_edge_count)
      tri_count++;
    if (4 == f->m_edge_count)
      quad_count++;
    else
      ngon_edge_sum += f->m_edge_count;
  }

  if (ON_SubD::SubDType::QuadCatmullClark == subd_type)
  {
    const unsigned int m0 = 1 << local_limit_mesh_parameters.m_display_density;
    const unsigned int m1 = (m0 > 1) ? m0 / 2 : 1;
    if (ngon_edge_sum > 0 || tri_count > 0)
    {
      ngon_edge_sum += 4 * quad_count + 3 * tri_count;
      quad_count = 0;
      tri_count = 0;
    }

    vertex_count = quad_count*(m0 + 1)*(m0 + 1) + ngon_edge_sum*(m1 + 1)*(m1 + 1);
    face_count = quad_count*m0*m0 + ngon_edge_sum*m1*m1;
  }
  else if (ON_SubD::SubDType::TriLoopWarren == subd_type)
  {
    // TODO ADD TRI SUPPORT
    return ON_SUBD_RETURN_ERROR(nullptr);
  }
  
  if (vertex_count < 4 || face_count < 1)
    return ON_SUBD_RETURN_ERROR(nullptr);

  std::unique_ptr< ON_Mesh > up;  
  ON_Mesh* mesh = nullptr;
  if (nullptr != destination_mesh)
    mesh = destination_mesh;
  else
  {
    up = std::make_unique< ON_Mesh >();
    mesh = up.get();
  }

  if ( nullptr == mesh)
    return ON_SUBD_RETURN_ERROR(nullptr);

  // mesh vertices

  // mesh face group ids
 
  GetLimitSurfaceMesh_context context;

  ON_3dPointArray& D = mesh->DoublePrecisionVertices();
  context.m_point_capacity = vertex_count;
  context.m_points = (double*)D.Reserve(vertex_count);
  context.m_point_stride = 3;

  ON_SimpleArray< ON_3dVector > mesh_N;
  context.m_normal_capacity = vertex_count;
  context.m_normals = (double*)mesh_N.Reserve(vertex_count);
  context.m_normal_stride = 3;

  context.m_quad_capacity = face_count;
  context.m_quads = (unsigned int*)mesh->m_F.Reserve(face_count);
  context.m_quad_stride = 4;

  ON_SimpleArray< ON__UINT_PTR > quad_group_ids_buffer;
  ON__UINT_PTR* quad_group_ids = quad_group_ids_buffer.Reserve(face_count);
  context.m_quad_group_ids = quad_group_ids;
  context.m_quad_group_id_stride = 1;

#if defined(OPENNURBS_SLEEPLOCK_AVAILABLE)
  context.m_bUseMultipleThreads = local_limit_mesh_parameters.m_bUseMultipleThreads;
#endif

  if (ON_SubD::SubDType::QuadCatmullClark == subd_type)
  {
    //ON_Interval progress_limits(0.1,0.9);
    const ON_Interval progress_limits = local_limit_mesh_parameters.m_progress_reporter_interval;
    if (progress_limits.IsIncreasing())
    {
      local_limit_mesh_parameters.m_progress_reporter_interval.Set(
        progress_limits.ParameterAt(0.0),
        progress_limits.ParameterAt(0.5)
        );
    }
    unsigned int quad_fragment_count = GetQuadLimitSurfaceMeshFragmentsHelper(
      fit,
      local_limit_mesh_parameters,
      (ON__UINT_PTR)&context,
      GetLimitSurfaceMesh_callback
      );
    local_limit_mesh_parameters.m_progress_reporter_interval = progress_limits;
    if ( 0 == quad_fragment_count )
      return ON_SUBD_RETURN_ERROR(nullptr);
  }
  else if (ON_SubD::SubDType::TriLoopWarren == subd_type)
  {
    // todo - add tri support
    return ON_SUBD_RETURN_ERROR(nullptr);
  }
  else
  {
    return ON_SUBD_RETURN_ERROR(nullptr);
  }
  
  const unsigned int mesh_point_count = (unsigned int)context.m_point_count;
  const unsigned int mesh_face_count = (unsigned int)context.m_quad_count;
  
  if (mesh_point_count < 3 || mesh_face_count < 1)
    return ON_SUBD_RETURN_ERROR(nullptr);

  // Set face count
  mesh->m_F.SetCount(mesh_face_count);

  // Set vertex counts
  D.SetCount(mesh_point_count);
  quad_group_ids_buffer.SetCount(mesh_face_count);
  mesh->UpdateSinglePrecisionVertices();

  // copy ON_3dVector vertex normals in mesh_N[] to ON_3fVector normals in mesh->m_N[] 
  mesh_N.SetCount(mesh_point_count);
  const ON_3dVector* dN = mesh_N.Array();
  const ON_3dVector* dN1  = dN + mesh_point_count;
  ON_3fVector* fN = mesh->m_N.Reserve(mesh_point_count);
  mesh->m_N.SetCount(mesh_point_count);
  while (dN < dN1)
    *fN++ = *dN++;

  // set bounding boxes
  mesh->BoundingBox();

  // group all mesh faces that came from a subd control net face into an ngon.
  for (;;)
  {
    const ON_3dPointListRef mesh_vertex_list(mesh);
    const ON_MeshFaceList mesh_face_list(mesh);
    ON_MeshVertexFaceMap vf_map;
    if (!vf_map.SetFromFaceList(mesh_vertex_list.PointCount(), mesh_face_list, false))
      break;
    const unsigned int *const* vertex_face_map = vf_map.VertexFaceMap();
    if (nullptr == vertex_face_map)
      break;

    ON_SimpleArray< unsigned int > mesh_face_index_buffer;
    mesh_face_index_buffer.Reserve(mesh_face_count);
    mesh_face_index_buffer.SetCount(mesh_face_count);
    unsigned int* mesh_face_index = mesh_face_index_buffer.Array();

    // mesh_face_index[] = permutation of (0,1,...,quad_count-1)
    //   such that the mesh faces that came from the same level zero subd face
    //   are grouped together.
    ON_Sort(ON::sort_algorithm::quick_sort, mesh_face_index, quad_group_ids, mesh_face_count, sizeof(quad_group_ids[0]), CompareQuadGroupId);

    ON_SimpleArray< unsigned int> ngon_fi(256);
    ON_SimpleArray< unsigned int> ngon_vi;
    ON_ProgressStepCounter counter = ON_ProgressStepCounter::Create(
      local_limit_mesh_parameters.m_progress_reporter,
      mesh_face_count,
      0.5, 1.0,
      50
      );
    for (unsigned int i = 0; i < mesh_face_count; /*empty increment*/)
    {
      // get list of faces in the mesh ngon
      ngon_fi.SetCount(0);
      ngon_fi.Append(mesh_face_index[i]);
      const ON__UINT_PTR ngon_group_id = quad_group_ids[mesh_face_index[i]];
      for (i++; i < mesh_face_count && ngon_group_id == quad_group_ids[mesh_face_index[i]]; i++)
      {
        ngon_fi.Append(mesh_face_index[i]);
      }

      counter.IncrementStep();
      if (ngon_fi.Count() < 2)
        continue;

      // create ngon
      ngon_vi.SetCount(0);
      if (ON_MeshNgon::FindNgonOuterBoundary(mesh_vertex_list, mesh_face_list, vertex_face_map, ngon_fi.UnsignedCount(), ngon_fi.Array(), ngon_vi) >= 3)
      {
        mesh->AddNgon(ngon_vi.UnsignedCount(), ngon_vi.Array(), ngon_fi.UnsignedCount(), ngon_fi.Array());
      }
    }
    break;
  }

  // success
  ON_ProgressReporter::ReportProgress(local_limit_mesh_parameters.m_progress_reporter,1.0);
  up.release();
  return mesh;
}

class VertexToDuplicate
{
public:
  const ON_SubDVertex* m_vertex = nullptr;
  const ON_SubDFace* m_face = nullptr;
  unsigned int m_mesh_V_index = 0;
  unsigned int m_mesh_F_index = 0;

  static int CompareVertexId(const class VertexToDuplicate* a, const class VertexToDuplicate*);
  static int CompareVertexAndFaceIds(const class VertexToDuplicate* a, const class VertexToDuplicate*);

  static bool NeedsDuplicated(
    const ON_SubDVertex* vertex
    );
};

int VertexToDuplicate::CompareVertexId(const class VertexToDuplicate* a, const class VertexToDuplicate* b)
{
  if ( a == b )
    return 0;
  if ( nullptr == a )
    return -1;
  if ( nullptr == b )
    return 1;

  unsigned int a_id = a->m_vertex ? a->m_vertex->m_id : 0;
  unsigned int b_id = b->m_vertex ? b->m_vertex->m_id : 0;
  if ( a_id < b_id )
    return -1;
  if ( a_id > b_id )
    return 1;

  return 0;
}
int VertexToDuplicate::CompareVertexAndFaceIds(const class VertexToDuplicate* a, const class VertexToDuplicate* b)
{
  if ( a == b )
    return 0;
  int rc = VertexToDuplicate::CompareVertexId(a,b);
  if (0 != rc)
    return rc;
  if (nullptr == a)
    return -1;
  if (nullptr == b)
    return 1;
  unsigned int a_id = a->m_face ? a->m_face->m_id : 0;
  unsigned int b_id = b->m_face ? b->m_face->m_id : 0;
  if (a_id < b_id)
    return -1;
  if (a_id > b_id)
    return 1;
  return 0;
}

bool VertexToDuplicate::NeedsDuplicated(
  const ON_SubDVertex* vertex
  )
{
  if ( nullptr == vertex || vertex->m_face_count <= 0 || vertex->m_edge_count < 2 || nullptr == vertex->m_edges )
    return false;
  if (vertex->IsSmooth())
    return false;
  const unsigned int edge_count = vertex->m_edge_count;
  for (unsigned int vei = 0; vei < edge_count; vei++)
  {
    const ON_SubDEdge* edge = vertex->Edge(vei);
    if ( nullptr != edge && false == edge->IsSmooth(true) && edge->m_face_count > 1 )
      return true;
  }
  return false;
}

static bool ChangeMeshFaceIndex(
  unsigned int mesh_V_index0,
  unsigned int mesh_F_count,
  ON_Mesh* mesh,
  VertexToDuplicate& dup,
  ON_SimpleArray<VertexToDuplicate>& dups_sub_array
  )
{
  int k = dups_sub_array.BinarySearch(&dup,VertexToDuplicate::CompareVertexAndFaceIds);
  if (k < 0)
  {
    // error.  terminate creation of dups.
    ON_SubDIncrementErrorCount();
    return false;
  }

  VertexToDuplicate* dupk = dups_sub_array.Array() + k;

  if (mesh_V_index0 != dup.m_mesh_V_index)
  {
    if (mesh_V_index0 == dupk->m_mesh_V_index && dupk->m_mesh_F_index < mesh_F_count)
    {
      unsigned int* fvi = (unsigned int*)(mesh->m_F[dupk->m_mesh_F_index].vi);
      if (fvi[0] == mesh_V_index0)
        fvi[0] = dup.m_mesh_V_index;
      if (fvi[1] == mesh_V_index0)
        fvi[1] = dup.m_mesh_V_index;
      if (fvi[2] == mesh_V_index0)
        fvi[2] = dup.m_mesh_V_index;
      if (fvi[3] == mesh_V_index0)
        fvi[3] = dup.m_mesh_V_index;
    }
  }
  dupk->m_mesh_V_index = ON_UNSET_UINT_INDEX;
  dupk->m_mesh_F_index = ON_UNSET_UINT_INDEX;
  return true;
}

static bool DuplicateVerticesAtCreases(
  ON_Mesh* mesh,
  ON_3dPointArray& D,
  ON_SimpleArray<VertexToDuplicate>& dups_array
  )
{
  const unsigned int mesh_F_count = mesh->m_F.UnsignedCount();
  const unsigned int mesh_D_count0 = D.UnsignedCount();

  const unsigned int dups_count = dups_array.UnsignedCount();
  if (dups_count <= 1)
    return true;

  dups_array.QuickSort(VertexToDuplicate::CompareVertexAndFaceIds);
  ON_SimpleArray<VertexToDuplicate> dups_sub_array; // for searching
  VertexToDuplicate* dups = dups_array;
  VertexToDuplicate dup;
  unsigned int i1 = 0;
  for (unsigned int i0 = i1; i0 < dups_count; i0 = i1)
  {
    dup = dups[i0];
    if (nullptr == dup.m_vertex)
    {
      ON_SubDIncrementErrorCount();
      return false;
    }
    for (i1 = i0 + 1; i1 < dups_count; i1++)
    {
      int rc = VertexToDuplicate::CompareVertexId(&dup,dups+i1);
      if (rc < 0)
        break;
      if ( 0 != rc
          || dup.m_vertex != dups[i1].m_vertex 
          || dup.m_mesh_V_index != dups[i1].m_mesh_V_index 
          || dup.m_mesh_V_index >= mesh_D_count0
          )
      {
        ON_SubDIncrementErrorCount();
        return false;
      }
    }

    if ( i1 == i0+1)
      continue;

    const unsigned int mesh_V_index0 = dup.m_mesh_V_index;
    const ON_3dPoint P = D[mesh_V_index0];
    dups_sub_array.SetArray(dups+i0,i1-i0,0);
    ON_SubDSectorIterator sit;
    unsigned int sector_count = 0;
    bool bDupError = false;
    for (unsigned int i = i0; i < i1 && false == bDupError; i++)
    {
      if (dups[i].m_mesh_V_index >= mesh_D_count0 || dups[i].m_mesh_F_index >= mesh_F_count)
      {
        if (sector_count > 0
          && ON_UNSET_UINT_INDEX == dups[i].m_mesh_V_index
          && ON_UNSET_UINT_INDEX == dups[i].m_mesh_F_index
          )
        {
          // this dup[i] was part of a previously processed sector.
          continue;
        }
        // error.  terminate creation of dups.
        ON_SubDIncrementErrorCount();
        bDupError = true;
        break;
      }

      if (nullptr == sit.Initialize(dups[i].m_face, 0, dup.m_vertex))
      {
        // error.  terminate creation of dups.
        ON_SubDIncrementErrorCount();
        bDupError = true;
        break;
      }
      if ( nullptr == sit.IncrementToCrease(-1) )
      {
        // error.  terminate creation of dups.
        ON_SubDIncrementErrorCount();
        bDupError = true;
        break;
      }

      if (dup.m_vertex->IsDart())
      {
        const ON_SubDEdge* edge = sit.CurrentEdge(0);
        if (nullptr == edge || false == edge->IsCrease(false) || 2 != edge->m_face_count)
        {
          ON_SubDIncrementErrorCount();
          bDupError = true;
          break;
        }
        for (unsigned int efi = 0; efi < 2; efi++)
        {
          dup.m_face = edge->Face(efi);
          dup.m_mesh_V_index = D.UnsignedCount();
          D.Append(P);
          if (false == ChangeMeshFaceIndex(mesh_V_index0, mesh_F_count, mesh, dup, dups_sub_array))
          {
            ON_SubDIncrementErrorCount();
            bDupError = true;
            break;
          }
        }

        sit.NextFace(true);
      }

      sector_count++;
      if (sector_count > 1)
      {
        dup.m_mesh_V_index = D.UnsignedCount();
        D.Append(P);
      }
      else
      {
        dup.m_mesh_V_index = mesh_V_index0;
      }

      for (dup.m_face = sit.CurrentFace(); nullptr != dup.m_face && false == bDupError; dup.m_face = sit.NextFace(true))
      {
        if (false == ChangeMeshFaceIndex(mesh_V_index0, mesh_F_count, mesh, dup, dups_sub_array))
        {
          ON_SubDIncrementErrorCount();
          bDupError = true;
          break;
        }
      }
      if (bDupError)
        break;

    }
    dups_sub_array.SetCapacity(0);
    if (bDupError)
      return false;
  }
  return true;
}



ON_Mesh* ON_SubD::GetControlNetMesh(
  ON_Mesh* destination_mesh
  ) const
{
  if (destination_mesh)
    destination_mesh->Destroy();

  const ON_SubDLevel& level = ActiveLevel();
  if (level.IsEmpty())
    return ON_SUBD_RETURN_ERROR(nullptr);

  VertexToDuplicate dup;
  ON_SimpleArray<VertexToDuplicate> dups_array;

  const ON_SubDimple* subdimple = SubDimple();
  if ( nullptr == subdimple)
    return nullptr;

  const unsigned int subd_vertex_count = level.m_vertex_count;

  unsigned int mesh_ngon_count = 0;
  unsigned int mesh_face_count = 0;
  unsigned int max_ngon_Vcount = 0;
  for (const ON_SubDFace* face = level.m_face[0]; nullptr != face; face = face->m_next_face)
  {
    if ( face->m_edge_count < 2 )
      continue;
    if (face->m_edge_count <= 4)
    {
      mesh_face_count++;
      continue;
    }
    mesh_ngon_count++;
    mesh_face_count += face->m_edge_count;
    if ( max_ngon_Vcount < face->m_edge_count )
      max_ngon_Vcount = face->m_edge_count;
  }

  if (subd_vertex_count < 4 || mesh_face_count < 1 )
    return ON_SUBD_RETURN_ERROR(nullptr);

  std::unique_ptr< ON_Mesh > up;  
  ON_Mesh* mesh = nullptr;
  if (nullptr != destination_mesh)
    mesh = destination_mesh;
  else
  {
    up = std::make_unique< ON_Mesh >();
    mesh = up.get();
  }

  ON_3dPointArray& D = mesh->DoublePrecisionVertices();
  D.Reserve(subd_vertex_count+mesh_ngon_count);
  D.SetCount(0);

  mesh->m_F.Reserve(mesh_face_count);
  mesh->m_F.SetCount(0);

  ON_SimpleArray< ON_2udex > ngon_spans(mesh_ngon_count);

  bool rc = false;
  for (;;)
  {

    unsigned int archive_id_partition[4] = {};
    level.SetArchiveId(archive_id_partition);
    if (archive_id_partition[1] - archive_id_partition[0] != subd_vertex_count)
      break;

    for (const ON_SubDVertex* vertex = level.m_vertex[0]; nullptr != vertex; vertex = vertex->m_next_vertex)
    {
      unsigned int vi = vertex->ArchiveId();
      if (vi < 1 || vi > subd_vertex_count)
        break;
      if (D.UnsignedCount()+1 != vi)
        break;
      D.AppendNew() = vertex->m_P;
    }

    if (D.UnsignedCount() != subd_vertex_count)
      break;

    ngon_spans.Reserve(mesh_ngon_count);
    unsigned int max_ngon_face_count = 0;
    mesh_face_count = 0;
    for (const ON_SubDFace* face = level.m_face[0]; nullptr != face; face = face->m_next_face)
    {
      ON_MeshFace meshf = {};

      if (face->m_edge_count <= 4)
      {
        if (face->m_edge_count < 3)
          continue;

        for (unsigned short fvi = 0; fvi < face->m_edge_count; fvi++)
        {
          const ON_SubDVertex* vertex = face->Vertex(fvi);
          meshf.vi[fvi] = (int)((nullptr != vertex) ? vertex->ArchiveId() : 0U);
          if (meshf.vi[fvi] < 1 || meshf.vi[fvi] > (int)subd_vertex_count)
          {
            meshf.vi[0] = -1;
            break;
          }
          meshf.vi[fvi]--;
          if (VertexToDuplicate::NeedsDuplicated(vertex))
          {
            dup.m_vertex = vertex;
            dup.m_face = face;
            dup.m_mesh_F_index = mesh->m_F.UnsignedCount();
            dup.m_mesh_V_index = meshf.vi[fvi];
            dups_array.Append(dup);
          }
        }
        if (-1 == meshf.vi[0] )
          continue;
        if ( 3 == face->m_edge_count)
          meshf.vi[3] = meshf.vi[2];
        mesh->m_F.Append(meshf);
        continue;
      }
      else
      {
        ON_3dPoint center_point;
        if (false == face->GetSubdivisionPoint(ON_SubD::SubDType::QuadCatmullClark, true, center_point))
          continue;

        ON_2udex ngon_span = { mesh->m_F.UnsignedCount(), 0 };

        const unsigned int dup_count0 = dups_array.UnsignedCount();

        const unsigned int Dcount0 = D.UnsignedCount();
        const unsigned int Fcount0 = mesh->m_F.UnsignedCount();
        meshf.vi[2] = (int)Dcount0;
        meshf.vi[3] = meshf.vi[2];

        const ON_SubDVertex* vertex = face->Vertex(0);
        meshf.vi[1] = (nullptr != vertex) ? vertex->ArchiveId() : 0;
        if (meshf.vi[1] < 1 || meshf.vi[1] >= (int)subd_vertex_count)
          continue;
        meshf.vi[1]--;

        if (VertexToDuplicate::NeedsDuplicated(vertex))
        {
          dup.m_vertex = vertex;
          dup.m_face = face;
          dup.m_mesh_F_index = mesh->m_F.UnsignedCount();
          dup.m_mesh_V_index = meshf.vi[1];
          dups_array.Append(dup);
        }

        D.Append(center_point);

        for (unsigned short fvi = 1; fvi <= face->m_edge_count; fvi++)
        {
          meshf.vi[0] = meshf.vi[1];
          vertex = face->Vertex(fvi % face->m_edge_count);
          meshf.vi[1] = (int)((nullptr != vertex) ? vertex->ArchiveId() : 0U);
          if (meshf.vi[1] < 1 || meshf.vi[1] > (int)subd_vertex_count)
          {
            meshf.vi[0] = -1;
            break;
          }
          meshf.vi[1]--;

          if (VertexToDuplicate::NeedsDuplicated(vertex))
          {
            dup.m_vertex = vertex;
            dup.m_face = face;
            dup.m_mesh_F_index = mesh->m_F.UnsignedCount();
            dup.m_mesh_V_index = meshf.vi[1];
            dups_array.Append(dup);
          }

          mesh->m_F.Append(meshf);
        }
        ngon_span.j = mesh->m_F.UnsignedCount();

        unsigned int ngon_face_count = ngon_span.j - ngon_span.i;
        if (-1 == meshf.vi[0] || ngon_face_count < 3)
        {
          D.SetCount(Dcount0);
          mesh->m_F.SetCount(Fcount0);
          dups_array.SetCount(dup_count0);
          continue;
        }
        ngon_span.j = mesh->m_F.UnsignedCount();
        if (ngon_face_count >= 3)
        {
          ngon_spans.Append(ngon_span);
          if ( ngon_face_count > max_ngon_face_count)
            max_ngon_face_count = ngon_face_count;
        }
      }
    }

    if (mesh->m_F.UnsignedCount() <= 0)
      break;

    rc = true;
    break;
  }

  level.ClearArchiveId();
  if (false == rc )
    return ON_SUBD_RETURN_ERROR(nullptr);

  if (D.UnsignedCount() < 3 || mesh->m_F.UnsignedCount() < 1)
    return ON_SUBD_RETURN_ERROR(nullptr);

  DuplicateVerticesAtCreases(mesh,D,dups_array);
  mesh->UpdateSinglePrecisionVertices();
  mesh->ComputeFaceNormals();
  mesh->ComputeVertexNormals();
  mesh->BoundingBox();

  // group all mesh faces that came from the same level zero subd face into an ngon.
  if (ngon_spans.UnsignedCount() > 0 && max_ngon_Vcount >= 3)
  {
    ON_SimpleArray< unsigned int> ngon_buffer;
    unsigned int* ngon_fi = ngon_buffer.Reserve(2*max_ngon_Vcount);
    unsigned int* ngon_vi = ngon_fi + max_ngon_Vcount;
    for (unsigned int ngon_dex = 0; ngon_dex < ngon_spans.UnsignedCount(); ngon_dex++ )
    {
      ON_2udex ngon_span = ngon_spans[ngon_dex];
      unsigned int Fcount = ngon_span.j-ngon_span.i;
      if ( Fcount < 3)
        continue;

      ngon_fi[0] = ngon_span.i;
      ngon_fi[0] = (unsigned int)mesh->m_F[ngon_fi[0]].vi[0];

      unsigned int ngon_Vcount = 0;
      for (unsigned int i = ngon_span.i; i < ngon_span.j; i++)
      {
        ngon_fi[ngon_Vcount] = i;
        ngon_vi[ngon_Vcount] = (unsigned int)(mesh->m_F[i].vi[0]);
        ngon_Vcount++;
      }
      mesh->AddNgon(ngon_Vcount, ngon_vi, ngon_Vcount, ngon_fi );
    }
  }

  up.release();
  return mesh;
}

double* ON_SubDQuadFaceMesher::Internal_Buffer(size_t buffer_capacity)
{
  if (buffer_capacity < m__buffer_capacity)
    return m__buffer;
  if (nullptr != m__buffer)
  {
    delete[] m__buffer;
    m__buffer = nullptr;
  }
  m__buffer_capacity = 0;
  m__buffer = new(std::nothrow) double[buffer_capacity];
  if (nullptr != m__buffer)
    m__buffer_capacity = buffer_capacity;
  return m__buffer;
}

void ON_SubDQuadFaceMesher::SetDestinationInitialize(
  ON_SubDQuadFaceMesher::Output output
  )
{
  // Reuse workspaces
  ReturnAllFixedSizeHeaps();

  m_output = output;
  m_display_density = 0;
  m_count = 0;
  m_capacity = 0;
  m_point_stride0 = 0;
  m_point_stride1 = 0;
  m_points = nullptr;
  m_normal_stride0 = 0;
  m_normal_stride1 = 0;
  m_normals = nullptr;
  m_patcher = nullptr;
}

bool ON_SubDQuadFaceMesher::SetDestinationToLocalMeshBuffer(
  unsigned int mesh_density
  )
{
  const unsigned int count = ON_SubDLimitMeshFragment::SideSegmentCountFromDisplayDensity(mesh_density);
  
  const size_t point_count = (count > 0) ? ((count + 1)*(count + 1)) : 0;

  // initialize this to make another mesh.
  SetDestinationInitialize(ON_SubDQuadFaceMesher::Output::mesh);

  double* buffer = Internal_Buffer(6 * point_count);
  if (point_count > 0 && nullptr == buffer )
  {
    return ON_SUBD_RETURN_ERROR(false);
  }
  
  if (0 == count && mesh_density > 0)
    return ON_SUBD_RETURN_ERROR(false);

  m_points = buffer;
  m_normals = buffer + 3*point_count;
  m_point_stride0 = 3;
  m_point_stride1 = (count+1)*m_point_stride0;
  m_normal_stride0 = m_point_stride0;
  m_normal_stride1 = m_point_stride1;

  m_display_density = mesh_density;
  m_count = 0;
  m_capacity = count;

  return (count == m_count);
}

bool ON_SubDQuadFaceMesher::SetDestinationToMeshFragment(
  unsigned int mesh_density,
  class ON_SubDLimitMeshFragment& fragment
  )
{
  // initialize this
  SetDestinationInitialize(ON_SubDQuadFaceMesher::Output::mesh);
  
  if ( nullptr == fragment.m_P || nullptr == fragment.m_N)
    return ON_SUBD_RETURN_ERROR(false);
  
  const unsigned int count = ON_SubDLimitMeshFragment::SideSegmentCountFromDisplayDensity(mesh_density);
  const unsigned int point_count = (count + 1)*(count + 1);
  if ( point_count > fragment.m_P_capacity )
    return ON_SUBD_RETURN_ERROR(false);

  const unsigned int quad_count = count*count;
  if (fragment.m_grid.m_F_count != quad_count || 0 != fragment.m_grid.m_F_level_of_detail 
    || nullptr == fragment.m_grid.m_F || fragment.m_grid.m_F_stride != 4)
  {
    fragment.m_grid = ON_SubDLimitMeshFragmentGrid::Quads(count,0);
    if ( nullptr == fragment.m_grid.m_F)
      return ON_SUBD_RETURN_ERROR(false);
  }

  m_point_stride0 = fragment.m_P_stride;
  m_point_stride1 = (count+1)*m_point_stride0;
  m_points = fragment.m_P;

  m_normal_stride0 = fragment.m_N_stride;
  m_normal_stride1 = (count+1)*m_normal_stride0;
  m_normals = fragment.m_N;

  m_display_density = mesh_density;
  m_count = 0;
  m_capacity = count;

  return true;
}

bool ON_SubDQuadFaceMesher::SetDestinationToPatchFragment(
  class ON_SubDQuadFacePatcher& patcher
  )
{
  // initialize this
  SetDestinationInitialize(ON_SubDQuadFaceMesher::Output::patches);
  m_patcher = &patcher;
  m_display_density = patcher.m_display_density;
  
  return true;
}


bool ON_SubDQuadFaceMesher::UnsetMeshPoints()
{
  if (ON_SubDQuadFaceMesher::Output::mesh != m_output)
    return ON_SUBD_RETURN_ERROR(false);

  if (nullptr == m_points || 0 == m_count)
    return ON_SUBD_RETURN_ERROR(false);

  double* p1end = m_points + (m_count + 1)*m_point_stride1;
  for (double* p1 = m_points; p1 < p1end; p1 += m_point_stride1)
  {
    double* pend = p1 + (m_count + 1)*m_point_stride0;
    for (double* p = p1; p < pend; p += m_point_stride0)
      *p = ON_UNSET_VALUE;
  }

  return true;
}

void ON_SubDQuadFaceMesher::Get6xCubicBasis(
  double t,
  double b6[4]
  )
{
  const double knot[6] = {-2.0,-1.0,0.0,1.0,2.0,3.0};
  double N[16];
  ON_EvaluateNurbsBasis( 4, knot, t, N );
  b6[0] = 6.0*N[0];
  b6[1] = 6.0*N[1];
  b6[2] = 6.0*N[2];
  b6[3] = 6.0*N[3];

  // TODO - hard code polynomial formula
  //const double tt = t*t;
  //const double s = 1.0 - t;
  //const double ss = s*s;
 
  //b6[0] = (t*tt);
  //b6[1] = (1.0 + 3.0*(s*tt));
  //b6[2] = (1.0 + 3.0*(t*ss));
  //b6[3] = (s*ss);
}


void ON_SubDQuadFaceMesher::GetCubicBasisDerivative(
  double t,
  double d[4]
  )
{
  const double knot[6] = {-2.0,-1.0,0.0,1.0,2.0,3.0};
  double N[16];
  ON_EvaluateNurbsBasis( 4, knot, t, N );
  ON_EvaluateNurbsBasisDerivatives( 4, knot, 1, N );
  d[0] = N[4];
  d[1] = N[5];
  d[2] = N[6];
  d[3] = N[7];

  // TODO - hard code polynomial formula
  //const double tt = t*t;
  //const double s = 1.0 - t;
  //const double ss = s*s;

  //d[0] = tt;
  //d[1] =  (0.5 + t) - 1.5*tt;
  //d[2] = -(0.5 + s) + 1.5*ss;
  //d[3] = -ss;
}

bool ON_SubDQuadFaceMesher::Internal_EvaluateSurfaceNormalBackup1(
  double s,
  double t,
  unsigned int count,
  unsigned int i,
  unsigned int j,
  double* N
) const
{
  const double knot[6] = {-2.0,-1.0,0.0,1.0,2.0,3.0};
  ON_3dVector v[6]; // P, Du, Dv, Duu, Duv, Dvv
  bool bHaveNormal = ON_EvaluateNurbsSurfaceSpan(
        3,    // dim
        0,    // is_rat
        4, 4, // order, order
        knot, // knot0[] array of (2*order0-2) doubles
        knot, // knot1[] array of (2*order1-2) doubles
        12,3, // cv_stride0, cv_stride1
        &m_srf_cv[0][0][0],// cv at "lower left" of bispan
        2,    // der_count,                      // number of derivatives to compute (>=0)
        s,t,  // evaluation parameters
        3,    // v_stride
        &v[0][0] // P, Du, Dv, Duu, Duv, Dvv returned here
        );

  if (bHaveNormal)
  {
    // P[], Du, Dv and V[0], V[1], V[2] should be nearly identical
    int limit_dir = 0;
    if (0 == i)
    {
      limit_dir = (j < count) ? 1 : 2;
    }
    else if (count == i)
    {
      limit_dir = (j < count) ? 4 : 3;
    }
    if (0 == j)
    {
      limit_dir = 1;
    }
    else if (count == j)
    {
      limit_dir = 4;
    }
    bHaveNormal = ON_EvNormal(limit_dir, v[1], v[2], v[3], v[4], v[5], *((ON_3dVector*)N));
  }

  return bHaveNormal;
}

bool ON_SubDQuadFaceMesher::Internal_EvaluateSurfaceNormalBackup2(
  const double* P00,
  unsigned int count,
  unsigned int i,
  unsigned int j,
  double* N
) const
{
  if  (false == ((const ON_3dPoint*)(P00 + ((i * m_point_stride0) + (j * m_point_stride1))))->IsValid() )
    return false;

  ON_2dex corners_dex[4];
  ON_3dPoint corners[4];
  if ( i <= 0 || i >= count || j <= 0 || j >= count )
  {
    corners_dex[0].i = (i > 0) ? (i - 1) : 0;
    corners_dex[0].j = (j > 0) ? (j - 1) : 0;
    corners_dex[2].i = (i < count) ? (i + 1) : count;
    corners_dex[2].j = (j < count) ? (j + 1) : count;
    corners_dex[1].i = corners_dex[2].i;
    corners_dex[1].j = corners_dex[0].j;
    corners_dex[3].i = corners_dex[0].i;
    corners_dex[3].j = corners_dex[2].j;
  }
  else
  {
    corners_dex[0].i = (i - 1);
    corners_dex[0].j = j;
    corners_dex[1].i = i;
    corners_dex[1].j = (j - 1);
    corners_dex[2].i = (i + 1);
    corners_dex[2].j = j;
    corners_dex[3].i = i;
    corners_dex[3].j = (j + 1);
  }

  for (int k = 0; k < 4; k++)
  {
    corners[k] = P00 + ((corners_dex[k].i * m_point_stride0) + (corners_dex[k].j * m_point_stride1));
    if (false == corners[k].IsValid())
      return false;
  }
  const ON_3dVector A (corners[2] - corners[0]);
  const ON_3dVector B (corners[3] - corners[1]);
  *((ON_3dVector*)N) = ON_CrossProduct(A, B);
  bool bHaveNormal = ((ON_3dVector*)N)->Unitize();
  return bHaveNormal;
}

bool ON_SubDQuadFaceMesher::EvaluateSurface(
  unsigned int count,
  unsigned int point_i0,
  unsigned int point_j0
  ) const
{
  double iso_cv[4][3];
  double b[4], Du[3], Dv[3], s, t;
  unsigned int i, j;

  if ( nullptr == m_points || m_point_stride0 < 3 || m_point_stride1 < 3)
    return ON_SUBD_RETURN_ERROR(false);

  // verify that count is a power of 2
  for ( i = count; i > 1; i /= 2)
  {
    if (0 != (i%2))
      return ON_SUBD_RETURN_ERROR(false); // count != 2^n
  }
  if (1 != i)
    return ON_SUBD_RETURN_ERROR(false); // count != 2^n
  
  double* P00 = m_points + point_i0*m_point_stride0 + point_j0*m_point_stride1;

  const size_t normal_stride[2] = {(nullptr != m_normals)?m_normal_stride0:0,(nullptr != m_normals)?m_normal_stride1:0};
  double* N00 = m_normals + point_i0*normal_stride[0] + point_j0*normal_stride[1];

  const double delta_t = 1.0 / ((double)count);

  if (nullptr != N00)
  {
    for (i = 0; i <= count; i++)
    {

      // Set  iso_cv[][] = cvs for isocurve(s) = srf(i*delta_t,s)
      s = (i < count) ? (i*delta_t) : 1.0;
      ON_SubDQuadFaceMesher::Get6xCubicBasis(s, b);
      const double* srf = &m_srf_cv[0][0][0];
      iso_cv[0][0] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[0][1] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[0][2] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[1][0] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[1][1] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[1][2] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[2][0] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[2][1] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[2][2] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[3][0] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[3][1] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;
      srf++;
      iso_cv[3][2] = ((b[0] * srf[0] + b[3] * srf[36]) + (b[1] * srf[12] + b[2] * srf[24])) / 6.0;

      double* P = P00 + (i * m_point_stride0);
      double* N = N00 + (i * normal_stride[0]);
      for (j = 0; j <= count; j++)
      {
        if (ON_UNSET_VALUE == P[0])
        {
          // evaluate Dv(i*delta_t,j*delta_t) and save it in N[0,1,2]
          t = (j < count) ? (j*delta_t) : 1.0;
          ON_SubDQuadFaceMesher::GetCubicBasisDerivative(t, b);
          N[0] = ((b[0] * iso_cv[0][0] + b[3] * iso_cv[3][0]) + (b[1] * iso_cv[1][0] + b[2] * iso_cv[2][0]));
          N[1] = ((b[0] * iso_cv[0][1] + b[3] * iso_cv[3][1]) + (b[1] * iso_cv[1][1] + b[2] * iso_cv[2][1]));
          N[2] = ((b[0] * iso_cv[0][2] + b[3] * iso_cv[3][2]) + (b[1] * iso_cv[1][2] + b[2] * iso_cv[2][2]));
        }
        P += m_point_stride1;
        N += normal_stride[1];
      }
    }
  }

  bool bUseSurfaceNormalBackup2 = false;

  for (j = 0; j <= count; j++)
  {
    // Set  iso_cv[][] = cvs for isocurve(s) = srf(s,j*delta_t)
    t = (j < count) ? (j*delta_t) : 1.0;
    ON_SubDQuadFaceMesher::Get6xCubicBasis(t, b);
    const double* srf = &m_srf_cv[0][0][0];
    // The ((first + fourth) + (second + third)) insures the values of the 
    // mesh points and normals will be exactly symmetric when the control points are exactly
    // symmetric.
    iso_cv[0][0] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[0][1] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[0][2] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf = &m_srf_cv[1][0][0];
    iso_cv[1][0] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[1][1] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[1][2] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf = &m_srf_cv[2][0][0];
    iso_cv[2][0] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[2][1] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[2][2] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf = &m_srf_cv[3][0][0];
    iso_cv[3][0] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[3][1] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;
    srf++;
    iso_cv[3][2] = ((b[0] * srf[0] + b[3] * srf[9]) + (b[1] * srf[3] + b[2] * srf[6])) / 6.0;

    double* P = P00 + (j * m_point_stride1);
    double* N = N00 + (j * normal_stride[1]);
    for (i = 0; i <= count; i++)
    {
      if (ON_UNSET_VALUE == P[0])
      {
        s = (i < count) ? (i*delta_t) : 1.0;
        ON_SubDQuadFaceMesher::Get6xCubicBasis(s, b);
        // P = srf(i*delta_t,j*delta_t)
        P[0] = ((b[0] * iso_cv[0][0] + b[3] * iso_cv[3][0]) + (b[1] * iso_cv[1][0] + b[2] * iso_cv[2][0])) / 6.0;
        P[1] = ((b[0] * iso_cv[0][1] + b[3] * iso_cv[3][1]) + (b[1] * iso_cv[1][1] + b[2] * iso_cv[2][1])) / 6.0;
        P[2] = ((b[0] * iso_cv[0][2] + b[3] * iso_cv[3][2]) + (b[1] * iso_cv[1][2] + b[2] * iso_cv[2][2])) / 6.0;

        if (nullptr != N)
        {
          // Above, I saved Dv in N[];
          Dv[0] = N[0];
          Dv[1] = N[1];
          Dv[2] = N[2];

          ON_SubDQuadFaceMesher::GetCubicBasisDerivative(s, b);
          // evaluate = Du(i*delta_t,j*delta_t)
          Du[0] = ((b[0] * iso_cv[0][0] + b[3] * iso_cv[3][0]) + (b[1] * iso_cv[1][0] + b[2] * iso_cv[2][0]));
          Du[1] = ((b[0] * iso_cv[0][1] + b[3] * iso_cv[3][1]) + (b[1] * iso_cv[1][1] + b[2] * iso_cv[2][1]));
          Du[2] = ((b[0] * iso_cv[0][2] + b[3] * iso_cv[3][2]) + (b[1] * iso_cv[1][2] + b[2] * iso_cv[2][2]));

          // surface normal = Du X Dv
          *((ON_3dVector*)N) = ON_CrossProduct(Du, Dv);
          bool bHaveNormal = ((ON_3dVector*)N)->Unitize();
          if (false == bHaveNormal)
          {
            bHaveNormal = Internal_EvaluateSurfaceNormalBackup1(s,t,count,i,j,N);
            if (false == bHaveNormal)
            {
              bUseSurfaceNormalBackup2 = true;
              N[0] = 0.0;
              N[1] = 0.0;
              N[2] = 0.0;
            }
          }
        }
      }
      P += m_point_stride0;
      N += normal_stride[0];
    }
  }

  if (bUseSurfaceNormalBackup2)
  {
    for (j = 0; j <= count; j++)
    {
      double* N = N00 + (j * normal_stride[1]);
      for (i = 0; i <= count; i++)
      {
        if (0.0 == N[0] && 0.0 == N[1] && 0.0 == N[2] )
        {
          if ( false == Internal_EvaluateSurfaceNormalBackup2(P00,count,i,j,N) )
          {
            bUseSurfaceNormalBackup2 = true;
            ON_ERROR("Unable to get surface normal.");
            N[0] = 0.0;
            N[1] = 0.0;
            N[2] = 0.0;
          }
        }
        N += normal_stride[0];
      }
    }
  }
 
  return true;
}

bool ON_SubDQuadFacePatcher::SendSinglePatch(
  unsigned int display_density,
  const class ON_SubDQuadFaceSubdivisionCounter& quad_face_subdivision_counter,
  ON_SubDLimitPatchFragment::PatchType patch_type
  )
{
  m_patch_fragment.m_patch_level = (unsigned short)(m_display_density - display_density);
  m_patch_fragment.m_patch_type[0] = patch_type;
  m_patch_fragment.m_patch_type[1] = ON_SubDLimitPatchFragment::PatchType::Unset;
  m_patch_fragment.m_patch_type[2] = ON_SubDLimitPatchFragment::PatchType::Unset;
  m_patch_fragment.m_patch_type[3] = ON_SubDLimitPatchFragment::PatchType::Unset;
  m_patch_fragment.m_face_subdivision_count = quad_face_subdivision_counter.m_subdivision_count;
  unsigned int count 
    = (sizeof(m_patch_fragment.m_face_region_index) <= sizeof(quad_face_subdivision_counter.m_corner_index))
    ? (unsigned int)(sizeof(m_patch_fragment.m_face_region_index)/sizeof(m_patch_fragment.m_face_region_index[0]))
    : (unsigned int)(sizeof(quad_face_subdivision_counter.m_corner_index)/sizeof(quad_face_subdivision_counter.m_corner_index[0]));
  if ( ((unsigned int)quad_face_subdivision_counter.m_subdivision_count) < count )
    count = quad_face_subdivision_counter.m_subdivision_count;
  for (unsigned int i = 0; i < count; i++)
  {
    m_patch_fragment.m_face_region_index[i] = quad_face_subdivision_counter.m_corner_index[i];
  }
  return m_fragment_callback_function(m_fragment_callback_context,&m_patch_fragment);
}
  
bool ON_SubDQuadFacePatcher::SendMultiPatch(
  unsigned int display_density,
  const class ON_SubDQuadFaceSubdivisionCounter& quad_face_subdivision_counter,
  const ON_SubDLimitPatchFragment::PatchType patch_type[4]
  )
{
  m_patch_fragment.m_patch_level = (unsigned short)(m_display_density - display_density);
  m_patch_fragment.m_patch_type[0] = patch_type[0];
  m_patch_fragment.m_patch_type[1] = patch_type[1];
  m_patch_fragment.m_patch_type[2] = patch_type[2];
  m_patch_fragment.m_patch_type[3] = patch_type[3];
  m_patch_fragment.m_face_subdivision_count = quad_face_subdivision_counter.m_subdivision_count;
  unsigned int count 
    = (sizeof(m_patch_fragment.m_face_region_index) <= sizeof(quad_face_subdivision_counter.m_corner_index))
    ? (unsigned int)(sizeof(m_patch_fragment.m_face_region_index)/sizeof(m_patch_fragment.m_face_region_index[0]))
    : (unsigned int)(sizeof(quad_face_subdivision_counter.m_corner_index)/sizeof(quad_face_subdivision_counter.m_corner_index[0]));
  if ( ((unsigned int)quad_face_subdivision_counter.m_subdivision_count) < count )
    count = quad_face_subdivision_counter.m_subdivision_count;
  for (unsigned int i = 0; i < count; i++)
  {
    m_patch_fragment.m_face_region_index[i] = quad_face_subdivision_counter.m_corner_index[i];
  }
  return m_fragment_callback_function(m_fragment_callback_context,&m_patch_fragment);
}


bool ON_SubDQuadFaceMesher::GetLimitSubMeshOrPatch(
  class ON_SubDQuadFaceSubdivisionCounter& quad_face_subdivision_counter,
  ON_SubDQuadNeighborhood* qft,
  unsigned int display_density,
  unsigned int point_i0,
  unsigned int point_j0
  )
{
  // count = power of 2 = 2^display_density
  unsigned int count = ON_SubDLimitMeshFragment::SideSegmentCountFromDisplayDensity(display_density);
  if ( 0 == count )
    return ON_SUBD_RETURN_ERROR(false);

  if (nullptr == qft || false == qft->IsSet())
    return ON_SUBD_RETURN_ERROR(false);

  if ( ON_SubDQuadFaceMesher::Output::mesh == m_output && count > m_capacity)
    return ON_SUBD_RETURN_ERROR(false);

  if (qft->m_bIsCubicPatch)
  {
    switch (m_output)
    {
    case ON_SubDQuadFaceMesher::Output::mesh:
      if (false == qft->GetLimitSurfaceCV(&m_srf_cv[0][0][0], 4U))
        return ON_SUBD_RETURN_ERROR(false);
      return EvaluateSurface(count, point_i0, point_j0);
      break;

    case ON_SubDQuadFaceMesher::Output::patches:
      if (false == qft->GetLimitSurfaceCV(&m_patcher->m_patch_fragment.m_patch_cv[0][0][0], 5U))
        return ON_SUBD_RETURN_ERROR(false);
      m_patcher->SendSinglePatch(display_density,quad_face_subdivision_counter,ON_SubDLimitPatchFragment::PatchType::Bicubic);
      return true; // even if callback returns false
      break;
    }
    return ON_SUBD_RETURN_ERROR(false);
  }

  if (count > 1 && display_density > 0)
  {
    count /= 2;

    bool bSubDivide[4] = {};
    unsigned int subdivide_count = 0;
    for (unsigned int q0fvi = 0; q0fvi < 4; q0fvi++)
    {
      if ( qft->m_bExactQuadrantPatch[q0fvi] )
      {
        if (ON_SubDQuadFaceMesher::Output::mesh == m_output)
        {
          // Use the exact patch to calcuate exact mesh vertex and normal values
          if (qft->GetLimitSubSurfaceSinglePatchCV(q0fvi, m_srf_cv))
          {
            unsigned int submesh_point_i0 = point_i0 + ((1 == q0fvi || 2 == q0fvi) ? count : 0);
            unsigned int submesh_point_j0 = point_j0 + ((2 == q0fvi || 3 == q0fvi) ? count : 0);
            if (false == EvaluateSurface(count, submesh_point_i0, submesh_point_j0))
              return ON_SUBD_RETURN_ERROR(false);
            continue;
          }
        }
      }
      else
      {
        // local subdivision is required
        bSubDivide[q0fvi] = true;
        subdivide_count++;
      }
    }

    if (ON_SubDQuadFaceMesher::Output::patches == m_output && subdivide_count < 4 )
    {
      // Get the bicubic NURBS control points for the patches that are exact.
      // (subdivide_count < 4) means there is at least one.
      // These are delivered as a collection to enable merging them into as
      // large a face/patch/... as possible.
      ON_SubDLimitPatchFragment::PatchType pt[4] =
      {
        ON_SubDLimitPatchFragment::PatchType::Unset,
        ON_SubDLimitPatchFragment::PatchType::Unset,
        ON_SubDLimitPatchFragment::PatchType::Unset,
        ON_SubDLimitPatchFragment::PatchType::Unset
      };
      // Harvest any exact patches that are available at this subdivision level
      const bool bEnableApproximatePatch = false;
      unsigned int quadrant_count = qft->GetLimitSubSurfaceMultiPatchCV(
        ON_UNSET_VALUE, 
        bEnableApproximatePatch,
        m_patcher->m_patch_fragment.m_patch_cv,
        pt
        );
      if ( 4 != subdivide_count + quadrant_count )
        return ON_SUBD_RETURN_ERROR(false);
      bool bCallbackResult = m_patcher->SendMultiPatch( display_density, quad_face_subdivision_counter, pt );
      if ( false == bCallbackResult)
        return true;
    }

    for (unsigned int q0fvi = 0; q0fvi < 4; q0fvi++)
    {
      if ( false == bSubDivide[q0fvi])
        continue;
      
      // local subdivision is required
      ON_SubD_FixedSizeHeap* fsh = CheckOutLocalFixedSizeHeap();
      if ( nullptr == fsh )
        return ON_SUBD_RETURN_ERROR(false);
      
      ON_SubDQuadNeighborhood qft1;      
      if (false == qft->Subdivide( q0fvi, *fsh, &qft1 ))
      {
        ReturnLocalFixedSizeHeap(fsh);
        return ON_SUBD_RETURN_ERROR(false);
      }

      subdivide_count--;
      if (0 == subdivide_count)
      {
        // If there is an exraordiary vertex and qft is using one of the
        // m_fsh[] on this class, then the recursion will need the
        // m_fsh the no longer needed qft is using.
        const bool bRetainFixedSizeHeap 
          = nullptr == qft->m_fsh
          || false == ReturnLocalFixedSizeHeap(qft->m_fsh);
        ON_SubDQuadNeighborhood::Clear(qft, bRetainFixedSizeHeap);
      }

      unsigned int submesh_point_i0 = point_i0 + ((1==q0fvi || 2==q0fvi) ? count : 0);
      unsigned int submesh_point_j0 = point_j0 + ((2==q0fvi || 3==q0fvi) ? count : 0);
      quad_face_subdivision_counter.Push(q0fvi);
      bool rc = GetLimitSubMeshOrPatch(quad_face_subdivision_counter, &qft1, display_density-1, submesh_point_i0, submesh_point_j0 );      
      quad_face_subdivision_counter.Pop();
      ReturnLocalFixedSizeHeap(fsh);
      if ( false == rc )
        return ON_SUBD_RETURN_ERROR(false);
    }

    return true;
  }
  
  if (1 == count && 0 == display_density)
  {
    // No more subdivison steps are permitted
    if (ON_SubDQuadFaceMesher::Output::patches == m_output)
    {
      ON_SubDLimitPatchFragment::PatchType pt[4] =
      {
        ON_SubDLimitPatchFragment::PatchType::Unset,
        ON_SubDLimitPatchFragment::PatchType::Unset,
        ON_SubDLimitPatchFragment::PatchType::Unset,
        ON_SubDLimitPatchFragment::PatchType::Unset
      };

      // Harvest whatever patches are available and allow approximate patches to be returned
      // if an appropriate number of subdivisions have been performed
      const bool bEnableApproximatePatch = (qft->m_extraordinary_corner_vertex_count <= 1 && quad_face_subdivision_counter.m_subdivision_count >= 2);
      unsigned int quadrant_count = qft->GetLimitSubSurfaceMultiPatchCV(
        ON_UNSET_VALUE,
        bEnableApproximatePatch,
        m_patcher->m_patch_fragment.m_patch_cv,
        pt
        );

      if (quadrant_count > 0)
      {
        bool bCallbackResult = m_patcher->SendMultiPatch(display_density, quad_face_subdivision_counter, pt);

        if (false == bCallbackResult)
          return true;
      }

      return true;
    }

    // Use limit point evalators to get exact locations and normals
    // for the unset corners of this mesh quad.
    // In a qft, all the vertices have single sectors
    double* P[2][2];
    double* N[2][2];
    ON_SubDSectorLimitPoint limit_point;
    for (unsigned int i = 0; i < 2; i++)
    {
      for (unsigned int j = 0; j < 2; j++)
      {
        P[i][j] = m_points + (point_i0 + i*count)*m_point_stride0 + (point_j0 + j*count)*m_point_stride1;
        if (ON_UNSET_VALUE == P[i][j][0])
        {
          N[i][j] = m_normals + (point_i0 + i*count)*m_normal_stride0 + (point_j0 + j*count)*m_normal_stride1;

          if (false == qft->m_vertex_grid[1 + i][1 + j]->GetLimitPoint(ON_SubD::SubDType::QuadCatmullClark, qft->m_face_grid[1][1], true, limit_point ))
            return ON_SUBD_RETURN_ERROR(false);

          P[i][j][0] = limit_point.m_limitP[0];
          P[i][j][1] = limit_point.m_limitP[1];
          P[i][j][2] = limit_point.m_limitP[2];

          N[i][j][0] = limit_point.m_limitN[0];
          N[i][j][1] = limit_point.m_limitN[1];
          N[i][j][2] = limit_point.m_limitN[2];
        }
      }
    }
    return true;
  }
    
  return ON_SUBD_RETURN_ERROR(false);
}

bool ON_SubDQuadFaceMesher::GetLimitMesh(
  class ON_SubDQuadFaceSubdivisionCounter& quad_face_subdivision_counter,
  const ON_SubDFace* face
  )
{

  ReturnAllFixedSizeHeaps();

  m_count = 0;

  if (ON_SubDQuadFaceMesher::Output::mesh != m_output)
    return ON_SUBD_RETURN_ERROR(false);

  if (nullptr == face || 4 != face->m_edge_count)
    return ON_SUBD_RETURN_ERROR(false);

  unsigned int count = ON_SubDLimitMeshFragment::SideSegmentCountFromDisplayDensity(m_display_density);

  if (0 == count)
  {
    return ON_SUBD_RETURN_ERROR(false);
  }


  if (count > m_capacity)
  {
    return ON_SUBD_RETURN_ERROR(false);
  }

  // Get neighborhood topology information
  ON_SubDQuadNeighborhood qft;
  if (false == qft.Set(face))
    return ON_SUBD_RETURN_ERROR(false);

  // GetLimitSubMesh is recursive.
  m_count = count;
  UnsetMeshPoints();

  return GetLimitSubMeshOrPatch(quad_face_subdivision_counter,&qft,m_display_density,0,0);
}


bool ON_SubDQuadFaceMesher::GetLimitPatches(
  class ON_SubDQuadFaceSubdivisionCounter& quad_face_subdivision_counter,
  const ON_SubDFace* face
  )
{
  ReturnAllFixedSizeHeaps();

  m_count = 0;

  if (ON_SubDQuadFaceMesher::Output::patches != m_output)
    return ON_SUBD_RETURN_ERROR(false);

  if (nullptr == face || 4 != face->m_edge_count)
    return ON_SUBD_RETURN_ERROR(false);

  unsigned int count = ON_SubDLimitMeshFragment::SideSegmentCountFromDisplayDensity(m_display_density);

  if (0 == count)
  {
    return ON_SUBD_RETURN_ERROR(false);
  }

  // Get neighborhood topology information
  ON_SubDQuadNeighborhood qft;
  if (false == qft.Set(face))
    return ON_SUBD_RETURN_ERROR(false);

  // GetLimitSubMesh is recursive.
  return GetLimitSubMeshOrPatch(quad_face_subdivision_counter,&qft,m_display_density,0,0);
}


static bool GetLimitSurfaceInStepsSetup(
  const ON_SubD& subd,
  ON_SubDDisplayParameters& limit_mesh_parameters
  )
{
  const unsigned int level_count = subd.LevelCount();
  if (0 == level_count)
    return ON_SUBD_RETURN_ERROR(false);


  if (1 == level_count && ON_SubD::SubDType::Unset == subd.ActiveLevelSubDType())
    const_cast< ON_SubD& >(subd).SetSubDType(ON_SubD::SubDType::QuadCatmullClark);

  const ON_SubD::SubDType subd_type = subd.ActiveLevelSubDType();
  if (ON_SubD::SubDType::QuadCatmullClark != subd_type)
  {
    // TODO - support tri subd after quad stuff is finished.
    return ON_SUBD_RETURN_ERROR(false);
  }

  limit_mesh_parameters.m_progress_reporter_interval.Intersection(ON_Interval::ZeroToOne);
  if ( false == limit_mesh_parameters.m_progress_reporter_interval.IsIncreasing())
    limit_mesh_parameters.m_progress_reporter_interval = ON_Interval::ZeroToOne;

  return true;
}

unsigned int ON_SubD::GetLimitSurfaceMeshInFragments(
  const class ON_SubDDisplayParameters& limit_mesh_parameters,
  ON__UINT_PTR fragment_callback_context,
  bool(*fragment_callback_function)(ON__UINT_PTR, const class ON_SubDLimitMeshFragment*)
  ) const
{
  ON_SubDDisplayParameters local_limit_mesh_parameters = limit_mesh_parameters;

  if ( false == GetLimitSurfaceInStepsSetup(*this,local_limit_mesh_parameters) )
    return ON_SUBD_RETURN_ERROR(0);


  ON_SubDFaceIterator fit(*this);
  unsigned int fragment_count = GetQuadLimitSurfaceMeshFragmentsHelper(
    fit,
    local_limit_mesh_parameters,
    fragment_callback_context,
    fragment_callback_function
    );

  if ( fragment_count > 0 )
    return fragment_count;

  return ON_SUBD_RETURN_ERROR(0);
}  

unsigned int ON_SubD::GetLimitSurfaceInPatches(
  const class ON_SubDDisplayParameters& limit_mesh_parameters,
  ON__UINT_PTR fragment_callback_context,
  bool(*fragment_callback_function)(ON__UINT_PTR, const class ON_SubDLimitPatchFragment*)
  ) const
{
  ON_SubDDisplayParameters local_limit_mesh_parameters = limit_mesh_parameters;

  if ( false == GetLimitSurfaceInStepsSetup(*this,local_limit_mesh_parameters) )
    return ON_SUBD_RETURN_ERROR(0);

  ON_SubDFaceIterator fit(*this);
  unsigned int fragment_count = GetQuadLimitSurfacePatchFragmentsHelper(
    fit,
    local_limit_mesh_parameters,
    fragment_callback_context,
    fragment_callback_function
    );

  if ( fragment_count > 0 )
    return fragment_count;

  return ON_SUBD_RETURN_ERROR(0);
}



unsigned int ON_SubDFaceIterator::LimitSurfaceMeshFragmentCount(
  ON_SubD::FacetType facet_type
  ) const
{
  unsigned int fragment_count = 0;
  unsigned short ordinary_edge_count 
    = (ON_SubD::FacetType::Tri == facet_type)
    ? 3U
    : 4U; // default is quads

  for (const ON_SubDFace* face = m_face_first; nullptr != face; face = face->m_next_face)
  {
    if ( ordinary_edge_count == face->m_edge_count )
      fragment_count++;
    else
      fragment_count += face->m_edge_count;
  }
  return fragment_count;
}


unsigned int ON_SubD::LimitSurfaceMeshFragmentCount() const
{
  return ActiveLevel().LimitSurfaceMeshFragmentCount();
}

unsigned int ON_SubDLevel::LimitSurfaceMeshFragmentCount() const
{
  unsigned int fragment_count = 0;
  ON_SubD::FacetType facet_type = ON_SubD::FacetTypeFromSubDType(m_subdivision_type);
  unsigned short ordinary_edge_count 
    = (ON_SubD::FacetType::Tri == facet_type)
    ? 3U
    : 4U; // default is quads

  for (const ON_SubDFace* face = m_face[0]; nullptr != face; face = face->m_next_face)
  {
    if ( ordinary_edge_count == face->m_edge_count )
      fragment_count++;
    else
      fragment_count += face->m_edge_count;
  }
  return fragment_count;
}

ON_SubDLimitMesh ON_SubD::UpdateLimitSurfaceMesh(
  unsigned int minimum_display_density
  ) const
{

  ON_SubDDisplayParameters display_parameters;
  display_parameters.m_display_density = minimum_display_density;
  return ActiveLevel().UpdateLimitSurfaceMesh(*this,display_parameters);
}

ON_SubDLimitMesh ON_SubDLevel::UpdateLimitSurfaceMesh(
    const ON_SubD& subd,
    const class ON_SubDDisplayParameters& display_parameters
    ) const
{
  if ( IsEmpty() )
    return ON_SubDLimitMesh::Empty;

  if (m_limit_mesh.IsEmpty() || display_parameters.m_display_density > m_limit_mesh.DisplayParameters().m_display_density)
  {
    ON_SubDLimitMesh local_limit_mesh;
    if (nullptr != ON_SubDLimitMesh::Create(subd, display_parameters, &local_limit_mesh))
    {
      ON_SubDLimitMesh::Swap(m_limit_mesh,local_limit_mesh);
      local_limit_mesh.Clear();
    }
  }

  return m_limit_mesh;
}

class ON_SubDLimitMesh ON_SubD::LimitSurfaceMesh() const
{
  return ActiveLevel().m_limit_mesh;
}

void ON_SubD::ClearLimitSurfaceMesh() const
{
  const ON_SubDLevel* level = ActiveLevelConstPointer();

  if ( nullptr != level )
    level->m_limit_mesh = ON_SubDLimitMesh::Empty;
}

void ON_SubD::ClearEvaluationCache() const
{
  const ON_SubDLevel* level = ActiveLevelConstPointer();

  if (nullptr != level)
  {
    level->ClearEdgeFlags();
    level->ClearBoundingBox();
    level->ClearSubdivisonAndLimitPoints();
    level->m_limit_mesh = ON_SubDLimitMesh::Empty;
  }
}



////////////////////////////////////////////////////////////////////////////


class CPatchGetter
{
public:
  CPatchGetter(
    const ON_SubD& subd,
    unsigned int patch_density,
    bool bClampPatchKnots,
    const wchar_t* sUserStringPatchIdKey,
    ON_SimpleArray<ON_NurbsSurface*>& patches
    )
      : m_subd(subd)
      , m_patch_density(patch_density)
      , m_bClampPatchKnots(bClampPatchKnots)
      , m_sUserStringPatchIdKey((nullptr != sUserStringPatchIdKey && sUserStringPatchIdKey[0] > ON_wString::Space) ? sUserStringPatchIdKey : nullptr)
      , m_patches(patches)
    {}

  const ON_SubD& m_subd;

  const unsigned int m_patch_density = 2;
  const bool m_bClampPatchKnots = false;
  const wchar_t* m_sUserStringPatchIdKey = nullptr;

  ON_SimpleArray<ON_NurbsSurface*>& m_patches;
  
  unsigned int m_x_count = 0;
  unsigned int m_s_count = 0;

  bool AddPatch(
    const ON_SubDLimitPatchFragment* patch_fragment
    );

  static bool GetLimitSurfaceInPatchesCallback(
    ON__UINT_PTR  context, // contest = CPatchGetter*
    const ON_SubDLimitPatchFragment* patch_fragment
    );

private:
  static bool CheckCVs(const ON_NurbsSurface& s);
  wchar_t* AppendUnsigned(
    wchar_t prefix,
    unsigned int i,
    wchar_t* s,
    wchar_t* send
    );
private:
  CPatchGetter() = delete;
  CPatchGetter(const CPatchGetter&) = delete;
  CPatchGetter& operator=(const CPatchGetter&) = delete;
};

bool CPatchGetter::GetLimitSurfaceInPatchesCallback(
  ON__UINT_PTR context,
  const ON_SubDLimitPatchFragment* patch_fragment
  )
{
  return ((CPatchGetter*)context)->AddPatch(patch_fragment);
}

wchar_t* CPatchGetter::AppendUnsigned(
  wchar_t prefix,
  unsigned int i,
  wchar_t* s,
  wchar_t* send
  )
{
  if ( 0 != prefix && s < send)
    *s++ = prefix;
  wchar_t buffer[64];
  wchar_t* sdigit = buffer;
  wchar_t* sdigit1 = sdigit + (sizeof(buffer)/sizeof(buffer[0]));
  for ( *sdigit++ = 0; sdigit < sdigit1; sdigit++ )
  {
    *sdigit = (wchar_t)('0' + (i%10));
    i /= 10;
    if (0 == i)
    {
      while ( s < send && 0 != (*s = *sdigit--) )
        s++;
      return s;
    }
  }
  return s;
}

bool CPatchGetter::CheckCVs(
  const ON_NurbsSurface& s
  )
{
  for (int i = 0; i < s.m_cv_count[0]; i++)
  {
    for (int j = 0; j < s.m_cv_count[1]; j++)
    {
      double * cv = s.CV(i, j);
      for (unsigned k = 0; k < 3; k++)
      {
        if (!ON_IsValid(cv[k]))
        {
          return ON_SUBD_RETURN_ERROR(false);
        }
      }
    }
  }
  return true;
}

bool CPatchGetter::AddPatch(
  const ON_SubDLimitPatchFragment* patch_fragment
  )
{
  unsigned int exact_bispan_count = 0;
  unsigned int approximate_bispan_count = 0;
  unsigned int fvi;
  for (fvi = 0; fvi < 4; fvi++)
  {
    switch (patch_fragment->m_patch_type[fvi])
    {
    case ON_SubDLimitPatchFragment::PatchType::None:
      break;

    case ON_SubDLimitPatchFragment::PatchType::Bicubic:
    case ON_SubDLimitPatchFragment::PatchType::BicubicQuadrant:
      // The NURBS bispan exactly matches the Catmull-Clark SubD surface.
      exact_bispan_count++;
      break;

    case ON_SubDLimitPatchFragment::PatchType::ApproximateBicubic:
    case ON_SubDLimitPatchFragment::PatchType::ApproximateBicubicQuadrant:
      // The NURBS bispan approximates the Catmull-Clark SubD surface.
      // Typically a limit point interpolation calculation was required 
      // to set a surface cv.
      approximate_bispan_count++;
      break;

    case ON_SubDLimitPatchFragment::PatchType::Unset:
      break;
    default:
      //ON_ERROR("Invalid patch_fragment->m_patch_type[] value.");
      ON_SubDIncrementErrorCount();
      break;
    }
  }

  const unsigned int bispan_count = exact_bispan_count + approximate_bispan_count;
  
  const unsigned int max_bispan_count 
    = (   0 == patch_fragment->m_face_subdivision_count 
       && 1 == bispan_count 
       && ON_SubDLimitPatchFragment::PatchType::Bicubic == patch_fragment->m_patch_type[0]
       )
    ? 1
    : 4;

  if (patch_fragment->m_face_subdivision_count > 0)
  {
    if ( m_x_count > 0 )
      m_x_count--;
  }

  m_x_count += max_bispan_count - bispan_count;
  if ( bispan_count <= 0 )
    return true;


  // attribute name setup
  const bool bSetPatchId = (nullptr != m_sUserStringPatchIdKey);
  const wchar_t* sOrdinary = L"Ordinary";
  const wchar_t* sExtraordinary = L"Extraordinary";

  wchar_t sFaceRegion[64];
  wchar_t* s = sFaceRegion;
  wchar_t* send = s + (sizeof(sFaceRegion)/sizeof(sFaceRegion[0]) - 1);
  *send = 0;

  if (nullptr != m_sUserStringPatchIdKey)
  {
    s = AppendUnsigned('f', patch_fragment->m_level0_face->m_id, s, send);
    for (unsigned short i = 0; i < patch_fragment->m_face_subdivision_count; i++)
      s = AppendUnsigned('.', patch_fragment->m_face_region_index[i], s, send);
  }

  const double knots[7] = {-2,-1,0,1,2,3,4};
  ON_NurbsSurface patch_srf;
  patch_srf.m_dim = 3;
  patch_srf.m_is_rat = 0;
  patch_srf.m_order[0] = 4;
  patch_srf.m_order[1] = 4;
  patch_srf.m_knot[0] = (double*)knots;
  patch_srf.m_knot[1] = (double*)knots;
  patch_srf.m_cv_stride[0] = 5*3;
  patch_srf.m_cv_stride[1] = 3;
  
  ON_wString patch_name;

  if (4 == bispan_count)
  {
    patch_srf.m_cv_count[0] = 5;
    patch_srf.m_cv_count[1] = 5;
    patch_srf.m_cv = (double*)patch_fragment->m_patch_cv[0][0];
    ON_NurbsSurface* surface = new ON_NurbsSurface(patch_srf);
    CheckCVs(*surface);
    if (m_bClampPatchKnots)
    {
      surface->ClampEnd(0, 2);
      surface->ClampEnd(1, 2);
      CheckCVs(*surface);
    }

    if (bSetPatchId)
    {
      patch_name.Format(
        L"%ls %ls",
        ((approximate_bispan_count > 0) ? sExtraordinary : sOrdinary),
        sFaceRegion
        );
      surface->SetUserString( m_sUserStringPatchIdKey, static_cast<const wchar_t*>(patch_name));
    }

    m_patches.Append(surface);
    m_s_count += 4;
  }
  else
  {
    const ON_2dex cvdex[4] = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } };
    for (fvi = 0; fvi < 4; fvi++)
    {
      if (ON_SubDLimitPatchFragment::PatchType::Unset == patch_fragment->m_patch_type[fvi])
        continue;
      if (ON_SubDLimitPatchFragment::PatchType::None == patch_fragment->m_patch_type[fvi])
        continue;

      patch_srf.m_cv_count[0] = 4;
      patch_srf.m_cv_count[1] = 4;
      patch_srf.m_cv = (double*)patch_fragment->m_patch_cv[cvdex[fvi].i][cvdex[fvi].j];
      ON_NurbsSurface* surface = new ON_NurbsSurface(patch_srf);
      CheckCVs(*surface);
      if (m_bClampPatchKnots)
      {
        surface->ClampEnd(0, 2);
        surface->ClampEnd(1, 2);
        CheckCVs(*surface);
      }

      if (bSetPatchId)
      {
        if (max_bispan_count > 1)
        {
          *s = 0;
          AppendUnsigned('.', fvi, s, send);
        }

        const wchar_t* sPatchType;

        switch (patch_fragment->m_patch_type[fvi])
        {
        case ON_SubDLimitPatchFragment::PatchType::Unset:
          sPatchType = L"Unset";
          break;

        case ON_SubDLimitPatchFragment::PatchType::Bicubic:
        case ON_SubDLimitPatchFragment::PatchType::BicubicQuadrant:
          sPatchType = sOrdinary;
          break;

        case ON_SubDLimitPatchFragment::PatchType::ApproximateBicubic:
        case ON_SubDLimitPatchFragment::PatchType::ApproximateBicubicQuadrant:
          sPatchType = sExtraordinary;
          break;

        default:
          sPatchType = L"?";
          break;
        }

        patch_name.Format(
          L"%s %s",
          sPatchType,
          sFaceRegion
          );
        surface->SetUserString( this->m_sUserStringPatchIdKey, static_cast<const wchar_t*>(patch_name));
      }

      m_patches.Append(surface);
      m_s_count++;
    }
  }

  patch_srf.m_knot[0] = nullptr;
  patch_srf.m_knot[1] = nullptr;
  patch_srf.m_cv = nullptr;

  return true;
}

unsigned int ON_SubD::GetLimitSurfacePatches(
  const class ON_SubDDisplayParameters& display_parameters,
  bool bClampPatchKnots,
  const wchar_t* sUserStringPatchKey,
  ON_SimpleArray< ON_NurbsSurface* >& patches
  ) const
{
  CPatchGetter patch_getter(
    *this,
    display_parameters.m_display_density,
    bClampPatchKnots,
    sUserStringPatchKey,
    patches
    );

  GetLimitSurfaceInPatches(
    display_parameters,
    (ON__UINT_PTR)&patch_getter,
    CPatchGetter::GetLimitSurfaceInPatchesCallback
    );

  return patch_getter.m_s_count;
}
