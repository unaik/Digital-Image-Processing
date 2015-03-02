// assignment42.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "assignment42.h"
#include "blepo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;
using namespace blepo;

typedef struct {
	int mew;
	int numpix;
	int equiv;
}twig;

struct forest
{

	forest(int elements) ;
	int findset(int elementId) const;
	void merge(int setId1,int setId2,int w);
    int numpix(int elementId) const; 
	int numsets () const;
	twig * bush;
	int num;
	

};


int forest::numsets() const
{
	 return num; 
}

int forest::numpix(int elementId) const
{ 
	twig *a=&bush[elementId];
	{return (*a).numpix;}
}
forest::forest(int elements)
{
	bush= new twig [elements];
	num= elements;

	for(int i=0;i<elements;i++)
	{
		bush[i].mew=0;
		bush[i].numpix=1;
		bush[i].equiv=i;
	}
}



int forest:: findset(int elementId) const
{
	twig * b= &bush[elementId];
	while(  bush[elementId].equiv != elementId)
	{elementId=bush[elementId].equiv;}
	return elementId;
}

 void forest::merge(int setId1, int setId2, int w)
{
  
  if(setId1==setId2)
  {return;}

  twig * setIda = &bush[setId1];
  twig * setIdb = &bush[setId2];

  int a = min(findset(setId1),findset(setId2));
  int b=  max(findset(setId1),findset(setId2));

  (*setIdb).equiv=setId1;
  (*setIda).mew=max(w,max((*setIda).mew,(*setIdb).mew));
  (*setIda).numpix += (*setIdb).numpix;
  num--;
 }


float euclideandist(ImgFloat& F,ImgFloat& F1,ImgFloat& F2,int x1,int y1,int x2, int y2)
{
  float param,param1,param2,ed;
  param=F(x1,y1)-F(x2,y2);
  param1=F1(x1,y1)-F1(x2,y2);
  param2=F2(x1,y1)-F2(x2,y2);
  ed=sqrt(pow(param,2)+ pow(param1,2) +pow(param2,2));
  return ed;
  
}
 
 struct edge 
{
  float weight;
  int pt1,pt2;
  bool operator< (const edge& other )
  { return weight< other.weight;}
};

edge* calcedges(ImgFloat& F,ImgFloat& F1,ImgFloat& F2,int& ptr)
{
	int width=F.Width();
	int height=F.Height();
	int i=0;
	edge * edges;
	edges= new edge[width*height*10];
	for(int y=0;y<height;y++)
	{
		for(int x=0;x<width;x++)
		{
			if(x<width-1)
			{
				edges[i].pt1=y*width+x;
				edges[i].pt2=y*width+(x+1);
				edges[i].weight=euclideandist(F,F1,F2,x,y,x+1,y);
				i++;
			}
			
			if(y<height-1)
			{
			   edges[i].pt1=y*width+x;
			   edges[i].pt2=(y+1)*width+x;
			   edges[i].weight=euclideandist(F,F1,F2,x,y,x,y+1);
			   i++;
			}

			
		}
	}
	cout<<endl<<i;
	ptr=i;
	return edges;
}

edge* EdgeSorter(edge* edges,int i)
{
  std::sort(edges,edges+i);
  return edges;
    
}

forest *obtainfor(int vertices, int edges, edge *edges1, float k )
{
      forest *tempo= new forest(vertices);
	  float *threshold = new float[vertices];
	  for (int p=0; p<vertices; p++)
	  {threshold[p]= k/(*tempo).numpix(p);}

	 for (int i = 0; i < edges; i++)
	 {
		
	 	 int a = (*tempo).findset(edges1[i].pt1);
		 int b = (*tempo).findset(edges1[i].pt2);
		 if (a!=b)
		 {
			 if((edges1[i].weight) <= min(threshold[a],threshold[b]))
			 {
			  (*tempo).merge(a,b,edges1[i].weight);
			  a= (*tempo).findset(a);
			  threshold[a]=edges1[i].weight + (k/(*tempo).numpix(a));
			 }
		 }
	 }
	 
	 
	
	 return tempo;
}
Bgr RandomColor()
{
   Bgr random;

   random.b = rand() % 256;
   random.g = rand() % 256;
   random.r = rand() % 256;

   return random;
}

