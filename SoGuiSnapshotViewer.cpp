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
 * iti3dv - 3D mesh viewer based on COIN3D examples (snapshotviewer)
 *
 * George Litos (gl@iti.gr)
 *
\**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <Inventor/SoOffscreenRenderer.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/sensors/SoAlarmSensor.h>
#ifdef WIN32
#include <Inventor/Win/viewers/SoWinExaminerViewer.h>
#endif

#include <SoGuiSnapshotViewer.h>

//--------------------------------------------------------------------------
#define INFO_TIME	6
#define STATUS_LINE 10
#define SNAPSHOT_JPG "snapshot%04d.jpg"
#define SNAPSHOT_EPS "snapshot%04d.eps"
#define SNAPSHOT_PNG "snapshot%04d.png"
#define FRAME_FILENAME "frame%04d.png"
//--------------------------------------------------------------------------

class SoGuiSnapshotViewerP
{
	public:
		SoGuiSnapshotViewerP(SoWinSnapshotViewer * viewer);
		~SoGuiSnapshotViewerP(void);

		static void snoopCB(void * userdata, SoAction * action);
		static void hideCB(void * userdata, SoSensor * sensor);

		SbBool antialiasing;
		SbBool transparency;
		SbBool autocropping;
		SbBool save_sequence;
		int cropborder;

		const char * defaultfilename;
		SbVec2s viewport;

		SoNode * realroot;

		SoRotationXYZ *myXRotXYZ, *myYRotXYZ, *myZRotXYZ;
		double rot_angle;
		int imageNo;

		SoAlarmSensor * alarm;
		SoSwitch * inforoot;
		SoText2 * infotext;
		SoTranslation * infotrans;

		SoWinSnapshotViewer * api;
};
//--------------------------------------------------------------------------
SoGuiSnapshotViewerP::SoGuiSnapshotViewerP(SoWinSnapshotViewer * viewer)
{
	this->api = viewer;

	this->antialiasing = FALSE;
	this->transparency = TRUE;
	this->autocropping = FALSE;
	this->cropborder = 0;
	this->defaultfilename = strdup(SNAPSHOT_JPG);		//was png
	this->viewport = SbVec2s(0, 0);
	this->realroot = NULL;

	this->rot_angle = 0.0;
	this->imageNo = 1;
	this->save_sequence = FALSE;

	this->alarm = new SoAlarmSensor(SoGuiSnapshotViewerP::hideCB, this);
	this->infotext = new SoText2;
	this->infotext->string.set1Value(0, "");

	SoOrthographicCamera * cam = new SoOrthographicCamera;
	cam->height.setValue(1.0);
	cam->nearDistance.setValue(0.0);
	cam->farDistance.setValue(1.0);

	this->infotrans = new SoTranslation;
	this->infotrans->translation.setValue(SbVec3f(0,0,-0.1));
	SoBaseColor * color = new SoBaseColor;
	color->rgb.setValue(SbColor(1.0, 0.0, 0.0));

	// add rotation engines
	this->myXRotXYZ = new SoRotationXYZ;
	this->myXRotXYZ->axis = SoRotationXYZ::X;// rotate about X axis
	this->myYRotXYZ = new SoRotationXYZ;
	this->myYRotXYZ->axis = SoRotationXYZ::Y;// rotate about Y axis
	this->myZRotXYZ = new SoRotationXYZ;
	this->myZRotXYZ->axis = SoRotationXYZ::Z;// rotate about Z axis

	this->inforoot = new SoSwitch;
	this->inforoot->whichChild.setValue(SO_SWITCH_ALL);
	this->inforoot->addChild(cam);
	this->inforoot->addChild(color);
	this->inforoot->addChild(this->infotrans);
	this->inforoot->addChild(this->infotext);
	this->inforoot->ref();
}
//--------------------------------------------------------------------------
SoGuiSnapshotViewerP::~SoGuiSnapshotViewerP(void)
{
	if ( this->defaultfilename ) free((void *)this->defaultfilename);
	if ( this->realroot ) this->realroot->unref();
	if ( this->alarm->isScheduled() ) this->alarm->unschedule();
	delete this->alarm;
	this->inforoot->unref();
}
//--------------------------------------------------------------------------
void
SoGuiSnapshotViewerP::snoopCB(void * userdata, SoAction * action)
{
	SoGuiSnapshotViewerP * pimpl = (SoGuiSnapshotViewerP *) userdata;
	if ( action->getTypeId() != SoGLRenderAction::getClassTypeId() )
		return;
	if ( action->getWhatAppliedTo() != SoAction::NODE )
		return;
	if ( pimpl->realroot ) pimpl->realroot->unref();
	pimpl->realroot = action->getNodeAppliedTo();
	pimpl->realroot->ref();
}
//--------------------------------------------------------------------------
void
SoGuiSnapshotViewerP::hideCB(void * userdata, SoSensor * sensor)
{
	SoGuiSnapshotViewerP * pimpl = (SoGuiSnapshotViewerP *) userdata;
	pimpl->inforoot->whichChild.setValue(SO_SWITCH_NONE);
	pimpl->infotext->string.set1Value(STATUS_LINE, "");
	pimpl->api->scheduleRedraw();
	if ( sensor->isScheduled() ) sensor->unschedule();
}

