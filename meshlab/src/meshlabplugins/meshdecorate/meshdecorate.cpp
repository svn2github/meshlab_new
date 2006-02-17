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
Revision 1.26  2006/02/17 16:09:31  glvertex
Partial restyle in drawAxis and drawQuotedBox
A lot of optimizations

Revision 1.25  2006/02/16 17:06:54  glvertex
Solved nasty bug on choosing axis candidate

Revision 1.24  2006/02/16 12:34:35  glvertex
Fixed quoted box
Some optimizations

Revision 1.23  2006/02/16 12:01:51  alemochi
correct bug

Revision 1.22  2006/02/15 16:27:33  glvertex
- Added labels to the quoted box
- Credits

Revision 1.21  2006/02/06 22:44:02  davide_portelli
Some changes in DrawAxis in order to compile under gcc

****************************************************************************/

#include <QtGui>

#include <math.h>
#include <limits>
#include <stdlib.h>

#include "meshdecorate.h"
#include<vcg/complex/trimesh/base.h>
#include <wrap/gl/addons.h>

using namespace vcg;
const ActionInfo &ExtraMeshDecoratePlugin::Info(QAction *action)
 {
   static ActionInfo ai;

	 if(action->text() == ST(DP_SHOW_AXIS))
	 {
		 ai.Help = tr("Draws XYZ axes in world coordinates");
		 ai.ShortHelp = tr("Draws XYZ axes in world coordinates");
	 }

	 if(action->text() == ST(DP_SHOW_BOX_CORNERS))
	 {
		 ai.Help = tr("Draws object's bounding box corners");
		 ai.ShortHelp = tr("Draws object's bounding box corners");
	 }

	 if(action->text() == ST(DP_SHOW_NORMALS))
	 {
		 ai.Help = tr("Draws object vertex normals");
		 ai.ShortHelp = tr("Draws object vertex normals");
	 }

	 if(action->text() == ST(DP_SHOW_QUOTED_BOX))
	 {
		 ai.Help = tr("Draws quoted box");
		 ai.ShortHelp = tr("Draws quoted box");
	 }

   return ai;
 }

 const PluginInfo &ExtraMeshDecoratePlugin::Info()
{
   static PluginInfo ai;
   ai.Date=tr("January 2006");
	 ai.Author=tr("Paolo Cignoni &  Alessio Mochi, Davide Portelli, Daniele Vacca ");
	 ai.Version=tr("1.0");
   return ai;
 }

const QString ExtraMeshDecoratePlugin::ST(int id) const
{
  switch(id)
  {
    case DP_SHOW_NORMALS      : return QString("Show Normals");
    case DP_SHOW_BOX_CORNERS  : return QString("Show Box Corners");
    case DP_SHOW_AXIS         : return QString("Show Axis");
		case DP_SHOW_QUOTED_BOX		:	return QString("Show Quoted Box");
    default: assert(0);
  }
  return QString("error!");
}

void ExtraMeshDecoratePlugin::Decorate(QAction *a, MeshModel &m, RenderMode &/*rm*/, GLArea *gla)
{
	if(a->text() == ST(DP_SHOW_NORMALS))
	{
    glPushAttrib(GL_ENABLE_BIT );
    float LineLen = m.cm.bbox.Diag()/20.0;
    CMeshO::VertexIterator vi;
    glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_LINES);
		glColor4f(.4f,.4f,1.f,.3f);
    for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
    {
      glVertex((*vi).P());
      glVertex((*vi).P()+(*vi).N()*LineLen);
    }
    glEnd();
   glPopAttrib();
  }
  if(a->text() == ST(DP_SHOW_BOX_CORNERS))	DrawBBoxCorner(m);
  if(a->text() == ST(DP_SHOW_AXIS))					DrawAxis(m,gla);
	if(a->text() == ST(DP_SHOW_QUOTED_BOX))		DrawQuotedBox(m,gla);
}

