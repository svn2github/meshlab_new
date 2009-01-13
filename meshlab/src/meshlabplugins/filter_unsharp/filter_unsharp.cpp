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
#include <Qt>
#include <QtGui>
#include "filter_unsharp.h"

#include <vcg/complex/trimesh/update/quality.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/complex/trimesh/crease_cut.h>


using namespace vcg;
using namespace std;

FilterUnsharp::FilterUnsharp() 
{
  typeList <<  
		FP_CREASE_CUT << 
		FP_LAPLACIAN_SMOOTH<< 
    FP_HC_LAPLACIAN_SMOOTH<<
		FP_SD_LAPLACIAN_SMOOTH<< 
    FP_TWO_STEP_SMOOTH<< 
    FP_TAUBIN_SMOOTH<< 
		FP_VERTEX_QUALITY_SMOOTHING<<
		FP_FACE_NORMAL_SMOOTHING<<
		FP_UNSHARP_NORMAL<<
		FP_UNSHARP_GEOMETRY<<
		FP_UNSHARP_QUALITY <<
		FP_UNSHARP_VERTEX_COLOR <<
		FP_RECOMPUTE_VERTEX_NORMAL <<
		FP_RECOMPUTE_FACE_NORMAL <<
		FP_FACE_NORMAL_NORMALIZE <<
		FP_VERTEX_NORMAL_NORMALIZE <<
		FP_LINEAR_MORPH	;
;
 
  FilterIDType tt;
  foreach(tt , types())
	    actionList << new QAction(filterName(tt), this);

}

FilterUnsharp::~FilterUnsharp() {
	for (int i = 0; i < actionList.count() ; i++ ) 
		delete actionList.at(i);
}

const QString FilterUnsharp::filterName(FilterIDType filter) 
{
 switch(filter)
  {
		case FP_LAPLACIAN_SMOOTH :						return QString("Laplacian Smooth");
		case FP_HC_LAPLACIAN_SMOOTH :					return QString("HC Laplacian Smooth");
		case FP_SD_LAPLACIAN_SMOOTH :					return QString("ScaleDependent Laplacian Smooth");
		case FP_TWO_STEP_SMOOTH :	    				return QString("TwoStep Smooth");
		case FP_TAUBIN_SMOOTH :							return QString("Taubin Smooth");
		case FP_CREASE_CUT :							return QString("Cut mesh along crease edges");
  	case FP_FACE_NORMAL_NORMALIZE:		return QString("Normalize Face Normals"); 
  	case FP_VERTEX_NORMAL_NORMALIZE:		return QString("Normalize Vertex Normals"); 
  	case FP_FACE_NORMAL_SMOOTHING:	  return QString("Smooth Face Normals"); 
  	case FP_VERTEX_QUALITY_SMOOTHING:	return QString("Smooth vertex quality"); 
  	case FP_UNSHARP_NORMAL:						return QString("UnSharp Mask Normals"); 
  	case FP_UNSHARP_GEOMETRY:					return QString("UnSharp Mask Geometry"); 
  	case FP_UNSHARP_QUALITY:					return QString("UnSharp Mask Quality"); 
  	case FP_UNSHARP_VERTEX_COLOR:	    return QString("UnSharp Mask Color"); 
	  case FP_RECOMPUTE_VERTEX_NORMAL:	return QString("Recompute Vertex Normals"); 
	  case FP_RECOMPUTE_FACE_NORMAL:		return QString("Recompute Face Normals"); 
		case FP_LINEAR_MORPH :	return QString("Vertex Linear Morphing");
		

  	default: assert(0);
  }
  return QString("error!");
}

