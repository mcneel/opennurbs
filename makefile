###############################################################
#
# Gnu tools makefile
#
# gnumake, gcc, g++
#
# (Tested with gcc 4.2.1 on OSX 10.8.1)
#
# If you want to use other versions of make or other compilers,
# then you will have to modify this file.
#

RM = /bin/rm
# If you don't mind the risk of using the -f option and your rm 
# asks too many questions, then uncomment the next line.
# RM = /bin/rm -f

AR = ar qvl

# If your system doesn't use ranlib, uncomment the "echo" define.
RANLIB = ranlib
# RANLIB = echo

###############################################################
#
#

# Choose one of the following definitions for GCC_OPTIMIZER_FLAGS
# unoptimized debug build
ON_GNU_OPTIMIZER_FLAGS = -g
# optimized release build
#ON_GNU_OPTIMIZER_FLAGS = -O

ON_GNU_WARNING_FLAGS = -Wall \
	-Wno-overloaded-virtual \
	-Wno-switch \
	-Wno-unknown-pragmas \
	-Wno-unused-private-field
	
#	-Wno-inconsistent-missing-override \


# Required to get Apple platform NSString declaration
GCC_INPUT_FILETYPE = sourcecode.cpp.objcpp

# In order to get full wide character (UNICODE) support, you need to define _GNU_SOURCE
ON_GNU_COMMON_FLAGS = $(ON_GNU_OPTIMIZER_FLAGS) $(ON_GNU_WARNING_FLAGS) -D_GNU_SOURCE -I.

# C compiler and flags
CC = gcc
CFLAGS = $(ON_GNU_COMMON_FLAGS) 

# C++ compiler and flags
CCC = g++
CCFLAGS = $(ON_GNU_COMMON_FLAGS) -std=c++14

LINK = $(CCC)
LINKFLAGS =

###############################################################

.cpp.o :
	$(CCC) $(CCFLAGS) -c $*.cpp -o $*.o

###############################################################

OPENNURBS_LIB_NAME = opennurbs_public
OPENNURBS_LIB_FILE = lib$(OPENNURBS_LIB_NAME).a

