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

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

const double ON_2x2Matrix::ZeroTolerance = 1.0e-12;
const ON_2x2Matrix ON_2x2Matrix::Zero(0.0);
const ON_2x2Matrix ON_2x2Matrix::Identity(1.0);
const ON_2x2Matrix ON_2x2Matrix::Nan(ON_DBL_QNAN);

ON_2x2Matrix::ON_2x2Matrix(double M[2][2])
{
  m[0][0] = M[0][0];
  m[0][1] = M[0][1];
  m[1][0] = M[1][0];
  m[1][1] = M[1][1];
}

ON_2x2Matrix::ON_2x2Matrix(const double M[2][2])
{
  m[0][0] = M[0][0];
  m[0][1] = M[0][1];
  m[1][0] = M[1][0];
  m[1][1] = M[1][1];
}

ON_2x2Matrix::ON_2x2Matrix(double d)
{
  m[0][0] = d;
  m[0][1] = 0.0;
  m[1][0] = 0.0;
  m[1][1] = d;
}

ON_2x2Matrix::ON_2x2Matrix(double d0, double d1)
{
  m[0][0] = d0;
  m[0][1] = 0.0;
  m[1][0] = 0.0;
  m[1][1] = d1;
}

ON_2x2Matrix::ON_2x2Matrix(double a, double b, double c, double d)
{
  m[0][0] = a;
  m[0][1] = b;
  m[1][0] = c;
  m[1][1] = d;
}

ON_2x2Matrix::ON_2x2Matrix(const class ON_Matrix& M)
{
  if (2 == M.RowCount() && 2 == M.ColCount())
  {
    this->m[0][0] = M.m[0][0];
    this->m[0][1] = M.m[0][1];
    this->m[1][0] = M.m[1][0];
    this->m[1][1] = M.m[1][1];
  }
  else
  {
    this->m[0][0] = ON_DBL_QNAN;
    this->m[0][1] = ON_DBL_QNAN;
    this->m[1][0] = ON_DBL_QNAN;
    this->m[1][1] = ON_DBL_QNAN;
  }
}

int ON_2x2Matrix::Rank(double zero_tolerance) const
{
  double det;
  return RankAndDeterminantHelper(zero_tolerance, det);
}

int ON_2x2Matrix::RankAndDeterminantHelper(double zero_tolerance, double& det) const
{
  det = ON_DBL_QNAN;

  if (false == (zero_tolerance > 0.0))
    zero_tolerance = 0.0;

  int piv = 0;

  double e = fabs(m[0][0]);
  if (false == (e < ON_UNSET_POSITIVE_VALUE))
    return -1;

  double f = fabs(m[0][1]);
  if (false == (f < ON_UNSET_POSITIVE_VALUE))
    return -1;
  if (f > e)
  {
    e = f;
    piv = 1;
  }

  f = fabs(m[1][0]);
  if (false == (f < ON_UNSET_POSITIVE_VALUE))
    return -1;
  if (f > e)
  {
    e = f;
    piv = 2;
  }

  f = fabs(m[1][1]);
  if (false == (f < ON_UNSET_POSITIVE_VALUE))
    return -1;
  if (f > e)
  {
    e = f;
    piv = 3;
  }

  if (e <= zero_tolerance)
  {
    // matrix is "zero"
    det = 0.0;
    return 0;
  }

  switch (piv)
  {
  case 0:
    if (m[0][0] < 0.0)
      e = -e;
    det = e * (m[1][1] - m[1][0] * (m[0][1] / e));
    break;
  case 1:
    if (m[0][1] < 0.0)
      e = -e;
    det = e * (m[1][1] * (m[0][0] / e) - m[1][0]);
    break;
  case 2:
    if (m[1][0] < 0.0)
      e = -e;
    det = e * (m[0][0] * (m[1][1] / e) - m[0][1]);
    break;
  case 3:
    if (m[1][1] < 0.0)
      e = -e;
    det = e * (m[0][0] - m[0][1] * (m[1][0] / e));
    break;
  default:
    det = 0.0;
  }

  if (fabs(det) <= zero_tolerance)
  {
    det = 0.0;
    return 1;
  }

  return 2;
}

int ON_2x2Matrix::Rank() const
{
  return Rank(0.0);
}

const ON_2x2Matrix ON_2x2Matrix::Transpose() const
{
  ON_2x2Matrix T;
  T.m[0][0] = m[0][0];
  T.m[0][1] = m[1][0];
  T.m[1][0] = m[0][1];
  T.m[1][1] = m[1][1];
  return T;
}

const ON_2x2Matrix ON_2x2Matrix::SwapRows() const
{
  ON_2x2Matrix T;
  T.m[0][0] = m[1][0];
  T.m[0][1] = m[1][1];
  T.m[1][0] = m[0][0];
  T.m[1][1] = m[0][1];
  return T;
}

const ON_2x2Matrix ON_2x2Matrix::SwapColumns() const
{
  ON_2x2Matrix T;
  T.m[0][0] = m[0][1];
  T.m[0][1] = m[0][0];
  T.m[1][0] = m[1][1];
  T.m[1][1] = m[1][0];
  return T;
}



const ON_2x2Matrix ON_2x2Matrix::SwapRowsAndColumns() const
{
  ON_2x2Matrix T;
  T.m[0][0] = m[1][1];
  T.m[0][1] = m[1][0];
  T.m[1][0] = m[0][1];
  T.m[1][1] = m[0][0];
  return T;
}

const ON_2x2Matrix ON_2x2Matrix::Rotation(ON_2dVector V)
{
  if (false == V.IsUnitVector())
  {
    if (false == V.IsValid())
      return ON_2x2Matrix::Nan;
    V = V.UnitVector();
  }
  return ON_2x2Matrix::Rotation(V.x, V.y);
}

const ON_2x2Matrix ON_2x2Matrix::Rotation(double cos_angle, double sin_angle)
{
  ON_2x2Matrix R;
  R.m[0][0] = cos_angle;
  R.m[0][1] = -sin_angle;
  R.m[1][0] = sin_angle;
  R.m[1][1] = cos_angle;
  return R;
}

const ON_2x2Matrix ON_2x2Matrix::RotationRadians(double angle_radians)
{
  return Rotation(cos(angle_radians), sin(angle_radians));
}

const ON_2x2Matrix ON_2x2Matrix::RotationDegrees(double angle_degrees)
{
  return RotationRadians((180.0 / ON_PI) * angle_degrees);
}

double ON_2x2Matrix::MaximumDigonalAbsoluteValue() const
{
  const double a = fabs(m[0][0]);
  const double d = fabs(m[1][1]);
  return (a >= d) ? a : (a < d ? d : ON_DBL_QNAN);
}

double ON_2x2Matrix::MinimumDigonalAbsoluteValue() const
{
  const double a = fabs(m[0][0]);
  const double d = fabs(m[1][1]);
  return (a <= d) ? a : (a > d ? d : ON_DBL_QNAN);
}

double ON_2x2Matrix::MaximumOffDigonalAbsoluteValue() const
{
  const double b = fabs(m[0][1]);
  const double c = fabs(m[1][0]);
  return (b >= c) ? b : (b < c ? c : ON_DBL_QNAN);
}

double ON_2x2Matrix::MinimumOffDigonalAbsoluteValue() const
{
  const double b = fabs(m[0][1]);
  const double c = fabs(m[1][0]);
  return (b <= c) ? b : (b > c ? c : ON_DBL_QNAN);
}

double ON_2x2Matrix::MaximumCoefficientAbsoluteValue() const
{
  const double x = MaximumDigonalAbsoluteValue();
  const double y = MaximumOffDigonalAbsoluteValue();
  return (x >= y) ? x : (x < y) ? y : ON_DBL_QNAN;
}

double ON_2x2Matrix::MinimumCoefficientAbsoluteValue() const
{
  const double x = MinimumDigonalAbsoluteValue();
  const double y = MinimumOffDigonalAbsoluteValue();
  return (x >= y) ? x : (x < y) ? y : ON_DBL_QNAN;
}

bool ON_2x2Matrix::IsValid() const
{
  return
    ON_UNSET_VALUE < m[0][0] && m[0][0] < ON_UNSET_POSITIVE_VALUE
    && ON_UNSET_VALUE < m[0][1] && m[0][1] < ON_UNSET_POSITIVE_VALUE
    && ON_UNSET_VALUE < m[1][0] && m[1][0] < ON_UNSET_POSITIVE_VALUE
    && ON_UNSET_VALUE < m[1][1] && m[1][1] < ON_UNSET_POSITIVE_VALUE;
}

bool ON_2x2Matrix::IsIdentity(double tolerance) const
{
  if (false == (tolerance > 0.0 && tolerance < ON_UNSET_POSITIVE_VALUE))
    tolerance = 0.0;
  return
    fabs(m[0][0] - 1.0) <= tolerance
    && fabs(m[0][1]) <= tolerance
    && fabs(m[1][0]) <= tolerance
    && fabs(m[1][1] - 1.0) <= tolerance;
}

bool ON_2x2Matrix::IsZero(double zero_tolerance) const
{
  if (false == (zero_tolerance > 0.0 && zero_tolerance < ON_UNSET_POSITIVE_VALUE))
    zero_tolerance = 0.0;
  return
    fabs(m[0][0]) <= zero_tolerance
    && fabs(m[0][1]) <= zero_tolerance
    && fabs(m[1][0]) <= zero_tolerance
    && fabs(m[1][1]) <= zero_tolerance;
}

bool ON_2x2Matrix::IsZero() const
{
  return (0.0 == m[0][0] && 0.0 == m[0][1] && 0.0 == m[1][0] && 0.0 == m[1][1]);
}

bool ON_2x2Matrix::IsNotZero(double zero_tolerance) const
{
  if (false == IsValid())
    return false;
  if (false == (zero_tolerance > 0.0))
    zero_tolerance = 0.0;
  return (
    fabs(m[0][0]) > zero_tolerance
    || fabs(m[0][1]) > zero_tolerance
    || fabs(m[1][0]) > zero_tolerance
    || fabs(m[1][1]) > zero_tolerance
    );
}

bool ON_2x2Matrix::IsNotZero() const
{
  if (false == IsValid())
    return false;
  return (
    fabs(m[0][0]) > 0.0
    || fabs(m[0][1]) > 0.0
    || fabs(m[1][0]) > 0.0
    || fabs(m[1][1]) > 0.0
    );
}


bool ON_2x2Matrix::IsIdentity() const
{
  return (1.0 == m[0][0] && 0.0 == m[0][1] && 0.0 == m[1][0] && 1.0 == m[1][1]);
}

bool ON_2x2Matrix::IsDiagonal(double relative_tolerance) const
{
  const double maxd = MaximumDigonalAbsoluteValue();
  const double maxoffd = MaximumOffDigonalAbsoluteValue();
  const double r =
    (relative_tolerance >= 0.0 && relative_tolerance < 1.0)
    ? relative_tolerance
    : ON_DBL_QNAN;
  return (maxoffd <= maxd * r);
}

bool ON_2x2Matrix::IsDiagonal() const
{
  return (
    ON_UNSET_VALUE <= m[0][0] && m[0][0] < ON_UNSET_POSITIVE_VALUE
    && 0.0 == m[0][1] 
    && 0.0 == m[1][0] 
    && ON_UNSET_VALUE <= m[1][1] && m[1][1] < ON_UNSET_POSITIVE_VALUE
    );
}

bool ON_2x2Matrix::IsOrthoganal(double tolerance) const
{
  if (false == (tolerance > 0.0 && tolerance < ON_UNSET_POSITIVE_VALUE))
    tolerance = 0.0;
  const ON_2dVector R0(m[0][0], m[0][1]);
  if (fabs(R0.Length() - 1.0) <= tolerance)
  {
    const ON_2dVector R1(m[1][0], m[1][1]);
    if (fabs(R1.Length() - 1.0) <= tolerance)
    {
      if (fabs(m[0][0] * m[1][0] + m[0][1] * m[1][1]) <= tolerance)
        return true;
    }
  }
  return false;
}

bool ON_2x2Matrix::IsOrthoganal() const
{
  return IsOrthoganal(ON_2x2Matrix::ZeroTolerance);
}

double ON_2x2Matrix::Determinant() const
{
  double det;
  const int rank = RankAndDeterminantHelper(0.0, det);
  return (2 == rank) ? det : ((0 == rank || 1 == rank) ? 0.0 : ON_DBL_QNAN);
}

double ON_2x2Matrix::Trace() const
{
  return 
    (fabs(m[0][0]) < ON_UNSET_POSITIVE_VALUE && fabs(m[1][1]) < ON_UNSET_POSITIVE_VALUE)
    ? (m[0][0] + m[1][1])
    : ON_DBL_QNAN;
}

const ON_2x2Matrix ON_2x2Matrix::Inverse() const
{
  const double d = Determinant();
  if (d != 0.0)
    return ON_2x2Matrix(m[1][1] / d, -m[0][1] / d, -m[1][0] / d, m[0][0] / d);
  return ON_2x2Matrix::Nan;
}

double ON_2x2Matrix::Coefficient(int i, int j) const
{
  return (i >= 0 && i <= 1 && 0 >= j && j <= 1) ? m[i][j] : ON_DBL_QNAN;
}

const double* ON_2x2Matrix::operator[](int i) const
{
  return (i >= 0 && i <= 1) ? m[i] : nullptr;
}

double* ON_2x2Matrix::operator[](int i)
{
  return (i >= 0 && i <= 1) ? m[i] : nullptr;
}

static double ON_SqrtSum(double x, double y)
{
  return (x == y) ? sqrt(2.0 * x) : sqrt(x + y);
}

