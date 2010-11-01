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
                                     "http://photosynth.net/view.aspx?cid=0799dbca-08a8-4bb2-bdce-b00db6069828",
                                     "Synth URL",
                                     "Paste the synth URL from your browser."));
      parlst.addParam(new RichBool ("saveImages", false, "Download images", "Download images making up the specified synth."));
      //parlst.addParam(new RichSaveFile("savePath","./",".jpg","Save to","Select the path where images will be saved to"));
      parlst.addParam(new RichString("savePath",
                                     "./",
                                     "Save to",
                                     "Enter the path where images will be saved to"));
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
  QString path("");
  if(par.getBool("saveImages"))
    //path = par.getSaveFileName("savePath");
    path = par.getString("savePath");
  SynthData *synthData = SynthData::downloadSynthInfo(url,path);

  //Hangs on active wait until data are available from the server
  while(!synthData->_dataReady)
  {
    int progress = 0;
    switch(synthData->_progress)
    {
      case SynthData::WEB_SERVICE:
        progress = 0;
        break;
      case SynthData::DOWNLOAD_JSON:
        progress = 20;
        break;
      case SynthData::PARSE_JSON:
        progress = 40;
        break;
      case SynthData::DOWNLOAD_BIN:
        progress = 60;
        break;
      case SynthData::LOADING_BIN:
        progress = 80;
        break;
      case SynthData::DOWNLOAD_IMG:
        progress = 100;
    }

    cb(progress,SynthData::progress[synthData->_progress]);
    //allows qt main loop to process the events relative to the response from the server,
    //triggering the signals that cause the invocation of the slots that process the response
    //and set the control variable that stops this active wait.
    //Note that a call to the function usleep() causes an infinite loop, because when the process awakes,
    //the control remains inside this loop and doesn't reach qt main loop that this way can't process events.
    QApplication::processEvents();
  }

  if(!synthData->isValid())
  {
    this->errorMessage = SynthData::errors[synthData->_state];
    return false;
  }

  QDir dir(QDir::currentPath());
  QFile file(dir.filePath("Cam.txt"));
  bool success = true;
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
      success = false;
  QTextStream out(&file);
  const QList<CoordinateSystem*> *coordinateSystems = synthData->_coordinateSystems;
  CoordinateSystem *sys;
  foreach(sys, *coordinateSystems)
  {
    if(sys->_pointCloud)
    {
      MeshModel *mm = md.addNewMesh("coordsys"); // After Adding a mesh to a MeshDocument the new mesh is the current one
      Point p;
      foreach(p, sys->_pointCloud->_points)
      {
        tri::Allocator<CMeshO>::AddVertices(mm->cm,1);
        mm->cm.vert.back().P() = Point3f(p._x,p._z,p._y);
        mm->cm.vert.back().C() = Color4b(p._r,p._g,p._b,255);
      }

    }
    CameraParameters cam;
    foreach(cam, sys->_cameraParametersList)
    {
      //qreal w = sqrt(cam[CameraParameters::ROT_X] * cam[CameraParameters::ROT_X] +
      //               cam[CameraParameters::ROT_Y] * cam[CameraParameters::ROT_Y] +
      //               cam[CameraParameters::ROT_Z] * cam[CameraParameters::ROT_Z]);
      Matrix44d rot;
      rot.FromEulerAngles(cam[CameraParameters::ROT_X], cam[CameraParameters::ROT_Y], cam[CameraParameters::ROT_Z]);
      Shotd s;
      s.Extrinsics.SetRot(rot);
      s.Extrinsics.SetTra(Point3d(cam[CameraParameters::POS_X], cam[CameraParameters::POS_Y], cam[CameraParameters::POS_Z]));
      qreal focal = cam[CameraParameters::FOCAL_LENGTH];
      s.Intrinsics.FocalMm = focal * 10;
      s.Intrinsics.PixelSizeMm = Point2d(0.1,0.1);
      Image img = synthData->_imageMap->value(cam._imageID);
      s.Intrinsics.ViewportPx = Point2i(img._width,img._height);
      s.Intrinsics.CenterPx = Point2d(img._width/2,img._height/2);
      if(success)
      {
        QString traVec = QString("TranslationVector=\"%1 %2 %3 1\"").arg(s.Extrinsics.Tra().X()).arg(s.Extrinsics.Tra().Y()).arg(s.Extrinsics.Tra().Z());
        QString lensDist("LensDistortion=\"0 0\"");
        QString viewPx = QString("ViewportPixel=\"%1 %2\"").arg(img._width).arg(img._height);
        QString pxSize("PixelSizeMm=\"0.1 0.1\"");
        QString centerPx = QString("CenterPx=\"%1 %2\"").arg(img._width/2).arg(img._height/2);
        QString focalMm = QString("FocalMm=\"%1\"").arg(s.Intrinsics.FocalMm);
        out << "Image: " << img._ID << "\n";
        out << "<VCGCamera ";
        out << traVec << " ";
        out << lensDist << " ";
        out << viewPx << " ";
        out << pxSize << " ";
        out << centerPx << " ";
        out << focalMm << " ";
        out << "RotationMatrix=\"";

        unsigned int i, j;
        for(i = 0; i < 4; i++)
          for(j = 0; j < 4; j++)
            out << rot.ElementAt(i,j) << " ";
        out << "\"/>\n\n";
      }
    }
  }
  file.close();
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
