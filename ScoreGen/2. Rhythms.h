#pragma region head

	#pragma once

	using namespace System;

#pragma endregion

namespace ScoreGenLib {
namespace Music {

	ref struct Rhythm;
	ref struct TupletRatio;

	// Figure rythmique (exemples : noire, croche...)
	public ref class RhythmFigure {

		// Champs
		private:
			int     _duration_1024th;
			String^ _name;

		// Constructeurs et conversions
		public:
			RhythmFigure(int duration_1024th) :
				_duration_1024th(duration_1024th)
			{
				
				// Initialisation du nom, et vérifications
				if      (_duration_1024th ==    4) _name = "256th"  ;
				else if (_duration_1024th ==    8) _name = "128th"  ;
				else if (_duration_1024th ==   16) _name = "64th"   ;
				else if (_duration_1024th ==   32) _name = "32nd"   ;
				else if (_duration_1024th ==   64) _name = "16th"   ;
				else if (_duration_1024th ==  128) _name = "eighth" ;
				else if (_duration_1024th ==  256) _name = "quarter";
				else if (_duration_1024th ==  512) _name = "half"   ;
				else if (_duration_1024th == 1024) _name = "whole"  ;
				else if (_duration_1024th == 2048) _name = "breve"  ;
				else if (_duration_1024th == 4096) _name = "long"   ;
				else throw gcnew Exception("To create a 'RhythmFigure', only the durations which are powers of 2 between 4 and 4096 are accepted.");
				
			}
		public:
			operator Rhythm^();

		// Accesseurs
		public:
			property int duration_1024th {
				int get() {
					return _duration_1024th;
				}
			}
			property String^ name {
				String^ get() {
					return _name;
				}
			}

		// Valeurs
		public:
			static RhythmFigure^ _256th   = gcnew RhythmFigure(   4);
			static RhythmFigure^ _128th   = gcnew RhythmFigure(   8);
			static RhythmFigure^ _64th    = gcnew RhythmFigure(  16);
			static RhythmFigure^ _32nd    = gcnew RhythmFigure(  32);
			static RhythmFigure^ _16th    = gcnew RhythmFigure(  64);
			static RhythmFigure^ _eighth  = gcnew RhythmFigure( 128);
			static RhythmFigure^ _quarter = gcnew RhythmFigure( 256);
			static RhythmFigure^ _half    = gcnew RhythmFigure( 512);
			static RhythmFigure^ _whole   = gcnew RhythmFigure(1024);
			static RhythmFigure^ _breve   = gcnew RhythmFigure(2048);
			static RhythmFigure^ _long    = gcnew RhythmFigure(4096);

		// Opérateurs
		public:
			static inline bool operator == (RhythmFigure^ a, RhythmFigure^ b) {

				return a->_duration_1024th == b->_duration_1024th;

			}
			static inline bool operator != (RhythmFigure^ a, RhythmFigure^ b) {
			
				return a->_duration_1024th != b->_duration_1024th;

			}
		
		// Conversion en texte MusicXML (balise <type></type>)
		public:
			String^ convertToMusicXML() {
		
				return "<type>" + _name + "</type>";
		
			}

	};