ON_INC = opennurbs.h \
	opennurbs_3dm.h \
	opennurbs_3dm_attributes.h \
	opennurbs_3dm_properties.h \
	opennurbs_3dm_settings.h \
	opennurbs_annotationbase.h \
	opennurbs_arc.h \
	opennurbs_arccurve.h \
	opennurbs_archive.h \
	opennurbs_array.h \
	opennurbs_array_defs.h \
	opennurbs_base32.h \
	opennurbs_base64.h \
	opennurbs_beam.h \
	opennurbs_bezier.h \
	opennurbs_bitmap.h \
	opennurbs_bounding_box.h \
	opennurbs_box.h \
	opennurbs_brep.h \
	opennurbs_circle.h \
	opennurbs_color.h \
	opennurbs_compress.h \
	opennurbs_compstat.h \
	opennurbs_cone.h \
	opennurbs_crc.h \
	opennurbs_curve.h \
	opennurbs_curveonsurface.h \
	opennurbs_curveproxy.h \
	opennurbs_cylinder.h \
	opennurbs_date.h \
	opennurbs_defines.h \
	opennurbs_detail.h \
	opennurbs_dimension.h \
	opennurbs_dimensionformat.h \
	opennurbs_dimensionstyle.h \
	opennurbs_dll_resource.h \
	opennurbs_ellipse.h \
	opennurbs_error.h \
	opennurbs_evaluate_nurbs.h \
	opennurbs_extensions.h \
	opennurbs_file_utilities.h \
	opennurbs_font.h \
	opennurbs_fpoint.h \
	opennurbs_freetype.h \
	opennurbs_freetype_include.h \
	opennurbs_fsp.h \
	opennurbs_fsp_defs.h \
	opennurbs_function_list.h \
	opennurbs_geometry.h \
	opennurbs_gl.h \
	opennurbs_group.h \
	opennurbs_hash_table.h \
	opennurbs_hatch.h \
	opennurbs_hsort_template.h \
	opennurbs_input_libsdir.h \
	opennurbs_instance.h \
	opennurbs_internal_defines.h \
	opennurbs_internal_glyph.h \
	opennurbs_internal_V2_annotation.h \
	opennurbs_internal_V5_annotation.h \
	opennurbs_internal_V5_dimstyle.h \
	opennurbs_intersect.h \
	opennurbs_ipoint.h \
	opennurbs_knot.h \
	opennurbs_layer.h \
	opennurbs_leader.h \
	opennurbs_light.h \
	opennurbs_line.h \
	opennurbs_linecurve.h \
	opennurbs_linestyle.h \
	opennurbs_linetype.h \
	opennurbs_locale.h \
	opennurbs_lock.h \
	opennurbs_lookup.h \
	opennurbs_mapchan.h \
	opennurbs_material.h \
	opennurbs_math.h \
	opennurbs_matrix.h \
	opennurbs_md5.h \
	opennurbs_memory.h \
	opennurbs_mesh.h \
	opennurbs_model_component.h \
	opennurbs_model_geometry.h \
	opennurbs_nurbscurve.h \
	opennurbs_nurbssurface.h \
	opennurbs_object.h \
	opennurbs_object_history.h \
	opennurbs_objref.h \
	opennurbs_offsetsurface.h \
	opennurbs_optimize.h \
	opennurbs_parse.h \
	opennurbs_photogrammetry.h \
	opennurbs_plane.h \
	opennurbs_planesurface.h \
	opennurbs_pluginlist.h \
	opennurbs_point.h \
	opennurbs_pointcloud.h \
	opennurbs_pointgeometry.h \
	opennurbs_pointgrid.h \
	opennurbs_polycurve.h \
	opennurbs_polyedgecurve.h \
	opennurbs_polyline.h \
	opennurbs_polylinecurve.h \
	opennurbs_private_wrap_defs.h \
	opennurbs_progress_reporter.h \
	opennurbs_qsort_template.h \
	opennurbs_quaternion.h \
	opennurbs_rand.h \
	opennurbs_rendering.h \
	opennurbs_revsurface.h \
	opennurbs_rtree.h \
	opennurbs_sha1.h \
	opennurbs_sphere.h \
	opennurbs_std_string.h \
	opennurbs_string.h \
	opennurbs_string_value.h \
	opennurbs_subd.h \
	opennurbs_subd_data.h \
	opennurbs_sumsurface.h \
	opennurbs_surface.h \
	opennurbs_surfaceproxy.h \
	opennurbs_system.h \
	opennurbs_system_compiler.h \
	opennurbs_system_runtime.h \
	opennurbs_terminator.h \
	opennurbs_text.h \
	opennurbs_text_style.h \
	opennurbs_textcontext.h \
	opennurbs_textglyph.h \
	opennurbs_textiterator.h \
	opennurbs_textlog.h \
	opennurbs_textobject.h \
	opennurbs_textrun.h \
	opennurbs_texture.h \
	opennurbs_texture_mapping.h \
	opennurbs_topology.h \
	opennurbs_torus.h \
	opennurbs_unicode.h \
	opennurbs_userdata.h \
	opennurbs_uuid.h \
	opennurbs_version.h \
	opennurbs_version_number.h \
	opennurbs_viewport.h \
	opennurbs_windows_targetver.h \
	opennurbs_wip.h \
	opennurbs_workspace.h \
	opennurbs_xform.h \
	opennurbs_zlib.h

