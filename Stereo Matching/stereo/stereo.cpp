// stereo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stereo.h"
#include "blepo.h"
#include <string>
#include <fstream>
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace blepo;
using namespace std;


typedef struct dbar
{
  ImgGray dissim_map;
};




dbar* ComputeSummedDissimilarity(ImgGray& Il,ImgGray& Ir,int dmin, int dmax)
{
   int width= Il.Width();
   int height= Il.Height();
   
   dbar * delta = new dbar[(dmax+1) ];

   for (int i=0; i <= dmax; i++)
   {
	   delta[i].dissim_map.Reset(width,height);
    for(int y=0 ; y< height; y++)
		for(int x=0; x< width; x++)
		{
         if((x-i)>0 ) 
		 ((delta[i].dissim_map(x,y)=abs(Il(x,y)-Ir(x-i,y))));
		}
    ConvolveBox5x5(delta[i].dissim_map,&delta[i].dissim_map);
   }
 return delta;
}




ImgInt LeftDisparityImage(dbar * delta,ImgGray& Il, ImgInt& disparity_map,ImgGray::Iterator endcond, int dmax, int gate)
{
	vector <ImgGray::Iterator> scanner(dmax+1);
	int width=Il.Width(),height=Il.Height(),disparity;
	ImgInt::Iterator dispoint = disparity_map.Begin();

	for(int i=0;i<=dmax; i++)
	{scanner[i]=delta[i].dissim_map.Begin();}
		
	ImgInt::Iterator test;
	for(int y=0;y<height;y++)
		for(int x=0;x<width;x++)
		{test = disparity_map.Begin(x,y);}

	while(scanner[0] != endcond)
	{
		gate=width*height + 1;
		for(int j=0;j <= dmax; j++)
		{
			int tempo = *(scanner[j])++;
			if (tempo<gate)
			{
				gate=tempo;
				disparity=j;
			}
		}
	 *dispoint++=disparity;
	}
 return disparity_map;
}

ImgInt RightDisparityImage(dbar * delta, ImgGray& Ir,ImgInt& disparity_map,ImgGray::Iterator endcond, int dmax, int gate)
{
	int width= Ir.Width(), height=Ir.Height(),disparity;
	ImgInt::Iterator dispoint = disparity_map.Begin();
	vector <ImgGray::Iterator> scanner(dmax+1);
	
	for(int j=0; j <= dmax ; j++)
	{scanner[j]=delta[j].dissim_map.Begin(j,0);}
	
	ImgInt::Iterator test;
	for(int y=0;y<height;y++)
		for(int x=0;x<width;x++)
		{test = disparity_map.Begin(x,y);}

	while(scanner[0] != endcond)
	{
		gate= width*height +1;
		for(int j=0;j<=dmax;j++)
		{
			int tempo = *(scanner[j])++;
			if(tempo<gate)
			{
				gate=tempo;
				disparity=j;
			}
		}
	 *dispoint++=disparity;
	}
 return disparity_map;
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
			ImgGray Il,Ir;
			ImgBgr mesh;
			char path1[50],path2[50],path3[50];
			cout<<"Enter Path for left image"<<endl;
			gets(path1);
			cout<<"Enter Path for right image"<<endl;
			gets(path2);
			cout<<"Enter Path for image to be used in ply file"<<endl;
			gets(path3);
			int dmin=0,dmax=13;
			Load(path1,&Il);
			Load(path2,&Ir);
			Load(path3,&mesh);
			int width=Il.Width(); int height= Il.Height();
			ImgGray disp_map(Il.Width(),Il.Height()),disp_map1(Il.Width(),Il.Height()),disp_map2(Il.Width(),Il.Height()),disp_map3(Il.Width(),Il.Height());
			ImgInt Disp(Il.Width(),Il.Height()),Disp1(Il.Width(),Il.Height()),Disp2(Il.Width(),Il.Height()),Disp3(Il.Width(),Il.Height());
			ImgInt::Iterator inti;
			for(inti=Disp3.Begin();inti != Disp3.End(); inti++)
			{*inti=0;}
			Figure right,left;
			dbar * delta = ComputeSummedDissimilarity(Il,Ir,dmin,dmax);
			ImgGray::Iterator endcond = delta[0].dissim_map.End();
			ImgGray::Iterator endcond1 = delta[0].dissim_map.End()-dmax;
			int gate=width*height +1;
			Disp1= LeftDisparityImage(delta,Il,Disp,endcond, dmax, gate);
			Disp2=RightDisparityImage(delta,Ir,Disp3,endcond1,dmax,gate);
			ImgInt comp(width,height),comp1(width,height);
			comp=Disp2;
			comp1=Disp1;
			ImgInt::Iterator tes;
			for(tes= Disp1.Begin(); tes != Disp1.End(); tes++)
			{*tes+= 1;}
			for(tes= Disp2.Begin(); tes != Disp2.End(); tes++)
			{*tes += 1;}
			int jj=0;
			for(int y=0;y<height;y++)
			    for(int x=0; x<width; x++)
				{
					if( (x-Disp1(x,y)>0)  && Disp1(x,y)!= Disp2(x-Disp1(x,y),y)  )
					{
						Disp1(x,y)=0;
						jj++;
					}
										
				}
			cout<<"jj "<<jj;
			
			
			right.Draw(Disp2);
			right.SetTitle("Right Disparity Image");
			left.Draw(comp1);
			left.SetTitle("Left Disparity Image");
			Figure LR;
			LR.Draw(Disp1);
			ImgGray G1(mesh.Width(),mesh.Height()),G2(mesh.Width(),mesh.Height()),G3(mesh.Width(),mesh.Height());
			ImgFloat F1,F2,F3;
			ExtractBgr(mesh, &G1,&G2, &G3);
			Convert(G1,&F1);
			Convert(Disp,&F2);
			ofstream meshfile;
			meshfile.open("mesh.ply");
			meshfile<<"ply"<<endl<<"format ascii 1.0"<<endl<<"element vertex "<<G1.Width()*G1.Height()<<endl<<"property float x"<<endl<<"property float y"<<endl<<"property float z"<<endl<<"property uchar diffuse_red"<<endl<<"property uchar diffuse_green"<<endl<<"property uchar diffuse_blue"<<endl<<"end_header"<<endl;
			cout<<endl<<G1.Width()*G1.Height()-jj;
			float z=0;
			for(int y=0; y< G1.Height(); y++)
				for(int x=0; x <G1.Width() ; x++)
				{
					z=F2(x,y)*-2.000000;
					float x1=x,y1=y,u0=F1.Width()/2,v0=F1.Height()/2;
					float X= z*(x1-u0);
					float Y= z*(y1-v0); 
					int val,val1,val2;
					val=G3(x,y);
					val1=G1(x,y);
					val2=G2(x,y);
					meshfile<<x<<" "<<y<<" "<<z<<" "<<val<<" "<<val1<<" "<<val2<<endl;

				}
            meshfile.close();
			cout<<"done";
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
