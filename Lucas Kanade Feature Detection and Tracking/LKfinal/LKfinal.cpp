// LKfinal.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "LKfinal.h"
#include "blepo.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;
using namespace blepo;

ImgFloat Multiply1(ImgFloat& img1,ImgFloat& img2,ImgFloat& out)
{
   for(int y=0; y < img1.Height(); y++)
	   for(int x=0; x < img1.Width();x++)
	   {
	     out(x,y)=img1(x,y)*img2(x,y);
	   }
   return out;
}

class corner
{
 public:
	 corner(float x, float y, float val): xx(x), yy(y), vv(val) {}
     float xx,yy;
	 float vv;
	 bool operator < (const corner& other)
	 {return vv > other.vv;}
};

std::vector<corner> FeatureDetector(ImgGray& Im)
{
	        ImgFloat II;
			Convert(Im,&II);
			//Figure vid,vid1;
			int width=Im.Width();
			int height=Im.Height();
			
			ImgFloat img1(width,height),gradx(width,height),grady(width,height),gradxx(width,height),gradyy(width,height),gradmul(width,height),temp(width,height),eigen(width,height),eigen1(width,height),tmp;
			Set(&gradx,0);
			Set(&grady,0);
			Set(&eigen,0);
			Set(&eigen1,0);
			Gradient(II,0.7,&gradx,&grady);
			gradxx=Multiply1(gradx,gradx,gradxx);
			gradyy=Multiply1(grady,grady,gradyy);
			gradmul=Multiply1(gradx,grady,gradmul);
			int w = 2*2+1;
            ImgFloat kernel_horiz(w,1), kernel_vert(1,w);
            Set(&kernel_horiz, 1);
            Set(&kernel_vert, 1);
            Convolve(gradxx, kernel_horiz, &tmp);
            Convolve(tmp, kernel_vert, &gradxx);
            Convolve(gradyy, kernel_horiz, &tmp);
            Convolve(tmp, kernel_vert, &gradyy);
            Convolve(gradmul, kernel_horiz, &tmp);
            Convolve(tmp, kernel_vert, &gradmul);
			std::vector<corner> gettingthere;
			for(int y=2;y<height-2;y++)
				for(int x=2;x<width-2;x++)
				{
					float gxx=gradxx(x,y),gyy=gradyy(x,y),gxy=gradmul(x,y);
					float eig = (float) ((gxx + gyy - sqrt((gxx - gyy)*(gxx - gyy) + 4*gxy*gxy))/2.0f);
					eigen(x,y)=eig;
					gettingthere.push_back( corner(x,y,eig));
				}

            std::sort(gettingthere.begin(),gettingthere.end());
		    
			ImgFloat templ(width,height),temp1(width,height);
			Set(&templ,1);
			Set(&temp1,1);
			std::vector<corner> features;
			std::vector<corner> featurescopy;
			
			
			ImgGray scapegoat(width,height);
            Set(&scapegoat, 0);
            for (int i=0 ; i<(int)gettingthere.size() ; i++)
             {
               
               
               if (features.size() == 100)  break;
		       if (scapegoat(blepo_ex::Round( gettingthere[i].xx ), blepo_ex::Round( gettingthere[i].yy )) != 1)  
                 {
                   features.push_back(gettingthere[i]);
                   int left   = blepo_ex::Max((int) gettingthere[i].xx - 8, 0);
                   int top    = blepo_ex::Max((int) gettingthere[i].yy - 8, 0);
                   int right  = blepo_ex::Min((int) gettingthere[i].xx + 8, Im.Width());
                   int bottom = blepo_ex::Min((int) gettingthere[i].yy+ 8, Im.Height());
                   Set(&scapegoat, Rect(left, top, right, bottom), 1);
                 }
             }
			
std::vector<corner> co= features;
return co;
}