ON_SRC = opennurbs_3dm_attributes.cpp \
	opennurbs_3dm_properties.cpp \
	opennurbs_3dm_settings.cpp \
	opennurbs_annotationbase.cpp \
	opennurbs_arc.cpp \
	opennurbs_arccurve.cpp \
	opennurbs_archive.cpp \
	opennurbs_archive_manifest.cpp \
	opennurbs_array.cpp \
	opennurbs_base32.cpp \
	opennurbs_base64.cpp \
	opennurbs_beam.cpp \
	opennurbs_bezier.cpp \
	opennurbs_beziervolume.cpp \
	opennurbs_bitmap.cpp \
	opennurbs_bounding_box.cpp \
	opennurbs_box.cpp \
	opennurbs_brep.cpp \
	opennurbs_brep_extrude.cpp \
	opennurbs_brep_io.cpp \
	opennurbs_brep_isvalid.cpp \
	opennurbs_brep_region.cpp \
	opennurbs_brep_tools.cpp \
	opennurbs_brep_v2valid.cpp \
	opennurbs_calculator.cpp \
	opennurbs_circle.cpp \
	opennurbs_color.cpp \
	opennurbs_compress.cpp \
	opennurbs_compstat.cpp \
	opennurbs_cone.cpp \
	opennurbs_crc.cpp \
	opennurbs_curve.cpp \
	opennurbs_curveonsurface.cpp \
	opennurbs_curveproxy.cpp \
	opennurbs_cylinder.cpp \
	opennurbs_date.cpp \
	opennurbs_defines.cpp \
	opennurbs_detail.cpp \
	opennurbs_dimension.cpp \
	opennurbs_dimensionformat.cpp \
	opennurbs_dimensionstyle.cpp \
	opennurbs_ellipse.cpp \
	opennurbs_embedded_file.cpp \
	opennurbs_error.cpp \
	opennurbs_error_message.cpp \
	opennurbs_evaluate_nurbs.cpp \
	opennurbs_extensions.cpp \
	opennurbs_file_utilities.cpp \
	opennurbs_font.cpp \
	opennurbs_freetype.cpp \
	opennurbs_fsp.cpp \
	opennurbs_function_list.cpp \
	opennurbs_geometry.cpp \
	opennurbs_group.cpp \
	opennurbs_hash_table.cpp \
	opennurbs_hatch.cpp \
	opennurbs_instance.cpp \
	opennurbs_internal_V2_annotation.cpp \
	opennurbs_internal_V5_annotation.cpp \
	opennurbs_internal_V5_dimstyle.cpp \
	opennurbs_internal_Vx_annotation.cpp \
	opennurbs_intersect.cpp \
	opennurbs_ipoint.cpp \
	opennurbs_knot.cpp \
	opennurbs_layer.cpp \
	opennurbs_leader.cpp \
	opennurbs_light.cpp \
	opennurbs_line.cpp \
	opennurbs_linecurve.cpp \
	opennurbs_linetype.cpp \
	opennurbs_locale.cpp \
	opennurbs_lock.cpp \
	opennurbs_lookup.cpp \
	opennurbs_material.cpp \
	opennurbs_math.cpp \
	opennurbs_matrix.cpp \
	opennurbs_md5.cpp \
	opennurbs_memory_util.cpp \
	opennurbs_mesh.cpp \
	opennurbs_mesh_ngon.cpp \
	opennurbs_mesh_tools.cpp \
	opennurbs_mesh_topology.cpp \
	opennurbs_model_component.cpp \
	opennurbs_model_geometry.cpp \
	opennurbs_morph.cpp \
	opennurbs_nurbscurve.cpp \
	opennurbs_nurbssurface.cpp \
	opennurbs_nurbsvolume.cpp \
	opennurbs_object.cpp \
	opennurbs_object_history.cpp \
	opennurbs_objref.cpp \
	opennurbs_offsetsurface.cpp \
	opennurbs_optimize.cpp \
	opennurbs_parse_angle.cpp \
	opennurbs_parse_length.cpp \
	opennurbs_parse_number.cpp \
	opennurbs_parse_point.cpp \
	opennurbs_parse_settings.cpp \
	opennurbs_photogrammetry.cpp \
	opennurbs_plane.cpp \
	opennurbs_planesurface.cpp \
	opennurbs_pluginlist.cpp \
	opennurbs_point.cpp \
	opennurbs_pointcloud.cpp \
	opennurbs_pointgeometry.cpp \
	opennurbs_pointgrid.cpp \
	opennurbs_polycurve.cpp \
	opennurbs_polyedgecurve.cpp \
	opennurbs_polyline.cpp \
	opennurbs_polylinecurve.cpp \
	opennurbs_precompiledheader.cpp \
	opennurbs_progress_reporter.cpp \
	opennurbs_public_memory.cpp \
	opennurbs_quaternion.cpp \
	opennurbs_rand.cpp \
	opennurbs_revsurface.cpp \
	opennurbs_rtree.cpp \
	opennurbs_sha1.cpp \
	opennurbs_sort.cpp \
	opennurbs_sphere.cpp \
	opennurbs_statics.cpp \
	opennurbs_std_string_format.cpp \
	opennurbs_std_string_utf.cpp \
	opennurbs_string.cpp \
	opennurbs_string_compare.cpp \
	opennurbs_string_format.cpp \
	opennurbs_string_scan.cpp \
	opennurbs_string_values.cpp \
	opennurbs_subd.cpp \
	opennurbs_subd_archive.cpp \
	opennurbs_subd_copy.cpp \
	opennurbs_subd_data.cpp \
	opennurbs_subd_eval.cpp \
	opennurbs_subd_fragment.cpp \
	opennurbs_subd_frommesh.cpp \
	opennurbs_subd_heap.cpp \
	opennurbs_subd_iter.cpp \
	opennurbs_subd_limit.cpp \
	opennurbs_subd_matrix.cpp \
	opennurbs_subd_mesh.cpp \
	opennurbs_subd_ref.cpp \
	opennurbs_subd_ring.cpp \
	opennurbs_subd_sector.cpp \
	opennurbs_sum.cpp \
	opennurbs_sumsurface.cpp \
	opennurbs_surface.cpp \
	opennurbs_surfaceproxy.cpp \
	opennurbs_terminator.cpp \
	opennurbs_text.cpp \
	opennurbs_text_style.cpp \
	opennurbs_textcontext.cpp \
	opennurbs_textglyph.cpp \
	opennurbs_textiterator.cpp \
	opennurbs_textlog.cpp \
	opennurbs_textobject.cpp \
	opennurbs_textrun.cpp \
	opennurbs_topology.cpp \
	opennurbs_torus.cpp \
	opennurbs_unicode.cpp \
	opennurbs_units.cpp \
	opennurbs_userdata.cpp \
	opennurbs_userdata_obsolete.cpp \
	opennurbs_uuid.cpp \
	opennurbs_version.cpp \
	opennurbs_version_number.cpp \
	opennurbs_viewport.cpp \
	opennurbs_workspace.cpp \
	opennurbs_wstring.cpp \
	opennurbs_xform.cpp \
	opennurbs_zlib.cpp \
	opennurbs_zlib_memory.cpp

