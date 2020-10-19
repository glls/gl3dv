/*
 *  This file is part of a set of example programs for the Coin library.
 *  Copyright (C) 2000-2003 by Systems in Motion. All rights reserved.
 *
 *                   <URL:http://www.coin3d.org>
 *
 *  This sourcecode can be redistributed and/or modified under the
 *  terms of the GNU General Public License version 2 as published by
 *  the Free Software Foundation. See the file COPYING at the root
 *  directory of the distribution for more details.
 *
 *  As a special exception, all sourcecode of the demo examples can be
 *  used for any purpose for licensees of the Coin Professional
 *  Edition License, without the restrictions of the GNU GPL. See our
 *  web pages for information about how to acquire a Professional Edition
 *  License.
 *
 *  Systems in Motion, <URL:http://www.sim.no>, <mailto:support@sim.no>
 *
\**************************************************************************/
/*
 * gl3dv - 3D mesh viewer based on COIN3D examples (snapshotviewer)
 *
 * George Litos (gl@cyberpunk.gr)
 *
\**************************************************************************/

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoMaterial.h> 
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoPointSet.h>

#ifdef WIN32
#include <Inventor/Win/viewers/SoWinViewer.h>
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif														// HAVE_CONFIG_H

#ifdef HAVE_SIMVOLEON_LIBRARY
#include <VolumeViz/nodes/SoVolumeRendering.h>
#endif														// HAVE_SIMVOLEON_LIBRARY

#ifdef HAVE_SMALLCHANGE_LIBRARY
#include <SmallChange/misc/Init.h>
#endif														// HAVE_SMALLCHANGE_LIBRARY

#define MAX_NUMPOINTS 32768

//--------------------------------------------------------------------------
void
init_extra_libraries(void)
{
	#ifdef HAVE_SMALLCHANGE_LIBRARY
	// It is useful to include and init the SmallChange library, as we
	// can then automatically load iv-files which includes extension
	// nodes, engines, nodekits, etc from SmallChange.
	smallchange_init();
	#endif													// HAVE_SMALLCHANGE_LIBRARY

	#ifdef HAVE_SIMVOLEON_LIBRARY
	// Inits SimVoleon, if available, for the same reason as for
	// SmallChange above.
	SoVolumeRendering::init();
	#endif													// HAVE_SIMVOLEON_LIBRARY
}

//--------------------------------------------------------------------------
// Returns random value between 0.0f and 1.0f.
//--------------------------------------------------------------------------
static float
normalized_rand(void)
{
	return float(rand())/float(RAND_MAX);
}

//--------------------------------------------------------------------------

template <class Type>
inline Type cu_max( const Type A, const Type B )
{
	return (A < B) ? B : A;
}

//--------------------------------------------------------------------------

template <class Type>
inline Type cu_min( const Type A, const Type B )
{
	return (A < B) ? A : B;
}

//--------------------------------------------------------------------------
// pointset loader
//--------------------------------------------------------------------------
int 
point_input(SoSeparator * root, char * filename)
{
	const int NUMPOINTS = MAX_NUMPOINTS;
	float xyz[NUMPOINTS][3], rgb[NUMPOINTS][3];
	float rf,gf,bf;
	FILE *fp;
	int i;

	printf("Reading file: %s\n",filename);
	if( (fp  = fopen( filename, "rt" )) == NULL ) {
		printf( "ERROR: Opening file %s failed.\n" );
		return 0;
	}
	
	SoMaterialBinding * mb = new SoMaterialBinding;
	mb->value = SoMaterialBinding::PER_VERTEX;
	root->addChild(mb);
	
	while (!feof(fp)) {
		
		for (i=0; i < NUMPOINTS; i++) 
		{
#ifdef DEBUG
			printf("pos:%ld pointset:%d \r", ftell(fp), i);
#endif
			if(feof(fp)) break;
			//fscanf(fp, "%f %f %f %d %d %d",
			// x y z R G B
			fscanf(fp, "%f %f %f %f %f %f", &xyz[i][0], &xyz[i][1], &xyz[i][2], &rf, &gf ,&bf );
			rgb[i][0] = (float)(rf / 255.0);
			rgb[i][1] = (float)(gf / 255.0);
			rgb[i][2] = (float)(bf / 255.0);
		}

		SoMaterial * mat = new SoMaterial;
		mat->diffuseColor.setValues(0, i, rgb);
		root->addChild(mat);
    
		SoCoordinate3 * coord3 = new SoCoordinate3;
		coord3->point.setValues(0, i, xyz);
		root->addChild(coord3);
		
		SoPointSet * pointset = new SoPointSet;
		root->addChild(pointset);
	}
	
	fclose(fp);
	return 1;
}
//--------------------------------------------------------------------------

