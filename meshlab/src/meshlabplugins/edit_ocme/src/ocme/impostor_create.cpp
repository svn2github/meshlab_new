
#include "impostor_definition.h"
#include "ocme_definition.h"

#include <GL/glew.h>
#include <vcg/space/box3.h>
#include <vcg/space/plane3.h>
#include <vcg/space/fitting3.h>
#include <vcg/complex/trimesh/clustering.h>
#include <wrap/gl/trimesh.h>

#include "cell.h"
#include "stdmatrix3.h"
#include "plane_box_intersection.h"
#include "plane_box_quantization.h"
#include "../utils/memory_debug.h"
#include "../utils/std_util.h"

 

//////////////////////////////////////////////////////////


// Init/clear those data structures needed for cumulating the data
void Impostor::InitDataCumulate( vcg::Box3f b){
	box = b;
	cellsize = (box.max[0]-box.min[0])/Gridsize();
};

void Impostor::ClearDataCumulate(){
	Delete(n_samples);
	Delete(normals);
	Delete(centroids);

};

//////////////////////////////////////////////////////////


void Impostor::ClearAll(){
		occupancy = 0;
		data_occupancy = 0;
		non_empty_children = 0;
	}


vcg::Point3<char> Impostor::Acc(vcg::Point3<char> p,vcg::Point3<char> d, unsigned int n){
		vcg::Point3<char> res;
		for(int i  = 0 ; i < 3; ++i)
				res[i] = std::min(127.f,std::max(-126.f, (((float) p[i]) * n + d[i])/(n+1)));
		return res;
}


void  Impostor::AddSample( vcg::Point3f  p,   vcg::Point3f   n){
	if(box.IsIn(p)){
		const int ii = std::min<int>( int((p[0]-box.min[0])/cellsize),Gridsize()-1);
		const int jj = std::min<int>( int((p[1]-box.min[1])/cellsize),Gridsize()-1);
		const int kk = std::min<int>( int((p[2]-box.min[2])/cellsize),Gridsize()-1);

		if(n_samples.IsOn(ii,jj,kk)){
				assert(n_samples.At(ii,jj,kk) != 0);
			centroids.At(ii,jj,kk) = Acc(centroids.At(ii,jj,kk),F2C(p,ii,jj,kk),n_samples.At(ii,jj,kk));
			normals.At(ii,jj,kk)   = Acc(normals.At(ii,jj,kk),F2C_01(n),n_samples.At(ii,jj,kk));
			++n_samples.At(ii,jj,kk);
		}
		else{
			n_samples.At(ii,jj,kk) = 1;
			centroids.At(ii,jj,kk)  =  F2C(p,ii,jj,kk);
			normals.At(ii,jj,kk) =  F2C_01(n);
		}

	}
}

void Impostor::AddSamplesFromImpostor(Impostor * ch){

		vcg::Point3f p,n;
		for(PointCellIterator pi  = ch->proxies.begin();pi != ch->proxies.end(); ++pi){
				ch->GetPointNormal(*pi,p,n);
			this->AddSample(p,n);
		}

}


void Impostor::Create(OCME*o,CellKey ck){
		std::map<unsigned short,vcg::Point3<char> >::iterator pi,ni;
		pi = this->centroids.data.begin();
		ni = this->normals.data.begin();

		for(; pi != this->centroids.data.end(); ++pi,++ni )
				proxies.push_back ( PointCell((*pi).first,PointNormal((*pi).second,(*ni).second)) );
}


void Impostor::SetCentroids( bool and_data_occupancy){
	cellsize = (box.max[0]-box.min[0])/Gridsize();

	if(and_data_occupancy)
		data_occupancy = 0;

		for(unsigned int i = 0; i < Gridsize(); ++i)
			for(unsigned int j = 0; j < Gridsize(); ++j)
				for(unsigned int k = 0; k < Gridsize(); ++k) 
					if(n_samples.IsOn(i,j,k))
				{
					if(and_data_occupancy) ++data_occupancy;
//					centroids.At(i,j,k) = Div(centroids.At(i,j,k), n_samples.cAt(i,j,k));
//					normals.At(i,j,k)   = Div(normals.At(i,j,k) ,  n_samples.cAt(i,j,k)) ;
				}

	if(and_data_occupancy){
		if(data_occupancy/float(Gridsize()*Gridsize()*Gridsize()) > occupancy) 
			occupancy = float(Gridsize()*Gridsize()*Gridsize());
	}

}


