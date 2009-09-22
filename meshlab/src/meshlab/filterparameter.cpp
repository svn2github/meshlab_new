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
$Log: filterparameter.h,v $
****************************************************************************/

#include <QtCore>
#include <QMap>
#include <QPair>
#include <QAction>
#include <vcg/math/matrix44.h>
#include "filterparameter.h"


using namespace vcg;

bool RichParameterSet::hasParameter(QString name)
{
	QList<RichParameter*>::iterator fpli;
	for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
		if((*fpli)->name ==name)
			return true;

	return false;
}
RichParameter* RichParameterSet::findParameter(QString name)
{
	QList<RichParameter*>::iterator fpli;
	for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
		if((*fpli)->name ==name)
			return *fpli;

	qDebug("FilterParameter Warning: Unable to find a parameter with name '%s',\n"
		"      Please check types and names of the parameter in the calling filter",qPrintable(name));
	return 0;
}

const RichParameter* RichParameterSet::findParameter(QString name) const
{
	QList<RichParameter*>::const_iterator fpli;
	for(fpli=paramList.begin();fpli!=paramList.end();++fpli)
	{
		if((*fpli != NULL) && (*fpli)->name==name)
			return *fpli;
	}
	qDebug("FilterParameter Warning: Unable to find a parameter with name '%s',\n"
		"      Please check types and names of the parameter in the calling filter",qPrintable(name));
	return 0;
}

RichParameterSet& RichParameterSet::removeParameter(QString name){
	paramList.removeAll(findParameter(name));
	return (*this);
}

RichParameterSet& RichParameterSet::addParam(RichParameter* pd )
{
	paramList.push_back(pd);
	return (*this);
}

//--------------------------------------


void RichParameterSet::setValue(QString name,const Value& newval)
{
	RichParameter *p=findParameter(name);
	assert(p);
	p->val->set(newval);
}

//--------------------------------------

bool RichParameterSet::getBool(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getBool();
}

//--------------------------------------

int	 RichParameterSet::getInt(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getInt();
}

//--------------------------------------

float RichParameterSet::getFloat(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getFloat();
}

//--------------------------------------

QColor RichParameterSet::getColor(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getColor();
}

Color4b RichParameterSet::getColor4b(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getColor4b();
}

//--------------------------------------


QString RichParameterSet::getString(QString name) const
{
	const RichParameter*p=findParameter(name);
	assert(p);
	return p->val->getString();
}

//--------------------------------------


Matrix44f		RichParameterSet::getMatrix44(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getMatrix44f();
}

//--------------------------------------

Point3f		RichParameterSet::getPoint3f(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getPoint3f();
}

//--------------------------------------

float		RichParameterSet::getAbsPerc(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return float(p->val->getAbsPerc());
}

int RichParameterSet::getEnum(QString name) const {
	const RichParameter *p=findParameter(name);
	assert(p);
	return float(p->val->getEnum());
}

QList<float> RichParameterSet::getFloatList(QString name) const
{
	const RichParameter *p = findParameter(name);
	assert(p);
	return p->val->getFloatList();
}

/* ---- */

MeshModel * RichParameterSet::getMesh(QString name) const {
	const RichParameter *p=findParameter(name);
	assert(p);
	return p->val->getMesh();
}

/* ---- */
/* Dynamic Float Members*/
/* ---- */

float RichParameterSet::getDynamicFloat(QString name) const
{
	const RichParameter *p=findParameter(name);
	assert(p);
	return float(p->val->getDynamicFloat());
}

QString RichParameterSet::getOpenFileName(QString name) const
{
	const RichParameter *p = findParameter(name);
	assert(p);
	return p->val->getFileName();
}


QString RichParameterSet::getSaveFileName(QString name) const
{
	const RichParameter *p = findParameter(name);
	assert(p);
	return p->val->getFileName();
}

RichParameterSet& RichParameterSet::operator=( const RichParameterSet& rps )
{
	return copy(rps);
}

bool RichParameterSet::operator==( const RichParameterSet& rps )
{
	if (rps.paramList.size() != paramList.size())
		return false;

	bool iseq = true;
	unsigned int ii = 0;
	while((ii < rps.paramList.size()) && iseq)
	{
		if (!(*rps.paramList.at(ii) == *paramList.at(ii)))
			iseq = false;
		++ii;
	}
	
	return iseq;
}

