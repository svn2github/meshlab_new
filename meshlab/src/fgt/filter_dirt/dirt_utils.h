/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2007                                                \/)\/    *
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
#ifndef DIRT_UTILS_H
#define DIRT_UTILS_H

#define PI 3.14159265
//Include Files

#include <math.h>
#include <common/meshmodel.h>
#include <common/interfaces.h>
#include<vector>
#include<vcg/simplex/vertex/base.h>
#include<vcg/simplex/face/base.h>
#include<vcg/complex/trimesh/base.h>
#include <vcg/space/point3.h>
#include <vcg/space/intersection2.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/clean.h>
#include "particle.h"

using namespace vcg;
using namespace tri;


CMeshO::CoordType fromBarCoords(Point3f bc,CMeshO::FacePointer f){

    CMeshO::CoordType p;
    Point3f p0=f->P(0);
    Point3f p1=f->P(1);
    Point3f p2=f->P(2);
    p[0]=p0[0]*bc[0]+p1[0]*bc[1]+p2[0]*bc[2];
    p[1]=p0[1]*bc[0]+p1[1]*bc[1]+p2[1]*bc[2];
    p[2]=p0[2]*bc[0]+p1[2]*bc[1]+p2[2]*bc[2];
    return p;
};


CMeshO::CoordType CoordinateCorrection(CMeshO::CoordType p,CMeshO::FacePointer f){
    Point3f bc;
    CMeshO::CoordType c_p; //Correct Position
    InterpolationParameters(*f,p,bc);

    if(bc[0]==0){
        bc[1]=1-bc[2];
        c_p=fromBarCoords(bc,f);

    }

    if(bc[1]==0){
        bc[0]=1-bc[2];
        c_p=fromBarCoords(bc,f);

    }

    if(bc[2]==0){
        bc[0]=1-bc[1];
        c_p=fromBarCoords(bc,f);
     }

    return c_p;
};

bool SameSide(Point3f p1,Point3f p2,Point3f a,Point3f b){
        Point3f cp1;
        Point3f cp2;
        cp1 = (b-a)^(p1-a);
        cp2 = (b-a)^(p2-a);
        if (cp1.dot(cp2) >= 0) return true;
        else return false;

};