Bgr GetColor(int x, int y,ImgBgr& B2)
{
  Bgr col;
  col=B2(x,y);
  return col;
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
			CString  image;
			CFileDialog fileDlg(TRUE, NULL, NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,NULL);
			if(fileDlg.DoModal() == IDOK)
			{
				 image = fileDlg.GetPathName();
			}

			float k;
			cout<<"enter value of k"<<endl;
			cin>>k;
			float sigma;
			cout<<"enter value for sigma"<<endl;
			cin>>sigma;
			int minimumsize;
			cout<<"enter value of minimum size that you wish to enforce";
			cin>>minimumsize;
			ImgBgr B,B1,B2,B3;
			ImgGray G,G1,G2;
			ImgFloat F,F1,F2;
			int ptr=0;
			edge * edges;
			Load(image,&B);
			int width=B.Width();
			int height=B.Height();
			B1=B;
			B2=B;
			ExtractBgr(B,&G,&G1,&G2);
			Convert(G,&F);
			Convert(G1,&F1);
			Convert(G2,&F2);
			Smooth(F,sigma,&F);
			Smooth(F1,sigma,&F1);
			Smooth(F2,sigma,&F2);
			Figure f,f1,f2;
			f.Draw(B);
			f.SetTitle("Orignal Image");
			f1.Draw(F1);
			f2.Draw(F2);
			edges=calcedges(F,F1,F2,ptr);
			cout<<endl<<"ptr value  "<<ptr<<endl;
			edges=EdgeSorter(edges,ptr);
			forest *forrest=obtainfor(width*height, ptr, edges, k);
			

			for(int i=0; i<ptr; i++)
			{
			   int a= (*forrest).findset(edges[i].pt1);
			   int b= (*forrest).findset(edges[i].pt2);
			   
			   if((a != b) && (((*forrest).numpix(a)< minimumsize) || ((*forrest).numpix(b)< minimumsize)))
				   (*forrest).merge(a,b,edges[i].weight);
			}

			
			cout<<"number of sets"<<(*forrest).numsets();
			int numsets= (*forrest).numsets();
			ImgFloat I(width,height),I1(width,height);
			ImgInt RI(width,height);

			for(int y=0;y< height; y++)
				for(int x=0; x< width; x++)
				{
				  RI(x,y)=0;
				  RI(x,y)=(*forrest).findset(y*width+x);
			  	}
            Bgr * mean = new Bgr[width*height*2];
			int j=0;
			for(int y=0;y<height-1;y++)
				for(int x=0;x<width-1;x++)
				{
				  mean[j]=GetColor(x,y,B2);
				  j++;
				}

			for(int y=0;y<height-1;y++)
				for(int x=0;x<width-1;x++)
				{
				  int color1= (*forrest).findset(y*width+x);
				  B2(x,y)=mean[color1];
				  
				}

				
				

            Bgr * psuedo = new Bgr [width*height];
            for(int i=0;i<width*height;i++)
			{
			   psuedo[i]=RandomColor();

			}
				
			for(int y=0;y<height-1;y++)
				for(int x=0;x<width-1;x++)
				{
				   int color = (*forrest).findset(y*width+x);
				   B(x,y)=psuedo[color];
				} 
			Bgr boundary;
			boundary.b=255;boundary.g=255;boundary.r=255;
			for (int y=0;y<height-1;y++)
				for(int x=0;x<width-1;x++)
				{
				  
					  if(RI(x,y) != RI(x+1,y))
					  {B1(x,y)=boundary;} 

					  if(RI(x,y) != RI(x,y+1))
					  {B1(x,y)=boundary;}
				  

				}

				
            Figure In;
			
			In.Draw(B1);
			In.SetTitle("Boundaries Overlaid");

			f1.Draw(B);
			f1.SetTitle("Psuedo Color");
			f2.Draw(B2);
			f2.SetTitle("Mean Image");
			
			
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