/// <summary>
/// Get the singular value decomposition of the 2x2 matrix {{a,b},{c,d}}
/// as U * Diagnoal(singular_values[0], singular_values[1]) * Transpose(V)
/// </summary>
/// <param name="a">
/// a = coefficient in top row and left column.
/// </param>
/// <param name="b">
/// b = coefficient in top row and right column.
/// </param>
/// <param name="c">
/// c = coefficient in bottom row and left column
/// </param>
/// <param name="d">
/// d = coefficient in bottom row and right column
/// </param>
/// <param name="U"></param>
/// <param name="singular_values">
/// The returned singular values satisfy 
/// 0 &lt;= singular_values[0] &lt;= singular_values[1].
/// </param>
/// <param name="V"></param>
/// <returns>
/// If the singular value decomposition is returned, then true
/// is returned. Otherwise the singular_values[] elements are 
/// set to ON_DBL_QNAN and false is returned.
/// </returns>
bool ON_Get2x2SVD(
  double a, double b, double c, double d,
  double U[2][2],
  double singular_values[2],
  double V[2][2])
{
  // The input matrix is A[2][2] = { {a,b},{c,d} };
  // The singlar value decomposition is 
  // A = U*Diagonal(sigma0,sigma1)*Transpose(V)
  // with sigma0 >= sigma1 >= 0.
  if (0.0 == c && 0.0 == b)
  {
    // This clause handles the case when A is zero or a diagonal.

    if (fabs(a) <= fabs(d))
    {
      singular_values[0] = fabs(a);
      singular_values[1] = fabs(d);

      U[0][0] = 1.0;
      U[0][1] = 0.0;
      U[1][0] = 0.0;
      U[1][1] = 1.0;

      V[0][0] = (a < 0.0) ? -1.0 : 1.0;
      V[0][1] = 0.0;
      V[1][0] = 0.0;
      V[1][1] = (d < 0.0) ? -1.0 : 1.0;
    }
    else if (fabs(a) > fabs(d))
    {
      U[0][0] = 0.0;
      U[0][1] = 1.0;
      U[1][0] = 1.0;
      U[1][1] = 0.0;

      singular_values[0] = fabs(d);
      singular_values[1] = fabs(a);

      V[0][0] = 0.0;
      V[0][1] = (a < 0.0) ? -1.0 : 1.0;
      V[1][0] = (d < 0.0) ? -1.0 : 1.0;
      V[1][1] = 0.0;
    }
    else
    {
      singular_values[0] = ON_DBL_QNAN;
      singular_values[1] = ON_DBL_QNAN;

      U[0][0] = 1.0;
      U[0][1] = 0.0;
      U[1][0] = 0.0;
      U[1][1] = 1.0;

      V[0][0] = 1.0;
      V[0][1] = 0.0;
      V[1][0] = 0.0;
      V[1][1] = 1.0;
      return false;
    }
    return true;
  }

  const double aa = a * a;
  const double bb = b * b;
  const double cc = c * c;
  const double dd = d * d;

  const double rel_zero_tol = 1e-12;

  // We know A is not diagonal (including not zero).
  // 
  // Calculate U = rotation(theta) so that
  // A*Transpose(A) = {{UJ, UL}, {UL, UK}} = U*D*Transpose(U)
  // where D = Diagonal(s0*s0, s1,s1) = Diagonal(E,F) (0 <= E <= F).
  const double UJ = aa + bb;
  const double UK = cc + dd;
  const double UL = a * c + b * d;
  const double UJminusUK = UJ - UK;
  const double Ux = 0.5 * ON_SqrtSum(UJminusUK * UJminusUK, 4.0 * (UL * UL));
  const bool bTrickyTheta = Ux <= 0.5 * rel_zero_tol * (UJ + UK);

  // Calculate W = rotation(phi)
  // Transpose(A)*A = {{J, L}, {L, K}} = W*D*Transpose(W)
  // where D = Diagonal(s0*s0, s1,s1) = Diagonal(E,F) (0 <= E <= F).
  const double WJ = aa + cc;
  const double WK = bb + dd;
  const double WL = a * b + c * d;
  const double WJminusWK = WJ - WK;
  const double Wx = 0.5 * ON_SqrtSum(WJminusWK * WJminusWK, 4.0 * (WL * WL));
  const bool bTrickyPhi = Wx <= 0.5 * rel_zero_tol * (WJ + WK);

  // For the U and W version of J, K, L, the following are satisfied.
  // J + K = E + F
  // J - K = cos(2*(theta or phi))*(E-F)
  // 2L = sin(2*(theta or phi))*(E-F)

  if (bTrickyTheta || bTrickyPhi)
  {
    // If x = (F-E) < rel_zero_tol*(E + F).
    // then we basically have E = F and atan2() will return garbage
    // for theta, phi, or both.
    // When E == F, then J = K = E = F >= 0, L = 0, and A*Transpose(A) = Diagnonal(J,J);
    // For example, if A = scale*rotation, then
    // J = K = E = F = scale^2,
    // L = 0, and
    // E = F = scale^2 and fabs(s0) = fabs(s1) = fabs(scale)
    double E;
    if (false == bTrickyPhi)
    {
      // bTrikyTheta = true, bTrikyPhi = false.
      // Use the U versions of J,K to get the singular values.
      // Note E >= 0 because UJ >= 0 and UK >= 0.
      E = (UJ == UK) ? UJ : 0.5 * (UJ + UK);
    }
    else if (false == bTrickyTheta)
    {
      // bTrikyTheta = false, bTrikyPhi = true.
      // Use the W versions of J,K to get the singular values.
      // Note E >= 0 because WJ >= 0 and WK >= 0.
      E = (WJ == WK) ? WJ : 0.5 * (WJ + WK);
    }
    else
    {
      // bTrikyTheta = bTrikyPhi = true.
      // In a perfect world, in this siutation we would have
      // J = K and L = 0.
      // Choose the U / W versions of J, K, L
      // that are closet to ideal.
      // Note E >= 0 because J >= 0 and K >= 0.
      const double Uerr = fabs(UJ - UK) + fabs(UL);
      const double Werr = fabs(WJ - WK) + fabs(WL);
      E = (Uerr <= Werr)
        ? ((UJ == UK) ? UJ : 0.5 * (UJ + UK))
        : ((WJ == WK) ? WJ : 0.5 * (WJ + WK))
        ;
    }


    // E=F is known to be >= 0
    const double sigma = sqrt(E);

    singular_values[0] = sigma;
    singular_values[1] = sigma;

    V[0][0] = 1.0;
    V[0][1] = 0.0;
    V[1][0] = 0.0;
    V[1][1] = 1.0;

    ON_2dVector U0(a, b);
    ON_2dVector U1(c, d);
    if (false == U0.IsUnitVector())
      U0 = U0.UnitVector();
    if (false == U1.IsUnitVector())
      U1 = U1.UnitVector();

    const double dot = U0 * U1;
    if (fabs(dot) < 1e-6)
    {
      U[0][0] = U0.x;
      U[0][1] = U0.y;
      U[1][0] = U1.x;
      U[1][1] = U1.y;
      return true;
    }

    U[0][0] = ON_DBL_QNAN;
    U[0][1] = ON_DBL_QNAN;
    U[1][0] = ON_DBL_QNAN;
    U[1][1] = ON_DBL_QNAN;

    return false;
  }

  double x, JplusKover2;

  // Since E <= F
  // 2 * E = (J + K) - sqrt((J-K)^2 + (2*L)^2)
  // 2 * F = (J + K) + sqrt((J-K)^2 + (2*L)^2)
  JplusKover2 = 0.5 * (UJ + UK);
  x = (Ux > JplusKover2) ? JplusKover2 : Ux;
  const double Us0 = ON_SqrtSum(JplusKover2, -x);
  const double Us1 = ON_SqrtSum(JplusKover2, x);

  // Solve Transpose(U) * A * Transpose(A) * U = D using the trig double angle formulae to get
  const double theta = 0.5 * atan2(-2.0 * UL, -UJminusUK);
  U[0][0] = cos(theta);
  U[1][1] = U[0][0];
  U[1][0] = sin(theta);
  U[0][1] = -U[1][0];

  // matrix solution for abs(singular values) using Transpose(U)*A*Transpose(A)*U = D = diag(s0*s0, s1*s1);
  const double TransposeUxA[2][2] =
  { {U[0][0] * a + U[1][0] * c, U[0][0] * b + U[1][0] * d},{U[0][1] * a + U[0][0] * c, U[0][1] * b + U[0][0] * d} };
  const double UDs0 = ON_SqrtSum(TransposeUxA[0][0] * TransposeUxA[0][0], TransposeUxA[0][1] * TransposeUxA[0][1]);
  const double UDs1 = ON_SqrtSum(TransposeUxA[1][1] * TransposeUxA[1][1], TransposeUxA[1][0] * TransposeUxA[1][0]);

  //Solve Transpose(W) * Transpose(A) * A * W = D using the trig double angle formulae to get
  const double phi = 0.5 * atan2(-2.0 * WL, -WJminusWK);
  double W[2][2];
  W[0][0] = cos(phi);
  W[1][1] = W[0][0];
  W[1][0] = sin(phi);
  W[0][1] = -W[1][0];

  // algebraic solution for abs(singular values) using W definition and trig
  JplusKover2 = 0.5 * (WJ + WK);
  x = (Wx > JplusKover2) ? JplusKover2 : Wx;
  const double Ws0 = ON_SqrtSum(JplusKover2, -x);
  const double Ws1 = ON_SqrtSum(JplusKover2, x);

  // matrix solution for abs(singular values) using Transpose(W)*Transpose(A)*A*W = D = diag(s0*s0, s1*s1);
  const double AxW[2][2] =
  { {a * W[0][0] + b * W[1][0], a * W[0][1] + b * W[0][0]},{c * W[0][0] + d * W[1][0], c * W[0][1] + d * W[0][0]} };
  const double WDs0 = ON_SqrtSum(AxW[0][0] * AxW[0][0], AxW[1][0] * AxW[1][0]);
  const double WDs1 = ON_SqrtSum(AxW[1][1] * AxW[1][1], AxW[0][1] * AxW[0][1]);

  // Find the correction matrix for the right side
  // S = diagonal(Transpose(U) * A * W);
  const double S00 = (a * U[0][0] + c * U[1][0]) * W[0][0] + (b * U[0][0] + d * U[1][0]) * W[1][0];
  const double S11 = (a * U[1][0] - c * U[0][0]) * W[1][0] - (b * U[1][0] - d * U[0][0]) * W[0][0];
  const double C[2] = { (S00 >= 0.0 ? 1.0 : -1.0), (S11 >= 0.0 ? 1.0 : -1.0) };
  V[0][0] = C[0] * W[0][0];
  V[0][1] = C[1] * W[0][1];
  V[1][0] = C[0] * W[1][0];
  V[1][1] = C[1] * W[1][1];


  // choose the best sigmas

  bool bHaveSigma = ON_IsValid(Us0) && ON_IsValid(Us1);
  if (bHaveSigma)
  {
    singular_values[0] = Us0;
    singular_values[1] = Us1;
  }

  const ON_2x2Matrix A(a, b, c, d);
  double SVD_error
    = bHaveSigma
    ? (A - ON_2x2Matrix::CombineSVD(U, Us0, Us1, V)).MaximumCoefficientAbsoluteValue()
    : ON_DBL_QNAN;
  if (0.0 == SVD_error)
    return true;

  if (ON_IsValid(UDs0) && ON_IsValid(UDs1))
  {
    const double e = (A - ON_2x2Matrix::CombineSVD(U, UDs0, UDs1, V)).MaximumCoefficientAbsoluteValue();
    if (e < SVD_error || false == bHaveSigma)
    {
      singular_values[0] = UDs0;
      singular_values[1] = UDs1;
      if (0.0 == e)
        return true;
      bHaveSigma = true;
      SVD_error = e;
    }
  }

  if (ON_IsValid(Ws0) && ON_IsValid(Ws1))
  {
    const double e = (A - ON_2x2Matrix::CombineSVD(U, Ws0, Ws1, V)).MaximumCoefficientAbsoluteValue();
    if (e < SVD_error || false == bHaveSigma)
    {
      singular_values[0] = Ws0;
      singular_values[1] = Ws1;
      if (0.0 == e)
        return true;
      bHaveSigma = true;
      SVD_error = e;
    }
  }

  if (ON_IsValid(Ws0) && ON_IsValid(Ws1))
  {
    double e = (A - ON_2x2Matrix::CombineSVD(U, WDs0, WDs1, V)).MaximumCoefficientAbsoluteValue();
    if (e < SVD_error || false == bHaveSigma)
    {
      singular_values[0] = WDs0;
      singular_values[1] = WDs1;
      if (0.0 == e)
        return true;
      bHaveSigma = true;
      SVD_error = e;
    }
  }

  if (false == bHaveSigma)
  {
    singular_values[0] = ON_DBL_QNAN;
    singular_values[1] = ON_DBL_QNAN;
  }

  return bHaveSigma;
}



/// <summary>
/// Get the singular value decomposition of the 2x2 matrix {{a,b},{c,d}}
/// as U * {{singular_values[0], 0}, {0, singular_values[1]}} * Transpose(V)/// 
/// </summary>
/// <param name="A"></param>
/// <param name="U"></param>
/// <param name="singular_values"></param>
/// <param name="V"></param>
/// <returns></returns>
bool ON_Get2x2SVD(
  const double A[2][2],
  double U[2][2],
  double singular_values[2],
  double V[2][2])
{
  return ON_Get2x2SVD(A[0][0], A[0][1], A[1][0], A[1][1], U, singular_values, V);
}

bool ON_2x2Matrix::SVD(
  ON_2x2Matrix& U,
  double singular_values[2],
  ON_2x2Matrix& V
) const
{
  return ON_Get2x2SVD(this->m, U.m, singular_values, V.m);
}

const ON_2x2Matrix ON_2x2Matrix::PseudoInverse() const
{
  ON_2x2Matrix U, V;
  double sigmas[2];
  const bool bHaveSVD = ON_Get2x2SVD(
    m[0][0], m[0][1], m[1][0], m[1][1],
    U.m,
    sigmas,
    V.m
  );
  return
    bHaveSVD
    ? ON_2x2Matrix::CombineSVD(
      V,
      0.0 != sigmas[0] ? 1.0 / sigmas[0] : 0.0,
      0.0 != sigmas[1] ? 1.0 / sigmas[1] : 0.0,
      U.Transpose())
    : ON_2x2Matrix::Nan;
 }

const ON_2x2Matrix ON_2x2Matrix::CombineSVD(
  const ON_2x2Matrix& U,
  double s0,
  double s1,
  const ON_2x2Matrix& V
)
{
  ON_2x2Matrix A;
  A.m[0][0] = s0 * U.m[0][0] * V.m[0][0] + s1 * U.m[0][1] * V.m[0][1];
  A.m[0][1] = s0 * U.m[0][0] * V.m[1][0] + s1 * U.m[0][1] * V.m[1][1];
  A.m[1][0] = s0 * U.m[1][0] * V.m[0][0] + s1 * U.m[1][1] * V.m[0][1];
  A.m[1][1] = s0 * U.m[1][0] * V.m[1][0] + s1 * U.m[1][1] * V.m[1][1];
  return A;
}


