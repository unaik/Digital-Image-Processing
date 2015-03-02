// assignment2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "assignment2.h"
#include "blepo.h"
#include <iostream>
#include <vector>
#include <string>
#include <stack>

#define PI 3.14159265
using namespace std;
using namespace blepo;
stack <CPoint> frontier;
char str[50];
ImgInt useful;
ImgBgr universal;




	 
void Dilate(ImgBinary& imgbin,ImgBinary& out)
{
out.Reset(imgbin.Width(),imgbin.Height());
ImgBinary temp;
temp.Reset(imgbin.Width(),imgbin.Height());

	for(int x=0;x < imgbin.Width();x++)
		{
			for(int y=0;y< imgbin.Height();y++)
				{
					if( ((x-1>=0) && (y-1>=0) && imgbin(x-1,y-1) == 1) || 
						((y-1>=0) && imgbin(x,y-1) == 1) || 
						((y-1>=0) && (x+1<imgbin.Width()) && imgbin(x+1,y-1)==1) || 
						((x-1>=0) && imgbin(x-1,y) == 1) || 
						((x-1>=0) && (y+1<imgbin.Height() ) && imgbin(x-1,y+1)==1) || 
						((y+1<imgbin.Height()) && imgbin(x,y+1)== 1) || 
						((x+1<imgbin.Width() ) && (y+1<imgbin.Height() ) && imgbin(x+1,y+1)==1) || 
						((x+1<imgbin.Width() ) && imgbin(x+1,y) ==1) ||
						(imgbin(x,y)==1) )
							{
								temp(x,y)=1;
							}
			else
				{
					temp(x,y)=0;
				}
				}
		}
out = temp;

}

void Erode1(ImgBinary& imgbin,ImgBinary& out)
{
out.Reset(imgbin.Width(),imgbin.Height());
ImgBinary temp;
temp.Reset(imgbin.Width(),imgbin.Height());

for(int x=0;x < imgbin.Width();x++)
	{
		for(int y=0;y< imgbin.Height();y++)
			{
				if( ((x-1>=0) && (y-1>=0) && imgbin(x-1,y-1) == 1) && 
					((y-1>=0) && imgbin(x,y-1) == 1) && 
					((y-1>=0) && (x+1<imgbin.Width()) && imgbin(x+1,y-1)==1) && 
					((x-1>=0) && imgbin(x-1,y) == 1) &&
					((x-1>=0) && (y+1<imgbin.Height() ) && imgbin(x-1,y+1)==1) && 
					((y+1<imgbin.Height()) && imgbin(x,y+1)== 1) && 
					((x+1<imgbin.Width()) && (y+1<imgbin.Height() ) && imgbin(x+1,y+1)==1) && 
					((x+1<imgbin.Width()) && imgbin(x+1,y)==1) &&
					(imgbin(x,y)==1) )
					{
						temp(x,y)=1;
					}
				else
					{
						temp(x,y)=0;
					}
			}
	}
out = temp;
}





