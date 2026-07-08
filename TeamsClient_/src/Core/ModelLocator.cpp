#include "ModelLocator.h"

ModelLocator& ModelLocator::instance() {
  static ModelLocator locator;
  return locator;
}

void ModelLocator::clear() {
  models_.clear();
}