const ON_2x2Matrix ON_2x2Matrix::ProcrustesRotation(const ON_2x2Matrix& H)
{
  // Kabsch-Umeyama algorithm
  // 
  // P = 2 x N matrix (N 2d points as columns) (average of P[] = (0,0))
  // Q = 2 x N matrix (N 2d points as columns) (average of Q[] = (0,0))
  // H = P * Transpose(Q) is a 2x2 matrix that is "this" 2x2 matrix.

  ON_2x2Matrix V;
  ON_2x2Matrix W;
  double sigmas[2];
  if (false == H.SVD(V, sigmas, W))
    return ON_2x2Matrix::Nan;

  const int v = (V.m[0][0] * V.m[1][1] < V.m[0][1] * V.m[1][0]) ? -1 : 1;
  const int w = (W.m[0][0] * W.m[1][1] < W.m[0][1] * W.m[1][0]) ? -1 : 1;
  const double d = (v != w) ? -1.0 : 1.0;

  // R = Procrustes rotation (acting on the left of points)
  const ON_2x2Matrix R = ON_2x2Matrix::CombineSVD(W, 1.0, d, V);

  // Since R is a left rotation, clean up any slop so we return an
  // exact rotation ((cos, -sin),(sin, cos)).
  // cs.x = cos(rotation angle), cs.y = sin(rotation angle).
  ON_2dVector cs(
    (R.m[0][0] == R.m[1][1]) ? R.m[0][0] : (0.5 * (R.m[0][0] + R.m[1][1])),
    (-R.m[0][1] == R.m[1][0]) ? -R.m[0][1] : (0.5 * (R.m[1][0] - R.m[0][1])));
  if (fabs(cs.x) >= 1.0 - ON_EPSILON || fabs(cs.y) <= ON_EPSILON)
  {
    cs.x = (cs.x < 0.0) ? -1.0 : 1.0;
    cs.y = 0.0;
  }
  else if (fabs(cs.x) <= ON_EPSILON || fabs(cs.y) >= 1.0 - ON_EPSILON)
  {
    cs.x = 0.0;
    cs.y = (cs.y < 0.0) ? -1.0 : 1.0;
  }
  else
    cs = cs.UnitVector();

  const ON_2x2Matrix CS(cs.x, -cs.y, cs.y, cs.x);
  const double dd = (CS - R).MaximumCoefficientAbsoluteValue();
  if (fabs(dd) <= 1e-15)
    return CS;
  return CS;
}


const ON_2x2Matrix ON_2x2Matrix::ProcrustesRotation(
  const ON_2dPoint& P0, const ON_2dPoint& P1, const ON_2dPoint& P2, 
  const ON_2dPoint& Q0, const ON_2dPoint& Q1, const ON_2dPoint& Q2)
{
  // Kabsch-Umeyama algorithm
  const ON_2x2Matrix H(
    P0.x * Q0.x + P1.x * Q1.x + P2.x * Q2.x,
    P0.x * Q0.y + P1.x * Q1.y + P2.x * Q2.y,
    P0.y * Q0.x + P1.y * Q1.x + P2.y * Q2.x,
    P0.y * Q0.y + P1.y * Q1.y + P2.y * Q2.y
  );

  return ProcrustesRotation(H);
}

const ON_2x2Matrix operator*(const ON_2x2Matrix& A, const ON_2x2Matrix& B)
{
  ON_2x2Matrix AB;
  AB.m[0][0] = A.m[0][0] * B.m[0][0] + A.m[0][1] * B.m[1][0];
  AB.m[0][1] = A.m[0][0] * B.m[0][1] + A.m[0][1] * B.m[1][1];
  AB.m[1][0] = A.m[1][0] * B.m[0][0] + A.m[1][1] * B.m[1][0];
  AB.m[1][1] = A.m[1][0] * B.m[0][1] + A.m[1][1] * B.m[1][1];
  return AB;
}

const ON_2x2Matrix operator+(const ON_2x2Matrix& A, const ON_2x2Matrix& B)
{
  ON_2x2Matrix AplusB;
  AplusB.m[0][0] = A.m[0][0] + B.m[0][0];
  AplusB.m[0][1] = A.m[0][1] + B.m[0][1];
  AplusB.m[1][0] = A.m[1][0] + B.m[1][0];
  AplusB.m[1][1] = A.m[1][1] + B.m[1][1];
  return AplusB;
}

const ON_2x2Matrix operator-(const ON_2x2Matrix& A, const ON_2x2Matrix& B)
{
  ON_2x2Matrix AminusB;
  AminusB.m[0][0] = A.m[0][0] - B.m[0][0];
  AminusB.m[0][1] = A.m[0][1] - B.m[0][1];
  AminusB.m[1][0] = A.m[1][0] - B.m[1][0];
  AminusB.m[1][1] = A.m[1][1] - B.m[1][1];
  return AminusB;
}

const ON_2x2Matrix operator-(const ON_2x2Matrix& A)
{
  ON_2x2Matrix minusA;
  minusA.m[0][0] = -A.m[0][0];
  minusA.m[0][1] = -A.m[0][1];
  minusA.m[1][0] = -A.m[1][0];
  minusA.m[1][1] = -A.m[1][1];
  return minusA;
}

const ON_2dVector operator*(const ON_2x2Matrix& A, const ON_2dVector& V)
{
  return ON_2dVector(A.m[0][0] * V.x + A.m[0][1] * V.y, A.m[1][0] * V.x + A.m[1][1] * V.y);
}

const ON_2dVector operator*(const ON_2dVector& V, const ON_2x2Matrix& A)
{
  return ON_2dVector(A.m[0][0] * V.x + A.m[1][0] * V.y, A.m[0][1] * V.x + A.m[1][1] * V.y);
}

const ON_2dPoint operator*(const ON_2x2Matrix& A, const ON_2dPoint& V)
{
  return ON_2dPoint(A.m[0][0] * V.x + A.m[0][1] * V.y, A.m[1][0] * V.x + A.m[1][1] * V.y);
}

const ON_2dPoint operator*(const ON_2dPoint& V, const ON_2x2Matrix& A)
{
  return ON_2dPoint(A.m[0][0] * V.x + A.m[1][0] * V.y, A.m[0][1] * V.x + A.m[1][1] * V.y);
}

static void ON_2x2Matrix_Jiggle(ON_RandomNumberGenerator& RNG, ON_2x2Matrix& A)
{
  double tiny = A.MaximumCoefficientAbsoluteValue() * 1e-3;
  if (0 == tiny)
    tiny = 1e-4;
  const ON_Interval tiny_range(-tiny, tiny);
  const ON_Interval wiggle_range(0.999, 1.001);
  A[0][0] = (A[0][0] + RNG.RandomDouble(tiny_range)) * RNG.RandomDouble(wiggle_range);
  A[0][1] = (A[0][1] + RNG.RandomDouble(tiny_range)) * RNG.RandomDouble(wiggle_range);
  A[1][0] = (A[1][0] + RNG.RandomDouble(tiny_range)) * RNG.RandomDouble(wiggle_range);
  A[1][1] = (A[1][1] + RNG.RandomDouble(tiny_range)) * RNG.RandomDouble(wiggle_range);
}

static bool ON_2x2Matrix_TestPermutationsAndJiggles(
  const ON_2x2Matrix& A,
  const int Arank,
  ON_RandomNumberGenerator& RNG,
  unsigned& callback_count,
  ON__UINT_PTR context, bool(*Callback)(ON__UINT_PTR, int, const ON_2x2Matrix)
)
{
  if (false == Callback(context, Arank, A))
    return false;
  ++callback_count;

  ON_2x2Matrix J1 = A.SwapRows();
  if (false == Callback(context, Arank, J1))
    return false;
  ++callback_count;

  ON_2x2Matrix J2 = A.SwapColumns();
  if (false == Callback(context, Arank, J2))
    return false;
  ++callback_count;

  ON_2x2Matrix J3 = J1.SwapColumns();
  if (false == Callback(context, Arank, J3))
    return false;
  ++callback_count;

  const int unknown_rank = -1;
  ON_2x2Matrix J0 = A;
  ON_2x2Matrix_Jiggle(RNG, J0);
  if (false == Callback(context, unknown_rank, J0))
    return false;
  ++callback_count;

  ON_2x2Matrix_Jiggle(RNG, J1);
  if (false == Callback(context, unknown_rank, J1))
    return false;
  ++callback_count;

  ON_2x2Matrix_Jiggle(RNG, J2);
  if (false == Callback(context, unknown_rank, J2))
    return false;
  ++callback_count;

  ON_2x2Matrix_Jiggle(RNG, J3);
  if (false == Callback(context, unknown_rank, J3))
    return false;
  ++callback_count;

  return true;
}

unsigned ON_2x2Matrix::TestList(ON__UINT_PTR context, bool (*Callback)(ON__UINT_PTR, int, const ON_2x2Matrix))
{
  if (nullptr == Callback)
    return 0;

  unsigned callback_count = 0;

  ON_RandomNumberGenerator RNG;

  if (false == ON_2x2Matrix_TestPermutationsAndJiggles(ON_2x2Matrix::Identity, 2, RNG, callback_count, context, Callback))
    return callback_count;

  if (false == ON_2x2Matrix_TestPermutationsAndJiggles(ON_2x2Matrix::Zero, 0, RNG, callback_count, context, Callback))
    return callback_count;

  if (false == ON_2x2Matrix_TestPermutationsAndJiggles(ON_2x2Matrix(1.0, 0.0), 1, RNG, callback_count, context, Callback))
    return callback_count;


  const int half_maxi = 6 * 360;
  const int maxi = 2 * half_maxi;

  for (int i = 1; i < maxi; ++i)
  {
    const double scale = ((double)i) / ((double)half_maxi);
    if (false == ON_2x2Matrix_TestPermutationsAndJiggles(ON_2x2Matrix(scale), 2, RNG, callback_count, context, Callback))
      return callback_count;

    if (false == ON_2x2Matrix_TestPermutationsAndJiggles(ON_2x2Matrix(-scale), 2, RNG, callback_count, context, Callback))
      return callback_count;

    if (false == ON_2x2Matrix_TestPermutationsAndJiggles(ON_2x2Matrix(scale, 0.0), 1, RNG, callback_count, context, Callback))
      return callback_count;

    if (false == ON_2x2Matrix_TestPermutationsAndJiggles(ON_2x2Matrix(-scale, 0.0), 1, RNG, callback_count, context, Callback))
      return callback_count;
  }

  for (int i = 0; i < maxi; ++i)
  {
    const ON_2x2Matrix R = ON_2x2Matrix::RotationRadians((double(i) / (double)maxi));
    if (false == ON_2x2Matrix_TestPermutationsAndJiggles(R, 2, RNG, callback_count, context, Callback))
      return callback_count;

    if (false == ON_2x2Matrix_TestPermutationsAndJiggles(ON_2x2Matrix(0.125) * R, 2, RNG, callback_count, context, Callback))
      return callback_count;

    if (false == ON_2x2Matrix_TestPermutationsAndJiggles(ON_2x2Matrix(8.0) * R, 2, RNG, callback_count, context, Callback))
      return callback_count;
  }

  const ON_Interval sval_range(-1000.0, 1000.0);
  for (int i = 0; i < 1000; ++i)
  {
    const double Ua = RNG.RandomDouble(ON_Interval::ZeroToTwoPi);
    const ON_2x2Matrix U = ON_2x2Matrix::RotationRadians(Ua);
    const double Va = RNG.RandomDouble(ON_Interval::ZeroToTwoPi);
    const ON_2x2Matrix V = ON_2x2Matrix::RotationRadians(Va).Transpose();
    const double sigma0 = RNG.RandomDouble(sval_range);
    const double sigma1 = RNG.RandomDouble(sval_range);

    const int sigma0_rank = ((0.0 != sigma0) ? 1 : 0);
    const int sigma1_rank = ((0.0 != sigma1) ? 1 : 0);

    if (false == ON_2x2Matrix_TestPermutationsAndJiggles(ON_2x2Matrix::CombineSVD(U, sigma0, sigma1, V), sigma0_rank + sigma1_rank, RNG, callback_count, context, Callback))
      return callback_count;

    if (false == ON_2x2Matrix_TestPermutationsAndJiggles(ON_2x2Matrix::CombineSVD(U, sigma0, 0.0, V), sigma0_rank, RNG, callback_count, context, Callback))
      return callback_count;

    if (false == ON_2x2Matrix_TestPermutationsAndJiggles(ON_2x2Matrix::CombineSVD(U, sigma1, 0.0, V), sigma1_rank, RNG, callback_count, context, Callback))
      return callback_count;
  }

  return callback_count;
}

// do not export ON_2x2MatrixContext
class ON_2x2MatrixContext
{
public:
  // Result of SVD test
  bool m_bSVDPassed = false;

  // Result of comparing Determinant() to product of singular values
  bool m_bDeterminantPassed = false;

  // Result of comparing Rank() to number of nonzero singular values
  bool m_bRankPassed = false;

  // Result of comparing Inverse() to PseudoInverse()
  bool m_bInversePassed = false;

  bool m_bAllPassed = false;

  ON_String m_error_description;

  // m_A.SVD() call failed.
  ON_2x2Matrix m_A = ON_2x2Matrix::Nan; // failed matrix

  // failed SVD
  ON_2x2Matrix m_U = ON_2x2Matrix::Nan;
  double m_sigma[2] = { ON_DBL_QNAN, ON_DBL_QNAN };
  ON_2x2Matrix m_V = ON_2x2Matrix::Nan;

  void Clear()
  {
    m_bSVDPassed = false;
    m_bDeterminantPassed = false;
    m_bRankPassed = false;
    m_bInversePassed = false;
    m_bAllPassed = false;
    m_error_description = ON_String::EmptyString;
    m_A = ON_2x2Matrix::Nan;
  }

  double SVDUDeterminant() const
  {
    const double d = (m_U.m[0][0] * m_U.m[1][1] - m_U.m[0][1] * m_U[1][0]);
    return (d > 0.0) ? 1.0 : (d < 0.0 ? -1.0 : ON_DBL_QNAN);
  }

  double SVDVDeterminant() const
  {
    const double d = (m_V.m[0][0] * m_V.m[1][1] - m_V.m[0][1] * m_V[1][0]);
    return (d > 0.0) ? 1.0 : (d < 0.0 ? -1.0 : ON_DBL_QNAN);
  }

  double SVDDeterminant() const
  {
    return
      (m_sigma[0] >= 0.0 && m_sigma[1] >= 0.0)
      ? (SVDUDeterminant() * SVDVDeterminant() * (m_sigma[0]*m_sigma[1]))
      : ON_DBL_QNAN;
  }

