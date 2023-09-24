#include "NEW_Pitch.hpp"

namespace NEW_ScoreGenLib {
namespace NEW_Music {

	// NEW_Alteration
	NEW_Exception NEW_Alteration::badAlterationValue = NEW_Exception("An alteration should be an integer or a half-integers between -2 and 2.");

	// NEW_Pitch
	NEW_Pitch::NEW_Pitch(double midiValue) {
		// Vérifier, et extraire la partie entière et la partie fractionnaire
		double twice = midiValue + midiValue;
		double twiceRound8 = floor(twice * 100000000 + 0.5) * 0.00000001;
		double twiceDiff = abs(twice - twiceRound8);
		if (twiceDiff != 0) throw NEW_Exception("The MIDI value should be an integer or a half-integer.");
		// Initialiser
		int ffloor = (int)floor(midiValue + 0.1);
		bool isHalf = (midiValue - ffloor > 0.1);
		initializeFromMidiValue(ffloor, isHalf);
	}
	void NEW_Pitch::initializeFromStepAlterationOctave(char step, const NEW_Alteration& alteration, int octave) {
		_type = 'n';
		// Initialisation de '_step'
		if      (step >= 'A' && step <= 'G') _step = step;
		else if (step >= 'a' && step <= 'g') _step = step - 32;
		else                                 throw NEW_Exception("The step should be a char between A and G or between a and g.");
		// Initialisation des autres champs
		_midiValue  = calcMidiValue(_step, alteration, octave);
		_alteration = alteration                              ;
		_octave     = octave                                  ;
		// Vérification de la valeur MIDI
		if (_midiValue < -5000 || _midiValue > 5127) throw NEW_Exception("The MIDI value of a pitch should be between -5000 and 5127.");
	}
	void NEW_Pitch::initializeFromMidiValue(int midiValue_floor, bool addHalf) {
		_type = 'n';
		_midiValue = addHalf ? 0.5 + (double)midiValue_floor : (double)midiValue_floor;
		// Vérifier la valeur MIDI
		if (_midiValue < -5000 || _midiValue > 5127) throw NEW_Exception("The MIDI value of a pitch should be between -5000 and 5127.");
		// Déterminer l'octave et conserver l'information de note
		_octave = (midiValue_floor + 120000) / 12 - 10001;
		int note_floor = midiValue_floor - 12 * (_octave + 1);
		// Déterminer la note et l'altération
		if      (note_floor ==  0 && !addHalf) { _step = 'C'; _alteration = NEW_Alteration(0   ); }
		else if (note_floor ==  0            ) { _step = 'C'; _alteration = NEW_Alteration(0.5 ); }
		else if (note_floor ==  1 && !addHalf) { _step = 'C'; _alteration = NEW_Alteration(1   ); }
		else if (note_floor ==  1            ) { _step = 'D'; _alteration = NEW_Alteration(-0.5); }
		else if (note_floor ==  2 && !addHalf) { _step = 'D'; _alteration = NEW_Alteration(0   ); }
		else if (note_floor ==  2            ) { _step = 'D'; _alteration = NEW_Alteration(0.5 ); }
		else if (note_floor ==  3 && !addHalf) { _step = 'E'; _alteration = NEW_Alteration(-1  ); }
		else if (note_floor ==  3            ) { _step = 'E'; _alteration = NEW_Alteration(-0.5); }
		else if (note_floor ==  4 && !addHalf) { _step = 'E'; _alteration = NEW_Alteration(0   ); }
		else if (note_floor ==  4            ) { _step = 'E'; _alteration = NEW_Alteration(0.5 ); }
		else if (note_floor ==  5 && !addHalf) { _step = 'F'; _alteration = NEW_Alteration(0   ); }
		else if (note_floor ==  5            ) { _step = 'F'; _alteration = NEW_Alteration(0.5 ); }
		else if (note_floor ==  6 && !addHalf) { _step = 'F'; _alteration = NEW_Alteration(1   ); }
		else if (note_floor ==  6            ) { _step = 'G'; _alteration = NEW_Alteration(-0.5); }
		else if (note_floor ==  7 && !addHalf) { _step = 'G'; _alteration = NEW_Alteration(0   ); }
		else if (note_floor ==  7            ) { _step = 'G'; _alteration = NEW_Alteration(0.5 ); }
		else if (note_floor ==  8 && !addHalf) { _step = 'G'; _alteration = NEW_Alteration(1   ); }
		else if (note_floor ==  8            ) { _step = 'A'; _alteration = NEW_Alteration(-0.5); }
		else if (note_floor ==  9 && !addHalf) { _step = 'A'; _alteration = NEW_Alteration(0   ); }
		else if (note_floor ==  9            ) { _step = 'A'; _alteration = NEW_Alteration(0.5 ); }
		else if (note_floor == 10 && !addHalf) { _step = 'B'; _alteration = NEW_Alteration(-1  ); }
		else if (note_floor == 10            ) { _step = 'B'; _alteration = NEW_Alteration(-0.5); }
		else if (note_floor == 11 && !addHalf) { _step = 'B'; _alteration = NEW_Alteration(0   ); }
		else if (note_floor == 11            ) { _step = 'B'; _alteration = NEW_Alteration(0.5 ); }
		else throw NEW_Exception();
	}
	void NEW_Pitch::initializeFromValAndNbVals(int val, int nbVals) {
		_type = 'p';
		// Vérifications
		if (nbVals < 1 || nbVals > 25) throw NEW_Exception("The number of possible values should be between 1 and 25.");
		if (val < 1 || val > nbVals) throw NEW_Exception("The value should be between 1 and the number of possible values.");
		// Valeur MIDI (position sur la portée)
		int midi;
		switch (nbVals) {
			case 1: // 1 ligne : B4
				midi = 71;
			break;
			case 2: // 2 lignes : G4 D5
				if (val == 1) midi = 67;
				else          midi = 74;
			break;
			case 3: // 3 lignes : E4 B4 F5
				if      (val == 1) midi = 64;
				else if (val == 2) midi = 71;
				else               midi = 77;
			break;
			case 4: // 4 lignes : F4 A4 C5 E5
				if      (val == 1) midi = 65;
				else if (val == 2) midi = 69;
				else if (val == 3) midi = 72;
				else               midi = 76;
			break;
			case 5: // 5 lignes : E4 G4 B4 D5 F5
				if      (val == 1) midi = 64;
				else if (val == 2) midi = 67;
				else if (val == 3) midi = 71;
				else if (val == 4) midi = 74;
				else               midi = 77;
			break;
			case 6: // 6 interlignes : D4 F4 A4 C5 E5 G5
				if      (val == 1) midi = 62;
				else if (val == 2) midi = 65;
				else if (val == 3) midi = 69;
				else if (val == 4) midi = 72;
				else if (val == 5) midi = 76;
				else               midi = 79;
			break;
			case 7: // 7 lignes : C4 E4 G4 B4 D5 F5 A5
				if      (val == 1) midi = 60;
				else if (val == 2) midi = 64;
				else if (val == 3) midi = 67;
				else if (val == 4) midi = 71;
				else if (val == 5) midi = 74;
				else if (val == 6) midi = 77;
				else               midi = 81;
			break;
			case 8: // 8 interlignes : B3 D4 F4 A4 C5 E5 G5 B5
				if      (val == 1) midi = 59;
				else if (val == 2) midi = 62;
				else if (val == 3) midi = 65;
				else if (val == 4) midi = 69;
				else if (val == 5) midi = 72;
				else if (val == 6) midi = 76;
				else if (val == 7) midi = 79;
				else               midi = 83;
			break;
			default: // Notes contiguës de la gamme de do (centrées autour de B4)
				int indexInScale = 6 - (nbVals + 1) / 2 + val;
				int octaveShift = 0;
				while (indexInScale < 0) { indexInScale += 7; octaveShift--; }
				while (indexInScale > 6) { indexInScale -= 7; octaveShift++; }
				if      (indexInScale == 0) midi = 60 + 12 * octaveShift;
				else if (indexInScale == 1) midi = 62 + 12 * octaveShift;
				else if (indexInScale == 2) midi = 64 + 12 * octaveShift;
				else if (indexInScale == 3) midi = 65 + 12 * octaveShift;
				else if (indexInScale == 4) midi = 67 + 12 * octaveShift;
				else if (indexInScale == 5) midi = 69 + 12 * octaveShift;
				else                        midi = 71 + 12 * octaveShift;
		}
		_midiValue = midi;
		// Step, altération, octave
		{
			int midi = (int)_midiValue;
			switch (midi % 12) {
				case  0: _step = 'C'; break;
				case  2: _step = 'D'; break;
				case  4: _step = 'E'; break;
				case  5: _step = 'F'; break;
				case  7: _step = 'G'; break;
				case  9: _step = 'A'; break;
				case 11: _step = 'B'; break;
				default: throw NEW_Exception();
			}
			_alteration = 0;
			_octave = midi / 12 - 1;
		}
	}
	double NEW_Pitch::calcMidiValue(char step, const NEW_Alteration& alteration, int octave) {
		int step_int; {
			if      (step == 'C') step_int =  0;
			else if (step == 'D') step_int =  2;
			else if (step == 'E') step_int =  4;
			else if (step == 'F') step_int =  5;
			else if (step == 'G') step_int =  7;
			else if (step == 'A') step_int =  9;
			else if (step == 'B') step_int = 11;
			else throw NEW_Exception();
		}
		return 12 * (octave + 1) + step_int + (double)alteration;
	}
	std::string NEW_Pitch::convertToMusicXML() const {
		double xml_alter = (double)_alteration;
		std::string ans = "";
		ans += "<pitch>";
		ans +=     "<step>" + toString(_step) + "</step>";
		ans +=     (xml_alter == 0 ? "" : "<alter>" + toString(xml_alter) + "</alter>");
		ans +=     "<octave>" + toString(_octave) + "</octave>";
		ans += "</pitch>";
		return ans;
	}

}
}
