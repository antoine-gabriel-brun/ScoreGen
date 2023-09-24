#pragma region head

	#pragma once

	#include "1. Tools.h"

	using namespace System;

#pragma endregion

namespace ScoreGenLib {
namespace Music {

	// Indication de mesure
	public ref class Signature {
	
		// Champs
		private:
			int            _nbNumerators;
			array<int, 1>^ _numerators;
			int            _numeratorsSum;
			int            _denominator;
			String^        _symbol;
			int            _duration_1024th;

		// Constructeurs
		public:
			// D�finies comme un rapport ou comme un rapport compos�
			Signature(int numerator, int denominator) :
				_nbNumerators(1),
				_numeratorsSum(numerator),
				_denominator(denominator),
				_symbol(""),
				_duration_1024th(calcDuration_1024th(_numeratorsSum, _denominator))
			{
			
				// Initialisation des num�rateurs
				_numerators = gcnew array<int, 1> { numerator };

				// V�rifications
				checkFields_throwIfIncorrect();

			}
			Signature(array<int, 1>^ numerators, int denominator) :
				_nbNumerators(numerators->Length),
				_numerators(copy(numerators)),
				_denominator(denominator),
				_symbol("")
			{
				
				// Calcul de la somme, et de la dur�e, qui en d�pend
				_numeratorsSum = 0;
				for (int i = 0; i < _nbNumerators; i++) {
					_numeratorsSum += numerators[i];
				}
				_duration_1024th = calcDuration_1024th(_numeratorsSum, _denominator);
				
				// V�rifications
				checkFields_throwIfIncorrect();

			}
		public:
			// D�finies par une dur�e
			Signature(int duration_1024th, Signature^ optionalTimeSignatureToImitate) {
			
				if (optionalTimeSignatureToImitate != nullptr && optionalTimeSignatureToImitate->duration_1024th == duration_1024th) {
                    // Si la mesure � imiter a la bonne dur�e, on la recopie
                    _nbNumerators    = optionalTimeSignatureToImitate->_nbNumerators   ;
                    _numerators      = optionalTimeSignatureToImitate->_numerators     ;  
                    _numeratorsSum   = optionalTimeSignatureToImitate->_numeratorsSum  ;
                    _denominator     = optionalTimeSignatureToImitate->_denominator    ;
                    _symbol          = optionalTimeSignatureToImitate->_symbol         ;  
                    _duration_1024th = optionalTimeSignatureToImitate->_duration_1024th;
				} else {
					// Sinon, on choisit le plus grand d�nominateur qui convient (mais pas plus de 4)
					int denom, durDenom_1024th; {
						if      (duration_1024th % 256 == 0) { denom =    4; durDenom_1024th = 256; }
						else if (duration_1024th % 128 == 0) { denom =    8; durDenom_1024th = 128; }
						else if (duration_1024th %  64 == 0) { denom =   16; durDenom_1024th =  64; }
						else if (duration_1024th %  32 == 0) { denom =   32; durDenom_1024th =  32; }
						else if (duration_1024th %  16 == 0) { denom =   64; durDenom_1024th =  16; }
						else if (duration_1024th %   8 == 0) { denom =  128; durDenom_1024th =   8; }
						else if (duration_1024th %   4 == 0) { denom =  256; durDenom_1024th =   4; }
						else if (duration_1024th %   2 == 0) { denom =  512; durDenom_1024th =   2; }
						else                                 { denom = 1024; durDenom_1024th =   1; }
					}
					// Calculer le num�rateur
					int num = duration_1024th / durDenom_1024th;
					// Initialiser
					_nbNumerators	 = 1;
					_numerators		 = gcnew array<int, 1> { num };
					_numeratorsSum	 = num;
					_denominator	 = denom;
					_symbol			 = (num == 4 && denom == 4) ? "common" : "";
					_duration_1024th = duration_1024th;
				}

				// V�rifier
				checkFields_throwIfIncorrect();

			}
		private:
			// Constructeur explicite priv�, sans v�rification ni copie
			Signature(array<int, 1>^ numerators, int numeratorsSum, int denominator, String^ symbol) :
				_nbNumerators   (numerators->Length),
				_numerators     (numerators        ),
				_numeratorsSum  (numeratorsSum     ),
				_denominator    (denominator       ),
				_symbol         (symbol            ),
				_duration_1024th(calcDuration_1024th(_numeratorsSum, _denominator))
			{ }
		public:
			// Standard
			static Signature^ common = gcnew Signature(gcnew array<int, 1> { 4 }, 4, 4, "common");
			static Signature^ cut    = gcnew Signature(gcnew array<int, 1> { 2 }, 2, 2, "cut"   );

		// Accesseurs
		public:
			property int nbNumerators {
				int get() {
					return _nbNumerators;
				}
			}
			property array<int, 1>^ numerators {
				array<int, 1>^ get() {
					return copy(_numerators);
				}
			}
			property int denominator {
				int get() {
					return _denominator;
				}
			}
			property String^ symbol {
				String^ get() {
					return _symbol;
				}
			}
			property int duration_1024th {
				int get() {
					return _duration_1024th;
				}
			}

		// Comparaison
		public:
			static bool operator == (Signature^ a, Signature^ b) {
				
				bool a_isNull = ((Object^)a) == nullptr;
				bool b_isNull = ((Object^)b) == nullptr;

				if (a_isNull && b_isNull) return true;
				if (a_isNull || b_isNull) return false;

				return    a->_nbNumerators  == b->_nbNumerators
				    && eq(a->_numerators    ,  b->_numerators)
				    &&    a->_numeratorsSum == b->_numeratorsSum
				    &&    a->_denominator   == b->_denominator
				    &&    a->_symbol        == b->_symbol;

			}
			static bool operator != (Signature^ a, Signature^ b) {
				
				bool a_isNull = ((Object^)a) == nullptr;
				bool b_isNull = ((Object^)b) == nullptr;

				if (a_isNull && b_isNull) return false;
				if (a_isNull || b_isNull) return true;

				return     a->_nbNumerators  != b->_nbNumerators
				    || !eq(a->_numerators    ,  b->_numerators)
				    ||     a->_numeratorsSum != b->_numeratorsSum
				    ||     a->_denominator   != b->_denominator
				    ||     a->_symbol        != b->_symbol;

			}
		private:
			static bool eq(array<int, 1>^ a, array<int, 1>^ b) {
				
				int len = a->Length;
				if (b->Length != len) return false;

				for (int i = 0; i < len; i++) {
					if (a[i] != b[i]) {
						return false;
					}
				}

				return true;

			}

		// Fonctions
		private:
			void checkFields_throwIfIncorrect() {
			
				// Nombre de num�rateurs
				if (_nbNumerators < 1) throw gcnew Exception("There should be at least one numerator.");
				if (_nbNumerators != _numerators->Length) throw gcnew Exception;

				// Num�rateurs
				for (int i = 0; i < _nbNumerators; i++) if (_numerators[i] <= 0) throw gcnew Exception("The numerators shouldn't be negative.");

				// Somme des num�rateurs
				int sum = 0; for (int i = 0; i < _nbNumerators; i++) sum += _numerators[i]; if (_numeratorsSum != sum) throw gcnew Exception;

				// D�nominateur
				if (_denominator != 1 && _denominator != 2 && _denominator != 4 && _denominator != 8 && _denominator != 16 && _denominator != 32 && _denominator != 64 && _denominator != 128 && _denominator != 256) throw gcnew Exception("The denominator should be a power of two in the range 1 - 256.");

				// Symbole
				if (_symbol != "") {
					if (_symbol == "common") {
						if (!(
							_nbNumerators == 1 &&
							_numerators[0] == 4 &&
							_numeratorsSum == 4 &&
							_denominator == 4
						)) throw gcnew Exception;
					} else if (_symbol == "cut") {
						if (!(
							_nbNumerators == 1 &&
							_numerators[0] == 2 &&
							_numeratorsSum == 2 &&
							_denominator == 2
						)) throw gcnew Exception;
					} else throw gcnew Exception;
				}

			}
			static int calcDuration_1024th(int numeratorsSum, int denominator) {
			
				return numeratorsSum * 1024 / denominator;

			}

		// Conversion en texte MusicXML
		public:
			String^ convertToMusicXML() {

				// Balises englobantes <time></time>
				String^ time_open = (_symbol == "") ? "<time>" : "<time symbol=\"" + _symbol + "\">";
				String^ time_close = "</time>";

				// Num�rateurs (balise <beats></beats>)
				String^ beats; {
					beats = "";
					for each (int num in _numerators) {
						if (beats == "") beats +=       num;
						else             beats += "+" + num;
					}
					beats = "<beats>" + beats + "</beats>";
				}
			
				// D�nominateur (balise <beat-type></beat-type>)
				String^ beatType = "<beat-type>" + _denominator + "</beat-type>";

				// Retourner la version MusicXML
				return
					time_open +
						beats +
						beatType +
					time_close;

			}

	};
	
}
}