void	Impostor::ComputeDataOccupancy(){
	data_occupancy = 0;

	for(unsigned int i = 0; i < Gridsize(); ++i)
		for(unsigned int j = 0; j < Gridsize(); ++j)
			for(unsigned int k = 0; k < Gridsize(); ++k) 
				if(n_samples.IsOn(i,j,k)  )
					data_occupancy++;
}


void  Impostor::AddSamples(std::vector<vcg::Point3f> & smp){

	// stupid first approximation, just take the barycenters of all the faces
	std::vector<vcg::Point3f>::iterator si;
	for(si = smp.begin(); si != smp.end(); ++si){
		vcg::Point3f  & p = *si;
		if(box.IsIn(p)){
			const int ii = std::min<int>( int((p[0]-box.min[0])/cellsize),Gridsize()-1);
			const int jj = std::min<int>( int((p[1]-box.min[1])/cellsize),Gridsize()-1);
			const int kk = std::min<int>( int((p[2]-box.min[2])/cellsize),Gridsize()-1);
assert(0*ii*jj*kk);
		}
	}
}




/* serialization */

int Impostor::SizeOf(){
	unsigned int siz  = sizeof(unsigned int )  +sizeof(vcg::Box3f) + sizeof(unsigned int);
	for(unsigned int i = 0; i < Gridsize(); ++i)
		for(unsigned int j = 0; j < Gridsize(); ++j)
			for(unsigned int k = 0; k < Gridsize(); ++k) 
				if( n_samples.IsOn(i,j,k) )
					siz+= 6 *sizeof(char) + 3 * sizeof(unsigned char);
	return siz;
};


char * Impostor::Serialize (char * ptr ){
//	vcg::Plane3f riprova,ori;
	char buf[3];
	*(unsigned int*)ptr  = Gridsize();										ptr	+=	sizeof(unsigned int);
	memcpy(ptr,&box,sizeof(vcg::Box3f));									ptr +=	sizeof(vcg::Box3f);
	 *(float * )ptr = data_occupancy;										ptr +=  sizeof(unsigned int);
	for(unsigned int i = 0; i < Gridsize(); ++i)
		for(unsigned int j = 0; j < Gridsize(); ++j)
			for(unsigned int k = 0; k < Gridsize(); ++k) 
				if(n_samples.IsOn(i,j,k)){
					*(unsigned char * ) ptr = i; ptr += sizeof(unsigned char);
					*(unsigned char * ) ptr = j; ptr += sizeof(unsigned char);
					*(unsigned char * ) ptr = k; ptr += sizeof(unsigned char);

					memcpy(ptr,&centroids.At( i,j,k)[0],3*sizeof(char)); ptr+= 3*sizeof(char);
					memcpy(ptr,&normals.At( i,j,k)[0],3*sizeof(char)); ptr+= 3*sizeof(char);

				}
			
			return ptr;
};

char * Impostor::DeSerialize (char * ptr){
		char buf[3];
		unsigned int i,j,k;
		Impostor::Gridsize() = *(unsigned int*)ptr ;				ptr	+=	sizeof(unsigned int);
		memcpy(&box,ptr,sizeof(vcg::Box3f));									ptr +=	sizeof(vcg::Box3f);
		//InitDataPlanes();
		InitDataCumulate(box);
		data_occupancy = *(float * ) ptr;		ptr +=  sizeof(float);
		occupancy = data_occupancy/float(Gridsize()*Gridsize()*Gridsize());
 		for(unsigned int ii  = 0; ii < data_occupancy;++ii){
					i = *(unsigned char * ) ptr ; ptr += sizeof(unsigned char);
					j = *(unsigned char * ) ptr ; ptr += sizeof(unsigned char);
					k = *(unsigned char * ) ptr ; ptr += sizeof(unsigned char);

					memcpy(&centroids.At( i,j,k)[0],ptr,3*sizeof(char)); ptr+= 3*sizeof(char);
					memcpy(&normals.At( i,j,k)[0],ptr,3*sizeof(char)); ptr+= 3*sizeof(char);
					n_samples.At(i,j,k) = 1;
		}
		cellsize = (box.max[0]-box.min[0])/Gridsize();
//		ClearDataPlanes(); ////// DECOMMENT THIS!!!


		return ptr;
};


 
