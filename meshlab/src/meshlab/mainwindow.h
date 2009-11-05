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
Revision 1.102  2008/04/22 09:07:09  cignoni
1.2.0b version string

Revision 1.101  2008/04/18 17:39:34  cignoni
added menus for the new filter classes (smoothing, normal, quality)

Revision 1.100  2008/03/22 08:05:14  cignoni
Version 1.1.1

Revision 1.99  2008/02/02 15:56:38  cignoni
Version 1.1.1b

Revision 1.98  2008/01/30 11:19:47  cignoni
Version 1.1.0

Revision 1.97  2008/01/28 08:55:01  cignoni
added delete layer functionality

Revision 1.96  2008/01/16 01:39:54  cignoni
added two slot for better managing the exit from editing tools

Revision 1.95  2008/01/04 00:46:28  cignoni
Changed the decoration framework. Now it accept a, global, parameter set. Added static calls for finding important directories in a OS independent way.

Revision 1.94  2007/12/27 09:43:08  cignoni
moved colorize filters to a submenu of the filter menu

Revision 1.93  2007/12/23 10:50:57  cignoni
removed useless decl

Revision 1.92  2007/12/13 00:18:28  cignoni
added meshCreation class of filter, and the corresponding menu new under file

Revision 1.91  2007/11/20 11:49:48  cignoni
added setcurrent slot

Revision 1.90  2007/11/05 13:49:52  cignoni
better managment of the filter parameter dialog (stddialog)

Revision 1.89  2007/10/06 23:29:51  cignoni
corrected management of suspeneded editing actions. Added filter toolbar

Revision 1.88  2007/10/02 07:59:42  cignoni
New filter interface. Hopefully more clean and easy to use.

Revision 1.87  2007/07/24 07:19:04  cignoni
managed failure in loading of project. Added safe cleaning of meshes with nan coords

Revision 1.86  2007/07/10 07:19:21  cignoni
** Serious Changes **
again on the MeshDocument, the management of multiple meshes, layers, and per mesh transformation

Revision 1.85  2007/05/16 15:02:05  cignoni
Better management of toggling between edit actions and camera movement

Revision 1.84  2007/04/16 10:19:18  cignoni
v.1.1.0b string

Revision 1.83  2007/04/16 09:24:37  cignoni
** big change **
Added Layers managemnt.
Interfaces are changing...

Revision 1.82  2007/03/27 12:20:16  cignoni
Revamped logging iterface, changed function names in automatic parameters, better selection handling

Revision 1.81  2007/03/05 12:23:01  cignoni
v.1.0.0 string

Revision 1.80  2007/03/03 02:03:25  cignoni
Reformatted lower bar, added number of selected faces. Updated about dialog

Revision 1.79  2007/02/28 00:05:13  cignoni
Added Bug submitting menu

Revision 1.78  2007/02/26 12:03:44  cignoni
Added Help online and check for updates

Revision 1.77  2007/02/08 16:04:18  cignoni
Corrected behaviour of edit actions

Revision 1.76  2007/01/13 02:00:51  cignoni
Ver 0.9.1b

Revision 1.75  2006/12/21 21:24:54  cignoni
version 0.9.1

Revision 1.74  2006/12/13 17:37:02  pirosu
Added standard plugin window support

Revision 1.73  2006/12/12 11:16:08  cignoni
changed version string 0.9RC -> 0.9

Revision 1.72  2006/12/06 21:38:56  cignoni
0.9RC

Revision 1.71  2006/11/29 00:53:43  cignoni
Improved logging and added web based version checking

Revision 1.70  2006/11/08 01:04:48  cignoni
First version with http communications

Revision 1.69  2006/10/26 12:07:12  corsini
add lighting properties option

Revision 1.68  2006/06/27 08:07:42  cignoni
Restructured plugins interface for simplifying the server

Revision 1.67  2006/06/18 21:27:49  cignoni
Progress bar redesigned, now integrated in the workspace window

Revision 1.66  2006/06/16 01:26:07  cignoni
Added Initial Filter Script Dialog

Revision 1.65  2006/06/15 13:05:57  cignoni
added Filter History Dialogs

Revision 1.64  2006/06/12 15:20:44  cignoni
Initial Dragdrop support (still not working,,,)

Revision 1.63  2006/05/25 04:57:45  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.

Revision 1.62  2006/04/12 15:12:18  cignoni
Added Filter classes (cleaning, meshing etc)

