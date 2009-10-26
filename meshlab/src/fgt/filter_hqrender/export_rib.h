#ifndef __VCGLIB_EXPORT_RIB
#define __VCGLIB_EXPORT_RIB

//#include<wrap/ply/io_mask.h>
#include<wrap/io_trimesh/io_mask.h>
#include<wrap/callback.h>
//#include<vcg/container/simple_temporary_data.h>


#include <stdio.h>

namespace vcg {
namespace tri {
namespace io {


template <class SaveMeshType>
class ExporterRIB
{

public:
typedef typename SaveMeshType::VertexPointer VertexPointer;
typedef typename SaveMeshType::ScalarType ScalarType;
typedef typename SaveMeshType::VertexType VertexType;
typedef typename SaveMeshType::FaceType FaceType;
typedef typename SaveMeshType::FacePointer FacePointer;
typedef typename SaveMeshType::VertexIterator VertexIterator;
typedef typename SaveMeshType::FaceIterator FaceIterator;


static int Save(SaveMeshType &m,  const char * filename, bool binary, CallBackPos *cb=0) {
	return Save(m,filename, Mask::IOM_ALL, binary, cb);
}

static int Save(SaveMeshType &m,  const char * filename, int savemask, bool binary, CallBackPos *cb=0)
{
	//ignore binary for now!
  
  FILE *fout = fopen(filename,"wb");
  if(fout==NULL) {
    //pi.status=::vcg::ply::E_CANTOPEN;
	//return ::vcg::ply::E_CANTOPEN;
  }

  QTime tt; tt.start(); //debug

  fprintf(fout,"#generated by VCGLIB\n"); //commento d'intestazione????

  //initial declaring
  if(m.HasPerVertexColor() && (savemask & Mask::IOM_VERTCOLOR))
    fprintf(fout,"Declare \"Cs\" \"facevarying color\"\n");

  if((HasPerVertexTexCoord(m) && (savemask & Mask::IOM_VERTTEXCOORD)) || 
	  (HasPerWedgeTexCoord(m) && (savemask & Mask::IOM_WEDGTEXCOORD)))
    fprintf(fout,"Declare \"st\" \"facevarying float[2]\"\n");
  
  if(HasPerVertexNormal(m) && (savemask & Mask::IOM_VERTNORMAL))
    fprintf(fout,"Declare \"N\" \"facevarying normal\"\n");


  Allocator<CMeshO>::CompactVertexVector(m);
  Allocator<CMeshO>::CompactFaceVector(m);

  //first step: faces topology
  fprintf(fout,"PointsPolygons\n[\n");
  for(int i=0; i<m.fn; i++) {
    fprintf(fout,"3 ");//\n");
  }
  fprintf(fout,"\n");
  fprintf(fout,"]\n[\n");
  qDebug("PointsPolygons %i",tt.elapsed());

  //second step: index of vertex for face
  UpdateFlags<CMeshO>::VertexClearV(m);
  for(CMeshO::FaceIterator fi=m.face.begin(); fi!=m.face.end(); ++fi) {
    for(int j=0; j<3; ++j) {						
      int indexOfVertex = (*fi).V(j) - &(m.vert[0]);
	  fprintf(fout,"%i ",indexOfVertex);
	  //if it's the first visit, set visited bit
	  if(!(*fi).V(j)->IsV()) {
		(*fi).V(j)->SetV();
	  }
	}
	//fprintf(fout,"\n");
	fprintf(fout," ");
  }
  fprintf(fout,"\n]\n");
  qDebug("coords %i",tt.elapsed());

  //third step: vertex coordinates
  fprintf(fout,"\"P\"\n[\n");  
  for(CMeshO::VertexIterator vi=m.vert.begin(); vi!=m.vert.end(); ++vi) {
    if(vi->IsV()) {
	  Point3f p = vi->P();
	  //fprintf(fout,"%g %g %g\n",p[0],p[1],p[2]);
	  fprintf(fout,"%g %g %g ",p[0],p[2],p[1]);
	}
  }
  fprintf(fout,"\n]\n");
  qDebug("coords %i",tt.elapsed());

  //fourth step: vertex normal
  if(HasPerVertexNormal(m) && (savemask & Mask::IOM_VERTNORMAL)) {
    fprintf(fout,"\"N\"\n[\n");
    for(CMeshO::FaceIterator fi=m.face.begin(); fi!=m.face.end(); ++fi) {
	  //for each face, foreach vertex write normal
	  for(int j=0; j<3; ++j) {			
	    Point3f &n=(*fi).V(j)->N();
		fprintf(fout,"%g %g %g ",n[0],n[1],n[2]);
	  }
	}
	fprintf(fout,"\n]\n");
	qDebug("normal %i",tt.elapsed());
  }

  //fifth step: vertex color (ignore face color?)
  if(m.HasPerVertexColor() && (savemask & Mask::IOM_VERTCOLOR)) {
    fprintf(fout,"\"Cs\"\n[\n");
	for(CMeshO::FaceIterator fi=m.face.begin(); fi!=m.face.end(); ++fi) {
	  //for each face, foreach vertex write color
	  for(int j=0; j<3; ++j) {
	    Color4b &c=(*fi).V(j)->C();
		//fprintf(fout,"%g %g %g\n",float(c[0])/255,float(c[1])/255,float(c[2])/255);
		fprintf(fout,"%g %g %g ",float(c[0])/255,float(c[1])/255,float(c[2])/255);
	  }
	}
	fprintf(fout,"\n]\n");
	qDebug("color %i",tt.elapsed());
  }

  //sixth step: texture coordinates (for edge)
  if((HasPerVertexTexCoord(m) && (savemask & Mask::IOM_VERTTEXCOORD)) || 
	  (HasPerWedgeTexCoord(m) && (savemask & Mask::IOM_WEDGTEXCOORD))) {
	fprintf(fout,"\"st\"\n[\n");
	for(CMeshO::FaceIterator fi=m.face.begin(); fi!=m.face.end(); ++fi) {
	  //for each face, foreach vertex write uv coord
	  for(int j=0; j<3; ++j) {
	    //fprintf(fout,"%g %g\n",(*fi).WT(j).U(),(*fi).WT(j).V());
		fprintf(fout,"%g %g ",(*fi).WT(j).U(),(*fi).WT(j).V());
	  }
	}
	fprintf(fout,"\n]\n");
	qDebug("texcoords %i",tt.elapsed());
  }

  fclose(fout);

  return 0;
}

static const char *ErrorMsg(int error)
{/*
  static std::vector<std::string> ply_error_msg;
  if(ply_error_msg.empty())
  {
    ply_error_msg.resize(PlyInfo::E_MAXPLYINFOERRORS );
    ply_error_msg[ply::E_NOERROR				]="No errors";
	  ply_error_msg[ply::E_CANTOPEN				]="Can't open file";
    ply_error_msg[ply::E_NOTHEADER ]="Header not found";
	  ply_error_msg[ply::E_UNESPECTEDEOF	]="Eof in header";
	  ply_error_msg[ply::E_NOFORMAT				]="Format not found";
	  ply_error_msg[ply::E_SYNTAX				]="Syntax error on header";
	  ply_error_msg[ply::E_PROPOUTOFELEMENT]="Property without element";
	  ply_error_msg[ply::E_BADTYPENAME		]="Bad type name";
	  ply_error_msg[ply::E_ELEMNOTFOUND		]="Element not found";
	  ply_error_msg[ply::E_PROPNOTFOUND		]="Property not found";
	  ply_error_msg[ply::E_BADTYPE				]="Bad type on addtoread";
	  ply_error_msg[ply::E_INCOMPATIBLETYPE]="Incompatible type";
	  ply_error_msg[ply::E_BADCAST				]="Bad cast";

    ply_error_msg[PlyInfo::E_NO_VERTEX      ]="No vertex field found";
    ply_error_msg[PlyInfo::E_NO_FACE        ]="No face field found";
	  ply_error_msg[PlyInfo::E_SHORTFILE      ]="Unespected eof";
	  ply_error_msg[PlyInfo::E_NO_3VERTINFACE ]="Face with more than 3 vertices";
	  ply_error_msg[PlyInfo::E_BAD_VERT_INDEX ]="Bad vertex index in face";
	  ply_error_msg[PlyInfo::E_NO_6TCOORD     ]="Face with no 6 texture coordinates";
	  ply_error_msg[PlyInfo::E_DIFFER_COLORS  ]="Number of color differ from vertices";
  }
*/
  static const char * dae_error_msg[] =
		{
			"No errors"
		};

		if(error>0 || error<0) return "Unknown error";
		else return dae_error_msg[error];

 
};

