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

#ifndef EXTRADECORATEPLUGIN_H
#define EXTRADECORATEPLUGIN_H

#include <common/interfaces.h>
#include <wrap/gui/coordinateframe.h>
#include "colorhistogram.h"
typedef vcg::ColorHistogram<float>  CHist;


class ExtraMeshDecoratePlugin : public QObject, public MeshDecorateInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshDecorateInterface)
  virtual QString filterInfo(QAction *) const;
  QString decorateInfo(QAction *) const;
  
  enum {
    DP_SHOW_FACE_NORMALS,
    DP_SHOW_VERT_NORMALS,
    DP_SHOW_VERT,
    DP_SHOW_EDGE,
    DP_SHOW_NON_FAUX_EDGE,
		DP_SHOW_VERT_PRINC_CURV_DIR,
    DP_SHOW_BOX_CORNERS,
    DP_SHOW_BOX_CORNERS_ABS,
    DP_SHOW_AXIS,
		DP_SHOW_QUOTED_BOX,
		DP_SHOW_VERT_LABEL, 
    DP_SHOW_VERT_QUALITY_HISTOGRAM,
    DP_SHOW_FACE_QUALITY_HISTOGRAM,
		DP_SHOW_FACE_LABEL,
		DP_SHOW_CAMERA,
    DP_SHOW_TEXPARAM,
  };

  QString filterName(FilterIDType filter) const;

private:
	float niceRound2(float value,float base);
	float niceRound(float value);

  void	drawQuotedLine(const vcg::Point3d &a,const vcg::Point3d &b,float aVal, float bVal,float tickDist,QPainter *painter, QFont qf,float angle =0,bool rightAlign=false);

	void	chooseX(vcg::Box3f &box,double *modelview,double *projection,GLint *viewport,vcg::Point3d &x1,vcg::Point3d &x2);
	void	chooseY(vcg::Box3f &box,double *modelview,double *projection,GLint *viewport,vcg::Point3d &y1,vcg::Point3d &y2);
	void	chooseZ(vcg::Box3f &box,double *modelview,double *projection,GLint *viewport,vcg::Point3d &z1,vcg::Point3d &z2);
  void drawHistogram(QGLWidget *gla, CHist &ch);
public:
     
	ExtraMeshDecoratePlugin()
	{
    typeList << 
    DP_SHOW_VERT <<
    DP_SHOW_NON_FAUX_EDGE <<
    DP_SHOW_FACE_NORMALS <<
    DP_SHOW_VERT_NORMALS <<
    DP_SHOW_VERT_QUALITY_HISTOGRAM <<
    DP_SHOW_FACE_QUALITY_HISTOGRAM <<
    DP_SHOW_VERT_PRINC_CURV_DIR <<
    DP_SHOW_BOX_CORNERS <<
    DP_SHOW_BOX_CORNERS_ABS <<
    DP_SHOW_AXIS <<
    DP_SHOW_QUOTED_BOX <<
		DP_SHOW_VERT_LABEL << 
		DP_SHOW_FACE_LABEL << 
        DP_SHOW_CAMERA <<
        DP_SHOW_TEXPARAM;

    FilterIDType tt;
    foreach(tt , types()){
          actionList << new QAction(filterName(tt), this);
    }
    QAction *ap;
    foreach(ap,actionList){
        ap->setCheckable(true);
    }
  }

	void DrawBBoxCorner(MeshModel &m, bool absBBoxFlag=true);
  void DrawQuotedBox(MeshModel &m,QPainter *gla, QFont qf);
  void DrawVertLabel(MeshModel &m, QPainter *gla, QFont qf);
  void DrawFaceLabel(MeshModel &m, QPainter *gla, QFont qf);
  void DrawCamera(MeshModel &m, QPainter *painter, QFont qf);
  void DrawTexParam(MeshModel &m, GLArea *gla, QPainter *painter, RichParameterSet *, QFont qf);
  void DrawColorHistogram(CHist &ch, GLArea *gla, QPainter *painter, RichParameterSet *, QFont qf);

  virtual void decorate(QAction *a, MeshDocument &md, RichParameterSet *, GLArea *gla, QPainter *painter);
  virtual bool startDecorate(QAction * /*mode*/, MeshDocument &/*m*/, RichParameterSet *, GLArea * /*parent*/);
  bool isDecorationApplicable(QAction *action, const MeshModel& m, QString &ErrorMessage) const;

  void initGlobalParameterSet(QAction *, RichParameterSet &/*globalparam*/);
  inline QString CameraInfoParam() const     { return  "MeshLab::Decoration::CameraInfo" ; }
  inline QString TextureStyleParam() const   { return  "MeshLab::Decoration::TextureStyle" ; }
  inline QString VertDotSizeParam() const    { return  "MeshLab::Decoration::VertDotSize" ; }
  inline QString HistBinNumParam() const     { return  "MeshLab::Decoration::HistBinNumParam" ; }
  inline QString UseFixedHistParam() const   { return  "MeshLab::Decoration::UseFixedHistParam" ; }
  inline QString FixedHistMinParam() const   { return  "MeshLab::Decoration::FixedHistMinParam" ; }
  inline QString FixedHistMaxParam() const   { return  "MeshLab::Decoration::FixedHistMaxParam" ; }
  inline QString FixedHistWidthParam() const { return  "MeshLab::Decoration::FixedHistWidthParam" ; }
  inline QString AreaHistParam() const { return  "MeshLab::Decoration::AreaHistParam" ; }
  inline QString NormalLength() const { return  "MeshLab::Decoration::NormalLength" ; }

  bool textureWireParam;
};

#endif
