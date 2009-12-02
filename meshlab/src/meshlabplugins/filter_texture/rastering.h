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

#ifndef _RASTERING_H
#define _RASTERING_H

#include <QtGui>
#include <meshlab/interfaces.h>
#include <vcg/complex/trimesh/point_sampling.h>
#include <vcg/space/triangle2.h>

class RasterSampler
{
	QImage &trgImg;
	
	// Callback stuff
	vcg::CallBackPos *cb;
	const CMeshO::FaceType *currFace;
	int faceNo, faceCnt, start, offset;
	
public:
	RasterSampler(QImage &_img) : trgImg(_img) {}
	
	void InitCallback(vcg::CallBackPos *_cb, int _faceNo, int _start=0, int _offset=100)
	{
		assert(_faceNo > 0);
		assert(_start>=0);
		assert(_offset>=0 && _offset <= 100-_start);
		cb = _cb;
		faceNo = _faceNo;
		faceCnt = 0;
		start = _start;
		offset = _offset;
		currFace = NULL;
	}
	
	// expects points outside face (face affecting) with baycentric coords = (bX, bY, -<distance from closest edge>)
	void AddTextureSample(const CMeshO::FaceType &f, const CMeshO::CoordType &p, const vcg::Point2i &tp)
	{
		CMeshO::VertexType::ColorType c;
		CMeshO::CoordType bary = p;
		int alpha = 255;
		if (fabs(p[0]+p[1]+p[2]-1)>=0.00001)
			if (p[0] <.0) {alpha = 254+p[0]*128; bary[0] = 0.;} else
				if (p[1] <.0) {alpha = 254+p[1]*128; bary[1] = 0.;} else
					if (p[2] <.0) {alpha = 254+p[2]*128; bary[2] = 0.;}
		if (alpha==255 || qAlpha(trgImg.pixel(tp.X(), trgImg.height() - tp.Y())) < alpha)
		{
			c.lerp(f.V(0)->cC(), f.V(1)->cC(), f.V(2)->cC(), bary);
			trgImg.setPixel(tp.X(), trgImg.height() - tp.Y(), qRgba(c[0], c[1], c[2], alpha));
		}
		if (cb)
		{
			if (&f != currFace) {currFace = &f; ++faceCnt;}
			cb(start + faceCnt*offset/faceNo, "Rasterizing faces ...");
		}
	}
};

class TransferColorSampler
{
	typedef vcg::GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;
	
	QImage &trgImg;
	QImage *srcImg;
	float dist_upper_bound;
	bool fromTexture;
	MetroMeshGrid unifGridFace;
	
	// Callback stuff
	vcg::CallBackPos *cb;
	const CMeshO::FaceType *currFace;
	int faceNo, faceCnt, start, offset;
	
	typedef vcg::tri::FaceTmark<CMeshO> MarkerFace;
	MarkerFace markerFunctor;
	
public:
	TransferColorSampler(CMeshO &_srcMesh, QImage &_trgImg, float upperBound)
	: trgImg(_trgImg), dist_upper_bound(upperBound)
	{
		unifGridFace.Set(_srcMesh.face.begin(),_srcMesh.face.end());
		markerFunctor.SetMesh(&_srcMesh);
		fromTexture = false;
	}
	
	TransferColorSampler(CMeshO &_srcMesh, QImage &_trgImg, QImage *_srcImg, float upperBound)
	: trgImg(_trgImg), dist_upper_bound(upperBound)
	{
		assert(_srcImg != NULL);
		srcImg = _srcImg;
		unifGridFace.Set(_srcMesh.face.begin(),_srcMesh.face.end());
		markerFunctor.SetMesh(&_srcMesh);
		fromTexture = true;
	}
	