/**
@def Verify if a point lies on a face

@param Point3f p   - Coordinates of the point
@param FacePointer f - Pointer to the face

@return true if point p is on face f, false elsewhere.
*/
bool IsOnFace(Point3f p, CMeshO::FacePointer f){
    //Verify if the point is on the same plane of the face


    CMeshO::CoordType n=f->N();

    Point3f a=f->P(0);
    if(math::Abs((p-a).dot(n))>0.0005) return false;



    //Point3f bc;
    //isOnFace=InterpolationParameters(*f,vcg::Normal(*f),p,bc);

    //if(isOnFace && bc[0]>=0 && bc[1]>=0 && (bc[0]+bc[1]<=1)) return true;


    //Point3f a=f->P(0);
    //float tmp2=n.dot(p);
    //float tmp=n.dot(a);
    //if(math::Abs(tmp2-tmp)>0.01) return false;
    //Point3f v0 = (f->V(0))->P();
    //Point3f v1 = (f->V(1))->P();
    //Point3f v2 = (f->V(2))->P();


 //  if(SameSide(p,v0,v1,v2) && SameSide(p,v1,v0,v2) && SameSide(p,v2,v0,v1)) return true;
  //  return false;
/*
    //Compute Barycentric coordinates
    Point3f bc;
    bool isOnFace=false;
    isOnFace=InterpolationParameters(*f,vcg::Normal(*f),p,bc);
    if(!isOnFace) return false;
    isOnFace=(bc[0]>=0 && bc[1]>=0) && (bc[0]+bc[1] < 1);
    return isOnFace;
*/
   /*

    CMeshO::CoordType n=f->N();
    Point3f a=f->P(0);
    float tmp2=n.dot(p);
    float tmp=n.dot(a);
    if(math::Abs(tmp2-tmp)>0.001) return false;
*/

/*
    int max_c;

    if(math::Abs(n[0])>math::Abs(n[1])){
          if(math::Abs(n[0])>math::Abs(n[2])) max_c=0;
          else max_c=2;
    }else{
        if(math::Abs(n[1])>math::Abs(n[2])) max_c=1;
        else max_c=2;
              }


    CMeshO::CoordType fv0 = (f->V(0))->P();
    CMeshO::CoordType fv1 = (f->V(1))->P();
    CMeshO::CoordType fv2 = (f->V(2))->P();
    Point2<float> A;
    Point2<float> B;
    Point2<float> C;
    Point2<float> P;
    switch(max_c){

      case 0:{
            A=Point2f(fv0[1],fv0[2]);
            B=Point2f(fv1[1],fv1[2]);
            C=Point2f(fv2[1],fv2[2]);
            P=Point2f(p[1],p[2]);
            break;
        }

    case 1:{
            A=Point2f(fv0[0],fv0[2]);
            B=Point2f(fv1[0],fv1[2]);
            C=Point2f(fv2[0],fv2[2]);
            P=Point2f(p[0],p[2]);
            break;
        }

    case 2:{
            A=Point2f(fv0[0],fv0[1]);
            B=Point2f(fv1[0],fv1[1]);
            C=Point2f(fv2[0],fv2[1]);
            P=Point2f(p[0],p[1]);
        break;
        }
    }
*/


/*
    Point3f A=f->V(0)->P();
    Point3f B=f->V(1)->P();
    Point3f C=f->V(2)->P();

    Point3fv0 = C - A;
    Point3fv1 = B - A;
    Point3fv2 = p - A;

    // Compute dot products
    double dot00 = v0.dot(v0);
    double dot01 = v0.dot(v1);
    double dot02 = v0.dot(v2);
    double dot11 = v1.dot(v1);
    double dot12 = v1.dot(v2);

    // Compute barycentric coordinates
    double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Check if point is in triangle
    //Compute Barycentric coordinates
    Point3f bc;

    InterpolationParameters(*f,vcg::Normal(*f),p,bc);

    bool isOnFace=(u >= 0) && (v >= 0) && (u + v <= 1.00001);
    return isOnFace;
*/





    int max_c;

    float n0=math::Abs(n[0]);
    float n1=math::Abs(n[1]);
    float n2=math::Abs(n[2]);

    if(n0>n1){
          if(n0>n2) max_c=0;
          else max_c=2;
    }else{
        if(n1>n2) max_c=1;
        else max_c=2;
              }


    Point2f p_2d;
    Point2f p0_2d;
    Point2f p1_2d;
    Point2f p2_2d;

    CMeshO::CoordType p0 = f->P(0);
    CMeshO::CoordType p1 = f->P(1);
    CMeshO::CoordType p2 = f->P(2);
    switch(max_c){
    case 0:{
            p0_2d=Point2f(p0[1],p0[2]);
            p1_2d=Point2f(p1[1],p1[2]);
            p2_2d=Point2f(p2[1],p2[2]);
            p_2d =Point2f(p[1],p[2]);
            break;
        }
    case 1:{
            p0_2d=Point2f(p0[0],p0[2]);
            p1_2d=Point2f(p1[0],p1[2]);
            p2_2d=Point2f(p2[0],p2[2]);
            p_2d =Point2f(p[0],p[2]);

            break;
        }
    case 2:{
            p0_2d=Point2f(p0[0],p0[1]);
            p1_2d=Point2f(p1[0],p1[1]);
            p2_2d=Point2f(p2[0],p2[1]);
            p_2d =Point2f(p[0],p[1]);

            break;
        }
    }




    Triangle2<float> f_2d=Triangle2<float>(p0_2d,p1_2d,p2_2d);



    return IsInsideTrianglePoint(f_2d,p_2d);

};





float getElapsedTime(CMeshO::CoordType pi,CMeshO::CoordType pf,Point3f vi,Point3f a){

    float t1=0;
    float t2=0;
    float s=pi[0]-pf[0];
    float d=pow(vi[0],2)- 2*a[0]*s;
    if(d<0) d=0;
    t1=(-vi[0]+sqrt(d))/a[0];
    t2=(-vi[0]-sqrt(d))/a[0];
    if(t1>0) return t1;
    if(t2>0) return t2;
    return 0;

};