  int SVDRank() const
  {
    return
      (m_sigma[0] > 0.0 && m_sigma[1] > 0.0)
      ? 2
      : ((0.0 == m_sigma[0] && m_sigma[1] > 0.0)
        ? 1
        : ((0.0 == m_sigma[0] && 0.0 == m_sigma[1])
          ? 0
          : -1))
      ;
  }

  const ON_2x2Matrix SVDPseudoInverse() const
  {
    return (m_sigma[0] >= 0.0 || m_sigma[1] >= 0.0)
      ? ON_2x2Matrix::CombineSVD(
        m_V,
        (m_sigma[0] > 0.0 ? 1.0 / m_sigma[0] : 0.0),
        (m_sigma[1] > 0.0 ? 1.0 / m_sigma[1] : 0.0),
        m_U.Transpose())
      : ON_2x2Matrix::Nan;
  }

  double m_determinant = ON_DBL_QNAN;
  ON_2x2Matrix m_inverse = ON_2x2Matrix::Nan;
};

/// <summary>
/// Used to test basic ON_2x2Matrix functionality.
/// </summary>
/// <param name="context_ptr"></param>
/// <param name="A"></param>
/// <returns></returns>
static bool ON_2x2Matrix_TestCallback(ON__UINT_PTR context_ptr, int Arank, const ON_2x2Matrix A)
{
  // Generally test that different ways of computing the same complicated result
  // agree to 8, 10, 12 or 14 decimal places.
  // When the "ideal" rank of A is 2, test to 12 decimal places.
  // When the "ideal" rank of A is 1, test to 8 decimal places.
  // When the "ideal" rank of A is 0, test to 14 decimal places.
  // When the "ideal" A is unknown, test to 10 decimal places.
  const double zero_tolerance 
    = (2 == Arank) ? 1e-12
    : ((1 == Arank) ? 1.0e-8 
      : ((0 == Arank) ? 1e-14 
        : 1.0e-10)); // unknown rank

  ON_2x2MatrixContext context;
  context.m_A = A;
  for (;;)
  {
    if (false == A.SVD(context.m_U, context.m_sigma, context.m_V))
    {
      context.m_error_description = "A.SVD() returned false.";
      break;
    }


    const ON_2x2Matrix UsigmaV = ON_2x2Matrix::CombineSVD(context.m_U, context.m_sigma[0], context.m_sigma[1], context.m_V);
    const ON_2x2Matrix SVD_error = A - UsigmaV;
    const double absolute_SVD_error = SVD_error.MaximumCoefficientAbsoluteValue();

    const double max_A = A.MaximumCoefficientAbsoluteValue();
    const double e
      = (max_A > 1.0 && zero_tolerance > 0.0 && zero_tolerance < ON_UNSET_POSITIVE_VALUE)
      ? (absolute_SVD_error / max_A)
      : absolute_SVD_error;

    if (false == (e <= 1.0e-10))
    {
      context.m_error_description = ON_String::FormatToString("|A - U*Sigma*Transpose(V)| = %g.", absolute_SVD_error);
      break;
    }

    if (false == (context.m_sigma[0] >= 0.0 && context.m_sigma[0] <= context.m_sigma[1]))
    {
      // The singular values are negative or in the wrong order.
      context.m_error_description = "Singular values are not >= 0 and increasing.";
      break;
    }

    const double Uerr = (context.m_U * context.m_U.Transpose() - ON_2x2Matrix::Identity).MaximumCoefficientAbsoluteValue();
    const double Verr = (context.m_V * context.m_V.Transpose() - ON_2x2Matrix::Identity).MaximumCoefficientAbsoluteValue();
    if (false == (Uerr < 1.0e-12))
    {
      // The singular values are negative or in the wrong order.
      context.m_error_description = "SVD U is not orthoganal.";
      break;
    }

    if (false == (Verr <= 1.0e-12))
    {
      context.m_error_description = "SVD V is not orthoganal.";
      break;
    }

    context.m_bSVDPassed = true;

    const double SVDdet = context.SVDDeterminant();
    context.m_determinant = A.Determinant();

    const int SVDrank = context.SVDRank();
    const int rank = A.Rank();

    const double A00xA11 = fabs(A.m[0][0] * A.m[1][1]);
    const double A01xA10 = fabs(A.m[0][1] * A.m[1][0]);
    const double zero_det = ON_EPSILON * (SVDrank < 2 && 2 == rank ? 2.0 : 1.0) * (A00xA11 <= A01xA10 ? A00xA11 : ((A00xA11 > A01xA10) ? A01xA10 : ON_DBL_QNAN));

    const bool bUnstableDet 
      = fabs(context.m_determinant) > 0.0 
      && zero_det >= 0.0 
      && zero_det < ON_UNSET_POSITIVE_VALUE 
      && fabs(context.m_determinant) <= zero_det 
      && SVDdet <= zero_det;

    const bool bUnstableSVDDet 
      = context.m_sigma[0] > 0.0 
      && context.m_sigma[0] <= ON_EPSILON * context.m_sigma[1];

    const bool bUnstableMatrix = bUnstableDet && 2 == rank;
    const bool bUnstableSVD = bUnstableSVDDet && 2 == SVDrank;

    const bool bUnstable = bUnstableMatrix || bUnstableSVD;

    // test that two ways to compute determinant agree to 10 decimal places or
    // the determinant is nearly zero and unstable.
    const double det_tol = zero_tolerance * (0.5 * (fabs(SVDdet) + fabs(context.m_determinant)) + 1.0);
    const double det_delta = fabs(SVDdet - context.m_determinant);
    if (false == (det_delta <= det_tol))
    {
      if (false == bUnstable)
      {
        context.m_error_description = "A.Determinant() != SVDDeterminant().";
        break;
      }
    }
    context.m_bDeterminantPassed = true;

    if (rank != SVDrank)
    {
      if (false == bUnstable)
      {
        context.m_error_description = "A.Rank() != SVDRank().";
        break;
      }
    }
    context.m_bRankPassed = true;

    if (2 == rank)
    {
      const ON_2x2Matrix two(2.0, 2.0);
      context.m_inverse = A.Inverse();
      const ON_2x2Matrix Id1 = A * context.m_inverse;
      const double e1 = (Id1 - ON_2x2Matrix::Identity).MaximumCoefficientAbsoluteValue();
      const ON_2x2Matrix Id2 = context.m_inverse * A;
      const double e2 = (Id2 - ON_2x2Matrix::Identity).MaximumCoefficientAbsoluteValue();

      const double inverse_error = (e1 >= e2) ? e1 : (e2 > e1 ? e2 : ON_DBL_QNAN);
      double inverse_tol = zero_tolerance * (1.0 +  context.m_sigma[1]);
      if (inverse_tol > 1.0e-6)
        inverse_tol = 1.0e-6;

      if (false == (inverse_error <= inverse_tol))
      {
        if (false == bUnstable)
        {
          context.m_error_description = "A * A.Inverse() and/or A.Inverse() * A  is not the identity.";
          break;
        }
      }
    }
    context.m_bInversePassed = true;

    context.m_bAllPassed = true;
    break;
  }

  if (0 != context_ptr)
    *((ON_2x2MatrixContext*)context_ptr) = context;

  return context.m_bAllPassed;
}

static bool ON_2x2Matrix_TestProcrustesRotation(ON_TextLog& text_log)
{
  double max_Rerror = 0.0;
  double max_rmsd = 0.0;
  double max_wigglermsd = 0.0;
  const int jmax = 720;
  ON_RandomNumberGenerator RNG;
  ON_2dVector P0, P1, P2, Q0, Q1, Q2;
  for (int i = 0; i < 100; ++i)
  {
    if (0 == i)
    {
      Q0 = ON_2dVector(-0.5, 0.5);
      Q1 = ON_2dVector(0.5, 0);
      Q2 = ON_2dVector(0, 0.5);
    }
    else
    {
      Q0 = ON_2dVector(RNG.RandomDouble(), RNG.RandomDouble());
      Q1 = ON_2dVector(RNG.RandomDouble(), RNG.RandomDouble());
      Q2 = ON_2dVector(RNG.RandomDouble(), RNG.RandomDouble());
      const ON_2dVector C((Q0.x + Q1.x + Q2.x) / 3.0, (Q0.y + Q1.y + Q2.y) / 3.0);
      Q0 -= C;
      Q1 -= C;
      Q2 -= C;
    }
    const double area = 0.5 * ON_2x2Matrix(Q1.x - Q0.x, Q1.y - Q0.y, Q2.x - Q0.x, Q2.y - Q0.y).Determinant();
    //if (area < 0.0)
    //{
    //  ON_2dVector tmp = Q2;
    //  Q2 = Q1;
    //  Q1 = tmp;
    //}
    for (int j = 0; j < jmax; ++j)
    {
      const double angle = (((double)j) / ((double)jmax)) * ON_2PI;
      const ON_2x2Matrix InverseR = ON_2x2Matrix::RotationDegrees(-angle);
      for (int wiggle = 0; wiggle < 2; ++wiggle)
      {
        P0 = InverseR * Q0;
        P1 = InverseR * Q1;
        P2 = InverseR * Q2;
        const double w = (1 == wiggle) ? sqrt(fabs(area)) / 32.0 : ON_DBL_QNAN;
        const ON_Interval wiggle_room(-w, w);
        if (1 == wiggle)
        {
          P0.x += RNG.RandomDouble(wiggle_room);
          P0.y += RNG.RandomDouble(wiggle_room);
          P1.x += RNG.RandomDouble(wiggle_room);
          P1.y += RNG.RandomDouble(wiggle_room);
          P2.x += RNG.RandomDouble(wiggle_room);
          P2.y += RNG.RandomDouble(wiggle_room);
        }
        const ON_2x2Matrix L = ON_2x2Matrix::ProcrustesRotation(P0, P1, P2, Q0, Q1, Q2);
        const ON_2x2Matrix D(1.0, 0.0, 0.0, L.Determinant() < 0.0 ? -1.0 : 1.0);
        const double Rerror = (L - D * ON_2x2Matrix::RotationDegrees(angle)).MaximumCoefficientAbsoluteValue();
        const ON_2dVector V0 = L * P0 - Q0;
        const ON_2dVector V1 = L * P1 - Q1;
        const ON_2dVector V2 = L * P2 - Q2;
        const double rmsd = sqrt((V0.LengthSquared() + V1.LengthSquared() + V2.LengthSquared()) / 3.0);
        if (1 == wiggle)
        {
          const double normalized_rmsd = w > 0.0 ? (rmsd / w) : 0.0;
          if (false == (normalized_rmsd <= 1.5))
          {
            text_log.Print(
              "Wiggled input ProcrustesRotation() failed on angle = %g. R error = %g, rmsd = %g, wiggle_room = [%g, %g]\n",
              angle, Rerror, rmsd, wiggle_room[0], wiggle_room[1]);
            text_log.Print("Q = (%g,%g), (%g,%g), (%g,%g) area = %g\n",
              Q0.x, Q0.y, Q1.x, Q1.y, Q2.x, Q2.y, area);
            ON_2x2Matrix::ProcrustesRotation(P0, P1, P2, Q0, Q1, Q2);
            return false;
          }
          if (normalized_rmsd > max_wigglermsd)
            max_wigglermsd = normalized_rmsd;
        }
        else
        {
          if (false == (Rerror <= 1.0e-8 && rmsd <= 1.0e-4))
          {
            text_log.Print("Exact input ProcrustesRotation() failed on angle = %g. R error = %g rmsd = %g\n", angle, Rerror, rmsd);
            text_log.Print("Q = (%g,%g), (%g,%g), (%g,%g) area = %g\n",
              Q0.x, Q0.y, Q1.x, Q1.y, Q2.x, Q2.y, area);
            ON_2x2Matrix::ProcrustesRotation(P0, P1, P2, Q0, Q1, Q2);
            return false;
          }
          if (Rerror > max_Rerror)
            max_Rerror = Rerror;
          if (rmsd > max_rmsd)
            max_rmsd = rmsd;
        }
      }
    }
  }
  text_log.Print(
    "All ProcrustesRotation() tests passed. Max R error = %g, max rmsd = %g, max wiggled input normalized rmsd = %g\n",
    max_Rerror, max_rmsd, max_wigglermsd
  );
  return true;
}


bool ON_2x2Matrix::Test(ON_TextLog& text_log)
{
  ON_2x2MatrixContext context;
  const unsigned pass_count = ON_2x2Matrix::TestList((ON__UINT_PTR)&context, ON_2x2Matrix_TestCallback);

  if (context.m_bAllPassed)
  {
    text_log.Print("All %u matrices passed all the ON_2x2Matrix::SVD() tests.\n", pass_count);

    const bool bProcrustesPassed = ON_2x2Matrix_TestProcrustesRotation(text_log);
    return bProcrustesPassed;
  }

  if (pass_count > 0)
    text_log.Print("%u matrices passed the SVD tests.\n", pass_count);

  if (context.m_error_description.IsNotEmpty())
  {
    // failure message
    text_log.PrintString(context.m_error_description);
    text_log.PrintNewLine();
  }

  const ON_2x2Matrix A(context.m_A);

  if (false == context.m_bSVDPassed)
  {

    ON_2x2Matrix U = ON_2x2Matrix::Nan;
    double s[2] = { ON_DBL_QNAN, ON_DBL_QNAN };
    ON_2x2Matrix V = ON_2x2Matrix::Nan;
    context.m_A.SVD(U, s, V); // <- breakpoint here
    const ON_2x2Matrix UsigmaV = ON_2x2Matrix::CombineSVD(U, s[0], s[1], V);
    const double A_error = (A - UsigmaV).MaximumCoefficientAbsoluteValue();

    text_log.Print("ON_2x2Matrix.SVD() error = %g.\n", A_error);
  }
  else if (false == context.m_bDeterminantPassed)
  {
    const double SVDdet = context.SVDDeterminant();
    const double Adet = context.m_A.Determinant();
    const double e = fabs(context.m_determinant - SVDdet);
    text_log.Print("ON_2x2Matrix.Determinant() error = %g. Determinant() = %g, SVD det = %g.\n", e, Adet, SVDdet);
  }
  else if (false == context.m_bRankPassed)
  {
    const int SVDrank = context.SVDRank();
    const int Arank = context.m_A.Rank();
    text_log.Print("ON_2x2Matrix.Rank() error. Rank() = %d, SVD rank = %d.\n", Arank, SVDrank);
  }
  else if (false == context.m_bInversePassed)
  {
    //const ON_2x2Matrix PseudoInverse = context.SVDPseudoInverse();
    const ON_2x2Matrix AInverse = context.m_A.Inverse();
    const ON_2x2Matrix Id1 = A * AInverse;
    const ON_2x2Matrix Id2 = AInverse * A;
    const double e1 = (Id1 - ON_2x2Matrix::Identity).MaximumCoefficientAbsoluteValue();
    const double e2 = (Id2 - ON_2x2Matrix::Identity).MaximumCoefficientAbsoluteValue();
    const double e = (e1 >= e2) ? e1 : (e2 > e1 ? e2 : ON_DBL_QNAN);
    text_log.Print("ON_2x2Matrix.Inverse() error = %g.\n", e);
  }

  return false;
}


