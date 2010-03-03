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

#include <QApplication>
#include <common/interfaces.h>
#include <common/pluginmanager.h>
#include <common/filterscript.h>
class FilterData
{
public:
	FilterData();
	QString name;
	QString info;
	int filterClass;
	bool operator <(const FilterData &d) const {return name<d.name;}
};

class MeshLabServer
{
	public:
	MeshLabServer()
	{
	}

	~MeshLabServer()
	{
	}

	static bool FilterCallBack(const int pos, const char * str)
	{
		int static lastPos=-1;
		if(pos==lastPos) return true;
		lastPos=pos;
    printf("%s",str);
		return true;
	}

	// Here we need a better way to find the plugins directory.
	// To be implemented:
	// use the QSettings togheter with MeshLab.
	// When meshlab starts if he find the plugins write the absolute path of that directory in a persistent qsetting place.
	// Here we use that QSetting. If it is not set we remember to run meshlab first once.
	// in this way it works safely on mac too and allows the user to put the small meshlabserver binary wherever they desire (/usr/local/bin).

	void loadPlugins(FILE *fp=0)
	{
		PM.loadPlugins(defaultGlobal);

		if(fp)
		{
			foreach(MeshFilterInterface *iFilter, PM.meshFilterPlugins())
				foreach(QAction *filterAction, iFilter->actions())
					fprintf(fp, "*<b><i>%s</i></b> <br>%s<br>\n",qPrintable(filterAction->text()), qPrintable(iFilter->filterInfo(filterAction)));
		}

		printf("Total %i filtering actions\n", PM.actionFilterMap.size());
		printf("Total %i io plugins\n", PM.meshIOPlug.size());
	}

	bool Open(MeshModel &mm, QString fileName)
	{
		// Opening files in a transparent form (IO plugins contribution is hidden to user)
		QStringList filters;

		// HashTable storing all supported formats togheter with
		// the (1-based) index  of first plugin which is able to open it
		QHash<QString, MeshIOInterface*> allKnownFormats;

		PM.LoadFormats(filters, allKnownFormats,PluginManager::IMPORT);

		QFileInfo fi(fileName);
		QDir curdir= QDir::current();
		// this change of dir is needed for subsequent textures/materials loading
		//QDir::setCurrent(fi.absoluteDir().absolutePath());

		QString extension = fi.suffix();
		qDebug("Opening a file with extention %s",qPrintable(extension));
		// retrieving corresponding IO plugin
		MeshIOInterface* pCurrentIOPlugin = allKnownFormats[extension.toLower()];
		if (pCurrentIOPlugin == 0)
		{
			printf("Error encountered while opening file: ");
			return false;
		}
		int mask = 0;

		RichParameterSet prePar;
		pCurrentIOPlugin->initPreOpenParameter(extension, fileName,prePar);

		if (!pCurrentIOPlugin->open(extension, fileName, mm ,mask,prePar))
		{
			printf("MeshLabServer: Failed loading of %s from dir %s\n",qPrintable(fileName),qPrintable(curdir.path()));
			return false;
		}
		vcg::tri::UpdateBounding<CMeshO>::Box(mm.cm);
		QDir::setCurrent(curdir.path());
		return true;
	}