float getElapsedTime(CMeshO::CoordType vi,CMeshO::CoordType vf,CMeshO::FacePointer face,float m,CMeshO::CoordType force){

    float t=0;
    float v_initial=sqrt(pow(vi[0],2)+pow(vi[1],2)+pow(vi[2],2));
    float v_final=sqrt(pow(vf[0],2)+pow(vf[1],2)+pow(vf[2],2));
    Point3f n= face->N();
    float a=n[0]*force[0]+n[1]*force[1]+n[2]*force[2];



    Point3f f;

    f[0]=force[0]-a*n[0];
    f[1]=force[1]-a*n[1];
    f[2]=force[2]-a*n[2];

    float acceleration=sqrt(pow(f[0]/m,2)+pow(f[1]/m,2)+pow(f[2]/m,2));
    t=(v_final-v_initial)/acceleration;


    return t;
};

/**
@def

*/
bool IsOnEdge(CMeshO::CoordType p,CMeshO::FacePointer f){
    if(f==0) return false;
    float bc[3];
    //f->C()=Color4b::Green;
    InterpolationParameters(*f,f->N(),p,bc[0],bc[1],bc[2]);
    if(bc[0]==0.0f || bc[1]==0.0f || bc[2]==0.0f) return true;
    return false;
};

CMeshO::CoordType GetVelocityComponents(float v,float l,CMeshO::FacePointer face){

    Point3f dir = Point3f(0,-1,0);
    Point3f n=face->N();
    //n.Normalize();
    float a = dir.dot(n);
    Point3f vel;

    vel[0]=dir[0]-n[0]*a;
    vel[1]=dir[1]-n[1]*a;
    vel[2]=dir[2]-n[2]*a;


    Point3f axis_x=Point3f(1,0,0);
    Point3f axis_y=Point3f(0,1,0);
    Point3f axis_z=Point3f(0,0,1);

    Point3f nx=Point3f(n[0],0,0);
    Point3f ny=Point3f(0,n[1],0);
    Point3f nz=Point3f(0,0,n[2]);
    float alpha=90-(acos(axis_x.dot(nx))*(180/PI));
    float beta =90-(acos(axis_y.dot(ny))*(180/PI));
    float gamma=90-(acos(axis_z.dot(nz))*(180/PI));

    //vel[0]=v*cos(alpha*PI/180)*l;
    //vel[1]=v*cos(beta*PI/180)*l;
    //vel[2]=v*cos(gamma*PI/180)*l;

    vel[0]=vel[0]*l*v;
    vel[1]=vel[1]*l*v;
    vel[2]=vel[2]*l*v;

    return vel;




};

CMeshO::CoordType ComputeVelocity(CMeshO::CoordType vi,CMeshO::CoordType ac,float t){

    CMeshO::CoordType n_vel;

    n_vel[0]=vi[0]+ac[0]*t;
    n_vel[1]=vi[1]+ac[1]*t;
    n_vel[2]=vi[2]+ac[2]*t;

    return n_vel;
};
CMeshO::CoordType UpdateVelocity(CMeshO::CoordType pf,CMeshO::CoordType pi,CMeshO::CoordType v,float m,CMeshO::FacePointer &face,CMeshO::CoordType force){

    CMeshO::CoordType new_vel;
    Point3f n= face->cN();
    float a=n[0]*force[0]+n[1]*force[1]+n[2]*force[2];
    //float b=n[0]*vel[0]+n[1]*vel[1]+n[2]*vel[2];

    Point3f f;
    //Point3f v;

    f[0]=force[0]-a*n[0];
    f[1]=force[1]-a*n[1];
    f[2]=force[2]-a*n[2];


    //v[0]=vel[0]-b*n[0];
    //v[1]=vel[1]-b*n[1];
    //v[2]=vel[2]-b*n[2];

    new_vel[0]=sqrt(pow(v[0],2)+2*(f[0]/m)*(pf[0]-pi[0]) );
    new_vel[1]=sqrt(pow(v[1],2)+2*(f[1]/m)*(pf[1]-pi[1]) );
    new_vel[2]=sqrt(pow(v[2],2)+2*(f[2]/m)*(pf[2]-pi[2]) );


    return new_vel;
};

