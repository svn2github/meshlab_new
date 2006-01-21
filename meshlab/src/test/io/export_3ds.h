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

/****************************************************************************
  History

 $Log$
 Revision 1.24  2006/01/21 15:19:51  fmazzant
 changed:
 inserting coord texture in to map from key = index, value=coord
 to key=coord, value=index_of_vertix.

 Revision 1.23  2006/01/20 14:15:52  fmazzant
 added texture filename on material 3ds and coordtexture on face

 Revision 1.22  2006/01/19 09:36:29  fmazzant
 cleaned up history log

 Revision 1.21  2006/01/18 16:14:52  fmazzant
 update small error

 Revision 1.20  2006/01/18 16:12:58  fmazzant
 updated material

 Revision 1.19  2006/01/18 15:09:45  fmazzant
 added texture base

 Revision 1.18  2006/01/18 14:57:26  fmazzant
 added Lib3dsNode in export_3ds

****************************************************************************/

#ifndef __VCGLIB_EXPORT_3DS
#define __VCGLIB_EXPORT_3DS

#include <wrap/callback.h>
#include <vcg/complex/trimesh/allocate.h>
#include <wrap/io_trimesh/io_mask.h>

#include <lib3ds/file.h>
#include <lib3ds/io.h>
#include <lib3ds/mesh.h>
#include <lib3ds/types.h>
#include <lib3ds/material.h>

#include <vector>
#include <iostream>
#include <fstream>

#include <QMessageBox>

namespace vcg {
namespace tri {
namespace io {


	template <class SaveMeshType>
	class Exporter3DS
	{
	public:	
		typedef typename SaveMeshType::FaceIterator FaceIterator;
		typedef typename SaveMeshType::VertexIterator VertexIterator;
		typedef typename SaveMeshType::VertexType VertexType;
	
		enum SaveError
		{
			E_NOERROR,					// 0
			E_CANTOPENFILE,				// 1
			E_CANTCLOSEFILE,			// 2
			E_UNESPECTEDEOF,			// 3
			E_ABORTED,					// 4
			E_NOTDEFINITION,			// 5
			E_NOTVEXTEXVALID,			// 6
			E_NOTFACESVALID,			// 7
			E_NOTEXCOORDVALID			// 8
		};

		/*
			stampa messaggio di errore dell'export obj
		*/
		static const char* ErrorMsg(int error)
		{
			static const char* obj_error_msg[] =
			{
					"No errors",							// 0
					"Can't open file",						// 1
					"can't close file",						// 2
					"Premature End of file",				// 3
					"File saving aborted",					// 4
					"Function not defined",					// 5
					"Vertices not valid",					// 6
					"Faces not valid"						// 7
					"Texture Coord not valid"				// 8
				};

			if(error>8 || error<0) return "Unknown error";
			else return obj_error_msg[error];
		};

		/*
			restituisce quali informazioni sono possibili salvare in base alla maschera
		*/
		static int GetExportMaskCapability()
		{
			int capability = 0;

			//camera
			//capability |= vcg::tri::io::Mask::IOM_CAMERA;

			//vert
			capability |= vcg::tri::io::Mask::IOM_VERTQUALITY;

			//face
			capability |= vcg::tri::io::Mask::IOM_FACEFLAGS;
			capability |= vcg::tri::io::Mask::IOM_FACECOLOR;
			capability |= vcg::tri::io::Mask::IOM_FACEQUALITY;
			capability |= vcg::tri::io::Mask::IOM_FACENORMAL;

			//wedg
			capability |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;

			return capability;
		}

		static int SaveASCII(SaveMeshType &m, const char * filename)	
		{
			return E_NOTDEFINITION;
		}