const QString FilterUnsharp::filterInfo(FilterIDType filterId)
{
  switch(filterId)
  {
		case FP_LAPLACIAN_SMOOTH :          return tr("Laplacian smooth of the mesh: for each vertex it calculates the average position with nearest vertex");  
    case FP_HC_LAPLACIAN_SMOOTH : 			return tr("HC Laplacian Smoothing, extended version of Laplacian Smoothing, based on the paper of Vollmer, Mencl, and Muller");  
    case FP_SD_LAPLACIAN_SMOOTH : 			return tr("Scale Dependent Laplacian Smoothing, extended version of Laplacian Smoothing, based on the Fujiwara extended umbrella operator");  
    case FP_TWO_STEP_SMOOTH : 			    return tr("Two Step Smoothing, a feature preserving/enhancing fairing filter. It is based on a Normal Smoothing step where similar normals are averaged toghether and a step where the vertexes are fitted on the new normals");  
    case FP_TAUBIN_SMOOTH :							return tr("The $lambda-mu$ taubin smoothing, it make two steps of smoothing, forth and back, for each iteration");  
		case FP_CREASE_CUT:									return tr("Cut the mesh along crease edges, duplicating the vertices as necessary. Crease edges are defined according to the variation of normal of the adjacent faces"); 
		case FP_FACE_NORMAL_NORMALIZE:	    return tr("Normalize Face Normal Lenghts"); 
		case FP_VERTEX_NORMAL_NORMALIZE:	    return tr("Normalize Vertex Normal Lenghts"); 
		case FP_VERTEX_QUALITY_SMOOTHING:	  return tr("Laplacian smooth of the quality values."); 
		case FP_FACE_NORMAL_SMOOTHING:	    return tr("Smooth Face Normals without touching the position of the vertices."); 
  	case FP_UNSHARP_NORMAL:							return tr("Unsharp mask filtering of the normals, putting in more evidence normal variations"); 
  	case FP_UNSHARP_GEOMETRY:						return tr("Unsharp mask filtering of geometric shape, putting in more evidence ridges and valleys variations"); 
  	case FP_UNSHARP_QUALITY:						return tr("Unsharp mask filtering of the quality field"); 
  	case FP_UNSHARP_VERTEX_COLOR:				return tr("Unsharp mask filtering of the color, putting in more evidence color edge variations"); 
		case FP_RECOMPUTE_VERTEX_NORMAL:		return tr("Recompute vertex normals as an area weighted average of normal of the incident faces");
		case FP_RECOMPUTE_FACE_NORMAL:			return tr("Recompute face normals as the normal of the plane of the face");
		case FP_LINEAR_MORPH :							return tr("Morph current mesh towards a target with the same number of vertices. <br> The filter assumes that the two meshes have also the same vertex ordering.");

  	default: assert(0);
  }
  return QString("error!");
}

const FilterUnsharp::FilterClass FilterUnsharp::getClass(QAction *a)
{
  switch(ID(a))
  {
			case FP_CREASE_CUT :
			return MeshFilterInterface::FilterClass( 	MeshFilterInterface::Normal | MeshFilterInterface::Remeshing);				
  		case FP_SD_LAPLACIAN_SMOOTH:
			case FP_HC_LAPLACIAN_SMOOTH:
			case FP_LAPLACIAN_SMOOTH:
			case FP_TWO_STEP_SMOOTH:
			case FP_TAUBIN_SMOOTH:
			case FP_FACE_NORMAL_SMOOTHING:	  
			case FP_VERTEX_QUALITY_SMOOTHING:
			case FP_UNSHARP_NORMAL:				
			case FP_UNSHARP_GEOMETRY:	    
			case FP_UNSHARP_QUALITY:
			case FP_LINEAR_MORPH :
					return 	MeshFilterInterface::Smoothing;

			case FP_UNSHARP_VERTEX_COLOR:	     
					return MeshFilterInterface::FilterClass( 	MeshFilterInterface::Smoothing | MeshFilterInterface::VertexColoring);
				
			case FP_RECOMPUTE_FACE_NORMAL :
			case FP_RECOMPUTE_VERTEX_NORMAL :
			case FP_FACE_NORMAL_NORMALIZE:	  
			case FP_VERTEX_NORMAL_NORMALIZE:	  
					return MeshFilterInterface::Normal;

    default : return MeshFilterInterface::Generic;
  }
}

