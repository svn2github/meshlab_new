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
Revision 1.11  2006/02/15 01:53:24  glvertex
Debugged drawing method

Revision 1.10  2006/01/25 15:37:15  glvertex
Added variable vertical spacing

Revision 1.9  2006/01/17 16:35:27  glvertex
Added Scalable fonts

Revision 1.8  2006/01/07 12:07:16  glvertex
Set default font

Revision 1.7  2006/01/02 19:13:57  glvertex
Multi level logging

Revision 1.6  2005/12/22 20:05:09  glvertex
Fixed starting position

****************************************************************************/

#include <qfont.h>
#include <stdio.h>

#include "GLLogStream.h"

using namespace std;

void GLLogStream::print(QStringList &l)
{
	list<pair <int,string> > ::iterator li;
	
	for(li=S.begin();li!=S.end();++li)
		l << (*li).second.c_str();
}

void GLLogStream::Log(int Level, const char * f, ... )
{
	char buf[4096];
	va_list marker;
	va_start( marker, f );     

	vsprintf(buf,f,marker);
	va_end( marker );              

	S.push_back(make_pair<int,string>(Level,buf));
}

void GLLogStream::Save(int Level, const char * filename )
{
	FILE *fp=fopen(filename,"wb");
	list<pair <int,string> > ::iterator li;
	for(li=S.begin();li!=S.end();++li)
		fprintf(fp,(*li).second.c_str());
}


void  GLLogStream::glDraw(QGLWidget *qgl, int Level, int nlines,float vSpacing,QFont font)
{
	list<pair <int,string> > ::iterator li;
	li=S.end();

	advance(li,-nlines); 

	if(li==S.end())
		li=S.begin();

	int StartLine = qgl->height() - nlines * vSpacing;

	for(;li!=S.end();++li)
	{
		if(Level == -1 || (*li).first == Level)
			qgl->renderText(20,StartLine+=vSpacing,(*li).second.c_str(),font);
	}
}
