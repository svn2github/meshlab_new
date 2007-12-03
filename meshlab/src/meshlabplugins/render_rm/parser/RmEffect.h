/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2008                                           \/)\/    *
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
Revision 1.2  2007/12/03 11:24:36  corsini
code restyling


****************************************************************************/

#ifndef __RMEFFECT_H__
#define __RMEFFECT_H__

// Qt headers
#include <QString>
#include <QList>

// Local headers
#include "RmPass.h"

class RmEffect
{

// private data members 
private:

	QString name;
	QList<RmPass> passlist;
	
// constructor
public:

		RmEffect( QString _name ) { name = _name; }

// public methods
public:

		int size() { return passlist.size(); }
		RmPass & at( int idx ) { return passlist[idx]; }
		RmPass & operator[] (int idx) {return passlist[idx]; }

		QList<RmPass> & getPassList() { return passlist; }
		
		void addPass( RmPass pass ) { passlist.append(pass); }
		QString & getName() { return name; }

		void sortPasses() {  qSort( passlist.begin(), passlist.end() ); }
};

#endif