  static int GetExportMaskCapability()
  {
	  int capability = 0;			
	  capability |= vcg::tri::io::Mask::IOM_VERTCOORD    ;
	  //capability |= vcg::tri::io::Mask::IOM_VERTFLAGS    ;
	  capability |= vcg::tri::io::Mask::IOM_VERTCOLOR    ;
	  //capability |= vcg::tri::io::Mask::IOM_VERTQUALITY  ;
	  capability |= vcg::tri::io::Mask::IOM_VERTNORMAL   ;
	  //capability |= vcg::tri::io::Mask::IOM_VERTRADIUS   ;
	  capability |= vcg::tri::io::Mask::IOM_VERTTEXCOORD ;
	  //capability |= vcg::tri::io::Mask::IOM_FACEINDEX    ;
	  //capability |= vcg::tri::io::Mask::IOM_FACEFLAGS    ;
	  //capability |= vcg::tri::io::Mask::IOM_FACECOLOR    ;
	  //capability |= vcg::tri::io::Mask::IOM_FACEQUALITY  ;
	  // capability |= vcg::tri::io::Mask::IOM_FACENORMAL   ;
	  capability |= vcg::tri::io::Mask::IOM_WEDGCOLOR    ;
	  capability |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD ;
	  capability |= vcg::tri::io::Mask::IOM_WEDGTEXMULTI ;
    //capability |= vcg::tri::io::Mask::IOM_WEDGNORMAL   ;
	//	capability |= vcg::tri::io::Mask::IOM_CAMERA   ;
	  return capability;
  }


}; // end class



} // end namespace tri
} // end namespace io
} // end namespace vcg
//@}
#endif