//--------------------------------------------------------------------------
// SoWinSnapshotViewer
//--------------------------------------------------------------------------

/*	GL :
 *	THISP->realroot is the root, load new file there 
*/
#define THISP (this->pimpl)

//--------------------------------------------------------------------------
SOWIN_OBJECT_SOURCE(SoWinSnapshotViewer);

//--------------------------------------------------------------------------

SoWinSnapshotViewer::SoWinSnapshotViewer(HWND parent,
const char * name,
SbBool embed)
: SoWinExaminerViewer(parent, name, embed)
{
	THISP = new SoGuiSnapshotViewerP(this);
}
//--------------------------------------------------------------------------
SoWinSnapshotViewer::~SoWinSnapshotViewer(void)
{
	delete THISP;
}
//--------------------------------------------------------------------------
void
SoWinSnapshotViewer::setAntialiasingEnabled(SbBool enable)
{
	THISP->antialiasing = enable;
}
//--------------------------------------------------------------------------
SbBool
SoWinSnapshotViewer::isAntialiasingEnabled(void) const
{
	return THISP->antialiasing;
}
//--------------------------------------------------------------------------
void
SoWinSnapshotViewer::setTransparencyEnabled(SbBool enable)
{
	THISP->transparency = enable;
}
//--------------------------------------------------------------------------
SbBool
SoWinSnapshotViewer::isTransparencyEnabled(void) const
{
	return THISP->transparency;
}
//--------------------------------------------------------------------------
void
SoWinSnapshotViewer::setAutoCroppingEnabled(SbBool enable)
{
	THISP->autocropping = enable;
}
//--------------------------------------------------------------------------
SbBool
SoWinSnapshotViewer::isAutoCroppingEnabled(void) const
{
	return THISP->autocropping;
}
//--------------------------------------------------------------------------
void
SoWinSnapshotViewer::setCropBorderSize(int size)
{
	if ( size < 0 ) size = 0;
	THISP->cropborder = size;
}
//--------------------------------------------------------------------------
int
SoWinSnapshotViewer::getCropBorderSize(void) const
{
	return THISP->cropborder;
}
//--------------------------------------------------------------------------
void
SoWinSnapshotViewer::setDefaultSnapshotFilename(const char * filename)
{
	if ( THISP->defaultfilename )
	{
		free((void *)THISP->defaultfilename);
		THISP->defaultfilename = NULL;
	}
	if ( filename )
		THISP->defaultfilename = strdup(filename);
}
//--------------------------------------------------------------------------
const char *
SoWinSnapshotViewer::getDefaultSnapshotFilename(void) const
{
	return THISP->defaultfilename;
}
//--------------------------------------------------------------------------
void
SoWinSnapshotViewer::showInfo(float seconds)
{
	// Information panel
	SbString string;
	string.sprintf("d[i]splay OSD");
	THISP->infotext->string.set1Value(1, string);
	string.sprintf("[s]ave JPEG snapshot");
	THISP->infotext->string.set1Value(2, string);
	string.sprintf("save [P]NG, [E]PS snapshot");
	THISP->infotext->string.set1Value(3, string);
	string.sprintf("[a]ntialiasing    : %s", this->isAntialiasingEnabled() ? "on" : "off");
	THISP->infotext->string.set1Value(4, string);
	string.sprintf("[t]ransparency    : %s", this->isTransparencyEnabled() ? "on" : "off");
	THISP->infotext->string.set1Value(5, string);
	string.sprintf("auto[c]ropping    : %s", this->isAutoCroppingEnabled() ? "on" : "off");
	THISP->infotext->string.set1Value(6, string);
	string.sprintf("crop border [b/B] : %d pixels", this->getCropBorderSize());
	THISP->infotext->string.set1Value(7, string);
	string.sprintf("viewport size     : %dx%d", THISP->viewport[0], THISP->viewport[1]);
	THISP->infotext->string.set1Value(8, string);
	string.sprintf("[x|y|z] rotation axis");
	THISP->infotext->string.set1Value(9, string);
	string.sprintf("save i[m]age sequence : %s", THISP->save_sequence ? "on" : "off");
	THISP->infotext->string.set1Value(10, string);

	if ( THISP->alarm->isScheduled() ) THISP->alarm->unschedule();
	THISP->alarm->setTime(SbTime::getTimeOfDay() + seconds);
	THISP->alarm->schedule();

	THISP->inforoot->whichChild.setValue(SO_SWITCH_ALL);
	this->scheduleRedraw();
}
//--------------------------------------------------------------------------
void
SoWinSnapshotViewer::hideInfo(void)
{
	THISP->inforoot->whichChild.setValue(SO_SWITCH_NONE);
	THISP->infotext->string.set1Value(STATUS_LINE, "");
	this->scheduleRedraw();
	if ( THISP->alarm->isScheduled() ) THISP->alarm->unschedule();
}
//--------------------------------------------------------------------------
void
SoWinSnapshotViewer::createSnapshot(const char * filename)
{
	SoOffscreenRenderer * renderer = new SoOffscreenRenderer(this->getGLSize());

	if ( THISP->transparency )
	{
		// FIXME: why does transparency work at home, and not at work?
		renderer->setComponents(SoOffscreenRenderer::RGB_TRANSPARENCY);
		// FIXME: when using transparency - maybe set white background?  this is
		// only an issue if it affects the antialiasing along the object edges,
		// and/or if the background color stains transparent surfaces...
		renderer->setBackgroundColor(this->getSceneManager()->getBackgroundColor());
	}
	else
	{
		renderer->setComponents(SoOffscreenRenderer::RGB);
		renderer->setBackgroundColor(this->getSceneManager()->getBackgroundColor());
	}

	if ( THISP->antialiasing )
	{
		renderer->getGLRenderAction()->setNumPasses(9);
	}

	renderer->render(THISP->realroot);

	int left = 0;
	int right = THISP->viewport[0] - 1;
	int top = 0;
	int bottom = THISP->viewport[1] - 1;
	if ( THISP->autocropping )
	{
		const int components = renderer->getComponents();
		unsigned char * buffer = renderer->getBuffer();
		SbColor background = renderer->getBackgroundColor();
		SbVec4f bg = SbVec4f(background[0], background[1], background[2], 255);
		if ( THISP->transparency ) bg[3] = 0;
		SbBool loop = TRUE;
		// note that the border indices come out off-by-one when breaking the loop
		for ( ; loop && (left < THISP->viewport[0]); left++ )
			for ( int i = 0; loop && (i < THISP->viewport[1]); i++ )
				for ( int c = 0; loop && (c < components); c++ )
					if ( buffer[(i*components*THISP->viewport[0])+(left*components)+c] != bg[c] )
						loop = FALSE;
		loop = TRUE;
		for ( ; loop && (right >= left); right-- )
			for ( int i = 0; loop && (i < THISP->viewport[1]); i++ )
				for ( int c = 0; loop && (c < components); c++ )
					if ( buffer[(i*components*THISP->viewport[0])+(right*components)+c] != bg[c] )
						loop = FALSE;
		loop = TRUE;
		for ( ; loop && (top < THISP->viewport[1]); top++ )
			for ( int i = 0; loop && (i < THISP->viewport[0]); i++ )
				for ( int c = 0; loop && (c < components); c++ )
					if ( buffer[(top*components*THISP->viewport[0])+(i*components)+c] != bg[c] )
						loop = FALSE;
		loop = TRUE;
		for ( ; loop && (bottom >= top); bottom-- )
			for ( int i = 0; loop && (i < THISP->viewport[0]); i++ )
				for ( int c = 0; loop && (c < components); c++ )
					if ( buffer[(bottom*components*THISP->viewport[0])+(i*components)+c] != bg[c] )
						loop = FALSE;
		left -= THISP->cropborder + 1;
		right += THISP->cropborder + 1;
		top -= THISP->cropborder + 1;
		bottom += THISP->cropborder + 1;

		// cropping support is a hack, but it works
		SoOffscreenRenderer * cropped = new SoOffscreenRenderer(SbViewportRegion(right-left+1, bottom-top+1));
		if ( !THISP->transparency )
		{
			cropped->setComponents(SoOffscreenRenderer::RGB);
		}
		else
		{
			cropped->setComponents(SoOffscreenRenderer::RGB_TRANSPARENCY);
		}
		// FIXME: figure out what to do with the background when crop-border extends image size
		cropped->setBackgroundColor(SbColor(bg[0], bg[1], bg[2]));
		do
		{
			SoSeparator * empty = new SoSeparator();
			empty->ref();
			cropped->render(empty);
			empty->unref();
		} while ( FALSE );
		unsigned char * newbuf = cropped->getBuffer();
		for ( int y = top; y <= bottom; y++ )
		{
			if ( (y >= 0) && (y < THISP->viewport[1]) )
			{
				for ( int x = left; x <= right; x++ )
				{
					if ( (x >= 0) && (x < THISP->viewport[0]) )
					{
						for ( int a = 0; a < components; a++ )
						{
							newbuf[((y-top)*components*(right-left+1))+((x-left)*components)+a] =
								buffer[(y*components*THISP->viewport[0])+(x*components)+a];
						}
					}
				}
			}
		}
		delete renderer;
		renderer = cropped;
	}

	const char * format = strrchr(filename, '.');
	if ( format ) format++;
	else format = "rgb";
	if ( !renderer->isWriteSupported(format) )
	{
		SoDebugError::postWarning("SoWinSnapshotViewer::createSnapshot",
			"\"%s\" is not a supported format - writing as rgb\n", format);
		format = "rgb";
	}
	if ( !renderer->writeToFile(filename, format) )
	{
		SoDebugError::post("SoWinSnapshotViewer::createSnapshot",
			"error writing snapshot with format %s\n", format);
		THISP->infotext->string.set1Value(STATUS_LINE, "SAVE ERROR");
	}
	else
	{
		SbString status;
		status.sprintf("SAVE OK (%dx%d%+d%+d)", right - left + 1, bottom - top + 1, left, top);
		THISP->infotext->string.set1Value(STATUS_LINE, status);
	}
	delete renderer;
	this->showInfo(INFO_TIME);
}
//--------------------------------------------------------------------------

