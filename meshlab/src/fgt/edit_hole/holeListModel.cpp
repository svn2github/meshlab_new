/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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

#include "holeListModel.h"

using namespace vcg; 


HoleListModel::HoleListModel(MeshModel *m, QObject *parent)
	: QAbstractItemModel(parent)	
{		
	state = HoleListModel::Selection;
	mesh = m;
	userBitHole = -1;
	updateModel();
}

void HoleListModel::clearModel()
{
	holes.clear();

	if(userBitHole > 0)
	{
		//mesh->clearDataMask(userBitHole);
		CMeshO::FaceIterator fi;
		for(fi = mesh->cm.face.begin(); fi!=mesh->cm.face.end(); ++fi)
		{
			//if(!(*fi).IsD())
				(*fi).ClearUserBit(userBitHole);
		}
	}
}

void HoleListModel::updateModel()
{
	clearModel();
	mesh->clearDataMask(MeshModel::MM_BORDERFLAG);
	mesh->updateDataMask(MeshModel::MM_FACETOPO);
	mesh->updateDataMask(MeshModel::MM_BORDERFLAG);

	userBitHole = FgtHole<CMeshO>::GetMeshHoles(mesh->cm, holes);
	emit dataChanged( index(0, 0), index(holes.size(), 2) );
	emit SGN_needUpdateGLA();
}

void HoleListModel::drawHoles() const
{
	// Disegno i contorni del buco
	glLineWidth(2.0f);
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_DEPTH_TEST); 
	glDepthMask(GL_FALSE);
	glDisable(GL_LIGHTING);

	// scorro tutti i buchi
	HoleVector::const_iterator it = holes.begin();
	for( ; it != holes.end(); ++it)
	{
		if( it->isSelected)
		{
			if(it->isAccepted)
				glColor(Color4b::DarkGreen);
			else
				glColor(Color4b::DarkRed);
		}
		else
			glColor(Color4b::DarkBlue);
		it->Draw();

	}

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);

	for(it = holes.begin(); it != holes.end(); ++it)
	{
		if(it->isSelected)
		{
			if(it->isAccepted)
				glColor(Color4b::Green);
			else
				glColor(Color4b::Red);
		}
		else
			glColor(Color4b::Blue);
				
		it->Draw();		
	}
}

void HoleListModel::drawCompenetratingFaces() const
{
	//glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_LIGHTING);
	glColor3f(0.8f, 0.8f, 0.f);
	HoleVector::const_iterator it = holes.begin();		
	it = holes.begin();
	for(it = holes.begin(); it != holes.end(); ++it)
		if(it->isCompenetrating())
			it->DrawCompenetratingFace(GL_LINE_LOOP);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	for(it = holes.begin(); it != holes.end(); ++it)
		if(it->isCompenetrating())
			it->DrawCompenetratingFace(GL_TRIANGLES);

	glLineWidth(4.0f);
	glColor3f(1.0f, 1.0f, 0.f);
	for(it = holes.begin(); it != holes.end(); ++it)
		if(it->isCompenetrating())
			it->DrawCompenetratingFace(GL_LINE_LOOP);

}

void HoleListModel::toggleSelectionHoleFromBorderFace(CFaceO *bface)
{
	int ind = -1;
	ind = FgtHole<CMeshO>::FindHoleFromBorderFace(bface, holes);
	if(ind == -1)
		return;
	holes[ind].isSelected = !holes[ind].isSelected;
	
	emit dataChanged( index(ind, 3), index(ind, 3) );
	emit SGN_needUpdateGLA();
}

void HoleListModel::toggleAcceptanceHoleFromPatchFace(CFaceO *bface)
{
	int ind = -1;
	assert(state == HoleListModel::Filled);
	ind = FgtHole<CMeshO>::FindHoleFromHoleFace(bface, holes);
	if(ind == -1)
		return;
	holes[ind].isAccepted = !holes[ind].isAccepted;

	emit dataChanged( index(ind, 5), index(ind, 5) );	
	emit SGN_needUpdateGLA();
}



void HoleListModel::fill(bool antiSelfIntersection)
{
	mesh->clearDataMask(MeshModel::MM_FACEMARK);
	mesh->updateDataMask(MeshModel::MM_FACEMARK);

	std::vector<CMeshO::FacePointer *> local_facePointer;
	
	HoleVector::iterator it = holes.begin();
	for( ; it != holes.end(); it++ )
		local_facePointer.push_back(&it->holeInfo.p.f);
	
	for(it = holes.begin(); it != holes.end(); it++ )
	{
		if( it->isSelected )
		{
			it->facesPatch.clear();
			
			if (antiSelfIntersection)
				vcgHole::FillHoleEar<tri::SelfIntersectionEar< CMeshO> >(mesh->cm, it->holeInfo, userBitHole, local_facePointer, &(it->facesPatch));
			else
				vcgHole::FillHoleEar<vcg::tri::TrivialEar<CMeshO> >(mesh->cm, it->holeInfo, userBitHole, local_facePointer, &(it->facesPatch));

			it->updateSelfIntersectionState(mesh->cm);
			state = HoleListModel::Filled;
		}
	}
	emit layoutChanged();
}

