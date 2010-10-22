/****************************************************************************
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

#include "synthData.h"
#include "filter_photosynth.h"
#include <QtScript>
#include <unistd.h>

using namespace vcg;

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly
FilterPhotosynthPlugin::FilterPhotosynthPlugin()
{
  typeList << FP_IMPORT_PHOTOSYNTH;
  foreach(FilterIDType tt , types())
    actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
QString FilterPhotosynthPlugin::filterName(FilterIDType filterId) const
{
  switch(filterId)
  {
    case FP_IMPORT_PHOTOSYNTH:  return QString("Import Photosynth data");
    default:
        assert(0);
  }
  return QString();
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
QString FilterPhotosynthPlugin::filterInfo(FilterIDType filterId) const
{
  switch(filterId)
  {
    case FP_IMPORT_PHOTOSYNTH:
      return QString("Downloads the synth data from the given URL and creates a document with multiple layers, each containing a set of points");
    default:
      assert(0);
  }
  return QString("Unknown Filter");
}

// The FilterClass describes in which generic class of filters it fits.
// This choice affect the submenu in which each filter will be placed
// More than a single class can be choosen.
FilterPhotosynthPlugin::FilterClass FilterPhotosynthPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_IMPORT_PHOTOSYNTH:
      return MeshFilterInterface::MeshCreation;
    default:
      assert(0);
  }
  return MeshFilterInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define,
// - the name of the parameter,
// - the default value
// - the string shown in the dialog
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterPhotosynthPlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst)
{
  switch(ID(action))
  {
    case FP_IMPORT_PHOTOSYNTH:
      parlst.addParam(new RichString("synthURL",
//                                     "http://photosynth.net",
                                     "http://photosynth.net/view.aspx?cid=1d814021-a4ab-4cdb-a533-570fc2b5758d",
                                     "Synth URL",
                                     "Paste the synth URL from your browser."));
      break;
    default:
      assert(0);
  }
}

// The Real Core Function doing the actual mesh processing.
// Imports
bool FilterPhotosynthPlugin::applyFilter(QAction */*filter*/, MeshDocument &md, RichParameterSet &par, vcg::CallBackPos *cb)
{
  QString url = par.getString("synthURL");
  SynthData *synthData = SynthData::downloadSynthInfo(url);

  //Hangs on active wait until data are available from the server
  while(!synthData->dataReady())
  {
    int progress = 0;
    switch(synthData->step())
    {
    case SynthData::WEB_SERVICE:
      progress = 0;
      break;
    case SynthData::DOWNLOAD_JSON:
      progress = 25;
      break;
    case SynthData::PARSE_JSON:
      progress = 50;
      break;
    case SynthData::DOWNLOAD_BIN:
      progress = 75;
      break;
    case SynthData::LOADING_BIN:
      progress = 100;
    }

    cb(progress,synthData->progressInfo());
    //allows qt main loop to process the events relative to the response from the server,
    //triggering the signals that cause the invocation of the slots that process the response
    //and set the control variable that stops this active wait.
    //Note that a call to the function usleep() causes an infinite loop, because when the process awakes,
    //the control remains inside this loop and doesn't reach qt main loop that this way can't process events.
    QApplication::processEvents();
  }

  if(!synthData->isValid())
  {
    this->errorMessage = SynthData::errors[synthData->state()];
    return false;
  }



  const QList<CoordinateSystem*> *coordinateSystems = synthData->coordinateSystems();
  CoordinateSystem *sys;
  foreach(sys, *coordinateSystems)
  {
    MeshModel *mm= md.addNewMesh("coordsys"); // After Adding a mesh to a MeshDocument the new mesh is the current one
    qDebug("Adding a coordsys of %i points",sys->pointCloud()->points.size());
    Point p;
    foreach(p, sys->pointCloud()->points)
    {
      tri::Allocator<CMeshO>::AddVertices(mm->cm,1);
      mm->cm.vert.back().P() = Point3f(p._x,p._y,p._z);
      mm->cm.vert.back().C() = Color4b(p._r,p._g,p._b,255);
    }
    return true;
  }

  return true;
}

QString FilterPhotosynthPlugin::filterScriptFunctionName(FilterIDType filterID)
{
  switch(filterID)
  {
    case FP_IMPORT_PHOTOSYNTH:
      return QString("importPhotosynth");
    default:
      assert(0);
  }
  return QString();
}

Q_EXPORT_PLUGIN(FilterPhotosynthPlugin)
