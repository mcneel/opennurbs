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

#if !defined(OPENNURBS_CYLINDER_INC_)
#define OPENNURBS_CYLINDER_INC_

class ON_NurbsSurface;
class ON_RevSurface;
class ON_Brep;

/*
Description:
  ON_Cylinder is a right circular cylinder.
*/
class ON_CLASS ON_Cylinder
{
public:
  // zeros all fields - cylinder is invalid
  ON_Cylinder(); 

  // infinite cylinder
  ON_Cylinder( 
    const ON_Circle& circle
    );

  // finite cylinder
  ON_Cylinder( 
    const ON_Circle& circle,
    double height
    );

  ~ON_Cylinder();

  bool Create(
    const ON_Circle& circle
    );

  bool Create(
    const ON_Circle& circle,
    double height
    );

  bool IsValid() const; // returns true if all fields contain reasonable
                        // information and equation jibes with point and Z.

  bool IsFinite() const; // returns true if the cylinder is finite
                         // (height[0] != height[1]) and false if the
                         // cylinder is infinite.

  const ON_3dVector& Axis() const;

  const ON_3dPoint& Center() const;

  // returns the height or 0 for infinite cylinder
  double Height() const; 

  ON_Circle CircleAt( 
        double linear_parameter // height from base circle's plane
        ) const;

  ON_Line LineAt( 
        double angular_parameter // [0,2pi]
        ) const;

  // evaluate parameters and return point
  ON_3dPoint PointAt(
    double angular_parameter, // [0,2pi]
    double linear_parameter   // height from base circle's plane
    ) const;

  // evaluate parameters and return normal vector
  ON_3dPoint NormalAt(
    double angular_parameter, // [0,2pi]
    double linear_parameter   // height from base circle's plane
    ) const;

  // returns parameters of point on cylinder that is closest to given point
  bool ClosestPointTo( 
         ON_3dPoint P, 
         double* angular_parameter, // [0,2pi]
         double* linear_parameter   // height from base circle's plane
         ) const;

  // returns point on cylinder that is closest to given point
  ON_3dPoint ClosestPointTo( 
         ON_3dPoint P
         ) const;

  // For intersections see ON_Intersect();

  // rotate cylinder about its origin
  bool Rotate(
        double sin_angle,
        double cos_angle,
        const ON_3dVector& rotation_axis
        );

  // rotate cylinder along a given axis
  bool Rotate(
        double angle_rad,
        const ON_3dVector& rotation_axis
        );

  // rotate cylinder about a point and axis
  bool Rotate(
        double sin_angle,
        double cos_angle,
        const ON_3dVector& rotation_axis,
        const ON_3dPoint& rotation_center
        );

  // rotate cylinder about a point and axis
  bool Rotate(
        double angle_rad,
        const ON_3dVector& rotation_axis,
        const ON_3dPoint& rotation_center
        );

  // translate the cylinder along a vector
  bool Translate(
        const ON_3dVector& translation_vector
        );

  // parameterization of NURBS surface does not match cylinder's transcendental parameterization
  int GetNurbForm( ON_NurbsSurface& ) const; // returns 0=failure, 2=success

  /*
  Description:
    Creates a surface of revolution definition of the cylinder.
  Parameters:
    srf - [in] if not nullptr, then this srf is used.
  Result:
    A surface of revolution or nullptr if the cylinder is not 
    valid or is infinite.
  */
  ON_RevSurface* RevSurfaceForm( ON_RevSurface* srf = nullptr ) const;

public: // members left public
  // base circle
  ON_Circle  circle;

  
  // If height[0] = height[1], the cylinder is infinite,
  // Otherwise, height[0] < height[1] and the center of
  // the "bottom" cap is 
  //
  //          circle.plane.origin + height[0]*circle.plane.zaxis,
  //
  // and the center of the top cap is 
  //
  //          circle.plane.origin + height[1]*circle.plane.zaxis.
  double height[2];
};

#endif