// 8 July 2003 Dale Lear
//    changed ON_Matrix to use multiple allocations
//    for coefficient memory in large matrices.

// ON_Matrix.m_cmem points to a struct DBLBLK
struct DBLBLK
{
  int count;
  double* a;
  struct DBLBLK *next;
};

double const * const * ON_Matrix::ThisM() const
{
  // When the "expert" constructor Create(row_count,col_count,user_memory,...)
  // is used, m_rowmem[] is empty and m = user_memory;
  // When any other constructor is used, m_rowmem[] is the 0-based
  // row memory.
  return (m_row_count == m_rowmem.Count()) ? m_rowmem.Array() : m;
}

double * * ON_Matrix::ThisM()
{
  // When the "expert" constructor Create(row_count,col_count,user_memory,...)
  // is used, m_rowmem[] is empty and m = user_memory;
  // When any other constructor is used, m_rowmem[] is the 0-based
  // row memory.
  return (m_row_count == m_rowmem.Count()) ? m_rowmem.Array() : m;
}


double* ON_Matrix::operator[](int i)
{
  return m[i];
}

const double* ON_Matrix::operator[](int i) const
{
  return m[i];
}

ON_Matrix::ON_Matrix()
: m(0)
, m_row_count(0)
, m_col_count(0)
, m_Mmem(0)
, m_row_offset(0)
, m_col_offset(0)
, m_cmem(0)
{
}

ON_Matrix::ON_Matrix( int row_size, int col_size ) 
: m(0)
, m_row_count(0)
, m_col_count(0)
, m_Mmem(0)
, m_row_offset(0)
, m_col_offset(0)
, m_cmem(0)
{
  Create(row_size,col_size);
}

ON_Matrix::ON_Matrix( int row_size, int col_size, double fill_value ) 
: m(0)
, m_row_count(0)
, m_col_count(0)
, m_Mmem(0)
, m_row_offset(0)
, m_col_offset(0)
, m_cmem(0)
{
  Create(row_size,col_size,fill_value);
}

ON_Matrix::ON_Matrix( int row0, int row1, int col0, int col1 ) 
: m(0)
, m_row_count(0)
, m_col_count(0)
, m_Mmem(0)
, m_row_offset(0)
, m_col_offset(0)
, m_cmem(0)
{
  Create(row0,row1,col0,col1);
}

ON_Matrix::ON_Matrix( const ON_Xform& x ) 
: m(0)
, m_row_count(0)
, m_col_count(0)
, m_Mmem(0)
, m_row_offset(0)
, m_col_offset(0)
, m_cmem(0)
{
  *this = x;
}

ON_Matrix::ON_Matrix( const ON_Matrix& src )
: m(0)
, m_row_count(0)
, m_col_count(0)
, m_Mmem(0)
, m_row_offset(0)
, m_col_offset(0)
, m_cmem(0)
{
  *this = src;
}

ON_Matrix::ON_Matrix(
  int row_count,
  int col_count,
  double** M,
  bool bDestructorFreeM
  )
: m(0)
, m_row_count(0)
, m_col_count(0)
, m_Mmem(0)
, m_row_offset(0)
, m_col_offset(0)
, m_cmem(0)
{
  Create(row_count,col_count,M,bDestructorFreeM);
}

ON_Matrix::~ON_Matrix()
{
  if ( 0 != m_Mmem )
  {
    onfree(m_Mmem);
    m_Mmem = 0;
  }
  m_row_offset = 0;
  m_col_offset = 0;
  struct DBLBLK* p = (struct DBLBLK*)m_cmem;
  m_cmem = 0;
  while(0 != p)
  {
    struct DBLBLK* next = p->next;
    onfree(p);
    p = next;
  }
}

#if defined(ON_HAS_RVALUEREF)

ON_Matrix::ON_Matrix(ON_Matrix&& src) ON_NOEXCEPT
  : m(src.m)
  , m_row_count(src.m_row_count)
  , m_col_count(src.m_col_count)
  , m_rowmem(std::move(src.m_rowmem))
  , m_Mmem(src.m_Mmem)
  , m_row_offset(src.m_row_offset)
  , m_cmem(src.m_cmem)
{
  src.m = nullptr;
  src.m_row_count = 0;
  src.m_col_count = 0;
  // src.m_rowmem - std::move insures src.m_rowmem is zeroed
  src.m_Mmem = nullptr;
  src.m_row_offset = 0;
  src.m_cmem = nullptr;
}

ON_Matrix& ON_Matrix::operator=(ON_Matrix&& src)
{
  if (this != &src)
  {
    m = src.m;
    m_row_count = src.m_row_count;
    m_col_count = src.m_col_count;
    m_rowmem = std::move(src.m_rowmem);
    m_Mmem = src.m_Mmem;
    m_row_offset = src.m_row_offset;
    m_cmem = src.m_cmem;

    src.m = nullptr;
    src.m_row_count = 0;
    src.m_col_count = 0;
    src.m_Mmem = nullptr;
    src.m_row_offset = 0;
    src.m_cmem = nullptr;
  }
  return *this;
}

#endif


double** ON_Matrix::Allocate(
  unsigned int row_count,
  unsigned int col_count
  )
{
  if (row_count < 1 || row_count >= ON_UNSET_UINT_INDEX / 2)
    return nullptr;
  if (col_count < 1 || col_count >= ON_UNSET_UINT_INDEX / 2)
    return nullptr;
  double** M;
  size_t sizeof_element = sizeof(M[0][0]);
  size_t sizeof_ptr = sizeof(M[0]);
  size_t sz0 = row_count*sizeof_ptr;
  if (0 != sz0 % sizeof_element)
    sz0 += sizeof_ptr;
  size_t sz1 = row_count*col_count*sizeof_element;
  if (0 != sz1 % sizeof_ptr)
    sz1 += sizeof_ptr;
  M = new (std::nothrow) double*[(sz0 + sz1) / sizeof_ptr];
  if (nullptr == M)
    return nullptr;
  double* row = (double*)(((char*)M) + sz0);
  for (unsigned int i = 0; i < row_count; i++)
  {
    M[i] = row;
    row += col_count;
  }
  return M;
}

void ON_Matrix::Deallocate(double** M)
{
  if (nullptr != M)
  {
    delete[] M;
  }
}

int ON_Matrix::RowCount() const
{
  return m_row_count;
}

int ON_Matrix::ColCount() const
{
  return m_col_count;
}

int ON_Matrix::MinCount() const
{
  return (m_row_count <= m_col_count) ? m_row_count : m_col_count;
}

int ON_Matrix::MaxCount() const
{
  return (m_row_count >= m_col_count) ? m_row_count : m_col_count;
}

unsigned int ON_Matrix::UnsignedRowCount() const
{
  return (m_row_count>0) ? (unsigned int)m_row_count : 0;
}

unsigned int ON_Matrix::UnsignedColCount() const
{
  return (m_col_count>0) ? (unsigned int)m_col_count : 0;
}

unsigned int ON_Matrix::UnsignedMinCount() const
{
  const unsigned int rc[2] = { UnsignedRowCount(), UnsignedColCount() };
  return (rc[0] < rc[1]) ? rc[0] : rc[1];
}

unsigned int ON_Matrix::UnsignedMaxCount() const
{
  const unsigned int rc[2] = { UnsignedRowCount(), UnsignedColCount() };
  return (rc[0] > rc[1]) ? rc[0] : rc[1];
}

bool ON_Matrix::Create( int row_count, int col_count)
{
  bool b = false;
  Destroy();
  if ( row_count > 0 && col_count > 0 ) 
  {
    m_rowmem.Reserve(row_count);
    if ( 0 != m_rowmem.Array() )
    {
      m_rowmem.SetCount(row_count);
      // In general, allocate coefficient memory in chunks 
      // of <= max_dblblk_size bytes.  The value of max_dblblk_size
      // is tuned to maximize speed on calculations involving
      // large matrices.  If all of the coefficients will fit
      // into a chunk of memory <= 1.1*max_dblblk_size, then 
      // a single chunk is allocated.
      
      // In limited testing, these two values appeared to work ok.
      // The latter was a hair faster in solving large row reduction
      // problems (for reasons I do not understand).
      //const int max_dblblk_size = 1024*1024*8;
      const int max_dblblk_size = 512*1024;

      int rows_per_block = max_dblblk_size/(col_count*sizeof(double));
      if ( rows_per_block > row_count )
        rows_per_block = row_count;
      else if ( rows_per_block < 1 )
        rows_per_block = 1;
      else if ( rows_per_block < row_count && 11*rows_per_block >= 10*row_count )
        rows_per_block = row_count;

      int j, i = row_count; 
      m = m_rowmem.Array();
      double** row = m;
      for ( i = row_count; i > 0; i -= rows_per_block )
      {
        if ( i < rows_per_block )
          rows_per_block = i;
        int dblblk_count = rows_per_block*col_count;
        // TODO: Make this malloc size a static on the DBLBLK class, this is too ugly
        struct DBLBLK* p = (struct DBLBLK*)onmalloc(sizeof(*p) + dblblk_count*sizeof(p->a[0]));
        p->a = (double*)(p+1);
        p->count = dblblk_count;
        p->next = (struct DBLBLK*)m_cmem;
        m_cmem = p;
        *row = p->a;
        j = rows_per_block-1;
        while(j--)
        {
          row[1] = row[0] + col_count;
          row++;
        }
        row++;
      }
      m_row_count = row_count;
      m_col_count = col_count;
      b = true;
    }
  }
  return b;
}

bool ON_Matrix::Create(
  int row_count,
  int col_count,
  double fill_value
)
{
  bool b = Create(row_count, col_count);
  if (b) Fill(fill_value);
  return b;
}

bool ON_Matrix::Create( // E.g., Create(1,5,1,7) creates a 5x7 sized matrix that with
             // "top" row = m[1][1],...,m[1][7] and "bottom" row
             // = m[5][1],...,m[5][7].  The result of Create(0,m,0,n) is
             // identical to the result of Create(m+1,n+1).
   int ri0, // first valid row index
   int ri1, // last valid row index
   int ci0, // first valid column index
   int ci1 // last valid column index
   )
{
  bool b = false;
  if ( ri1 > ri0 && ci1 > ci0 ) 
  {
    // juggle m[] pointers so that m[ri0+i][ci0+j] = m_row[i][j];
    b = Create( ri1-ri0, ci1-ci0 );
    if (b)
    {
      m_row_offset = ri0; // this is the only line of code where m_row_offset should be set to a non-zero value
      m_col_offset = ci0; // this is the only line of code where m_col_offset should be set to a non-zero value
      if ( ci0 != 0 )
      {
        int i;
        for ( i = 0; i < m_row_count; i++ ) {
          m[i] -= ci0;
        }
      }
      if ( ri0 != 0 )
        m -= ri0;
    }
  }
  return b;
}

bool ON_Matrix::Create(
  int row_count,
  int col_count,
  double** M,
  bool bDestructorFreeM
  )
{
  Destroy();
  if ( row_count < 1 || col_count < 1 || 0 == M )
    return false;
  m = M;
  m_row_count = row_count;
  m_col_count = col_count;
  if ( bDestructorFreeM )
    m_Mmem = M;
  return true;
}


void ON_Matrix::Destroy()
{
  m = 0;
  m_row_count = 0;
  m_col_count = 0;
  m_rowmem.SetCount(0);
  if ( 0 != m_Mmem )
  {
    // pointer passed to Create( row_count, col_count, M, bDestructorFreeM )
    // when bDestructorFreeM = true.
    onfree(m_Mmem);
    m_Mmem = 0;
  }
	m_row_offset = 0;
	m_col_offset = 0;
  struct DBLBLK* cmem = (struct DBLBLK*)m_cmem;
  m_cmem = 0;
  while( 0 != cmem )
  {
    struct DBLBLK* next_cmem = cmem->next;
    onfree(cmem);
    cmem = next_cmem;
  }
}

void ON_Matrix::EmergencyDestroy()
{
  // call if memory pool used matrix by becomes invalid
  m = 0;
  m_row_count = 0;
  m_col_count = 0;
  m_rowmem.EmergencyDestroy();
	m_Mmem = 0;
	m_row_offset = 0;
	m_col_offset = 0;
  m_cmem = 0;
}

ON_Matrix& ON_Matrix::operator=(const ON_Matrix& src)
{
  if ( this != &src ) 
  {
    if ( src.m_row_count != m_row_count || src.m_col_count != m_col_count || 0 == m )
    {
      Destroy();
      Create( src.RowCount(), src.ColCount() );
    }
    if (src.m_row_count == m_row_count && src.m_col_count == m_col_count && 0 != m )
    {
      int i;
      // src rows may be permuted - copy row by row
      double** m_dest = ThisM();
      double const*const* m_src = src.ThisM();
      const int sizeof_row = m_col_count*sizeof(m_dest[0][0]);
      for ( i = 0; i < m_row_count; i++ ) 
      {
        memcpy( m_dest[i], m_src[i], sizeof_row );
      }
      m_row_offset = src.m_row_offset;
      m_col_offset = src.m_col_offset;
    }
  }
  return *this;
}

ON_Matrix& ON_Matrix::operator=(const ON_Xform& src)
{
  m_row_offset = 0;
  m_col_offset = 0;
  if ( 4 != m_row_count || 4 != m_col_count || 0 == m )
  {
    Destroy();
    Create( 4, 4 );
  }
  if ( 4 == m_row_count && 4 == m_col_count && 0 != m )
  {
    double** this_m = ThisM();
    if ( this_m )
    {
      memcpy( this_m[0], &src.m_xform[0][0], 4*sizeof(this_m[0][0]) );
      memcpy( this_m[1], &src.m_xform[1][0], 4*sizeof(this_m[0][0]) );
      memcpy( this_m[2], &src.m_xform[2][0], 4*sizeof(this_m[0][0]) );
      memcpy( this_m[3], &src.m_xform[3][0], 4*sizeof(this_m[0][0]) );
    }
  }
  return *this;
}