	void InitCallback(vcg::CallBackPos *_cb, int _faceNo, int _start=0, int _offset=100)
	{
		assert(_faceNo > 0);
		assert(_start>=0);
		assert(_offset>=0 && _offset <= 100-_start);
		cb = _cb;
		faceNo = _faceNo;
		faceCnt = 0;
		start = _start;
		offset = _offset;
		currFace = NULL;
	}
	// expects points outside face (face affecting) with baycentric coords = (bX, bY, -<distance from closest edge instead of 0>) 
	void AddTextureSample(const CMeshO::FaceType &f, const CMeshO::CoordType &p, const vcg::Point2i &tp)
	{
		// Calculate correct brycentric coords
		CMeshO::CoordType bary = p;
		int alpha = 255;
		if (fabs(p[0]+p[1]+p[2]-1)>=0.00001)
			if (p[0] <.0) {alpha = 254+p[0]*128; bary[0] = 0.;} else
				if (p[1] <.0) {alpha = 254+p[1]*128; bary[1] = 0.;} else
					if (p[2] <.0) {alpha = 254+p[2]*128; bary[2] = 0.;}
		
		// Get point on face
		CMeshO::CoordType startPt;
		startPt[0] = bary[0]*f.V(0)->P().X()+bary[1]*f.V(1)->P().X()+bary[2]*f.V(2)->P().X();
		startPt[1] = bary[0]*f.V(0)->P().Y()+bary[1]*f.V(1)->P().Y()+bary[2]*f.V(2)->P().Y();
		startPt[2] = bary[0]*f.V(0)->P().Z()+bary[1]*f.V(1)->P().Z()+bary[2]*f.V(2)->P().Z();
		
		// Retrieve closest point on source mesh
		CMeshO::CoordType closestPt;
		vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
		float dist=dist_upper_bound;
		CMeshO::FaceType *nearestF;
		nearestF =  unifGridFace.GetClosest(PDistFunct, markerFunctor, startPt, dist_upper_bound, dist, closestPt);
		if (dist == dist_upper_bound) return;
		
		// Convert point to barycentric coords
		vcg::Point3f interp;
		int axis = 0;
		float tmp = -1;
		for (int i=0; i<3; ++i)
			if (fabs(nearestF->cN()[i]) > tmp) {tmp = fabs(nearestF->cN()[i]); axis = i;}
		bool ret = InterpolationParameters(*nearestF, axis, closestPt, interp);
		assert(ret);
		interp[2]=1.0-interp[1]-interp[0];
		
		if (fromTexture)
		{
			// NOT IMPLEMENTED YET
		}
		else
		{	
			// Calculate and set color 
			if (alpha==255 || qAlpha(trgImg.pixel(tp.X(), trgImg.height() - tp.Y())) < alpha)
			{
				CMeshO::VertexType::ColorType c;
				c.lerp(nearestF->V(0)->cC(), nearestF->V(1)->cC(), nearestF->V(2)->cC(), interp);
				trgImg.setPixel(tp.X(), trgImg.height() - tp.Y(), qRgba(c[0], c[1], c[2], alpha));
			}
		}
		
		if (cb)
		{
			if (&f != currFace) {currFace = &f; ++faceCnt;}
			cb(start + faceCnt*offset/faceNo, "Rasterizing faces ...");
		}
	}
};