CMeshO::CoordType ComputeAcceleration(float m,CMeshO::FacePointer face,CMeshO::CoordType dir){
    CMeshO::CoordType acc;

    Point3f n= face->N();
    float a=n[0]*dir[0]+n[1]*dir[1]+n[2]*dir[2];

    acc[0]=dir[0]-a*n[0];
    acc[1]=dir[1]-a*n[1];
    acc[2]=dir[2]-a*n[2];

    return acc;
};

/**
  @description Simulate the movement of a point, affected by a force "dir" on a face.

  @param CoordType p   - coordinates of the point
  @param CoordType v   - velocity of the particle
  @param float     m   - mass of the particle
  @param FaceType face - pointer to the face
  @param CoordType dir - direction of the force
  @param float     t   - time step

  @return new coordinates of the point
*/
CMeshO::CoordType StepForward(CMeshO::CoordType p,
                              float v,
                              float m,
                              CMeshO::FacePointer &face,
                              CMeshO::CoordType dir,float l,
                              float t=1){

   /* if(v<0.005 && IsOnEdge(p,face))
        return p;
    */



    dir=dir*l;
    Point3f new_pos;
    Point3f n= face->N();
    float a=n[0]*dir[0]+n[1]*dir[1]+n[2]*dir[2];

//    float b=n[0]*vel[0]+n[1]*vel[1]+n[2]*vel[2];



    Point3f f;
    Point3f vel=GetVelocityComponents(v,l,face);
    //Calcolo le componenti della forza lungo la faccia
    f[0]=dir[0]-a*n[0];
    f[1]=dir[1]-a*n[1];
    f[2]=dir[2]-a*n[2];


    new_pos[0]=p[0]+vel[0]*t+0.5*(f[0]/m)*pow(t,2);
    new_pos[1]=p[1]+vel[1]*t+0.5*(f[1]/m)*pow(t,2);
    new_pos[2]=p[2]+vel[2]*t+0.5*(f[2]/m)*pow(t,2);

    return new_pos;
};

void DrawDirt(MeshModel *m/*,std::vector<Point3f> &dp*/){
        float base_color=255;
        float s_color;
        std::pair<float,float> minmax = tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(m->cm);
        CMeshO::FaceIterator fi;
        for(fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi)
        {
            s_color=base_color*(1-(((*fi).Q()-minmax.first)/(minmax.second-minmax.first)));
            (*fi).C()=Color4b(s_color, s_color, s_color, 0);
        }
};