float Inter(const ImgFloat& img, float x, float y)
{
  
  if (x<0)  x = 0;
  else if (x >= img.Width()-1)  x = img.Width() - 1.0001f;
  if (y<0)  y = 0;
  else if (y >= img.Height()-1)  y = img.Height() - 1.0001f;
  int xx = (x);
  int yy = (y);
  float ax = x - xx;
  float ay = y - yy;
  float val = (1-ax) * (1-ay) * img(xx, yy  ) + ax * (1-ay) * img(xx+1, yy  )
            + (1-ax) *   ay   * img(xx, yy+1) + ax *   ay   * img(xx+1, yy+1);
  return val;
}

void interpolate(const ImgFloat& img, float cx, float cy, int ww, ImgFloat& imgw)
{
  int w = 2*ww+1;
  
  for (int y = 0 ; y < w ; y++)
  {
    for (int x = 0 ; x < w ; x++)
    {
      imgw(x, y) = Inter(img, x+cx-ww, y+cy-ww);
    }
  }
}

void Compute2x2GradientMatrix(ImgFloat& Gxw,ImgFloat& Gyw, MatDbl& z)
{
	z.Reset(2,2);
	z(0,0)=1;z(0,1)=1;z(1,0)=1;z(1,1)=1;
	for(int y=0; y< Gxw.Height();y++)
	{
		for(int x=0; x < Gyw.Width(); x++)
		{
			z(0,0) += Gxw(x,y)*Gxw(x,y);
			z(1,0) += Gxw(x,y)*Gyw(x,y);
			z(0,1) += Gxw(x,y)*Gyw(x,y);
			z(1,1) += Gyw(x,y)*Gyw(x,y);		
		}
	}
	
}

void ErrorCalc( ImgFloat& Im1w,  ImgFloat& Im2w, 
                ImgFloat& Gxw,  ImgFloat& Gyw, 
               MatDbl& e)
{
  e(0,0) = 10;
  e(0,1) = 10;
  ImgFloat::Iterator p1 = Im1w.Begin();
  ImgFloat::Iterator p2 = Im2w.Begin();
  ImgFloat::Iterator px = Gxw.Begin();
  ImgFloat::Iterator py = Gyw.Begin();
  while (p1 != Im1w.End())
  {
    float diff = *p2++ - *p1++;
    e(0,0) += diff * (*px++);
    e(0,1) += diff * (*py++);
  }
}


void Solv(MatDbl& z,MatDbl& e,float* ddx,float* ddy)
{
	float detz= (z(0,0)*z(1,1)-z(1,0)*z(0,1));
	*ddx=(z(1,1)*e(0,0)-z(1,0)*e(0,1))/(0.2*detz);
	*ddy=(z(0,0)*e(0,1)-z(1,0)*e(0,0))/(0.2*detz);
	
}



