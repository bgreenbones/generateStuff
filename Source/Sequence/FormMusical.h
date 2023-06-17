
/*
  ==============================================================================

    Form.h
    Created: 3 May 2023 6:45:09pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/
#pragma once

#include "GenerateStuffEditorState.h"
#include "Sequence.h"
#include "TimedEvent.h"


// class ScheduleItem : public TimedEvent {
//   public:
//     Phrase phrase;
// };

// class Section : public TimedEvent {
//   GenerateStuffEditorState editorState;
// };



class Form : public Time {
public:
  Sequence<Time> sections;
  Form() : Time(0, Bars(8, true)), sections(*this)
   {}
};