/**
@def Compute the intersection of the segment from p1 to p2 and the face f

@param CoordType p1 - position of the first point
@param Coordtype p2 - position of the second poin
@param Facepointer f - pointer to the face
@param CoordType int_point - intersection point this is a return parameter for the function.
@param FacePointer face - pointer to the new face

@return true if there is an intersection
*/
bool ComputeIntersection(CMeshO::CoordType p1,CMeshO::CoordType p2,CMeshO::FacePointer &f,CMeshO::FacePointer &new_f,CMeshO::CoordType &int_point)
{

    CMeshO::CoordType n=f->N().Normalize();
    //n.Normalize();
    int max_c;

    float n0=math::Abs(n[0]);
    float n1=math::Abs(n[1]);
    float n2=math::Abs(n[2]);

    if(n0>n1){
          if(n0>n2) max_c=0;
          else max_c=2;
    }else{
        if(n1>n2) max_c=1;
        else max_c=2;
              }
    Point2f int_p;

    Line2f seg;

    CMeshO::CoordType fv0 = f->P(0);//(f->V(0))->P();
    CMeshO::CoordType fv1 = f->P(1);//(f->V(1))->P();
    CMeshO::CoordType fv2 = f->P(2);//(f->V(2))->P();
    Segment2f line0;
    Segment2f line1;
    Segment2f line2;
    Point2f p1_2d;
    Point2f p2_2d;
    switch(max_c){
    case 0:{
            line0=Segment2f(Point2f(fv0[1],fv0[2]),Point2f(fv1[1],fv1[2]));
            line1=Segment2f(Point2f(fv1[1],fv1[2]),Point2f(fv2[1],fv2[2]));
            line2=Segment2f(Point2f(fv2[1],fv2[2]),Point2f(fv0[1],fv0[2]));
            p1_2d=Point2f(p1[1],p1[2]);
            p2_2d=Point2f(p2[1],p2[2]);
            seg=Line2f(p1_2d,p2_2d);

            break;
        }
    case 1:{
            line0=Segment2f(Point2f(fv0[0],fv0[2]),Point2f(fv1[0],fv1[2]));
            line1=Segment2f(Point2f(fv1[0],fv1[2]),Point2f(fv2[0],fv2[2]));
            line2=Segment2f(Point2f(fv2[0],fv2[2]),Point2f(fv0[0],fv0[2]));
            p1_2d=Point2f(p1[0],p1[2]);
            p2_2d=Point2f(p2[0],p2[2]);
            seg=Line2f(p1_2d,p2_2d);
            break;
        }
    case 2:{
            line0=Segment2f(Point2f(fv0[0],fv0[1]),Point2f(fv1[0],fv1[1]));
            line1=Segment2f(Point2f(fv1[0],fv1[1]),Point2f(fv2[0],fv2[1]));
            line2=Segment2f(Point2f(fv2[0],fv2[1]),Point2f(fv0[0],fv0[1]));
            p1_2d=Point2f(p1[0],p1[1]);
            p2_2d=Point2f(p2[0],p2[1]);
            seg=Line2f(p1_2d,p2_2d);
            break;
        }
    }


    Point2f tmp_int;
    bool int_found=false;
    int edge;
    float dist=0;
    if(LineSegmentIntersection(seg,line0,tmp_int)){
        dist=Distance(p2_2d,tmp_int);
        edge=0;
        int_p=tmp_int;
        int_found=true;
    }


    if(LineSegmentIntersection(seg,line1,tmp_int)){
        if(int_found){
            if(Distance(p2_2d,tmp_int)<dist){
            dist=Distance(p2_2d,tmp_int);
            edge=1;
            int_p=tmp_int;
            }

        }else{
            int_found=true;
            dist=Distance(p2_2d,tmp_int);
            edge=1;
            int_p=tmp_int;
         }

    }


    if(LineSegmentIntersection(seg,line2,tmp_int)){
        if(int_found){
            if(Distance(p2_2d,tmp_int)<dist){
            dist=Distance(p2_2d,tmp_int);
            edge=2;
            int_p=tmp_int;
            }

        }else{
            int_found=true;
            dist=Distance(p2_2d,tmp_int);
            edge=2;
            int_p=tmp_int;
         }

    }




    if(!int_found){
        Point3f bc;
        InterpolationParameters(*f,f->N(),p1,bc);
        int p=1;
    }

    /*
    if(SegmentSegmentIntersection(line0,seg,tmp)){
        if(tmp!=p1_2d){
            int_p=tmp;
            edge=0;
            int_found=true;
        }
     }
        if(SegmentSegmentIntersection(line1,seg,tmp)){
            if(tmp!=p1_2d){
                int_p =tmp;
                edge=1;
                int_found=true;
            }
        }
        if(SegmentSegmentIntersection(line2,seg,tmp)){
            if(tmp!=p1_2d){
                int_p =tmp;
                edge=2;
                int_found=true;
            }
        }
    */

    /*
    bool int_found=false;
    int edge=-1;
    Point2f tmp;
    if(SegmentSegmentIntersection(line0,seg,tmp)){
       int_found=true;
       int_p=tmp;
       edge=0;
    }
    if(SegmentSegmentIntersection(line1,seg,tmp)){
        if(int_found){

            if(Segment2f(tmp,p2_2d).Length()<Segment2f(int_p,p2_2d).Length()){
            int_found=true;
            int_p=tmp;
            edge=1;
            }
        }else{
            int_found=true;
            int_p=tmp;
            edge=1;
        }
    }
    if(SegmentSegmentIntersection(line2,seg,tmp)){
        if(int_found){
            if(Segment2f(tmp,p2_2d).Length()<Segment2f(int_p,p2_2d).Length()){
            int_found=true;
            int_p=tmp;
            edge=2;
            }
        }else{
            int_found=true;
            int_p=tmp;
            edge=2;
        }
    }

    */

    if(int_found){
        new_f=f->FFp(edge);
        switch(max_c){
            case 0:{
                int_point[0]=(n[0]*fv0[0]-n[1]*(int_p[0]-fv0[1])-n[2]*(int_p[1]-fv0[2]))/n[0];
                int_point[1]=int_p[0];
                int_point[2]=int_p[1];
                break;
                }
            case 1:{
                int_point[0]=int_p[0];
                int_point[1]=(n[1]*fv0[1]-n[0]*(int_p[0]-fv0[0])-n[2]*(int_p[1]-fv0[2]))/n[1];
                int_point[2]=int_p[1];
                break;
                }
            case 2:{
                int_point[0]=int_p[0];
                int_point[1]=int_p[1];
                int_point[2]=(n[2]*fv0[2]-n[0]*(int_p[0]-fv0[0])-n[1]*(int_p[1]-fv0[1]))/n[2];
                break;
              }
          }
    }

    /*      if(int_found){
       Point3f bc;
       InterpolationParameters(*new_f,new_f->N(),int_point,bc);
       int min_c;

       if(bc[0]<bc[1]){
           if(bc[0]<bc[2]){
            min_c=0;
           }else{
            min_c=2;
           }
        }else{
            if(bc[1]<bc[2]){
            min_c=1;
            }else{
             min_c=2;
            }
                }

        switch(edge){

        case 0:{
            bc[0]=0;
            bc[1]=bc[1];
            bc[2]=1-bc[1]-0.0001;
            break;
            }
        case 1:{
                bc[0]=bc[0];
                bc[1]=0;
                bc[2]=1-bc[0]-0.0001;
                break;
            }
        case 2:{
                bc[0]=bc[0];
                bc[1]=1-bc[0]-0.0001;
                bc[2]=0;
                break;
            }
        }


        Point3f nf0=new_f->P(0);
        Point3f nf1=new_f->P(1);
        Point3f nf2=new_f->P(2);
        int_point[0]=nf0[0]*bc[0]+nf1[0]*bc[1]+nf2[0]*bc[2];
        int_point[1]=nf0[1]*bc[0]+nf1[1]*bc[1]+nf2[1]*bc[2];
        int_point[2]=nf0[2]*bc[0]+nf1[2]*bc[1]+nf2[2]*bc[2];

    }
*/

    return int_found;
};