const int FilterUnsharp::getRequirements(QAction *action)
{
  switch(ID(action))
  {
		case FP_HC_LAPLACIAN_SMOOTH:  
    case FP_SD_LAPLACIAN_SMOOTH:  
    case FP_TAUBIN_SMOOTH:  
    case FP_LAPLACIAN_SMOOTH:     return MeshModel::MM_FACEFLAGBORDER;
    case FP_TWO_STEP_SMOOTH:      return MeshModel::MM_VERTFACETOPO;
		case FP_UNSHARP_GEOMETRY:	
		case FP_UNSHARP_QUALITY:	
		case FP_VERTEX_QUALITY_SMOOTHING:
		case FP_UNSHARP_VERTEX_COLOR:	return MeshModel::MM_FACEFLAGBORDER;
    case FP_CREASE_CUT :	return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER;
		case FP_UNSHARP_NORMAL:		return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER;
		case FP_FACE_NORMAL_SMOOTHING : return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER;
		case FP_RECOMPUTE_FACE_NORMAL :
		case FP_RECOMPUTE_VERTEX_NORMAL :
		case FP_FACE_NORMAL_NORMALIZE:
		case FP_VERTEX_NORMAL_NORMALIZE:
		case FP_LINEAR_MORPH :
											return 0; 
			
    default: assert(0);
  }
  return 0;
}