ON_OBJ = opennurbs_3dm_attributes.o \
	opennurbs_3dm_properties.o \
	opennurbs_3dm_settings.o \
	opennurbs_annotationbase.o \
	opennurbs_arc.o \
	opennurbs_arccurve.o \
	opennurbs_archive.o \
	opennurbs_archive_manifest.o \
	opennurbs_array.o \
	opennurbs_base32.o \
	opennurbs_base64.o \
	opennurbs_beam.o \
	opennurbs_bezier.o \
	opennurbs_beziervolume.o \
	opennurbs_bitmap.o \
	opennurbs_bounding_box.o \
	opennurbs_box.o \
	opennurbs_brep.o \
	opennurbs_brep_extrude.o \
	opennurbs_brep_io.o \
	opennurbs_brep_isvalid.o \
	opennurbs_brep_region.o \
	opennurbs_brep_tools.o \
	opennurbs_brep_v2valid.o \
	opennurbs_calculator.o \
	opennurbs_circle.o \
	opennurbs_color.o \
	opennurbs_compress.o \
	opennurbs_compstat.o \
	opennurbs_cone.o \
	opennurbs_crc.o \
	opennurbs_curve.o \
	opennurbs_curveonsurface.o \
	opennurbs_curveproxy.o \
	opennurbs_cylinder.o \
	opennurbs_date.o \
	opennurbs_defines.o \
	opennurbs_detail.o \
	opennurbs_dimension.o \
	opennurbs_dimensionformat.o \
	opennurbs_dimensionstyle.o \
	opennurbs_ellipse.o \
	opennurbs_embedded_file.o \
	opennurbs_error.o \
	opennurbs_error_message.o \
	opennurbs_evaluate_nurbs.o \
	opennurbs_extensions.o \
	opennurbs_file_utilities.o \
	opennurbs_font.o \
	opennurbs_freetype.o \
	opennurbs_fsp.o \
	opennurbs_function_list.o \
	opennurbs_geometry.o \
	opennurbs_group.o \
	opennurbs_hash_table.o \
	opennurbs_hatch.o \
	opennurbs_instance.o \
	opennurbs_internal_V2_annotation.o \
	opennurbs_internal_V5_annotation.o \
	opennurbs_internal_V5_dimstyle.o \
	opennurbs_internal_Vx_annotation.o \
	opennurbs_intersect.o \
	opennurbs_ipoint.o \
	opennurbs_knot.o \
	opennurbs_layer.o \
	opennurbs_leader.o \
	opennurbs_light.o \
	opennurbs_line.o \
	opennurbs_linecurve.o \
	opennurbs_linetype.o \
	opennurbs_locale.o \
	opennurbs_lock.o \
	opennurbs_lookup.o \
	opennurbs_material.o \
	opennurbs_math.o \
	opennurbs_matrix.o \
	opennurbs_md5.o \
	opennurbs_memory_util.o \
	opennurbs_mesh.o \
	opennurbs_mesh_ngon.o \
	opennurbs_mesh_tools.o \
	opennurbs_mesh_topology.o \
	opennurbs_model_component.o \
	opennurbs_model_geometry.o \
	opennurbs_morph.o \
	opennurbs_nurbscurve.o \
	opennurbs_nurbssurface.o \
	opennurbs_nurbsvolume.o \
	opennurbs_object.o \
	opennurbs_object_history.o \
	opennurbs_objref.o \
	opennurbs_offsetsurface.o \
	opennurbs_optimize.o \
	opennurbs_parse_angle.o \
	opennurbs_parse_length.o \
	opennurbs_parse_number.o \
	opennurbs_parse_point.o \
	opennurbs_parse_settings.o \
	opennurbs_photogrammetry.o \
	opennurbs_plane.o \
	opennurbs_planesurface.o \
	opennurbs_pluginlist.o \
	opennurbs_point.o \
	opennurbs_pointcloud.o \
	opennurbs_pointgeometry.o \
	opennurbs_pointgrid.o \
	opennurbs_polycurve.o \
	opennurbs_polyedgecurve.o \
	opennurbs_polyline.o \
	opennurbs_polylinecurve.o \
	opennurbs_precompiledheader.o \
	opennurbs_progress_reporter.o \
	opennurbs_public_memory.o \
	opennurbs_quaternion.o \
	opennurbs_rand.o \
	opennurbs_revsurface.o \
	opennurbs_rtree.o \
	opennurbs_sha1.o \
	opennurbs_sort.o \
	opennurbs_sphere.o \
	opennurbs_statics.o \
	opennurbs_std_string_format.o \
	opennurbs_std_string_utf.o \
	opennurbs_string.o \
	opennurbs_string_compare.o \
	opennurbs_string_format.o \
	opennurbs_string_scan.o \
	opennurbs_string_values.o \
	opennurbs_subd.o \
	opennurbs_subd_archive.o \
	opennurbs_subd_copy.o \
	opennurbs_subd_data.o \
	opennurbs_subd_eval.o \
	opennurbs_subd_fragment.o \
	opennurbs_subd_frommesh.o \
	opennurbs_subd_heap.o \
	opennurbs_subd_iter.o \
	opennurbs_subd_limit.o \
	opennurbs_subd_matrix.o \
	opennurbs_subd_mesh.o \
	opennurbs_subd_ref.o \
	opennurbs_subd_ring.o \
	opennurbs_subd_sector.o \
	opennurbs_sum.o \
	opennurbs_sumsurface.o \
	opennurbs_surface.o \
	opennurbs_surfaceproxy.o \
	opennurbs_terminator.o \
	opennurbs_text.o \
	opennurbs_text_style.o \
	opennurbs_textcontext.o \
	opennurbs_textglyph.o \
	opennurbs_textiterator.o \
	opennurbs_textlog.o \
	opennurbs_textobject.o \
	opennurbs_textrun.o \
	opennurbs_topology.o \
	opennurbs_torus.o \
	opennurbs_unicode.o \
	opennurbs_units.o \
	opennurbs_userdata.o \
	opennurbs_userdata_obsolete.o \
	opennurbs_uuid.o \
	opennurbs_version.o \
	opennurbs_version_number.o \
	opennurbs_viewport.o \
	opennurbs_workspace.o \
	opennurbs_wstring.o \
	opennurbs_xform.o \
	opennurbs_zlib.o \
	opennurbs_zlib_memory.o

