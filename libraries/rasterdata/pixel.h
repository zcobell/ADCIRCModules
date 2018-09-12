#ifndef PIXEL_H
#define PIXEL_H

#include <cstddef>

class Pixel {
 public:
  Pixel();
  Pixel(size_t i, size_t j);

  size_t i() const;
  size_t j() const;

  void set(const size_t &i, const size_t &j);

 private:
  size_t m_i;
  size_t m_j;
};

#endif  // PIXEL_H