	// Figure rythmique complétée de 0 à 4 points rythmiques (exemples : croche pointée, noire simple, ronde triplement pointée...)
	public ref struct Rhythm {
	
		// Champs
		private:
			RhythmFigure^ _rhythmFigure;
			int           _dots;
			int           _duration_1024th; // dans le cas des rythmes de mesure entière et des appoggiatures, peut être sans lien avec la figure rythmique
			bool          _isWholeBar;
			bool          _isGrace;

		// Constructeurs et conversions
		public:
			Rhythm(RhythmFigure^ rhythmFigure) :
				_rhythmFigure   (rhythmFigure),
				_dots           (0),
				_duration_1024th(rhythmFigure->duration_1024th),
				_isWholeBar     (false),
				_isGrace        (false)
			{ }
			Rhythm(RhythmFigure^ rhythmFigure, int dots) :
				_rhythmFigure(rhythmFigure),
				_dots        (dots),
				_isWholeBar  (false),
				_isGrace     (false)
			{
				
				if (((Object^)_rhythmFigure) == nullptr) throw gcnew ArgumentNullException;
				if (_dots < 0 || _dots > 4) throw gcnew Exception("Dots number should be in the range 0 - 4.");
		
				_duration_1024th = calcDuration_1024th(_rhythmFigure, _dots);

			}
			Rhythm(int duration_1024th) :
				_duration_1024th(duration_1024th),
				_isWholeBar     (false),
				_isGrace        (false)
			{
			
				array<Rhythm^, 1>^ sum = getSum(_duration_1024th);

				if (sum->Length != 1) throw gcnew Exception("This duration can't be represented by a single Rhythm. For a sequence of rhythms, you can use getSum() instead. For a special whole-bar rhythm, you can use wholeBarRhythm().");

				_rhythmFigure = sum[0]->_rhythmFigure;
				_dots         = sum[0]->_dots        ;

			}
		private:
			Rhythm(Rhythm^ rhythm) :
				_rhythmFigure   (rhythm->_rhythmFigure   ),
				_dots           (rhythm->_dots           ),
				_duration_1024th(rhythm->_duration_1024th),
				_isWholeBar     (rhythm->_isWholeBar     ),
				_isGrace        (rhythm->_isGrace        )
			{ }
		public:
			static Rhythm^ wholeBarRhythm(int specialDuration_1024th) {
			
				Rhythm^ ans = gcnew Rhythm(RhythmFigure::_whole);

				ans->_isWholeBar = true;
				ans->_duration_1024th = specialDuration_1024th;

				return ans;

			}
			static Rhythm^ graceRhythm(Rhythm^ rhythm) {
			
				Rhythm^ ans = gcnew Rhythm(rhythm->_rhythmFigure, rhythm->_dots);

				ans->_isGrace = true;
				ans->_duration_1024th = 0;

				return ans;

			}

		// Accesseurs
		public:
			property RhythmFigure^ rhythmFigure {
				RhythmFigure^ get() {
					return _rhythmFigure;
				}
			}
			property int dots {
				int get() {
					return _dots;
				}
			}
			property int duration_1024th {
				int get() {
					return _duration_1024th;
				}
			}
			property bool isWholeBar {
				bool get() {
					return _isWholeBar;
				}
			}
			property bool isGrace {
				bool get() {
					return _isGrace;
				}
			}

		// Création d'une liste rythmique à partir d'une durée
		public:
			static array<Rhythm^, 1>^ getSum(int totalDuration_1024th) {
	
				if (totalDuration_1024th % 4 != 0) throw gcnew Exception("Integers can be seen as a sum of rhythmic figures with dots only if divisible by 4.");
				if (totalDuration_1024th == 0) return gcnew array<Rhythm^, 1>(0);

				// Décomposer en base 2 : obtenir une suite de puissances de 2 décroissantes entre 2048 et 4. Exemples ci-dessous.
				{
					// Exemple 1 : 1728 = 1024+512+128+64 = ronde pointée liée à croche pointée.
					// Exemple 2 : 4544 = 2048+2048+256+128+64 = deux notes carrées liées à une noire liée à une croche pointée.
				}
				array<int, 1>^ elements;
				int nbElements;
				{
					List<int>^ elements_lst = gcnew List<int>;
					int remaining = totalDuration_1024th;
					while (remaining >= 2048) {
						elements_lst->Add(2048);
						remaining -= 2048;
					}
					for (int i = 1024; i >= 4; i /= 2) {
						if (remaining >= i) {
							elements_lst->Add(i);
							remaining -= i;
						}
					}
					elements = elements_lst->ToArray();
					nbElements = elements->Length;
				}
		
				// Créer la liste de réponse, en partant des plus petites valeurs
				List<Rhythm^>^ ans = gcnew List<Rhythm^>; {
					int nextIndex = nbElements - 1;
					while (nextIndex >= 0) {
						if (nextIndex > 0 && elements[nextIndex - 1] == 2 * elements[nextIndex]) {
							// Valeur pointée
							ans->Add(gcnew Rhythm(gcnew RhythmFigure(elements[nextIndex - 1]), 1));
							nextIndex -= 2;
						} else {
							// Valeur non pointée
							ans->Add(gcnew Rhythm(gcnew RhythmFigure(elements[nextIndex]), 0));
							nextIndex -= 1;
						}
					}
				}
				
				// Inverser l'ordre et retourner sous forme de tableau
				ans->Reverse();
				return ans->ToArray();

			}

		// Comparaison
		public:
			static bool operator == (Rhythm^ r, Rhythm^ s) {
			
				return r->_rhythmFigure    == s->_rhythmFigure
					&& r->_dots            == s->_dots
					&& r->_duration_1024th == s->_duration_1024th
					&& r->_isWholeBar      == s->_isWholeBar;

			}
			static bool operator != (Rhythm^ r, Rhythm^ s) {
			
				return r->_rhythmFigure    != s->_rhythmFigure
					|| r->_dots            != s->_dots
					|| r->_duration_1024th != s->_duration_1024th
					|| r->_isWholeBar      != s->_isWholeBar;

			}

		// Fonctions
		private:
			static int calcDuration_1024th(RhythmFigure^ rhythmFigure, int dots) {
			
				int sum = 0;
				int current = rhythmFigure->duration_1024th;

				for (int i = 0; i <= dots; i++) {
					sum += current;
					current = current / 2;
				}

				return sum;
			
			}

		// Conversion en texte MusicXML
		public:
			String^ convertToMusicXML(int instrumentIndex, bool tiedFromPrev, bool tiedToNext, TupletRatio^ tupletRatio);
			
	};
	
