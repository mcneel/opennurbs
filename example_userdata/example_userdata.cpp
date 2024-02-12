#include "../opennurbs_public_examples.h"

// This example demonstrates how to attach customized "user data"
// to any class derived from ON_Object.  In particular, you can
// attach custom information to any piece of geometry in a 3DM
// file and have it persist in files, transform, and copy.

class MyUserData : public ON_UserData
{
  ON_OBJECT_DECLARE(MyUserData);

public:
  // Note well:
  // 1) All constructors must initialize ON_UserData::m_userdata_uuid to
  //    the UUID that identifies this kind of user data.  
  // 2) All constructors must initialize ON_UserData::m_copy_count to
  //    1 if the user data should be copied when the parent object is
  //    copied.
  // For more details, see comments in the constructor code below.
  MyUserData();
  virtual ~MyUserData();

  MyUserData(const MyUserData&);
  MyUserData& operator=(const MyUserData&);

  // In order to get your user data to persist in files, you must
  // override ON_UserData::Archive(), ON_Object::Write() and 
  // ON_Object::Read()

  bool Write(
    ON_BinaryArchive&
  ) const override;

  bool Read(
    ON_BinaryArchive&
  ) override;

  // Archive() must return true in order for user data to get saved
  // in a file.
  bool Archive() const override;
  
  // You must override ON_UserData::GetDescription().
  bool GetDescription( ON_wString& ) override;

  // If your user data is attached to some type of ON_Geometry and you
  // want the user data to be transformed when the parent ON_Geometry
  // is transformed, then you must override ON_UserData::Transform().
  //
  // If you don't override ON_UserData::Transform(), then the net
  // result of any transforms is stored in ON_UserData::m_userdata_xform.
  // At appropriate times, you can inspect ON_UserData::m_userdata_xform
  // and reset it to the identity after you've taken whatever actions
  // you deem to be appropriate.
  bool Transform( const ON_Xform& ) override;

  // possible information you might want to attach.
  int       m_my_int;
  ON_Line   m_my_line;
  ON_String m_my_string;
};

ON_OBJECT_IMPLEMENT( MyUserData, ON_UserData, "53114529-1CD7-4872-818E-311CB19101FA" );

// {D11E26D2-9A77-4a2f-AEC8-4498F2EABBA1}
static const ON_UUID my_application_id = 
{ 0xd11e26d2, 0x9a77, 0x4a2f, { 0xae, 0xc8, 0x44, 0x98, 0xf2, 0xea, 0xbb, 0xa1 } };

MyUserData::MyUserData()
{
  // Each kind of user data needs a uuid and your constructors MUST
  // initialize ON_UserData::m_userdata_uuid with the value of this
  // uuid.  The value of ON_UserData::m_userdata_uuid uuid is used to
  // identify the user data and is passed to ON_Object::GetUserData()
  // when an application wants to access user data that has been
  // attached to an object.
  //
  // In simple cases, the class UUID can be used as is shown below.
  m_userdata_uuid = MyUserData::m_MyUserData_class_rtti.Uuid();

  // In order for user data to be saved in 3dm files, it must have
  // a non-nil application id.
  m_application_uuid = my_application_id;

  // If you want your user data to be copied when its parent ON_Object
  // is copied, then your constructor must initialize 
  // ON_UserData::m_userdata_copycount to 1.
  // By default, ON_UserData::m_userdata_copycount is zero and the
  // user data is not copied.  See the comments in the ON_UserData
  // class definition for more details.
  m_userdata_copycount = 1;

  m_my_int = 0;
}

MyUserData::MyUserData(const MyUserData& src) 
          : ON_UserData(src) // critical - be sure to call base class copy constructor
{
  // The base class copy constructor copies
  // m_userdata_uuid, m_application_id, m_userdata_copycount, 
  // and m_userdata_xform.  Then if m_userdata_copycount is
  // not zero, it is incremented.

  m_my_int = src.m_my_int;
  m_my_line = src.m_my_line;
  m_my_string = src.m_my_string;
}

MyUserData& MyUserData::operator=(const MyUserData& src)
{
  if ( this != &src ) {
    // critical - be sure to call base class operator=()
    ON_UserData::operator=(src);
    
    m_my_int = src.m_my_int;
    m_my_line = src.m_my_line;
    m_my_string = src.m_my_string;
  }
  return *this;
}

MyUserData::~MyUserData()
{
}

bool MyUserData::Archive() const
{
  return true;
}


bool MyUserData::Read( ON_BinaryArchive& file )
{
  bool rc = true;
  if ( rc ) 
    rc = file.ReadInt(&m_my_int);
  if ( rc ) 
    rc = file.ReadLine(m_my_line);
  if ( rc ) 
    rc = file.ReadString(m_my_string);
  return rc;
}

