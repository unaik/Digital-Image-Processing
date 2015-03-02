// assignment33.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "assignment33.h"
#include "blepo.h"
#include <stack>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;
using namespace blepo;



void YGradient(double sigma,ImgGray& I, ImgFloat& Gy)
{
	ImgGray tempo(I.Width(),I.Height());
	tempo=I;
	int mu=ceil(2.5*sigma-0.5);
	int w=(2*mu + 1);
	double *g,*g1;
	double sum=0,sum1=0;
	double param,param1;
	g = new double[w];
	g1 = new double[w];
			for(int i=0;i<w;i++)
			{
				param= ((-(i-mu)*(i-mu))/(2*sigma*sigma));
				g[i]=exp(param);
				sum += g[i];
			}
			
			for(int i=0;i<w;i++)
			{
				g[i]=g[i]/sum;
			}
				
			
			for(int i=0;i<w;i++)
			{
				param1= ((-(i-mu)*(i-mu))/(2*sigma*sigma));
				g1[i]=-(i-mu)*exp(param1);
				
				sum1 += g1[i]*i;
			}
			
			for(int i=0;i<w;i++)
			{
				g1[i] /=sum1;
				
			}

			for(int y=0;y<I.Height();y++)                      //horizontal convolution with gaussian kernel
				for(int x=mu;x<I.Width()-mu;x++)
				 {
					
					float val=0;
					for(int i=0;i<w;i++)
					{
						val+=g[i]*tempo(x+mu-i,y);
					    
					}
					I(x,y)=val; 
									
				 }

				


           for(int y=mu;y<Gy.Height()-mu;y++)
				for(int x=0;x<Gy.Width();x++)
				{
					float val=0;
					for(int i=0;i<w;i++)
					{
						val+=g1[i]*I(x,y+mu-i);
					}
					
					Gy(x,y)=val;
				}
}


void XGradient(double sigma,ImgGray& I, ImgFloat& Gx)
{
	ImgGray tempo(I.Width(),I.Height());
	tempo=I;
	int mu=ceil(2.5*sigma-0.5);
	int w=(2*mu + 1);
	double *g,*g1;
	double sum=0,sum1=0;
	double param,param1;
	g = new double[w];
	g1 = new double[w];
			for(int i=0;i<w;i++)
			{
				param= ((-(i-mu)*(i-mu))/(2*sigma*sigma));
				g[i]=exp(param);
				sum += g[i];
			}
			
			for(int i=0;i<w;i++)
			{
				g[i]=g[i]/sum;
			}
				
			
			for(int i=0;i<w;i++)
			{
				param1= ((-(i-mu)*(i-mu))/(2*sigma*sigma));
				g1[i]=-(i-mu)*exp(param1);
				sum1 += g1[i]*i;
			}
			
			for(int i=0;i<w;i++)
			{
				g1[i] /=sum1;
				
			}

			for(int y=mu;y<I.Height()-mu;y++)                      //vertical convolution with gaussian kernel
				for(int x=0;x<I.Width();x++)
				 {
					
					float val=0;
					for(int i=0;i<w;i++)
					{
						val+=g[i]*tempo(x,y+mu-i);
					    
					}
					I(x,y)=val; 
									
				 }

				

           for(int y=0;y<Gx.Height();y++)
				for(int x=mu;x<Gx.Width()-mu;x++)
				{
					float val=0;
					for(int i=0;i<w;i++)
					{
						val+=g1[i]*I(x+mu-i,y);
					}
					
					Gx(x,y)=val;
				}
}

void phasemag(ImgFloat& phase,ImgFloat& mag, ImgFloat& Gx,ImgFloat& Gy,int mu)
{
  for(int y=mu;y<Gx.Height()-mu;y++)
		for(int x=mu;x<Gx.Width()-mu;x++)
				{
				 float val=Gx(x,y);
				 float val1=Gy(x,y);
				 float power= pow(val,2);
				 float power1= pow(val1,2);
				 float summ=power+power1;
				 float val3=sqrt(summ);
				 phase(x,y)= val3;
				 }

   for(int y=mu;y<mag.Height()-mu;y++)
				for(int x=mu;x<mag.Width()-mu;x++)
				{
				 double val=Gx(x,y);
				 double val1=Gy(x,y);
				 double angle=atan2(Gy(x,y),Gx(x,y));
				 double angle1;
				 angle=angle*(180/3.14);
				 mag(x,y)= angle;
				 
				}	
}