void StemFind(ImgBinary& imgbin3)
{
int width=imgbin3.Width();
int height=imgbin3.Height();
ImgBinary stem,stem1,imgbin4,imgbin5;
stem=imgbin3;
Bgr magenta(255,0,255),null1(0,0,0),yellow1(0,255,255);
Erode1( stem, stem);
Erode1(stem, stem);
Erode1( stem, stem);
Erode1( stem, stem);
Erode1( stem, stem);
Erode1(stem, stem);
Dilate(stem, stem);
Dilate(stem, stem);
Dilate(stem, stem);
Dilate(stem, stem);
Dilate(stem, stem);
Dilate(stem, stem);
imgbin4=stem;
Xor(imgbin3,stem,&stem);
Erode1(stem, stem);
Dilate(stem, stem);
stem1=stem;
Erode1(stem1, stem1);
Erode1(imgbin4, imgbin5);
Xor(stem1,stem,&stem1);
Xor(imgbin4,imgbin5,&imgbin4);

for(int i=0;i<=width-1;i++)
		    for(int j=0;j<=height-1;j++)
			    {
				    if(stem1(i,j)==1)
					{universal(i,j)=null1;
					universal(i,j)=magenta;}

					if(imgbin4(i,j)==1)
					{universal(i,j)=yellow1;}
				}


}
void floodfillseparate(ImgBinary& img1,ImgInt& out,CPoint pixel, int nextlabel)
{
 CPoint pt1,pt2;
 if (img1(pixel.x,pixel.y)==1)
 { frontier.push(pixel);}
 int width2=img1.Width();
 int height2=img1.Height();
 
 while(! frontier.empty())
 {
   pt1=frontier.top();
   frontier.pop();

   if((pt1.x>=0,pt1.y+1>=0) && (pt1.x<=width2-1,pt1.y+1<=height2-1) && (img1(pt1.x,pt1.y+1)==1))
   {
	   pt2.x=pt1.x; pt2.y=pt1.y+1;
	   frontier.push(pt2);
	   out(pt2.x,pt2.y)=nextlabel;
	   img1(pt2.x,pt2.y)=0;
   }

  if((pt1.x>=0,pt1.y-1>=0) && (pt1.x<=width2-1,pt1.y-1<=height2-1) && (img1(pt1.x,pt1.y-1)==1))
   {
	   pt2.x=pt1.x; pt2.y=pt1.y-1;
	   frontier.push(pt2);
	   out(pt2.x,pt2.y)=nextlabel;
	   img1(pt2.x,pt2.y)=0;
   }

  if((pt1.x+1>=0,pt1.y>=0) && (pt1.x+1<=width2-1,pt1.y<=height2-1) && (img1(pt1.x+1,pt1.y)==1))
   {
	   pt2.x=pt1.x+1; pt2.y=pt1.y;
	   frontier.push(pt2);
	   out(pt2.x,pt2.y)=nextlabel;
	   img1(pt2.x,pt2.y)=0;
   }

  if((pt1.x-1>=0,pt1.y>=0) && (pt1.x-1<=width2-1,pt1.y<=height2-1) && (img1(pt1.x-1,pt1.y)==1))
   {
	   pt2.x=pt1.x-1; pt2.y=pt1.y;
	   frontier.push(pt2);
	   out(pt2.x,pt2.y)=nextlabel;
	   img1(pt2.x,pt2.y)=0;
   }
}
}

int LabellingFunc(ImgBinary& out)
{
	CPoint point2,point3;
    int nextlabel=1;
	int width1=out.Width();
	int height1=out.Height();
	ImgInt Label(width1,height1);
	int* p;
	for (p = Label.Begin() ; p != Label.End() ; p++)
	*p=0;
	
	for(point2.x=0;point2.x<width1;point2.x++)
		for(point2.y=0;point2.y<height1;point2.y++)
		if((out(point2.x,point2.y)==1) && (Label(point2.x,point2.y)==0))
			{
				floodfillseparate(out,Label,point2,nextlabel);
			    nextlabel++;
			}
		
	useful=Label;
	return nextlabel-1;
}
void fruitclassify(ImgBinary& imgbin2,ImgBinary& imgbin3,double compactness, double eccentricity, double m00)
{
	int width=imgbin2.Width();
	int height=imgbin2.Height();
	ImgBgr yellow(width,height),green(width,height),red(width,height);
	Bgr yellow1(0,255,255),green1(0,255,0),red1(0,0,255),null(0,0,0);
	
	
	
	if((compactness>0.191500 && compactness<0.204000) && (m00>3030 && m00<3280) && (eccentricity>0.292480 && eccentricity<0.411200))
	{
		for(int i=0;i<=width-1;i++)
		    for(int j=0;j<=height-1;j++)
			    {
				    if(imgbin2(i,j)==1)
					{green(i,j)=red1;}

					else
					green(i,j)=null;
		        }
	
	
	Or(green, universal,&universal);
	cout<<"It is an apple"<<endl<<endl;
	}

	if((compactness>0.0695550 && compactness<0.0749100) && (m00>4900 && m00<5860) && (eccentricity>0.935400 && eccentricity<0.948400))
	{
		for(int i=0;i<=width-1;i++)
		    for(int j=0;j<=height-1;j++)
			    {
				    if(imgbin2(i,j)==1)
					{yellow(i,j)=yellow1;}

					else
					yellow(i,j)=null;
		        }
	StemFind(imgbin3);
	cout<<"It is a banana"<<endl<<endl;
	}
	
	if((compactness>0.201750 && compactness<0.207750) && (m00>5900 && m00<6370) && (eccentricity>0.244500 && eccentricity<0.377100))
	{
		for(int i=0;i<=width-1;i++)
		    for(int j=0;j<=height-1;j++)
			    {
				    if(imgbin2(i,j)==1)
					{red(i,j)=green1;}

					else
					red(i,j)=null;
		        }
	
	Or(red, universal,&universal);
	cout<<"It is a grape"<<endl<<endl;
	}
}

