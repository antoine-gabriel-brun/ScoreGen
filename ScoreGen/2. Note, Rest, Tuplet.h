#pragma region head

	#pragma once

	using namespace System;

	#include "1. Elements bases.h"

#pragma endregion

namespace ScoreGenLib {
namespace Music {

	// Trois types d'éléments spécialisés : (notes) - (silences) - (n-uplets contenant notes, silences)
	public ref class Note : public NoteOrRest {
		
		// Champs
		private:
			array<Pitch^, 1>^ _pitches;
			NoteDecorations^  _decorations;

		// Constructeurs
		public:
			template <typename TypePitch> Note(Rhythm^ rhythm, TypePitch pitchOrPitches, NoteDecorations^ decorations, array<Words::Words^, 1>^ words) : NoteOrRest(rhythm, words) { constructPitchesAndDecorations(pitchOrPitches, decorations          ); }
			template <typename TypePitch> Note(Rhythm^ rhythm, TypePitch pitchOrPitches, NoteDecorations^ decorations                                ) : NoteOrRest(rhythm       ) { constructPitchesAndDecorations(pitchOrPitches, decorations          ); }
			template <typename TypePitch> Note(Rhythm^ rhythm, TypePitch pitchOrPitches,                               array<Words::Words^, 1>^ words) : NoteOrRest(rhythm, words) { constructPitchesAndDecorations(pitchOrPitches, gcnew NoteDecorations); }
			template <typename TypePitch> Note(Rhythm^ rhythm, TypePitch pitchOrPitches                                                              ) : NoteOrRest(rhythm       ) { constructPitchesAndDecorations(pitchOrPitches, gcnew NoteDecorations); }
		private:
			void constructPitchesAndDecorations(array<Pitch^, 1>^ pitches, NoteDecorations^ decorations) {
				
				// Initialiser les champs
				_pitches     = copy(pitches);
				_decorations = decorations  ;

				// Vérifier '_pitches'
				if (_pitches == nullptr) throw gcnew ArgumentNullException;
				int nbPitches = _pitches->Length;
				if (nbPitches == 0) throw gcnew Exception("A note should have at least one pitch. Specify one or more pitches, or use the class Rest instead.");
				for (int i = 0; i < nbPitches; i++) if (((Object^)(_pitches[i])) == nullptr) throw gcnew ArgumentNullException;
				
				// Vérifier '_decorations'
				if (((Object^)_decorations) == nullptr) throw gcnew ArgumentNullException;

			}
			void constructPitchesAndDecorations(array<int, 1>^ pitches_int, NoteDecorations^ decorations) {
				
				// Convertir les pitches (int --> Pitch^)
				array<Pitch^, 1>^ pitches; {
					if (pitches_int == nullptr) throw gcnew ArgumentNullException;
					int nbPitches = pitches_int->Length;
					pitches = gcnew array<Pitch^, 1>(nbPitches);
					for (int i = 0; i < nbPitches; i++) {
						pitches[i] = gcnew Pitch(pitches_int[i]);
					}
				}

				// Appeler la fonction 'construct' normale
				constructPitchesAndDecorations(pitches, decorations);

			}
			void constructPitchesAndDecorations(array<double, 1>^ pitches_dbl, NoteDecorations^ decorations) {
				
				// Convertir les pitches (Decimal --> Pitch^)
				array<Pitch^, 1>^ pitches; {
					if (pitches_dbl == nullptr) throw gcnew ArgumentNullException;
					int nbPitches = pitches_dbl->Length;
					pitches = gcnew array<Pitch^, 1>(nbPitches);
					for (int i = 0; i < nbPitches; i++) {
						pitches[i] = gcnew Pitch(pitches_dbl[i]);
					}
				}

				// Appeler la fonction 'construct' normale
				constructPitchesAndDecorations(pitches, decorations);

			}
			void constructPitchesAndDecorations(array<String^, 1>^ pitches_stringsToParse, NoteDecorations^ decorations) {
				
				throw gcnew NotImplementedException; // il faut utiliser 'quick'

//				// Convertir les pitches (String^ --> Pitch^)
//				array<Pitch^, 1>^ pitches; {
//					if (pitches_stringsToParse == nullptr) throw gcnew ArgumentNullException;
//					int nbPitches = pitches_stringsToParse->Length;
//					pitches = gcnew array<Pitch^, 1>(nbPitches);
//					for (int i = 0; i < nbPitches; i++) {
//						pitches[i] = gcnew Pitch(pitches_stringsToParse[i]);
//					}
//				}
//
//				// Appeler la fonction 'construct' normale
//				constructPitchesAndDecorations(pitches, decorations);

			}
			void constructPitchesAndDecorations(Pitch^ pitch, NoteDecorations^ decorations) {
				
				constructPitchesAndDecorations(gcnew array<Pitch^, 1> { pitch }, decorations);

			}
			void constructPitchesAndDecorations(int pitch_int, NoteDecorations^ decorations) {

				constructPitchesAndDecorations(gcnew array<int, 1> { pitch_int }, decorations);

			}
			void constructPitchesAndDecorations(double pitch_dbl, NoteDecorations^ decorations) {

				constructPitchesAndDecorations(gcnew array<double, 1> { pitch_dbl }, decorations);

			}
			void constructPitchesAndDecorations(String^ pitch_stringToParse, NoteDecorations^ decorations) {
				
				constructPitchesAndDecorations(gcnew array<String^, 1> { pitch_stringToParse }, decorations);

			}
		public:
			Note(Note^ note, Tuplet^ parentTuplet) :
				NoteOrRest  (note, parentTuplet  ),
				_pitches    (copy(note->_pitches)),
				_decorations(note->_decorations  )
			{}
			Note(Note^ note, array<Pitch^, 1>^ newPitches) :
				NoteOrRest  (note),
				_pitches    (copy(newPitches)    ),
				_decorations(note->_decorations  )
			{
				
				if (((Object^)_pitches) == nullptr) throw gcnew ArgumentNullException;
				for (int i = 0; i < _pitches->Length; i++) if (((Object^)(_pitches[i])) == nullptr) throw gcnew ArgumentNullException;
				if (_pitches->Length == 0) throw gcnew Exception("A note should have at least one pitch. Specify one or more pitches, or use the class Rest instead.");
				
			}
			Note(Note^ note, Pitch^ newPitch) :
				NoteOrRest  (note),
				_pitches    (newArray(newPitch)),
				_decorations(note->_decorations)
			{
				
				if (((Object^)newPitch) == nullptr) throw gcnew ArgumentNullException;
				
			}
		private:
			Note(Note^ note) :
				NoteOrRest  (note                ),
				_pitches    (copy(note->_pitches)),
				_decorations(note->_decorations  )
			{ }
		
		// Pseudo-constructeurs
		public:
			// Appogiatures
			static Note^ graceNote(Rhythm^ rhythm, array<Pitch^, 1>^ pitches, NoteDecorations^ decorations) {
			
				Note^ ans = gcnew Note(rhythm, pitches, decorations);

				ans->_isGrace = true;
				ans->_rhythm = Rhythm::graceRhythm(rhythm);

				return ans;

			}
			static Note^ graceNote(Rhythm^ rhythm, array<Pitch^, 1>^ pitches) {
			
				Note^ ans = gcnew Note(rhythm, pitches);

				ans->_isGrace = true;
				ans->_rhythm = Rhythm::graceRhythm(rhythm);

				return ans;

			}
		public:
			// Création d'une liste de notes liées à partir d'une durée
			static array<Note^, 1>^ getSum(array<Pitch^, 1>^ pitches, int totalDuration_1024th, NoteDecorations^ deco, array<Words::Words^, 1>^ words) {
				
				array<Rhythm^, 1>^ rhythms = Rhythm::getSum(totalDuration_1024th);
				int nb = rhythms->Length;

				array<Note^, 1>^ ans = gcnew array<Note^, 1>(nb);

				array<Articulation^, 1> ^articulationsBeg, ^articulationsEnd; {
					List<Articulation^>^ articulationsBeg_lst = gcnew List<Articulation^>;
					List<Articulation^>^ articulationsEnd_lst = gcnew List<Articulation^>;
					for each (Articulation^ articulation in deco->articulations) {
						if (articulation->whenCutShouldStayWithRightNote) articulationsEnd_lst->Add(articulation);
						else                                              articulationsBeg_lst->Add(articulation);
					}
					articulationsBeg = articulationsBeg_lst->ToArray();
					articulationsEnd = articulationsEnd_lst->ToArray();
				}

				for (int i = 0; i < nb; i++) {
					array<Words::Words^, 1>^ wordsHere = (i == 0) ? words : gcnew array<Words::Words^, 1>(0);
					array<Articulation^, 1>^ articulationsHere; {
						if      (nb == 1    ) articulationsHere = deco->articulations;
						else if (i == 0     ) articulationsHere = articulationsBeg;
						else if (i == nb - 1) articulationsHere = articulationsEnd;
						else                  articulationsHere = gcnew array<Articulation^, 1>(0);
					}
					bool tiedHere    = (i < nb - 1) ? true : deco->tied    ;
					bool slurBegHere = (i == 0            && deco->slurBeg);
					bool slurEndHere = (i == nb - 1       && deco->slurEnd);
					ans[i] = gcnew Note(
						rhythms[i],
						pitches,
						gcnew NoteDecorations(articulationsHere, tiedHere, slurBegHere, slurEndHere),
						wordsHere
					);
				}

				return ans;

			}
			static array<Note^, 1>^ getSum(array<Pitch^, 1>^ pitches, int totalDuration_1024th) {
				
				return getSum(pitches, totalDuration_1024th, gcnew NoteDecorations, gcnew array<Words::Words^, 1>(0));

			}

		// Accesseurs
		public:
			property array<Pitch^, 1>^ pitches {
				array<Pitch^, 1>^ get() {
					return copy(_pitches);
				}
			}
			property NoteDecorations^ decorations {
				NoteDecorations^ get() {
					return _decorations;
				}
			}
			property bool tied    { bool get() { return decorations->tied   ; } }
			property bool slurBeg { bool get() { return decorations->slurBeg; } }
			property bool slurEnd { bool get() { return decorations->slurEnd; } }

		// Implémentations
		public:
			virtual bool isNote() override { return true ; }
			virtual bool isRest() override { return false; }
			virtual LeftRight<array<Element^, 1>^>^ cutAt(int pos_1024th) override {
				
				// Positions incorrectes ou triviales : retour immédiat
				{
					if (pos_1024th < 0 || pos_1024th > _rhythm->duration_1024th) {
						throw gcnew Exception("Can't cut out of the element duration.");
					} else if (pos_1024th == 0) {
						return gcnew LeftRight<array<Element^, 1>^>(
							gcnew array<Element^, 1>(0),
							newArray(this)
						);
					} else if (pos_1024th == _rhythm->duration_1024th) {
						return gcnew LeftRight<array<Element^, 1>^>(
							newArray(this),
							gcnew array<Element^, 1>(0)
						);
					}
				}

				// Couper les décorations en deux
				LeftRight<NoteDecorations^>^ decoLeftRight = _decorations->cut();

				// Diviser la durée, et créer les deux séries de notes
				array<Note^, 1> ^sumLeft, ^sumRight; {
					// Diviser la durée
					int durLeft, durRight; {
						int thisDur = _rhythm->duration_1024th;
						durLeft  = pos_1024th;
						durRight = thisDur - durLeft;
						if (durLeft < 0 || durRight < 0) throw gcnew Exception("Can't cut the note because the position is out of the note duration.");
					}
					// Créer les deux séries de notes
					sumLeft  = Note::getSum(_pitches, durLeft , decoLeftRight->left , _words                          );
					sumRight = Note::getSum(_pitches, durRight, decoLeftRight->right, gcnew array<Words::Words^, 1>(0));
				}
				
				// Retourner
				return gcnew LeftRight<array<Element^, 1>^>(sumLeft, sumRight);

			}
			virtual Element^ transposition(double semitones) override {
			
				Note^ ans = gcnew Note(this);

				int nbPitches = _pitches->Length;
				for (int i = 0; i < nbPitches; i++) {
					ans->_pitches[i] = _pitches[i]->transposition(semitones);
				}

				return ans;

			}

		// Conversion en texte MusicXML
		public:
			virtual String^ convertToMusicXML(int instrumentIndex) override;

	};
	public ref class Rest : public NoteOrRest {

		// Constructeurs
		public:
			Rest(Rhythm^ rhythm, array<Words::Words^, 1>^ words) :
				NoteOrRest(rhythm, words)
			{ }
			Rest(Rhythm^ rhythm) :
				NoteOrRest(rhythm)
			{ }
			Rest(Rest^ rest, Tuplet^ parentTuplet) :
				NoteOrRest(rest, parentTuplet)
			{ }

		// Pseudo-constructeurs
		public:
			// Appogiatures, pauses de mesure complète
			static Rest^ graceRest(Rhythm^ rhythm) {
			
				Rest^ ans = gcnew Rest(rhythm);

				ans->_isGrace = true;
				ans->_rhythm = Rhythm::graceRhythm(rhythm);

				return ans;

			}
			static Rest^ wholeBarRest(int specialDuration_1024th, array<Words::Words^, 1>^ words) {

				return gcnew Rest(Rhythm::wholeBarRhythm(specialDuration_1024th), words);

			}
			static Rest^ wholeBarRest(int specialDuration_1024th) {

				return gcnew Rest(Rhythm::wholeBarRhythm(specialDuration_1024th));

			}
		public:
			// Création d'une liste de silences à partir d'une durée
			static array<Rest^, 1>^ getSum(int totalDuration_1024th, array<Words::Words^, 1>^ words) {
				
				array<Rhythm^, 1>^ rhythms = Rhythm::getSum(totalDuration_1024th);
				int nb = rhythms->Length;

				array<Rest^, 1>^ ans = gcnew array<Rest^, 1>(nb);

				for (int i = 0; i < nb; i++) {
					array<Words::Words^, 1>^ wordsHere = (i == 0) ? words : gcnew array<Words::Words^, 1>(0);
					ans[i] = gcnew Rest(rhythms[i], wordsHere);
				}

				return ans;

			}
			static array<Rest^, 1>^ getSum(int totalDuration_1024th) {
				
				return getSum(totalDuration_1024th, gcnew array<Words::Words^, 1>(0));

			}
		
		// Implémentations
		public:
			virtual bool isNote() override { return false; }
			virtual bool isRest() override { return true ; }
			virtual LeftRight<array<Element^, 1>^>^ cutAt(int pos_1024th) override {
				
				// Positions incorrectes ou triviales : retour immédiat
				{
					if (pos_1024th < 0 || pos_1024th > _rhythm->duration_1024th) {
						throw gcnew Exception("Can't cut out of the element duration.");
					} else if (pos_1024th == 0) {
						return gcnew LeftRight<array<Element^, 1>^>(
							gcnew array<Element^, 1>(0),
							newArray(this)
						);
					} else if (pos_1024th == _rhythm->duration_1024th) {
						return gcnew LeftRight<array<Element^, 1>^>(
							newArray(this),
							gcnew array<Element^, 1>(0)
						);
					}
				}
				
				// Diviser la durée, et créer les deux séries de silences
				array<Rest^, 1> ^sumLeft, ^sumRight; {
					// Diviser la durée
					int durLeft, durRight; {
						int thisDur = _rhythm->duration_1024th;
						durLeft  = pos_1024th;
						durRight = thisDur - durLeft;
						if (durLeft < 0 || durRight < 0) throw gcnew Exception("Can't cut the note because the position is out of the note duration.");
					}
					// Créer les deux séries de silences
					sumLeft  = Rest::getSum(durLeft , _words                          );
					sumRight = Rest::getSum(durRight, gcnew array<Words::Words^, 1>(0));
				}

				// Retourner
				return gcnew LeftRight<array<Element^, 1>^>(sumLeft, sumRight);

			}
			virtual Element^ transposition(double semitones) override {
			
				return this;

			}

		// Conversion en texte MusicXML
		public:
			virtual String^ convertToMusicXML(int instrumentIndex) override;

	};
	public ref class Tuplet : public Element {

		// Champs
		private:
			TupletRatio^           _ratio;
			int                    _nbElements;
			array<NoteOrRest^, 1>^ _elements;
		
		// Constructeurs
		public:
			Tuplet(TupletRatio^ ratio, array<NoteOrRest^, 1>^ elements) {
				
				construct(ratio, elements);

			}
			Tuplet(TupletRatio^ ratio, List<NoteOrRest^>^ elements) {
				
				construct(ratio, elements->ToArray());

			}
		public:
			Tuplet(Tuplet^ tuplet, array<array<Pitch^, 1>^, 1>^ newChords) {
				
				int nb = tuplet->_nbElements;
				int nbReplacementChords = newChords->Length;

				// Nouveau tableau d'éléments
				array<NoteOrRest^, 1>^ newElements = gcnew array<NoteOrRest^, 1>(nb);
				int cpt = 0;
				for (int i = 0; i < nb; i++) {
					NoteOrRest^ nor = tuplet->_elements[i];
					if (nor->isNote()) {
						if (cpt >= nbReplacementChords) throw gcnew Exception("There are not enough replacement chords.");
						newElements[i] = gcnew Note((Note^)nor, newChords[cpt++]);
					} else {
						newElements[i] = nor;
					}
				}
				if (cpt != nbReplacementChords) throw gcnew Exception("Some replacement chords weren't used.");

				// Construire
				construct(tuplet->_ratio, newElements);

			}
			Tuplet(Tuplet^ tuplet, array<Pitch^, 1>^ newPitches) {
				
				int nb = tuplet->_nbElements;
				int nbReplacementPitches = newPitches->Length;

				// Nouveau tableau d'éléments
				array<NoteOrRest^, 1>^ newElements = gcnew array<NoteOrRest^, 1>(nb);
				int cpt = 0;
				for (int i = 0; i < nb; i++) {
					NoteOrRest^ nor = tuplet->_elements[i];
					if (nor->isNote()) {
						if (cpt >= nbReplacementPitches) throw gcnew Exception("There are not enough replacement pitches.");
						newElements[i] = gcnew Note((Note^)nor, newPitches[cpt++]);
					} else {
						newElements[i] = nor;
					}
				}
				if (cpt != nbReplacementPitches) throw gcnew Exception("Some replacement pitches weren't used.");

				// Construire
				construct(tuplet->_ratio, newElements);

			}
			Tuplet(Tuplet^ tuplet, Pitch^ newPitch) {
				
				int nb = tuplet->_nbElements;

				// Nouveau tableau d'éléments
				array<NoteOrRest^, 1>^ newElements = gcnew array<NoteOrRest^, 1>(nb);
				for (int i = 0; i < nb; i++) {
					NoteOrRest^ nor = tuplet->_elements[i];
					if (nor->isNote()) newElements[i] = gcnew Note((Note^)nor, newPitch);
					else               newElements[i] = nor;
				}

				// Construire
				construct(tuplet->_ratio, newElements);

			}
		private:
			void construct(TupletRatio^ ratio, array<NoteOrRest^>^ elements) {

				// Objets nuls ou autres erreurs ?
				if (((Object^)ratio   ) == nullptr) throw gcnew ArgumentNullException;
				if (((Object^)elements) == nullptr) throw gcnew ArgumentNullException;
				for each (NoteOrRest^ noteOrRest in elements) if (((Object^)noteOrRest) == nullptr) throw gcnew ArgumentNullException;
				for each (NoteOrRest^ noteOrRest in elements) if (noteOrRest->rhythm->isGrace) throw gcnew Exception("To avoid a bug in Sibelius, grace notes are not supported in tuplets.");

				// Initialiser les champs
				_ratio      = ratio           ;
				_nbElements = elements->Length;
				_elements   = copy(elements)  ;

				// Vérifier la somme des durées
				int dur_elements = 0; for each (Element^ element in _elements) dur_elements += element->duration_1024th;
				int dur_expected = _ratio->innerDuration_1024th;
				if (dur_elements != dur_expected) throw gcnew Exception("Couldn't create this Tuplet, because the inner duration doesn't match the actual duration.");

				// Dès qu'on déclare un Tuplet, ses éléments doivent avoir un tuplet parent
				for (int i = 0; i < _nbElements; i++) {
					if      (_elements[i]->isNote()) _elements[i] = gcnew Note((Note^)(_elements[i]), this);
					else if (_elements[i]->isRest()) _elements[i] = gcnew Rest((Rest^)(_elements[i]), this);
				}

			}

		// Accesseurs
		public:
			property TupletRatio^ ratio {
				TupletRatio^ get() {
					return _ratio;
				}
			}
			property int nbElements {
				int get() {
					return _nbElements;
				}
			}
			property array<NoteOrRest^>^ elements {
				array<NoteOrRest^>^ get() {
					return copy(_elements);
				}
			}
			int countNotes() {
				
				// Compter
				int ans = 0;
				for each (NoteOrRest^ nor in _elements) {
					if (nor->isNote()) ans++;
				}

				// Retourner
				return ans;

			}

		// Implémentations
		public:
			virtual bool isNote  () override { return false; }
			virtual bool isRest  () override { return false; }
			virtual bool isTuplet() override { return true ; }
			property int duration_1024th {
				virtual int get() override {
					return _ratio->duration_1024th;
				}
			}
			property int innerDuration_1024th {
				int get() {
					return _ratio->innerDuration_1024th ;
				}
			}
			virtual LeftRight<array<Element^, 1>^>^ cutAt(int pos_1024th) override {
				
				// Positions incorrectes ou triviales : retour immédiat
				{
					int duration = duration_1024th;
					if (pos_1024th < 0 || pos_1024th > duration) {
						throw gcnew Exception("Can't cut out of the element duration.");
					} else if (pos_1024th == 0) {
						return gcnew LeftRight<array<Element^, 1>^>(
							gcnew array<Element^, 1>(0),
							newArray(this)
						);
					} else if (pos_1024th == duration) {
						return gcnew LeftRight<array<Element^, 1>^>(
							newArray(this),
							gcnew array<Element^, 1>(0)
						);
					}
				}

				// Calculer le ratio du Tuplet (par exemple : 2/3 pour un triolet)
				Fraction^ upletRatioFraction = gcnew Fraction(_ratio->normal, _ratio->actual);

				// Déterminer la durée avant et après, dans le temps du Tuplet
				Fraction^ dur1inUplet_1024th = pos_1024th / upletRatioFraction;
				Fraction^ dur2inUplet_1024th = _ratio->innerDuration_1024th - dur1inUplet_1024th;

				// Déterminer l'élément durant lequel se produit la coupure, et à quel moment dans cet élément
				int cutIndex;
				Fraction^ cutAfterInElement_1024th;
				{
					int currentTime = 0;
					int cpt = 0;
					for each (NoteOrRest^ noteOrRest in _elements) {
						int thisElementDuration = noteOrRest->duration_1024th;
						int next_currentTime = currentTime + thisElementDuration;
						if (next_currentTime > dur1inUplet_1024th) {
							cutIndex = cpt;
							cutAfterInElement_1024th = dur1inUplet_1024th - currentTime;
							break;
						}
						currentTime = next_currentTime;
						cpt++;
					}
				}
				bool cutAnElement = (cutAfterInElement_1024th != 0);

				// Exprimer les éléments du Tuplet comme des fractions de noires dans le temps général
				array<Fraction^, 1>^ fractions = gcnew array<Fraction^, 1>(_nbElements); {
					for (int i = 0; i < _nbElements; i++) {
						int thisRhythmDuration_1024th = _elements[i]->duration_1024th;
						Fraction^ thisElementQuarterFractionInUplet = gcnew Fraction(thisRhythmDuration_1024th, 256);
						fractions[i] = thisElementQuarterFractionInUplet * upletRatioFraction;
					}
				}

				// Séparer l'élement se trouvant à 'cutIndex' en deux fractions de noires dans le temps général, et en deux éléments de base
				Fraction^ durCutElementLeft_generalTime;
				Fraction^ durCutElementRight_generalTime;
				array<NoteOrRest^, 1>^ originalCutElement_left;
				array<NoteOrRest^, 1>^ originalCutElement_right;
				if (cutAnElement) {
					// Fractions
					durCutElementLeft_generalTime = gcnew Fraction(pos_1024th, 256);
					for (int i = 0; i < cutIndex; i++) durCutElementLeft_generalTime = durCutElementLeft_generalTime - fractions[i];
					durCutElementRight_generalTime = fractions[cutIndex] - durCutElementLeft_generalTime;
					// Elément coupé en deux
					if (!cutAfterInElement_1024th->isInteger()) throw gcnew Exception;
					int cutAfterInElement_1024th___int = cutAfterInElement_1024th->a;
					LeftRight<array<Element^, 1>^>^ leftRight = _elements[cutIndex]->cutAt(cutAfterInElement_1024th___int);
					originalCutElement_left  = convert<Element^, NoteOrRest^>(leftRight->left );
					originalCutElement_right = convert<Element^, NoteOrRest^>(leftRight->right);
				}

				// Etablir les deux séries de fractions de noires (gauche et droite), et les deux séries d'éléments de base correspondants
				array<Fraction^>^ fractionsLeft ;
				array<Fraction^>^ fractionsRight;
				array<array<NoteOrRest^, 1>^>^ originalElementsLeft ;
				array<array<NoteOrRest^, 1>^>^ originalElementsRight;
				{
					List<Fraction^>^ fractionsLeft_lst  = gcnew List<Fraction^>;
					List<Fraction^>^ fractionsRight_lst = gcnew List<Fraction^>;
					List<array<NoteOrRest^, 1>^>^ originalElementsLeft_lst  = gcnew List<array<NoteOrRest^, 1>^>;
					List<array<NoteOrRest^, 1>^>^ originalElementsRight_lst = gcnew List<array<NoteOrRest^, 1>^>;
					for (int i = 0; i < cutIndex; i++) {
						fractionsLeft_lst->Add(fractions[i]);
						originalElementsLeft_lst->Add(newArray(_elements[i]));
					}
					if (cutAnElement) {
						fractionsLeft_lst->Add(durCutElementLeft_generalTime);
						originalElementsLeft_lst->Add(originalCutElement_left);
					}
					if (cutAnElement) {
						fractionsRight_lst->Add(durCutElementRight_generalTime);
						originalElementsRight_lst->Add(originalCutElement_right);
					}
					for (int i = cutIndex + (cutAnElement ? 1 : 0); i < _nbElements; i++) {
						fractionsRight_lst->Add(fractions[i]);
						originalElementsRight_lst->Add(newArray(_elements[i]));
					}
					fractionsLeft         = fractionsLeft_lst        ->ToArray();
					fractionsRight        = fractionsRight_lst       ->ToArray();
					originalElementsLeft  = originalElementsLeft_lst ->ToArray();
					originalElementsRight = originalElementsRight_lst->ToArray();
				}

				// Extraire les deux nouvelles séries d'éléments (le plus souvent constituées d'un seul élément : un Tuplet)
				array<Element^, 1>^ newElementsLeft  = quarterFractionsToElements(fractionsLeft , originalElementsLeft , true, false);
				array<Element^, 1>^ newElementsRight = quarterFractionsToElements(fractionsRight, originalElementsRight, false, true);

				// Si une note a été coupée, ajouter une liaison
				if (cutAnElement) {
					int len = newElementsLeft->Length;
					if (len > 0) {
						if (newElementsLeft[len - 1]->isNote()) {
							Note^ noteToSlur = (Note^)(newElementsLeft[len - 1]);
							NoteDecorations^ deco = noteToSlur->decorations;
							deco = gcnew NoteDecorations(
								deco->articulations,
								true,
								deco->slurBeg,
								deco->slurEnd
							);
							noteToSlur = gcnew Note(noteToSlur->rhythm, noteToSlur->pitches, deco, noteToSlur->words);
							newElementsLeft[len - 1] = noteToSlur;
						} else if (newElementsLeft[len - 1]->isTuplet()) {
							Tuplet^ tuplet = (Tuplet^)(newElementsLeft[len - 1]);
							array<NoteOrRest^, 1>^ subElements = tuplet->elements;
							int nbSubElements = subElements->Length;
							if (nbSubElements > 0) {
								if (subElements[nbSubElements - 1]->isNote()) {
									Note^ noteToSlur = (Note^)(subElements[nbSubElements - 1]);
									NoteDecorations^ deco = noteToSlur->decorations;
									deco = gcnew NoteDecorations(
										deco->articulations,
										true,
										deco->slurBeg,
										deco->slurEnd
									);
									noteToSlur = gcnew Note(noteToSlur->rhythm, noteToSlur->pitches, deco, noteToSlur->words);
									subElements[nbSubElements - 1] = noteToSlur;
								}
							}
							newElementsLeft[len - 1] = gcnew Tuplet(tuplet->ratio, subElements);
						}
					}
				}

				// Retourner
				return gcnew LeftRight<array<Element^, 1>^>(newElementsLeft, newElementsRight);

			}
			virtual Element^ transposition(double semitones) override {
				
				array<NoteOrRest^>^ new_elements = gcnew array<NoteOrRest^>(_nbElements);

				for (int i = 0; i < _nbElements; i++) {
					new_elements[i] = (NoteOrRest^)(_elements[i]->transposition(semitones));
				}

				return gcnew Tuplet(_ratio, new_elements);

			}

		// Comparaison
		public:
			static bool operator == (Tuplet^ u, Tuplet^ v) {
			
				bool u_isNull = ((Object^)u) == nullptr;
				bool v_isNull = ((Object^)v) == nullptr;
			
				if (u_isNull && v_isNull) {
					return true;
				} else if (u_isNull) {
					return false;
				} else if (v_isNull) {
					return false;
				} else {
					throw gcnew Exception("The '==' and '!=' operators can compare Tuplets only if at least one of them is null.");
				}
			
			}
			static bool operator != (Tuplet^ u, Tuplet^ v) {
			
				return !(u == v);
			
			}

		// Fonctions
		private:
			static array<Element^, 1>^ quarterFractionsToElements(array<Fraction^, 1>^ fractionsOfQuarters, array<array<NoteOrRest^, 1>^, 1>^ originalElements, bool isFirstPart, bool isSecondPart) {
				
				int nb = fractionsOfQuarters->Length; if (originalElements->Length != nb) throw gcnew Exception;

				// Exprimer les fractions avec des numérateurs différents et un dénominateur commun
				array<int, 1>^ nums = gcnew array<int, 1>(nb);
				int den;
				{
					// Extraire les numérateurs et les dénominateurs
					array<int, 1>^ dens = gcnew array<int, 1>(nb); {
						for (int i = 0; i < nb; i++) {
							nums[i] = fractionsOfQuarters[i]->a;
							dens[i] = fractionsOfQuarters[i]->b;
						}
					}
					// Remettre toutes les fractions sur un dénominateur commun
					{
						// Stocker dans 'den' le PPCM des dénominateurs
						{
							den = dens[0];
							for (int i = 1; i < nb; i++) {
								den = Fraction::lcm(den, dens[i]);
							}
						}
						// Tout mettre sur ce dénominateur
						for (int i = 0; i < nb; i++) {
							nums[i] *= den / dens[i];
						}
					}
				}

				// Calculer la somme des numérateurs
				int numsSum = 0; for each (int num in nums) numsSum += num;

				// Calculer le facteur impair de ce dénominateur commun
				int den_odd = den; while (den_odd % 2 == 0) den_odd /= 2;

				// Déduire le 'TupletRatio' correspondant
				TupletRatio^ tupletRatio;
				{
					if (den_odd == 1) {
						// Pas besoin de TupletRatio : on a une subdivision naturelle
						tupletRatio = nullptr;
					} else {
						// Le rythme de base correspond à la puissance de 2 inférieure à 'den'
						int den_ = den; {
							bool isPowerOf2;
							do {
								den_--;
								// den_ est-il une puissance de 2 ?
								int a = den_;
								while (a % 2 == 0) a /= 2;
								isPowerOf2 = (a == 1);
							} while (!isPowerOf2);
						}
						// Calculer le TupletRatio
						RhythmFigure^ base = gcnew RhythmFigure(256 / den_);
						tupletRatio = gcnew TupletRatio(base, numsSum, numsSum * den_ / den);
					}
				}

				// Lister les éléments à retourner, selon si un Tuplet est nécessaire
				List<Element^>^ ans = gcnew List<Element^>; {
					if (tupletRatio == nullptr) {
						// Pas de Tuplet : on calcule les valeurs rythmiques correspondant aux fractions de noires (dans le temps général)
						for (int i = 0; i < nb; i++) {
							Fraction^ fraction = fractionsOfQuarters[i];
							Fraction^ value_1024th = 256 * fraction;
							if (!value_1024th->isInteger()) throw gcnew Exception;
							int value_1024th_int = value_1024th->a;
							array<NoteOrRest^, 1>^ originalElementsHere = originalElements[i];
							bool areAllRests, areAllNotes; {
								areAllRests = true;
								areAllNotes = true;
								for each (NoteOrRest^ noteOrRest in originalElementsHere) {
									if      (noteOrRest->isNote()) areAllRests = false;
									else if (noteOrRest->isRest()) areAllNotes = false;
									else                           throw gcnew Exception;
								}
							}
							if (areAllRests) {
								// Collecter tous les mots
								List<Words::Words^>^ words = gcnew List<Words::Words^>; {
									for each (Rest^ rest in originalElementsHere) {
										for each (Words::Words^ wordswords in rest->words) {
											words->Add(wordswords);
										}
									}
								}
								// Ajouter les silences
								array<Rest^, 1>^ rests = Rest::getSum(value_1024th_int);
								int nbRests = rests->Length;
								for (int i = 0; i < nbRests; i++) {
									Rest^ rest = (i == 0) ? (gcnew Rest(rests[i]->rhythm, words->ToArray())) : (rests[i]);
									ans->Add(rest);
								}
							} else if (areAllNotes) {
								// Collecter tous les mots
								List<Words::Words^>^ words = gcnew List<Words::Words^>; {
									for each (Note^ note in originalElementsHere) {
										for each (Words::Words^ wordswords in note->words) {
											words->Add(wordswords);
										}
									}
								}
								// Collecter toutes les décorations en une seule
								NoteDecorations^ deco = gcnew NoteDecorations; {
									for each (Note^ note in originalElementsHere) {
										deco = deco + note->decorations;
									}
								}
								// Ajouter les notes
								array<Note^, 1>^ notes = Note::getSum(((Note^)(originalElementsHere[0]))->pitches, value_1024th_int, deco, words->ToArray());
								for each (Note^ note in notes) {
									ans->Add(note);
								}
							} else {
								throw gcnew Exception;
							}
						}
					} else {
						// Tuplet : on calcule les valeurs rythmiques correspondant aux fractions de noires (dans le temps du Tuplet)
						List<NoteOrRest^>^ upletContents = gcnew List<NoteOrRest^>; {
							Fraction^ upletFraction = gcnew Fraction(tupletRatio->actual, tupletRatio->normal);
							for (int i = 0; i < nb; i++) {
								Fraction^ fraction = fractionsOfQuarters[i] * upletFraction;
								Fraction^ value_1024th = 256 * fraction;
								if (!value_1024th->isInteger()) throw gcnew Exception;
								int value_1024th_int = value_1024th->a;
								array<NoteOrRest^, 1>^ originalElementsHere = originalElements[i];
								bool areAllRests, areAllNotes; {
									areAllRests = true;
									areAllNotes = true;
									for each (NoteOrRest^ noteOrRest in originalElementsHere) {
										if      (noteOrRest->isNote()) areAllRests = false;
										else if (noteOrRest->isRest()) areAllNotes = false;
										else                           throw gcnew Exception;
									}
								}
								if (areAllRests) {
									// Collecter tous les mots
									List<Words::Words^>^ words = gcnew List<Words::Words^>; {
										for each (Rest^ rest in originalElementsHere) {
											for each (Words::Words^ wordswords in rest->words) {
												words->Add(wordswords);
											}
										}
									}
									// Ajouter les silences
									array<Rest^, 1>^ rests = Rest::getSum(value_1024th_int);
									int nbRests = rests->Length;
									for (int i = 0; i < nbRests; i++) {
										Rest^ rest = (i == 0) ? (gcnew Rest(rests[i]->rhythm, words->ToArray())) : (rests[i]);
										upletContents->Add(rest);
									}
								} else if (areAllNotes) {
									// Collecter tous les mots
									List<Words::Words^>^ words = gcnew List<Words::Words^>; {
										for each (Note^ note in originalElementsHere) {
											for each (Words::Words^ wordswords in note->words) {
												words->Add(wordswords);
											}
										}
									}
									// Collecter toutes les décorations en une seule
									NoteDecorations^ deco = gcnew NoteDecorations; {
										for each (Note^ note in originalElementsHere) {
											deco = deco + note->decorations;
										}
									}
									// Ajouter les notes
									array<Note^, 1>^ notes = Note::getSum(((Note^)(originalElementsHere[0]))->pitches, value_1024th_int, deco, words->ToArray());
									for each (Note^ note in notes) {
										upletContents->Add(note);
									}
								} else {
									throw gcnew Exception;
								}
							}
						}
						// Créer et ajouter le Tuplet
						ans->Add(gcnew Tuplet(tupletRatio, upletContents));
					}
				}

				// Retourner
				return ans->ToArray();

			}

		// Conversion en texte MusicXML
		public:
			virtual String^ convertToMusicXML(int instrumentIndex) override {
				
				System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder;

				// Ajouter le texte XML de chaque note
				for (int i = 0; i < _nbElements; i++) {
					Element^ element = _elements[i];
					bool isFirst = (i == 0              );
					bool isLast  = (i == _nbElements - 1);
					// Récupérer le texte XML de la note
					String^ element_str = element->convertToMusicXML(instrumentIndex);
					// Ajouter au texte de la note une notation de crochet de n-uplet
					{
						// Ajouter <notations>...</notations> si absent
						if (!element_str->Contains("<notations>")) element_str = element_str->Replace("</note>", "<notations></notations></note>");
						// Créer la notation du crochet de n-uplet
						String^ tupletNotation_str = ""; {
							if (isFirst) tupletNotation_str += "<tuplet type=\"start\" />";
							if (isLast ) tupletNotation_str += "<tuplet type=\"stop\" />"; // isFirst et isLast peuvent être tous deux vrais
						}
						// Ajouter la notation du crochet de n-uplet
						element_str = element_str->Replace("<notations>", "<notations>" + tupletNotation_str);
					}
					// Ajouter
					sb->Append(element_str);
				}

				// Retourner
				return sb->ToString();

			}

	};

}
}
