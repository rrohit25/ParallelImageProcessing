// --*-c++-*--


#ifndef _IMAGE_H
#define _IMAGE_H

#include <stdint.h>

#include <vector>
#include <string>

class Image {
public:
  Image();
  Image(const Image &other);
  Image& operator=(const Image &rhs);
 
  void load_tiff(std::string input_filename);
  void save_tiff_rgb(std::string output_filename);
  void save_tiff_grey_32bit(std::string output_filename);
  void save_tiff_grey_8bit(std::string output_filename);

  void make_greyscale();
  void image_filter_mean(int windowSize);
  void image_filter_median(int windowSize);
void image_filter_parallel_mean(int *rbuf,int width,int height,int windowSize);
 void image_filter_median_parallel(int *rbuf,int height,int width,int windowSize);
  inline size_t& height() 
  { 
    return m_height; 
  };

  inline size_t& width() 
  { 
    return m_width; 
  };

  inline uint32_t& operator()(int r, int c) 
  {
     return P(r,c);
  }
  uint32_t* getdata()
  {
	return &m_image_data[0];

  }
  void setdata(uint32_t * buf, int size)
  {
       for(int i=0;i<size;i++)
 		m_image_data.push_back(buf[i]);
  }
  inline uint32_t& P(std::vector<uint32_t> &data, int r, int c)
  {
     size_t index = m_width * r + c;
     return data[index];
  } 

  inline uint32_t& P(int r, int c)
  {
     size_t index = m_width * r + c;
     return m_image_data[index];
  }
 
private:
  std::vector<uint32_t> m_image_data;
  size_t m_width;
  size_t m_height;
};

#endif 