ZLIB_INC = zlib/crc32.h \
		zlib/deflate.h \
		zlib/inffast.h \
		zlib/inffixed.h \
		zlib/inflate.h \
		zlib/inftrees.h \
		zlib/trees.h \
		zlib/zconf.h \
		zlib/zlib.h \
		zlib/zutil.h \

ZLIB_SRC = zlib/adler32.c \
		zlib/compress.c \
		zlib/crc32.c \
		zlib/deflate.c \
		zlib/infback.c \
		zlib/inffast.c \
		zlib/inflate.c \
		zlib/inftrees.c \
		zlib/trees.c \
		zlib/uncompr.c \
		zlib/zutil.c

ZLIB_OBJ = zlib/adler32.o \
		zlib/compress.o \
		zlib/crc32.o \
		zlib/deflate.o \
		zlib/infback.o \
		zlib/inffast.o \
		zlib/inflate.o \
		zlib/inftrees.o \
		zlib/trees.o \
		zlib/uncompr.o \
		zlib/zutil.o

FREETYPE_INC= freetype263/include/ft2build.h \
	freetype263/include/freetype/config/ftconfig.h \
	freetype263/include/freetype/config/ftheader.h \
	freetype263/include/freetype/config/ftmodule.h \
	freetype263/include/freetype/config/ftoption.h \
	freetype263/include/freetype/config/ftstdlib.h \
	freetype263/include/freetype/freetype.h \
	freetype263/include/freetype/ftadvanc.h \
	freetype263/include/freetype/ftautoh.h \
	freetype263/include/freetype/ftbbox.h \
	freetype263/include/freetype/ftbdf.h \
	freetype263/include/freetype/ftbitmap.h \
	freetype263/include/freetype/ftbzip2.h \
	freetype263/include/freetype/ftcache.h \
	freetype263/include/freetype/ftcffdrv.h \
	freetype263/include/freetype/ftchapters.h \
	freetype263/include/freetype/ftcid.h \
	freetype263/include/freetype/fterrdef.h \
	freetype263/include/freetype/fterrors.h \
	freetype263/include/freetype/ftfntfmt.h \
	freetype263/include/freetype/ftgasp.h \
	freetype263/include/freetype/ftglyph.h \
	freetype263/include/freetype/ftgxval.h \
	freetype263/include/freetype/ftgzip.h \
	freetype263/include/freetype/ftimage.h \
	freetype263/include/freetype/ftincrem.h \
	freetype263/include/freetype/ftlcdfil.h \
	freetype263/include/freetype/ftlist.h \
	freetype263/include/freetype/ftlzw.h \
	freetype263/include/freetype/ftmac.h \
	freetype263/include/freetype/ftmm.h \
	freetype263/include/freetype/ftmodapi.h \
	freetype263/include/freetype/ftmoderr.h \
	freetype263/include/freetype/ftotval.h \
	freetype263/include/freetype/ftoutln.h \
	freetype263/include/freetype/ftpfr.h \
	freetype263/include/freetype/ftrender.h \
	freetype263/include/freetype/ftsizes.h \
	freetype263/include/freetype/ftsnames.h \
	freetype263/include/freetype/ftstroke.h \
	freetype263/include/freetype/ftsynth.h \
	freetype263/include/freetype/ftsystem.h \
	freetype263/include/freetype/fttrigon.h \
	freetype263/include/freetype/ftttdrv.h \
	freetype263/include/freetype/fttypes.h \
	freetype263/include/freetype/ftwinfnt.h \
	freetype263/include/freetype/t1tables.h \
	freetype263/include/freetype/ttnameid.h \
	freetype263/include/freetype/tttables.h \
	freetype263/include/freetype/tttags.h \
	freetype263/include/freetype/ttunpat.h \
	freetype263/include/freetype/internal/autohint.h \
	freetype263/include/freetype/internal/ftcalc.h \
	freetype263/include/freetype/internal/ftdebug.h \
	freetype263/include/freetype/internal/ftdriver.h \
	freetype263/include/freetype/internal/ftgloadr.h \
	freetype263/include/freetype/internal/fthash.h \
	freetype263/include/freetype/internal/ftmemory.h \
	freetype263/include/freetype/internal/ftobjs.h \
	freetype263/include/freetype/internal/ftpic.h \
	freetype263/include/freetype/internal/ftrfork.h \
	freetype263/include/freetype/internal/ftserv.h \
	freetype263/include/freetype/internal/ftstream.h \
	freetype263/include/freetype/internal/fttrace.h \
	freetype263/include/freetype/internal/ftvalid.h \
	freetype263/include/freetype/internal/internal.h \
	freetype263/include/freetype/internal/psaux.h \
	freetype263/include/freetype/internal/pshints.h \
	freetype263/include/freetype/internal/sfnt.h \
	freetype263/include/freetype/internal/t1types.h \
	freetype263/include/freetype/internal/tttypes.h \
	freetype263/include/freetype/internal/services/svbdf.h \
	freetype263/include/freetype/internal/services/svcid.h \
	freetype263/include/freetype/internal/services/svfntfmt.h \
	freetype263/include/freetype/internal/services/svgldict.h \
	freetype263/include/freetype/internal/services/svgxval.h \
	freetype263/include/freetype/internal/services/svkern.h \
	freetype263/include/freetype/internal/services/svmm.h \
	freetype263/include/freetype/internal/services/svotval.h \
	freetype263/include/freetype/internal/services/svpfr.h \
	freetype263/include/freetype/internal/services/svpostnm.h \
	freetype263/include/freetype/internal/services/svprop.h \
	freetype263/include/freetype/internal/services/svpscmap.h \
	freetype263/include/freetype/internal/services/svpsinfo.h \
	freetype263/include/freetype/internal/services/svsfnt.h \
	freetype263/include/freetype/internal/services/svttcmap.h \
	freetype263/include/freetype/internal/services/svtteng.h \
	freetype263/include/freetype/internal/services/svttglyf.h \
	freetype263/include/freetype/internal/services/svwinfnt.h

