#pragma region head

	#pragma once

	#include "2. Transformation.h"

	using namespace System;

#pragma endregion

namespace ScoreGenLib {
namespace Music {
	
	// Altération (entier ou semi-entier non mutable entre -2 et 2)
	public ref class Alteration {
	
		// Champs
		private:
			double _val;
		private:
			static Exception^ badAlterationValue = gcnew Exception("An alteration should be an integer or a half-integers between -2 and 2.");
		
		// Constructeur
		public:
			Alteration(double val) :
				_val(val)
			{
				
				if (_val != 0 && _val != 1 && _val != -1 && _val != 2 && _val != -2 && _val != 0.5 && _val != -0.5 && _val != 1.5 && _val != -1.5) {
					throw badAlterationValue;
				}
			
			}
			Alteration(int val) :
				_val(val)
			{
			
				if (_val < -2 || _val > 2) {
					throw badAlterationValue;
				}
			
			}
		
		// Valeurs
		public:
			static property Alteration^ natural       { inline Alteration^ get() { return gcnew Alteration( 0  ); } }
			static property Alteration^ sharp         { inline Alteration^ get() { return gcnew Alteration( 1  ); } }
			static property Alteration^ flat          { inline Alteration^ get() { return gcnew Alteration(-1  ); } }
			static property Alteration^ doubleSharp   { inline Alteration^ get() { return gcnew Alteration( 2  ); } }
			static property Alteration^ doubleFlat    { inline Alteration^ get() { return gcnew Alteration(-2  ); } }
			static property Alteration^ halfSharp     { inline Alteration^ get() { return gcnew Alteration( 0.5); } }
			static property Alteration^ halfFlat      { inline Alteration^ get() { return gcnew Alteration(-0.5); } }
			static property Alteration^ sharpAndAHalf { inline Alteration^ get() { return gcnew Alteration( 1.5); } }
			static property Alteration^ flatAndAHalf  { inline Alteration^ get() { return gcnew Alteration(-1.5); } }
		
		// Opérateurs
		public:
			static inline bool operator==(Alteration^ a, Alteration^ b) {
	
				return a->_val == b->_val;
	
			}
			static inline bool operator!=(Alteration^ a, Alteration^ b) {
				
				return a->_val != b->_val;
	
			}
			static inline bool operator==(Alteration^ a, double val) {
	
				return a->_val == val;
	
			}
			static inline bool operator!=(Alteration^ a, double val) {
				
				return a->_val != val;
	
			}
			static inline bool operator==(Alteration^ a, int val) {
	
				return a->_val == val;
	
			}
			static inline bool operator!=(Alteration^ a, int val) {
				
				return a->_val != val;
	
			}
			static inline operator double(Alteration^ a) {
				
				return a->_val;
				
			}
	
	};