bool MyUserData::Write( ON_BinaryArchive& file ) const
{
  bool rc = true;
  if ( rc ) 
    rc = file.WriteInt(m_my_int);
  if ( rc ) 
    rc = file.WriteLine(m_my_line);
  if ( rc ) 
    rc = file.WriteString(m_my_string);
  return rc;
}

bool MyUserData::GetDescription( ON_wString& s )
{
  s = L"my user data with point, line, and string";
  return true;
}

bool MyUserData::Transform( const ON_Xform& xform )
{
  // Optional: call the ON_UserData::Transform() if you want the
  // ON_UserData::m_userdata_xform value to be updated.
  ON_UserData::Transform(xform);


  // Transform any geometry you have in your class.
  bool rc = m_my_line.Transform(xform);
  return rc;
}

static ON_ModelGeometryComponent read_file( 
  const wchar_t* filename,
  bool bManageGeometryObject
)
{
  // see example_read.cpp for information about read 3dm files
  // This code will only read the file created by write_file().
  // This code should not be used as a model for reading general 3dm files.
  
  ONX_Model model;
  ON_BinaryFile archive(ON::archive_mode::read3dm, filename);
  if (!model.IncrementalReadBegin(archive, true, 0, nullptr))
    return ON_ModelGeometryComponent::Unset;

  ON_ModelComponentReference mcr;
  if (!model.IncrementalReadModelGeometry(archive, true, bManageGeometryObject, true, 0, mcr))
    return ON_ModelGeometryComponent::Unset;

  const ON_ModelGeometryComponent* mgc = ON_ModelGeometryComponent::Cast(mcr.ModelComponent());
  if (nullptr == mgc)
    return ON_ModelGeometryComponent::Unset;

  return *mgc;
}

int main()
{
  ON::Begin();

  // uuid used to get user data via ON_Object::GetUserData()
  const ON_UUID my_user_data_uuid = MyUserData::m_MyUserData_class_rtti.Uuid();

  // We'll attach a MyUserData user data to a point.  In general,
  // you can attach user data to any class derived from ON_Object.
  ON_Point point(0.0,0.0,0.0);

  // User data must be created by a call to new
  MyUserData* ud = new MyUserData();
  ud->m_my_int = 1;
  ud->m_my_line.from.Set(0.0,0.0,0.0);
  ud->m_my_line.to.Set(1.0,1.0,1.0);
  ud->m_my_string = "my user data";

  // This attaches the user data to point.  When the point is destroied,
  // the user data will be destroyed.  
  //
  point.AttachUserData(ud);

  // Use ON_Object::GetUserData() to get user data.
  MyUserData* original_ud = MyUserData::Cast( point.GetUserData( my_user_data_uuid ) );

  printf("original_ud->m_userdata_copycount = %d\n",original_ud->m_userdata_copycount);

  // When the point is copied, the user data will be copied if 
  // ud->m_userdata_copycount > 0.
  //
  ON_Point copy_of_point = point;

  // Use ON_Object::GetUserData() to get user data.
  MyUserData* copy_of_ud = MyUserData::Cast( copy_of_point.GetUserData( my_user_data_uuid ) );

  if ( 0 == copy_of_ud )
    printf("ON_UserData::m_copy_count must be > 0 for user data to be copied.\n");
  else
    printf("copy_of_ud->m_userdata_copycount = %d\n",copy_of_ud->m_userdata_copycount);

  // When the point is transformed, the virtual ON_UserData::Transform()
  // is called to transform the point.


  // When the point is saved to a file, the virtual ON_Object::Write() is
  // called to write the attached user data.
  const wchar_t* filename = L"my_point_with_user_data.3dm";
  ON_WriteOneObjectArchive(filename, point);

  // When the point is read from a file, the virtual ON_Object::Read() is
  // called to read the user data.
  // If bManageGeometryObject is true, then the ON_ModelGeometryComponent destructor 
  // will delete point_from_file.
  bool bManageGeometryObject = true;
  ON_ModelGeometryComponent mgr = read_file( filename, bManageGeometryObject );
  const ON_Geometry* point_from_file = mgr.Geometry(nullptr);

  if ( nullptr != point_from_file )
  {
    // Use ON_Object::GetUserData() to get user data.
    MyUserData* ud_from_file = MyUserData::Cast( point_from_file->GetUserData( my_user_data_uuid ) );

    printf("ud_from_file->m_userdata_copycount = %d\n",ud_from_file->m_userdata_copycount);

    if (false == bManageGeometryObject)
    {
      // caller must manage point_from_file;
      delete const_cast<ON_Geometry*>(point_from_file);
    }
  }

  ON::End();

  return 0;
}
