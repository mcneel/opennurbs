/* $Header: /src4/opennurbs/Tests/TestBooleanUnion.cpp 1     2/11/05 6:32a Dalelear $ */
/* $NoKeywords: $ */

#include "Tests.h"

void TestBooleanUnion( 
                      const ON_Brep* brepA, const wchar_t* nameA
                      const ON_Brep* brepB, const wchar_t* nameB 
                      )
{
  // todo test this union
}

void TestBooleanUnion( ON_TextLog& text_log )
{
  TEST_HEADER(text_log,"TestBooleanUnion");

  ONX_Model model;
  int i, pass;

  double sample_start_distance = 0.0;
  double sample_stop_distance = 0.0;

  const char* filename = "\\src4\\opennurbs\\Tests\\Input\\TestBooleanUnion.3dm";
  //const char* filename = "\\src4\\opennurbs\\Tests\\Input\\SmallTest.3dm";

  ON_SimpleArray<const ON_Brep*> brep;
  ON_ClassArray<ON_wString> brep_name;
  
  if ( model.Read( filename,&text_log) )
  {
    brep.Reserve(model.m_object_table.Count());
    brep_name.Reserve(model.m_object_table.Count());
    for ( i = 0; i < model.m_object_table.Count(); i++ )
    {
      const ON_Brep* b = ON_Brep::Cast(model.m_object_table[i].m_object);
      if ( b )
      {
        const wchar_t* name = model.m_object_table[i].m_attributes.m_name;
        if ( 0 == name || 0 == *name)
        {
          name = L"anonymous";
        }
        brep_name.AppendNew() = name;
        brep.Append(brep);
      }
    }
  }

  const wchar_t *nameA, *nameB;
  const ON_Brep *brepA, *brepB;
  for ( i = 0; i < brep.Count(); i++ )
  {
    brepA = brep[i];
    nameA = brep_name[i];
    for ( j = i+1; j < brep.Count(); j++ )
    {
      brepB = brep[j];
      nameB = brep_name[j];
      TestBooleanUnion( brepA,nameA,brepB,nameB);
      TestBooleanUnion( brepB,nameB,brepA,nameA);
    }
  }

}