bool ON_Matrix::Transpose()
{
  bool rc = false;
  int i, j;
  double t;
  const int row_count = RowCount();
  const int col_count = ColCount();
  if ( row_count > 0 && col_count > 0 ) 
  {
    double** this_m = ThisM();
    if ( row_count == col_count )
    {
      rc = true;
      for ( i = 0; i < row_count; i++ ) for ( j = i+1; j < row_count; j++ )
      {
        t = this_m[i][j]; this_m[i][j] = this_m[j][i]; this_m[j][i] = t;
      }
    }
    else if ( this_m == m_rowmem.Array() )
    {
      ON_Matrix A(*this);
      rc = Create(col_count,row_count) 
           && m_row_count == A.ColCount()
           && m_col_count == A.RowCount();
      if (rc)
      {
        double const*const* Am = A.ThisM();
  		  this_m = ThisM(); // Create allocates new memory
        for ( i = 0; i < row_count; i++ ) for ( j = 0; j < col_count; j++ ) 
        {
          this_m[j][i] = Am[i][j];
        }
        m_row_offset = A.m_col_offset;
        m_col_offset = A.m_row_offset;
      }
      else
      {
        // attempt to put values back
        *this = A;
      }
    }
  }
  return rc;
}

bool ON_Matrix::SwapRows( int row0, int row1 )
{
  bool b = false;
  double** this_m = ThisM();
  row0 -= m_row_offset;
  row1 -= m_row_offset;
  if ( this_m && 0 <= row0 && row0 < m_row_count && 0 <= row1 && row1 < m_row_count )
  {
    if ( row0 != row1 )
    {
      double* tmp = this_m[row0]; this_m[row0] = this_m[row1]; this_m[row1] = tmp;
    }
    b = true;
  }
  return b;
}

bool ON_Matrix::SwapCols( int col0, int col1 )
{
  bool b = false;
  int i;
  double t;
  double** this_m = ThisM();
  col0 -= m_col_offset;
  col1 -= m_col_offset;
  if ( this_m && 0 <= col0 && col0 < m_col_count && 0 <= col1 && col1 < m_col_count ) 
  {
    if ( col0 != col1 ) 
    {
      for ( i = 0; i < m_row_count; i++ )
      {
        t = this_m[i][col0]; this_m[i][col0] = this_m[i][col1]; this_m[i][col1] = t;
      }
    }
    b = true;
  }
  return b;
}

void ON_Matrix::RowScale( int dest_row, double s )
{
  double** this_m = ThisM();
  dest_row -= m_row_offset;
  ON_ArrayScale( m_col_count, s, this_m[dest_row], this_m[dest_row] );
}

void ON_Matrix::RowOp( int dest_row, double s, int src_row )
{
  double** this_m = ThisM();
  dest_row -= m_row_offset;
  src_row -= m_row_offset;
  ON_Array_aA_plus_B( m_col_count, s, this_m[src_row], this_m[dest_row], this_m[dest_row] );
}

void ON_Matrix::ColScale( int dest_col, double s )
{
  int i;
  double** this_m = ThisM();
  dest_col -= m_col_offset;
  for ( i = 0; i < m_row_count; i++ ) 
  {
    this_m[i][dest_col] *= s;
  }
}

void ON_Matrix::ColOp( int dest_col, double s, int src_col )
{
  int i;
  double** this_m = ThisM();
  dest_col -= m_col_offset;
  src_col  -= m_col_offset;
  for ( i = 0; i < m_row_count; i++ ) 
  {
    this_m[i][dest_col] += s*this_m[i][src_col];
  }
}

int
ON_Matrix::RowReduce( 
    double zero_tolerance,
    double& determinant,
    double& pivot 
    )
{
  double x, piv, det;
  int i, k, ix, rank;

  double** this_m = ThisM();
  piv = det = 1.0;
  rank = 0;
  const int n = m_row_count <= m_col_count ? m_row_count : m_col_count;
  for ( k = 0; k < n; k++ ) {
    ix = k;
    x = fabs(this_m[ix][k]);
    for ( i = k+1; i < m_row_count; i++ ) {
      if ( fabs(this_m[i][k]) > x ) {
        ix = i;
        x = fabs(this_m[ix][k]);
      }
    }
    if ( x < piv || k == 0 ) {
      piv = x;
    }
    if ( x <= zero_tolerance ) {
      det = 0.0;
      break;
    }
    rank++;

    if ( ix != k )
    {
      // swap rows
      SwapRows( ix, k );
      det = -det;
    }

    // scale row k of matrix and B
    det *= this_m[k][k];
    x = 1.0/this_m[k][k];
    this_m[k][k] = 1.0;
    ON_ArrayScale( m_col_count - 1 - k, x, &this_m[k][k+1], &this_m[k][k+1] );

    // zero column k for rows below this_m[k][k]
    for ( i = k+1; i < m_row_count; i++ ) {
      x = -this_m[i][k];
      this_m[i][k] = 0.0;
      if ( fabs(x) > zero_tolerance ) {
        ON_Array_aA_plus_B( m_col_count - 1 - k, x, &this_m[k][k+1], &this_m[i][k+1], &this_m[i][k+1] );
      }
    }
  }

  pivot = piv;
  determinant = det;

  return rank;
}

int
ON_Matrix::RowReduce( 
    double zero_tolerance,
    double* B,
    double* pivot 
    )
{
  double t;
  double x, piv;
  int i, k, ix, rank;

  double** this_m = ThisM();
  piv = 0.0;
  rank = 0;
  const int n = m_row_count <= m_col_count ? m_row_count : m_col_count;
  for ( k = 0; k < n; k++ ) {
    ix = k;
    x = fabs(this_m[ix][k]);
    for ( i = k+1; i < m_row_count; i++ ) {
      if ( fabs(this_m[i][k]) > x ) {
        ix = i;
        x = fabs(this_m[ix][k]);
      }
    }
    if ( x < piv || k == 0 ) {
      piv = x;
    }
    if ( x <= zero_tolerance )
      break;
    rank++;

    if ( ix != k )
    {
      // swap rows of matrix and B
      SwapRows( ix, k );
      t = B[ix]; B[ix] = B[k]; B[k] = t;
    }

    // scale row k of matrix and B
    x = 1.0/this_m[k][k];
    this_m[k][k] = 1.0;
    ON_ArrayScale( m_col_count - 1 - k, x, &this_m[k][k+1], &this_m[k][k+1] );
    B[k] *= x;

    // zero column k for rows below this_m[k][k]
    for ( i = k+1; i < m_row_count; i++ ) {
      x = -this_m[i][k];
      this_m[i][k] = 0.0;
      if ( fabs(x) > zero_tolerance ) {
        ON_Array_aA_plus_B( m_col_count - 1 - k, x, &this_m[k][k+1], &this_m[i][k+1], &this_m[i][k+1] );
        B[i] += x*B[k];
      }
    }
  }

  if ( pivot )
    *pivot = piv;

  return rank;
}

int
ON_Matrix::RowReduce( 
    double zero_tolerance,
    ON_3dPoint* B,
    double* pivot 
    )
{
  ON_3dPoint t;
  double x, piv;
  int i, k, ix, rank;

  double** this_m = ThisM();
  piv = 0.0;
  rank = 0;
  const int n = m_row_count <= m_col_count ? m_row_count : m_col_count;
  for ( k = 0; k < n; k++ ) {
    //onfree( onmalloc( 1)); // 8-06-03 lw for cancel thread responsiveness
    onmalloc( 0); // 9-4-03 lw changed to 0
    ix = k;
    x = fabs(this_m[ix][k]);
    for ( i = k+1; i < m_row_count; i++ ) {
      if ( fabs(this_m[i][k]) > x ) {
        ix = i;
        x = fabs(this_m[ix][k]);
      }
    }
    if ( x < piv || k == 0 ) {
      piv = x;
    }
    if ( x <= zero_tolerance )
      break;
    rank++;

    if ( ix != k )
    {
      // swap rows of matrix and B
      SwapRows( ix, k );
      t = B[ix]; B[ix] = B[k]; B[k] = t;
    }

    // scale row k of matrix and B
    x = 1.0/this_m[k][k];
    this_m[k][k] = 1.0;
    ON_ArrayScale( m_col_count - 1 - k, x, &this_m[k][k+1], &this_m[k][k+1] );
    B[k] *= x;

    // zero column k for rows below this_m[k][k]
    for ( i = k+1; i < m_row_count; i++ ) {
      x = -this_m[i][k];
      this_m[i][k] = 0.0;
      if ( fabs(x) > zero_tolerance ) {
        ON_Array_aA_plus_B( m_col_count - 1 - k, x, &this_m[k][k+1], &this_m[i][k+1], &this_m[i][k+1] );
        B[i] += x*B[k];
      }
    }
  }

  if ( pivot )
    *pivot = piv;

  return rank;
}

int
ON_Matrix::RowReduce( 
    double zero_tolerance,
    int pt_dim, int pt_stride, double* pt,
    double* pivot 
    )
{
  const int sizeof_pt = pt_dim*sizeof(pt[0]);
  double* tmp_pt = (double*)onmalloc(pt_dim*sizeof(tmp_pt[0]));
  double *ptA, *ptB;
  double x, piv;
  int i, k, ix, rank, pti;

  double** this_m = ThisM();
  piv = 0.0;
  rank = 0;
  const int n = m_row_count <= m_col_count ? m_row_count : m_col_count;
  for ( k = 0; k < n; k++ ) {
//    onfree( onmalloc( 1)); //  8-06-03 lw for cancel thread responsiveness
    onmalloc( 0); // 9-4-03 lw changed to 0
    ix = k;
    x = fabs(this_m[ix][k]);
    for ( i = k+1; i < m_row_count; i++ ) {
      if ( fabs(this_m[i][k]) > x ) {
        ix = i;
        x = fabs(this_m[ix][k]);
      }
    }
    if ( x < piv || k == 0 ) {
      piv = x;
    }
    if ( x <= zero_tolerance )
      break;
    rank++;

    // swap rows of matrix and B
    if ( ix != k ) {
      SwapRows( ix, k );
      ptA = pt + (ix*pt_stride);
      ptB = pt + (k*pt_stride);
      memcpy( tmp_pt, ptA, sizeof_pt );
      memcpy( ptA, ptB, sizeof_pt );
      memcpy( ptB, tmp_pt, sizeof_pt );
    }

    // scale row k of matrix and B
    x = 1.0/this_m[k][k];
    if ( x != 1.0 ) {
      this_m[k][k] = 1.0;
      ON_ArrayScale( m_col_count - 1 - k, x, &this_m[k][k+1], &this_m[k][k+1] );
      ptA = pt + (k*pt_stride);
      for ( pti = 0; pti < pt_dim; pti++ )
        ptA[pti] *= x;
    }

    // zero column k for rows below this_m[k][k]
    ptB = pt + (k*pt_stride);
    for ( i = k+1; i < m_row_count; i++ ) {
      x = -this_m[i][k];
      this_m[i][k] = 0.0;
      if ( fabs(x) > zero_tolerance ) {
        ON_Array_aA_plus_B( m_col_count - 1 - k, x, &this_m[k][k+1], &this_m[i][k+1], &this_m[i][k+1] );
        ptA = pt + (i*pt_stride);
        for ( pti = 0; pti < pt_dim; pti++ ) {
          ptA[pti] += x*ptB[pti];
        }
      }
    }
  }

  if ( pivot )
    *pivot = piv;

  onfree(tmp_pt);

  return rank;
}


bool
ON_Matrix::BackSolve( 
    double zero_tolerance,
    int Bsize,
    const double* B,
    double* X
    ) const
{
  int i;

  if ( m_col_count > m_row_count )
    return false; // under determined

  if ( Bsize < m_col_count || Bsize > m_row_count )
    return false; // under determined

  for ( i = m_col_count; i < Bsize; i++ ) {
    if ( fabs(B[i]) > zero_tolerance )
      return false; // over determined
  }

  // backsolve
  double const*const* this_m = ThisM();
  const int n = m_col_count-1;
  if ( X != B )
    X[n] = B[n];
  for ( i = n-1; i >= 0; i-- ) {
    X[i] = B[i] - ON_ArrayDotProduct( n-i, &this_m[i][i+1], &X[i+1] );
  }

  return true;
}

bool
ON_Matrix::BackSolve( 
    double zero_tolerance,
    int Bsize,
    const ON_3dPoint* B,
    ON_3dPoint* X
    ) const
{
  int i, j;

  if ( m_col_count > m_row_count )
    return false; // under determined

  if ( Bsize < m_col_count || Bsize > m_row_count )
    return false; // under determined

  for ( i = m_col_count; i < Bsize; i++ ) {
    if ( B[i].MaximumCoordinate() > zero_tolerance )
      return false; // over determined
  }

  // backsolve
  double const*const* this_m = ThisM();
  if ( X != B )
  {
    X[m_col_count-1] = B[m_col_count-1];
    for ( i = m_col_count-2; i >= 0; i-- ) {
      X[i] = B[i];
      for ( j = i+1; j < m_col_count; j++ ) {
        X[i] -= this_m[i][j]*X[j];
      }
    }
  }
  else {
    for ( i = m_col_count-2; i >= 0; i-- ) {
      for ( j = i+1; j < m_col_count; j++ ) {
        X[i] -= this_m[i][j]*X[j];
      }
    }
  }

  return true;
}


bool
ON_Matrix::BackSolve( 
    double zero_tolerance,
    int pt_dim,
    int Bsize,
    int Bpt_stride,
    const double* Bpt,
    int Xpt_stride,
    double* Xpt
    ) const
{
  const int sizeof_pt = pt_dim*sizeof(double);
  double mij;
  int i, j, k;
  const double* Bi;
  double* Xi;
  double* Xj;

  if ( m_col_count > m_row_count )
    return false; // under determined

  if ( Bsize < m_col_count || Bsize > m_row_count )
    return false; // under determined

  for ( i = m_col_count; i < Bsize; i++ ) 
  {
    Bi = Bpt + i*Bpt_stride;
    for( j = 0; j < pt_dim; j++ )
    {
      if ( fabs(Bi[j]) > zero_tolerance )
        return false; // over determined
    }
  }

  // backsolve
  double const*const* this_m = ThisM();
  if ( Xpt != Bpt )
  {
    Xi = Xpt + (m_col_count-1)*Xpt_stride;
    Bi = Bpt + (m_col_count-1)*Bpt_stride;
    memcpy(Xi,Bi,sizeof_pt);
    for ( i = m_col_count-2; i >= 0; i-- ) {
      Xi = Xpt + i*Xpt_stride;
      Bi = Bpt + i*Bpt_stride;
      memcpy(Xi,Bi,sizeof_pt);
      for ( j = i+1; j < m_col_count; j++ ) {
        Xj = Xpt + j*Xpt_stride;
        mij = this_m[i][j];
        for ( k = 0; k < pt_dim; k++ )
          Xi[k] -= mij*Xj[k];
      }
    }
  }
  else {
    for ( i = m_col_count-2; i >= 0; i-- ) {
      Xi = Xpt + i*Xpt_stride;
      for ( j = i+1; j < m_col_count; j++ ) {
        Xj = Xpt + j*Xpt_stride;
        mij = this_m[i][j];
        for ( k = 0; k < pt_dim; k++ )
          Xi[k] -= mij*Xj[k];
      }
    }
  }

  return true;
}