void CalcMoments(ImgBinary& imgbin)
{
	double  m00=0,m10=0,m01=0,m20=0,m02=0,m11=0,perimeter=0;
	double u00,u20,u02,u11,xbar,ybar,compactness;
	ImgBinary imgbin1,imgbin2,imgbin3;
	imgbin3=imgbin;
	imgbin2=imgbin;
	imgbin1=imgbin;
	Erode1(imgbin1, imgbin1);
	Xor(imgbin2,imgbin1,&imgbin2); 
	int height=imgbin.Height();
	int width=imgbin.Width();
	for(int x=0;x<=width-1;x++)
		for(int y=0;y<=height-1;y++)
		{
			perimeter +=imgbin2(x,y);
			m00 += imgbin(x,y);
			m10 += imgbin(x,y)*x;
			m01 += imgbin(x,y)*y;
			m20 += imgbin(x,y)*x*x;
			m02 += imgbin(x,y)*y*y;
			m11 += imgbin(x,y)*x*y;
		}
		cout<<" m00 ="<<m00<<endl<<" m10 ="<<m10<<endl<<" m01 ="<<m01<<endl<<" m20 ="<<m20<<endl<<" m02 ="<<m02<<endl<<" m11 ="<<m11<<endl;
		compactness=(4*(m00))/(pow(perimeter,2));
		cout<<"compactness ="<<compactness<<endl;
		u00=m00;
		xbar=m10/m00;
		ybar=m01/m00;
		u11=m11-(ybar*m10);
		u20=m20-(xbar*m10);
		u02=m02-(ybar*m01);
		cout<<" area = "<<u00<<endl<<" xbar ="<<xbar<<endl<<" ybar ="<<ybar<<endl<<" u11 ="<<u11<<endl<<" u20 ="<<u20<<endl<<" u02 ="<<u02<<endl;
	    double lambdamax,lambdamin,subtraction,power,power1,param,subtraction2,param2,eccentricity,result;
		subtraction=u20-u02;
		power=pow(subtraction,2);
		power1=pow(u11,2);
		param=power+(4*power1);
		lambdamax=(1/(2*u00))*(u20+u02+sqrt(param));
		lambdamin=(1/(2*u00))*(u20+u02-sqrt(param));
		cout<<" lambdamax=  "<<lambdamax<<endl<<"  lambdamin=  "<<lambdamin<<endl;
		subtraction2=lambdamax-lambdamin;
		param2=((subtraction2)/(lambdamax));
		eccentricity=sqrt(param2);
		cout<<"  eccentricity = "<<eccentricity<<endl;
		result= (2*(u11))/(u20+u02-sqrt(param));
		result=atan(result);
		result=atan2(((2)*(u11)),((u20)-(u02)));
		result=((result)/(2))*(180/(PI));
		int res1=result;
		if (res1<0)
			res1=(-res1);
		else
			res1=360-res1;
		cout<<"  angle = "<<res1<<endl;
		result=res1*((PI)/180);
		double xcoord=2*pow(lambdamin,0.4)*sin(result);
		double ycoord=2*pow(lambdamin,0.4)*cos(result);
		double xcoord1=2*pow(lambdamin,0.4)*sin(result);
		double ycoord1=2*pow(lambdamin,0.4)*cos(result);
		double xcoord2=2*pow(lambdamax,0.4)*cos(result);
		double ycoord2=2*pow(lambdamax,0.4)*sin(result);
		double xcoord3=2*pow(lambdamax,0.4)*cos(result);
		double ycoord3=2*pow(lambdamax,0.4)*sin(result);
		double xcoord4=2*pow(lambdamax,0.1)*cos(result);
		double ycoord4=2*pow(lambdamax,0.1)*sin(result);
		double xcoord5=2*pow(lambdamax,0.1)*cos(result);
		double ycoord5=2*pow(lambdamax,0.1)*sin(result);
		double xcoord6=2*pow(lambdamax,0.1)*sin(result);
		double ycoord6=2*pow(lambdamax,0.1)*cos(result);
		double xcoord7=2*pow(lambdamax,0.1)*sin(result);
		double ycoord7=2*pow(lambdamax,0.1)*cos(result);
		
		Bgr colorb(255,0,0),colorg(0,0,0),colorc(255,255,0);
		xcoord=xbar+xcoord;
		ycoord=ybar+ycoord;
		xcoord1=xbar-xcoord1;
		ycoord1=ybar-ycoord1;
		ycoord2=ybar-ycoord2;
		xcoord2=xbar+xcoord2;
		ycoord3=ybar+ycoord3;
		xcoord3=xbar-xcoord3;
		ycoord4=ybar+ycoord4;
		xcoord4=xbar-xcoord4;
		ycoord5=ybar-ycoord5;
		xcoord5=xbar+xcoord5;
		ycoord6=ybar+ycoord6;
		xcoord6=xbar+xcoord6;
		ycoord7=ybar-ycoord7;
		xcoord7=xbar-xcoord7;
		CPoint lpt1,lpt2,lpt3,lpt4,lpt5,lpt6,lpt7,lpt8,lpt9;
		lpt1.x=xbar; lpt1.y=ybar;
		lpt2.x=xcoord;lpt2.y=ycoord;
		lpt3.x=xcoord1;lpt3.y=ycoord1;
		lpt4.x=xcoord2;lpt4.y=ycoord2;
		lpt5.x=xcoord3;lpt5.y=ycoord3;
		lpt6.x=xcoord4;lpt6.y=ycoord4;
		lpt7.x=xcoord5;lpt7.y=ycoord5;
		lpt8.x=xcoord6;lpt8.y=ycoord6;
		lpt9.x=xcoord7;lpt9.y=ycoord7;
		DrawLine(lpt1, lpt2, &universal, colorb);
	    DrawLine(lpt1, lpt3, &universal, colorb);
		DrawLine(lpt1, lpt4, &universal, colorg);
		DrawLine(lpt1, lpt5, &universal, colorg);
		DrawLine(lpt7, lpt6 , &universal,colorc);
		DrawLine(lpt8, lpt9, &universal, colorc);
		
		fruitclassify(imgbin2,imgbin3,compactness,eccentricity,m00);
		
		
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

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
			
			ImgGray img;
			int binx=img.Width();
			int biny=img.Height();
			ImgBinary out,high,low;
			cout<<"enter the path"<<endl;
			gets(str);
			Load(str,&img);
			Load(str,&universal);
			Figure fig,fig1,fig2;
			Figure fruit;
			fruit.Draw(img);
			fruit.SetTitle("Orignal image");
			int thresh=ComputeThreshold(img);
			cout<<" threshold ="<<thresh<<endl;
			int th=thresh+30;
			int tl=thresh-18;
			Threshold(img,thresh,&out);
			Threshold(img,th,&high);
			Threshold(img,tl,&low);
			fig1.Draw(high);
			fig1.SetTitle("High Threshold");
			fig2.Draw(low);
			fig2.SetTitle("Low Threshold");
			int height=img.Height();
			int width=img.Width();
			CPoint point1;
			for(point1.x=0;point1.x<width;point1.x++)
				for(point1.y=0;point1.y<height;point1.y++)
					if(low(point1.x,point1.y)>tl && low(point1.x,point1.y)<th)
						low(point1.x,point1.y)=255;
			
			Erode1( low, low);
			Erode1( low, low);
			Dilate(low,low);
			Dilate(low,low);
			Erode1( low,low);
			Dilate(low,low);
			Dilate(low,low);
			fig.Draw(low);
			fig.SetTitle("Double Thresholded and processed");
			CPoint pt;
			int nexta=LabellingFunc( low);
		    ImgBinary imgbin(width,height);
			int xpw=1;
			while(xpw<=nexta)
			{

			for(int p=0;p<=width-1;p++)
				for(int q=0;q<=height-1;q++)
				{   
					if(useful(p,q)==xpw)
				   {
					   imgbin(p,q)=1;
				   }
					else
						imgbin(p,q)=0;
				}


			cout<<"  label ="<<xpw<<endl;
			CalcMoments(imgbin);
			xpw++;
			}
			Figure imb;
			imb.Draw(universal);
			imb.SetTitle("final output");
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

