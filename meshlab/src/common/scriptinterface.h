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

#ifndef SCRIPTINTERFACE_H
#define SCRIPTINTERFACE_H

#include <QtScript>
#include "filterparameter.h"
#include "xmlfilterinfo.h"
#include "meshmodel.h"
#include "vcg/space/point3.h"

class ScriptAdapterGenerator
{
private:
	QString parNames(const RichParameterSet& set) const;
	QString parNames(const QString&  filterName,const MLXMLPluginInfo& xmlInfo) const;
	static inline QString optName() {return QString("optional");}
public:
	inline static const QString meshDocVarName() {return QString("meshDoc");}
	static const QStringList javaScriptLibraryFiles();

	QString mergeOptParamsCodeGenerator() const;
	QString funCodeGenerator(const QString&  filtername,const RichParameterSet& set) const;
	QString funCodeGenerator(const QString&  filtername,const MLXMLPluginInfo& xmlInfo) const;
};


QScriptValue PluginInterfaceInit(QScriptContext *context, QScriptEngine *engine, void * param);
QScriptValue PluginInterfaceApply(QScriptContext *context, QScriptEngine *engine, void * param);
QScriptValue PluginInterfaceApplyXML(QScriptContext *context, QScriptEngine *engine, void * param);

QScriptValue IRichParameterSet_prototype_setBool(QScriptContext* c,QScriptEngine* e);
QScriptValue IRichParameterSet_prototype_setInt(QScriptContext* c,QScriptEngine* e);
QScriptValue IRichParameterSet_prototype_setAbsPerc(QScriptContext* c,QScriptEngine* e);
QScriptValue IRichParameterSet_prototype_setFloat(QScriptContext* c,QScriptEngine* e);

QScriptValue IRichParameterSet_ctor(QScriptContext* c,QScriptEngine* e);

QScriptValue myprint (QScriptContext* sc, QScriptEngine* se);

class ScriptInterfaceUtilities
{
public:
	static QVector<float> vcgPointToVector(const vcg::Point3f& p);
};

//class VCGPoint3fSI : public QObject
//{
//	Q_OBJECT
//public:
//
//	VCGPoint3fSI& operator =(const VCGPoint3fSI& b);
//	VCGPoint3fSI();
//	VCGPoint3fSI(const float x,const float y,const float z);
//	Q_INVOKABLE void add(const VCGPoint3fSI& p);
//	Q_INVOKABLE void mult(const float s);
//	//Q_INVOKABLE VCGPoint3fSI& set(const float x,const float y,const float z);
//	vcg::Point3f pp;
//};



typedef vcg::Point3f VCGPoint3SI;
typedef QVector<float> Point3;
typedef QVector<Point3> Point3Vector;

class VCGVertexSI : public QObject
{
	Q_OBJECT 
public:
	VCGVertexSI(CMeshO::VertexType& v);

	//Q_INVOKABLE vcg::Point3f* p();
	Q_INVOKABLE Point3 getP();
	Q_INVOKABLE VCGPoint3SI getPoint();
	Q_INVOKABLE void setPC(const float x,const float y,const float z);
	Q_INVOKABLE void setP(const Point3& p);
	Q_INVOKABLE void setPoint(const VCGPoint3SI& p);
	Q_INVOKABLE Point3 getN();
	Q_INVOKABLE VCGPoint3SI getNormal();
	Q_INVOKABLE void setNormal(const VCGPoint3SI& p);
	Q_INVOKABLE void setN(const float x,const float y,const float z);

	CMeshO::VertexType& vv;
};

class MeshDocumentSI : public QObject
{
	Q_OBJECT

public:
	MeshDocumentSI(MeshDocument* doc);
	~MeshDocumentSI(){}

	Q_INVOKABLE MeshModelSI* getMesh(const int meshId);
	Q_INVOKABLE MeshModelSI* operator[](const QString& name);
	Q_INVOKABLE MeshModelSI* getMeshByName(const QString& name);
	Q_INVOKABLE MeshModelSI* current();
	Q_INVOKABLE int currentId();
	Q_INVOKABLE int setCurrent(const int meshId);
	MeshDocument* md;
};

class ShotSI;

class MeshModelSI : public QObject
{
	Q_OBJECT

public:
	MeshModelSI(MeshModel& meshModel,MeshDocumentSI* mdsi);
	
	Q_INVOKABLE int id() const;
	Q_INVOKABLE float bboxDiag() const;
	Q_INVOKABLE QVector<float> bboxMin() const;
	Q_INVOKABLE QVector<float> bboxMax() const;
Q_INVOKABLE inline float computeMinVQ() const {  return vcg::tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(mm.cm).first;  }
Q_INVOKABLE inline float computeMaxVQ() const {  return vcg::tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(mm.cm).second; }
Q_INVOKABLE inline float computeMinFQ() const {  return vcg::tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(mm.cm).first;  }
Q_INVOKABLE inline float computeMaxFQ() const {  return vcg::tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(mm.cm).second; }