void ON_Matrix::Zero()
{
  struct DBLBLK* cmem = (struct DBLBLK*)m_cmem;
  while ( 0 != cmem )
  {
    if ( 0 != cmem->a && cmem->count > 0 )
    {
      memset( cmem->a, 0, cmem->count*sizeof(cmem->a[0]) );
    }
    onmalloc(0);  // allow canceling
    cmem = cmem->next;
  }

  //m_a.Zero();
}

void ON_Matrix::Fill(double d)
{
  struct DBLBLK* cmem = (struct DBLBLK*)m_cmem;
  while (0 != cmem)
  {
    if (0 != cmem->a && cmem->count > 0)
    {
      for (int i = 0; i < cmem->count; ++i)
      {
        cmem->a[i] = d;
      }
    }
    onmalloc(0);  // allow canceling
    cmem = cmem->next;
  }
}

void ON_Matrix::SetDiagonal( double d)
{
  const int n = MinCount();
  int i;
  Zero();
  double** this_m = ThisM();
  for ( i = 0; i < n; i++ ) 
  {
    this_m[i][i] = d;
  }
}

void ON_Matrix::SetDiagonal( const double* d )
{
  Zero();
  if (d) 
  {
    double** this_m = ThisM();
    const int n = MinCount();
    int i;
    for ( i = 0; i < n; i++ )
    {
      this_m[i][i] = *d++;
    }
  }
}

void ON_Matrix::SetDiagonal( int count, const double* d )
{
  Create(count,count);
  Zero();
  SetDiagonal(d);
}

void ON_Matrix::SetDiagonal( const ON_SimpleArray<double>& a )
{
  SetDiagonal( a.Count(), a.Array() );
}

bool ON_Matrix::IsValid() const
{
  if ( m_row_count < 1 || m_col_count < 1 )
    return false;
  if ( 0 == m )
    return false;
  return true;
}

int ON_Matrix::IsSquare() const
{
  return ( m_row_count > 0 && m_col_count == m_row_count ) ? m_row_count : 0;
}

bool ON_Matrix::IsRowOrthoganal() const
{
  double d0, d1, d;
  int i0, i1, j;
  double const*const* this_m = ThisM();
  bool rc = ( m_row_count <= m_col_count && m_row_count > 0 );
  for ( i0 = 0; i0 < m_row_count && rc; i0++ ) for ( i1 = i0+1; i1 < m_row_count && rc; i1++ ) {
    d0 = d1 = d = 0.0;
    for ( j = 0; j < m_col_count; j++ ) {
      d0 += fabs(this_m[i0][j]);
      d1 += fabs(this_m[i0][j]);
      d  += this_m[i0][j]*this_m[i1][j];
    }
    if ( d0 <=  ON_EPSILON || d1 <=  ON_EPSILON || fabs(d) >= d0*d1* ON_SQRT_EPSILON )
      rc = false;
  }
  return rc;
}

bool ON_Matrix::IsRowOrthoNormal() const
{
  double d;
  int i, j;
  bool rc = IsRowOrthoganal();
  if ( rc ) {
    double const*const* this_m = ThisM();
    for ( i = 0; i < m_row_count; i++ ) {
      d = 0.0;
      for ( j = 0; j < m_col_count; j++ ) {
        d += this_m[i][j]*this_m[i][j];
      }
      if ( fabs(1.0-d) >=  ON_SQRT_EPSILON )
        rc = false;
    }
  }
  return rc;
}

bool ON_Matrix::IsColOrthoganal() const
{
  double d0, d1, d;
  int i, j0, j1;
  bool rc = ( m_col_count <= m_row_count && m_col_count > 0 );
  double const*const* this_m = ThisM();
  for ( j0 = 0; j0 < m_col_count && rc; j0++ ) for ( j1 = j0+1; j1 < m_col_count && rc; j1++ ) {
    d0 = d1 = d = 0.0;
    for ( i = 0; i < m_row_count; i++ ) {
      d0 += fabs(this_m[i][j0]);
      d1 += fabs(this_m[i][j0]);
      d  += this_m[i][j0]*this_m[i][j1];
    }
    if ( d0 <=  ON_EPSILON || d1 <=  ON_EPSILON || fabs(d) >  ON_SQRT_EPSILON )
      rc = false;
  }
  return rc;
}

bool ON_Matrix::IsColOrthoNormal() const
{
  double d;
  int i, j;
  bool rc = IsColOrthoganal();
  double const*const* this_m = ThisM();
  if ( rc ) {
    for ( j = 0; j < m_col_count; j++ ) {
      d = 0.0;
      for ( i = 0; i < m_row_count; i++ ) {
        d += this_m[i][j]*this_m[i][j];
      }
      if ( fabs(1.0-d) >=  ON_SQRT_EPSILON )
        rc = false;
    }
  }
  return rc;
}

bool ON_Matrix::Invert( double zero_tolerance )
{
  ON_Workspace ws;
  int i, j, k, ix, jx, rank;
  double x;
  const int n = MinCount();
  if ( n < 1 )
    return false;

  ON_Matrix I(m_col_count, m_row_count);

  int* col = ws.GetIntMemory(n);

  I.SetDiagonal(1.0);
  rank = 0;

  double** this_m = ThisM();

  for ( k = 0; k < n; k++ ) {
    // find largest value in sub matrix
    ix = jx = k;
    x = fabs(this_m[ix][jx]);
    for ( i = k; i < n; i++ ) {
      for ( j = k; j < n; j++ ) {
        if ( fabs(this_m[i][j]) > x ) {
          ix = i;
          jx = j;
          x = fabs(this_m[ix][jx]);
        }
      }
    }

    SwapRows( k, ix );
    I.SwapRows( k, ix );

    SwapCols( k, jx );
    col[k] = jx;

    if ( x <= zero_tolerance ) {
      break;
    }
    x = 1.0/this_m[k][k];
    this_m[k][k] = 1.0;
    ON_ArrayScale( m_col_count-k-1, x, &this_m[k][k+1], &this_m[k][k+1] );
    I.RowScale( k, x );

    // zero this_m[!=k][k]'s 
    for ( i = 0; i < n; i++ ) {
      if ( i != k ) {
        x = -this_m[i][k];
        this_m[i][k] = 0.0;
        if ( fabs(x) > zero_tolerance ) {
          ON_Array_aA_plus_B( m_col_count-k-1, x, &this_m[k][k+1], &this_m[i][k+1], &this_m[i][k+1] );
          I.RowOp( i, x, k );
        }
      }
    }
  }

  // take care of column swaps
  for ( i = k-1; i >= 0; i-- ) {
    if ( i != col[i] )
      I.SwapRows(i,col[i]);
  }

  *this = I;

  return (k == n) ? true : false;
}

bool ON_Matrix::Multiply( const ON_Matrix& a, const ON_Matrix& b )
{
  int i, j, k, mult_count;
  double x;
  if (a.ColCount() != b.RowCount() )
    return false;
  if ( a.RowCount() < 1 || a.ColCount() < 1 || b.ColCount() < 1 )
    return false;
  if ( this == &a ) {
    ON_Matrix tmp(a);
    return Multiply(tmp,b);
  }
  if ( this == &b ) {
    ON_Matrix tmp(b);
    return Multiply(a,tmp);
  }
  Create( a.RowCount(), b.ColCount() );
  mult_count = a.ColCount();
  double const*const* am = a.ThisM();
  double const*const* bm = b.ThisM();
  double** this_m = ThisM();
  for ( i = 0; i < m_row_count; i++ ) for ( j = 0; j < m_col_count; j++ ) {
    x = 0.0;
    for (k = 0; k < mult_count; k++ ) {
      x += am[i][k] * bm[k][j];
    }
    this_m[i][j] = x;
  }
  return true;  
}

bool ON_Matrix::Add( const ON_Matrix& a, const ON_Matrix& b )
{
  int i, j;
  if (a.ColCount() != b.ColCount() )
    return false;
  if (a.RowCount() != b.RowCount() )
    return false;
  if ( a.RowCount() < 1 || a.ColCount() < 1 )
    return false;
  if ( this != &a && this != &b ) {
    Create( a.RowCount(), b.ColCount() );
  }
  double const*const* am = a.ThisM();
  double const*const* bm = b.ThisM();
  double** this_m = ThisM();
  for ( i = 0; i < m_row_count; i++ ) for ( j = 0; j < m_col_count; j++ ) {
    this_m[i][j] = am[i][j] + bm[i][j];
  }
  return true;  
}

bool ON_Matrix::Scale( double s )
{
  bool rc = false;
  if ( m_row_count > 0 && m_col_count > 0 ) 
  {
    struct DBLBLK* cmem = (struct DBLBLK*)m_cmem;
    int i;
    double* p;
    while ( 0 != cmem )
    {
      if ( 0 != cmem->a && cmem->count > 0 )
      {
        p = cmem->a;
        i = cmem->count;
        while(i--)
          *p++ *= s;
      }
      cmem = cmem->next;
    }
    rc = true;
  }
  /*
  int i = m_a.Capacity();
  if ( m_row_count > 0 && m_col_count > 0 && m_row_count*m_col_count <= i ) {
    double* p = m_a.Array();
    while ( i-- )
      *p++ *= s;
    rc = true;
  }
  */
  return rc;
}


int ON_RowReduce( int row_count, 
                  int col_count,
                  double zero_pivot,
                  double** A, 
                  double** B, 
                  double pivots[2] 
                 )
{
  // returned A is identity, B = inverse of input A
  const int M = row_count;
  const int N = col_count;
  const size_t sizeof_row = N*sizeof(A[0][0]);
  int i, j, ii;
  double a, p, p0, p1;
  const double* ptr0;
  double* ptr1;

  if ( pivots )
  {
    pivots[0] = 0.0;
    pivots[1] = 0.0;
  }

  if ( zero_pivot <= 0.0 || !ON_IsValid(zero_pivot) )
    zero_pivot = 0.0;


  for ( i = 0; i < M; i++ )
  {
    memset(B[i],0,sizeof_row);
    if ( i < N )
      B[i][i] = 1.0;
  }

  p0 = p1 = A[0][0];

  for ( i = 0; i < M; i++ )
  {
    p = fabs(a = A[i][i]);
    if ( p < p0 ) p0 = p; else if (p > p1) p1 = p;
    if ( 1.0 != a )
    {
      if ( p <= zero_pivot || !ON_IsValid(a) )
      {
        break;
      }
      a = 1.0/a;

      //A[i][i] = 1.0; // no need to do this

      // The "ptr" voodoo is faster but does the same thing as
      //
      //for ( j = i+1; j < N; j++ )
      //  A[i][j] *= a;
      //      
      j = i+1;
      ptr1 = A[i] + j;
      j = N - j;
      while(j--) *ptr1++ *= a;
      
      // The "ptr" voodoo is faster but does the same thing as
      //
      //for ( j = 0; j <= i; j++ )
      //  B[i][j] *= a;
      //            
      ptr1 = B[i];
      j = i+1;
      while(j--) *ptr1++ *= a;
    }

    for ( ii = i+1; ii < M; ii++ )
    {
      a = A[ii][i];
      if ( 0.0 == a )
        continue;
      a = -a;
      
      //A[ii][i] = 0.0;  // no need to do this

      // The "ptr" voodoo is faster but does the same thing as
      //
      //for( j = i+1; j < N; j++ )
      //  A[ii][j] += a*A[i][j];
      //
      j = i+1;
      ptr0 = A[i] + j;
      ptr1 = A[ii] + j;
      j = N - j;
      while(j--) *ptr1++ += a* *ptr0++;

      for( j = 0; j <= i; j++ )
        B[ii][j] += a*B[i][j];
    }
  }

  if ( pivots )
  {
    pivots[0] = p0;
    pivots[1] = p1;
  }

  if ( i < M )
  {
    return i;
  }


  // A is now upper triangular with all 1s on diagonal 
  //   (That is, if the lines that say "no need to do this" are used.)
  // B is lower triangular with a nonzero diagonal
  for ( i = M-1; i >= 0; i-- )
  {
    for ( ii = i-1; ii >= 0; ii-- )
    {
      a = A[ii][i];
      if ( 0.0 == a )
        continue;
      a = -a;
      //A[ii][i] = 0.0; // no need to do this

      // The "ptr" voodoo is faster but does the same thing as
      //
      //for( j = 0; j < N; j++ )
      //  B[ii][j] += a*B[i][j];
      //
      ptr0 = B[i];
      ptr1 = B[ii];
      j = N;
      while(j--) *ptr1++ += a* *ptr0++;
    }
  }

  // At this point, A is trash.
  // If the input A was really nice (positive definite....)
  // the B = inverse of the input A.  If A was not nice,
  // B is also trash.
  return M;
}