void NonMaxSup(ImgFloat& phase,ImgFloat& mag,ImgFloat& nonmax,int mu)
{
	nonmax=phase;
	 for(int y=mu;y<phase.Height()-mu;y++)
				for(int x=mu;x<phase.Width()-mu;x++)
				{
				 float ang=mag(x,y);
				 float vall=phase(x,y);
				 if((ang>=-180 && ang< 22.5) && ((vall<phase(x-1,y)) || (vall<phase(x+1,y))))
					 nonmax(x,y)=0;
				 else if((ang>=22.5 && ang<67.5) && ((vall<phase(x-1,y-1)) || (vall<phase(x+1,y+1))))
					 nonmax(x,y)=0;
				 else if((ang>=67.5 && ang<112.5) && ((vall<phase(x,y-1)) || (vall<phase(x,y+1))))
					 nonmax(x,y)=0;
				 else if((ang>=112.5 && ang<180) && ((vall<phase(x-1,y+1)) || (vall<phase(x+1,y-1))))
					 nonmax(x,y)=0;
				}
}

int GetThresh(ImgGray& nonmaxgray, int mu)
{
	int h[256]={0};
   for(int i=0;i<=255;i++)
   {
     for(int y=mu;y<nonmaxgray.Height()-mu;y++)
				for(int x=mu;x<nonmaxgray.Width()-mu;x++)
				{
				  if(nonmaxgray(x,y)==i)
					  h[i]+=1;
				 
				}
				
   }

  
  
   for(int i=2;i<=255;i++)
   {
     
	   if(h[i]!=0)
		  h[i]+=h[i-1];
	  else
		  h[i]=h[i-1];
   }  
  
   int p;
   
   for(int i=2;h[i]<=(0.9)*h[255];i++)
   {
    p=i;
   }
  
				    
   return p;
   
}

void HighLow(ImgGray& nonmaxgray,ImgGray& nonmaxgray1,int mu,int thigh,int tlow)
{
  for(int y=mu;y<nonmaxgray.Height()-mu;y++)
				for(int x=mu;x<nonmaxgray.Width()-mu;x++)
				{
				  if(nonmaxgray(x,y)>thigh)
					  nonmaxgray(x,y)=255;
				  else
					  nonmaxgray(x,y)=0;
				}
	
 for(int y=mu;y<nonmaxgray1.Height()-mu;y++)
				for(int x=mu;x<nonmaxgray1.Width()-mu;x++)
				{
				  if(nonmaxgray1(x,y)>tlow)
					  nonmaxgray1(x,y)=255;
				  else
					  nonmaxgray1(x,y)=0;
				}
   
}


