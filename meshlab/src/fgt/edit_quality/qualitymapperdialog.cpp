#include "qualitymapperdialog.h"
#include <limits>
#include <QPen>
#include <QBrush>

using namespace vcg;


QualityMapperDialog::QualityMapperDialog(QWidget *parent, MeshModel *m) : QDialog(parent), mesh(m)
{
	ui.setupUi(this);

	//building up histogram...
	int numberOfBins = 100;
	_equalizer_histogram = new Histogramf();
	Frange mmmq(tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(mesh->cm));
	this->ComputePerVertexQualityHistogram(mesh->cm, mmmq, _equalizer_histogram, numberOfBins);
	//...histogram built

	_histogram_info = 0;

	_transferFunction = new TransferFunction( STARTUP_TF_TYPE );
	_isTransferFunctionInitialized = false;
	_transferFunction_info = 0;

	connect(ui.minSpinBox, SIGNAL(valueChanged(double)), &_equalizerHandles[0], SLOT(setXBySpinBoxValueChanged(double)));
	connect(ui.midSpinBox, SIGNAL(valueChanged(double)), &_equalizerHandles[1], SLOT(setXBySpinBoxValueChanged(double)));
	connect(ui.maxSpinBox, SIGNAL(valueChanged(double)), &_equalizerHandles[2], SLOT(setXBySpinBoxValueChanged(double)));
	
	connect(&_equalizerHandles[0], SIGNAL(positionChangedToSpinBox(double)), ui.minSpinBox, SLOT(setValue(double)));
	connect(&_equalizerHandles[1], SIGNAL(positionChangedToSpinBox(double)), ui.midSpinBox, SLOT(setValue(double)));
	connect(&_equalizerHandles[2], SIGNAL(positionChangedToSpinBox(double)), ui.maxSpinBox, SLOT(setValue(double)));
	
	connect(&_equalizerHandles[0], SIGNAL(positionChangedToMidHandle()), &_equalizerHandles[1], SLOT(moveMidHandle()));
	connect(&_equalizerHandles[2], SIGNAL(positionChangedToMidHandle()), &_equalizerHandles[1], SLOT(moveMidHandle()));


}

QualityMapperDialog::~QualityMapperDialog()
{
	if ( _histogram_info )
	{
		delete _histogram_info;
		_histogram_info = 0;
	}

	if ( _transferFunction )
	{
		delete _transferFunction;
		_transferFunction = 0;
	}

	if ( _transferFunction_info )
	{
		delete _transferFunction_info;
		_transferFunction_info = 0;
	}
}

/*
void QualityMapperDialog::setValues(const QualityMapperSettings& qms)
{
_settings=qms;
QString qnum="%1";
ui.minSpinBox->setValue(_settings.meshMinQ);
ui.midSpinBox->setValue(_settings.meshMidQ);
ui.maxSpinBox->setValue(_settings.meshMaxQ);
}

QualityMapperSettings QualityMapperDialog::getValues()
{
_settings.manualMinQ=ui.minSpinBox->value();
_settings.manualMidQ=ui.midSpinBox->value();
_settings.manualMaxQ=ui.maxSpinBox->value();

return _settings;
}*/


void QualityMapperDialog::ComputePerVertexQualityHistogram( CMeshO &m, Frange range, Histogramf *h, int bins )    // V1.0
{
			h->Clear();
			h->SetRange( range.minV, range.maxV, bins);
			CMeshO::VertexIterator vi;
			for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)
				if(!(*vi).IsD()) h->Add((*vi).Q());		
}

void QualityMapperDialog::clearScene(QGraphicsScene *scene)
{
	//deleting scene items
	QList<QGraphicsItem *>allItems = _transferFunctionScene.items();
	QGraphicsItem *item = 0;
	foreach (item, allItems)
		scene->removeItem( item );
}

