// Assignment1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Assignment1.h"
#include "blepo.h"
#include <stack>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;
using namespace blepo;


ImgBgr Floodfill(ImgBgr& img,CPoint pixel,Bgr pixv,Bgr newcolor)
{
  ImgBgr temp;
  Bgr nu;
  nu.r=1;nu.g=1;nu.b=1;
  std::stack <CPoint> frontier;
  temp=img;
  CPoint pt1,pt2;

  if(temp(pixel.x,pixel.y)==pixv)
    {frontier.push(pixel);}

  int width=img.Width();
  int height=img.Height();

  while(!frontier.empty())
  {
	  pt1=frontier.top();
	  frontier.pop();

	  if(pt1.y+1<height-1 && temp(pt1.x,pt1.y+1)==pixv)
	  {
		  pt2.x=pt1.x;pt2.y=pt1.y+1;
		  frontier.push(pt2);
		  img(pt2.x,pt2.y)=newcolor;
		  temp(pt2.x,pt2.y)=nu;
	  }

	  if(pt1.y-1>0 && temp(pt1.x,pt1.y-1)==pixv)
	  {
		  pt2.x=pt1.x;pt2.y=pt1.y-1;
		  frontier.push(pt2);
		  img(pt2.x,pt2.y)=newcolor;
		  temp(pt2.x,pt2.y)=nu;
	  }

	  if(pt1.x+1<width-1 && temp(pt1.x+1,pt1.y)==pixv)
	  {
		  pt2.x=pt1.x+1;pt2.y=pt1.y;
		  frontier.push(pt2);
		  img(pt2.x,pt2.y)=newcolor;
		  temp(pt2.x,pt2.y)=nu;
	  }

	  if(pt1.x-1>0 && temp(pt1.x-1,pt1.y)==pixv)
	  {
		  pt2.x=pt1.x-1;pt2.y=pt1.y;
		  frontier.push(pt2);
		  img(pt2.x,pt2.y)=newcolor;
		  temp(pt2.x,pt2.y)=nu;
	  }

	  if(pt1.x+1<width-1 && pt1.y+1<height-1 && temp(pt1.x+1,pt1.y+1)==pixv)
	  {
		  pt2.x=pt1.x+1;pt2.y=pt1.y+1;
		  frontier.push(pt2);
		  img(pt2.x,pt2.y)=newcolor;
		  temp(pt2.x,pt2.y)=nu;
	  }

	  if(pt1.x+1<width-1 && pt1.y-1>0 && temp(pt1.x+1,pt1.y-1)==pixv)
	  {
		  pt2.x=pt1.x+1;pt2.y=pt1.y-1;
		  frontier.push(pt2);
		  img(pt2.x,pt2.y)=newcolor;
		  temp(pt2.x,pt2.y)=nu;
	  }

	  if(pt1.x-1>0 && pt1.y+1<height-1 && temp(pt1.x-1,pt1.y+1)==pixv)
	  {
		  pt2.x=pt1.x-1;pt2.y=pt1.y+1;
		  frontier.push(pt2);
		  img(pt2.x,pt2.y)=newcolor;
		  temp(pt2.x,pt2.y)=nu;
	  }

	  if(pt1.x-1>0 && pt1.y-1>0 && temp(pt1.x-1,pt1.y-1)==pixv)
	  {
		  pt2.x=pt1.x-1;pt2.y=pt1.y-1;
		  frontier.push(pt2);
		  img(pt2.x,pt2.y)=newcolor;
		  temp(pt2.x,pt2.y)=nu;
	  }
  }


  return img;
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
			ImgBgr img,img1;
			Bgr colo,pixv;
			colo.b=0;colo.g=0;colo.r=255;
		    char str[50];
			cout<<"enter path to your image:";
			gets(str);
			Load(str,&img);
			Figure disp;
			disp.Draw(img);
			CPoint pixel=disp.GrabMouseClick();
			pixv=img(pixel.x,pixel.y);
			img1=Floodfill(img,pixel,pixv,colo);
			disp.Draw(img1);
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