	// Hauteur de note (autorisant aussi les quarts de tons, les notes de percus et les hauteurs non spécifiées)
	public ref class Pitch : Transformations::Transformable<Pitch^> {
		
		// Champs
		private:
			char        _type      ; // n = note | p = percu | x = pitch non spécifié (par exemple : rythmique en attente de mélodie)
			double      _midiValue ; // valeur MIDI du pitch, ou valeur MIDI de la position sur la portée
			char        _step      ; // A - G
			Alteration^ _alteration; // altération entière ou demi-entière, entre -2 et +2
			int         _octave    ; // l'octave 4 commence au do serrure
		private:
			int         _p_val     ; // dans le cas d'un pitch de percu, trace de la valeur parmi les valeurs possibles
			int         _p_nbVals  ; // dans le cas d'un pitch de percu, trace du nombre total de valeurs possibles
		
		// Constructeurs
		public:
			// Type n : par le nom de note, l'altération et l'octave
			Pitch(char step, Alteration^ alteration, int octave) {
			
				initializeFromStepAlterationOctave(step, alteration, octave);
			
			}
			Pitch(char step, double alteration, int octave) {
			
				initializeFromStepAlterationOctave(step, gcnew Alteration(alteration), octave);
			
			}
			Pitch(char step, int alteration, int octave) {
			
				initializeFromStepAlterationOctave(step, gcnew Alteration(alteration), octave);
			
			}
			// Type n : par la valeur MIDI
			Pitch(double midiValue) {
				
				// Vérifications
				double twice = midiValue + midiValue;
				double twiceDiff = Math::Abs(twice - Math::Round(twice, 8));
				if (twiceDiff != 0) throw gcnew Exception("The MIDI value should be an integer or a half-integer.");
				
				// Initialisation
				int floor = (int)Math::Floor(midiValue + 0.1);
				bool isHalf = (midiValue - floor > 0.1);
				initializeFromMidiValue(floor, isHalf);

			}
			Pitch(int midiValue) {
				
				initializeFromMidiValue(midiValue, false);

			}
			// Type p : par la valeur et le nombre total de valeurs
			Pitch(int val, int nbVals) {
				
				initializeFromValAndNbVals(val, nbVals);
			
			}
			// Type x
			Pitch() :
				_type      ('x'),
				_midiValue (71),
				_step      ('B'),
				_alteration(Alteration::natural),
				_octave    (4)
			{ }
		private:
			// Initialisateurs privés
			void initializeFromStepAlterationOctave(char step, Alteration^ alteration, int octave) {
				
				_type = 'n';

				// Initialisation de '_step'
				if      (step >= 'A' && step <= 'G') _step = step;
				else if (step >= 'a' && step <= 'g') _step = step - 32;
				else                                 throw gcnew Exception("The step should be a char between A and G or between a and g.");

				// Initialisation des autres champs
				_midiValue  = calcMidiValue(_step, alteration, octave);
				_alteration = alteration                              ;
				_octave     = octave                                  ;

				// Vérification de la valeur MIDI
				if (_midiValue < -5000 || _midiValue > 5127) throw gcnew Exception("The MIDI value of a pitch should be between -5000 and 5127.");
	
			}
			void initializeFromMidiValue(int midiValue_floor, bool addHalf) {

				_type = 'n';
				_midiValue = addHalf ? 0.5 + (double)midiValue_floor : (double)midiValue_floor;

				// Vérifier la valeur MIDI
				if (_midiValue < -5000 || _midiValue > 5127) throw gcnew Exception("The MIDI value of a pitch should be between -5000 and 5127.");

				// Déterminer l'octave et conserver l'information de note
				_octave = (midiValue_floor + 120000) / 12 - 10001;
				int note_floor = midiValue_floor - 12 * (_octave + 1);

				// Déterminer la note et l'altération
				if      (note_floor ==  0 && !addHalf) { _step = 'C'; _alteration = gcnew Alteration(0   ); }
				else if (note_floor ==  0            ) { _step = 'C'; _alteration = gcnew Alteration(0.5 ); }
				else if (note_floor ==  1 && !addHalf) { _step = 'C'; _alteration = gcnew Alteration(1   ); }
				else if (note_floor ==  1            ) { _step = 'D'; _alteration = gcnew Alteration(-0.5); }
				else if (note_floor ==  2 && !addHalf) { _step = 'D'; _alteration = gcnew Alteration(0   ); }
				else if (note_floor ==  2            ) { _step = 'D'; _alteration = gcnew Alteration(0.5 ); }
				else if (note_floor ==  3 && !addHalf) { _step = 'E'; _alteration = gcnew Alteration(-1  ); }
				else if (note_floor ==  3            ) { _step = 'E'; _alteration = gcnew Alteration(-0.5); }
				else if (note_floor ==  4 && !addHalf) { _step = 'E'; _alteration = gcnew Alteration(0   ); }
				else if (note_floor ==  4            ) { _step = 'E'; _alteration = gcnew Alteration(0.5 ); }
				else if (note_floor ==  5 && !addHalf) { _step = 'F'; _alteration = gcnew Alteration(0   ); }
				else if (note_floor ==  5            ) { _step = 'F'; _alteration = gcnew Alteration(0.5 ); }
				else if (note_floor ==  6 && !addHalf) { _step = 'F'; _alteration = gcnew Alteration(1   ); }
				else if (note_floor ==  6            ) { _step = 'G'; _alteration = gcnew Alteration(-0.5); }
				else if (note_floor ==  7 && !addHalf) { _step = 'G'; _alteration = gcnew Alteration(0   ); }
				else if (note_floor ==  7            ) { _step = 'G'; _alteration = gcnew Alteration(0.5 ); }
				else if (note_floor ==  8 && !addHalf) { _step = 'G'; _alteration = gcnew Alteration(1   ); }
				else if (note_floor ==  8            ) { _step = 'A'; _alteration = gcnew Alteration(-0.5); }
				else if (note_floor ==  9 && !addHalf) { _step = 'A'; _alteration = gcnew Alteration(0   ); }
				else if (note_floor ==  9            ) { _step = 'A'; _alteration = gcnew Alteration(0.5 ); }
				else if (note_floor == 10 && !addHalf) { _step = 'B'; _alteration = gcnew Alteration(-1  ); }
				else if (note_floor == 10            ) { _step = 'B'; _alteration = gcnew Alteration(-0.5); }
				else if (note_floor == 11 && !addHalf) { _step = 'B'; _alteration = gcnew Alteration(0   ); }
				else if (note_floor == 11            ) { _step = 'B'; _alteration = gcnew Alteration(0.5 ); }
				else throw gcnew Exception;

			}
			void initializeFromValAndNbVals(int val, int nbVals) {

				_type = 'p';
				_p_val = val;
				_p_nbVals = nbVals;

				// Vérifications
				if (nbVals < 1 || nbVals > 25) throw gcnew Exception("The number of possible values should be between 1 and 25.");
				if (val < 1 || val > nbVals) throw gcnew Exception("The value should be between 1 and the number of possible values.");

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
					switch (midi % 12) {
						case  0: _step = 'C'; break;
						case  2: _step = 'D'; break;
						case  4: _step = 'E'; break;
						case  5: _step = 'F'; break;
						case  7: _step = 'G'; break;
						case  9: _step = 'A'; break;
						case 11: _step = 'B'; break;
						default: throw gcnew Exception;
					}
					_alteration = Alteration::natural;
					_octave = midi / 12 - 1;
				}

			}
		public:
			// Conversion implicite en tableau singleton (--> pouvoir toujours donner un pitch unique là où un accord est attendu)
			operator array<Pitch^, 1>^ () {
			
				return gcnew array<Pitch^, 1>{ this };

			}
		
