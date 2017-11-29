#pragma once

#include "parametervirtualwidget.h"

class ParameterSlider : public ParameterVirtualWidget
{
  Q_OBJECT
public:
  ParameterSlider(ParameterObject *parameterobject, int showDescription);
  void setValue();
  void setParameterFocus();

private:
  double step;
  bool pressed;
  bool volatile suppressUpdate;

protected slots:
  void onSliderChanged(int);
  void onSpinBoxChanged(double);
  void onReleased();
  void onPressed();
};
