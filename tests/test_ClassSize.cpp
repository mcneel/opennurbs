#include <gtest/gtest.h>
#include "../opennurbs_public.h"

TEST(ONTest, ONTestClassSizeON_3dmObjectAttributes) {

  size_t sz = sizeof(ON_3dmObjectAttributes); //368

  EXPECT_EQ(sz, 368);

}

TEST(ONTest, ONTestClassSizeON_3dmRevisionHistory) {

  size_t sz = sizeof(ON_3dmRevisionHistory); //96

  EXPECT_EQ(sz, 96);

}

TEST(ONTest, ONTestClassSizeON_3dmNotes) {

  size_t sz = sizeof(ON_3dmNotes); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_3dmApplication) {

  size_t sz = sizeof(ON_3dmApplication); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_3dmProperties) {

  size_t sz = sizeof(ON_3dmProperties); //512

  EXPECT_EQ(sz, 512);

}

TEST(ONTest, ONTestClassSizeON_3dmUnitsAndTolerances) {

  size_t sz = sizeof(ON_3dmUnitsAndTolerances); //56

  EXPECT_EQ(sz, 56);

}

TEST(ONTest, ONTestClassSizeON_3dmAnnotationSettings) {

  size_t sz = sizeof(ON_3dmAnnotationSettings); //104

  EXPECT_EQ(sz, 104);

}

TEST(ONTest, ONTestClassSizeON_3dmConstructionPlaneGridDefaults) {

  size_t sz = sizeof(ON_3dmConstructionPlaneGridDefaults); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_3dmConstructionPlane) {

  size_t sz = sizeof(ON_3dmConstructionPlane); //168

  EXPECT_EQ(sz, 168);

}

TEST(ONTest, ONTestClassSizeON_3dmViewPosition) {

  size_t sz = sizeof(ON_3dmViewPosition); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_3dmViewTraceImage) {

  size_t sz = sizeof(ON_3dmViewTraceImage); //336

  EXPECT_EQ(sz, 336);

}

TEST(ONTest, ONTestClassSizeON_3dmWallpaperImage) {

  size_t sz = sizeof(ON_3dmWallpaperImage); //192

  EXPECT_EQ(sz, 192);

}

TEST(ONTest, ONTestClassSizeON_3dmPageSettings) {

  size_t sz = sizeof(ON_3dmPageSettings); //64

  EXPECT_EQ(sz, 64);

}

TEST(ONTest, ONTestClassSizeON_StandardDisplayModeId) {

  size_t sz = sizeof(ON_StandardDisplayModeId); //1

  EXPECT_EQ(sz, 1);

}

TEST(ONTest, ONTestClassSizeON_3dmView) {

  size_t sz = sizeof(ON_3dmView); //1520

  EXPECT_EQ(sz, 1520);

}

TEST(ONTest, ONTestClassSizeON_3dmRenderSettings) {

  size_t sz = sizeof(ON_3dmRenderSettings); //184

  EXPECT_EQ(sz, 184);

}

TEST(ONTest, ONTestClassSizeON_EarthAnchorPoint) {

  size_t sz = sizeof(ON_EarthAnchorPoint); //152

  EXPECT_EQ(sz, 152);

}

TEST(ONTest, ONTestClassSizeON_3dmIOSettings) {

  size_t sz = sizeof(ON_3dmIOSettings); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_3dmSettings) {

  size_t sz = sizeof(ON_3dmSettings); //1560

  EXPECT_EQ(sz, 1560);

}

TEST(ONTest, ONTestClassSizeON_3dmAnimationProperties) {

  size_t sz = sizeof(ON_3dmAnimationProperties); //304

  EXPECT_EQ(sz, 304);

}

TEST(ONTest, ONTestClassSizeON_Annotation) {

  size_t sz = sizeof(ON_Annotation); //792

  EXPECT_EQ(sz, 792);

}

TEST(ONTest, ONTestClassSizeON_TextDot) {

  size_t sz = sizeof(ON_TextDot); //72

  EXPECT_EQ(sz, 72);

}

TEST(ONTest, ONTestClassSizeON_Arc) {

  size_t sz = sizeof(ON_Arc); //152

  EXPECT_EQ(sz, 152);

}

TEST(ONTest, ONTestClassSizeON_ArcCurve) {

  size_t sz = sizeof(ON_ArcCurve); //192

  EXPECT_EQ(sz, 192);

}

TEST(ONTest, ONTestClassSizeON_Buffer) {

  size_t sz = sizeof(ON_Buffer); //64

  EXPECT_EQ(sz, 64);

}

TEST(ONTest, ONTestClassSizeON_3DM_BIG_CHUNK) {

  size_t sz = sizeof(ON_3DM_BIG_CHUNK); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_UserDataItemFilter) {

  size_t sz = sizeof(ON_UserDataItemFilter); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_ComponentManifest) {

  size_t sz = sizeof(ON_ComponentManifest); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_ComponentManifestItem) {

  size_t sz = sizeof(ON_ComponentManifestItem); //80

  EXPECT_EQ(sz, 80);

}

TEST(ONTest, ONTestClassSizeON_ManifestMapItem) {

  size_t sz = sizeof(ON_ManifestMapItem); //48

  EXPECT_EQ(sz, 48);

}

TEST(ONTest, ONTestClassSizeON_ManifestMap) {

  size_t sz = sizeof(ON_ManifestMap); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_3dmAnnotationContext) {

  size_t sz = sizeof(ON_3dmAnnotationContext); //64

  EXPECT_EQ(sz, 64);

}

TEST(ONTest, ONTestClassSizeON_3dmArchiveTableStatus) {

  size_t sz = sizeof(ON_3dmArchiveTableStatus); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_BinaryArchive) {

  size_t sz = sizeof(ON_BinaryArchive); //512

  EXPECT_EQ(sz, 512);

}

TEST(ONTest, ONTestClassSizeON_3dmGoo) {

  size_t sz = sizeof(ON_3dmGoo); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_BinaryFile) {

  size_t sz = sizeof(ON_BinaryFile); //560

  EXPECT_EQ(sz, 560);

}