Revision 1.61  2006/02/17 11:17:23  glvertex
- Moved closeAction in FileMenu
- Minor changes

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

//None of this should happen if we are compiling c, not c++
#ifdef __cplusplus
#include <GL/glew.h>

#include <QDir>
#include <QMainWindow>
//#include <QWorkspace>
#include <QMdiArea>
#include <QStringList>
#include <QColorDialog>
#include "meshmodel.h"
#include "glarea.h"
#include "stdpardialog.h"

#define MAXRECENTFILES 4

class QAction;
class QActionGroup;
class QMenu;
class QScrollArea;
class QSignalMapper;
class QProgressDialog;
class QHttp;


class MainWindow : public QMainWindow, MainWindowInterface
{
	Q_OBJECT

public:
	// callback function to execute a filter
	void executeFilter(QAction *action, RichParameterSet &srcpar, bool isPreview);

  MainWindow();
   static bool QCallBack(const int pos, const char * str);
	 const QString appName() const {return tr("MeshLab v")+appVer(); }
   const QString appVer() const {return tr("1.2.3b"); }

signals:
   void dispatchCustomSettings(RichParameterSet& rps);

public slots:

	bool open(QString fileName=QString(), GLArea *gla=0);
	bool openIn(QString fileName=QString());
	bool openProject(QString fileName=QString());
	void saveProject();
	void delCurrentMesh();
	void updateGL();
	void endEdit();
	void updateCustomSettings();

private slots:

	//////////// Slot Menu File //////////////////////
	void reload();
	void openRecentFile();
	bool saveAs(QString fileName = QString());
	bool save();
	bool saveSnapshot();
	///////////Slot Menu Edit ////////////////////////
	void applyEditMode();
	void suspendEditMode();
	///////////Slot Menu Filter ////////////////////////
	void startFilter();
	void applyLastFilter();
	void runFilterScript();
	void showFilterScript();
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
	void setLightingProperties();
	void setColorMode(QAction *qa);
	void applyRenderMode();
	//void applyColorMode();
	void toggleBackFaceCulling();
	void toggleSelectionRendering();
	void applyDecorateMode();
	///////////Slot Menu View ////////////////////////
	void fullScreen();
	void showToolbarFile();
	void showToolbarRender();
	void showInfoPane();
	void showTrackBall();
	void resetTrackBall();
	void showLayerDlg();
	///////////Slot Menu Windows /////////////////////
	void updateWindowMenu();
	void updateMenus();
	void updateStdDialog();

	///////////Slot Menu Preferences /////////////////
	void setCustomize();
	///////////Slot Menu Help ////////////////////////
	void about();
	void aboutPlugins();
	void helpOnline();
	void helpOnscreen();
	void submitBug();
	void checkForUpdates(bool verboseFlag=true);

	///////////Slot General Purpose ////////////////////////

	void dropEvent ( QDropEvent * event );
	void dragEnterEvent(QDragEnterEvent *);
	void connectionDone(bool status);

	///////////Solt Wrapper for QMdiArea //////////////////
	void wrapSetActiveSubWindow(QWidget* window);

private:
	void initGlobalParameters();
	void createStdPluginWnd();
	void createActions();
	void createMenus();
	void createToolBars();
	void loadMeshLabSettings();
	void loadPlugins();
	void keyPressEvent(QKeyEvent *);
	void updateRecentFileActions();
	void setCurrentFile(const QString &fileName);
	void addToMenu(QList<QAction *>, QMenu *menu, const char *slot);
	//void saveHistory(QAction *action,const RichParameterSet &params);
	//void LoadKnownFilters(QStringList &filters, QHash<QString, int> &allKnownFormats, int type);


	QHttp *httpReq;
	QBuffer myLocalBuf;
	int idHost;
	int idGet;
	bool VerboseCheckingFlag;

	MeshlabStdDialog *stddialog;
	static QProgressBar *qb;
	QMdiArea *mdiarea;
	QSignalMapper *windowMapper;
	QDir pluginsDir;
	QStringList pluginFileNames;
	std::vector<MeshIOInterface*> meshIOPlugins;
	QList<QAction *> editActionList;
	QList<QAction *> decoratorActionList;

	/* 
		For each running instance of meshlab, for the global params we have default (hardwired) values and current(saved,modified) values. 
	*/
	RichParameterSet currentGlobalParams;
	RichParameterSet defaultGlobalParams;
  