	bool Save(MeshModel *mm, int mask, QString fileName)
	{
		// Opening files in a transparent form (IO plugins contribution is hidden to user)
		QStringList filters;

		// HashTable storing all supported formats togheter with
		// the (1-based) index  of first plugin which is able to open it
		QHash<QString, MeshIOInterface*> allKnownFormats;

		PM.LoadFormats( filters, allKnownFormats,PluginManager::EXPORT);

		QFileInfo fi(fileName);
		// this change of dir is needed for subsequent textures/materials loading
		// QDir::setCurrent(fi.absoluteDir().absolutePath());

		QString extension = fi.suffix();

		// retrieving corresponding IO plugin
		MeshIOInterface* pCurrentIOPlugin = allKnownFormats[extension.toLower()];
		if (pCurrentIOPlugin == 0)
		{
		printf("Error encountered while opening file: ");
			//QString errorMsgFormat = "Error encountered while opening file:\n\"%1\"\n\nError details: The \"%2\" file extension does not correspond to any supported format.";
			//QMessageBox::critical(this, tr("Opening Error"), errorMsgFormat.arg(fileName, extension));
			return false;
		}

		// optional saving parameters (like ascii/binary encoding)
		RichParameterSet savePar;
		pCurrentIOPlugin->initSaveParameter(extension, *mm, savePar);

		if (!pCurrentIOPlugin->save(extension, fileName, *mm ,mask, savePar))
		{
			printf("Failed saving\n");
			return false;
		}

		return true;
	}


	bool Script(MeshDocument &meshDocument, QString scriptfile){

		MeshModel &mm = *meshDocument.mm();

		FilterScript scriptPtr;

		//Open/Load FilterScript

		if (scriptfile.isEmpty())
		{
			printf("No script specified\n");
			return false;
		}
		scriptPtr.open(scriptfile);
		printf("Starting Script of %i actions",scriptPtr.actionList.size());
		FilterScript::iterator ii;
		for(ii = scriptPtr.actionList.begin();ii!= scriptPtr.actionList.end();++ii)
		{
      //RichParameterSet &par = (*ii).second;
      //QString &name = (*ii).first;
			printf("filter: %s\n",qPrintable((*ii).first));

			QAction *action = PM.actionFilterMap[ (*ii).first];
			if (action == NULL)
			{
				printf("filter %s not found",qPrintable((*ii).first));
				return false;
			}
			MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(action->parent());
			iFilter->setLog(NULL);
			int req = iFilter->getRequirements(action);
			mm.updateDataMask(req);
					//make sure the PARMESH parameters are initialized

			//A filter in the script file couldn't have all the required parameter not defined (a script file not generated by MeshLab).
			//So we have to ask to the filter the default values for all the parameters and integrate them with the parameters' values 
			//defined in the script file.
			RichParameterSet required;
			iFilter->initParameterSet(action,meshDocument,required);
			RichParameterSet &parameterSet = (*ii).second;
			
			//The parameters in the script file are more than the required parameters of the filter. The script file is not correct.
			if (required.paramList.size() < parameterSet.paramList.size())
			{
				printf("The parameters in the script file are more than the filter %s requires.\n",qPrintable((*ii).first));
				return false;
			}

			for(int i = 0; i < required.paramList.size(); i++)
			{
				RichParameterCopyConstructor v;
				if (!parameterSet.hasParameter(required.paramList[i]->name))
				{
					required.paramList[i]->accept(v);
					parameterSet.addParam(v.lastCreated);
				}	
				assert(parameterSet.paramList.size() == required.paramList.size());
				RichParameter* parameter = parameterSet.paramList[i];
				//if this is a mesh paramter and the index is valid
				if(parameter->val->isMesh())
				{
					MeshDecoration* md = reinterpret_cast<MeshDecoration*>(parameter->pd);
					if(	md->meshindex < meshDocument.size() &&
						md->meshindex >= 0  )
					{
						RichMesh* rmesh = new RichMesh(parameter->name,meshDocument.getMesh(md->meshindex),&meshDocument);
						parameterSet.paramList.replace(i,rmesh);
					} else
					{
						printf("Meshes loaded: %i, meshes asked for: %i \n", meshDocument.size(), md->meshindex );
						printf("One of the filters in the script needs more meshes than you have loaded.\n");
						exit(-1);
					}
					delete parameter;
				}
			}

			bool ret = iFilter->applyFilter( action, meshDocument, (*ii).second, FilterCallBack);
			//iFilter->applyFilter( action, mm, (*ii).second, QCallBack );
			//GLA()->log.Logf(GLLogStream::WARNING,"Re-Applied filter %s",qPrintable((*ii).first));

			if(!ret)
			{
				printf("Problem with filter: %s\n",qPrintable((*ii).first));
				return false;
			}
		}

		return true;
	}

