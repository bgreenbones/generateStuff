#  Failed code that isn't worth it now



in Rhythm::randomClave()
            // todo: trying to reduce failed clave tries
            // put last note in a random spot, but only pick from ones that work.
            // I think this falls under pre-mature optimization
//
//            if (noteInd == numNotes - 1) { // last note
//                Note pentultimateNote = clave.mNotes.back();
//                vector<float> possiblePositions = {};
//
//                for (float pentultimateNoteLength = minNoteLength;
//                     pentultimateNoteLength <= maxNoteLength;
//                     pentultimateNoteLength += clave.mSubdivision) {
//
//                    float possiblePosition = pentultimateNote.mTime + pentultimateNoteLength;
//                    float finalNoteLength = (clave.mLength - possiblePosition) + clave.mNotes.front().mTime;
//
//                    if (possiblePosition < clave.mLength &&
//                        (finalNoteLength >= minNoteLengthInSubdivisions &&
//                         finalNoteLength <= maxNoteLengthInSubdivisions)) {
//                        possiblePositions.push_back(possiblePosition);
//                    }
//                }
//
//                // no possible spots for last note
//                if (possiblePositions.size() == 0) {
//                    constraintsBroken = true;
//                    break;
//                }
//
//                int choice = rand() % possiblePositions.size();
//                note.mTime = possiblePositions.at(choice);
//                note.mDuration = (clave.mLength - note.mTime) + clave.mNotes.front().mTime;
//                clave.mNotes.at(clave.mNotes.size() - 1) = note.mTime - pentultimateNote.mTime;
//            } else {
//            }
