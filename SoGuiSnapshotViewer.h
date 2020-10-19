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

#ifndef COIN_SOWINSNAPSHOTVIEWER_H
#define COIN_SOWINSNAPSHOTVIEWER_H

#include <Inventor/Win/viewers/SoWinExaminerViewer.h>

class SoGuiSnapshotViewerP;

class SoWinSnapshotViewer : public SoWinExaminerViewer
{
	SOWIN_OBJECT_HEADER(SoWinSnapshotViewer, SoWinExaminerViewer);

	public:
		SoWinSnapshotViewer(HWND parent = NULL,
			const char * name = NULL,
			SbBool embed = TRUE);
		~SoWinSnapshotViewer(void);

		void setAntialiasingEnabled(SbBool enable);
		SbBool isAntialiasingEnabled(void) const;

		void setTransparencyEnabled(SbBool enable);
		SbBool isTransparencyEnabled(void) const;

		void setAutoCroppingEnabled(SbBool enable);
		SbBool isAutoCroppingEnabled(void) const;

		void setCropBorderSize(int size);
		int getCropBorderSize(void) const;

		void setBackgroundColor(SbColor background);
		SbColor getBackgroundColor(void) const;

		void setDefaultSnapshotFilename(const char * filename);
		const char * getDefaultSnapshotFilename(void) const;

		void showInfo(float seconds = 10);
		void hideInfo(void);

		void createSnapshot(const char * filename);

		virtual void setSceneGraph(SoNode * root);
		virtual SoNode * getSceneGraph(void);

	protected:
		virtual SbBool processSoEvent(const SoEvent * const event);

		virtual void sizeChanged(const SbVec2s & size);

	private:
		SoGuiSnapshotViewerP * pimpl;

};
#endif														// !COIN_SOWINSNAPSHOTVIEWER_H
