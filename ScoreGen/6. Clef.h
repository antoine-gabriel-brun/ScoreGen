#pragma region head

	#pragma once

	using namespace System;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::Drawing;
	using namespace System::Drawing::Drawing2D;

	#include "5. Block.h"

#pragma endregion

namespace ScoreGenLib {
namespace Music {
	
	// Clé musicale
	public ref class Clef {

		// Variables
		private:
			String^ _sign;
			int     _line;
			int     _octaveChange;

		// Constructeur
		public:
			Clef(String^ sign, int line, int octaveChange) {
				
				// Initialisation et vérifications : signe
				sign = sign->Trim()->ToLower();
				if      (sign == "c"                           ) _sign = "C";
				else if (sign == "g"                           ) _sign = "G";
				else if (sign == "f"                           ) _sign = "F";
				else if (sign == "perc" || sign == "percussion") _sign = "percussion";
				else                                             throw gcnew Exception("The sign should be 'C', 'G', 'F', or 'percussion' or 'perc'.");

				// Initialisation et vérifications : ligne
				if (line >= 1 && line <= 5) _line = line;

				// Initialisation et vérifications : octaveChange
				if (octaveChange >= -2 && octaveChange <= 2) _octaveChange = octaveChange;
			
			}
			Clef(String^ sign, int line) :
				_octaveChange(0)
			{
				
				// Initialisation et vérifications : signe
				sign = sign->Trim()->ToLower();
				if      (sign == "c"                           ) _sign = "C";
				else if (sign == "g"                           ) _sign = "G";
				else if (sign == "f"                           ) _sign = "F";
				else if (sign == "perc" || sign == "percussion") _sign = "percussion";
				else                                             throw gcnew Exception("The sign should be 'C', 'G', 'F', or 'percussion' or 'perc'.");

				// Initialisation et vérifications : ligne
				if (line >= 1 && line <= 5) _line = line;
			
			}
		public:
			static Clef^ G1 = gcnew Clef("G", 1);
			static Clef^ G2 = gcnew Clef("G", 2);
			static Clef^ C1 = gcnew Clef("C", 1);
			static Clef^ C2 = gcnew Clef("C", 2);
			static Clef^ C3 = gcnew Clef("C", 3);
			static Clef^ C4 = gcnew Clef("C", 4);
			static Clef^ C5 = gcnew Clef("C", 5);
			static Clef^ F3 = gcnew Clef("F", 3);
			static Clef^ F4 = gcnew Clef("F", 4);
			static Clef^ F5 = gcnew Clef("F", 5);
			static Clef^ G2_m1 = gcnew Clef("G", 2, -1);
			static Clef^ G2_m2 = gcnew Clef("G", 2, -2);
			static Clef^ G2_p1 = gcnew Clef("G", 2, +1);
			static Clef^ G2_p2 = gcnew Clef("G", 2, +2);
			static Clef^ F4_m1 = gcnew Clef("F", 4, -1);
			static Clef^ F4_m2 = gcnew Clef("F", 4, -2);
			static Clef^ F4_p1 = gcnew Clef("F", 4, +1);
			static Clef^ F4_p2 = gcnew Clef("F", 4, +2);

		// Accesseurs
		public:
			property String^ sign {
				String^ get() {
					return _sign;
				}
			}
			property int line {
				int get() {
					return _line;
				}
			}
			property int octaveChange {
				int get() {
					return _octaveChange;
				}
			}

		// Comparaison
		public:
			static inline bool operator == (Clef^ a, Clef^ b) {

				return a->_sign         == b->_sign
				    && a->_line         == b->_line
				    && a->_octaveChange == b->_octaveChange;

			}
			static inline bool operator != (Clef^ a, Clef^ b) {

				return a->_sign         != b->_sign
				    || a->_line         != b->_line
				    || a->_octaveChange != b->_octaveChange;

			}

		// Conversion en texte MusicXML
		public:
			String^ convertToMusicXML() {
				
				String^ clefOctaveChange = (_octaveChange == 0) ? "" : "<clef-octave-change>" + _octaveChange + "</clef-octave-change>";

				return
					"<clef>" +
						"<sign>" + _sign + "</sign>" +
						"<line>" + _line + "</line>" +
						clefOctaveChange +
					"</clef>";

			}

	};
	
}
}