void ExtraMeshDecoratePlugin::DrawQuotedBox(MeshModel &m,GLArea *gla)
{
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);

	// Get gl state values
	double mm[16],mp[16];
	int vp[4];
	glGetDoublev(GL_MODELVIEW_MATRIX,mm);
	glGetDoublev(GL_PROJECTION_MATRIX,mp);
	glGetIntegerv(GL_VIEWPORT,vp);

	// Mesh boundingBox
	Box3f b(m.cm.bbox);
	
	glColor(Color4b::LightGray);
	glBoxWire(b);

	glLineWidth(1.f);
	glPointSize(3.f);

	Point3d p1,p2;

	glColor(Color4b::LightRed);
	chooseX(b,mm,mp,vp,p1,p2);					// Selects x axis candidate
	glPushMatrix();
	glTranslate(b.Center());
	glScalef(1,1.1f,1.1f);
	drawQuotedLine(p1,p2,b.DimX(),calcSlope(p1,p2,b.DimX(),10,mm,mp,vp),gla);	// Draws x axis
	glPopMatrix();

	glColor(Color4b::LightGreen);
	chooseY(b,mm,mp,vp,p1,p2);					// Selects y axis candidate
	glPushMatrix();
	glTranslate(b.Center());
	glScalef(1.1f,1,1.1f);
	drawQuotedLine(p1,p2,b.DimY(),calcSlope(p1,p2,b.DimY(),10,mm,mp,vp),gla);	// Draws y axis
	glPopMatrix();


	glColor(Color4b::LightBlue);
	chooseZ(b,mm,mp,vp,p1,p2);					// Selects z axis candidate
	glPushMatrix();
	glTranslate(b.Center());
	glScalef(1.1,1.1f,1);
	drawQuotedLine(p1,p2,b.DimZ(),calcSlope(p1,p2,b.DimZ(),10,mm,mp,vp),gla);	// Draws z axis
	glPopMatrix();

	glPopAttrib();

}

void ExtraMeshDecoratePlugin::chooseX(Box3f &box,double *mm,double *mp,int *vp,Point3d &x1,Point3d &x2)
{
	float d = -std::numeric_limits<float>::max();
	Point3d c;
	// Project the bbox center
	gluProject(box.Center()[0],box.Center()[1],box.Center()[2],mm,mp,vp,&c[0],&c[1],&c[2]);
	c[2] = 0;

	Point3d out1,out2;
	Point3f in1,in2;

	for (int i=0;i<8;i+=2)
	{
		// find the furthest axis
		in1 = box.P(i);
		in2 = box.P(i+1);

		gluProject((double)in1[0],(double)in1[1],(double)in1[2],mm,mp,vp,&out1[0],&out1[1],&out1[2]);
		gluProject((double)in2[0],(double)in2[1],(double)in2[2],mm,mp,vp,&out2[0],&out2[1],&out2[2]);
		out1[2] = out2[2] = 0;

		float currDist = Distance(c,(out1+out2)*.5f);

		if(currDist > d)
		{
			d = currDist;
			x1.Import(in1);
			x2.Import(in2);
		}
	}
}


void ExtraMeshDecoratePlugin::chooseY(Box3f &box,double *mm,double *mp,int *vp,Point3d &y1,Point3d &y2)
{
	float d = -std::numeric_limits<float>::max();
	Point3d c;
	// Project the bbox center
	gluProject(box.Center()[0],box.Center()[1],box.Center()[2],mm,mp,vp,&c[0],&c[1],&c[2]);
	c[2] = 0;

	Point3d out1,out2;
	Point3f in1,in2;

	for (int i=0;i<6;++i)
	{
		if(i==2) i = 4;	// skip
		// find the furthest axis
		in1 = box.P(i);
		in2 = box.P(i+2);

		gluProject((double)in1[0],(double)in1[1],(double)in1[2],mm,mp,vp,&out1[0],&out1[1],&out1[2]);
		gluProject((double)in2[0],(double)in2[1],(double)in2[2],mm,mp,vp,&out2[0],&out2[1],&out2[2]);
		out1[2] = out2[2] = 0;

		float currDist = Distance(c,(out1+out2)*.5f);

		if(currDist > d)
		{
			d = currDist;
			y1.Import(in1);
			y2.Import(in2);
		}
	}
}