	QByteArray toolbarState;								//stato delle toolbar e dockwidgets

	QDir lastUsedDirectory;  //This will hold the last directory that was used to load/save a file/project in

public:
	GLArea *GLA() const {
	  if(mdiarea->currentSubWindow()==0) return 0;
	  GLArea *glw = qobject_cast<GLArea *>(mdiarea->currentSubWindow());
	  if(glw) return glw;
	  glw = qobject_cast<GLArea *>(mdiarea->currentSubWindow()->widget());
	  assert(glw);
	  return glw;
	}
  QMap<QString, QAction *> filterMap; // a map to retrieve an action from a name. Used for playing filter scripts.
  static QStatusBar *&globalStatusBar()
  {
    static QStatusBar *_qsb=0;
    return _qsb;
  }
	QMenu* layerMenu() { return filterMenuLayer; }

private:
	//////// ToolBars ///////////////
	QToolBar *mainToolBar;
	QToolBar *renderToolBar;
	QToolBar *editToolBar;
	QToolBar *filterToolBar;

	///////// Menus ///////////////
	QMenu *fileMenu;
	QMenu *fileMenuNew;
	QMenu *filterMenu;
	QMenu *filterMenuSelect;
  QMenu *filterMenuClean;
  QMenu *filterMenuRemeshing;
	QMenu *filterMenuColorize;
  QMenu *filterMenuSmoothing;
  QMenu *filterMenuQuality;
	QMenu *filterMenuNormal;
  QMenu *filterMenuLayer;
  QMenu *filterMenuRangeMap;
  QMenu *filterMenuPointSet;
  QMenu *filterMenuSampling;
	QMenu *filterMenuTexture;

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


	//////////// Actions Menu File ///////////////////////
	QAction *openAct;
	QAction *openInAct,*openProjectAct;
	QAction *closeAct;
	QAction *reloadAct;
	QAction *saveAct,*saveAsAct,*saveProjectAct;
	QAction *saveSnapshotAct;
	QAction *lastFilterAct;
	QAction *runFilterScriptAct;
	QAction *showFilterScriptAct;
	QAction *recentFileActs[MAXRECENTFILES];
	QAction *separatorAct;
	QAction *exitAct;
	/////////// Actions Menu Edit  /////////////////////
  QAction *suspendEditModeAct;
	/////////// Actions Menu Render /////////////////////
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
	QAction *setLightingPropertiesAct;
	QAction *setLightAct;
	QAction *backFaceCullAct;
  QAction *setSelectionRenderingAct;

	QActionGroup *colorModeGroupAct;
	QAction *colorModeNoneAct;
        QAction *colorModePerMeshAct;
	QAction *colorModePerVertexAct;
	QAction *colorModePerFaceAct;
	///////////Actions Menu View ////////////////////////
	QAction *fullScreenAct;
	QAction *showToolbarStandardAct;
	QAction *showToolbarRenderAct;
	QAction *showInfoPaneAct;
	QAction *showTrackBallAct;
	QAction *resetTrackBallAct;
	QAction *showLayerDlgAct;
	///////////Actions Menu Windows /////////////////////
	QAction *windowsTileAct;
	QAction *windowsCascadeAct;
	QAction *windowsNextAct;
	QAction *closeAllAct;
	///////////Actions Menu Preferences /////////////////
	QAction *setCustomizeAct;
	///////////Actions Menu Help ////////////////////////
	QAction *aboutAct;
	QAction *aboutPluginsAct;
	QAction *submitBugAct;
	QAction *onlineHelpAct;
	QAction *onscreenHelpAct;
	QAction *checkUpdatesAct;
	////////////////////////////////////////////////////
};

class FileOpenEater : public QObject
{
Q_OBJECT

public:
FileOpenEater() {noEvent=true;}
MainWindow *mainWindow;
bool noEvent;

protected:

bool eventFilter(QObject *obj, QEvent *event)
 {
	 if (event->type() == QEvent::FileOpen) {
						noEvent=false;
						QFileOpenEvent *fileEvent = static_cast<QFileOpenEvent*>(event);
						mainWindow->open(fileEvent->file());
						// QMessageBox::information(0,"Meshlab",fileEvent->file());
						return true;
        } else {
             // standard event processing
             return QObject::eventFilter(obj, event);
         }
     }
 };

#endif
#endif
