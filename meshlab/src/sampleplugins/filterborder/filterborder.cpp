/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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
 Revision 1.2  2008/04/04 10:03:52  cignoni
 Solved namespace ambiguities caused by the removal of a silly 'using namespace' in meshmodel.h

 Revision 1.1  2007/12/02 07:57:48  cignoni
 Added the new sample filter plugin that removes border faces

 
*****************************************************************************/
#include <Qt>
#include <QtGui>
#include "filterborder.h"

#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/clean.h>

using namespace std;
using namespace vcg;

FilterBorder::FilterBorder() 
{
  typeList << FP_REMOVE_BORDER_FACE;
 
  FilterIDType tt;
  foreach(tt , types())
	    actionList << new QAction(filterName(tt), this);

}

FilterBorder::~FilterBorder() {
	for (int i = 0; i < actionList.count() ; i++ ) 
		delete actionList.at(i);
}

const QString FilterBorder::filterName(FilterIDType filter) 
{
 switch(filter)
  {
	  case FP_REMOVE_BORDER_FACE :								return QString("Remove border faces");
  	default: assert(0);
  }
  return QString("error!");
}

const QString FilterBorder::filterInfo(FilterIDType filterId)
{
  switch(filterId)
  {
		case FP_REMOVE_BORDER_FACE:	     return tr("Remove all the faces that has at least one border vertex."); 
  	default: assert(0);
  }
  return QString("error!");
}

const FilterBorder::FilterClass FilterBorder::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_REMOVE_BORDER_FACE :
      return MeshFilterInterface::Cleaning;     
    default : return MeshFilterInterface::Generic;
  }
}

const int FilterBorder::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case FP_REMOVE_BORDER_FACE :	return MeshModel::MM_BORDERFLAG;
    default: assert(0);
  }
  return 0;
}

void FilterBorder::initParameterSet(QAction *action,MeshModel &m, FilterParameterSet & parlst)
{ 
	pair<float,float> qualityRange;
  switch(ID(action))
  {
    case FP_REMOVE_BORDER_FACE :
		  parlst.addInt("IterationNum",1,"Iteration","Number of times that the removal of facexxx");
		  parlst.addBool("DeleteVertices",true,"Delete unreferenced vertices","Longhelp");
 		  break;
	default: assert(0);
  }
}



bool FilterBorder::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos * cb) 
{
				CMeshO::FaceIterator fi;
				CMeshO::VertexIterator vi;
	if(filter->text() == filterName(FP_REMOVE_BORDER_FACE) )
	  {
			tri::UpdateFlags<CMeshO>::VertexClearV(m.cm);
      int IterationNum = par.getInt("IterationNum");		
      bool DeleteVertices = par.getBool("DeleteVertices");	
			for(int i=0;i<IterationNum;++i)
			{
				for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
					if(!(*fi).IsD())
					{
						if((*fi).V(0)->IsB() ||
							 (*fi).V(1)->IsB() ||
							 (*fi).V(2)->IsB() )
						{
							 (*fi).V(0)->SetV();
							 (*fi).V(1)->SetV();
							 (*fi).V(2)->SetV();
							 tri::Allocator<CMeshO>::DeleteFace(m.cm,*fi);
						}
					}
				for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
					if(!(*vi).IsD()) 
						if((*vi).IsV()) (*vi).SetB();
			}
			if(DeleteVertices)
			{
				for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
					if(!(*fi).IsD())
					{
						(*fi).V(0)->ClearV();
						(*fi).V(1)->ClearV();
						(*fi).V(2)->ClearV();
					}

					for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
						if(!(*vi).IsD()) 
							if((*vi).IsV()) tri::Allocator<CMeshO>::DeleteVertex(m.cm,*vi);
			}

			m.clearDataMask(MeshModel::MM_BORDERFLAG);
			m.updateDataMask(MeshModel::MM_BORDERFLAG);
	  }
	return true;
}


Q_EXPORT_PLUGIN(FilterBorder)
