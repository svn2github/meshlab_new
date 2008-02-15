#ifndef _EQHANDLE_H_
#define _EQHANDLE_H_

//#include <QObject>
//#include <QGraphicsItem>
#include "handle.h"
//#include "util.h"

enum EQUALIZER_HANDLE_TYPE
{
	LEFT_HANDLE = 0,
	MID_HANDLE,
	RIGHT_HANDLE,
	NUMBER_OF_EQHANDLES
};

/* Specific handle for equalizerHistogramScene 
It can only be dragged horizontally */
class EqHandle : public Handle
{
	Q_OBJECT

public:
	EqHandle(CHART_INFO *environment_info, QColor color, QPointF position,   
		EQUALIZER_HANDLE_TYPE type, EqHandle** handles, qreal* midHandlePercentilePosition, QDoubleSpinBox* spinbox,
		int zOrder, int size);
	~EqHandle(void);
	QRectF boundingRect () const;
	virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget); 
	
	void setSize						(int size)					{(size%2==0) ? _size=size+1 : _size=size;};
	void setBarHeight					(qreal height)				{_barHeight = height;};
	void setType						(EQUALIZER_HANDLE_TYPE type){_type = type;};	
	void setMidHandlePercentilePosition (qreal* pointer)			{_midHandlePercentilePosition = pointer;};	
	void setHandlesPointer				(EqHandle** pointer)			{_handlesPointer = pointer;};	
	void setSpinBoxPointer				(QDoubleSpinBox* pointer)	{_spinBoxPointer = pointer;};
	

protected:
	void mouseMoveEvent   (QGraphicsSceneMouseEvent *event);

private:
	EQUALIZER_HANDLE_TYPE _type;
	qreal			_barHeight;
	QPointF			_triangle[NUMBER_OF_EQHANDLES];
	qreal*			_midHandlePercentilePosition;
	EqHandle**		_handlesPointer;
	QDoubleSpinBox* _spinBoxPointer;

	qreal calculateMidHandlePercentilePosition(qreal newHandleX)
	{
		return (newHandleX - _handlesPointer[LEFT_HANDLE]->pos().x()) / (_handlesPointer[RIGHT_HANDLE]->pos().x() - _handlesPointer[LEFT_HANDLE]->pos().x());
	};

	qreal positionToQuality(qreal newHandleX)
	{
		qreal percentagePos = (newHandleX-_chartInfo->leftBorder) / _chartInfo->chartWidth;
		assert( (percentagePos>=-1.0f) && (percentagePos<=2.0f) );
		float maxX = (_handlesPointer[RIGHT_HANDLE]->_spinBoxPointer->value() > _chartInfo->maxX)?_handlesPointer[RIGHT_HANDLE]->_spinBoxPointer->value():_chartInfo->maxX;
		float minX = (_handlesPointer[LEFT_HANDLE]->_spinBoxPointer->value()  < _chartInfo->minX)?_handlesPointer[LEFT_HANDLE]->_spinBoxPointer->value() :_chartInfo->minX;
		return percentagePos * (maxX - minX)+minX;
		//return percentagePos * (_chartInfo->maxX - _chartInfo->minX) + _chartInfo->minX;
		//return percentagePos * (_handlesPointer[RIGHT_HANDLE]->_spinBoxPointer->value() - _handlesPointer[LEFT_HANDLE]->_spinBoxPointer->value()) +  _handlesPointer[LEFT_HANDLE]->_spinBoxPointer->value();
	};

signals:
	void positionChangedToSpinBox(double);
	void positionChanged();
	void insideHistogram(bool);

private slots:
	// changing equalizer spinboxes moves the connected handle
	void setXBySpinBoxValueChanged (double spinBoxValue);
	void moveMidHandle();
};

#endif // EQHANDLE_H