/**
@description generate random baricentric coordinates
*/
CMeshO::CoordType RandomBaricentric(){
    CMeshO::CoordType interp;

    static math::MarsenneTwisterRNG rnd;
    interp[1] = rnd.generate01();
    interp[2] = rnd.generate01();
        if(interp[1] + interp[2] > 1.0)
        {
                interp[1] = 1.0 - interp[1];
                interp[2] = 1.0 - interp[2];
                }

        assert(interp[1] + interp[2] <= 1.0);
        interp[0]=1.0-(interp[1] + interp[2]);
        return interp;
    };

/**
@def Compute the Normal Dust Amount Function per face of a Mesh m

@param m MeshModel
@param u CoordType dust direction
@param k float
@param s float
*/
bool ComputeNormalDustAmount(MeshModel* m,CMeshO::CoordType u,float k,float s){

    //Verify if FaceQualty is enabled




    CMeshO::FaceIterator fi;
    float d;
    for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
        d=k/s+(1+k/s)*pow(fi->N().dot(u),s);

        fi->Q()=d;
    }
    return true;
};

/**
@description This function compute the Surface Exposure per face of a Mesh m

@param  MeshModel* m - Pointer to the new mesh
@param int r - scaling factor
@param int n_ray - number of rays emitted

*/
void ComputeSurfaceExposure(MeshModel* m,int r,int n_ray){
    CMeshO::FaceIterator fi;
    CMeshO::FaceIterator fi0;

    CMeshO::CoordType b_c;
    CMeshO::CoordType bp_i; //Barycentric coordinates
    b_c[0]=0.3f;
    b_c[1]=0.3f;
    b_c[2]=1-(b_c[0]+b_c[1]);
    CMeshO::CoordType p_c;
    float dh=1.2;
    CMeshO::PerFaceAttributeHandle<float> eh=vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<float>(m->cm,std::string("exposure"));

    float exp=0;
    float distance=0;
    float u=0;
    float v=0;
    float t=0;
/*
    for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
        eh[fi]=1;
    }
*/
    for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
        if(fi->Q()!=0){

        //For every face  get the central point
        p_c[0]=fi->P(0)[0]*b_c[0]+fi->P(1)[0]*b_c[1]+fi->P(2)[0]*b_c[2];
        p_c[1]=fi->P(0)[1]*b_c[0]+fi->P(1)[1]*b_c[1]+fi->P(2)[1]*b_c[2];
        p_c[2]=fi->P(0)[2]*b_c[0]+fi->P(1)[2]*b_c[1]+fi->P(2)[2]*b_c[2];

        //Create a ray with p_c as origin and direction dir
        Ray3<float> ray=Ray3<float>(p_c,fi->N());

        exp=0;
        distance=0;



        for(fi0=m->cm.face.begin();fi0!=m->cm.face.end();++fi0){
            u=0;
            v=0;
            t=0;
            if(fi!=fi0 && IntersectionRayTriangle(ray,fi0->P(0),fi0->P(1),fi0->P(2),t,u,v)){
                if(distance==0 || t<distance ){
                    distance=t;
                    exp=dh/(dh+distance);
                }
            }

          }
        eh[fi]=1-(exp/n_ray);
    }else{
        eh[fi]=0;
     }
    }

};

