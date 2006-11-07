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
 Revision 1.1  2006/11/07 18:14:21  cignoni
 Moved from the epoch svn repository

 Revision 1.1  2006/01/20 13:03:27  cignoni
 *** empty log message ***

 *****************************************************************************/
#ifndef EXTRAIOPLUGINV3D_H
#define EXTRAIOPLUGINV3D_H

#include <QObject>
#include <QStringList>
#include <QString>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include "v3dImportDialog.h"

class EpochIO : public QObject, public MeshIOInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshIOInterface)
  
public:
	QList<Format> importFormats() const;
  QList<Format> exportFormats() const {return QList<Format>();};

  EpochIO();
  ~EpochIO();
  v3dImportDialog *epochDialog;
  QString lastFileName;
  const ActionInfo &Info(QAction *);
  const PluginInfo &Info();
  int GetExportMaskCapability(QString &) const {return 0;} 

	bool open(const QString &formatName, QString &fileName, MeshModel &m, int& mask, vcg::CallBackPos *cb=0, QWidget *parent=0);
	bool save(const QString &formatName, QString &fileName, MeshModel &m, const int& mask, vcg::CallBackPos *cb=0, QWidget *parent= 0);
};


#endif