		// Accesseurs
		public:
			property char type {
				inline char get() {
					return _type;
				}
			}
			inline bool isPitch() {
				return (_type == 'n');
			}
			inline bool isPercu() {
				return (_type == 'p');
			}
			inline bool isUnspecified() {
				return (_type == 'x');
			}
			inline bool isSpecified() {
				return (_type != 'x');
			}
			property double midiValue {
				inline double get() {
					return _midiValue;
				}
			}
			property char step {
				inline char get() {
					if (isSpecified()) return _step;
					else               throw gcnew Exception("This pitch hasn't a step.");
				}
			}
			property Alteration^ alteration {
				inline Alteration^ get() {
					if (isSpecified()) return _alteration;
					else               throw gcnew Exception("This pitch hasn't an alteration.");
				}
			}
			property int octave {
				inline int get() {
					if (isSpecified()) return _octave;
					else               throw gcnew Exception("This pitch hasn't an octave.");
				}
			}

		// Opérateurs
		public:
			// Comparaison
			inline bool operator==(Pitch^ other) {
				return _type == other->_type && _step == other->_step && _alteration == other->_alteration && _octave == other->_octave;
			}
			inline bool operator!=(Pitch^ other) {
				return _type != other->_type || _step != other->_step || _alteration != other->_alteration || _octave != other->_octave;
			}
			inline bool operator==(double other) {
				if (_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else              return (_midiValue == other);
			}
			inline bool operator!=(double other) {
				if (_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else              return (_midiValue != other);
			}
			inline bool operator==(int other) {
				if (_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else              return (_midiValue == other);
			}
			inline bool operator!=(int other) {
				if (_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else              return (_midiValue != other);
			}
			inline bool operator< (Pitch^ other) {
				if (_type == 'x' || other->_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else                                     return _midiValue < other->_midiValue;
			}
			inline bool operator< (double other) {
				if (_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else              return _midiValue < other;
			}
			inline bool operator< (int other) {
				if (_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else              return _midiValue < other;
			}
			inline bool operator> (Pitch^ other) {
				if (_type == 'x' || other->_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else                                     return _midiValue > other->_midiValue;
			}
			inline bool operator> (double other) {
				if (_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else              return _midiValue > other;
			}
			inline bool operator> (int other) {
				if (_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else              return _midiValue > other;
			}
			inline bool operator<=(Pitch^ other) {
				if (_type == 'x' || other->_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else                                     return _midiValue <= other->_midiValue;
			}
			inline bool operator<=(double other) {
				if (_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else              return _midiValue <= other;
			}
			inline bool operator<=(int other) {
				if (_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else              return _midiValue <= other;
			}
			inline bool operator>=(Pitch^ other) {
				if (_type == 'x' || other->_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else                                     return _midiValue >= other->_midiValue;
			}
			inline bool operator>=(double other) {
				if (_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else              return _midiValue >= other;
			}
			inline bool operator>=(int other) {
				if (_type == 'x') throw gcnew Exception("The pitch is unspecified.");
				else              return _midiValue >= other;
			}
			// Arithmétique
			inline Pitch^ operator+(double transpositionSemitones) {
				if (isPitch()) return gcnew Pitch(_midiValue + transpositionSemitones);
				else           throw gcnew Exception("This pitch can't be transposed.");
			}
			inline Pitch^ operator-(double transpositionSemitones) {
				if (isPitch()) return gcnew Pitch(_midiValue - transpositionSemitones);
				else           throw gcnew Exception("This pitch can't be transposed.");
			}
			inline double operator-(Pitch^ other) {
				if (isSpecified() && other->isSpecified()) return _midiValue - other->_midiValue;
				else                                       throw gcnew Exception("The pitch is unspecified.");
			}

		// Transformations
		protected:
			virtual Pitch^ transform_protected(Transformations::Transformation^ transformation) override {

				if (transformation->isTransposition()) {
					// Transposition
					auto transf = (Transformations::Transposition^)transformation;
					if (isPitch()) {
						double newMidi = _midiValue + transf->offset;
						newMidi = 0.5 * Math::Round(2 * newMidi);
						return gcnew Pitch(newMidi);
					} else if (isPercu()) {
						int newVal = (int)Math::Round(_p_val + transf->offset);
						if (newVal < 1) newVal = 1; else if (newVal > _p_nbVals) newVal = _p_nbVals;
						return gcnew Pitch(newVal, _p_nbVals);
					} else {
						throw gcnew Exception("This pitch can't be transposed.");
					}
				} else if (transformation->isInversion()) {
					// Inversion
					auto transf = (Transformations::Inversion^)transformation;
					if (isPitch()) {
						double newMidi = 2 * transf->center - _midiValue;
						newMidi = 0.5 * Math::Round(2 * newMidi);
						return gcnew Pitch(newMidi);
					} else if (isPercu()) {
						int newVal = (int)Math::Round(_p_nbVals + 1 - _p_val);
						return gcnew Pitch(newVal, _p_nbVals);
					} else {
						throw gcnew Exception("This pitch can't be inverted.");
					}
				} else if (transformation->isRetrograde() || transformation->isAugmentation() || transformation->isDiminution()) {
					// Rétrograde, augmentation, diminution : aucune modification
					return this;
				} else if (transformation->isMultiplication()) {
					// Multiplication
					auto transf = (Transformations::Multiplication^)transformation;
					if (isPitch()) {
						double newMidi = (_midiValue - transf->center) * transf->ratio + transf->center;
						newMidi = 0.5 * Math::Round(2 * newMidi);
						return gcnew Pitch(newMidi);
					} else if (isPercu()) {
						int newVal = (int)Math::Round((_p_val - transf->center) * transf->ratio + transf->center);
						if (newVal < 1) newVal = 1; else if (newVal > _p_nbVals) newVal = _p_nbVals;
						return gcnew Pitch(newVal, _p_nbVals);
					} else {
						throw gcnew Exception("This pitch can't be multiplied.");
					}
				} else if (transformation->isDivision()) {
					// Division
					auto transf = (Transformations::Division^)transformation;
					if (isPitch()) {
						double newMidi = (_midiValue - transf->center) / transf->ratio + transf->center;
						newMidi = 0.5 * Math::Round(2 * newMidi);
						return gcnew Pitch(newMidi);
					} else if (isPercu()) {
						int newVal = (int)Math::Round((_p_val - transf->center) / transf->ratio + transf->center);
						if (newVal < 1) newVal = 1; else if (newVal > _p_nbVals) newVal = _p_nbVals;
						return gcnew Pitch(newVal, _p_nbVals);
					} else {
						throw gcnew Exception("This pitch can't be divided.");
					}
				} else {
					throw gcnew Exception;
				}

			}
		public:
			virtual /* PROV : à supprimer */ Pitch^ transposition(double semitones) {
				
				throw gcnew NotImplementedException;

			}

		// Fonctions privées
		private:
			static double calcMidiValue(char step, Alteration^ alteration, int octave) {
				
				int step_int; {
					if      (step == 'C') step_int =  0;
					else if (step == 'D') step_int =  2;
					else if (step == 'E') step_int =  4;
					else if (step == 'F') step_int =  5;
					else if (step == 'G') step_int =  7;
					else if (step == 'A') step_int =  9;
					else if (step == 'B') step_int = 11;
					else throw gcnew Exception;
				}

				return 12 * (octave + 1) + step_int + (double)alteration;

			}

		// Conversion en texte MusicXML
		public:
			virtual String^ convertToMusicXML() {

				double xml_alter = (double)_alteration;

				return "<pitch>" +
						   "<step>" + (wchar_t)_step + "</step>" +
						   (xml_alter == 0 ? "" : "<alter>" + xml_alter + "</alter>") +
						   "<octave>" + _octave + "</octave>" +
					   "</pitch>";
		
			}

	};

}
}
