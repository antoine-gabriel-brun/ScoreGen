#ifndef NEW_PITCH_HPP
#define NEW_PITCH_HPP

#include "NEW_Tools.hpp"

#include <vector>
#include <string>
#include <cmath>

namespace NEW_ScoreGenLib {
namespace NEW_Music {

	// Altération (entier ou semi-entier non mutable entre -2 et 2)
	class NEW_Alteration {
		// Champs
		private:
			double _val;
		private:
			static NEW_Exception badAlterationValue;
		// Constructeur
		public:
			NEW_Alteration(double val) :
				_val(val)
			{
				if (_val != 0 && _val != 1 && _val != -1 && _val != 2 && _val != -2 && _val != 0.5 && _val != -0.5 && _val != 1.5 && _val != -1.5) {
					throw badAlterationValue;
				}
			}
			NEW_Alteration(int val) :
				_val(val)
			{
				if (_val < -2 || _val > 2) {
					throw badAlterationValue;
				}
			}
			NEW_Alteration() :
				_val(0)
			{ }
		// Valeurs
		public:
			static inline NEW_Alteration natural      () { return NEW_Alteration( 0  ); }
			static inline NEW_Alteration sharp        () { return NEW_Alteration( 1  ); }
			static inline NEW_Alteration flat         () { return NEW_Alteration(-1  ); }
			static inline NEW_Alteration doubleSharp  () { return NEW_Alteration( 2  ); }
			static inline NEW_Alteration doubleFlat   () { return NEW_Alteration(-2  ); }
			static inline NEW_Alteration halfSharp    () { return NEW_Alteration( 0.5); }
			static inline NEW_Alteration halfFlat     () { return NEW_Alteration(-0.5); }
			static inline NEW_Alteration sharpAndAHalf() { return NEW_Alteration( 1.5); }
			static inline NEW_Alteration flatAndAHalf () { return NEW_Alteration(-1.5); }
		// Opérateurs
		public:
			inline bool operator==(const NEW_Alteration& other) const { return _val == other._val; }
			inline bool operator!=(const NEW_Alteration& other) const { return _val != other._val; }
			inline bool operator==(double                other) const { return _val == other     ; }
			inline bool operator!=(double                other) const { return _val != other     ; }
			inline bool operator==(int                   other) const { return _val == other     ; }
			inline bool operator!=(int                   other) const { return _val != other     ; }
			inline operator double() const { return _val; }
	};

