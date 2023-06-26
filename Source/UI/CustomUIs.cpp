/*
  ==============================================================================

    CustomUIs.cpp
    Created: 5 Apr 2023 10:38:36pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "CustomUIs.h"


int ui::getButtonHeight(int rows, int height) {
    return (height - spaceBetweenControls * (rows - 1)) / rows;
};

int ui::getVoiceTileHeight(int height) {
  return getButtonHeight(6, height);
}
