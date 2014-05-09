// --*-c++-*--

#include <stdlib.h>
#include <mpi.h>
#include<assert.h>
#include <iostream>
#include <string>

#include "image.h"
#include "image_utils.h"
void parallel_mean(int *rbuf,int width,int height,int windowSize);
std::string parallel_main(int argc, char* argv[])
{
  if (argc < 5) {
    std::cout << "usage: " << argv[0] << " "
	      << "<in image> <out image> <filter type> <window size>"
              << std::endl;
    std::cout << " filter type = {mean|median} " << std::endl;
    exit(-1);
  }
 int rank, namelen,num_procs;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name(processor_name, &namelen);
  MPI_Comm_size(MPI_COMM_WORLD,&num_procs);
  std::string inFilename(argv[1]);
  std::string outFilename(argv[2]);
  std::string filterType(argv[3]);
  int windowSize = atoi(argv[4]);

  outFilename = "p_" + outFilename;
  uint32_t *buf;
  int height,width;
  Image input,output;
  if (rank == 0) {
    std::cout << "* doing parallel image filtering ** " << std::endl;
    std::cout << "* reading regular tiff: " << argv[1] << std::endl;
    input.load_tiff(std::string(argv[1]));
    output=input;
	output.make_greyscale();
	height=output.height();
    width=output.width();
	buf = (uint32_t *) malloc(sizeof(uint32_t)*height*width);
    buf=output.getdata();
    
    	 	
  }
	//std::cout<<"loaded the image \n";
  //Image output = input;
  
  MPI_Bcast(&height,1,MPI_INT,0,MPI_COMM_WORLD);
  MPI_Bcast(&width,1,MPI_INT,0,MPI_COMM_WORLD);
  MPI_Bcast(&num_procs,1,MPI_INT,0,MPI_COMM_WORLD);
  //MPI_Bcast(,1,MPI_INT,0,MPI_COMM_WORLD);
  int upperX= windowSize/2;
  int lowerX=windowSize-(windowSize/2);
  int chunk=height/num_procs;
  int *displace=(int *) malloc(sizeof(int)*num_procs); 
  int *sendcount=(int *) malloc(sizeof(int)*num_procs);
  int i;
  for(i=0;i<num_procs-1;i++)
  {
    if(i==0)
      upperX=0;
    sendcount[i]=(upperX+chunk+lowerX)*width;
    if(i==0)
       displace[i]=0;
    else
       displace[i]= (i*chunk-upperX)*width;

   upperX=windowSize/2;   
         
  }
    int rreceived;//int rreceived=(int*)malloc(sizeof(int)*(height/num_procs));
   if(num_procs == 1)
   {
	upperX = 0;
    }
  sendcount[i]=(upperX+chunk+(height%num_procs))*width;
  displace[i]=(i*chunk-upperX)*width;
  int *rbuf=(int *)malloc(sizeof(int)*(sendcount[rank]));
	//std::cout<<"initialized all the arrays and sending data \n";
	//std::cout<<"rank : "<<rank<<" displacement : "<<displace[rank]/width<<" send_count : "<<sendcount[rank]/width<<"\n";
  MPI_Scatterv(buf,sendcount,displace,MPI_UNSIGNED,rbuf,sendcount[rank],MPI_UNSIGNED,0,MPI_COMM_WORLD);
  std::cout << "[" << processor_name << "] processing with filter: " 
            << filterType << std::endl;
 time_t st;
  struct tm *das1;
  char xt[9];

   time( &st );
   memset(xt,0,sizeof(xt));
   das1 = localtime( &st );
   strncpy(xt,asctime(das1)+11,8);
   std::cout << xt << " "<< processor_name << " : received image size " << sendcount[rank]/width << " " << width << "\n";

   std::cout << xt << " " <<processor_name << " : processing " << filterType << "filter windowsize " << windowSize <<"\n";
  if (filterType == "mean") {

    parallel_mean(rbuf,width,sendcount[rank]/width,windowSize); 

  }
 else if (filterType == "median") {
     output.image_filter_median_parallel(rbuf,sendcount[rank]/width,width,windowSize);
  }
  int dis;
  int * rreceive=(int*)malloc(sizeof(int)*num_procs);
  for(i=0;i<num_procs;i++)
 {
  
  if(i<num_procs-1)
  rreceive[i]=(height/num_procs)*width;
  else
  rreceive[i]= ((height/num_procs)+(height%num_procs))*width;
	displace[i]= i*chunk*width;
 } 
	int send_count;	
	if(rank == 0)
    	dis=0;
  	else
    	dis=(windowSize/2)*width;
	if(rank == num_procs - 1 )
	send_count= ((height/num_procs)+(height%num_procs))*width;
	else
	send_count= (height/num_procs)*width;
	//std::cout<<"before gather \n";
	//std::cout<<"rank : "<<rank<<"offset : "<<dis<<"\n";
	//std::cout<<"rank : "<<rank<<"send count : "<<send_count/width<<"\n";
	//std::cout<<"rank : "<<rank<<" displacement : "<<displace[rank]/width<<" rec_count : "<<rreceive[rank]/width<<"\n";
	 time_t ttime;
  struct tm *das;
  char xt1[10];

  time( &ttime );
  memset(xt1,0,sizeof(xt1));
  das = localtime( &ttime);
  strncpy(xt1,asctime(das)+11,8);
  std::cout << xt1 << " " << processor_name <<  " send image to manager \n";
 MPI_Gatherv(rbuf+dis,send_count,MPI_UNSIGNED,buf,rreceive,displace,MPI_UNSIGNED,0,MPI_COMM_WORLD);
  
  if (rank == 0) {
	output.setdata(buf,width*height);
    std::cout << "[" << processor_name << "] saving image: " 
              << outFilename 
              << std::endl;
    output.save_tiff_grey_8bit(outFilename);
  }

  std::cout << "-- done --" << std::endl;
	
  return outFilename;
}
void parallel_mean(int *rbuf,int width,int height,int windowSize)
{
   // make copy of the existing image -- with borders
   std::vector<uint32_t> tmp_data;
   for(int i=0;i<width*height;i++)
       tmp_data.push_back(rbuf[i]);
    //m_width=width;
    //m_height=height;
   int edgex = windowSize/2;
   int edgey = windowSize/2;

   for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
         double sum = 0;
         int count = 0;
         for (int fx = 0; fx < windowSize; fx++) {
           for (int fy = 0; fy < windowSize; fy++) {
             int yy = y+fy-edgey;
             int xx = x+fx-edgex;
             if (yy < 0 || yy >= height || xx < 0 || xx >= width)
               continue;
             sum += (double)tmp_data.at((width*yy)+xx);
             count++;
           } 
         }
         assert(count != 0);
         rbuf[(width*y)+x]= (uint32_t)(sum/count);
         //P(m_image_data,y,x) 
      }
   }
}
