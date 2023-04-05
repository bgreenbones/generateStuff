/*
  ==============================================================================

    CustomUIs.h
    Created: 31 Dec 2022 11:29:40pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once


#include <string>
#include <vector>
#include <JuceHeader.h>
using std::string, std::vector;


namespace ui {
    const int editorWidth = 1200;
    const int editorHeight = 600;
    const int sliderWidth = 20;
    const int spaceBetweenControls = 10;

    static int getButtonHeight(int rows, int height) {
        return (height - spaceBetweenControls * (rows - 1)) / rows;
    };
}



/*
 ### Place for trying UI generalization stuff before moving out into their own files.
 */
