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

#if !defined(ON_SUN_INC_)
#define ON_SUN_INC_

class ON_CLASS ON_Sun final
{
public:
  ON_Sun();
  ON_Sun(ON_XMLNode& model_node);
  ON_Sun(const ON_Sun& sun);
  ~ON_Sun();

  // Returns the minimum allowed year for sun methods.
  static int MinYear(void);

  // Returns the maximum allowed year for sun methods.
  static int MaxYear(void);

  const ON_Sun& operator = (const ON_Sun& sun);

  bool operator == (const ON_Sun& sun);
  bool operator != (const ON_Sun& sun);

  // Returns true if enabling/disabling the sun is allowed, else false.
  bool EnableAllowed(void) const;

  // Returns true if the sun is enabled, else false.
  bool EnableOn(void) const;

  // Returns true if manual control of the sun position is allowed, else false.
  bool ManualControlAllowed(void) const;

  // Returns true if manual control of the sun position is in effect, else false.
  bool ManualControlOn(void) const;

  // Returns The world angle corresponding to North in degrees.
  // This angle is zero along the x-axis and increases anticlockwise.
  double North(void) const;

  // Returns The sun's direction vector in world space, taking into account the sun's azimuth &
  // altitude and the direction of north. See Azimuth(), Altitude(), North().
  // Note that this does not actually calculate the azimuth or altitude from the place and time;
  // it merely returns the values that were stored in the model.
  ON_3dVector Vector(void) const;

  // Returns the azimuth of the sun in degrees. The value increases Eastwards with North as zero.
  // Note: This value is not affected by the direction of north. See North()
  double Azimuth(void) const;

  // Returns the altitude of the sun in degrees.
  double Altitude(void) const;

  // Returns the latitude of the observer.
  double Latitude(void) const;

  // Returns the longitude of the observer.
  double Longitude(void) const;

  // Returns the time zone of the observer in decimal hours.
  double TimeZone(void) const;

  // Returns true if daylight saving is on, else false.
  bool DaylightSavingOn(void) const;

  // Returns the daylight saving offset of the observer in minutes.
  int DaylightSavingMinutes(void) const;

  // Retrieves the local date and time of the observer.
  // year accepts the year in the range MinYear() to MaxYear();
  // month accepts the month in the range 1 to 12.
  // day accepts the day in the range 1 to 31.
  // hours accepts the time expressed as decimal hours in the range 0 to 24.
  // Returns The local date and time of the observer.
  void LocalDateTime(int& year, int& month, int& day, double& hours) const;

  // Returns the intensity to be used for the sun. This is 1.0 by default.
  double Intensity(void) const;

  // Returns the shadow intensity to be used for the sun. This is 1.0 by default. 0.0 turns off all shadows.
  double ShadowIntensity(void) const;

  // Set whether or not enabling/disabling the sun is allowed.
  void SetEnableAllowed(bool allowed);

  // Set whether or not the sun is enabled.
  void SetEnableOn(bool on);

  // Set whether or not manual control of the sun position is allowed.
  void SetManualControlAllowed(bool allowed);

  // Set whether or not manual control of the sun position is in effect.
  void SetManualControlOn(bool manual);

  // Set the azimuth corresponding to North.
  // north is the world angle corresponding to North in degrees in the range 0 to 360.
  // This angle is zero along the x-axis and increases anticlockwise.
  void SetNorth(double north);

  // Set the sun's direction vector when manual control is in effect. This calculates and sets
  // the sun's azimuth & altitude and takes into account the direction of north.
  // See SetAzimuth(), SetAltitude(), North().
  void SetVector(const ON_3dVector& v);

  // Set the azimuth of the sun when manual control is in effect.
  // - azimuth is the sun's azimuth in degrees. The value increases Eastwards with North as zero.
  // Note: This value is not affected by the direction of north.
  void SetAzimuth(double azimuth);

  // Set the sun's altitude when manual control is in effect.
  // - altitude is the sun's altitude in degrees in the range -90 to +90.
  void SetAltitude(double altitude);

  // Set the latitude of the observer.
  // - latitude is the observer's latitude in degrees in the range -90 to +90.
  void SetLatitude(double latitude);

  // Set the longitude of the observer.
  // - longitude is the observer's longitude in degrees in the range -180 to +180.
  void SetLongitude(double longitude);

  // Set the time zone of the observer in hours, in the range -12 to +13.
  void SetTimeZone(double hours);

  // Set whether or not the observer is using daylight saving time.
  void SetDaylightSavingOn(bool on);

  // Set the daylight saving of the observer in minutes, in the range 0 to 120.
  void SetDaylightSavingMinutes(int minutes);

  // Set the local date and time of the observer.
  // - year is the year which must lie between MinYear() to MaxYear() inclusive.
  // - month is the month in the range 1 to 12.
  // - day is the day in the range 1 to 31.
  // - hours is the time expressed as decimal hours in the range 0 to 24.
  // Returns true if successful, false if the date is out of range.
  bool SetLocalDateTime(int year, int month, int day, double hours);

  // Set the shadow intensity to be used for the sun. This is 1.0 by default. 0.0 turns off all shadows.
  void SetShadowIntensity(double intensity);

  // Set the intensity to be used for the sun.  This is 1.0 by default.
  void SetIntensity(double intensity);

  // Returns the CRC of the sun state.
  ON__UINT32 DataCRC(ON__UINT32 current_remainder) const;

  // Returns true if all the sun parameters are valid, or false if any value is invalid.
  bool IsValid(void) const;

  // Get an ON_Light which represents the sun. Note that this does not actually calculate the sun's
  // azimuth or altitude from the place and time. It merely uses the values that were stored in the model.
  ON_Light Light(void) const;

  // Get a color for rendering a sun light when the sun is at a particular altitude in the sky.
  static ON_4fColor SunColorFromAltitude(double altitude);

public:
  class CImpl;
  CImpl* m_impl;
};

#endif
