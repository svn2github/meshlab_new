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
Revision 1.60  2006/02/01 12:44:42  glvertex
- Disabled EDIT menu when no editing tools loaded
- Solved openig bug when running by command line

Revision 1.59  2006/01/19 11:54:15  fmazzant
cleaned up code & cleaned up history log

Revision 1.58  2006/01/19 11:21:12  fmazzant
deleted old savemaskobj & old MaskObj

Revision 1.57  2006/01/17 13:47:45  fmazzant
update interface meshio : formats -> importFormats() & exportFormts

Revision 1.56  2006/01/14 00:51:06  davide_portelli
A little change

Revision 1.55  2006/01/07 11:04:49  glvertex
Added Apply Last Filter action

****************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QWorkspace>
#include <QStringList>
#include <QColorDialog>
#include "meshmodel.h"
#include "glarea.h"

#define MAXRECENTFILES 4

class QAction;
class QActionGroup;
class QMenu;
class QScrollArea;
class QSignalMapper;
class QProgressDialog;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
   static bool QCallBack(const int pos, const char * str);
	 const QString appName() const {return tr("MeshLab v0.6"); }
  // MaskObj maskobj;

public slots:
 void open(QString fileName=QString());
   
  
private slots:

	//////////// Slot Menu File //////////////////////
	void reload();
	void openRecentFile();							
	bool saveAs();
	bool saveSnapshot();
	///////////Slot Menu Filter ////////////////////////
	void applyFilter();
	void applyLastFilter();
	/////////// Slot Menu Render /////////////////////
	void renderBbox();
	void renderPoint();
	void renderWire();
	void renderFlat();
	void renderFlatLine();
	void renderHiddenLines();
	void renderSmooth();
	void renderTexture();
	void setLight();
	void setDoubleLighting();
	void setFancyLighting();
	void setColorMode(QAction *qa);
	void applyRenderMode();
	void applyColorMode();
  void applyEditMode();
	void toggleBackFaceCulling();
	void applyDecorateMode();
	///////////Slot Menu View ////////////////////////
	void fullScreen();
	void showToolbarFile();
	void showToolbarRender();
	void showLog();
	void showInfoPane();
	void showTrackBall();
	void resetTrackBall();
	///////////Slot Menu Windows /////////////////////
	void updateWindowMenu();
	void updateMenus();
	///////////Slot Menu Preferences /////////////////
	void setCustomize();
	///////////Slot Menu Help ////////////////////////
	void about();
	void aboutPlugins();	




private:
	void createActions();
	void createMenus();
	void createToolBars();
	void loadPlugins();
	void keyPressEvent(QKeyEvent *);
	void updateRecentFileActions();				
	void setCurrentFile(const QString &fileName);			
	void addToMenu(QList<QAction *>, QMenu *menu, const char *slot);
	void LoadKnownFilters(QStringList &filters, QHash<QString, int> &allKnownFormats, int type);


	
	static QProgressDialog *qb;
	QWorkspace *workspace;
	QSignalMapper *windowMapper;
  GLArea *GLA(){return qobject_cast<GLArea *>(workspace->activeWindow()); }
	QDir pluginsDir;
	QStringList pluginFileNames;
	std::vector<MeshIOInterface*> meshIOPlugins;
	QByteArray toolbarState;								//stato delle toolbar e dockwidgets
	
	
	////////ToolBar///////////////
	QToolBar *mainToolBar;
	QToolBar *renderToolBar;


	///////// Menu ///////////////
	QMenu *fileMenu;
	QMenu *filterMenu;
	QMenu *editMenu;
	//Render Menu and SubMenu ////
	QMenu *shadersMenu;
	QMenu *renderMenu;
	QMenu *renderModeMenu;
	QMenu *lightingModeMenu;
	QMenu *colorModeMenu;
	//View Menu and SubMenu //////
	QMenu *viewMenu;
	QMenu *trackBallMenu;
	QMenu *logMenu;
	QMenu *toolBarMenu;
	//////////////////////////////
	QMenu *windowsMenu;
	QMenu *preferencesMenu; 
	QMenu *helpMenu;


	//////////// Action Menu File ///////////////////////
	QAction *openAct;
	QAction *reloadAct;
	QAction *saveAsAct;
	QAction *saveSnapshotAct;
	QAction *lastFilterAct;
	QAction *recentFileActs[MAXRECENTFILES];
	QAction *separatorAct;										
	QAction *exitAct;
	/////////// Action Menu Render /////////////////////
	QActionGroup *renderModeGroupAct;
	QAction *renderBboxAct;
	QAction *renderModePointsAct;
	QAction *renderModeWireAct;
	QAction *renderModeHiddenLinesAct;
	QAction *renderModeFlatLinesAct;
	QAction *renderModeFlatAct;
	QAction *renderModeSmoothAct;
	QAction *renderModeTextureAct;
	QAction *setDoubleLightingAct;
	QAction *setFancyLightingAct;
	QAction *setLightAct;
	QAction *backFaceCullAct;

	QActionGroup *colorModeGroupAct;
	QAction *colorModeNoneAct;
	QAction *colorModePerVertexAct;
	QAction *colorModePerFaceAct;
	///////////Action Menu View ////////////////////////
	QAction *fullScreenAct;
	QAction *showToolbarStandardAct;
	QAction *showToolbarRenderAct;
	QAction *showLogAct;
	QAction *showInfoPaneAct;
	QAction *showTrackBallAct;
	QAction *resetTrackBallAct;
	///////////Action Menu Windows /////////////////////
	QAction *windowsTileAct;
	QAction *windowsCascadeAct;
	QAction *windowsNextAct;
	QAction *closeAct;
	QAction *closeAllAct;
	///////////Action Menu Preferences /////////////////
	QAction *setCustomizeAct;
	///////////Action Menu Help ////////////////////////
	QAction *aboutAct;
	QAction *aboutPluginsAct;
	////////////////////////////////////////////////////
	QList<QAction *> TotalDecoratorsList;
	////////////////////////////////////////////////////
};

#endif