unsigned int ON_RowReduce(
  unsigned int row_count,
  unsigned int col_count,
  double zero_pivot_tolerance,
  const double*const* constA,
  bool bInitializeB,
  bool bInitializeColumnPermutation,
  double** A,
  double** B,
  unsigned int* column_permutation,
  double pivots[3]
  )
{
  double pivots_buffer[3];
  if (nullptr == pivots)
    pivots = pivots_buffer;
  pivots[0] = pivots[1] = -1.0;
  pivots[2] = 0.0;

  if (row_count < 1 || ON_UNSET_UINT_INDEX == row_count)
    return ON_UNSET_UINT_INDEX;

  if (col_count < 1 || ON_UNSET_UINT_INDEX == col_count)
    return ON_UNSET_UINT_INDEX;

  if (nullptr == B)
    return ON_UNSET_UINT_INDEX;

  unsigned int i, j;
  double* a;
  double* b;

  if (bInitializeB)
  {
    for (i = 0; i < row_count; i++)
    {
      b = B[i];
      for (j = 0; j < row_count; j++)
        b[j] = 0.0;
      b[i] = 1.0;
    }
  }

  if (bInitializeColumnPermutation && nullptr != column_permutation)
  {
    for (j = 0; j < row_count; j++)
      column_permutation[j] = j;
  }

  if (!(zero_pivot_tolerance >= 0.0))
    zero_pivot_tolerance = 0.0;

  std::unique_ptr< ON_Matrix > uA;
  if (nullptr == A)
  {
    if (nullptr == constA)
      return ON_UNSET_UINT_INDEX;
    uA = std::unique_ptr< ON_Matrix >(new ON_Matrix(row_count, col_count));
    A = uA.get()->m;
  }

  if (nullptr != constA && nullptr != A)
  {
    for (i = 0; i < row_count; i++)
    {
      const double* c = constA[i];
      a = A[i];
      for (j = 0; j < row_count; j++)
        a[j] = c[j];
    }
  }

  double* r;
  double* s;
  double x, xx;
  unsigned int ii, jj;
  ii = jj = 0;
  xx = fabs(A[ii][jj]);
  unsigned int rank;
  for (rank = 0; rank < row_count; rank++)
  {
    if (rank >= col_count)
      return rank;

    xx = -1.0;
    ii = jj = rank;
    for (i = rank; i < row_count; i++)
    {
      r = A[i];
      for (j = rank; j < col_count; j++)
      {
        x = fabs(r[j]);
        if (x > xx)
        {
          ii = i;
          jj = j;
          xx = x;
        }
      }
    }

    if (!(xx >= 0.0))
      return ON_UNSET_UINT_INDEX;

    if (pivots[0] < 0.0)
      pivots[0] = pivots[1] = xx;

    if (xx <= zero_pivot_tolerance)
    {
      pivots[2] = xx;
      return rank;
    }

    if (xx > pivots[0])
      pivots[0] = xx;
    else if (xx < pivots[1])
      pivots[1] = xx;


    a = A[ii];
    b = B[ii];
    if (ii > rank)
    {
      // swap rows M[ii] and M[rank] so maximum coefficient is in M[rank][jj]
      A[ii] = A[rank];
      A[rank] = a;
      B[ii] = B[rank];
      B[rank] = b;
      if (nullptr != column_permutation)
      {
        j = column_permutation[ii];
        column_permutation[ii] = column_permutation[rank];
        column_permutation[rank] = j;
      }
    }

    // swap columns M[jj] and M[rank] to maximum coefficient is in M[rank][rank]
    xx = -a[jj];
    a[jj] = a[rank];
    //a[rank] = -xx; // DEBUG

    for (i = rank + 1; i < row_count; i++)
    {
      // swap columns M[jj] and M[rank] 
      r = A[i];
      x = r[jj] / xx;
      //double dd = r[jj]; // DEBUG
      r[jj] = r[rank];
      //r[rank] = dd; // DEBUG

      // Use row operation
      // M[i] = M[i] - (row[i]/M[rank][rank])*M[rank]
      // to M[i][rank]
      if (0.0 != x)
      {
        s = B[i];
        for (j = 0; j <= rank; j++)
        {
          s[j] += x*b[j];
          //r[j] += x*a[j]; // DEBUG
        }
        for (/*empty init*/; j < col_count; j++)
        {
          s[j] += x*b[j];
          r[j] += x*a[j];
        }
      }
    }
  }
  return rank;
}


double ON_EigenvectorPrecision(
  const unsigned int N,
  const double*const* M,
  bool bTransposeM,
  double lambda,
  const double* eigenvector
  )
{
  double delta = 0.0;
  double len = 0.0;
  if ( bTransposeM )
  {
    for (unsigned int i = 0; i < N; i++)
    {
      len += (eigenvector[i] * eigenvector[i]);
      const double* e = eigenvector;
      double Mei = 0.0;
      for (unsigned int j = 0; j < N; j++)
        Mei += M[j][i]*(*e++);
      double d = fabs(Mei - lambda*eigenvector[i]);
      if (d > delta)
        delta = d;
    }
  }
  else
  {
    const double* e0 = eigenvector + N;
    for (unsigned int i = 0; i < N; i++)
    {
      len += (eigenvector[i] * eigenvector[i]);
      const double* Mi = M[i];
      const double* e = eigenvector;
      double Mei = 0.0;
      while (e < e0)
        Mei += (*Mi++)*(*e++);
      double d = fabs(Mei - lambda*eigenvector[i]);
      if (d > delta)
        delta = d;
    }
  }
  if (delta > 0.0 && len > 0.0)
    delta /= sqrt(len);

  return delta;
}


double ON_MatrixSolutionPrecision(
  const unsigned int N,
  const double*const* M,
  bool bTransposeM,
  double lambda,
  const double* X,
  const double* B
  )
{
  double delta = 0.0;
  if (bTransposeM)
  {
    for (unsigned int i = 0; i < N; i++)
    {
      const double* e = X;
      double Mei = -(lambda*e[i]);
      for ( unsigned int j = 0; j < N; j++ )
        Mei += M[j][i]*(*e++);
      double d = fabs(Mei - B[i]);
      if (d > delta)
        delta = d;
    }
  }
  else
  {
    const double* e0 = X + N;
    for (unsigned int i = 0; i < N; i++)
    {
      const double* Mi = M[i];
      const double* e = X;
      double Mei = -(lambda*e[i]);
      while (e < e0)
        Mei += (*Mi++)*(*e++);
      double d = fabs(Mei - B[i]);
      if (d > delta)
        delta = d;
    }
  }

  return delta;
}

static int CompareDoubleIncreasing(const void* a, const void* b)
{
  double x = *((const double*)a);
  double y = *((const double*)b);
  if (x < y)
    return -1;
  if (x > y)
    return 1;
  return 0;
}

unsigned int ON_GetEigenvectors(
  const unsigned int N,
  const double*const* M,
  bool bTransposeM,
  double lambda,
  unsigned int lambda_multiplicity,
  const double* termination_tolerances,
  double** eigenvectors,
  double* eigenprecision,
  double* eigenpivots
  )
{
  if (N < 1 || ON_UNSET_UINT_INDEX == N)
    return ON_UNSET_UINT_INDEX;

  if (1 == N)
  {
    eigenvectors[0][0] = 1.0;
    if (nullptr != eigenpivots)
    {
      eigenpivots[0] = M[0][0];
      eigenpivots[1] = M[0][0];
      eigenpivots[2] = 0.0;
    }
    if (nullptr != eigenprecision)
    {
      eigenprecision[3] = fabs(lambda - M[0][0]);
    }
    return (1 == lambda_multiplicity) ? 1 : 0;
  }

  double tols[3] = { 1.0e-12, 1.0e-3, 1.0e4 };
  if (nullptr != termination_tolerances)
  {
    if (termination_tolerances[0] > 0.0)
      tols[0] = termination_tolerances[0];
    if (termination_tolerances[1] > 0.0)
      tols[1] = termination_tolerances[0];
    if (termination_tolerances[2] > 0.0)
      tols[2] = termination_tolerances[2];
  }


  ON_Matrix Abuffer(N, N);
  double** A = Abuffer.m;

  ON_Matrix Bbuffer(N, N);
  double** B = Bbuffer.m;

  unsigned int i, j;
  double pivots[3] = { 0.0, 0.0, 0.0 };
  double zero_pivot_tolerance0 = 0.0;
  double zero_pivot_tolerance = 0.0;
  const bool bUnknownLambdaMultiplicity = (lambda_multiplicity < 1 || lambda_multiplicity > N);
  if (bUnknownLambdaMultiplicity)
    lambda_multiplicity = 1;
  unsigned int rank = N + 1;
  bool bLastTry = false;
  for(unsigned int rank0 = N+1; rank0 > 0; /*empty increment*/)
  {
    if (bTransposeM)
    {
      // A = (M - lambda*I);
      for (i = 0; i < N; i++)
      {
        double* r = A[i];
        for (j = 0; j < N; j++)
        {
          r[j] = M[i][j];
        }
        r[i] -= lambda;
      }
    }
    else
    {
      // A = Transpose(M - lambda*I);
      for (i = 0; i < N; i++)
      {
        double* r = A[i];
        for (j = 0; j < N; j++)
        {
          r[j] = M[j][i];
        }
        r[i] -= lambda;
      }
    }

    zero_pivot_tolerance = pivots[1];
    if (!(zero_pivot_tolerance >= 0.0))
    {
      // This should never happen.  The test is here because
      // it has no performance penalty and will detect bugs
      // if this code is incorrectly modified.
      ON_ERROR("invalid zero_pivot_tolerance value");
      break;
    }

    pivots[0] = 0.0; // maximum pivot
    pivots[1] = 0.0; // minimum "nonzero" pivot
    pivots[2] = 0.0; // maximum "zero" pivot
    rank = ON_RowReduce(N, N, zero_pivot_tolerance, nullptr, true, false, A, B, nullptr, pivots);

    if (bLastTry)
    {
      // For an explanation, see the code below where bLastTry is set to true.
      break;
    }

    if (rank >= rank0 || rank > N)
    {
      // failure
      break;
    }

    if (rank == N - lambda_multiplicity)
    {
      // We found exactly lambda_multiplicity eigenvectors.
      // If the value of zero_pivot_tolerance was appropriate
      // and double precision arithmetic is appropriate for
      // the matrix, then we are returning reliable 
      // eigenvectors.
      break;
    }

    if (rank < N - lambda_multiplicity)
    {
      // Theoretically, the dimension of the eigenspace cannot be larger than
      // the multiplicity of the eigenvalue.
      //
      // Either we have an unstable case (perhaps there is another
      // eigenvalue that is close to lambda), or the value of
      // zero_pivot_tolerance was too big, or double precision 
      // arithmetic is not good enough for the matrix.
      if (rank0 > 0 && rank0 < N && zero_pivot_tolerance > zero_pivot_tolerance0 && zero_pivot_tolerance0 >= 0.0)
      {
        // We will use the previous result.
        pivots[1] = zero_pivot_tolerance0;
        bLastTry = true;
        continue;
      }

      if (bUnknownLambdaMultiplicity)
      {
        lambda_multiplicity = N - rank;
      }
      // Otherwise, we will pick the best eigenvectors below.
      break;
    }

    if (!(pivots[1] > 0.0 && pivots[0] >= pivots[1] && pivots[1] > pivots[2] && pivots[2] <= zero_pivot_tolerance))
    {
      // Basic sanity check failed.
      // - should have a positive minimum "nonzero" pivot
      // - maximum "nonzero" should be >= minimum "nonzero"
      // - maximum "zero" should be < minimum "nonzero"
      // - maximum "zero" should be <= zero_pivot_tolerance
      break;
    }

    if (!(pivots[1] > zero_pivot_tolerance))
    {
      // If we don't increase zero_pivot_tolerance,
      // we will note get a smaller rank from ON_RowReduce().
      break;
    }

    double r1 = pivots[1] / pivots[0];
    // At this point,
    //
    // pivot[0] = maximum pivot
    // pivot[1] = minimum "nonzero" pivot > zero_pivot_tolerance
    // pivot[2] = maximum "zero" pivot <= zero_pivot_tolerance
    //
    // from the call to ON_RowReduce and we have found
    // (N - rank) eigenvectors.  Assuming the input is valid,
    // that is lambda really is an eigenvalue with multiplicity
    // lambda_multiplicity, then there are more eigenvectors or
    // we are in the case where the dimension of the eigenspace
    // is less than the multiplicity of the eigenvalue 
    // (there are 1s on the super diagonal in Jordan decomposition).
    //
    // In order for an additional pass to be useful, the value
    // of zero_pivot_tolerance needs to be increased but it
    // must still be reasonable numerically.
    //
    // The value of pivot[1] is the smallest value for the
    // next candidate for zero_pivot_tolerance.
    //
    // If pivots[1] / pivots[0] <= tols[0](1.0e-12), pivots[1] is "tiny"
    // with respect to pivot[0], from the point of view of a
    // double and addition.  If the entries in the matrix
    // are numerically reasonable for double precision
    // calculations, then pivot[1] is a reasonable choice for
    // a zero tolerance.
    if (!(r1 <= tols[0]))
    {
      double d1 = pivots[0] - pivots[1];

      // If r1 < tols[0](1.0e-3), there are at least 3 orders of
      // magnitude between the maximum pivot and pivot[1].
      // If d1 > tols[2](1.0e4) *pivots[1], pivot[1] is substantially closer to
      // zero than it is to pivot[0].
      if (!(r1 <= tols[1] && d1 > tols[2]*pivots[1]))
      {
        break;
      }
    }

    rank0 = rank;
  }

  if (nullptr != eigenpivots)
  {
    eigenpivots[0] = pivots[0];
    eigenpivots[1] = pivots[1];
    eigenpivots[2] = pivots[2];
  }


  if (rank >= N)
    return 0;

  if (nullptr == B)
    return 0;

  // Return the best eigenvector candidates first.
  ON_SimpleArray< double > Bprecision(N - rank);
  for (unsigned int k = rank; k < N; k++)
    Bprecision.Append(ON_EigenvectorPrecision(N, M, bTransposeM, lambda, B[k]));
  ON_SimpleArray< unsigned int > _Bdex(Bprecision.UnsignedCount());
  unsigned int* Bdex = _Bdex.Array();
  ON_Sort(ON::sort_algorithm::quick_sort, Bdex, Bprecision.Array(), Bprecision.UnsignedCount(), sizeof(double), CompareDoubleIncreasing);

  const unsigned int Bi0 = rank;
  if (rank < N - lambda_multiplicity)
  {
    // we had too many candidates - return the best ones.
    rank = N - lambda_multiplicity;
  }

  // B is a nonsingular row operation matrix and the last (N-rank) rows of B*A are zero.
  // Therefore, the last (N-rank) columns of Transpose(A)*Transpose(B) are zero.
  // Therefore, the last (N-rank) columns of Transpose(B) are a basis for the kernel of Transpose(A).
  // Therefore, the last (N-rank) rows of B are a basis for the kernel of Transpose(A).
  // Therefore, the last (N-rank) rows of B are a basis for the kernel of of (M - lambda*I).
  // Therefore, the last (N-rank) rows of B are a basis for the lambda eigenspace of M.
  for (unsigned int k = rank; k < N; k++)
  {
    const unsigned int ei = k - rank;
    const unsigned int Bi = Bdex[ei];
    double* V = eigenvectors[ei];
    for (j = 0; j < N; j++)
      V[j] = B[Bi0 + Bi][j];

    if (nullptr != eigenprecision)
      eigenprecision[ei] = Bprecision[Bi];
  }

  return (rank < N) ? (N - rank) : 0U;
}