FREETYPE_SRC = freetype263/src/autofit/autofit.c \
	freetype263/src/bdf/bdf.c \
	freetype263/src/cff/cff.c \
	freetype263/src/base/ftbase.c \
	freetype263/src/base/ftbitmap.c \
	freetype263/src/base/ftfstype.c \
	freetype263/src/base/ftgasp.c \
	freetype263/src/base/ftglyph.c \
	freetype263/src/gzip/ftgzip.c \
	freetype263/src/base/ftinit.c \
	freetype263/src/lzw/ftlzw.c \
	freetype263/src/base/ftstroke.c \
	freetype263/src/base/ftsystem.c \
	freetype263/src/smooth/smooth.c \
	freetype263/src/base/ftbbox.c \
	freetype263/src/base/ftfntfmt.c \
	freetype263/src/base/ftgxval.c \
	freetype263/src/base/ftlcdfil.c \
	freetype263/src/base/ftmm.c \
	freetype263/src/base/ftotval.c \
	freetype263/src/base/ftpatent.c \
	freetype263/src/base/ftpfr.c \
	freetype263/src/base/ftsynth.c \
	freetype263/src/base/fttype1.c \
	freetype263/src/base/ftwinfnt.c \
	freetype263/src/pcf/pcf.c \
	freetype263/src/pfr/pfr.c \
	freetype263/src/psaux/psaux.c \
	freetype263/src/pshinter/pshinter.c \
	freetype263/src/psnames/psmodule.c \
	freetype263/src/raster/raster.c \
	freetype263/src/sfnt/sfnt.c \
	freetype263/src/truetype/truetype.c \
	freetype263/src/type1/type1.c \
	freetype263/src/cid/type1cid.c \
	freetype263/src/type42/type42.c \
	freetype263/src/winfonts/winfnt.c \
	freetype263/src/cache/ftcache.c

