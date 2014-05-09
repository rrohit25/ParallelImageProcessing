#include <stdlib.h>

#include <iostream>
#include <string>

#include "image.h"
#include "image_utils.h"

int main(){
Image input1,input2;
std::cout<<std::string(argv[5])<<" "<<std::string(argv[6])<<"\n";
input1.load_tiff(std::string(argv[5]));
input2.load_tiff(std::string(argv[6]));
int ii=0;
int height1 = input1.height();
int width1 = input1.width();

uint32_t *in1data,*in2data;
	in1data = (uint32_t *) malloc(sizeof(uint32_t)*height1*width1);
	in2data = (uint32_t *) malloc(sizeof(uint32_t)*height1*width1);
int flag=0;
std::cout<<"comparing data \n";
for(ii=0; ii<height1 * width1; ii++ )
{
	if(in1data[ii] == in2data[ii])
	continue;
	else 
	{
	flag=1;
	break;
	}
}
if(flag == 1)
std::cout<<"data is not same at : "<<ii<<"\n";
else
std::cout<<"data is the same\n";

return 0;
}
