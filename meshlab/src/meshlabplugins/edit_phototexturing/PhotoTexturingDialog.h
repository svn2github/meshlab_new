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

#ifndef PHOTOTEXTURINGDIALOG_H_
#define PHOTOTEXTURINGDIALOG_H_

#include <QDialog>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <wrap/gl/trimesh.h>
//#include <photoTexturing.h>
#include <src/PhotoTexturer.h>
#include "ui_photoTexturingDialog.h"

class PhotoTexturingDialog: public QDialog, Ui::photoTexturingDialog{

Q_OBJECT

private:
	MeshModel *mesh;
	GLArea *glarea;
	Ui::photoTexturingDialog ui;
	MeshEditInterface *ptPlugin;
	PhotoTexturer *photoTexturer;

	signals:
		void updateGLAreaTextures();
		void setGLAreaDrawMode(vcg::GLW::DrawMode mode);
		void setGLAreaColorMode(vcg::GLW::ColorMode mode);
		void setGLAreaTextureMode(vcg::GLW::TextureMode mode);

		void updateMainWindowMenus();

private slots:
	//void browseCalibrationFile();
	void loadConfigurationFile();
	void saveConfigurationFile();
	void exportCamerasToMaxScript();
	void addCamera();
	void removeCamera();

	void update();

	void assignImage();
	void calculateTextures();
	void selectCurrentTexture();
	void combineTextures();
	void apply();
	void close();
	void cancel();

public:
	PhotoTexturingDialog(MeshEditInterface*, PhotoTexturer*,MeshModel &m, GLArea *gla);
	~PhotoTexturingDialog();

};

#endif /*PHOTOTEXTURINGDIALOG_H_*/
