#include "adcircelement.h"

AdcircElement::AdcircElement() { this->m_nodes.resize(this->n()); }

AdcircElement::AdcircElement(int id, AdcircNode *n1, AdcircNode *n2,
                             AdcircNode *n3) {
  this->m_nodes.resize(3);
  this->m_id = id;
  this->m_nodes[0] = n1;
  this->m_nodes[1] = n2;
  this->m_nodes[2] = n3;
}

int AdcircElement::n() const { return this->m_n; }

void AdcircElement::setNode(int i, AdcircNode *node) {
  if (i < this->n())
    this->m_nodes[i] = node;
  return;
}

int AdcircElement::id() const { return this->m_id; }

void AdcircElement::setId(int id) { this->m_id = id; }

AdcircNode *AdcircElement::node(int i) {
  if (i < this->n())
    return this->m_nodes.at(i);
  return nullptr;
}