void HoleListModel::acceptFilling(bool forcedCancel)
{
	assert(state == HoleListModel::Filled);
	HoleVector::iterator it = holes.begin();
	for( ; it != holes.end(); it++ )
	{
		if( (it->isSelected && !it->isAccepted) || forcedCancel)
			it->RestoreHole(mesh->cm);
	}
	
	mesh->clearDataMask(MeshModel::MM_FACETOPO);
	updateModel();
	state = HoleListModel::Selection;
	emit layoutChanged();
}


void HoleListModel::acceptBrdging(bool forcedCancel)
{
	assert(state == HoleListModel::Bridged);
	HoleVector::iterator it = holes.begin();
	for( ; it != holes.end(); it++ )
	{
		if( (it->isSelected && !it->isAccepted) || forcedCancel)
		{
			//To DO
			//it->RestoreBridge(mesh->cm);
		}
	}
	
	mesh->clearDataMask(MeshModel::MM_FACETOPO);
	updateModel();
	state = HoleListModel::Selection;
	emit layoutChanged();
}


/************* Implementazione QAbstractItemModel class *****************/


QVariant HoleListModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() )
		return QVariant();
	
	if(role == Qt::DisplayRole)
	{
		switch(index.column())
		{
		case 0:
			return holes[index.row()].name;
		case 1:
			return holes.at(index.row()).getEdgeCount();
		case 2:
			return QString("%1").arg(holes.at(index.row()).getPerimeter(), 0, 'f', 5);
		}
	}
	else if (role == Qt::TextAlignmentRole)
	{
		if(index.column() < 4)
			return Qt::AlignLeft;
		else
			return Qt::AlignCenter;
	}
	else if (role == Qt::CheckStateRole)
	{
		bool checked;
		if(index.column() == 3)
			checked = holes[index.row()].isSelected;
		else if(state == HoleListModel::Filled && holes[index.row()].isSelected)
		{
			if(index.column() == 4)
				checked = holes[index.row()].isCompenetrating();
			else if(index.column() == 5)
				checked = holes[index.row()].isAccepted;
			else 
				return QVariant();
		}
		else 
			return QVariant();

		if(checked)
			return Qt::Checked;
		else
			return Qt::Unchecked;
	}
	
	return QVariant();
}


QVariant HoleListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch(section)
		{
		case 0:
			return tr("Hole");
		case 1:
			return tr("Edges");
		case 2:
			return tr("Perimeter");
		case 3:
			if(state == HoleListModel::Selection)
				return tr("Select");
			else 
				return tr("Fill");
		case 4:
			if(state == HoleListModel::Filled)
				return tr("Comp.");
		case 5:
			if(state == HoleListModel::Filled)
				return tr("Accept");
		}
	}
	else if (orientation == Qt::Horizontal && role == Qt::SizeHintRole)
	{
		switch(section)
		{
		case 0:
			return QSize(63, 20);
		case 1:
			return QSize(38, 20);
		case 2:
			return QSize(55, 20);
		case 3:
			if(state == HoleListModel::Filled)
				return QSize(20, 20);
			else
				return QSize(50, 20);
		case 4:
			return QSize(38, 20);
		case 5:
			return QSize(42, 20);
		}
	}
	else if (orientation == Qt::Horizontal && role == Qt::ToolTip && state==HoleListModel::Filled && section == 4)
		return tr("Compenetration");	

	return QVariant();
}


QModelIndex HoleListModel::index(int row, int column, const QModelIndex &parent) const
{
	if(row >= (int)holes.size())
		return QModelIndex();
	return createIndex(row,column, 0);
}


Qt::ItemFlags HoleListModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags ret = QAbstractItemModel::flags(index);
    
	if (!index.isValid())
        return Qt::ItemIsEnabled;

	if(index.column() == 0)
		ret = ret | Qt::ItemIsEditable;
	else if( (index.column() == 3 && state == HoleListModel::Selection) ||
			 (index.column() == 5 && state == HoleListModel::Filled) )
		return Qt::ItemIsUserCheckable;
		
	return ret;
}

bool HoleListModel::setData( const QModelIndex & index, const QVariant & value, int role )
{	
	if(!index.isValid())
		return false;

	bool ret = false;
	if(role == Qt::EditRole && index.column() == 0)
	{
		QString newName = value.toString().trimmed();
		if(newName != "")
		{
			holes[index.row()].name = newName;
			ret = true;
		}
	}
	else if(role == Qt::CheckStateRole)
	{
		if(state == HoleListModel::Selection)
		{
			if(index.column() == 3 && state == HoleListModel::Selection)
			{
				holes[index.row()].isSelected = !holes[index.row()].isSelected;
				ret = true;
			}			
		}
		else if(index.column() == 5)
		{	// check accept
			holes[index.row()].isAccepted = !holes[index.row()].isAccepted;
			ret = true;
		}
	}
	if(ret)
	{
		emit dataChanged(index, index);
		emit SGN_needUpdateGLA();
	}

	return ret;
}
