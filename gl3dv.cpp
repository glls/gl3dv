/*
 * gl3dv - 3D mesh viewer based on COIN3D examples (snapshotviewer)
 *
 * George Litos (gl@cyberpunk.gr)
 *
\**************************************************************************/

#include <Inventor/Win/SoWin.h>
#include <SoGuiSnapshotViewer.h>

#include "computils.h"

#define VERSION 1.2

//--------------------------------------------------------------------------
int
main(int argc, char ** argv)
{
	HWND w = SoWin::init(argv[0]);

	char version[255];
	sprintf(version, "gl3dv - 3D mesh viewer version %2.1f", VERSION);
	printf("%s\n", version);
	printf("by George Litos (gl@cyberpunk.gr)\n\n");
	printf("usage: gl3dv [filename] [options]\n");
	printf("options:\n\t--stereo\t--background R G B\n");
	printf("\t--size W H\t--numpasses N\n");
	printf("\t--anaglyph\t--stereooffset N\n");
	printf("filename:\n\tVRML file (wrl)\n\tPTS pointset file (pts)\n\tInventor file (iv)\n");

	init_extra_libraries();

	SoNode * root = get_scene_graph( argc, argv );
	if ( ! root ) return -1;

	SoWinSnapshotViewer * viewer = new SoWinSnapshotViewer(w);
	config_viewer(viewer, argc, argv);

	SoWin::show(w);
	viewer->show();

	viewer->setSceneGraph(root);
	viewer->setTitle(version);

	SoWin::mainLoop();

	delete viewer;
	return 0;
}
//--------------------------------------------------------------------------