FREETYPE_OBJ = freetype263/src/autofit/autofit.o \
	freetype263/src/bdf/bdf.o \
	freetype263/src/cff/cff.o \
	freetype263/src/base/ftbase.o \
	freetype263/src/base/ftbitmap.o \
	freetype263/src/base/ftfstype.o \
	freetype263/src/base/ftgasp.o \
	freetype263/src/base/ftglyph.o \
	freetype263/src/gzip/ftgzip.o \
	freetype263/src/base/ftinit.o \
	freetype263/src/lzw/ftlzw.o \
	freetype263/src/base/ftstroke.o \
	freetype263/src/base/ftsystem.o \
	freetype263/src/smooth/smooth.o \
	freetype263/src/base/ftbbox.o \
	freetype263/src/base/ftfntfmt.o \
	freetype263/src/base/ftgxval.o \
	freetype263/src/base/ftlcdfil.o \
	freetype263/src/base/ftmm.o \
	freetype263/src/base/ftotval.o \
	freetype263/src/base/ftpatent.o \
	freetype263/src/base/ftpfr.o \
	freetype263/src/base/ftsynth.o \
	freetype263/src/base/fttype1.o \
	freetype263/src/base/ftwinfnt.o \
	freetype263/src/pcf/pcf.o \
	freetype263/src/pfr/pfr.o \
	freetype263/src/psaux/psaux.o \
	freetype263/src/pshinter/pshinter.o \
	freetype263/src/psnames/psmodule.o \
	freetype263/src/raster/raster.o \
	freetype263/src/sfnt/sfnt.o \
	freetype263/src/truetype/truetype.o \
	freetype263/src/type1/type1.o \
	freetype263/src/cid/type1cid.o \
	freetype263/src/type42/type42.o \
	freetype263/src/winfonts/winfnt.o \
	freetype263/src/cache/ftcache.o