		static int SaveBinary(SaveMeshType &m, const char * filename, int &mask, CallBackPos *cb=0)
		{
			if(m.vert.size() == 0)
				return E_NOTVEXTEXVALID;
			if(m.face.size() == 0)
				return E_NOTFACESVALID;

			Lib3dsFile *file = lib3ds_file_new();//crea un nuovo file
			Lib3dsMesh *mesh = lib3ds_mesh_new("mesh");//crea una nuova mesh con nome mesh		

			std::vector<Material> materials;
			//std::map<vcg::TCoord2<float>,int> CoordTextures;
			std::map<int,vcg::TCoord2<float> > CoordTextures;
			
			int current = 0;
			int max = m.vert.size()+m.face.size();
			
			lib3ds_mesh_new_point_list(mesh, m.vert.size());//definisce il numero di vertici
	
			int v_index = 0;
			VertexIterator vi;
			if(mask & vcg::tri::io::Mask::IOM_VERTQUALITY)
			{
				for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if( !(*vi).IsD() )
				{
					Lib3dsPoint point;
					point.pos[0] = (*vi).P()[0];
					point.pos[1] = (*vi).P()[1];
					point.pos[2] = (*vi).P()[2];

					mesh->pointL[v_index] = point;		

					if (cb !=NULL)
						(*cb)(100.0 * (float)++current/(float)max, "writing vertices ");
					else
						return E_ABORTED;
					v_index++;
				}
			}
			lib3ds_mesh_new_face_list (mesh, m.face.size());
			int f_index = 0;//indice facce
			int t_index = 0;//indice texture
			FaceIterator fi;
			if(mask & vcg::tri::io::Mask::IOM_FACEQUALITY)
			{
				for(fi=m.face.begin(); fi!=m.face.end(); ++fi) if( !(*fi).IsD() )
				{
					Lib3dsFace face;
					face.points[0] = GetIndexVertex(m, (*fi).V(0));
					face.points[1] = GetIndexVertex(m, (*fi).V(1));
					face.points[2] = GetIndexVertex(m, (*fi).V(2));
					
					if(mask & vcg::tri::io::Mask::IOM_FACEFLAGS)
						face.flags = 0;
					
					face.smoothing = 10;//da modificare.
					if(mask & vcg::tri::io::Mask::IOM_FACENORMAL)
					{
						face.normal[0] = (*fi).N()[0];
						face.normal[1] = (*fi).N()[1];
						face.normal[2] = (*fi).N()[2];
					}

					if(mask & vcg::tri::io::Mask::IOM_FACECOLOR)
					{
						int material_index = CreateNewMaterial(m, materials, 0, fi);
						if(material_index == materials.size())
						{
							Lib3dsMaterial *material = lib3ds_material_new();//cre un nuovo materiale
							material->name[0] = 'm';
							material->name[1] = 'A' + material_index - 1;
							
							//ambient
							material->ambient[0] = materials[materials.size()-1].Ka[0];
							material->ambient[1] = materials[materials.size()-1].Ka[1];
							material->ambient[2] = materials[materials.size()-1].Ka[2];
							material->ambient[3] = materials[materials.size()-1].Tr;

							//diffuse
							material->diffuse[0] = materials[materials.size()-1].Kd[0];
							material->diffuse[1] = materials[materials.size()-1].Kd[1];
							material->diffuse[2] = materials[materials.size()-1].Kd[2];
							material->diffuse[3] = materials[materials.size()-1].Tr;

							//specular
							material->specular[0] = materials[materials.size()-1].Ks[0];
							material->specular[1] = materials[materials.size()-1].Ks[1];
							material->specular[2] = materials[materials.size()-1].Ks[2];
							material->specular[3] = materials[materials.size()-1].Tr;

							//shininess
							material->shininess = materials[materials.size()-1].Ns;
							
							//texture
							if(mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD)
							{
								strcpy(material->texture1_map.name,materials[materials.size()-1].map_Kd.c_str());
							}

							lib3ds_file_insert_material(file,material);//inserisce il materiale nella mesh
							
							face.material[0] = 'm';//associa alla faccia il materiale.
							face.material[1] = 'A' + material_index - 1;//l'idice del materiale...
						}
						else
						{	
							face.material[0] = 'm';//associa alla faccia il materiale.
							face.material[1] = 'A' + material_index;//l'idice del materiale...
						}
					}

					if(mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD)
					{
						unsigned int MAX = 3;
						for(unsigned int k=0;k<MAX;k++)
							if(m.HasPerWedgeTexture())
								if(AddNewTextureCoord(CoordTextures, (*fi).WT(k),GetIndexVertex(m, (*fi).V(k))))
									t_index++;
					}

					mesh->faceL[f_index]=face;

					if (cb !=NULL)
						(*cb)(100.0 * (float)++current/(float)max, "writing faces ");
					else 
						return E_ABORTED;
					f_index++;
					
				}
			}

			//aggiunge le coordinate di texture alla mesh
			if(m.HasPerWedgeTexture() && mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD )
			{
				if(lib3ds_mesh_new_texel_list(mesh,CoordTextures.size()))//alloca spazio per le coordinate di texture
				{
					typedef std::map<int, vcg::TCoord2<float> >::iterator MI;
					int i =0;
					for(MI coord = CoordTextures.begin();coord!=CoordTextures.end();++coord)
					{
						mesh->texelL[i][0] = (*coord).second.u();
						mesh->texelL[i][1] = (*coord).second.v();
						i++;
					}
				}	
				else
					return E_NOTEXCOORDVALID;
			}

			lib3ds_file_insert_mesh(file, mesh);//inserisce la mesh al file
			
			Lib3dsNode *node = lib3ds_node_new_object();//crea un nuovo nodo
			strcpy(node->name,mesh->name);
			node->parent_id = LIB3DS_NO_PARENT;	
			lib3ds_file_insert_node(file,node);

			bool result = lib3ds_file_save(file, filename); //salva il file
			if(result)
				return E_NOERROR; 
			else 
				return E_ABORTED;
		}
		