	Q_INVOKABLE QVector<VCGVertexSI*> vert();
	Q_INVOKABLE Point3Vector getVertPosArray();
	Q_INVOKABLE Point3Vector getVertNormArray();
	Q_INVOKABLE void setVertPosArray(const Point3Vector& pa);
	Q_INVOKABLE void setVertNormArray(const Point3Vector& na);
	//Q_INVOKABLE void setV(const QVector<VCGVertexSI*>& v);

	Q_INVOKABLE int vn() const;
	Q_INVOKABLE int fn() const;
	Q_INVOKABLE VCGVertexSI* v(const int ind);
	Q_INVOKABLE ShotSI* shot();

	MeshModel& mm;
}; 

Q_DECLARE_METATYPE(MeshDocumentSI*)
QScriptValue MeshDocumentScriptInterfaceToScriptValue(QScriptEngine* eng,MeshDocumentSI* const& in);

void MeshDocumentScriptInterfaceFromScriptValue(const QScriptValue& val,MeshDocumentSI*& out);

Q_DECLARE_METATYPE(MeshModelSI*)

QScriptValue MeshModelScriptInterfaceToScriptValue(QScriptEngine* eng,MeshModelSI* const& in);

void MeshModelScriptInterfaceFromScriptValue(const QScriptValue& val,MeshModelSI*& out);

Q_DECLARE_METATYPE(VCGVertexSI*)

QScriptValue VCGVertexScriptInterfaceToScriptValue(QScriptEngine* eng,VCGVertexSI* const& in);

void VCGVertexScriptInterfaceFromScriptValue(const QScriptValue& val,VCGVertexSI*& out);



Q_DECLARE_METATYPE(Point3)
Q_DECLARE_METATYPE(Point3Vector)
Q_DECLARE_METATYPE(QVector<VCGVertexSI*>)

Q_DECLARE_METATYPE(VCGPoint3SI*)
Q_DECLARE_METATYPE(VCGPoint3SI)

QScriptValue VCGPoint3ScriptInterface_ctor(QScriptContext *context, QScriptEngine *engine);

//QScriptValue VCGPoint3fScriptInterfaceToScriptValue(QScriptEngine* eng,VCGPoint3fSI* const& in);
//void VCGPoint3fScriptInterfaceFromScriptValue(const QScriptValue& val,VCGPoint3fSI*& out);

class Env :public QScriptEngine
{
	Q_OBJECT

	QString out;
public:
	Env();
	Q_INVOKABLE void insertExpressionBinding(const QString& nm,const QString& exp);
	QString output();
	void appendOutput(const QString& output);
};

QScriptValue Env_ctor(QScriptContext *context,QScriptEngine *engine);

Q_DECLARE_METATYPE(Env*)

class EnvWrap
{
private:
	bool constStatement(const QString& statement) const;
	QScriptValue evalExp( const QString& nm );
	Env* env;
public:
	EnvWrap():env(NULL){};
	EnvWrap(Env& envir);
	bool evalBool(const QString& nm);
	double evalDouble(const QString& nm);
	float evalFloat( const QString& nm );
	int evalInt( const QString& nm );
	vcg::Point3f evalVec3(const QString& nm);
	QColor evalColor(const QString& nm);
	/*QString getExpType(const QString& nm);*/
	QString evalString(const QString& nm);
	int evalEnum( const QString& nm );
	MeshModel* evalMesh(const QString& nm);
	//vcg::Shotf evalShot(const QString& nm);
};

QScriptValue EnvWrap_ctor(QScriptContext* c,QScriptEngine* e);

class ShotSI : public QObject
{
	Q_OBJECT
public:
	ShotSI(vcg::Shotf& st);
	~ShotSI() {};

	//only for c++
	vcg::Shotf getShot();

private:
	vcg::Shotf& shot; 
};

Q_DECLARE_METATYPE(ShotSI*)
QScriptValue ShotScriptInterfaceToScriptValue(QScriptEngine* eng,ShotSI* const& in);
void ShotScriptInterfaceFromScriptValue(const QScriptValue& val,ShotSI*& out);


	inline QScriptValue VCGPoint3SI_addV3(QScriptContext * c,QScriptEngine *e )
	{
		return e->toScriptValue(*qscriptvalue_cast<VCGPoint3SI*>(c->argument(0)) + *qscriptvalue_cast<VCGPoint3SI*>(c->argument(1)));
	}

	inline QScriptValue VCGPoint3SI_multV3S( QScriptContext * c,QScriptEngine *e )
	{
		return e->toScriptValue(*qscriptvalue_cast<VCGPoint3SI*>(c->argument(0)) * (float) c->argument(1).toNumber());
	}


//QScriptValue VCGPoint3SI_multV3S(QScriptContext * c,QScriptEngine *e );
//class EnvWrap : protected virtual QScriptEngine
//{
//private:
//	bool constStatement(const QString& statement) const;
//	QScriptValue getExp( const QString& nm );
//public:
//	EnvWrap();
//	bool getBool(const QString& nm);
//	float getFloat(const QString& nm);
//};
//
//
//class Env : public EnvWrap, public virtual QScriptEngine
//{
//public:
//	Env();
//	void insertExpressionBinding(const QString& nm,const QString& exp);
//};


#endif