void QualityMapperDialog::drawChartBasics(QGraphicsScene& scene, CHART_INFO *chart_info)
{
	//a valid chart_info must be passed
	assert( chart_info != 0 );

	QPen p( Qt::black, 2 );

	//drawing axis
	//x axis
	scene.addLine( chart_info->leftBorder, chart_info->lowerBorder, chart_info->rightBorder, chart_info->lowerBorder, p );
	//y axis
	scene.addLine( chart_info->leftBorder, chart_info->upperBorder, chart_info->leftBorder, chart_info->lowerBorder, p );
}

//_equalizerScene dovrebbe chiamarsi in realt� histogramScene
//l'histogram e la transfer function potrebbero diventare attributi di questa classe? valutare l'impatto.
//in generale il codice di questo metodo va ripulito un po'...
void QualityMapperDialog::drawEqualizerHistogram()
{
	//building histogram chart informations
	if ( _histogram_info == 0 )
	{
		//processing minY and maxY values for histogram
		int maxY = 0;
		int minY = std::numeric_limits<int>::max();
		for (int i=0; i<_equalizer_histogram->n; i++) 
		{
			if ( _equalizer_histogram->H[i] > maxY )
				maxY = _equalizer_histogram->H[i];

			if ( _equalizer_histogram->H[i] < minY )
				minY = _equalizer_histogram->H[i];
		}
		_histogram_info = new CHART_INFO( ui.equalizerGraphicsView->width(), ui.equalizerGraphicsView->height(), _equalizer_histogram->n, _equalizer_histogram->minv, _equalizer_histogram->maxv, minY, maxY );
		//_histogram_info->data = this;

	}

	//drawing axis and other basic items
	this->drawChartBasics( _equalizerScene, _histogram_info );

	float barHeight = 0.0f;					//initializing height of the histogram bars
	float barWidth = _histogram_info->dX;	//processing width of the histogram bars (4\5 of dX)
	//	float barSeparator = dX - barWidth;        //processing space between consecutive bars of the histogram bars (1\5 of dX)

	QPen drawingPen(Qt::black);
	QBrush drawingBrush (QColor(32, 32, 32),Qt::SolidPattern);

	QPointF startBarPt;
	QSizeF barSize;

	//drawing histogram bars
	for (int i = 0; i < _histogram_info->numOfItems; i++)
	{
		barHeight = (float)(_histogram_info->chartHeight * _equalizer_histogram->H[i]) / (float)_histogram_info->maxRoundedY;
		startBarPt.setX( _histogram_info->leftBorder + ( barWidth * i ) );
		startBarPt.setY( (float)_histogram_info->lowerBorder - barHeight );

		barSize.setWidth(barWidth);
		barSize.setHeight(barHeight);

		//drawing histogram bar
		_equalizerScene.addRect(startBarPt.x(), startBarPt.y(), barSize.width(), barSize.height(), drawingPen, drawingBrush);
	}

	//drawing handles
	QColor colors[] = { QColor(Qt::red), QColor(Qt::green), QColor(Qt::blue) };

	qreal xStart = _histogram_info->leftBorder;
	qreal xPos = 0.0;
	qreal yPos = _histogram_info->lowerBorder;
	_equalizerMidHandlePercentilePosition = 0.5f;
	for (int i=0; i<3; i++)
	{
		xPos = xStart + _histogram_info->chartWidth/2.0*i;
		_equalizerHandles[i].setColor(colors[i]);
		_equalizerHandles[i].setPos(xPos, yPos);
		_equalizerHandles[i].setBarHeight(_histogram_info->chartHeight);
		_equalizerHandles[i].setZValue(1);
		_equalizerHandles[i].setHistogramInfo(_histogram_info);
		_equalizerHandles[i].setHandlesPointer(_equalizerHandles);
		_equalizerHandles[i].setMidHandlePercentilePosition(&_equalizerMidHandlePercentilePosition);
		_equalizerScene.addItem(&_equalizerHandles[i]);
	}
	_equalizerHandles[0].setType(LEFT_HANDLE);
	_equalizerHandles[1].setType(MID_HANDLE);
	_equalizerHandles[2].setType(RIGHT_HANDLE);


	// Setting spinbox values
	// (Se venissero inizializzati prima di impostare setHistogramInfo sulle handles darebbero errore nello SLOT setX delle handles.)
	double singleStep = (_histogram_info->maxX - _histogram_info->minX) / _histogram_info->chartWidth;
	ui.minSpinBox->setValue(_histogram_info->minX);
	ui.minSpinBox->setRange(_histogram_info->minX, _histogram_info->maxX);
	ui.minSpinBox->setSingleStep(singleStep);

	ui.midSpinBox->setValue((_histogram_info->maxX + _histogram_info->minX) / 2.0f);
	ui.midSpinBox->setRange(_histogram_info->minX, _histogram_info->maxX);
	ui.midSpinBox->setSingleStep(singleStep);

	ui.maxSpinBox->setValue(_histogram_info->maxX);
	ui.maxSpinBox->setRange(_histogram_info->minX, _histogram_info->maxX);
	ui.maxSpinBox->setSingleStep(singleStep);

	ui.equalizerGraphicsView->setScene(&_equalizerScene);
}