RichParameterSet::~RichParameterSet()
{
	//int val = _CrtCheckMemory( );
	for(unsigned int ii = 0;ii < paramList.size();++ii)
		delete paramList.at(ii);
	paramList.clear();

}

RichParameterSet& RichParameterSet::copy( const RichParameterSet& rps )
{
	clear();

	RichParameterCopyConstructor copyvisitor;
	for(unsigned int ii = 0;ii < rps.paramList.size();++ii)
	{
		rps.paramList.at(ii)->accept(copyvisitor);
		paramList.push_back(copyvisitor.lastCreated);
	}
	return (*this);
}


RichParameterSet::RichParameterSet( const RichParameterSet& rps )
{
	clear();

	RichParameterCopyConstructor copyvisitor;
	for(unsigned int ii = 0;ii < rps.paramList.size();++ii)
	{
		rps.paramList.at(ii)->accept(copyvisitor);
		paramList.push_back(copyvisitor.lastCreated);
	}
}

RichParameterSet::RichParameterSet() :paramList()
{

}

bool RichParameterSet::isEmpty() const
{
	return paramList.isEmpty();
}

void RichParameterSet::clear()
{
	paramList.clear();
}
/****************************************/

void RichParameterCopyConstructor::visit( RichBool& pd )
{
	lastCreated = new RichBool(pd.name,pd.pd->defVal->getBool(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichInt& pd )
{
	lastCreated = new RichInt(pd.name,pd.pd->defVal->getInt(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichFloat& pd )
{
	lastCreated = new RichFloat(pd.name,pd.pd->defVal->getFloat(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichString& pd )
{
	lastCreated = new RichString(pd.name,pd.pd->defVal->getString(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichMatrix44f& pd )
{
	lastCreated = new RichMatrix44f(pd.name,pd.pd->defVal->getMatrix44f(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichPoint3f& pd )
{
	lastCreated = new RichPoint3f(pd.name,pd.pd->defVal->getPoint3f(),pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichColor& pd )
{
	lastCreated = new RichColor(pd.name,pd.pd->defVal->getColor(),pd.pd->fieldDesc,pd.pd->tooltip);
}


void RichParameterCopyConstructor::visit( RichAbsPerc& pd )
{
	AbsPercDecoration* dec = reinterpret_cast<AbsPercDecoration*>(pd.pd); 
	lastCreated = new RichAbsPerc(pd.name,pd.pd->defVal->getAbsPerc(),dec->min,dec->max,pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichEnum& pd )
{
	EnumDecoration* dec = reinterpret_cast<EnumDecoration*>(pd.pd);
	lastCreated = new RichEnum(pd.name,pd.pd->defVal->getEnum(),dec->enumvalues,pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichFloatList& pd )
{
	/*lastCreated = new FloatListWidget(par,&pd);*/
}

void RichParameterCopyConstructor::visit( RichDynamicFloat& pd )
{
	DynamicFloatDecoration* dec = reinterpret_cast<DynamicFloatDecoration*>(pd.pd); 
	lastCreated = new RichDynamicFloat(pd.name,pd.pd->defVal->getDynamicFloat(),dec->min,dec->max,pd.pd->fieldDesc,pd.pd->tooltip);
}

void RichParameterCopyConstructor::visit( RichOpenFile& pd )
{
	/*lastCreated = new OpenFileWidget(par,&pd);*/
}

void RichParameterCopyConstructor::visit( RichSaveFile& pd )
{
	/*lastCreated = new SaveFileWidget(par,&pd);*/
}

void RichParameterCopyConstructor::visit( RichMesh& pd )
{
	MeshDecoration* dec = reinterpret_cast<MeshDecoration*>(pd.pd); 
	if (dec->defVal != NULL)
		lastCreated = new RichMesh(pd.name,dec->defVal->getMesh(),dec->meshdoc,dec->fieldDesc,dec->tooltip);
	else
		lastCreated = new RichMesh(pd.name,dec->meshindex);
}

void RichParameterXMLVisitor::fillRichParameterAttribute(const QString& type,const QString& name)
{
	parElem = docdom.createElement("Param");
	parElem.setAttribute("type",type);
	parElem.setAttribute("name",name);
}

void RichParameterXMLVisitor::fillRichParameterAttribute(const QString& type,const QString& name,const QString& val)
{
	fillRichParameterAttribute(type,name);
	parElem.setAttribute("value",val);
}

void RichParameterXMLVisitor::visit( RichBool& pd )
{
	QString v;
	if (pd.val->getBool()) 
		v = "true";
	else	
		v = "false";
	fillRichParameterAttribute("RichBool",pd.name,v);
}

void RichParameterXMLVisitor::visit( RichInt& pd )
{
	fillRichParameterAttribute("RichInt",pd.name,QString::number(pd.val->getInt()));
}

void RichParameterXMLVisitor::visit( RichFloat& pd )
{
	fillRichParameterAttribute("RichFloat",pd.name,QString::number(pd.val->getFloat()));
}

void RichParameterXMLVisitor::visit( RichString& pd )
{
	fillRichParameterAttribute("RichString",pd.name,pd.val->getString());
}

void RichParameterXMLVisitor::visit( RichMatrix44f& pd )
{
	fillRichParameterAttribute("RichMatrix44f",pd.name);
	vcg::Matrix44f mat = pd.val->getMatrix44f();
	for(unsigned int ii = 0;ii < 16;++ii)
		parElem.setAttribute(QString("val")+QString::number(ii),QString::number(mat.V()[ii]));
}

void RichParameterXMLVisitor::visit( RichPoint3f& pd )
{
	fillRichParameterAttribute("RichPoint3f",pd.name);
	vcg::Point3f p = pd.val->getPoint3f();
	parElem.setAttribute("x",QString::number(p.X()));
	parElem.setAttribute("y",QString::number(p.Y()));
	parElem.setAttribute("z",QString::number(p.Z()));
}

void RichParameterXMLVisitor::visit( RichColor& pd )
{
	fillRichParameterAttribute("RichColor",pd.name);
	QColor p = pd.val->getColor();
	parElem.setAttribute("r",QString::number(p.red()));
	parElem.setAttribute("g",QString::number(p.green()));
	parElem.setAttribute("b",QString::number(p.blue()));
	parElem.setAttribute("a",QString::number(p.alpha()));
}

void RichParameterXMLVisitor::visit( RichAbsPerc& pd )
{
	fillRichParameterAttribute("RichAbsPerc",pd.name,QString::number(pd.val->getAbsPerc()));
	AbsPercDecoration* dec = reinterpret_cast<AbsPercDecoration*>(pd.pd);
	parElem.setAttribute("min",QString::number(dec->min));
	parElem.setAttribute("max",QString::number(dec->max));
}

void RichParameterXMLVisitor::visit( RichEnum& pd )
{
	fillRichParameterAttribute("RichEnum",pd.name,QString::number(pd.val->getEnum()));
	EnumDecoration* dec = reinterpret_cast<EnumDecoration*>(pd.pd);
	parElem.setAttribute("enum_cardinality",dec->enumvalues.size());
	for(unsigned int ii = 0; ii < dec->enumvalues.size();++ii)
		parElem.setAttribute(QString("enum_val")+QString::number(ii),dec->enumvalues.at(ii));

}

void RichParameterXMLVisitor::visit( RichFloatList& pd )
{
	assert(0);
}

void RichParameterXMLVisitor::visit(RichDynamicFloat& pd)
{
	fillRichParameterAttribute("RichDynamicFloat",pd.name,QString::number(pd.val->getDynamicFloat()));
	DynamicFloatDecoration* dec = reinterpret_cast<DynamicFloatDecoration*>(pd.pd);
	parElem.setAttribute("min",QString::number(dec->min));
	parElem.setAttribute("max",QString::number(dec->max));
}

void RichParameterXMLVisitor::visit( RichOpenFile& pd )
{
	assert(0);
}

void RichParameterXMLVisitor::visit( RichSaveFile& pd )
{
	assert(0);
}

void RichParameterXMLVisitor::visit( RichMesh& pd )
{
	MeshDecoration* dec = reinterpret_cast<MeshDecoration*>(pd.pd);
	fillRichParameterAttribute("RichMesh",pd.name,QString::number(dec->meshindex));
}

void RichParameterFactory::create( const QDomElement& np,RichParameter** par )
{
	QString name=np.attribute("name");
	QString type=np.attribute("type");

	qDebug("    Reading Param with name %s : %s",qPrintable(name),qPrintable(type));

	if(type=="RichBool")    
	{ 
		*par = new RichBool(name,np.attribute("value")!=QString("false")); 
		return; 
	}

	if(type=="RichInt")     
	{ 
		*par = new RichInt(name,np.attribute("value").toInt()); 
		return; 
	}

	if(type=="RichFloat")   
	{ 
		*par = new RichFloat(name,np.attribute("value").toFloat());
		return;
	}

	if(type=="RichString")  
	{ 
		*par = new RichString(name,np.attribute("value")); 
		return; 
	}
	
	if(type=="RichAbsPerc") 
	{ 
		*par = new RichAbsPerc(name,np.attribute("value").toFloat(),np.attribute("min").toFloat(),np.attribute("max").toFloat()); 
		return; 
	}

	if(type=="RichColor")		
	{ 
		unsigned int r = np.attribute("r").toUInt(); 
		unsigned int g = np.attribute("g").toUInt(); 
		unsigned int b = np.attribute("b").toUInt(); 
		unsigned int a = np.attribute("a").toUInt(); 
		QColor col(r,g,b,a);
		*par= new RichColor(name,col); 
		return; 
	}

	if(type=="RichMatrix44f")
	{
		Matrix44f mm;
		for(int i=0;i<16;++i)
			mm.V()[i]=np.attribute(QString("val")+QString::number(i)).toFloat();
		*par = new RichMatrix44f(name,mm);    
		return;                    
	}

	if(type=="RichEnum")
	{
		QStringList list = QStringList::QStringList();
		unsigned int enum_card = np.attribute(QString("enum_cardinality")).toUInt();

		for(int i=0;i<enum_card;++i)
			list<<np.attribute(QString("enum_val")+QString::number(i));

		*par = new RichEnum(name,np.attribute("value").toInt(),list);
		return;
	}

	if(type == "RichMesh")  
	{ 
		*par = new RichMesh(name, np.attribute("value").toInt()); 
		return; 
	}

	if(type == "RichFloatList")
	{
		//to be implemented
		assert(0);
	}

	if(type == "RichOpenFile")  
	{ 
		//to be implemented
		assert(0);
	}

	if(type == "RichSaveFile")  
	{ 
		//par.addOpenFileName(name, np.attribute(ValueName())); return; 
		
		//to be implemented
		assert(0);
	}

	if(type=="Point3f") 
	{
		Point3f val;
		val[0]=np.attribute("x").toFloat();
		val[1]=np.attribute("y").toFloat();
		val[2]=np.attribute("z").toFloat();
		*par = new RichPoint3f(name, val);  
		return; 
	}

	assert(0); // we are trying to parse an unknown xml element
}

BoolValue::BoolValue( const bool val ) : pval(val)
{

}

ParameterDecoration::ParameterDecoration( Value* defvalue,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :fieldDesc(desc),tooltip(tltip),defVal(defvalue)
{

}

ParameterDecoration::~ParameterDecoration()
{
	delete defVal;
}

BoolDecoration::BoolDecoration( BoolValue* defvalue,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :ParameterDecoration(defvalue,desc,tltip)
{

}

IntDecoration::IntDecoration( IntValue* defvalue,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip)
{

}

FloatDecoration::FloatDecoration( FloatValue* defvalue,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip)
{

}

StringDecoration::StringDecoration( StringValue* defvalue,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip)
{

}

Matrix44fDecoration::Matrix44fDecoration( Matrix44fValue* defvalue,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip)
{

}

Point3fDecoration::Point3fDecoration( Point3fValue* defvalue,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip)
{

}

ColorDecoration::ColorDecoration( ColorValue* defvalue,const QString desc /*= QString()*/,const QString tltip/*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip)
{

}

AbsPercDecoration::AbsPercDecoration( AbsPercValue* defvalue,const float minVal,const float maxVal,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip),min(minVal),max(maxVal)
{

}

EnumDecoration::EnumDecoration( EnumValue* defvalue, QStringList values,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip),enumvalues(values)
{

}

DynamicFloatDecoration::DynamicFloatDecoration( DynamicFloatValue* defvalue, const float minVal,const float maxVal,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip),min(minVal),max(maxVal)
{

}

FileDecoration::FileDecoration( FileValue* defvalue,const QString extension/*=QString(".*")*/,const QString desc /*= QString()*/,const QString tltip /*= QString()*/ ) :ParameterDecoration(defvalue,desc,tltip),ext(extension)
{

}

MeshDecoration::MeshDecoration( MeshValue* defvalue,MeshDocument* doc,const QString desc/*=QString()*/, const QString tltip/*=QString()*/ ) :ParameterDecoration(defvalue,desc,tltip),meshdoc(doc)
{
	meshindex = -1;
	if (doc != NULL) meshindex = doc->meshList.indexOf(defvalue->getMesh()); 
	assert((meshindex != -1) || (doc == NULL));
}

MeshDecoration::MeshDecoration( int meshind,MeshDocument* doc,const QString desc/*=QString()*/, const QString tltip/*=QString()*/ ) :ParameterDecoration(NULL,desc,tltip),meshdoc(doc)
{
	assert(meshind < doc->size() && meshind >= 0); 
	meshindex = meshind;
	if (doc != NULL)
		defVal = new MeshValue(doc->meshList.at(meshind));
}

MeshDecoration::MeshDecoration( int meshind ) :ParameterDecoration(NULL,QString(),QString()),meshdoc(NULL),meshindex(meshind)
{

}

RichParameter::RichParameter( const QString nm,Value* v,ParameterDecoration* prdec ) :name(nm),pd(prdec),val(v)
{

}

RichParameter::~RichParameter()
{
	delete val;delete pd;
}

RichBool::RichBool( const QString nm,const bool defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) : RichParameter(nm,new BoolValue(defval),new BoolDecoration(new BoolValue(defval),desc,tltip))
{

}

void RichBool::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichBool::operator==( const RichParameter& rb )
{
	return (rb.val->isBool() && (name == rb.name) && (val->getBool() == rb.val->getBool()));
}

RichBool::~RichBool()
{

}

RichInt::RichInt( const QString nm,const int defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new IntValue(defval),new IntDecoration(new IntValue(defval),desc,tltip))
{

}

void RichInt::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichInt::operator==( const RichParameter& rb )
{
	return (rb.val->isInt() &&(name == rb.name) && (val->getInt() == rb.val->getInt()));
}

RichInt::~RichInt()
{

}

RichFloat::RichFloat( const QString nm,const float defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new FloatValue(defval),new FloatDecoration(new FloatValue(defval),desc,tltip))
{

}

void RichFloat::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichFloat::operator==( const RichParameter& rb )
{
	return (rb.val->isFloat() &&(name == rb.name) && (val->getFloat() == rb.val->getFloat()));
}

RichFloat::~RichFloat()
{

}

RichString::RichString( const QString nm,const QString defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new StringValue(defval),new StringDecoration(new StringValue(defval),desc,tltip))
{

}

void RichString::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichString::operator==( const RichParameter& rb )
{
	return (rb.val->isString() &&(name == rb.name) && (val->getString() == rb.val->getString()));
}

RichString::~RichString()
{

}

RichMatrix44f::RichMatrix44f( const QString nm,const vcg::Matrix44f& defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new Matrix44fValue(defval),new Matrix44fDecoration(new Matrix44fValue(defval),desc,tltip))
{

}

void RichMatrix44f::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichMatrix44f::operator==( const RichParameter& rb )
{
	return (rb.val->isMatrix44f() &&(name == rb.name) && (val->getMatrix44f() == rb.val->getMatrix44f()));
}

RichMatrix44f::~RichMatrix44f()
{

}

RichPoint3f::RichPoint3f( const QString nm,const vcg::Point3f defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new Point3fValue(defval),new Point3fDecoration(new Point3fValue(defval),desc,tltip))
{

}

void RichPoint3f::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichPoint3f::operator==( const RichParameter& rb )
{
	return (rb.val->isPoint3f() &&(name == rb.name) && (val->getPoint3f() == rb.val->getPoint3f()));
}

RichPoint3f::~RichPoint3f()
{

}

RichColor::RichColor( const QString nm,const QColor defval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new ColorValue(defval),new ColorDecoration(new ColorValue(defval),desc,tltip))
{

}

void RichColor::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichColor::operator==( const RichParameter& rb )
{
	return (rb.val->isColor() &&(name == rb.name) && (val->getColor() == rb.val->getColor()));
}

RichColor::~RichColor()
{

}

RichAbsPerc::RichAbsPerc( const QString nm,const float defval,const float minval,const float maxval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm, new AbsPercValue(defval), new AbsPercDecoration(new AbsPercValue(defval),minval,maxval,desc,tltip))
{

}

void RichAbsPerc::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichAbsPerc::operator==( const RichParameter& rb )
{
	return (rb.val->isAbsPerc() &&(name == rb.name) && (val->getAbsPerc() == rb.val->getAbsPerc()));
}

RichAbsPerc::~RichAbsPerc()
{

}

RichEnum::RichEnum( const QString nm,const int defval,const QStringList values,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new EnumValue(defval),new EnumDecoration(new EnumValue(defval),values,desc,tltip))
{

}

void RichEnum::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichEnum::operator==( const RichParameter& rb )
{
	return (rb.val->isEnum() &&(name == rb.name) && (val->getEnum() == rb.val->getEnum()));
}

RichEnum::~RichEnum()
{

}

RichMesh::RichMesh( const QString nm,MeshModel* defval,MeshDocument* doc,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm, new MeshValue(defval),new MeshDecoration( new MeshValue(defval),doc,desc,tltip))
{

}

RichMesh::RichMesh( const QString nm,int meshindex,MeshDocument* doc,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,NULL, new MeshDecoration(meshindex,doc,desc,tltip))
{
	assert(meshindex < doc->size() && meshindex >= 0); 
	val = new MeshValue(doc->meshList.at(meshindex));
}

RichMesh::RichMesh( const QString nm,int meshindex ) :RichParameter(nm,new MeshValue(NULL),new MeshDecoration(meshindex))
{

}

void RichMesh::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichMesh::operator==( const RichParameter& rb )
{
	return (rb.val->isMesh() &&(name == rb.name) && (val->getMesh() == rb.val->getMesh()));
}

RichMesh::~RichMesh()
{

}

RichFloatList::RichFloatList( const QString nm,FloatListValue* v,FloatListDecoration* prdec ) :RichParameter(nm,v,prdec)
{

}

void RichFloatList::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichFloatList::operator==( const RichParameter& rb )
{
	return (rb.val->isFloatList() &&(name == rb.name) && (val->getFloatList() == rb.val->getFloatList()));
}

RichFloatList::~RichFloatList()
{

}

RichDynamicFloat::RichDynamicFloat( const QString nm,const float defval,const float minval,const float maxval,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new DynamicFloatValue(defval),new DynamicFloatDecoration(new DynamicFloatValue(defval),minval,maxval,desc,tltip))
{

}

void RichDynamicFloat::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichDynamicFloat::operator==( const RichParameter& rb )
{
	return (rb.val->isDynamicFloat() &&(name == rb.name) && (val->getDynamicFloat() == rb.val->getDynamicFloat()));
}

RichDynamicFloat::~RichDynamicFloat()
{

}

RichOpenFile::RichOpenFile( const QString nm,const QString defval,const QString ext /*= QString("*.*")*/,const QString desc/*=QString()*/,const QString tltip/*=QString()*/ ) :RichParameter(nm,new FileValue(defval),new FileDecoration(new FileValue(defval),tltip,desc))
{

}

void RichOpenFile::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichOpenFile::operator==( const RichParameter& rb )
{
	return (rb.val->isFileName() &&(name == rb.name) && (val->getFileName() == rb.val->getFileName()));
}

RichOpenFile::~RichOpenFile()
{

}

RichSaveFile::RichSaveFile( const QString nm,FileValue* v,FileDecoration* prdec ) :RichParameter(nm,v,prdec)
{

}

void RichSaveFile::accept( Visitor& v )
{
	v.visit(*this);
}

bool RichSaveFile::operator==( const RichParameter& rb )
{
	return (rb.val->isFileName() &&(name == rb.name) && (val->getFileName() == rb.val->getFileName()));
}

RichSaveFile::~RichSaveFile()
{

}