TEST(ONTest, ONTestClassSizeON_BinaryArchiveBuffer) {

  size_t sz = sizeof(ON_BinaryArchiveBuffer); //520

  EXPECT_EQ(sz, 520);

}

TEST(ONTest, ONTestClassSizeON_Read3dmBufferArchive) {

  size_t sz = sizeof(ON_Read3dmBufferArchive); //576

  EXPECT_EQ(sz, 576);

}

TEST(ONTest, ONTestClassSizeON_Write3dmBufferArchive) {

  size_t sz = sizeof(ON_Write3dmBufferArchive); //592

  EXPECT_EQ(sz, 592);

}

TEST(ONTest, ONTestClassSizeON_DebugWriteArchive) {

  size_t sz = sizeof(ON_DebugWriteArchive); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_UuidPair) {

  size_t sz = sizeof(ON_UuidPair); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_UuidList) {

  size_t sz = sizeof(ON_UuidList); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_UuidIndexList) {

  size_t sz = sizeof(ON_UuidIndexList); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_UuidPtrList) {

  size_t sz = sizeof(ON_UuidPtrList); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_UuidPairList) {

  size_t sz = sizeof(ON_UuidPairList); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_2dexMap) {

  size_t sz = sizeof(ON_2dexMap); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_Base64EncodeStream) {

  size_t sz = sizeof(ON_Base64EncodeStream); //64

  EXPECT_EQ(sz, 64);

}

TEST(ONTest, ONTestClassSizeON_DecodeBase64) {

  size_t sz = sizeof(ON_DecodeBase64); //552

  EXPECT_EQ(sz, 552);

}

TEST(ONTest, ONTestClassSizeON_Extrusion) {

  size_t sz = sizeof(ON_Extrusion); //208

  EXPECT_EQ(sz, 208);

}

TEST(ONTest, ONTestClassSizeON_PolynomialCurve) {

  size_t sz = sizeof(ON_PolynomialCurve); //56

  EXPECT_EQ(sz, 56);

}

TEST(ONTest, ONTestClassSizeON_PolynomialSurface) {

  size_t sz = sizeof(ON_PolynomialSurface); //72

  EXPECT_EQ(sz, 72);

}

TEST(ONTest, ONTestClassSizeON_BezierCurve) {

  size_t sz = sizeof(ON_BezierCurve); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_BezierSurface) {

  size_t sz = sizeof(ON_BezierSurface); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_BezierCage) {

  size_t sz = sizeof(ON_BezierCage); //48

  EXPECT_EQ(sz, 48);

}

TEST(ONTest, ONTestClassSizeON_BezierCageMorph) {

  size_t sz = sizeof(ON_BezierCageMorph); //216

  EXPECT_EQ(sz, 216);

}

TEST(ONTest, ONTestClassSizeON_Bitmap) {

  size_t sz = sizeof(ON_Bitmap); //328

  EXPECT_EQ(sz, 328);

}

TEST(ONTest, ONTestClassSizeON_WindowsBitmap) {

  size_t sz = sizeof(ON_WindowsBitmap); //352

  EXPECT_EQ(sz, 352);

}

TEST(ONTest, ONTestClassSizeON_WindowsBitmapEx) {

  size_t sz = sizeof(ON_WindowsBitmapEx); //352

  EXPECT_EQ(sz, 352);

}

TEST(ONTest, ONTestClassSizeON_EmbeddedBitmap) {

  size_t sz = sizeof(ON_EmbeddedBitmap); //352

  EXPECT_EQ(sz, 352);

}

TEST(ONTest, ONTestClassSizeON_BoundingBox) {

  size_t sz = sizeof(ON_BoundingBox); //48

  EXPECT_EQ(sz, 48);

}

TEST(ONTest, ONTestClassSizeON_BoundingBoxAndHash) {

  size_t sz = sizeof(ON_BoundingBoxAndHash); //72

  EXPECT_EQ(sz, 72);

}

TEST(ONTest, ONTestClassSizeON_BoundingBoxCache) {

  size_t sz = sizeof(ON_BoundingBoxCache); //584

  EXPECT_EQ(sz, 584);

}

TEST(ONTest, ONTestClassSizeON_Box) {

  size_t sz = sizeof(ON_Box); //176

  EXPECT_EQ(sz, 176);

}

TEST(ONTest, ONTestClassSizeON_BrepVertex) {

  size_t sz = sizeof(ON_BrepVertex); //88

  EXPECT_EQ(sz, 88);

}

TEST(ONTest, ONTestClassSizeON_BrepEdge) {

  size_t sz = sizeof(ON_BrepEdge); //136

  EXPECT_EQ(sz, 136);

}

TEST(ONTest, ONTestClassSizeON_BrepTrim) {

  size_t sz = sizeof(ON_BrepTrim); //232

  EXPECT_EQ(sz, 232);

}

TEST(ONTest, ONTestClassSizeON_BrepLoop) {

  size_t sz = sizeof(ON_BrepLoop); //120

  EXPECT_EQ(sz, 120);

}

TEST(ONTest, ONTestClassSizeON_BrepFaceSide) {

  size_t sz = sizeof(ON_BrepFaceSide); //48

  EXPECT_EQ(sz, 48);

}

TEST(ONTest, ONTestClassSizeON_BrepRegion) {

  size_t sz = sizeof(ON_BrepRegion); //120

  EXPECT_EQ(sz, 120);

}

