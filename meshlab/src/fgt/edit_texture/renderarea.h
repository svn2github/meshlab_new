#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <meshlab/meshmodel.h>
#include <wrap/gui/trackball.h>
#include <stdio.h>
#include <QGLWidget>

#define TEXTX 175
#define TEXTY 200
#define TRANSLATE 5
#define RADIUS 10
#define RECTDIM 30

#define NO_CHANGE -1
#define RESET -2
#define IGNORECMD -3
#define ENABLECMD -4
#define DISABLECMD -5

using namespace std;

class RenderArea : public QGLWidget
{
    Q_OBJECT

public:
	enum Mode { View, Edit, Select };
	enum EditMode { Scale, Rotate, NoEdit };
	enum SelectMode { Area, Connected };

    RenderArea(QWidget *parent = 0, QString path = QString(), MeshModel *m = 0, unsigned textNum = 0);
	~RenderArea();

public:
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);
	void setTexture(QString path);
	void ChangeMode(int index);
	void ChangeSelectMode(int index);
	void RemapClamp();
	void RemapMod();
	void SetDimension(int dim);

protected:
    void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent*e);

private:
	bool antialiased;	// Antialiasing 
    QImage image;		// Background texture
	unsigned textNum;	// Number of tab (--> index in 'textures' vector)
	QString fileName;	// Name of the texture
	MeshModel *model;	// Ref to the model (for upate)

	Mode mode;			// Action type
	EditMode editMode;
	SelectMode selectMode;

	// Trackball data
	vcg::Trackball *tb;
	Point2f viewport;
	float oldX, oldY;
	int tmpX, tmpY;

	QPen pen;			// For 2D painting
    QBrush brush;

	int panX, panY, tpanX, tpanY, oldPX, oldPY;	// Temp for axis
	int maxX, maxY, minX, minY;	// For texCoord out of border

	int selBit;
	bool selected;

	QPointF origin;		// Origin for rotate editing
	QRect originR;
	int orX, orY;

	QPoint start, end;	// Selection area
	QRect area;

	// Info for interactive editing
	vector<QRect> selRect;
	QRect selection;
	QPoint selStart, selOld, selEnd;
	int posX, posY, rectX, rectY, oldSRX, oldSRY;
	float degree, scaleX, scaleY;
	int highlighted, pressed;
	QPointF oScale;
	int initVX, initVY;

	QImage rot, scal;

	int AREADIM;

	void UpdateUV();
	void ResetTrack();
	void SelectFaces();
	void HandleScale(QPoint e);
	void HandleRotate(QPoint e);
	void RotateComponent(float theta);
	void ScaleComponent(float percentX, float percentY);
	void RecalculateSelectionArea();
	void UpdateSelectionArea(int x, int y);

signals:
	void UpdateStat(float u, float v, int faceN, int vertN, int countFace);

};

#endif