void ExtraMeshDecoratePlugin::chooseZ(Box3f &box,double *mm,double *mp,int *vp,Point3d &z1,Point3d &z2)
{
	float d = -std::numeric_limits<float>::max();
	Point3d c;
	// Project the bbox center
	gluProject(box.Center()[0],box.Center()[1],box.Center()[2],mm,mp,vp,&c[0],&c[1],&c[2]);
	c[2] = 0;

	Point3d out1,out2;
	Point3f in1,in2;

	Point3d m;

	for (int i=0;i<4;++i)
	{
		// find the furthest axis
		in1 = box.P(i);
		in2 = box.P(i+4);


		gluProject((double)in1[0],(double)in1[1],(double)in1[2],mm,mp,vp,&out1[0],&out1[1],&out1[2]);
		gluProject((double)in2[0],(double)in2[1],(double)in2[2],mm,mp,vp,&out2[0],&out2[1],&out2[2]);
		out1[2] = out2[2] = 0;

		float currDist = Distance(c,(out1+out2)*.5f);

		if(currDist > d)
		{
			d = currDist;
			z1.Import(in1);
			z2.Import(in2);
		}
	}
}

float ExtraMeshDecoratePlugin::calcSlope(Point3d &a,Point3d &b,float dim,int spacing,double *mm,double *mp,int *vp)
{
	Point3d p1,p2;

	gluProject(a[0],a[1],a[2],mm,mp,vp,&p1[0],&p1[1],&p1[2]);
	gluProject(b[0],b[1],b[2],mm,mp,vp,&p2[0],&p2[1],&p2[2]);
	p1[2]=p2[2]=0;

	float tickNum = spacing/Distance(p2,p1);// pxl spacing
	float slope = dim*tickNum;
	slope = vcg::math::Min<float>(niceRound(slope), 0.5*niceRound(2*slope));
	slope = vcg::math::Max<float>(niceRound(dim*.001f),slope); // prevent too small slope

	return slope;
}


void ExtraMeshDecoratePlugin::drawTickedLine(Point3d &a,Point3d &b,float dim,float tickDist)
{
	Point3d v(b-a);
	v = v /dim; // normalize without computing square roots and powers
	glBegin(GL_POINTS);
	float i;
	for(i=tickDist;i<dim;i+=tickDist)
		glVertex3f(a[0] + i*v[0],a[1] + i*v[1],a[2] + i*v[2]);
	glEnd();

	// Draws bigger ticks at 0 and at max size
	glPushAttrib(GL_POINT_BIT);
	glPointSize(6);
	
	glBegin(GL_POINTS);
			glVertex(a);		// Zero
			glVertex3f(a[0] + i*v[0],a[1] + i*v[1],a[2] + i*v[2]);
	glEnd();

	glPopAttrib();
}


void ExtraMeshDecoratePlugin::drawQuotedLine(Point3d &a,Point3d &b,float dim,float tickDist,GLArea *gla)
{
	glPushAttrib(GL_COLOR_BUFFER_BIT);
	glColor(Color4b::LightGray);
		glBegin(GL_LINES);
			glVertex(a); glVertex(b);
		glEnd();
	glPopAttrib();


	Point3d v(b-a);
	v = v /dim; // normalize without computing square roots and powers
	glBegin(GL_POINTS);
	float i;
	for(i=tickDist;i<dim;i+=tickDist)
		glVertex3f(a[0] + i*v[0],a[1] + i*v[1],a[2] + i*v[2]);
	glEnd();


	// Draws bigger ticks at 0 and at max size
	glPushAttrib(GL_POINT_BIT);
	glPointSize(6);
	
	glBegin(GL_POINTS);
			glVertex(a);		// Zero
			glVertex3f(a[0] + dim*v[0],a[1] + dim*v[1],a[2] + dim*v[2]);
	glEnd();

	glPopAttrib();

	int c=1;
	for(i=tickDist;i<dim;i+=tickDist)
	{
		if(!(c%2))
				gla->renderText(a[0]+i*v[0],a[1]+i*v[1],a[2]+i*v[2],tr("%1").arg(i),gla->getFont());
		++c;
	}

	// bold font at the end
	QFont qf = gla->getFont();
	qf.setBold(true);
	gla->renderText(a[0]+dim*v[0],a[1]+dim*v[1],a[2]+dim*v[2],tr("%1").arg(dim),qf);
}