void floodfillseparate8x8(ImgBinary& in,ImgBinary& out,CPoint pixel, int mu)
{
 stack <CPoint> frontier;
 CPoint pt1,pt2;
 if (in(pixel.x,pixel.y)==1)
 { frontier.push(pixel);}
 int width2=in.Width();
 int height2=in.Height();
 
 while(! frontier.empty())
 {
   pt1=frontier.top();
   frontier.pop();

   if((pt1.x>=0,pt1.y+1>=0) && (pt1.x<=width2-mu,pt1.y+1<=height2-mu) && (in(pt1.x,pt1.y+1)==1))
   {
	   pt2.x=pt1.x; pt2.y=pt1.y+1;
	   frontier.push(pt2);
	   out(pt2.x,pt2.y)=1;
	   in(pt2.x,pt2.y)=0;
   }
   
  if((pt1.x>=0,pt1.y-1>=0) && (pt1.x<=width2-mu,pt1.y-1<=height2-mu) && (in(pt1.x,pt1.y-1)==1))
   {
	   pt2.x=pt1.x; pt2.y=pt1.y-1;
	   frontier.push(pt2);
	   out(pt2.x,pt2.y)=1;
	   in(pt2.x,pt2.y)=0;
   }
   
  if((pt1.x+1>=mu+20,pt1.y>=mu+20) && (pt1.x+1<width2-5*mu,pt1.y<height2-5*mu) && (in(pt1.x+1,pt1.y)==1))
   {
	   pt2.x=pt1.x+1; pt2.y=pt1.y;
	   frontier.push(pt2);
	   out(pt2.x,pt2.y)=1;
	   in(pt2.x,pt2.y)=0;
   }
   
  if((pt1.x-1>mu+10,pt1.y>mu+10) && (pt1.x-1<width2-5*mu,pt1.y<height2-5*mu) && (in(pt1.x-1,pt1.y)==1))
   {
	   pt2.x=pt1.x-1; pt2.y=pt1.y;
	   frontier.push(pt2);
	   out(pt2.x,pt2.y)=1;
	   in(pt2.x,pt2.y)=0;
   }
  
  if((pt1.x-1>=mu+10,pt1.y-1>=mu+10) && (pt1.x-1<width2-5*mu,pt1.y-1<height2-5*mu) && (in(pt1.x-1,pt1.y-1)==1))
   {
	   pt2.x=pt1.x-1; pt2.y=pt1.y-1;
	   frontier.push(pt2);
	   out(pt2.x,pt2.y)=1;
	   in(pt2.x,pt2.y)=0;
   }
   
  if((pt1.x-1>=mu+10,pt1.y+1>=mu+10) && (pt1.x-1<width2-5*mu,pt1.y+1<height2-5*mu) && (in(pt1.x-1,pt1.y+1)==1))
   {
	   pt2.x=pt1.x-1; pt2.y=pt1.y+1;
	   frontier.push(pt2);
	   out(pt2.x,pt2.y)=1;
	   in(pt2.x,pt2.y)=0;
   }
   
  if((pt1.x+1>=mu+20,pt1.y-1>=mu+20) && (pt1.x+1<width2-5*mu,pt1.y-1<height2-5*mu) && (in(pt1.x+1,pt1.y-1)==1))
   {
	   pt2.x=pt1.x+1; pt2.y=pt1.y-1;
	   frontier.push(pt2);
	   out(pt2.x,pt2.y)=1;
	   in(pt2.x,pt2.y)=0;
   }
  
  if((pt1.x+1>=mu+20,pt1.y+1>=mu+20) && (pt1.x+1<width2-5*mu,pt1.y+1<height2-5*mu) && (in(pt1.x+1,pt1.y+1)==1))
   {
	   pt2.x=pt1.x+1; pt2.y=pt1.y+1;
	   frontier.push(pt2);
	   out(pt2.x,pt2.y)=1;
	   in(pt2.x,pt2.y)=0;
   }
}
}

void CannyEdges(ImgBinary& nonmaxbin,ImgBinary& tlowbin, ImgBinary& blankbin,int mu)
{
	CPoint pixel;
	ImgBinary::Pixel new_color=1;
	for(int y=mu;y<nonmaxbin.Height()-mu;y++)
				for(int x=mu;x<nonmaxbin.Width()-mu;x++)
				{
					if(nonmaxbin(x,y)==1 )
					{	pixel.x=x; pixel.y=y;
					   floodfillseparate8x8(tlowbin,blankbin,pixel,mu);
					   
					} 
				}
}
void CannyEdges1(ImgBinary& nonmaxbin,ImgBinary& tlowbin, ImgBinary& blankbin,int mu)
{
	CPoint pixel;
	ImgBinary::Pixel new_color=1;
	for(int y=mu;y<nonmaxbin.Height()-mu;y++)
				for(int x=mu;x<nonmaxbin.Width()-mu;x++)
				{
					if(nonmaxbin(x,y)==1 )
					{	pixel.x=x; pixel.y=y;
					   
					   FloodFill4(tlowbin,pixel.x,pixel.y,new_color,&blankbin);
					} 
				}
}


void ChamferDist(ImgBinary& blnkbin,ImgInt& champ)
{
  ImgInt tempo(champ.Width(),champ.Height());
  int infinity=((champ.Width())*(champ.Height())) + 1;
  for(int x=0;x<blnkbin.Width();x++)
	  for(int y=0;y<blnkbin.Height();y++)
	  {
		  if (blnkbin(x,y)==1)
			  tempo(x,y)=0;
		  else
			  tempo(x,y)=infinity;
	  }

  for(int x=2;x<blnkbin.Width()-2;x++)
	  for(int y=2;y<blnkbin.Height()-2;y++)
	  {
		  if ((tempo(x-1,y) != infinity) && (tempo(x,y-1)!= infinity))
		  {}
		  else
			  tempo(x,y)=min(infinity,min(1+tempo(x-1,y),1+tempo(x,y-1)));
	  }

  for(int x=champ.Width()-2;x>-1;x--)
	  for(int y=champ.Height()-2;y>-1;y--)
	  {
	    champ(x,y)=min(tempo(x,y),min(1+tempo(x,y+1),1+tempo(x+1,y)));
		tempo(x,y)=champ(x,y);
	  }
}