		static int Save(SaveMeshType &m, const char * filename, bool binary,int &mask, CallBackPos *cb=0)
		{
			return SaveBinary(m,filename,mask,cb);	
		}

		/*
			restituisce l'indice del vertice.
		*/
		inline static int GetIndexVertex(SaveMeshType &m, VertexType *p)
		{
			return p-&*(m.vert.begin());
		}
		
		/*
			aggiunge o sovrascrive il valore della coordinata di texture per vertice
			TODO:migliorare...possibilitÓ di eliminare anche la funzione.
		*/
		inline static bool AddNewTextureCoord(std::map<int, vcg::TCoord2<float> > &m, const vcg::TCoord2<float> &wt,int index_vertex)
		{
			//vcg::TCoord2<float> wtp = m[value];
			//if(wtp!=0){
			m[index_vertex]=wt;
			return true;
			//}
			//return false;
		}

		/*
			crea un nuovo materiale
		*/
		inline static int CreateNewMaterial(SaveMeshType &m, std::vector<Material> &materials, unsigned int index, FaceIterator &fi)
		{			
			unsigned char r = (*fi).C()[0];
			unsigned char g = (*fi).C()[1];
			unsigned char b = (*fi).C()[2];
			unsigned char alpha = (*fi).C()[3];
			
			Point3f diffuse;
			if(r > 1 || g > 1 || b > 1)
				diffuse = Point3f((float)r/255.0,(float)g/255.0,(float)b/255.0);
			else if (r != 0 || g != 0 || b != 0)
				diffuse = Point3f((float)r,(float)g,(float)b);
			else
				diffuse = Point3f(1.0,1.0,1.0);

			float Tr;
			if(alpha > 1)
				Tr = (float)alpha/255.0;
			else
				Tr = (float)alpha;
			
			int illum = 2; //default not use Ks!
			float ns = 0.0; //default

			Material mtl;

			mtl.index = index;//index of materials
			mtl.Ka = Point3f(0.2,0.2,0.2);//ambient
			mtl.Kd = diffuse;//diffuse
			mtl.Ks = Point3f(1.0,1.0,1.0);//specular
			mtl.Tr = Tr;//alpha
			mtl.Ns = ns;
			mtl.illum = illum;//illumination
			
			if(m.textures.size() && (*fi).WT(0).n() >=0 ) 		
				mtl.map_Kd = m.textures[(*fi).WT(0).n()];
			else
				mtl.map_Kd = "";
			
			int i = -1;
			if((i = MaterialsCompare(materials,mtl)) == -1)
			{
				materials.push_back(mtl);
				index++;
				return materials.size();
			}
			return i;
		}
		/*
			compara il materiale.
		*/
		inline static int MaterialsCompare(std::vector<Material> &materials, Material mtl)
		{
			for(int i=0;i<materials.size();i++)
			{
				bool ka = materials[i].Ka == mtl.Ka;
				bool kd = materials[i].Kd == mtl.Kd;
				bool ks = materials[i].Ks == mtl.Ks;
				bool tr = materials[i].Tr == mtl.Tr;
				bool illum = materials[i].illum == mtl.illum;
				bool ns = materials[i].Ns == mtl.Ns;
				bool map = materials[i].map_Kd == mtl.map_Kd;
				if(ka & kd & ks & tr & illum & ns & map){return i;}
			}
			return -1;
		}
	}; // end class

} // end Namespace tri
} // end Namespace io
} // end Namespace vcg

#endif
