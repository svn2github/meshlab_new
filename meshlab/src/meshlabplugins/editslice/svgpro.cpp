
#include "svgpro.h"


SVGPro::SVGPro(QWidget *parent, int numPlans, bool ExportMode )
	: QDialog(parent)
{    QString c;
	ui.setupUi(this);
	this->ExportMode=ExportMode;
	ui.numPlane->setText(c.setNum(numPlans));
	if(ExportMode){
		numRow=1;
		numCol=numPlans;
		ui.TitleLabel->setText("Exporting in single file");
		//ui.ImageWidth->setText(
	}
	else{
		numRow=1;
		numCol=1;
	    ui.TitleLabel->setText("Exporting in Multi file");
	}
	
	mesureUnit="cm";
}

SVGPro::~SVGPro()
{

}
void SVGPro::Init(int image_width, int image_height, int viewBox_width, int viewBox_height, float scale){
  QString c;
    if( /*ExportMode &&*/((image_width*numCol)!=(image_height*numRow)))
		ui.ImageWidth->setText(c.setNum(image_height*numCol));
	else
	ui.ImageWidth->setText(c.setNum(image_width));
    
	if(/*ExportMode &&*/((viewBox_width*numCol)!=(viewBox_height*numRow)))
		ui.viewBox_Width->setText(c.setNum(viewBox_height*numCol));
	else
		ui.viewBox_Width->setText(c.setNum(viewBox_width));

	ui.ImageHeight->setText(c.setNum(image_height));
	
	ui.viewBox_Height->setText(c.setNum(viewBox_height));
	
}



void SVGPro::on_ImageWidth_textChanged(const QString c)
{
     bool ok;
     QString q;
     int num=c.toInt(&ok, 10)/numCol; 
	
   if(ok)	 ui.WidthLabel->setText(q.setNum(num)+" "+mesureUnit);
}

void SVGPro::on_ImageHeight_textChanged(const QString c)
{
   bool ok;
     QString q;
     int num=c.toInt(&ok, 10)/numRow; 
	 if(ok)  ui.HeightLabel->setText(q.setNum(num)+" "+mesureUnit);
}



void SVGPro::on_cm_choose_clicked()
{
  mesureUnit="cm";
}

void SVGPro::on_pixel_choose_clicked()
{
  mesureUnit="px";
}

void SVGPro::on_viewBox_Width_textChanged(const QString c)
{
	 bool ok;
     QString q;
     int num=c.toInt(&ok, 10)/numCol; 
	 if(ok)  ui.WidthLabelViewBox->setText(q.setNum(num)+" px");
}

void SVGPro::on_viewBox_Height_textChanged(const QString c)
{
     bool ok;
     QString q;
     int num=c.toInt(&ok, 10)/numRow; 
	 if(ok)  ui.HeightLabelViewBox->setText(q.setNum(num)+" px");
}

void SVGPro::on_buttonBox_accepted()
{
   accept();
}

void SVGPro::on_buttonBox_rejected()
{
	reject (); 
}



void SVGPro::on_view_text_stateChanged(int i)
{
	if(i==0){
		showText=false;
	}
	else{
	    showText=true;
	}
}