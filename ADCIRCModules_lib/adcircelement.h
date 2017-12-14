#ifndef ADCIRCELEMENT_H
#define ADCIRCELEMENT_H

#include "adcircmodules_global.h"
#include "adcircnode.h"
#include <vector>

class AdcircElement {
public:
  explicit AdcircElement();
  explicit AdcircElement(int id, AdcircNode *n1, AdcircNode *n2,
                         AdcircNode *n3);

  int n() const;

  AdcircNode *node(int i);
  void setNode(int i, AdcircNode *node);

  int id() const;
  void setId(int id);

private:
  const int m_n = 3;

  int m_id;
  std::vector<AdcircNode *> m_nodes;
};

#endif // ADCIRCELEMENT_H