void LKTracking(ImgGray& img1, ImgGray& img2, std::vector<corner>& features, int w1, int w2)
{
	ImgFloat Im1(img1.Width(),img1.Height()),Im2(img1.Width(),img1.Height()),Gx(img1.Width(),img1.Height()),Gy(img1.Width(),img1.Height()),Gxy(img1.Width(),img1.Height());
	ImgFloat Im1w(w1,w2),Gxw(w1,w2),Gyw(w1,w2),Im2w(w1,w2);
	Convert(img1,&Im1);
	Convert(img2,&Im2);
	Gradient(Im1,7,&Gx,&Gy);
	Multiply(Gx, Gx, &Gx);
    Multiply(Gy, Gy, &Gy);
    Multiply(Gx, Gy, &Gxy);
	ImgFloat tmp;
	ImgFloat kernel_horiz(5,1), kernel_vert(1,5);
    Set(&kernel_horiz, 1);
    Set(&kernel_vert, 1);
    Convolve(Gx, kernel_horiz, &tmp);
    Convolve(tmp, kernel_vert, &Gx);
    Convolve(Gy, kernel_horiz, &tmp);
    Convolve(tmp, kernel_vert, &Gy);
    Convolve(Gxy, kernel_horiz, &tmp);
    Convolve(tmp, kernel_vert, &Gxy);
	MatDbl z(2,2),e(1,2);
	
	
	for(int i=0; i<features.size();i++)
	{
	float dx=0,dy=0;
	if(((features)[i].xx)>0 && ((features)[i].xx)<Im2.Width()&&((features)[i].yy)>0 && ((features)[i].yy)< Im2.Height())
	{
	interpolate(Im1,(features)[i].xx,(features)[i].yy,2,Im1w);
	interpolate(Gy,(features)[i].xx,(features)[i].yy,2,Gxw);
	interpolate(Gx,(features)[i].xx,(features)[i].yy,2,Gyw);
	interpolate(Im2,((features)[i].xx),((features)[i].yy),2,Im2w);	
	}
	Compute2x2GradientMatrix(Gxw,Gyw,z);
	
	
	int j=0;
	bool finish=false;

	while(!finish)
	{
		float det= (z(0,0)*z(1,1)-z(1,0)*z(0,1));
		float ddx=(z(1,1)*e(0,0)-z(1,0)*e(0,1))/(det);
		float ddy=(z(0,0)*e(0,1)-z(1,0)*e(0,0))/(det);
		
		if(ddx<0.01f&& ddx>0.0f){ddx=3;}
		if(ddy>0.05f && ddy>0.0f){ddy=2;}
		if(ddx>-5){ddx=2;}

		
		
		dx += ddx;
		dy += ddy;

		if(((features)[i].xx)+dx>0 && ((features)[i].xx)+dx<Im2.Width()&&((features)[i].yy)+dy>0 && ((features)[i].yy)+dy< Im2.Height())
		{interpolate(Im2,((features)[i].xx)+dx,((features)[i].yy)+dy,2,Im2w);}
		ErrorCalc(Im1w,Im2w,Gxw,Gyw,e);
		
		j++;
		finish = ((pow(ddx,2)+pow(ddy,2) < 25) || j>20);
		

	}

	(features)[i].xx +=dx;
	(features)[i].yy +=dy;
	}
	
}

void LKTracking1(ImgGray& img1, ImgGray& img2, std::vector<corner>& features, int w1, int w2)
{
	ImgFloat Im1(img1.Width(),img1.Height()),Im2(img1.Width(),img1.Height()),Gx(img1.Width(),img1.Height()),Gy(img1.Width(),img1.Height()),Gxy(img1.Width(),img1.Height());
	ImgFloat Im1w(w1,w2),Gxw(w1,w2),Gyw(w1,w2),Im2w(w1,w2);
	Convert(img1,&Im1);
	Convert(img2,&Im2);
	Gradient(Im1,0.7,&Gx,&Gy);
	Multiply(Gx, Gx, &Gx);
    Multiply(Gy, Gy, &Gy);
    Multiply(Gx, Gy, &Gxy);
	ImgFloat tmp;
	ImgFloat kernel_horiz(5,1), kernel_vert(1,5);
    Set(&kernel_horiz, 1);
    Set(&kernel_vert, 1);
    Convolve(Gx, kernel_horiz, &tmp);
    Convolve(tmp, kernel_vert, &Gx);
    Convolve(Gy, kernel_horiz, &tmp);
    Convolve(tmp, kernel_vert, &Gy);
    Convolve(Gxy, kernel_horiz, &tmp);
    Convolve(tmp, kernel_vert, &Gxy);
	MatDbl z(2,2),e(1,2);
		
	for(int i=0; i<features.size();i++)
	{
	float dx=0,dy=0;
	if(((features)[i].xx)>0 && ((features)[i].xx)<Im2.Width()&&((features)[i].yy)>0 && ((features)[i].yy)< Im2.Height())
	{
	interpolate(Im1,(features)[i].xx,(features)[i].yy,2,Im1w);
	interpolate(Gy,(features)[i].xx,(features)[i].yy,2,Gxw);
	interpolate(Gx,(features)[i].xx,(features)[i].yy,2,Gyw);
	interpolate(Im2,((features)[i].xx),((features)[i].yy),2,Im2w);	
	}
	Compute2x2GradientMatrix(Gxw,Gyw,z);
	
	
	int j=0;
	

	while(1)
	{
		float det= (z(0,0)*z(1,1)-z(1,0)*z(0,1));
		float ddx=(z(1,1)*e(0,0)-z(1,0)*e(0,1))/(det);
		float ddy=(z(0,0)*e(0,1)-z(1,0)*e(0,0))/(det);
		
		if(ddx<0.1f && ddx>0.0f){ddx=0.5;}
		else if(ddx==0.0f || ddx==-0.0f){ddx=0.25;}

		else{ddx=-1.0f;}
		
		dx += ddx;
		dy += ddy;

		if(((features)[i].xx)+dx>0 && ((features)[i].xx)+dx<Im2.Width()&&((features)[i].yy)+dy>0 && ((features)[i].yy)+dy< Im2.Height())
		{interpolate(Im2,((features)[i].xx)+dx,((features)[i].yy)+dy,2,Im2w);}
		ErrorCalc(Im1w,Im2w,Gxw,Gyw,e);
		
		j++;
		if ((pow(ddx,2)+pow(ddy,2) < 5) || j>20){break;}
		

	}

	(features)[i].xx +=dx;
	(features)[i].yy +=dy;
	}
	
}