template <class MetroMesh, class VertexSampler>
static void SingleFaceRasterWEdge(typename MetroMesh::FaceType &f,  VertexSampler &ps, 
								  const vcg::Point2<typename MetroMesh::ScalarType> & v0, 
								  const vcg::Point2<typename MetroMesh::ScalarType> & v1, 
								  const vcg::Point2<typename MetroMesh::ScalarType> & v2)
{
	typedef typename MetroMesh::ScalarType S;
	// Calcolo bounding box
	vcg::Box2i bbox;
	
	if(v0[0]<v1[0]) { bbox.min[0]=int(v0[0]); bbox.max[0]=int(v1[0]); }
	else            { bbox.min[0]=int(v1[0]); bbox.max[0]=int(v0[0]); }
	if(v0[1]<v1[1]) { bbox.min[1]=int(v0[1]); bbox.max[1]=int(v1[1]); }
	else            { bbox.min[1]=int(v1[1]); bbox.max[1]=int(v0[1]); }
	if(bbox.min[0]>int(v2[0])) bbox.min[0]=int(v2[0]);
	else if(bbox.max[0]<int(v2[0])) bbox.max[0]=int(v2[0]);
	if(bbox.min[1]>int(v2[1])) bbox.min[1]=int(v2[1]);
	else if(bbox.max[1]<int(v2[1])) bbox.max[1]=int(v2[1]);
	
	
	// Calcolo versori degli spigoli
	vcg::Point2<S> d10 = v1 - v0;
	vcg::Point2<S> d21 = v2 - v1;
	vcg::Point2<S> d02 = v0 - v2;
	
	// Preparazione prodotti scalari
	S b0  = (bbox.min[0]-v0[0])*d10[1] - (bbox.min[1]-v0[1])*d10[0];
	S b1  = (bbox.min[0]-v1[0])*d21[1] - (bbox.min[1]-v1[1])*d21[0];
	S b2  = (bbox.min[0]-v2[0])*d02[1] - (bbox.min[1]-v2[1])*d02[0];
	// Preparazione degli steps
	S db0 = d10[1];
	S db1 = d21[1];
	S db2 = d02[1];
	// Preparazione segni
	S dn0 = -d10[0];
	S dn1 = -d21[0];
	S dn2 = -d02[0];
	
	//Calcolo orientamento
	bool flipped = d02 * vcg::Point2<S>(-d10[1], d10[0]) >= 0;
	
	// Precalcolo Calcolo edge di bordo
	vcg::Segment2<S> borderEdges[3];
	S edgeLength[3];
	unsigned char edgeMask = 0;
	if (f.IsB(0)) {
		borderEdges[0] = vcg::Segment2<S>(v0, v1);
		edgeLength[0] = borderEdges[0].Length();
		edgeMask |= 1;
	}
	if (f.IsB(1)) {
		borderEdges[1] = vcg::Segment2<S>(v1, v2);
		edgeLength[1] = borderEdges[1].Length();
		edgeMask |= 2;
	}
	if (f.IsB(2)) {
		borderEdges[2] = vcg::Segment2<S>(v2, v0);
		edgeLength[2] = borderEdges[2].Length();
		edgeMask |= 4;
	}
	
	// Rasterizzazione
	double de = v0[0]*v1[1]-v0[0]*v2[1]-v1[0]*v0[1]+v1[0]*v2[1]-v2[0]*v1[1]+v2[0]*v0[1];
	
	for(int x=bbox.min[0]-1;x<=bbox.max[0]+1;++x)
	{
		bool in = false;
		S n[3]  = { b0-db0-dn0, b1-db1-dn1, b2-db2-dn2};
		for(int y=bbox.min[1]-1;y<=bbox.max[1]+1;++y)
		{
			if((n[0]>=0 && n[1]>=0 && n[2]>=0) || (n[0]<=0 && n[1]<=0 && n[2]<=0))
			{
				typename MetroMesh::CoordType baryCoord;
				baryCoord[0] =  double(-y*v1[0]+v2[0]*y+v1[1]*x-v2[0]*v1[1]+v1[0]*v2[1]-x*v2[1])/de;
				baryCoord[1] = -double( x*v0[1]-x*v2[1]-v0[0]*y+v0[0]*v2[1]-v2[0]*v0[1]+v2[0]*y)/de;
				baryCoord[2] = 1-baryCoord[0]-baryCoord[1];
				
				ps.AddTextureSample(f, baryCoord, vcg::Point2i(x,y));
				in = true;
			} else {
				// Check wheter a pixel outside (on a border edge side) triangle affects color inside it
				vcg::Point2<S> px(x, y);
				vcg::Point2<S> closePoint;
				int closeEdge = -1;
				S minDst = FLT_MAX;
				
				for (int i=0, t=0; t<2 && i<3 && (edgeMask>>i)%2 ; ++i)
				{
					vcg::Point2<S> close;
					S dst;
					if ( (flipped && n[i]<0 || !flipped && n[i]>0) &&
						(dst = ((close = ClosestPoint(borderEdges[i], px)) - px).Norm()) < minDst &&
						close.X() > px.X()-1 && close.X() < px.X()+1 &&
						close.Y() > px.Y()-1 && close.Y() < px.Y()+1)
					{
						minDst = dst;
						closePoint = close;
						closeEdge = i;
						++t;
					}
				}
				
				if (closeEdge >= 0)
				{
					// Add x,y sample with closePoint barycentric coords and -min
					typename MetroMesh::CoordType baryCoord;
					baryCoord[closeEdge] = (closePoint - borderEdges[closeEdge].P(1)).Norm()/edgeLength[closeEdge];
					baryCoord[(closeEdge+1)%3] = 1 - baryCoord[closeEdge];
					baryCoord[(closeEdge+2)%3] = -minDst;
					ps.AddTextureSample(f, baryCoord, vcg::Point2i(x,y));
					in = true;
				} else if (in) break;
			}
			n[0] += dn0;
			n[1] += dn1;
			n[2] += dn2;
		}
		b0 += db0;
		b1 += db1;
		b2 += db2;
	}
}

template <class MetroMesh, class VertexSampler>	
static void TextureCorrected(MetroMesh & m, VertexSampler &ps, int textureWidth, int textureHeight)
{
	typedef typename MetroMesh::FaceIterator FaceIterator;
	FaceIterator fi;
	
	printf("Similar Triangles face sampling\n");
	for(fi=m.face.begin(); fi != m.face.end(); fi++)
		if (!fi->IsD())
		{
			vcg::Point2f ti[3];
			for(int i=0;i<3;++i)
				ti[i]=vcg::Point2f((*fi).WT(i).U() * textureWidth - 0.5, (*fi).WT(i).V() * textureHeight + 0.5);
			//vcg::tri::SurfaceSampling<MetroMesh,VertexSampler>::SingleFaceRaster(*fi,  ps, ti[0],ti[1],ti[2]);
			SingleFaceRasterWEdge<MetroMesh,VertexSampler>(*fi,  ps, ti[0],ti[1],ti[2]);
		}
}

#endif