	// Ratio de n-tuplet, par exemple "3 noires dans 2"
	public ref struct TupletRatio {
	
		// Champs
		private:
			Rhythm^ _base;
			int     _actual;
			int     _normal;
			int     _duration_1024th;
			int     _innerDuration_1024th;
		
		// Constructeurs
		public:
			TupletRatio(Rhythm^ base, int actual, int normal) :
				_base                (base                          ),
				_actual              (actual                        ),
				_normal              (normal                        ),
				_duration_1024th     (base->duration_1024th * normal),
				_innerDuration_1024th(base->duration_1024th * actual)
			{
			
				// Vérifications
				if (actual < 1 || normal < 1) throw gcnew Exception("In an TupletRatio definition, the ratio values shouldn't be less than 1.");

			}
			TupletRatio(RhythmFigure^ base, int actual, int normal) :
				_base                (base                          ),
				_actual              (actual                        ),
				_normal              (normal                        ),
				_duration_1024th     (base->duration_1024th * normal),
				_innerDuration_1024th(base->duration_1024th * actual)
			{
			
				// Vérifications
				if (actual < 1 || normal < 1) throw gcnew Exception("In an TupletRatio definition, the ratio values shouldn't be less than 1.");

			}
		public:
			static TupletRatio^ /* 2 : 3*/ duplet     (RhythmFigure^ base) { return gcnew TupletRatio(base, 2, 3); }
			static TupletRatio^ /* 3 : 2*/ triplet    (RhythmFigure^ base) { return gcnew TupletRatio(base, 3, 2); }
			static TupletRatio^ /* 4 : 3*/ quadruplet (RhythmFigure^ base) { return gcnew TupletRatio(base, 4, 3); }
			static TupletRatio^ /* 5 : 4*/ quintuplet (RhythmFigure^ base) { return gcnew TupletRatio(base, 5, 4); }
			static TupletRatio^ /* 6 : 4*/ sextuplet  (RhythmFigure^ base) { return gcnew TupletRatio(base, 6, 4); }
			static TupletRatio^ /* 7 : 4*/ septuplet  (RhythmFigure^ base) { return gcnew TupletRatio(base, 7, 4); }
		
		// Accesseurs
		public:
			property Rhythm^ base {
				Rhythm^ get() {
					return _base;
				}
			}
			property int actual {
				int get() {
					return _actual;
				}
			}
			property int normal {
				int get() {
					return _normal;
				}
			}
			property int duration_1024th {
				int get() {
					return _duration_1024th;
				}
			}
			property int innerDuration_1024th {
				int get() {
					return _innerDuration_1024th;
				}
			}

		// Comparaison
		public:
			static bool operator == (TupletRatio^ u, TupletRatio^ v) {

				bool u_isNull = ((Object^)u) == nullptr;
				bool v_isNull = ((Object^)v) == nullptr;

				if (u_isNull && v_isNull) {
					return true;
				} else if (u_isNull) {
					return false;
				} else if (v_isNull) {
					return false;
				} else {
					return u->_base   == v->_base
						&& u->_actual == v->_actual
						&& u->_normal == v->_normal;
				}

			}
			static bool operator != (TupletRatio^ u, TupletRatio^ v) {

				bool u_isNull = ((Object^)u) == nullptr;
				bool v_isNull = ((Object^)v) == nullptr;

				if (u_isNull && v_isNull) {
					return false;
				} else if (u_isNull) {
					return true;
				} else if (v_isNull) {
					return true;
				} else {
					return u->_base   != v->_base
						|| u->_actual != v->_actual
						|| u->_normal != v->_normal;
				}
			}

		// Conversion en texte MusicXML
		public:
			String^ convertToMusicXML() {
			
				String^ base_str; {
					base_str =
						"<normal-type>" +
							_base->rhythmFigure->name +
						"</normal-type>";
					for (int i = 0; i < _base->dots; i++) base_str += "<normal-dot />";
				}

				return "<time-modification>" +
						 "<actual-notes>" + _actual + "</actual-notes>" +
						 "<normal-notes>" + _normal + "</normal-notes>" +
						 base_str +
					   "</time-modification>";

			}

	};

}
}