EXAMPLE_INC = examples.h \
		example_userdata/example_ud.h

EXAMPLE_OBJ = example_read/example_read.o \
      example_write/example_write.o \
      example_convert/example_convert.o \
      example_brep/example_brep.o \
      example_userdata/example_ud.o \
      example_userdata/example_userdata.o \
      example_roundtrip/example_roundtrip.o

EXAMPLES = example_read/example_read \
      example_write/example_write \
      example_convert/example_convert \
      example_brep/example_brep \
      example_roundtrip/example_roundtrip \
      example_userdata/example_userdata

all : $(OPENNURBS_LIB_FILE) $(EXAMPLES)

example_userdata/example_ud.o : example_userdata/example_ud.h $(ON_INC)

$(EXAMPLE_OBJ) : $(EXAMPLE_INC) $(ON_INC)

# opennurbs_freetype.h requires -I./freetype263/include
ON_OBJ_EXTRA_FLAGS = -DON_COMPILING_OPENNURBS -I./freetype263/include
$(ON_OBJ) : CFLAGS+=$(ON_OBJ_EXTRA_FLAGS)
$(ON_OBJ) : CCFLAGS+=$(ON_OBJ_EXTRA_FLAGS)
$(ON_OBJ) : $(ON_INC)

ZLIB_OBJ_EXTRA_FLAGS = -DMY_ZCALLOC -DZ_PREFIX
$(ZLIB_OBJ) : CFLAGS+=$(ZLIB_OBJ_EXTRA_FLAGS)
$(ZLIB_OBJ) : $(ZLIB_INC)

FREETYPE_OBJ_EXTRA_FLAGS = -DFT2_BUILD_LIBRARY -I./freetype263/include
$(FREETYPE_OBJ) : CFLAGS+=$(FREETYPE_OBJ_EXTRA_FLAGS)
$(FREETYPE_OBJ) : $(FREETYPE_INC)

$(OPENNURBS_LIB_FILE) : $(ON_OBJ) $(ZLIB_OBJ) $(FREETYPE_OBJ)
	-$(RM) $@
	$(AR) $@ $(ON_OBJ) $(ZLIB_OBJ)  $(FREETYPE_OBJ) 
	$(RANLIB) $@

example_read/example_read : example_read/example_read.o example_userdata/example_ud.o $(OPENNURBS_LIB_FILE)
	$(LINK) $(LINKFLAGS) example_read/example_read.o example_userdata/example_ud.o -L. -l$(OPENNURBS_LIB_NAME) -lm -o $@

example_write/example_write : example_write/example_write.o example_userdata/example_ud.o $(OPENNURBS_LIB_FILE)
	$(LINK) $(LINKFLAGS) example_write/example_write.o example_userdata/example_ud.o -L. -l$(OPENNURBS_LIB_NAME) -lm -o $@

example_convert/example_convert : example_convert/example_convert.o example_userdata/example_ud.o $(OPENNURBS_LIB_FILE)
	$(LINK) $(LINKFLAGS) example_convert/example_convert.o example_userdata/example_ud.o -L. -l$(OPENNURBS_LIB_NAME) -lm -o $@

example_brep/example_brep : example_brep/example_brep.o $(OPENNURBS_LIB_FILE)
	$(LINK) $(LINKFLAGS) example_brep/example_brep.o -L. -l$(OPENNURBS_LIB_NAME) -lm -o $@

example_userdata/example_userdata : example_userdata/example_userdata.o $(OPENNURBS_LIB_FILE)
	$(LINK) $(LINKFLAGS) example_userdata/example_userdata.o -L. -l$(OPENNURBS_LIB_NAME) -lm -o $@

example_roundtrip/example_roundtrip : example_roundtrip/example_roundtrip.o $(OPENNURBS_LIB_FILE)
	$(LINK) $(LINKFLAGS) example_roundtrip/example_roundtrip.o -L. -l$(OPENNURBS_LIB_NAME) -lm -o $@

clean :
	-$(RM) $(OPENNURBS_LIB_FILE)
	-$(RM) $(ON_OBJ)
	-$(RM) $(ZLIB_OBJ)
	-$(RM) $(EXAMPLE_OBJ)
	-$(RM) $(EXAMPLES)