bool FilterUnsharp::autoDialog(QAction *action) 
{
	switch(ID(action))
	{
			case FP_CREASE_CUT :
		  case FP_TWO_STEP_SMOOTH:
			case FP_LAPLACIAN_SMOOTH:
			case FP_TAUBIN_SMOOTH:
			case FP_SD_LAPLACIAN_SMOOTH:
			case FP_UNSHARP_GEOMETRY:		
			case FP_UNSHARP_QUALITY:		
			case FP_UNSHARP_VERTEX_COLOR:	
			case FP_UNSHARP_NORMAL:
			case FP_LINEAR_MORPH :
					return true;
	}
	return false;
}
void FilterUnsharp::initParameterSet(QAction *action, MeshModel &m, FilterParameterSet & parlst)
{
	switch(ID(action))
	{
		case FP_CREASE_CUT :
			parlst.addFloat("angleDeg", 90.f, tr("Crease Angle (degree)"), tr("If the angle between the normals of two adjacent faces is <b>larger</b> that this threshold the edge is considered a creased and the mesh is cut along it."));
			break;
		case FP_UNSHARP_NORMAL: 
			parlst.addBool("recalc", false, tr("Recompute Normals"), tr("Recompute normals from scratch before the unsharp masking"));
			parlst.addFloat("weight", 0.3f, tr("Unsharp Weight"), tr("the unsharp weight <i>w<sub><big>u</big></sub></i> in the unsharp mask equation: <br> <i>w<sub><big>o</big></sub>orig + w<sub><big>u</big></sub> (orig - lowpass)<i><br>"));
			parlst.addFloat("weightOrig", 1.f, tr("Original Weight"), tr("How much the original signal is used, e.g. the weight <i>w<sub><big>o</big></sub></i> in the above unsharp mask equation.<br> Usually you should not need to change the default 1.0 value."));
			parlst.addInt("iterations", 5, "Smooth Iterations", 	tr("number of laplacian face smooth iterations in every run"));
		break;
		case FP_UNSHARP_GEOMETRY:
			parlst.addFloat("weight", 0.3f, tr("Unsharp Weight"), tr("the unsharp weight <i>w<sub><big>u</big></sub></i> in the unsharp mask equation: <br> <i>w<sub><big>o</big></sub>orig + w<sub><big>u</big></sub> (orig - lowpass)<i><br>"));
			parlst.addFloat("weightOrig", 1.f, tr("Original Weight"), tr("How much the original signal is used, e.g. the weight <i>w<sub><big>o</big></sub></i> in the above unsharp mask equation<br> Usually you should not need to change the default 1.0 value."));
			parlst.addInt("iterations", 5, "Smooth Iterations", 	tr("number ofiterations of laplacian smooth in every run"));
			break;
		case FP_UNSHARP_VERTEX_COLOR:
			parlst.addFloat("weight", 0.3f, tr("Unsharp Weight"), tr("the unsharp weight <i>w<sub><big>u</big></sub></i> in the unsharp mask equation: <br> <i>w<sub><big>o</big></sub>orig + w<sub><big>u</big></sub> (orig - lowpass)<i><br>"));
			parlst.addFloat("weightOrig", 1.f, tr("Original Color Weight"), tr("How much the original signal is used, e.g. the weight <i>w<sub><big>o</big></sub></i> in the above unsharp mask equation<br> Usually you should not need to change the default 1.0 value."));
			parlst.addInt("iterations", 5, "Smooth Iterations", 	tr("number of iterations of laplacian smooth in every run"));
			break;
		case FP_UNSHARP_QUALITY:
			parlst.addFloat("weight", 0.3f, tr("Unsharp Weight"), tr("the unsharp weight <i>w<sub><big>u</big></sub></i> in the unsharp mask equation: <br> <i>w<sub><big>o</big></sub>orig + w<sub><big>u</big></sub> (orig - lowpass)<i><br>"));
			parlst.addFloat("weightOrig", 1.f, tr("Original Weight"), tr("How much the original signal is used, e.g. the weight <i>w<sub><big>o</big></sub></i> in the above unsharp mask equation<br> Usually you should not need to change the default 1.0 value."));
			parlst.addInt("iterations", 5, "Smooth Iterations", 	tr("number of iterations of laplacian smooth in every run"));
			break;
		case FP_TWO_STEP_SMOOTH:
		parlst.addInt  ("stepSmoothNum", (int) 3,"Smoothing steps", "The number of times that the whole algorithm (normal smoothing + vertex fitting) is iterated.");
		parlst.addFloat("normalThr", (float) 60,"Feature Angle Threshold (deg)", "Specify a threshold angle for features that you want to be preserved.\nFeatures forming angles LARGER than the specified threshold will be preserved.");
		parlst.addInt  ("stepNormalNum", (int) 20,"Normal Smoothing steps", "Number of iteration of normal smoothing step. The larger the better and (the slower)");
		parlst.addBool ("Selected",m.cm.sfn>0,"Affect only selected faces");
		break;
		case FP_LAPLACIAN_SMOOTH:
			parlst.addInt  ("stepSmoothNum", (int) 3,"Smoothing steps", "The number of times that the whole algorithm (normal smoothing + vertex fitting) is iterated.");
			parlst.addBool ("Selected",m.cm.sfn>0,"Affect only selected faces");
			break;
		case FP_TAUBIN_SMOOTH:
			parlst.addFloat("lambda", (float) 0.5,"Lambda", "The lambda parameter of the Taubin Smoothing algorithm");
			parlst.addFloat("mu", (float) -0.53,"mu", "The mu parameter of the Taubin Smoothing algorithm");
			parlst.addInt  ("stepSmoothNum", (int) 10,"Smoothing steps", "The number of times that the taubin smoothing is iterated. Usually it requires a larger number of iteration than the classical laplacian");
			parlst.addBool ("Selected",m.cm.sfn>0,"Affect only selected faces");
			break;
		case FP_SD_LAPLACIAN_SMOOTH:
		{
			parlst.addInt  ("stepSmoothNum", (int) 3,"Smoothing steps", "The number of times that the whole algorithm (normal smoothing + vertex fitting) is iterated.");
			float maxVal = m.cm.bbox.Diag()/10;
		  parlst.addAbsPerc("delta",maxVal*0.01,0,maxVal,"delta", "");
      parlst.addBool ("Selected",m.cm.sfn>0,"Affect only selected faces");
		}
		break;
		case FP_LINEAR_MORPH :
		{
			parlst.addMesh ("TargetMesh", 1, "Target Mesh", "The mesh that is the morph target.");
			
			parlst.addDynamicFloat("PercentMorph", 0.0, -150, 250, (MeshModel::MM_VERTCOORD | MeshModel::MM_VERTNORMAL), 
			"% Morph", "The percent you want to morph toward (or away from) the target. <br>"
			"0 means current mesh <br>"
			"100 means targe mesh <br>"
			"<0 and >100 linearly extrapolate between the two mesh <br>");
		}
		break;

	}
}