float ExtraMeshDecoratePlugin::niceRound2(float Val,float base)	{return powf(base,ceil(log10(Val)/log10(base)));}
float ExtraMeshDecoratePlugin::niceRound(float val)	{return powf(10.f,ceil(log10(val)));}

void ExtraMeshDecoratePlugin::DrawBBoxCorner(MeshModel &m)
{
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.0);
	glColor(Color4b::Cyan);
	Box3f b=m.cm.bbox;
	Point3f mi=b.min;
	Point3f ma=b.max;
	Point3f d3=(b.max-b.min)/4.0;
	Point3f zz(0,0,0);
	glBegin(GL_LINES);
	glVertex3f(mi[0],mi[1],mi[2]); glVertex3f(mi[0]+d3[0],mi[1]+zz[1],mi[2]+zz[2]);
	glVertex3f(mi[0],mi[1],mi[2]); glVertex3f(mi[0]+zz[0],mi[1]+d3[1],mi[2]+zz[2]);
	glVertex3f(mi[0],mi[1],mi[2]); glVertex3f(mi[0]+zz[0],mi[1]+zz[1],mi[2]+d3[2]);

	glVertex3f(ma[0],mi[1],mi[2]); glVertex3f(ma[0]-d3[0],mi[1]+zz[1],mi[2]+zz[2]);
	glVertex3f(ma[0],mi[1],mi[2]); glVertex3f(ma[0]+zz[0],mi[1]+d3[1],mi[2]+zz[2]);
	glVertex3f(ma[0],mi[1],mi[2]); glVertex3f(ma[0]+zz[0],mi[1]+zz[1],mi[2]+d3[2]);

  glVertex3f(mi[0],ma[1],mi[2]); glVertex3f(mi[0]+d3[0],ma[1]+zz[1],mi[2]+zz[2]);
	glVertex3f(mi[0],ma[1],mi[2]); glVertex3f(mi[0]+zz[0],ma[1]-d3[1],mi[2]+zz[2]);
	glVertex3f(mi[0],ma[1],mi[2]); glVertex3f(mi[0]+zz[0],ma[1]+zz[1],mi[2]+d3[2]);

	glVertex3f(ma[0],ma[1],mi[2]); glVertex3f(ma[0]-d3[0],ma[1]+zz[1],mi[2]+zz[2]);
	glVertex3f(ma[0],ma[1],mi[2]); glVertex3f(ma[0]+zz[0],ma[1]-d3[1],mi[2]+zz[2]);
	glVertex3f(ma[0],ma[1],mi[2]); glVertex3f(ma[0]+zz[0],ma[1]+zz[1],mi[2]+d3[2]);

	glVertex3f(mi[0],mi[1],ma[2]); glVertex3f(mi[0]+d3[0],mi[1]+zz[1],ma[2]+zz[2]);
	glVertex3f(mi[0],mi[1],ma[2]); glVertex3f(mi[0]+zz[0],mi[1]+d3[1],ma[2]+zz[2]);
	glVertex3f(mi[0],mi[1],ma[2]); glVertex3f(mi[0]+zz[0],mi[1]+zz[1],ma[2]-d3[2]);

	glVertex3f(ma[0],mi[1],ma[2]); glVertex3f(ma[0]-d3[0],mi[1]+zz[1],ma[2]+zz[2]);
	glVertex3f(ma[0],mi[1],ma[2]); glVertex3f(ma[0]+zz[0],mi[1]+d3[1],ma[2]+zz[2]);
	glVertex3f(ma[0],mi[1],ma[2]); glVertex3f(ma[0]+zz[0],mi[1]+zz[1],ma[2]-d3[2]);

	glVertex3f(mi[0],ma[1],ma[2]); glVertex3f(mi[0]+d3[0],ma[1]+zz[1],ma[2]+zz[2]);
	glVertex3f(mi[0],ma[1],ma[2]); glVertex3f(mi[0]+zz[0],ma[1]-d3[1],ma[2]+zz[2]);
	glVertex3f(mi[0],ma[1],ma[2]); glVertex3f(mi[0]+zz[0],ma[1]+zz[1],ma[2]-d3[2]);

	glVertex3f(ma[0],ma[1],ma[2]); glVertex3f(ma[0]-d3[0],ma[1]+zz[1],ma[2]+zz[2]);
	glVertex3f(ma[0],ma[1],ma[2]); glVertex3f(ma[0]+zz[0],ma[1]-d3[1],ma[2]+zz[2]);
	glVertex3f(ma[0],ma[1],ma[2]); glVertex3f(ma[0]+zz[0],ma[1]+zz[1],ma[2]-d3[2]);

	glEnd();
	glPopAttrib();
}