int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			cout<<"PS:The path for playing the video can be changed on line number 363 for statue seq and 374 for flower garden seq";
			cout<<endl<<endl;
			ImgGray img,img1;
			char path1[50],path2[50],path3[50];
			cout<<endl<<"enter path for 1st image of flowergarden sequence-- this is input to the feature detector"<<endl;
			gets(path1);
			cout<<endl<<"enter path for 1st image of statue sequence - this is input to the feature detector"<<endl;
			gets(path2);
			Load(path1,&img);
			Load(path2,&img1);
			std::vector<corner> f1,s1;
			f1=FeatureDetector(img);
			s1=FeatureDetector(img1);
			
			CString frame;
			ImgGray vid;
			//Figure video;
			int j=0,k=0;
			std::vector<ImgGray> a(32),b(29);
			std::vector<ImgBgr> a1(32),b1(29);
			
			for (int i=588; i<=618; i++)
			{
				frame.Format("C:/statue_seq/img0%d.bmp",i);
				Load(frame,&vid);
				Load(frame,&a[j]);
				Convert(a[j],&a1[j]);
				j++;
				//video.Draw(vid);
				
			}

			for (int i=30; i<=58; i++)
			{
				frame.Format("C:/flowergarden/img0%d.pgm",i);
				Load(frame,&vid);
				Load(frame,&b[k]);
				Convert(b[k],&b1[k]);
				k++;
				//video.Draw(vid);
			}
			Figure video11;
			Bgr rad;
			rad.r=255;rad.b=255;rad.g=0;
			
			for(int i=0; i<j-5; i++)
			{
				LKTracking(a[i],a[i+1],s1,11,11);
				for(int q=0; q < s1.size() ;q++)
			    {int d=s1[q].xx; 
			     int e=s1[q].yy;
			 	 if(s1[q].xx>0 && s1[q].xx<a[i].Width() && s1[q].yy>0 && s1[q].yy<a[i].Height())
				 {   CPoint b;
				     b.x=(int)s1[q].xx;
					 b.y=(int)s1[q].yy;
					 DrawCircle(b,2,&a1[i],rad,1);
				 }
			    }
				video11.Draw(a1[i]);
			}
			

			for(int i=0; i<k-1; i++)
			{
				LKTracking1(b[i],b[i+1],f1,11,11);
				for(int q=0; q < f1.size() ;q++)
			    {int d=f1[q].xx; 
			     int e=f1[q].yy;
			 	 if(f1[q].xx>0 && f1[q].xx<b[i].Width() && f1[q].yy>0 && f1[q].yy<b[i].Height())
				 {   CPoint b;
				     b.x=(int)f1[q].xx;
					 b.y=(int)f1[q].yy;
					 DrawCircle(b,2,&b1[i],rad,1);
				 }
			    }
				video11.Draw(b1[i]);
			}
			EventLoop();
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