void Templatematching(ImgBinary& blkbin,ImgInt& champ,ImgInt& probmap)
{
  for(int x=0;x<champ.Width()-blkbin.Width();x++)
	  for(int y=0;y<champ.Height()-blkbin.Height();y++)
	  {
		  for(int i=0;i<blkbin.Width()-1;i++)
			  for(int j=0;j<blkbin.Height()-1;j++)
			  {
			    if(blkbin(i,j)==1)
					probmap(x,y)+=blkbin(x+i,y+j);
			  }	  			  
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
			ImgGray I;                                                                   // orignal grayscale image
			int height=I.Height();                                                       //height, width
			int width=I.Width();
			ImgGray temp(width,height),temp1(width,height),nonmaxgray(width,height),nonmaxgray1(width,height);
			ImgFloat gx(width,height),gy(width,height),phase(width,height),mag(width,height),nonmax(width,height);  //Gx and Gy images
			ImgBinary nonmaxbin(width,height),tlowbin(width,height);
			ImgBinary blankbin(width,height);
			char str[50],str1[50],str2[50];
			cout<<"Enter Path"<<endl;
			gets(str);
			cout<<"Enter path for cherry pepsi image"<<endl;
			gets(str1);
			cout<<"Enter path for cherry pepsi template image"<<endl;
			gets(str2);
			Load(str,&I);
			temp=I;
			temp1=I;
			Convert(I,&gy);
			gx=gy;
			phase=gy;
			mag=gy;
			double sigma;                                                                //sigma value      
			cout<<"Enter Value of Sigma";
			cin>>sigma;
			int mu=ceil(2.5*sigma-0.5);
			YGradient(sigma,temp,gy);
			Figure gy1;
			gy1.Draw(gy);
			gy1.SetTitle("gy");
			XGradient(sigma,temp1,gx);
			Figure gx1;
			gx1.Draw(gx);
			gx1.SetTitle("gx");
			phasemag(phase,mag,gx,gy,mu);
			Figure ph1;
			ph1.Draw(phase);
			ph1.SetTitle("magnitude");
			Figure mg1;
			mg1.Draw(mag);
			mg1.SetTitle("angle");
			NonMaxSup(phase,mag,nonmax,mu);
			Figure nonma;
			nonma.Draw(nonmax);
			nonma.SetTitle("nonmax");
			Convert(nonmax,&nonmaxgray);
			nonmaxgray1=nonmaxgray;
			int thigh=GetThresh(nonmaxgray,mu);
			int tlow= 0.35*thigh;
			HighLow(nonmaxgray,nonmaxgray1,mu,thigh,tlow);
			Figure th;
			th.Draw(nonmaxgray);
			th.SetTitle("thigh");
			Figure tl;
			tl.Draw(nonmaxgray1);
			tl.SetTitle("tlow");

			Convert(nonmaxgray,&nonmaxbin);
			Convert(nonmaxgray1,&tlowbin);
			blankbin=tlowbin;
			ImgBinary::Iterator p;
			for(p=blankbin.Begin();p!=blankbin.End();p++)
				{				
				  *p=0;
				}
			CannyEdges(nonmaxbin,tlowbin,blankbin,mu);
			Figure fig3;
            fig3.Draw(blankbin);
			fig3.SetTitle("canny");
			
			///////////////////////////////////////////////////////////////////////////////////////////////
			ImgBgr Cherry;
			int width2=Cherry.Width(); int height2=Cherry.Height();
			ImgGray cherrygray(width2,height2),nmcgr(width2,height2),nmcgr1(width2,height2);
			ImgFloat cherryfloat(width2,height2),cherryfloatx(width2,height2);
			ImgFloat phase1(width2,height2),mag1(width2,height2),nmc(width2,height2);
			ImgBinary nmcbin,tlow1bin,blnkbin;
			Load(str1,&Cherry);
			Convert(Cherry,&cherrygray);
			Convert(cherrygray,&cherryfloat);
			cherryfloatx=cherryfloat;
			phase1=cherryfloat;
			mag1=cherryfloat;
			YGradient(sigma,cherrygray,cherryfloat);     //Ygradient for bigger image
			XGradient(sigma,cherrygray,cherryfloatx);     //Xgradient for bigger image
			phasemag(phase1,mag1,cherryfloatx,cherryfloat,mu);  //phase and mag of bigger img
			NonMaxSup(phase1,mag1,nmc,mu);
			Convert(nmc,&nmcgr);
			nmcgr1=nmcgr;
			int thigh1=GetThresh(nmcgr,mu);
			int tlow1=0.35*thigh1;
			HighLow(nmcgr,nmcgr1,mu,thigh1,tlow1);
			Convert(nmcgr,&nmcbin);
			Convert(nmcgr1,&tlow1bin);
			blnkbin=tlow1bin;
			for(p=blnkbin.Begin();p!=blnkbin.End();p++)
			{*p=0;}

			CannyEdges1(nmcbin,tlow1bin,blnkbin,mu);
			Figure f;
			f.Draw(blnkbin);
			
			////////////////////////////////////////////////////////////////////////////////////

			ImgBgr Template;
			int width3=Template.Width(); int height3=Template.Height();
			ImgGray tempgray(width3,height3),nmtempgr(width3,height3),nmtempgr1(width3,height3);
			ImgFloat tempfloat(width3,height3),tempfloatx(width3,height3);
			ImgFloat phase2(width3,height3),mag2(width3,height3),nmtemp(width3,height3);
			ImgBinary nmtempbin,tlow2bin,blkbin;
			Load(str2,&Template);
			Convert(Template,&tempgray);
			Convert(tempgray,&tempfloat);
			tempfloatx=tempfloat;
			phase2=tempfloat;
			mag2=tempfloat;
			YGradient(sigma,tempgray,tempfloat);     //Ygradient for bigger image
			Figure f1;//j1,k1;
			XGradient(sigma,tempgray,tempfloatx);     //Xgradient for bigger image
			phasemag(phase2,mag2,tempfloatx,tempfloat,mu);  //phase and mag of bigger img
			NonMaxSup(phase2,mag2,nmtemp,mu);
			Convert(nmtemp,&nmtempgr);
			nmtempgr1=nmtempgr;
			int thigh2=GetThresh(nmtempgr,mu);
			int tlow2=0.35*thigh2;
			HighLow(nmtempgr,nmtempgr1,mu,thigh1,tlow1);
			Convert(nmtempgr,&nmtempbin);
			Convert(nmtempgr1,&tlow2bin);
			blkbin=tlow2bin;
			for(p=blkbin.Begin();p!=blkbin.End();p++)
			{*p=0;}

			CannyEdges1(nmtempbin,tlow2bin,blkbin,mu);
			f1.Draw(blkbin);
           //////////////////////////////////////////////////////////////////////////////////////////////
			ImgInt champ(blnkbin.Width(),blnkbin.Height()),probmap(champ.Width()-blkbin.Width(),champ.Height()-blkbin.Height());
			ImgGray champ1;
			ImgInt::Iterator p1;
			for(p1=champ.Begin();p1!=champ.End();p1++)
			{*p1=0;}
			for(p1=probmap.Begin();p1!=probmap.End();p1++)
			{*p1=0;}

			ChamferDist(blnkbin,champ);
			f1.Draw(champ);
			Templatematching(blkbin,champ,probmap);
			Figure prob;
			prob.Draw(probmap);
			int min=probmap(0,0);
			Point plzwrk,plzwrk1;
			for(int x=30;x<probmap.Width()-40;x++)
				for(int y=30;y<probmap.Height()-40;y++)
				{
				 
				 if(probmap(x,y)<min)
				 {min=probmap(x,y);
				   plzwrk.x=x;
				   plzwrk.y=y;
				 }
				}

				cout<<"x "<<plzwrk.x<<"y"<<plzwrk.y;
				Figure final;
				
				int w1=plzwrk.x;
				int h1=plzwrk.y;
				plzwrk1.x=Template.Width();
				plzwrk1.y=Template.Height();
				Bgr colorb(255,255,0);
				Rect rectangle(plzwrk.x,plzwrk.y,plzwrk1.x+50,plzwrk.y+120);
				DrawRect(rectangle,&Cherry,colorb,1);
				final.Draw(Cherry);


			



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
