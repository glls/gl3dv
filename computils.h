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

#ifndef __SO_COMPUTILS_H__
#define __SO_COMPUTILS_H__

//--------------------------------------------------------------------------
class SoWinViewer;

void init_extra_libraries(void);

SoNode * get_scene_graph( int argc, char ** argv );
void config_viewer( SoWinViewer * viewer, int argc, char ** argv );

//--------------------------------------------------------------------------
#endif														// ! __SO_COMPUTILS_H__
