/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
/****************************************************************************
  History
$Log$
Revision 1.5  2005/12/05 18:10:05  ggangemi
Added isSupported method definition

Revision 1.4  2005/12/03 22:49:46  cignoni
Added copyright info

****************************************************************************/

#ifndef DUMMYPLUGIN_H
#define DUMMYPLUGIN_H

#include <QObject>
#include <QAction>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <meshlab/glarea.h>


class DummyPlugin : public QObject, public MeshRenderInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshRenderInterface)

	QList <QAction *> actionList;
  
public:
	DummyPlugin()
	{
		actionList << new QAction(QString("action 1"),this);
		actionList << new QAction(QString("action 2"),this);
		actionList << new QAction(QString("action 3"),this);
	}

	virtual bool isSupported() {return false;}
	QList<QAction *> actions () const {return actionList;}

	void Render(QAction *a, MeshModel &m, RenderMode &rm, GLArea *gla);
};

#endif