void ExtraMeshDecoratePlugin::DrawAxis(MeshModel &m,GLArea* gla)
{
	float hw=m.cm.bbox.Diag()/2.0;
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glLineWidth(2.0);
	glPointSize(3.f);

	// Get gl state values
	double mm[16],mp[16];
	int vp[4];

	glGetDoublev(GL_MODELVIEW_MATRIX,mm);
	glGetDoublev(GL_PROJECTION_MATRIX,mp);
	glGetIntegerv(GL_VIEWPORT,vp);

	Point3d a(hw,0,0);Point3d b(0,hw,0);Point3d c(0,0,hw);

	glBegin(GL_LINES);
		glColor(Color4b::Red); 		glVertex(-a);glVertex(a);
		glColor(Color4b::Green);	glVertex(-b);glVertex(b);
		glColor(Color4b::Blue);		glVertex(-c);glVertex(c);
	glEnd();

	glColor(Color4b::White);
	drawTickedLine(-a,a,2*hw,calcSlope(-a,a,2*hw,10,mm,mp,vp));	// Draws x axis
	drawTickedLine(-b,b,2*hw,calcSlope(-b,b,2*hw,10,mm,mp,vp));	// Draws y axis
	drawTickedLine(-c,c,2*hw,calcSlope(-c,c,2*hw,10,mm,mp,vp));	// Draws z axis

	float sf = hw*0.02f; // scale factor hw / 50
	glPushMatrix();
		glTranslate(a);  glScalef(sf,sf,sf);	Add_Ons::Cone(10,3,1,true);
	glPopMatrix();
	
	glPushMatrix();
		glTranslate(b);	glRotatef(90,0,0,1); glScalef(sf,sf,sf); Add_Ons::Cone(10,3,1,true);
	glPopMatrix();
	
	glPushMatrix();
		glTranslate(c);	glRotatef(-90,0,1,0);	glScalef(sf,sf,sf);	Add_Ons::Cone(10,3,1,true);
	glPopMatrix();

	QFont f(gla->getFont());
	f.setBold(true);
	glColor(Color4b::Red);	 gla->renderText(hw+(sf*3),0,0,QString("X"),f);
	glColor(Color4b::Green); gla->renderText(0,hw+(sf*3),0,QString("Y"),f);
	glColor(Color4b::Blue);  gla->renderText(0,0,hw+(sf*3),QString("Z"),f);

	glPopAttrib();
}

Q_EXPORT_PLUGIN(ExtraMeshDecoratePlugin)
