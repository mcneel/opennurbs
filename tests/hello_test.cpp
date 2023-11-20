#include <gtest/gtest.h>
#include "../opennurbs_public.h"

# define M_PI           3.14159265358979323846  /* pi */

TEST(HelloTest, BasicAssertions) {

  ON_3dPoint center(0,0,0);

  ON_Circle circle(center, 10.00);

  double circumference = 2 * M_PI * circle.radius;

  EXPECT_EQ(circumference, circle.Circumference());
  
}