void CreateDustTexture(MeshModel* m){
    QString textName="dust_texture";
    QString fileName(m->fullName());
    fileName = fileName.left(std::max<int>(fileName.lastIndexOf('\\'),fileName.lastIndexOf('/'))+1).append(textName);
    QFile textFile(fileName);
    QImage img(640, 640, QImage::Format_RGB32);
    img.fill(qRgb(0,0,0)); // black
    img.save(fileName,"jpg");
    m->cm.textures.clear();
    m->cm.textures.push_back(textName.toStdString());

}

/**
@def

@param
@param
@param

@return ?
*/
bool GenerateParticles(MeshModel* m,std::vector<CMeshO::CoordType> &cpv,std::vector< Particle<CMeshO> > &dpv,int d,float threshold){

    //Exposure Handler
    CMeshO::PerFaceAttributeHandle<float> eh=vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<float>(m->cm,std::string("exposure"));

    CMeshO::FaceIterator fi;
    CMeshO::CoordType p;
    cpv.clear();
    dpv.clear();



    /*float a1=1+1*eh[fi];
    if(a1<0){
    a=0;
    }
    if(a1>1){
    a=1;
    }
    if(a1>=0 && a1<=1){
       a=a1;
    }
*/
    for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
        float a=0;
        if(eh[fi]<1) a=0;
        else a=1;
        int n_dust=(int)d*fi->Q()*a;

        if(fi->Q()>threshold){
            for(int i=0;i<n_dust;i++){
               p=RandomBaricentric();
               CMeshO::CoordType n_p;
               n_p[0]=fi->P(0)[0]*p[0]+fi->P(1)[0]*p[1]+fi->P(2)[0]*p[2];
               n_p[1]=fi->P(0)[1]*p[0]+fi->P(1)[1]*p[1]+fi->P(2)[1]*p[2];
               n_p[2]=fi->P(0)[2]*p[0]+fi->P(1)[2]*p[1]+fi->P(2)[2]*p[2];
               cpv.push_back(n_p);
               Particle<CMeshO> part;
               part.face=&(*fi);
               part.bar_coord=p;
               dpv.push_back(part);
               }
            }
         fi->Q()=n_dust;
        }


