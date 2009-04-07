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


****************************************************************************/

#include <Qt>
#include <QtGui>
#include "filter_qhull.h"


#include <math.h>
#include <stdlib.h>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>

#include "qhull_tools.h"

using namespace std;
using namespace vcg;

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

QhullPlugin::QhullPlugin() 
{ 
	typeList << FP_QHULL_CONVEX_HULL  
			 <<	FP_QHULL_DELAUNAY_TRIANGULATION
			 <<	FP_QHULL_VORONOI_FILTERING
			 << FP_QHULL_ALPHA_COMPLEX_AND_SHAPE
			 << FP_QHULL_VISIBLE_POINTS;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

QhullPlugin::~QhullPlugin()
{
	for (int i = 0; i < actionList.count() ; i++ )
		delete actionList.at(i);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
const QString QhullPlugin::filterName(FilterIDType filterId) 
{
  switch(filterId) {
		case FP_QHULL_CONVEX_HULL :  return QString("Convex Hull"); 
		case FP_QHULL_DELAUNAY_TRIANGULATION :  return QString("Delaunay Triangulation"); 
		case FP_QHULL_VORONOI_FILTERING :  return QString("Voronoi Filtering"); 
		case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE : return QString("Alpha Complex/Shape");
		case FP_QHULL_VISIBLE_POINTS: return QString("Select Visible Points");
		default : assert(0); 
	}
  return QString("Error: Unknown Filter"); 
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString QhullPlugin::filterInfo(FilterIDType filterId)
{
  switch(filterId) {
		case FP_QHULL_CONVEX_HULL :  return QString("Calculate mesh convex hull with Qhull library.<br> "
										 "The convex hull of a set of points is the smallest convex set containing the points."); 
		case FP_QHULL_DELAUNAY_TRIANGULATION :  return QString("Calculate mesh Delaunay triangulations with Qhull library.<br>"
													"The Delaunay triangulation of a set of points in d-dimensional spaces is "
													"the projection of the convex hull of the projections of the points onto a "
													"(d+1)-dimensional paraboloid. <br>"); 
		case FP_QHULL_VORONOI_FILTERING :  return QString("Compute mesh Voronoi filtering with Qhull library. <br>" 
											   "The filter reconstructs the mesh surface starting from its vertices through a double "
											   "Delaunay triangulation. <br> The second one takes in input some Voronoi vertices too.<br>"); 
		case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE: return QString("Calculate Alpha Shape. <br>"
										"The Alpha Shape is the boundary of the alpha complex, that is a subcomplex of the Delaunay triangulation.<br>" 
										"For a given value of 'alpha', the alpha complex includes all the simplices in the Delaunay "
										"triangulation which have an empty circumsphere with radius equal or smaller than 'alpha'");
		case FP_QHULL_VISIBLE_POINTS: return QString("Select points visible from camera viewpoint");
		default : assert(0); 
	}
	return QString("Error: Unknown Filter");
}

// The FilterClass describes in which generic class of filters it fits. 
// This choice affect the submenu in which each filter will be placed 
// More than a single class can be choosen.
const QhullPlugin::FilterClass QhullPlugin::getClass(QAction *a)
{
  switch(ID(a))
	{
		case FP_QHULL_CONVEX_HULL :
		case FP_QHULL_DELAUNAY_TRIANGULATION :
		case FP_QHULL_VORONOI_FILTERING :
		case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE:
			return FilterClass (MeshFilterInterface::Remeshing) ; 
		case FP_QHULL_VISIBLE_POINTS:
			return FilterClass (MeshFilterInterface::Selection);
		default : assert(0); 
	}
  return FilterClass(0);
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void QhullPlugin::initParameterSet(QAction *action,MeshModel &m, FilterParameterSet & parlst) 
{
	 switch(ID(action))	 {
		case FP_QHULL_CONVEX_HULL :
			{
				break;
			}
		case FP_QHULL_DELAUNAY_TRIANGULATION :
			{
				break;
			}
		case FP_QHULL_VORONOI_FILTERING :
			{
				break;
			}	
		case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE:
			{
			    parlst.addAbsPerc("alpha",0,0,m.cm.bbox.Diag(),tr("Alpha value"),tr("Compute the alpha value as percentage of the diagonal of the bbox"));
				parlst.addEnum("Filtering", 0, 
									QStringList() << "Alpha Complex" << "Alpha Shape" , 
									tr("Get:"), 
									tr("Select the output. The Alpha Shape is the boundary of the Alpha Complex"));
				break;
			}
		case FP_QHULL_VISIBLE_POINTS:
			{
				parlst.addDynamicFloat("radiusThreshold",
												 0.0f, 0.0f, 7.0f,MeshModel::MM_VERTFLAGSELECT,
												 "radius threshold ","Bounds the radius oh the sphere used to select visible points. <br>"
												 "Use a big threshold for dense point clouds, a small one for sparse clouds");
				
				parlst.addBool ("usecamera",
												false,
												"Use ViewPoint from Mesh Camera",
												"Uses the ViewPoint from the camera associated to the current mesh\n if there is no camera, an error occurs");
				parlst.addPoint3f("viewpoint",
												Point3f(0.0f, 0.0f, 0.0f),
												"ViewPoint",
												"if UseCamera is true, this value is ignored");
				
				parlst.addBool("convex_hull",false,"Show Partial Convex Hull of flipped points", "Show Partial Convex Hull of the transformed point cloud");
				break;
			}
		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
bool QhullPlugin::applyFilter(QAction *filter, MeshDocument &md, FilterParameterSet & par, vcg::CallBackPos *cb)
{
	switch(ID(filter))
	{
		case FP_QHULL_CONVEX_HULL :
			{
				MeshModel &m=*md.mm();
			    MeshModel &pm =*md.addNewMesh("Convex Hull");
				
				if (m.hasDataMask(MeshModel::MM_WEDGTEXCOORD)){
					m.clearDataMask(MeshModel::MM_WEDGTEXCOORD);
				}
				if (m.hasDataMask(MeshModel::MM_VERTTEXCOORD)){
					m.clearDataMask(MeshModel::MM_VERTTEXCOORD);
				}
			    
				int dim= 3;				/* dimension of points */
				int numpoints= m.cm.vn;	/* number of mesh vertices */
				
				//facet_list contains the convex hull  
				//facet_list contains simplicial (triangulated) facets.
				//The convex hull of a set of points is the smallest convex set containing the points.

				facetT *facet_list = compute_convex_hull(dim,numpoints,m);

				if(facet_list!=NULL){

					int convexNumFaces = qh num_facets;
					int convexNumVert = qh_setsize(qh_facetvertices (facet_list, NULL, false));
					assert( qh num_vertices == convexNumVert);

					tri::Allocator<CMeshO>::AddVertices(pm.cm,convexNumVert);
					tri::Allocator<CMeshO>::AddFaces(pm.cm,convexNumFaces);


					/*ivp length is 'numpoints' because each vertex is accessed through its ID whose range is 
					  0<=qh_pointid(vertex->point)<numpoints. qh num_vertices is < numpoints*/
					vector<tri::Allocator<CMeshO>::VertexPointer> ivp(numpoints);
					vertexT *vertex;
					int i=0;
					FORALLvertices{	
						if ((*vertex).point){
							pm.cm.vert[i].P()[0] = (*vertex).point[0];
							pm.cm.vert[i].P()[1] = (*vertex).point[1];
							pm.cm.vert[i].P()[2] = (*vertex).point[2];
							ivp[qh_pointid(vertex->point)] = &pm.cm.vert[i];
							i++;
						}
					}
	 
					facetT *facet;
					i=0;
					FORALLfacet_(facet_list){		
						vertexT *vertex;
						int vertex_n, vertex_i;
						FOREACHvertex_i_((*facet).vertices){
							pm.cm.face[i].V(vertex_i)= ivp[qh_pointid(vertex->point)];	
						}
						i++;
					}

					assert( pm.cm.fn == convexNumFaces);
					Log(GLLogStream::FILTER,"Successfully created a mesh of %i vert and %i faces",convexNumVert,convexNumFaces);
					//m.cm.Clear();

					vcg::tri::UpdateBounding<CMeshO>::Box(pm.cm);
					vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(pm.cm);
					
					int curlong, totlong;	  /* memory remaining after qh_memfreeshort */
					qh_freeqhull(!qh_ALL);  
					qh_memfreeshort (&curlong, &totlong);
					if (curlong || totlong)
						fprintf (stderr, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n", 
									 totlong, curlong);

					return true;

				}
				else
					return false;
			}
		case FP_QHULL_DELAUNAY_TRIANGULATION:
			{
				MeshModel &m=*md.mm();
			    MeshModel &pm =*md.addNewMesh("Delaunay Triangulation");
				
				if (m.hasDataMask(MeshModel::MM_WEDGTEXCOORD)){
					m.clearDataMask(MeshModel::MM_WEDGTEXCOORD);
				}
				if (m.hasDataMask(MeshModel::MM_VERTTEXCOORD)){
					m.clearDataMask(MeshModel::MM_VERTTEXCOORD);
				}
			    
				int dim= 3;				/* dimension of points */
				int numpoints= m.cm.vn;	/* number of mesh vertices */
				
				//facet_list contains the Delaunauy triangulation as a list of tetrahedral facets */ 	
				facetT *facet_list = compute_delaunay(dim,numpoints,m);

				if(facet_list!=NULL){

					int convexNumVert = qh_setsize(qh_facetvertices (facet_list, NULL, false));
					assert( qh num_vertices == convexNumVert);

					tri::Allocator<CMeshO>::AddVertices(pm.cm,convexNumVert);

					vector<tri::Allocator<CMeshO>::VertexPointer> ivp(qh num_vertices);
					vertexT *vertex;
					int i=0;
					FORALLvertices{		
						if ((*vertex).point){
							pm.cm.vert[i].P()[0] = (*vertex).point[0];
							pm.cm.vert[i].P()[1] = (*vertex).point[1];
							pm.cm.vert[i].P()[2] = (*vertex).point[2];
							ivp[qh_pointid(vertex->point)] = &pm.cm.vert[i];
							i++;
						}
					}
					
					// In 3-d Delaunay triangulation each facet is a tetrahedron. If triangulated,
					//each ridge (d-1 vertices between two neighboring facets) is a triangle.

					facetT *facet, *neighbor;
					qh visit_id++;
					int ridgeCount=0;

					//Compute each ridge (triangle) once
					FORALLfacet_(facet_list)
					if (!facet->upperdelaunay) {
						facet->visitid= qh visit_id;
						qh_makeridges(facet);
						ridgeT *ridge, **ridgep;
						FOREACHridge_(facet->ridges) {
							neighbor= otherfacet_(ridge, facet);
							if (neighbor->visitid != qh visit_id) {
								tri::Allocator<CMeshO>::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(pm.cm,1);
								ridgeCount++;
								int vertex_n, vertex_i;
								FOREACHvertex_i_(ridge->vertices)
									(*fi).V(vertex_i)= ivp[qh_pointid(vertex->point)];
							}
						}
					}

					assert(pm.cm.fn == ridgeCount);
					Log(GLLogStream::FILTER,"Successfully created a mesh of %i vert and %i faces",pm.cm.vn,pm.cm.fn);
					//m.cm.Clear();	

					vcg::tri::UpdateBounding<CMeshO>::Box(pm.cm);
					vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(pm.cm);
					
					int curlong, totlong;	  /* memory remaining after qh_memfreeshort */
					qh_freeqhull(!qh_ALL);  
					qh_memfreeshort (&curlong, &totlong);
					if (curlong || totlong)
						fprintf (stderr, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n", 
									 totlong, curlong);
					return true;
				}
				else 
					return false;
			}
			case FP_QHULL_VORONOI_FILTERING:
			{
				MeshModel &m=*md.mm();
			    MeshModel &pm =*md.addNewMesh("Voronoi Filtering");
				
				if (m.hasDataMask(MeshModel::MM_WEDGTEXCOORD)){
					m.clearDataMask(MeshModel::MM_WEDGTEXCOORD);
				}
				if (m.hasDataMask(MeshModel::MM_VERTTEXCOORD)){
					m.clearDataMask(MeshModel::MM_VERTTEXCOORD);
				}
			    
				int dim= 3;				/* dimension of points */
				int numpoints= m.cm.vn;	/* number of mesh vertices */
 	
				bool result = compute_voronoi(dim,numpoints,m,pm);
				
				if(result){
					vcg::tri::UpdateBounding<CMeshO>::Box(pm.cm);
					vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(pm.cm);

					Log(GLLogStream::FILTER,"Successfully created a mesh of %i vert and %i faces",pm.cm.vn,pm.cm.fn);

					return true;
				}

				else return false;
			}
			case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE:
			{
				MeshModel &m=*md.mm();
				
				if (m.hasDataMask(MeshModel::MM_WEDGTEXCOORD)){
					m.clearDataMask(MeshModel::MM_WEDGTEXCOORD);
				}
				if (m.hasDataMask(MeshModel::MM_VERTTEXCOORD)){
					m.clearDataMask(MeshModel::MM_VERTTEXCOORD);
				}
				
				int dim= 3;				/* dimension of points */
				int numpoints= m.cm.vn;	/* number of mesh vertices */

				double alpha = par.getAbsPerc("alpha");

				bool alphashape = false;
				char* name = "";
				switch(par.getEnum("Filtering"))
				{
					case 0 :	
						alphashape=false;	
						name = ("Alpha Complex");
						break;
					case 1 :
						alphashape=true;
						name =("Alpha Shapes");
						break;
				}

				MeshModel &pm =*md.addNewMesh(name);

				if (!alphashape && !pm.hasDataMask(MeshModel::MM_FACEQUALITY))
				{
					pm.updateDataMask(MeshModel::MM_FACEQUALITY);
				}

				bool result =compute_alpha_shapes(dim,numpoints,m,pm,alpha,alphashape);

				if(result){
					vcg::tri::UpdateBounding<CMeshO>::Box(pm.cm);
					vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(pm.cm);

					Log(GLLogStream::FILTER,"Successfully created a mesh of %i vert and %i faces",pm.cm.vn,pm.cm.fn);
					Log(GLLogStream::FILTER,"Alpha = %f ",alpha);
					//m.cm.Clear();	

					return true;
				}
				else 
					return false;
			}
			case FP_QHULL_VISIBLE_POINTS:
			{
				MeshModel &m=*md.mm();
				if (!m.hasDataMask(MeshModel::MM_VERTCOLOR))
				{
					m.updateDataMask(MeshModel::MM_VERTCOLOR);
				}
				if (!m.hasDataMask(MeshModel::MM_VERTFLAGSELECT))
				{
					m.updateDataMask(MeshModel::MM_VERTFLAGSELECT);
				}
				
				int dim= 3;				/* dimension of points */
				int numpoints= m.cm.vn;	/* number of mesh vertices */

				bool usecam = par.getBool("usecamera");
				Point3f viewpoint = par.getPoint3f("viewpoint");
				float threshold = par.getDynamicFloat("radiusThreshold");

				// if usecamera but mesh does not have one
				if( usecam && !m.hasDataMask(MeshModel::MM_CAMERA) ) 
				{
					errorMessage = "Mesh has not a camera that can be used to compute view direction. Please set a view direction."; // text
					return false;
				}
				if(usecam)
				{
					viewpoint = m.cm.shot.GetViewPoint();
				}

			    MeshModel &pm =*md.addNewMesh("CH Flipped Points");
				
				if (m.hasDataMask(MeshModel::MM_WEDGTEXCOORD)){
					m.clearDataMask(MeshModel::MM_WEDGTEXCOORD);
				}
				if (m.hasDataMask(MeshModel::MM_VERTTEXCOORD)){
					m.clearDataMask(MeshModel::MM_VERTTEXCOORD);
				}


				bool convex_hull = par.getBool("convex_hull");
				int result = visible_points(dim,numpoints,m,pm,viewpoint,threshold,convex_hull);

				if(!convex_hull)
					md.delMesh(&pm);
				else{
					vcg::tri::UpdateBounding<CMeshO>::Box(pm.cm);
					vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(pm.cm);
					Log(GLLogStream::FILTER,"Successfully created a mesh of %i vert and %i faces",pm.cm.vn,pm.cm.fn);
				}
				
				if(result>=0){
					Log(GLLogStream::FILTER,"Selected %i visible points", result);
					return true;
				}

				else return false;
			}
	}
}

Q_EXPORT_PLUGIN(QhullPlugin)