void QualityMapperDialog::initTF()
{
	if ( _transferFunction == 0 )
		return ;

	for ( int i=0; i<NUMBER_OF_DEFAULT_TF; i++ )
		ui.presetComboBox->addItem( TransferFunction::defaultTFs[(STARTUP_TF_TYPE + i)%NUMBER_OF_DEFAULT_TF] );

	for (int i=0; i<_knownExternalTFs.size(); i++)
		ui.presetComboBox->addItem( _knownExternalTFs.at(i).name );

	_isTransferFunctionInitialized = true;
}


void QualityMapperDialog::drawTransferFunction()
{
	if ( !_isTransferFunctionInitialized )
		this->initTF();
	
	this->clearScene( &_transferFunctionScene );

	assert(_transferFunction != 0);

	//building transfer function chart informations
	if ( _transferFunction_info == 0 )
		_transferFunction_info = new CHART_INFO( ui.transferFunctionView->width(), ui.transferFunctionView->height(), _transferFunction->size(), 0.0f, 1.0f, 0.0f, 1.0f );

	//drawing axis and other basic items
	this->drawChartBasics( _transferFunctionScene, _transferFunction_info );

	//	_transferFunctionScene.addLine(0, 0, 100, 430, QPen(Qt::green, 3));

	//questo per il momento � fisso e definito qu�, ma dovr� essere gestito nella classe handle
	QPointF pointToRepresentLeft;
	QPointF pointToRepresentRight;
	QPointF previousPoint;
	QRectF pointRectLeft(0, 0, 2.0*MARKERS_RADIUS, 2.0*MARKERS_RADIUS );
	QRectF pointRectRight(0, 0, 2.0*MARKERS_RADIUS, 2.0*MARKERS_RADIUS );

	QColor channelColor;
	QPen drawingPen(Qt::black);
	QBrush drawingBrush ( QColor(32, 32, 32), Qt::SolidPattern );


	//drawing chart points
	//	TfChannel *tf_c = 0;
	TF_CHANNEL_VALUE val;

	for(int c=0; c<NUMBER_OF_CHANNELS; c++)
	{
		TYPE_2_COLOR((*_transferFunction)[c].getType(), channelColor);
		drawingPen.setColor( channelColor );
		drawingBrush.setColor( channelColor );

		for (int i=0; i<(*_transferFunction)[c].size(); i++)
		{
			val = (*_transferFunction)[c][i];

			pointToRepresentLeft.setX( _transferFunction_info->leftBorder + relative2AbsoluteValf( (* val.x), (float)_transferFunction_info->chartWidth ) );
			pointToRepresentLeft.setY( _transferFunction_info->lowerBorder - relative2AbsoluteValf( val.y->getLeftJunctionPoint(), (float)_transferFunction_info->chartHeight ) /*/ _transferFunction_info->maxRoundedY*/ );
			pointToRepresentRight.setX( pointToRepresentLeft.x() );
			pointToRepresentRight.setY( _transferFunction_info->lowerBorder - relative2AbsoluteValf( val.y->getRightJunctionPoint(), (float)_transferFunction_info->chartHeight ) /*/ _transferFunction_info->maxRoundedY*/ );

			//drawing single current point
			pointRectLeft.setX(pointToRepresentLeft.x() - MARKERS_RADIUS );
			pointRectLeft.setY(pointToRepresentLeft.y() - MARKERS_RADIUS );
			pointRectLeft.setWidth(2.0*MARKERS_RADIUS);
			pointRectLeft.setHeight(2.0*MARKERS_RADIUS);
			_transferFunctionScene.addEllipse( pointRectLeft, drawingPen, drawingBrush );
			if ( pointToRepresentLeft.y() != pointToRepresentRight.y() )
			{
				pointRectRight.setX(pointToRepresentRight.x() - MARKERS_RADIUS );
				pointRectRight.setY(pointToRepresentRight.y() - MARKERS_RADIUS );
				pointRectRight.setWidth(pointRectLeft.width());
				pointRectRight.setHeight(pointRectLeft.height());
				_transferFunctionScene.addEllipse( pointRectRight, drawingPen, drawingBrush );
			}

			//sostituire con disegno della TfHandle

			//		_transferFunctionScene.FillEllipse( drawingBrush, pointRect );

			//linear interpolation between current point and previous one
			//interpolation will not be executed if the current point is the first of the list
			if (i > 0)
			{
				_transferFunctionScene.addLine(previousPoint.x(), previousPoint.y(), pointToRepresentLeft.x(), pointToRepresentLeft.y(), drawingPen);

				if ( pointToRepresentLeft.y() != pointRectRight.y() )
					_transferFunctionScene.addLine( pointToRepresentLeft.x(), pointToRepresentLeft.y(), pointToRepresentRight.x(), pointToRepresentRight.y(), drawingPen );
			}

			//refresh of previous point.
			//So, it's possible to interpolate linearly the current point with the previous one
			previousPoint = pointToRepresentRight;
		}
	}

	ui.transferFunctionView->setScene( &_transferFunctionScene );
}