return true;
};







void associateParticles(MeshModel* b_m,MeshModel* c_m,float m,float v){
    CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph= tri::Allocator<CMeshO>::AddPerVertexAttribute<Particle<CMeshO> > (c_m->cm,std::string("ParticleInfo"));
    CMeshO::FaceIterator fi;
    CMeshO::VertexIterator vi;
    for(fi=b_m->cm.face.begin();fi!=b_m->cm.face.end();++fi){
        for(vi=c_m->cm.vert.begin();vi!=c_m->cm.vert.end();++vi){
            if(IsOnFace( vi->P(),&*fi)){
            Particle<CMeshO> part;
            fi->C()=Color4b::Blue;
            part.face=&*fi;
            part.mass=m;
            part.vel=v;
            ph[vi]=part;
            }
        }
    }
};



void prepareMesh(MeshModel* m){
    //clean flags

    tri::UpdateFlags<CMeshO>::FaceClear(m->cm);

    m->updateDataMask(MeshModel::MM_FACEFACETOPO);
    m->updateDataMask(MeshModel::MM_FACEMARK);
    m->updateDataMask(MeshModel::MM_FACECOLOR);
    m->updateDataMask(MeshModel::MM_VERTQUALITY);
    m->updateDataMask(MeshModel::MM_FACEQUALITY);
    //m->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
    tri::UnMarkAll(m->cm);

    //clean Mesh
    tri::Allocator<CMeshO>::CompactFaceVector(m->cm);

    tri::Clean<CMeshO>::RemoveUnreferencedVertex(m->cm);
    tri::Clean<CMeshO>::RemoveDuplicateVertex(m->cm);
    tri::Allocator<CMeshO>::CompactVertexVector(m->cm);



    //update Mesh
    tri::UpdateTopology<CMeshO>::FaceFace(m->cm);
    tri::UpdateNormals<CMeshO>::PerFaceNormalized(m->cm);
    tri::UpdateFlags<CMeshO>::FaceProjection(m->cm);

}


bool MoveCloudMeshForward(MeshModel *cloud,Point3f force,float l,float time){
    CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph = Allocator<CMeshO>::GetPerVertexAttribute<Particle<CMeshO> >(cloud->cm,"ParticleInfo");
    CMeshO::VertexIterator vi;
    CMeshO::FacePointer current_face;
    CMeshO::FacePointer new_face;
    float velocity;
    float mass;
    CMeshO::CoordType new_pos;
    CMeshO::CoordType current_pos;
    CMeshO::CoordType int_pos;

    for(vi=cloud->cm.vert.begin();vi!=cloud->cm.vert.end();++vi){
        float t=time;

        current_face=ph[vi].face;
        new_face=current_face   ;
        current_pos=vi->P();

        velocity=ph[vi].vel;
        mass=ph[vi].mass;

        new_pos=StepForward(vi->P(),velocity,mass,current_face,force,l,t);

        while(!IsOnFace(new_pos,current_face)){
            bool tmp=ComputeIntersection(current_pos,new_pos,current_face,new_face,int_pos);
            new_face->C()=Color4b::Yellow;
            if(!tmp){
                break;
            }
            new_pos=int_pos;

           //Debug
            current_face->C()=Color4b::Black;
            current_face=new_face;

            t=t/2;
            if(t>0.5){
            new_pos=StepForward(vi->P(),velocity,mass,current_face,force,l,t);
            }else{
                current_face->C()=Color4b::Green;
                if(!IsOnFace(new_pos,current_face)){
                    CoordinateCorrection(new_pos,current_face);
                    Point3f bc;
                    InterpolationParameters(*new_face,new_pos,bc);
                    current_face->C()=Color4b::Red;

                }

            }
        }

        vi->P()=new_pos;
        ph[vi].face=current_face;

    }

    return true;
};

#endif // DIRT_UTILS_H