	// Hauteur de note (autorisant les quarts de tons)
	class NEW_Pitch {
		// Champs
		private:
			char           _type      ; // n = note | p = percu | x = pitch non spécifié (par exemple : rythmique en attente de mélodie)
			double         _midiValue ; // valeur MIDI du pitch, ou valeur MIDI de la position sur la portée
			char           _step      ; // A - G
			NEW_Alteration _alteration; // altération entière ou demi-entière, entre -2 et +2
			int            _octave    ; // l'octave 4 commence au do serrure
		// Constructeurs
		public:
			// Type n : par le nom de note, l'altération et l'octave
			inline NEW_Pitch(char step, const NEW_Alteration& alteration, int octave) { initializeFromStepAlterationOctave(step, alteration                , octave); }
			inline NEW_Pitch(char step, double                alteration, int octave) { initializeFromStepAlterationOctave(step, NEW_Alteration(alteration), octave); }
			inline NEW_Pitch(char step, int                   alteration, int octave) { initializeFromStepAlterationOctave(step, NEW_Alteration(alteration), octave); }
			// Type n : par la valeur MIDI
			NEW_Pitch(double midiValue);
			inline NEW_Pitch(int midiValue) { initializeFromMidiValue(midiValue, false); }
			// Type p : par la valeur et le nombre total de valeurs
			inline NEW_Pitch(int val, int nbVals) { initializeFromValAndNbVals(val, nbVals); }
			// Type x
			inline NEW_Pitch() :
				_type      ('x'),
				_midiValue (71),
				_step      ('B'),
				_alteration(NEW_Alteration::natural()),
				_octave    (4)
			{ }
		private:
			// Initialisateurs privés
			void initializeFromStepAlterationOctave(char step, const NEW_Alteration& alteration, int octave);
			void initializeFromMidiValue(int midiValue_floor, bool addHalf);
			void initializeFromValAndNbVals(int val, int nbVals);
		// Accesseurs
		public:
			inline char           get_type      () const { return _type       ; }
			inline bool           isPitch       () const { return _type == 'n'; }
			inline bool           isPercu       () const { return _type == 'p'; }
			inline bool           isUnspecified () const { return _type == 'x'; }
			inline bool           isSpecified   () const { return _type != 'x'; }
			inline double         get_midiValue () const { return _midiValue ; }
			inline char           get_step      () const { if (isSpecified()) return _step      ; else throw NEW_Exception("This pitch hasn't a step."       ); }
			inline NEW_Alteration get_alteration() const { if (isSpecified()) return _alteration; else throw NEW_Exception("This pitch hasn't an alteration."); }
			inline int            get_octave    () const { if (isSpecified()) return _octave    ; else throw NEW_Exception("This pitch hasn't an octave."    ); }
		// Opérateurs
		public:
			// Comparaison
			inline bool operator==(const NEW_Pitch& other) const { return _type == other._type && _step == other._step && _alteration == other._alteration && _octave == other._octave; }
			inline bool operator!=(const NEW_Pitch& other) const { return _type != other._type || _step != other._step || _alteration != other._alteration || _octave != other._octave; }
			inline bool operator==(double other) const { if (_type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue == other; }
			inline bool operator!=(double other) const { if (_type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue != other; }
			inline bool operator==(int other)    const { if (_type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue == other; }
			inline bool operator!=(int other)    const { if (_type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue != other; }
			inline bool operator< (const NEW_Pitch& other) const { if (_type == 'x' || other._type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue <  other._midiValue; }
			inline bool operator> (const NEW_Pitch& other) const { if (_type == 'x' || other._type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue >  other._midiValue; }
			inline bool operator<=(const NEW_Pitch& other) const { if (_type == 'x' || other._type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue <= other._midiValue; }
			inline bool operator>=(const NEW_Pitch& other) const { if (_type == 'x' || other._type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue >= other._midiValue; }
			inline bool operator< (double           other) const { if (_type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue <  other; }
			inline bool operator> (double           other) const { if (_type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue >  other; }
			inline bool operator<=(double           other) const { if (_type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue <= other; }
			inline bool operator>=(double           other) const { if (_type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue >= other; }
			inline bool operator< (int              other) const { if (_type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue <  other; }
			inline bool operator> (int              other) const { if (_type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue >  other; }
			inline bool operator<=(int              other) const { if (_type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue <= other; }
			inline bool operator>=(int              other) const { if (_type == 'x') throw NEW_Exception("The pitch is unspecified."); else return _midiValue >= other; }
			// Arithmétique
			inline NEW_Pitch operator+(double transpositionSemitones) const { if (isPitch()) return NEW_Pitch(_midiValue + transpositionSemitones); else throw NEW_Exception("The pitch can't be transposed."); }
			inline NEW_Pitch operator-(double transpositionSemitones) const { if (isPitch()) return NEW_Pitch(_midiValue - transpositionSemitones); else throw NEW_Exception("The pitch can't be transposed."); }
			inline double    operator-(const NEW_Pitch& other) const { if (isSpecified() && other.isSpecified()) return _midiValue - other._midiValue; else throw NEW_Exception("The pitch is unspecified."); }
		// Fonctions publiques
		public:
			inline NEW_Pitch transpose(double semitones) const { if (isPitch()) return NEW_Pitch(_midiValue + semitones); else throw NEW_Exception("The pitch can't be transposed."); }
		// Fonctions privées
		private:
			static double calcMidiValue(char step, const NEW_Alteration& alteration, int octave);
		// Conversion en texte MusicXML
		public:
			std::string convertToMusicXML() const;
	};

}
}

#endif