static SoNode *
shape_to_scene_graph(SoShape * shape )
{
	SoSeparator * root = new SoSeparator;
	SoShapeHints * hints = new SoShapeHints;
	hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
	hints->shapeType = SoShapeHints::SOLID;
	hints->creaseAngle = 0.91f;
	root->addChild( hints );
	SoBaseColor * basecol = new SoBaseColor;
	basecol->rgb.setValue( cu_min(1.0f, 0.3f + normalized_rand()),
		cu_min(1.0f, 0.3f + normalized_rand()),
		cu_min(1.0f, 0.3f + normalized_rand()) );
	root->addChild( basecol );

	SoScale * nonuniformscale = new SoScale;
	nonuniformscale->scaleFactor.setValue( normalized_rand() * 5.0f + 1.0f,
		normalized_rand() * 5.0f + 1.0f,
		normalized_rand() * 5.0f + 1.0f );
	root->addChild( nonuniformscale );

	root->addChild( shape );
	return (SoNode *) root;
}															// shape_to_scene_graph()

//--------------------------------------------------------------------------

SoNode 
*get_scene_graph(int argc, char ** argv )
{
	srand( (unsigned int) time(NULL) );

	if ( argc > 1 )
	{
		#if HAVE_DIME2SO
		#endif
		#if HAVE_PROFIT2SO
		if ( prf_openflight_check_file( argv[1] ) )
			return prf_openinventor_import( argv[1] );
		#endif

		SoInput in;
		//test
		//shape_to_scene_graph( new SoCone );
		//test end
		if ( ! strcmp( argv[1], "--cube" ) )
		{
			return shape_to_scene_graph( new SoCube );
		}
		else if ( ! strcmp( argv[1], "--sphere" ) )
		{
			return shape_to_scene_graph( new SoSphere );
		}
		else if ( ! strcmp( argv[1], "--cone" ) )
		{
			return shape_to_scene_graph( new SoCone );
		}
		else if ( ! strcmp( argv[1], "--cylinder" ) )
		{
			return shape_to_scene_graph( new SoCylinder );
		}
		else if ( ! strcmp( argv[1], "--text3" ) )
		{
			SoText3 * textnode = new SoText3;
			const char * s[] = { "www", "georgelitos", "com" };
			textnode->string.setValues( 0, 3, s );
			textnode->parts.setValue( SoText3::ALL );
			textnode->justification = SoText3::CENTER;
			return shape_to_scene_graph( textnode );
		}
		else if (strstr(argv[1], ".pts")) 
		{
			SoSeparator *newnode = new SoSeparator;
			point_input(newnode, argv[1]);
			return (SoNode *) newnode;
			//shape_to_scene_graph(newnode);
		}
		else if ( ! in.openFile( argv[1] ) )
		{
			return NULL;
		}
		return SoDB::readAll( &in );
	}

	/*
	int randval = rand() % 5;
	switch ( randval )
	{
		case 0:  return shape_to_scene_graph( new SoCone );
		case 1:  return shape_to_scene_graph( new SoCube );
		case 2:  return shape_to_scene_graph( new SoCylinder );
		case 3:  return shape_to_scene_graph( new SoSphere );
		default:
		*/
			SoText3 * textnode = new SoText3;
			const char * s[] = { "www", "georgelitos", "com" };
			textnode->string.setValues( 0, 3, s );
			textnode->parts.setValue( SoText3::ALL );
			textnode->justification = SoText3::CENTER;
			return shape_to_scene_graph( textnode );
		/*
	}
	*/
}															// get_scene_graph()
//--------------------------------------------------------------------------
static inline void
setStereoViewing(SoWinViewer * v, SbBool f)
{
	#ifdef HAVE_VIEWER_STEREO_VIEWING
	v->setStereoViewing(f);
	#else													// !HAVE_VIEWER_STEREO_VIEWING
	SoDebugError::postWarning("computils:config_viewer",
		"SoWinViewer::setStereoViewing() not available");
	#endif													// !HAVE_VIEWER_STEREO_VIEWING
}
//--------------------------------------------------------------------------
static inline void
setQuadBufferStereo(SoWinViewer * v, SbBool f)
{
	#ifdef HAVE_VIEWER_QUADBUFFER_STEREO
	v->setQuadBufferStereo(f);
	#else													// !HAVE_VIEWER_QUADBUFFER_STEREO
	SoDebugError::postWarning("computils:config_viewer",
		"SoWinViewer::setQuadBufferStereo() not available");
	#endif													// !HAVE_VIEWER_QUADBUFFER_STEREO
}
//--------------------------------------------------------------------------
void
config_viewer(SoWinViewer * viewer, int argc, char ** argv)
{
	int numpasses = 1;
	SbBool smoothing = FALSE;

	int i = 1;
	while (i < argc)
	{
		if (!strcmp(argv[i], "--stereo"))
		{
			setQuadBufferStereo(viewer, TRUE);
			setStereoViewing(viewer, TRUE);
			i++;
		}
		else if (!strcmp(argv[i], "--anaglyph"))
		{
			setQuadBufferStereo(viewer, FALSE);
			setStereoViewing(viewer, TRUE);
			i++;
		}
		else if (!strcmp(argv[i], "--stereooffset") && (i < argc - 1))
		{
			viewer->setStereoOffset((float) atoi(argv[i+1]));
			i += 2;
		}
		else

		if (!strcmp(argv[i], "--title" ) && (i < argc - 1))
		{
			viewer->setTitle(argv[i+1]);
			i += 2;
		}
		else if (!strcmp(argv[i], "--size") && (i < argc - 2))
		{
			viewer->setSize(SbVec2s(atoi(argv[i+1]), atoi(argv[i+2])));
			i += 3;
		}
		else if (!strcmp(argv[i], "--noheadlight"))
		{
			viewer->setHeadlight(FALSE);
			i++;
		}
		else if (!strcmp(argv[i], "--noautoclip"))
		{
			viewer->setAutoClipping(FALSE);
			i++;
		}
		else if (!strcmp(argv[i], "--smoothing"))
		{
			smoothing = TRUE;
			i++;
		}
		else if (!strcmp(argv[i], "--numpasses") && (i < argc -1))
		{
			numpasses = atoi(argv[i+1]);
			i += 2;

			if ((numpasses < 1) || (numpasses > 15))
			{
				(void)fprintf(stderr,
					"Error: \"--numpasses\" option should give a value "
					"in the range [1, 15].\n");
				exit(1);
			}
		}
		else if (!strcmp(argv[i], "--background"))
		{
			SbBool err = FALSE;
			if (argc < i+4) { err = TRUE; }
			SbColor col;
			if (!err)
			{
				char * chk;
				errno = 0;
				for (int j=0; (j < 3) && !err; j++)
				{
					const char * nptr = argv[i+1+j];
					col[j] = strtod(nptr, &chk);
					if (((col[j] == 0.0f) && (chk == nptr)) || (errno!=0) ||
						(col[j] < 0.0f) || (col[j] > 1.0f))
					{
						err = TRUE;
					}
				}
			}

			if (err)
			{
				(void)fprintf(stderr,
					"Error: \"--background\" option must be given with "
					"3 \"R G B\" color values in the range [0.0, 1.0].\n");
				exit(1);
			}

			viewer->setBackgroundColor(col);
			i += 4;
		}
		else i++;
	}

	#ifdef HAVE_SOGUIGLWIDGET_ACCUMULATIONBUFFER
	if (numpasses > 1) viewer->setAccumulationBuffer(TRUE);
	#endif													// HAVE_SOGUIGLWIDGET_ACCUMULATIONBUFFER

	if (smoothing || numpasses > 1)
	{
		viewer->setAntialiasing(smoothing, numpasses);
	}

}															// config_viewer

//--------------------------------------------------------------------------
