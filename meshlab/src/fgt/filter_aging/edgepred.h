#ifndef EDGEPRED_H_
#define EDGEPRED_H_


#include <vector>

#include <meshlab/meshmodel.h>


using namespace vcg;


class QualityEdgePred
{
	public:
		QualityEdgePred() {
			lenp = EdgeLen<CMeshO, CMeshO::ScalarType>();
			lenp.thr2 = 1.0;
			this->qthVal = 1.0;
			this->selection = false;
			selbit = -1;
		}
		
		QualityEdgePred(bool sel, float lenTh, float qthVal) {
			lenp = EdgeLen<CMeshO, CMeshO::ScalarType>();
			lenp.thr2 = lenTh * lenTh;
			this->qthVal = qthVal;
			this->selection = sel;
			selbit = -1;
		}
		
		// Main method to test predicates over edges (used by RefineE function)
		bool operator()(face::Pos<CMeshO::FaceType> ep) const {
			// special case: this avoids to refine an edge between two faces
			// that have been selected by the dilate selection function
			if(selection && selbit!=-1 && !ep.f->IsUserBit(selbit) && !ep.f->FFp(ep.z)->IsUserBit(selbit)) 
				return false;
			// no special cases: normal predicate test
			return (lenp(ep) && testQuality(ep));
		}
		
		// Tests quality predicate but only on the first of the two vertexes of the edge
		bool qVertTest(face::Pos<CMeshO::FaceType> ep) const {
			return (ep.f->V(ep.z)->Q() > qthVal);
		}
		
		// Allocates two new user bits over the mesh faces
		void allocateSelBit() {
			selbit = CFaceO::NewBitFlag();
		}
		
		// Clear sel user bit on all the mesh faces
		void clearSelBit(CMeshO &m) {
			if(selbit == -1) return;
			for(CMeshO::FaceIterator fi = m.face.begin(); fi!=m.face.end(); fi++) 
				if(!(*fi).IsD()) (*fi).ClearUserBit(selbit);
		}
		
		// Set sel bit on face f 
		void setFaceSelBit(CFaceO* f) {
			if(selbit == -1) return;
			f->SetUserBit(selbit);
		}
		
		// Deallocates the two user bits from the mesh faces
		void deallocateSelBit() {
			CFaceO::DeleteBitFlag(selbit);
			selbit = -1;
		}
		
	protected:
		EdgeLen<CMeshO, CMeshO::ScalarType> lenp;	// edge len predicate object
		float qthVal;			// edge quality threshold value
		bool selection;		// working on the selection only
		int selbit;			// face user bit used when working on selections
		
		bool testQuality(face::Pos<CMeshO::FaceType> ep) const {
			return (ep.f->V(ep.z)->Q() > qthVal || ep.f->V1(ep.z)->Q() > qthVal);
		}
};


class AngleEdgePred 
{
	public:
		AngleEdgePred(float thVal) {
			this->athVal = thVal;
		}
		
		// tests angle predicate over current edge
		bool operator()(face::Pos<CMeshO::FaceType> ep) const {
			if(ep.f->IsB(ep.z)) return true;

			// the angle between the two face normals in degrees
			double ffangle = math::ToDeg(vcg::Angle(ep.f->N(), ep.f->FFp(ep.z)->N()));

			Point3<CVertexO::ScalarType> edge;		// vector along edge
			double seAngle;		// angle between the edge vector and the normal of one of the two faces
			if(ep.f->V(ep.z) < ep.f->V1(ep.z)) {
				edge = Point3<CVertexO::ScalarType>(ep.f->V1(ep.z)->P() - ep.f->V(ep.z)->P());
				seAngle = math::ToDeg(vcg::Angle(edge ^ ep.f->N(), ep.f->FFp(ep.z)->N()));
			}
			else {
				edge = Point3<CVertexO::ScalarType>(ep.f->V(ep.z)->P() - ep.f->V1(ep.z)->P());
				seAngle = math::ToDeg(vcg::Angle(edge ^ ep.f->FFp(ep.z)->N(), ep.f->N()));
			}

			/*  There are always 2 cases wich produce the same angle value:
								 ___|_		    ____
								|			   |  |
							   -|			   |-
				
				In the first case the edge lies in a concave area of the mesh
				while in the second case it lies in a convex area.
				We need a way to know wich is the current case.
				This is done comparing seAngle with thVal.
			*/
			return (ffangle-athVal >= -0.001  && seAngle-athVal <= 0.001);
		}
		
	protected:
		float athVal;			// edge angle threshold value
};


#endif /*EDGEPRED_H_*/