bool FilterUnsharp::applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & par, vcg::CallBackPos * cb) 
{
	switch(ID(filter))
	{
		case FP_CREASE_CUT :{
				float angleDeg = par.getFloat("angleDeg");
				tri::CreaseCut(m.cm, math::ToRad(60.0f));
				m.clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
		}
			break;
		
  case FP_FACE_NORMAL_SMOOTHING :
				 tri::Smooth<CMeshO>::FaceNormalLaplacianFF(m.cm);
			 break;
  case FP_VERTEX_QUALITY_SMOOTHING :
				 tri::Smooth<CMeshO>::VertexQualityLaplacian(m.cm);
			 break;
			 
	case FP_LAPLACIAN_SMOOTH :
	  {
			int stepSmoothNum = par.getInt("stepSmoothNum");
			size_t cnt=tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
      tri::Smooth<CMeshO>::VertexCoordLaplacian(m.cm,stepSmoothNum,cnt>0,cb);
			Log(GLLogStream::FILTER, "Smoothed %d vertices", cnt>0 ? cnt : m.cm.vn);	   
	    tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);	    
	  }
		break;
	case FP_SD_LAPLACIAN_SMOOTH:
	  {
			int stepSmoothNum = par.getInt("stepSmoothNum");
			size_t cnt=tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
			float delta = par.getAbsPerc("delta");
			tri::Smooth<CMeshO>::VertexCoordScaleDependentLaplacian_Fujiwara(m.cm,stepSmoothNum,delta);
			Log(GLLogStream::FILTER, "Smoothed %d vertices", cnt>0 ? cnt : m.cm.vn);	   
	    tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);	    
	  }
		break;
	case FP_HC_LAPLACIAN_SMOOTH:
	  {
			size_t cnt=tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
      tri::Smooth<CMeshO>::VertexCoordLaplacianHC(m.cm,1,cnt>0);
      tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);	    
	  }
		break;
  case FP_TWO_STEP_SMOOTH:
	  {
			tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);			
			tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
			int stepSmoothNum = par.getInt("stepSmoothNum");
			float normalThr   = cos(math::ToRad(par.getFloat("normalThr")));
      int stepNormalNum = par.getInt("stepNormalNum");
      bool selectedFlag = par.getBool("Selected");
      tri::UpdateNormals<CMeshO>::PerFaceNormalized(m.cm);
      tri::Smooth<CMeshO>::VertexCoordPasoDobleFast(m.cm, stepSmoothNum, normalThr, stepNormalNum,selectedFlag);
      tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);	    
	  }
		break;
	case FP_TAUBIN_SMOOTH :
	  {
			int stepSmoothNum = par.getInt("stepSmoothNum");
			float lambda=par.getFloat("lambda");
			float mu=par.getFloat("mu");

			size_t cnt=tri::UpdateSelection<CMeshO>::VertexFromFaceStrict(m.cm);
      tri::Smooth<CMeshO>::VertexCoordTaubin(m.cm,stepSmoothNum,lambda,mu,cnt>0,cb);
			Log(GLLogStream::FILTER, "Smoothed %d vertices", cnt>0 ? cnt : m.cm.vn);	   
	    tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);	    
	  }
			break;
	case FP_RECOMPUTE_FACE_NORMAL : 
			tri::UpdateNormals<CMeshO>::PerFace(m.cm);
			break;
	case FP_RECOMPUTE_VERTEX_NORMAL : 
			tri::UpdateNormals<CMeshO>::PerVertexFromCurrentFaceNormal(m.cm);
			break;
	case FP_FACE_NORMAL_NORMALIZE :
			tri::UpdateNormals<CMeshO>::NormalizeFace(m.cm);
			 break;
	case FP_VERTEX_NORMAL_NORMALIZE :
			tri::UpdateNormals<CMeshO>::NormalizeFace(m.cm);
			 break;
	
	case FP_UNSHARP_NORMAL:			
			{	
				float alpha=par.getFloat("weight");
				float alphaorig=par.getFloat("weightOrig");
				int smoothIter = par.getInt("iterations");
				
				tri::Allocator<CMeshO>::CompactFaceVector(m.cm);
				vector<Point3f> normalOrig(m.cm.fn);
				for(int i=0;i<m.cm.fn;++i)
					normalOrig[i]=m.cm.face[i].cN();
				
				for(int i=0;i<smoothIter;++i)
						tri::Smooth<CMeshO>::FaceNormalLaplacianFF(m.cm);
				
				for(int i=0;i<m.cm.fn;++i)
					m.cm.face[i].N() = normalOrig[i] + (normalOrig[i] - m.cm.face[i].N())*alpha;
				
			}	break;
	case FP_UNSHARP_GEOMETRY:			
			{	
				float alpha=par.getFloat("weight");
				float alphaorig=par.getFloat("weightOrig");
				int smoothIter = par.getInt("iterations");
				
				tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
				vector<Point3f> geomOrig(m.cm.vn);
				for(int i=0;i<m.cm.vn;++i)
					geomOrig[i]=m.cm.vert[i].P();
				
				tri::Smooth<CMeshO>::VertexCoordLaplacian(m.cm,smoothIter);
				
				for(int i=0;i<m.cm.vn;++i)
					m.cm.vert[i].P()=geomOrig[i]*alphaorig + (geomOrig[i] - m.cm.vert[i].P())*alpha;				
					
				tri::UpdateNormals<CMeshO>::PerVertexPerFace(m.cm);
				
			}	break;
	case FP_UNSHARP_VERTEX_COLOR:			
			{	
				float alpha=par.getFloat("weight");
				float alphaorig=par.getFloat("weightOrig");
				int smoothIter = par.getInt("iterations");
				
				tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
				vector<Color4f> colorOrig(m.cm.vn);
				for(int i=0;i<m.cm.vn;++i)
					colorOrig[i].Import(m.cm.vert[i].C());
				
				tri::Smooth<CMeshO>::VertexColorLaplacian(m.cm,smoothIter);
				for(int i=0;i<m.cm.vn;++i)
					{ 
						Color4f colorDelta = colorOrig[i] - Color4f::Construct(m.cm.vert[i].C());
						Color4f newCol = 	colorOrig[i]*alphaorig + colorDelta*alpha;	 // Unsharp formula 
						Clamp(newCol); // Clamp everything in the 0..1 range
						m.cm.vert[i].C().Import(newCol);
						
						}
			}	break;
	case FP_UNSHARP_QUALITY:			
	{	
				float alpha=par.getFloat("weight");
				float alphaorig=par.getFloat("weightOrig");
				int smoothIter = par.getInt("iterations");
				
				tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
				vector<float> qualityOrig(m.cm.vn);
				for(int i=0;i<m.cm.vn;++i)
					qualityOrig[i] = m.cm.vert[i].Q();
				
				tri::Smooth<CMeshO>::VertexQualityLaplacian(m.cm, smoothIter);
				for(int i=0;i<m.cm.vn;++i)
				{ 
					float qualityDelta = qualityOrig[i] - m.cm.vert[i].Q();		
					m.cm.vert[i].Q() = 	qualityOrig[i]*alphaorig + qualityDelta*alpha;	 // Unsharp formula 
				}
	}	break;
	
	case FP_LINEAR_MORPH:
	{
		CMeshO &targetMesh = par.getMesh("TargetMesh")->cm;
		CMeshO &sourceMesh = m.cm;
				
		//if the numbers of vertices dont match up
		if(sourceMesh.vn != targetMesh.vn)
		{
			errorMessage = "Number of vertices is not the same so you cant morph between these two meshes.";
			return false;
		}
		
		vcg::tri::Allocator<CMeshO>::CompactVertexVector(sourceMesh); 
		vcg::tri::Allocator<CMeshO>::CompactFaceVector(sourceMesh); 
		vcg::tri::Allocator<CMeshO>::CompactVertexVector(targetMesh); 
		vcg::tri::Allocator<CMeshO>::CompactFaceVector(targetMesh);

		float percentage = par.getDynamicFloat("PercentMorph")/100.f;
		
		int i;
		for(i=0;i<targetMesh.vn;++i)
		{
			CMeshO::CoordType &srcP =sourceMesh.vert[i].P();  
			CMeshO::CoordType &trgP =targetMesh.vert[i].P();  
			srcP = srcP + (trgP-srcP)*percentage;
		}
	
		tri::UpdateNormals<CMeshO>::PerVertexPerFace(sourceMesh);
	} break;
	default : assert(0);
	}

	
				
	return true;
}


Q_EXPORT_PLUGIN(FilterUnsharp)