void
SoWinSnapshotViewer::setSceneGraph(SoNode * root)
{
	if ( THISP->realroot != NULL )
	{
		THISP->realroot->unref();
		THISP->realroot = NULL;
	}
	SoGroup * myroot = new SoGroup;
	SoCallback * snoop = new SoCallback;

	snoop->setCallback(SoGuiSnapshotViewerP::snoopCB, THISP);
	myroot->addChild(snoop);
	// GL: insert rotation engines
	myroot->addChild(THISP->myXRotXYZ);
	myroot->addChild(THISP->myYRotXYZ);
	myroot->addChild(THISP->myZRotXYZ);
	//
	myroot->addChild(root);
	SoWinExaminerViewer::setSceneGraph(myroot);

	this->addSuperimposition(THISP->inforoot);
	this->setSuperimpositionEnabled(THISP->inforoot, TRUE);
}
//--------------------------------------------------------------------------

SoNode *
SoWinSnapshotViewer::getSceneGraph(void)
{
	SoGroup * myroot = (SoGroup *) SoWinExaminerViewer::getSceneGraph();
	return myroot->getChild(1);								// the user root
}
//--------------------------------------------------------------------------

SbBool
SoWinSnapshotViewer::processSoEvent(const SoEvent * const event)
{
	char fname[MAX_PATH];

	if (THISP->imageNo > 9999)
		THISP->imageNo = 1;

	if (													// this->isViewing() &&
		event->getTypeId() == SoKeyboardEvent::getClassTypeId() )
	{
		SoKeyboardEvent * kbevent = (SoKeyboardEvent *) event;
		switch ( kbevent->getKey() )
		{
			case SoKeyboardEvent::A:						// 'a' for antialiasing
				if ( kbevent->getState() == SoButtonEvent::DOWN )
				{
					this->setAntialiasingEnabled(this->isAntialiasingEnabled() ? FALSE : TRUE);
					this->showInfo(INFO_TIME);
				}
				return TRUE;
			case SoKeyboardEvent::M:						// 'm' jpg image seq toggle
				if ( kbevent->getState() == SoButtonEvent::DOWN )
				{
					//this->setAntialiasingEnabled(this->isAntialiasingEnabled() ? FALSE : TRUE);
					THISP->save_sequence = THISP->save_sequence ? FALSE : TRUE;
					this->showInfo(INFO_TIME);
				}
				return TRUE;
			case SoKeyboardEvent::X:
			case SoKeyboardEvent::Y:
			case SoKeyboardEvent::Z:
				if ( kbevent->getState() == SoButtonEvent::DOWN )
				{
					double rangle;
					int frame=1;
					for(rangle = 0.0; rangle < M_TWOPI; rangle += (M_TWOPI / 360.0))
					{
						// rotate
						THISP->rot_angle = rangle;
						if (kbevent->getKey() == SoKeyboardEvent::Y) 
							THISP->myYRotXYZ->angle = (float) pimpl->rot_angle;
						else if (kbevent->getKey() == SoKeyboardEvent::X)
							THISP->myXRotXYZ->angle = (float) pimpl->rot_angle;
						else if (kbevent->getKey() == SoKeyboardEvent::Z)
							THISP->myZRotXYZ->angle = (float) pimpl->rot_angle;

						this->redraw();

						// save picture (default dir)
						if(THISP->save_sequence)
						{
							sprintf(fname, FRAME_FILENAME, frame++);
							printf("%s\r", fname);
							this->createSnapshot(fname);
						}
					}

					this->showInfo(INFO_TIME);
				}
				return TRUE;
/*
			case SoKeyboardEvent::R:						// 'r' rotate in X axis
				if ( kbevent->getState() == SoButtonEvent::DOWN )
				{
					THISP->rot_angle += (M_PI/360);
					THISP->myXRotXYZ->angle = pimpl->rot_angle;
					this->showInfo(INFO_TIME);
				}
				return TRUE;
*/
			case SoKeyboardEvent::T:						// 't' for transparency
				if ( kbevent->getState() == SoButtonEvent::DOWN )
				{
					this->setTransparencyEnabled(this->isTransparencyEnabled() ? FALSE : TRUE);
					this->showInfo(INFO_TIME);
				}
				return TRUE;
			case SoKeyboardEvent::C:						// 'c' for cropping
				if ( kbevent->getState() == SoButtonEvent::DOWN )
				{
					this->setAutoCroppingEnabled(this->isAutoCroppingEnabled() ? FALSE : TRUE);
					this->showInfo(INFO_TIME);
				}
				return TRUE;
			case SoKeyboardEvent::B:						// 'b' for border
				if ( kbevent->getState() == SoButtonEvent::DOWN )
				{
					if ( !kbevent->wasShiftDown() )
					{
						this->setCropBorderSize(this->getCropBorderSize() + 1);
					}
					else
					{
						this->setCropBorderSize(this->getCropBorderSize() - 1);
					}
					this->showInfo(INFO_TIME);
				}
				return TRUE;
			case SoKeyboardEvent::I:						// 'i' for info
				if ( kbevent->getState() == SoButtonEvent::DOWN )
					this->showInfo(10);
				// else if ( kbevent->getState() == SoButtonEvent::UP )
				//  this->hideInfo();
				return TRUE;
			case SoKeyboardEvent::S:						// 's' for snapshot
				if ( kbevent->getState() == SoButtonEvent::DOWN )
				{
					sprintf(fname, SNAPSHOT_JPG, THISP->imageNo++);
					//this->getDefaultSnapshotFilename()
					this->createSnapshot(fname);
					this->showInfo(INFO_TIME);
					//
					printf("%s saved.\n", fname);
				}
				return TRUE;
			case SoKeyboardEvent::P:						// 'p' for snapshot png
				if ( kbevent->getState() == SoButtonEvent::DOWN )
				{
					sprintf(fname, SNAPSHOT_PNG, THISP->imageNo++);
					this->createSnapshot(fname);
					this->showInfo(INFO_TIME);
					//
					printf("%s saved.\n", fname);
				}
				return TRUE;
			case SoKeyboardEvent::E:						// 'e' for eps-snapshot
				if ( kbevent->getState() == SoButtonEvent::DOWN )
				{
					sprintf(fname, SNAPSHOT_EPS, THISP->imageNo++);
					this->createSnapshot(fname);
					this->showInfo(INFO_TIME);
					printf("%s saved.\n", fname);
				}
				return TRUE;
			default:
				break;
		}
	}
	return SoWinExaminerViewer::processSoEvent(event);
}
//--------------------------------------------------------------------------
void
SoWinSnapshotViewer::sizeChanged(const SbVec2s & size)
{
	SoWinExaminerViewer::sizeChanged(size);
	THISP->viewport = this->getGLSize();
	double aspect = float(THISP->viewport[0]) / float(THISP->viewport[1]);
	double x = 0, y = 0;
	if ( aspect < 1.0 )
	{
		x = -0.5;
		y = 0.5 * 1.0/aspect;
	}
	else
	{
		x = -0.5 * aspect;
		y = 0.5;
	}
	// need air - adjust inwards by four pixels
	x -= ((2.0 * x) / THISP->viewport[0]) * 4;
	y -= ((2.0 * y) / THISP->viewport[1]) * 4;
	THISP->infotrans->translation.setValue(SbVec3f(x, y, 0.1));
	this->showInfo(INFO_TIME);
}
//--------------------------------------------------------------------------