	void Usage()
	{
		printf("\nUsage:\n"
			"    meshlabserver arg1 arg2 ...  \n"
			"where args can be: \n"
			" -i [filename...]  mesh(s) that has to be loaded\n"
			" -o [filename...]  mesh(s) where to write the result(s)\n"
			" -s filename		    script to be applied\n"
			" -d filename       dump on a text file a list of all the filtering fucntion\n"
			" -om options       data to save in the output files: vc -> vertex colors, vf -> vertex flags, vq -> vertex quality, vn-> vertex normals, vt -> vertex texture coords, "
			" fc -> face colors, ff -> face flags, fq -> face quality, fn-> face normals, "
			" wc -> wedge colors, wn-> wedge normals, wt -> wedge texture coords \n"
			"Example:\n"
			"	'meshlabserver -i input.obj -o output.ply -s meshclean.mlx -om vc fq wn'\n"
			"\nNotes:\n\n"
			"There can be multiple meshes loaded and the order they are listed matters because \n"
			"filters that use meshes as parameters choose the mesh based on the order.\n"
			"The number of output meshes must be either one or equal to the number of input meshes.\n"
			"If the number of output meshes is one then only the first mesh in the input list is saved.\n"
			"The format of the output mesh is guessed by the used extension.\n"
			"Script is optional and must be in the format saved by MeshLab.\n"
			);

		exit(-1);
	}

private:
	PluginManager PM;
	RichParameterSet defaultGlobal;

};

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);  
	MeshLabServer server;
	MeshDocument meshDocument;
	QStringList meshNamesIn, meshNamesOut;
	QString scriptName;
	FILE *filterFP=0;
	int mask=0;
	if(argc < 3) server.Usage();
	int i = 1;
	QString res = qApp->applicationDirPath();
        QDir currentdir(QDir::currentPath());
	
	while(i < argc)
	{
		if(argv[i][0] != '-') server.Usage();
		switch(argv[i][1])
		{
			case 'i' :  
				while( ((i+1) < argc) && argv[i+1][0] != '-')
				{
					meshNamesIn << currentdir.absoluteFilePath(argv[i+1]);
					printf("Input mesh  %s\n", qPrintable(meshNamesIn.last() ));
					i++;
				}
				i++; 
				break; 
			case 'o' : 
				{
				if (argv[i][2]==0)
				{
					while( ((i+1) < argc) && argv[i+1][0] != '-')
					{
						meshNamesOut << currentdir.absoluteFilePath(argv[i+1]);
						printf("output mesh  %s\n", qPrintable(meshNamesOut.last()));
						i++;
					}
					i++; 
				break; 
				}
				else if (argv[i][2]=='m')
				{
					printf("Output mask:\n");
					while( ((i+1) < argc) && argv[i+1][0] != '-')
					{
						switch (argv[i+1][0])
						{
						case 'v' :
						{
							switch (argv[i+1][1])
							{
								case 'c' :
								{
									printf("vertex color, ");
									mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;
									i++;
									break;
								}
								case 'f' :
								{
									printf("vertex flags, ");
									mask |= vcg::tri::io::Mask::IOM_VERTFLAGS;
									i++;
									break;
								}
								case 'n' :
								{
									printf("vertex normals, ");
									mask |= vcg::tri::io::Mask::IOM_VERTNORMAL;
									i++;
									break;
								}
								case 'q' :
								{
									printf("vertex quality, ");
									mask |= vcg::tri::io::Mask::IOM_VERTQUALITY;
									i++;
									break;
								}
								case 't' :
								{
									printf("vertex tex coords, ");
									mask |= vcg::tri::io::Mask::IOM_VERTTEXCOORD;
									i++;
									break;
								}
								default :
								{
									i++;
									break;
								}
							}
							break;
						}
						case 'f' :
						{
							switch (argv[i+1][1])
							{
								case 'c' :
								{
									printf("face color, ");
									mask |= vcg::tri::io::Mask::IOM_FACECOLOR;
									i++;
									break;
								}
								case 'f' :
								{
									printf("face flags, ");
									mask |= vcg::tri::io::Mask::IOM_FACEFLAGS;
									i++;
									break;
								}
								case 'n' :
								{
									printf("face normals, ");
									mask |= vcg::tri::io::Mask::IOM_FACENORMAL;
									i++;
									break;
								}
								case 'q' :
								{
									printf("face quality, ");
									mask |= vcg::tri::io::Mask::IOM_FACEQUALITY;
									i++;
									break;
								}
								default :
								{
									i++;
									break;
								}
							}
							break;
						}
						case 'w' :
						{
							switch (argv[i+1][1])
							{
								case 'c' :
								{
									printf("wedge color, ");
									mask |= vcg::tri::io::Mask::IOM_WEDGCOLOR;
									i++;
									break;
								}
								case 'n' :
								{
									printf("wedge normals, ");
									mask |= vcg::tri::io::Mask::IOM_WEDGNORMAL;
									i++;
									break;
								}
								case 't' :
								{
									printf("wedge tex coords, ");
									mask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
									i++;
									break;
								}
								default :
								{
									i++;
									break;
								}
							}
							break;
						}
						default:
						{
							i++;
							break;
						}
						}
						
				}
				i++;
				break;
				}

			}
			case 's' :  
				if( argc <= i+1 ) {
					printf("Missing script name\n");  
					exit(-1);
				}
				scriptName = currentdir.absoluteFilePath(argv[i+1]);
				printf("script %s\n", qPrintable(scriptName));
				i += 2;
				break; 
			case 'd' :
				if( argc <= i+1 ) {
					printf("Missing dump name\n");  
					exit(-1);
				}
				filterFP=fopen(argv[i+1],"w");
			 i+=2;
			 break;
											 
		}
	}
	
	printf("Loading Plugins:\n");
	server.loadPlugins(filterFP);
	
	
	if(meshNamesIn.isEmpty()) {
		printf("No input mesh\n"); exit(-1);
	} else
	{
		for(int i = 0; i < meshNamesIn.size(); i++)
		{
			MeshModel *mm = new MeshModel( meshNamesIn.at(i).toStdString().c_str() );
			server.Open(*mm, meshNamesIn.at(i));
            printf("Mesh %s loaded has %i vn %i fn\n", qPrintable(mm->shortName()), mm->cm.vn, mm->cm.fn);

			//now add it to the document
            meshDocument.addNewMesh(qPrintable(mm->shortName()), mm);
		}
		//the first mesh is the one the script is applied to
		meshDocument.setCurrentMesh(0);
	}
				
	if(!scriptName.isEmpty())
	{		
		printf("Apply FilterScript: '%s'\n",qPrintable(scriptName));
		bool returnValue = server.Script(meshDocument, scriptName);
		if(!returnValue)
		{
			printf("Failed to apply FilterScript\n");
			exit(-1);
		}
	} else 
                printf("No Script to apply.\n");
	
	//if there is not one name or an equal number of in and out names then exit 
	if(meshNamesOut.isEmpty() )
		printf("No output mesh names given."); 
	else if(meshNamesOut.size() != 1 && meshNamesOut.size() != meshNamesIn.size() ) {
		printf("Wrong number of output mesh names given\n"); 
		exit(-1);
	} else 
	{
		for(int i = 0; i < meshNamesOut.size(); i++)
		{
			server.Save(meshDocument.getMesh(i), mask, meshNamesOut.at(i));
			printf("Mesh %s saved with: %i vn %i fn\n", qPrintable(meshNamesOut.at(i)), meshDocument.mm()->cm.vn, meshDocument.mm()->cm.fn);
		}
	}		
}