void QualityMapperDialog::on_addPointButton_clicked()
{

}

void QualityMapperDialog::on_savePresetButton_clicked()
{
	QString tfName = ui.presetComboBox->currentText();
	_transferFunction->saveColorBand( tfName );

	KNOWN_EXTERNAL_TFS newTF( tfName, tfName );
	_knownExternalTFs << newTF;
	_isTransferFunctionInitialized = false;
	this->initTF();
}


void QualityMapperDialog::on_loadPresetButton_clicked()
{
	QString csvFileName = QFileDialog::getOpenFileName(0, "Open Transfer Function File", QDir::currentPath(), "CSV File (*.csv)");
	if ( !csvFileName.isNull())
	{
		if ( _transferFunction )
		{
			delete _transferFunction;
			_transferFunction = 0;
		}
		_transferFunction = new TransferFunction( csvFileName );
	}

	KNOWN_EXTERNAL_TFS newTF( csvFileName, csvFileName );
	_knownExternalTFs << newTF;

	_isTransferFunctionInitialized = false;
	this->initTF();
	this->drawTransferFunction();
}



void QualityMapperDialog::on_presetComboBox_textChanged(const QString &newValue)
{
	for (int i=0; i<NUMBER_OF_DEFAULT_TF; i++)
	{
		if ( TransferFunction::defaultTFs[i] == newValue )
		{
			if ( _transferFunction )
			{
				delete _transferFunction;
				_transferFunction = new TransferFunction( (DEFAULT_TRANSFER_FUNCTIONS)i );
			}
		}
	}
}