TEST(ONTest, ONTestClassSizeON_BrepEdgeArray) {

  size_t sz = sizeof(ON_BrepEdgeArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_BrepTrimArray) {

  size_t sz = sizeof(ON_BrepTrimArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_BrepLoopArray) {

  size_t sz = sizeof(ON_BrepLoopArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_BrepFaceArray) {

  size_t sz = sizeof(ON_BrepFaceArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_BrepRegionTopology) {

  size_t sz = sizeof(ON_BrepRegionTopology); //56

  EXPECT_EQ(sz, 56);

}

TEST(ONTest, ONTestClassSizeON_Brep) {

  size_t sz = sizeof(ON_Brep); //312

  EXPECT_EQ(sz, 312);

}

TEST(ONTest, ONTestClassSizeON_Circle) {

  size_t sz = sizeof(ON_Circle); //136

  EXPECT_EQ(sz, 136);

}

TEST(ONTest, ONTestClassSizeON_Color) {

  size_t sz = sizeof(ON_Color); //4

  EXPECT_EQ(sz, 4);

}

TEST(ONTest, ONTestClassSizeON_CompressStream) {

  size_t sz = sizeof(ON_CompressStream); //64

  EXPECT_EQ(sz, 64);

}

TEST(ONTest, ONTestClassSizeON_UncompressStream) {

  size_t sz = sizeof(ON_UncompressStream); //64

  EXPECT_EQ(sz, 64);

}

TEST(ONTest, ONTestClassSizeON_CompressedBuffer) {

  size_t sz = sizeof(ON_CompressedBuffer); //48

  EXPECT_EQ(sz, 48);

}

TEST(ONTest, ONTestClassSizeON_ComponentStatus) {

  size_t sz = sizeof(ON_ComponentStatus); //1

  EXPECT_EQ(sz, 1);

}

TEST(ONTest, ONTestClassSizeON_AggregateComponentStatus) {

  size_t sz = sizeof(ON_AggregateComponentStatus); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_UniqueTester) {

  size_t sz = sizeof(ON_UniqueTester); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_Cone) {

  size_t sz = sizeof(ON_Cone); //144

  EXPECT_EQ(sz, 144);

}

TEST(ONTest, ONTestClassSizeON_StopWatch) {

  size_t sz = sizeof(ON_StopWatch); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_MeshCurveParameters) {

  size_t sz = sizeof(ON_MeshCurveParameters); //80

  EXPECT_EQ(sz, 80);

}

TEST(ONTest, ONTestClassSizeON_Curve) {

  size_t sz = sizeof(ON_Curve); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_CurveArray) {

  size_t sz = sizeof(ON_CurveArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_CurveOnSurface) {

  size_t sz = sizeof(ON_CurveOnSurface); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_CurveProxy) {

  size_t sz = sizeof(ON_CurveProxy); //64

  EXPECT_EQ(sz, 64);

}

TEST(ONTest, ONTestClassSizeON_Cylinder) {

  size_t sz = sizeof(ON_Cylinder); //152

  EXPECT_EQ(sz, 152);

}

TEST(ONTest, ONTestClassSizeON_2dex) {

  size_t sz = sizeof(ON_2dex); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_2udex) {

  size_t sz = sizeof(ON_2udex); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_3dex) {

  size_t sz = sizeof(ON_3dex); //12

  EXPECT_EQ(sz, 12);

}

TEST(ONTest, ONTestClassSizeON_3udex) {

  size_t sz = sizeof(ON_3udex); //12

  EXPECT_EQ(sz, 12);

}

TEST(ONTest, ONTestClassSizeON_4dex) {

  size_t sz = sizeof(ON_4dex); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_4udex) {

  size_t sz = sizeof(ON_4udex); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_COMPONENT_INDEX) {

  size_t sz = sizeof(ON_COMPONENT_INDEX); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_DetailView) {

  size_t sz = sizeof(ON_DetailView); //1608

  EXPECT_EQ(sz, 1608);

}

TEST(ONTest, ONTestClassSizeON_Dimension) {

  size_t sz = sizeof(ON_Dimension); //880

  EXPECT_EQ(sz, 880);

}

TEST(ONTest, ONTestClassSizeON_DimLinear) {

  size_t sz = sizeof(ON_DimLinear); //912

  EXPECT_EQ(sz, 912);

}

TEST(ONTest, ONTestClassSizeON_DimAngular) {

  size_t sz = sizeof(ON_DimAngular); //944

  EXPECT_EQ(sz, 944);

}

TEST(ONTest, ONTestClassSizeON_DimRadial) {

  size_t sz = sizeof(ON_DimRadial); //912

  EXPECT_EQ(sz, 912);

}

TEST(ONTest, ONTestClassSizeON_DimOrdinate) {

  size_t sz = sizeof(ON_DimOrdinate); //936

  EXPECT_EQ(sz, 936);

}

TEST(ONTest, ONTestClassSizeON_Centermark) {

  size_t sz = sizeof(ON_Centermark); //888

  EXPECT_EQ(sz, 888);

}

TEST(ONTest, ONTestClassSizeON_Arrowhead) {

  size_t sz = sizeof(ON_Arrowhead); //20

  EXPECT_EQ(sz, 20);

}

TEST(ONTest, ONTestClassSizeON_TextMask) {

  size_t sz = sizeof(ON_TextMask); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_DimStyle) {

  size_t sz = sizeof(ON_DimStyle); //1104

  EXPECT_EQ(sz, 1104);

}

TEST(ONTest, ONTestClassSizeON_DimStyleContext) {

  size_t sz = sizeof(ON_DimStyleContext); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_Ellipse) {

  size_t sz = sizeof(ON_Ellipse); //144

  EXPECT_EQ(sz, 144);

}

TEST(ONTest, ONTestClassSizeON_ErrorEvent) {

  size_t sz = sizeof(ON_ErrorEvent); //160

  EXPECT_EQ(sz, 160);

}

TEST(ONTest, ONTestClassSizeON_ErrorLog) {

  size_t sz = sizeof(ON_ErrorLog); //5136

  EXPECT_EQ(sz, 5136);

}

TEST(ONTest, ONTestClassSizeON_FileSystem) {

  size_t sz = sizeof(ON_FileSystem); //1

  EXPECT_EQ(sz, 1);

}

TEST(ONTest, ONTestClassSizeON_FileSystemPath) {

  size_t sz = sizeof(ON_FileSystemPath); //1

  EXPECT_EQ(sz, 1);

}

TEST(ONTest, ONTestClassSizeON_FileStream) {

  size_t sz = sizeof(ON_FileStream); //1

  EXPECT_EQ(sz, 1);

}

TEST(ONTest, ONTestClassSizeON_ContentHash) {

  size_t sz = sizeof(ON_ContentHash); //64

  EXPECT_EQ(sz, 64);

}

TEST(ONTest, ONTestClassSizeON_FileReference) {

  size_t sz = sizeof(ON_FileReference); //184

  EXPECT_EQ(sz, 184);

}

TEST(ONTest, ONTestClassSizeON_FileIterator) {

  size_t sz = sizeof(ON_FileIterator); //56

  EXPECT_EQ(sz, 56);

}

TEST(ONTest, ONTestClassSizeON_FontMetrics) {

  size_t sz = sizeof(ON_FontMetrics); //64

  EXPECT_EQ(sz, 64);

}

TEST(ONTest, ONTestClassSizeON_TextBox) {

  size_t sz = sizeof(ON_TextBox); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_OutlineFigurePoint) {

  size_t sz = sizeof(ON_OutlineFigurePoint); //12

  EXPECT_EQ(sz, 12);

}

TEST(ONTest, ONTestClassSizeON_OutlineFigure) {

  size_t sz = sizeof(ON_OutlineFigure); //72

  EXPECT_EQ(sz, 72);

}

TEST(ONTest, ONTestClassSizeON_Outline) {

  size_t sz = sizeof(ON_Outline); //120

  EXPECT_EQ(sz, 120);

}

TEST(ONTest, ONTestClassSizeON_OutlineAccumulator) {

  size_t sz = sizeof(ON_OutlineAccumulator); //96

  EXPECT_EQ(sz, 96);

}

TEST(ONTest, ONTestClassSizeON_FontGlyph) {

  size_t sz = sizeof(ON_FontGlyph); //96

  EXPECT_EQ(sz, 96);

}

TEST(ONTest, ONTestClassSizeON_WindowsDWriteFontInformation) {

  size_t sz = sizeof(ON_WindowsDWriteFontInformation); //696

  EXPECT_EQ(sz, 696);

}

TEST(ONTest, ONTestClassSizeON_FontFaceQuartet) {

  size_t sz = sizeof(ON_FontFaceQuartet); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_Font) {

  size_t sz = sizeof(ON_Font); //192

  EXPECT_EQ(sz, 192);

}

TEST(ONTest, ONTestClassSizeON_FontList) {

  size_t sz = sizeof(ON_FontList); //248

  EXPECT_EQ(sz, 248);

}

TEST(ONTest, ONTestClassSizeON_2fPoint) {

  size_t sz = sizeof(ON_2fPoint); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_3fPoint) {

  size_t sz = sizeof(ON_3fPoint); //12

  EXPECT_EQ(sz, 12);

}

TEST(ONTest, ONTestClassSizeON_4fPoint) {

  size_t sz = sizeof(ON_4fPoint); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_2fVector) {

  size_t sz = sizeof(ON_2fVector); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_3fVector) {

  size_t sz = sizeof(ON_3fVector); //12

  EXPECT_EQ(sz, 12);

}

TEST(ONTest, ONTestClassSizeON_FixedSizePool) {

  size_t sz = sizeof(ON_FixedSizePool); //72

  EXPECT_EQ(sz, 72);

}

TEST(ONTest, ONTestClassSizeON_FixedSizePoolIterator) {

  size_t sz = sizeof(ON_FixedSizePoolIterator); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_FunctionList) {

  size_t sz = sizeof(ON_FunctionList); //96

  EXPECT_EQ(sz, 96);

}

TEST(ONTest, ONTestClassSizeON_Geometry) {

  size_t sz = sizeof(ON_Geometry); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_Group) {

  size_t sz = sizeof(ON_Group); //144

  EXPECT_EQ(sz, 144);

}

TEST(ONTest, ONTestClassSizeON_Hash32TableItem) {

  size_t sz = sizeof(ON_Hash32TableItem); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_Hash32Table) {

  size_t sz = sizeof(ON_Hash32Table); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_HatchLoop) {

  size_t sz = sizeof(ON_HatchLoop); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_HatchLine) {

  size_t sz = sizeof(ON_HatchLine); //64

  EXPECT_EQ(sz, 64);

}

TEST(ONTest, ONTestClassSizeON_HatchPattern) {

  size_t sz = sizeof(ON_HatchPattern); //184

  EXPECT_EQ(sz, 184);

}

TEST(ONTest, ONTestClassSizeON_Hatch) {

  size_t sz = sizeof(ON_Hatch); //208

  EXPECT_EQ(sz, 208);

}

TEST(ONTest, ONTestClassSizeON_CurveRegionBoundaryElement) {

  size_t sz = sizeof(ON_CurveRegionBoundaryElement); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_ReferencedComponentSettings) {

  size_t sz = sizeof(ON_ReferencedComponentSettings); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_InstanceDefinition) {

  size_t sz = sizeof(ON_InstanceDefinition); //568

  EXPECT_EQ(sz, 568);

}

TEST(ONTest, ONTestClassSizeON_InstanceRef) {

  size_t sz = sizeof(ON_InstanceRef); //208

  EXPECT_EQ(sz, 208);

}

TEST(ONTest, ONTestClassSizeON_2iPoint) {

  size_t sz = sizeof(ON_2iPoint); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_2iVector) {

  size_t sz = sizeof(ON_2iVector); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_2iBoundingBox) {

  size_t sz = sizeof(ON_2iBoundingBox); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_2iSize) {

  size_t sz = sizeof(ON_2iSize); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_4iRect) {

  size_t sz = sizeof(ON_4iRect); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_Layer) {

  size_t sz = sizeof(ON_Layer); //232

  EXPECT_EQ(sz, 232);

}

TEST(ONTest, ONTestClassSizeON_Leader) {

  size_t sz = sizeof(ON_Leader); //840

  EXPECT_EQ(sz, 840);

}

TEST(ONTest, ONTestClassSizeON_Light) {

  size_t sz = sizeof(ON_Light); //240

  EXPECT_EQ(sz, 240);

}

TEST(ONTest, ONTestClassSizeON_Line) {

  size_t sz = sizeof(ON_Line); //48

  EXPECT_EQ(sz, 48);

}

TEST(ONTest, ONTestClassSizeON_Triangle) {

  size_t sz = sizeof(ON_Triangle); //72

  EXPECT_EQ(sz, 72);

}

TEST(ONTest, ONTestClassSizeON_LineCurve) {

  size_t sz = sizeof(ON_LineCurve); //88

  EXPECT_EQ(sz, 88);

}

TEST(ONTest, ONTestClassSizeON_DisplayMaterialRef) {

  size_t sz = sizeof(ON_DisplayMaterialRef); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_LinetypeSegment) {

  size_t sz = sizeof(ON_LinetypeSegment); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_Linetype) {

  size_t sz = sizeof(ON_Linetype); //176

  EXPECT_EQ(sz, 176);

}

TEST(ONTest, ONTestClassSizeON_Locale) {

  size_t sz = sizeof(ON_Locale); //160

  EXPECT_EQ(sz, 160);

}

TEST(ONTest, ONTestClassSizeON_Lock) {

  size_t sz = sizeof(ON_Lock); //4

  EXPECT_EQ(sz, 4);

}

TEST(ONTest, ONTestClassSizeON_SerialNumberMap) {

  size_t sz = sizeof(ON_SerialNumberMap); //112

  EXPECT_EQ(sz, 112);

}

TEST(ONTest, ONTestClassSizeON_MappingChannel) {

  size_t sz = sizeof(ON_MappingChannel); //152

  EXPECT_EQ(sz, 152);

}

TEST(ONTest, ONTestClassSizeON_MappingRef) {

  size_t sz = sizeof(ON_MappingRef); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_MaterialRef) {

  size_t sz = sizeof(ON_MaterialRef); //60

  EXPECT_EQ(sz, 60);

}

TEST(ONTest, ONTestClassSizeON_Material) {

  size_t sz = sizeof(ON_Material); //312

  EXPECT_EQ(sz, 312);

}

TEST(ONTest, ONTestClassSizeON_PhysicallyBasedMaterial) {

  size_t sz = sizeof(ON_PhysicallyBasedMaterial); //1

  EXPECT_EQ(sz, 1);

}

TEST(ONTest, ONTestClassSizeON_Sum) {

  size_t sz = sizeof(ON_Sum); //28736

  EXPECT_EQ(sz, 28736);

}

TEST(ONTest, ONTestClassSizeON_Evaluator) {

  size_t sz = sizeof(ON_Evaluator); //64

  EXPECT_EQ(sz, 64);

}

TEST(ONTest, ONTestClassSizeON_Matrix) {

  size_t sz = sizeof(ON_Matrix); //72

  EXPECT_EQ(sz, 72);

}

TEST(ONTest, ONTestClassSizeON_MD5_Hash) {

  size_t sz = sizeof(ON_MD5_Hash); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_MD5) {

  size_t sz = sizeof(ON_MD5); //120

  EXPECT_EQ(sz, 120);

}

TEST(ONTest, ONTestClassSizeON_MemoryAllocationTracking) {

  size_t sz = sizeof(ON_MemoryAllocationTracking); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_MeshParameters) {

  size_t sz = sizeof(ON_MeshParameters); //224

  EXPECT_EQ(sz, 224);

}

TEST(ONTest, ONTestClassSizeON_MeshCurvatureStats) {

  size_t sz = sizeof(ON_MeshCurvatureStats); //64

  EXPECT_EQ(sz, 64);

}

TEST(ONTest, ONTestClassSizeON_MeshFace) {

  size_t sz = sizeof(ON_MeshFace); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_MeshTriangle) {

  size_t sz = sizeof(ON_MeshTriangle); //12

  EXPECT_EQ(sz, 12);

}

TEST(ONTest, ONTestClassSizeON_MeshFaceList) {

  size_t sz = sizeof(ON_MeshFaceList); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_MeshVertexFaceMap) {

  size_t sz = sizeof(ON_MeshVertexFaceMap); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_MeshNgonBuffer) {

  size_t sz = sizeof(ON_MeshNgonBuffer); //80

  EXPECT_EQ(sz, 80);

}

TEST(ONTest, ONTestClassSizeON_MeshNgon) {

  size_t sz = sizeof(ON_MeshNgon); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_MeshNgonAllocator) {

  size_t sz = sizeof(ON_MeshNgonAllocator); //168

  EXPECT_EQ(sz, 168);

}

TEST(ONTest, ONTestClassSizeON_MeshTopology) {

  size_t sz = sizeof(ON_MeshTopology); //120

  EXPECT_EQ(sz, 120);

}

TEST(ONTest, ONTestClassSizeON_MeshPartition) {

  size_t sz = sizeof(ON_MeshPartition); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_MappingTag) {

  size_t sz = sizeof(ON_MappingTag); //152

  EXPECT_EQ(sz, 152);

}

TEST(ONTest, ONTestClassSizeON_TextureCoordinates) {

  size_t sz = sizeof(ON_TextureCoordinates); //184

  EXPECT_EQ(sz, 184);

}

TEST(ONTest, ONTestClassSizeON_Mesh) {

  size_t sz = sizeof(ON_Mesh); //1760

  EXPECT_EQ(sz, 1760);

}

TEST(ONTest, ONTestClassSizeON_MeshCache) {

  size_t sz = sizeof(ON_MeshCache); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_MeshNgonIterator) {

  size_t sz = sizeof(ON_MeshNgonIterator); //128

  EXPECT_EQ(sz, 128);

}

TEST(ONTest, ONTestClassSizeON_MeshComponentRef) {

  size_t sz = sizeof(ON_MeshComponentRef); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_ModelComponent) {

  size_t sz = sizeof(ON_ModelComponent); //144

  EXPECT_EQ(sz, 144);

}

TEST(ONTest, ONTestClassSizeON_ModelComponentContentMark) {

  size_t sz = sizeof(ON_ModelComponentContentMark); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_ModelComponentTypeIterator) {

  size_t sz = sizeof(ON_ModelComponentTypeIterator); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_ModelComponentReference) {

  size_t sz = sizeof(ON_ModelComponentReference); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_ModelComponentWeakReference) {

  size_t sz = sizeof(ON_ModelComponentWeakReference); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_ModelGeometryComponent) {

  size_t sz = sizeof(ON_ModelGeometryComponent); //176

  EXPECT_EQ(sz, 176);

}

TEST(ONTest, ONTestClassSizeON_NurbsCurve) {

  size_t sz = sizeof(ON_NurbsCurve); //64

  EXPECT_EQ(sz, 64);

}

TEST(ONTest, ONTestClassSizeON_TensorProduct) {

  size_t sz = sizeof(ON_TensorProduct); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_NurbsSurface) {

  size_t sz = sizeof(ON_NurbsSurface); //88

  EXPECT_EQ(sz, 88);

}

TEST(ONTest, ONTestClassSizeON_NurbsCage) {

  size_t sz = sizeof(ON_NurbsCage); //112

  EXPECT_EQ(sz, 112);

}

TEST(ONTest, ONTestClassSizeON_MorphControl) {

  size_t sz = sizeof(ON_MorphControl); //688

  EXPECT_EQ(sz, 688);

}

TEST(ONTest, ONTestClassSizeON_CageMorph) {

  size_t sz = sizeof(ON_CageMorph); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_ClassId) {

  size_t sz = sizeof(ON_ClassId); //272

  EXPECT_EQ(sz, 272);

}

TEST(ONTest, ONTestClassSizeON_UserString) {

  size_t sz = sizeof(ON_UserString); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_Object) {

  size_t sz = sizeof(ON_Object); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_CurveProxyHistory) {

  size_t sz = sizeof(ON_CurveProxyHistory); //344

  EXPECT_EQ(sz, 344);

}

TEST(ONTest, ONTestClassSizeON_PolyEdgeHistory) {

  size_t sz = sizeof(ON_PolyEdgeHistory); //120

  EXPECT_EQ(sz, 120);

}

TEST(ONTest, ONTestClassSizeON_HistoryRecord) {

  size_t sz = sizeof(ON_HistoryRecord); //264

  EXPECT_EQ(sz, 264);

}

TEST(ONTest, ONTestClassSizeON_ObjRefEvaluationParameter) {

  size_t sz = sizeof(ON_ObjRefEvaluationParameter); //96

  EXPECT_EQ(sz, 96);

}

TEST(ONTest, ONTestClassSizeON_ObjRef_IRefID) {

  size_t sz = sizeof(ON_ObjRef_IRefID); //400

  EXPECT_EQ(sz, 400);

}

TEST(ONTest, ONTestClassSizeON_ObjRef) {

  size_t sz = sizeof(ON_ObjRef); //224

  EXPECT_EQ(sz, 224);

}

TEST(ONTest, ONTestClassSizeON_BumpFunction) {

  size_t sz = sizeof(ON_BumpFunction); //80

  EXPECT_EQ(sz, 80);

}

TEST(ONTest, ONTestClassSizeON_OffsetSurfaceValue) {

  size_t sz = sizeof(ON_OffsetSurfaceValue); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_OffsetSurfaceFunction) {

  size_t sz = sizeof(ON_OffsetSurfaceFunction); //104

  EXPECT_EQ(sz, 104);

}

TEST(ONTest, ONTestClassSizeON_ArithmeticCalculator) {

  size_t sz = sizeof(ON_ArithmeticCalculator); //1024

  EXPECT_EQ(sz, 1024);

}

TEST(ONTest, ONTestClassSizeON_LengthUnitName) {

  size_t sz = sizeof(ON_LengthUnitName); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_AngleUnitName) {

  size_t sz = sizeof(ON_AngleUnitName); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_ParseSettings) {

  size_t sz = sizeof(ON_ParseSettings); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_AerialPhotoImageFrustum) {

  size_t sz = sizeof(ON_AerialPhotoImageFrustum); //96

  EXPECT_EQ(sz, 96);

}

TEST(ONTest, ONTestClassSizeON_AerialPhotoCameraPosition) {

  size_t sz = sizeof(ON_AerialPhotoCameraPosition); //304

  EXPECT_EQ(sz, 304);

}

TEST(ONTest, ONTestClassSizeON_AerialPhotoImage) {

  size_t sz = sizeof(ON_AerialPhotoImage); //440

  EXPECT_EQ(sz, 440);

}

TEST(ONTest, ONTestClassSizeON_Plane) {

  size_t sz = sizeof(ON_Plane); //128

  EXPECT_EQ(sz, 128);

}

TEST(ONTest, ONTestClassSizeON_ClippingPlaneInfo) {

  size_t sz = sizeof(ON_ClippingPlaneInfo); //56

  EXPECT_EQ(sz, 56);

}

TEST(ONTest, ONTestClassSizeON_ClippingPlane) {

  size_t sz = sizeof(ON_ClippingPlane); //184

  EXPECT_EQ(sz, 184);

}

TEST(ONTest, ONTestClassSizeON_PlaneSurface) {

  size_t sz = sizeof(ON_PlaneSurface); //208

  EXPECT_EQ(sz, 208);

}

TEST(ONTest, ONTestClassSizeON_ClippingPlaneSurface) {

  size_t sz = sizeof(ON_ClippingPlaneSurface); //392

  EXPECT_EQ(sz, 392);

}

TEST(ONTest, ONTestClassSizeON_PlugInRef) {

  size_t sz = sizeof(ON_PlugInRef); //120

  EXPECT_EQ(sz, 120);

}

TEST(ONTest, ONTestClassSizeON_Interval) {

  size_t sz = sizeof(ON_Interval); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_2dPoint) {

  size_t sz = sizeof(ON_2dPoint); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_3dPoint) {

  size_t sz = sizeof(ON_3dPoint); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_4dPoint) {

  size_t sz = sizeof(ON_4dPoint); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_2dVector) {

  size_t sz = sizeof(ON_2dVector); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_3dVector) {

  size_t sz = sizeof(ON_3dVector); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_3dRay) {

  size_t sz = sizeof(ON_3dRay); //48

  EXPECT_EQ(sz, 48);

}

TEST(ONTest, ONTestClassSizeON_PlaneEquation) {

  size_t sz = sizeof(ON_PlaneEquation); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_SurfaceCurvature) {

  size_t sz = sizeof(ON_SurfaceCurvature); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_2dPointArray) {

  size_t sz = sizeof(ON_2dPointArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_2fPointArray) {

  size_t sz = sizeof(ON_2fPointArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_3dPointArray) {

  size_t sz = sizeof(ON_3dPointArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_3fPointArray) {

  size_t sz = sizeof(ON_3fPointArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_4dPointArray) {

  size_t sz = sizeof(ON_4dPointArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_4fPointArray) {

  size_t sz = sizeof(ON_4fPointArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_2dVectorArray) {

  size_t sz = sizeof(ON_2dVectorArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_2fVectorArray) {

  size_t sz = sizeof(ON_2fVectorArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_3dVectorArray) {

  size_t sz = sizeof(ON_3dVectorArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_3fVectorArray) {

  size_t sz = sizeof(ON_3fVectorArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_3dPointListRef) {

  size_t sz = sizeof(ON_3dPointListRef); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_2dSize) {

  size_t sz = sizeof(ON_2dSize); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_4dRect) {

  size_t sz = sizeof(ON_4dRect); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_WindingNumber) {

  size_t sz = sizeof(ON_WindingNumber); //56

  EXPECT_EQ(sz, 56);

}

TEST(ONTest, ONTestClassSizeON_PeriodicDomain) {

  size_t sz = sizeof(ON_PeriodicDomain); //72

  EXPECT_EQ(sz, 72);

}

TEST(ONTest, ONTestClassSizeON_PointCloud) {

  size_t sz = sizeof(ON_PointCloud); //328

  EXPECT_EQ(sz, 328);

}

TEST(ONTest, ONTestClassSizeON_Point) {

  size_t sz = sizeof(ON_Point); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_PointGrid) {

  size_t sz = sizeof(ON_PointGrid); //56

  EXPECT_EQ(sz, 56);

}

TEST(ONTest, ONTestClassSizeON_PolyCurve) {

  size_t sz = sizeof(ON_PolyCurve); //64

  EXPECT_EQ(sz, 64);

}

TEST(ONTest, ONTestClassSizeON_Polyline) {

  size_t sz = sizeof(ON_Polyline); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_PolylineCurve) {

  size_t sz = sizeof(ON_PolylineCurve); //72

  EXPECT_EQ(sz, 72);

}

TEST(ONTest, ONTestClassSizeON_ProgressReporter) {

  size_t sz = sizeof(ON_ProgressReporter); //64

  EXPECT_EQ(sz, 64);

}

TEST(ONTest, ONTestClassSizeON_ProgressStepCounter) {

  size_t sz = sizeof(ON_ProgressStepCounter); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_Quaternion) {

  size_t sz = sizeof(ON_Quaternion); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_RandomNumberGenerator) {

  size_t sz = sizeof(ON_RandomNumberGenerator); //2500

  EXPECT_EQ(sz, 2500);

}

TEST(ONTest, ONTestClassSizeON_RenderingAttributes) {

  size_t sz = sizeof(ON_RenderingAttributes); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_ObjectRenderingAttributes) {

  size_t sz = sizeof(ON_ObjectRenderingAttributes); //56

  EXPECT_EQ(sz, 56);

}

TEST(ONTest, ONTestClassSizeON_RevSurface) {

  size_t sz = sizeof(ON_RevSurface); //160

  EXPECT_EQ(sz, 160);

}

TEST(ONTest, ONTestClassSizeON_RTreeMemPool) {

  size_t sz = sizeof(ON_RTreeMemPool); //56

  EXPECT_EQ(sz, 56);

}

TEST(ONTest, ONTestClassSizeON_RTreeIterator) {

  size_t sz = sizeof(ON_RTreeIterator); //528

  EXPECT_EQ(sz, 528);

}

TEST(ONTest, ONTestClassSizeON_RTree) {

  size_t sz = sizeof(ON_RTree); //72

  EXPECT_EQ(sz, 72);

}

TEST(ONTest, ONTestClassSizeON_SHA1_Hash) {

  size_t sz = sizeof(ON_SHA1_Hash); //20

  EXPECT_EQ(sz, 20);

}

TEST(ONTest, ONTestClassSizeON_SHA1) {

  size_t sz = sizeof(ON_SHA1); //128

  EXPECT_EQ(sz, 128);

}

TEST(ONTest, ONTestClassSizeON_SleepLock) {

  size_t sz = sizeof(ON_SleepLock); //4

  EXPECT_EQ(sz, 4);

}

TEST(ONTest, ONTestClassSizeON_SleepLockGuard) {

  size_t sz = sizeof(ON_SleepLockGuard); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_Sphere) {

  size_t sz = sizeof(ON_Sphere); //136

  EXPECT_EQ(sz, 136);

}

TEST(ONTest, ONTestClassSizeON_CheckSum) {

  size_t sz = sizeof(ON_CheckSum); //48

  EXPECT_EQ(sz, 48);

}

TEST(ONTest, ONTestClassSizeON_StringBuffer) {

  size_t sz = sizeof(ON_StringBuffer); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_wStringBuffer) {

  size_t sz = sizeof(ON_wStringBuffer); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_String) {

  size_t sz = sizeof(ON_String); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_wString) {

  size_t sz = sizeof(ON_wString); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_NameHash) {

  size_t sz = sizeof(ON_NameHash); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_UnitSystem) {

  size_t sz = sizeof(ON_UnitSystem); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_LengthValue) {

  size_t sz = sizeof(ON_LengthValue); //48

  EXPECT_EQ(sz, 48);

}

TEST(ONTest, ONTestClassSizeON_AngleValue) {

  size_t sz = sizeof(ON_AngleValue); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_ScaleValue) {

  size_t sz = sizeof(ON_ScaleValue); //128

  EXPECT_EQ(sz, 128);

}

TEST(ONTest, ONTestClassSizeON_SubDVertexPtr) {

  size_t sz = sizeof(ON_SubDVertexPtr); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_SubDEdgePtr) {

  size_t sz = sizeof(ON_SubDEdgePtr); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_SubDFacePtr) {

  size_t sz = sizeof(ON_SubDFacePtr); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_SubDComponentPtr) {

  size_t sz = sizeof(ON_SubDComponentPtr); //8

  EXPECT_EQ(sz, 8);

}

TEST(ONTest, ONTestClassSizeON_SubDComponentRefList) {

  size_t sz = sizeof(ON_SubDComponentRefList); //72

  EXPECT_EQ(sz, 72);

}

TEST(ONTest, ONTestClassSizeON_SubDSectorType) {

  size_t sz = sizeof(ON_SubDSectorType); //40

  EXPECT_EQ(sz, 40);

}

TEST(ONTest, ONTestClassSizeON_SumSurface) {

  size_t sz = sizeof(ON_SumSurface); //104

  EXPECT_EQ(sz, 104);

}

TEST(ONTest, ONTestClassSizeON_Surface) {

  size_t sz = sizeof(ON_Surface); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_SurfaceProperties) {

  size_t sz = sizeof(ON_SurfaceProperties); //80

  EXPECT_EQ(sz, 80);

}

TEST(ONTest, ONTestClassSizeON_SurfaceArray) {

  size_t sz = sizeof(ON_SurfaceArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_SurfaceProxy) {

  size_t sz = sizeof(ON_SurfaceProxy); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_Terminator) {

  size_t sz = sizeof(ON_Terminator); //56

  EXPECT_EQ(sz, 56);

}

TEST(ONTest, ONTestClassSizeON_TextContent) {

  size_t sz = sizeof(ON_TextContent); //216

  EXPECT_EQ(sz, 216);

}

TEST(ONTest, ONTestClassSizeON_TextStyle) {

  size_t sz = sizeof(ON_TextStyle); //208

  EXPECT_EQ(sz, 208);

}

TEST(ONTest, ONTestClassSizeON_TextContext) {

  size_t sz = sizeof(ON_TextContext); //1

  EXPECT_EQ(sz, 1);

}

TEST(ONTest, ONTestClassSizeON_TextLog) {

  size_t sz = sizeof(ON_TextLog); //120

  EXPECT_EQ(sz, 120);

}

TEST(ONTest, ONTestClassSizeON_TextLogIndent) {

  size_t sz = sizeof(ON_TextLogIndent); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_TextHash) {

  size_t sz = sizeof(ON_TextHash); //480

  EXPECT_EQ(sz, 480);

}

TEST(ONTest, ONTestClassSizeON_Text) {

  size_t sz = sizeof(ON_Text); //792

  EXPECT_EQ(sz, 792);

}

TEST(ONTest, ONTestClassSizeON_StackedText) {

  size_t sz = sizeof(ON_StackedText); //32

  EXPECT_EQ(sz, 32);

}

TEST(ONTest, ONTestClassSizeON_TextRun) {

  size_t sz = sizeof(ON_TextRun); //240

  EXPECT_EQ(sz, 240);

}

TEST(ONTest, ONTestClassSizeON_TextRunArray) {

  size_t sz = sizeof(ON_TextRunArray); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_Texture) {

  size_t sz = sizeof(ON_Texture); //512

  EXPECT_EQ(sz, 512);

}

TEST(ONTest, ONTestClassSizeON_TextureMapping) {

  size_t sz = sizeof(ON_TextureMapping); //560

  EXPECT_EQ(sz, 560);

}

TEST(ONTest, ONTestClassSizeON_ComponentAttributes) {

  size_t sz = sizeof(ON_ComponentAttributes); //1

  EXPECT_EQ(sz, 1);

}

TEST(ONTest, ONTestClassSizeON_Torus) {

  size_t sz = sizeof(ON_Torus); //144

  EXPECT_EQ(sz, 144);

}

TEST(ONTest, ONTestClassSizeON_UserData) {

  size_t sz = sizeof(ON_UserData); //200

  EXPECT_EQ(sz, 200);

}

TEST(ONTest, ONTestClassSizeON_UnknownUserData) {

  size_t sz = sizeof(ON_UnknownUserData); //240

  EXPECT_EQ(sz, 240);

}

TEST(ONTest, ONTestClassSizeON_ObsoleteUserData) {

  size_t sz = sizeof(ON_ObsoleteUserData); //216

  EXPECT_EQ(sz, 216);

}

TEST(ONTest, ONTestClassSizeON_UserStringList) {

  size_t sz = sizeof(ON_UserStringList); //224

  EXPECT_EQ(sz, 224);

}

TEST(ONTest, ONTestClassSizeON_UserDataHolder) {

  size_t sz = sizeof(ON_UserDataHolder); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_DocumentUserStringList) {

  size_t sz = sizeof(ON_DocumentUserStringList); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_UuidIndex) {

  size_t sz = sizeof(ON_UuidIndex); //20

  EXPECT_EQ(sz, 20);

}

TEST(ONTest, ONTestClassSizeON_UuidPtr) {

  size_t sz = sizeof(ON_UuidPtr); //24

  EXPECT_EQ(sz, 24);

}

TEST(ONTest, ONTestClassSizeON_Viewport) {

  size_t sz = sizeof(ON_Viewport); //584

  EXPECT_EQ(sz, 584);

}

TEST(ONTest, ONTestClassSizeON_Workspace) {

  size_t sz = sizeof(ON_Workspace); //16

  EXPECT_EQ(sz, 16);

}

TEST(ONTest, ONTestClassSizeON_Xform) {

  size_t sz = sizeof(ON_Xform); //128

  EXPECT_EQ(sz, 128);

}

TEST(ONTest, ONTestClassSizeON_ClippingRegion) {

  size_t sz = sizeof(ON_ClippingRegion); //784

  EXPECT_EQ(sz, 784);

}

TEST(ONTest, ONTestClassSizeON_ClippingRegionPoints) {

  size_t sz = sizeof(ON_ClippingRegionPoints); //48

  EXPECT_EQ(sz, 48);

}

TEST(ONTest, ONTestClassSizeON_PickPoint) {

  size_t sz = sizeof(ON_PickPoint); //72

  EXPECT_EQ(sz, 72);

}

TEST(ONTest, ONTestClassSizeON_Localizer) {

  size_t sz = sizeof(ON_Localizer); //88

  EXPECT_EQ(sz, 88);

}

TEST(ONTest, ONTestClassSizeON_SpaceMorph) {

  size_t sz = sizeof(ON_SpaceMorph); //32

  EXPECT_EQ(sz, 32);

}


