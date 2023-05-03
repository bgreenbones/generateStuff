/*
  ==============================================================================

    Form.h
    Created: 3 May 2023 6:45:09pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/
#pragma once

#include "Sequence.h"
#include "TimedEvent.h"



class Form : public TimedEvent {
  Sequence<TimedEvent> sections;
};