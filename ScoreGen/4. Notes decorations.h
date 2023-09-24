#pragma region head

	#pragma once

	using namespace System;

#pragma endregion

namespace ScoreGenLib {
namespace Music {
	
	ref class NoteDecorations;

	// Articulations
	public ref class Articulation {
		
		// Champs
		private:
			int     _id;
			String^ _name;
			bool    _whenCutShouldStayWithRightNote;
		
		// Accesseurs
		public:
			property int id {
				int get() {
					return _id;
				}
			}
			property String^ name {
				String^ get() {
					return _name;
				}
			}
			property bool whenCutShouldStayWithRightNote {
				bool get() {
					return _whenCutShouldStayWithRightNote;
				}
			}

		// Constructeurs et valeurs d'énumération
		private:
			Articulation(int id, String^ name, bool whenCutShouldStayWithRightNote) :
				_id                            (id                            ),
				_name                          (name                          ),
				_whenCutShouldStayWithRightNote(whenCutShouldStayWithRightNote)
			{ }
		public:
			static Articulation^ accent         = gcnew Articulation( 1, "accent"         , false);
			static Articulation^ strongAccent   = gcnew Articulation( 2, "strong-accent"  , false);
			static Articulation^ staccato       = gcnew Articulation( 3, "staccato"       , true );
			static Articulation^ tenuto         = gcnew Articulation( 4, "tenuto"         , false);
			static Articulation^ detachedLegato = gcnew Articulation( 5, "detached-legato", true );
			static Articulation^ staccatissimo  = gcnew Articulation( 6, "staccatissimo"  , true );
			static Articulation^ spiccato       = gcnew Articulation( 7, "spiccato"       , true );
			static Articulation^ scoop          = gcnew Articulation( 8, "scoop"          , false);
			static Articulation^ plop           = gcnew Articulation( 9, "plop"           , false);
			static Articulation^ doit           = gcnew Articulation(10, "doit"           , true );
			static Articulation^ falloff        = gcnew Articulation(11, "falloff"        , true );
			static Articulation^ breathMark     = gcnew Articulation(12, "breath-mark"    , true );
			static Articulation^ caesura        = gcnew Articulation(13, "caesura"        , true );
			static Articulation^ stress         = gcnew Articulation(14, "stress"         , false);
			static Articulation^ unstress       = gcnew Articulation(15, "unstress"       , false);
		public:
			// Conversion implicite en tableau singleton (--> pouvoir toujours donner une articulation unique là où plusieurs sont attendues)
			operator array<Articulation^, 1>^ () {
			
				return gcnew array<Articulation^, 1>{ this };

			}
		public:
			// Conversion en NoteDecorations
			operator NoteDecorations^();

		// Comparaison
		public:
			static bool operator == (Articulation^ a, Articulation^ b) {
				
				bool a_isNull = ((Object^)a) == nullptr;
				bool b_isNull = ((Object^)b) == nullptr;

				if (a_isNull && b_isNull) {
					return true;
				} else if (a_isNull || b_isNull) {
					return false;
				} else {
					return a->_id == b->_id;
				}

			}
			static bool operator != (Articulation^ a, Articulation^ b) {
				
				bool a_isNull = ((Object^)a) == nullptr;
				bool b_isNull = ((Object^)b) == nullptr;

				if (a_isNull && b_isNull) {
					return false;
				} else if (a_isNull || b_isNull) {
					return true;
				} else {
					return a->_id != b->_id;
				}

			}
			ref class ArticulationsComparer : public Collections::Generic::Comparer<Articulation^> {
				
				public:
					virtual int Compare(Articulation^ a, Articulation^ b) override {
						
						int A = a->_id;
						int B = b->_id;

						if      (A < B) return -1;
						else if (A > B) return  1;
						else            return  0;

					}

			};

		// Conversion en texte MusicXML
		public:
			String^ convertToMusicXML() {
				
				return "<" + _name + " />";

			}

	};
	
	// Ensemble d'éléments pouvant accompagner une note : articulations, liaisons
	public ref class NoteDecorations {
		
		// Champs
		private:
			array<Articulation^, 1>^ _articulations;
			bool                     _tied;
			bool                     _slurBeg;
			bool                     _slurEnd;

		// Constructeurs
		public:
			NoteDecorations(array<Articulation^, 1>^ articulations, bool tied, bool slurBeg, bool slurEnd) {
			
				initialize(articulations, tied, slurBeg, slurEnd);

			}
			NoteDecorations(array<Articulation^, 1>^ articulations) {
			
				initialize(articulations, false, false, false);

			}
			NoteDecorations(List<Articulation^>^ articulations, bool tied, bool slurBeg, bool slurEnd) {
			
				initialize(articulations->ToArray(), tied, slurBeg, slurEnd);

			}
			NoteDecorations(List<Articulation^>^ articulations) {
			
				initialize(articulations->ToArray(), false, false, false);

			}
			NoteDecorations(bool tied, bool slurBeg, bool slurEnd) {
			
				initialize(gcnew array<Articulation^, 1>(0), tied, slurBeg, slurEnd);

			}
			NoteDecorations() {
			
				initialize(gcnew array<Articulation^, 1>(0), tied, slurBeg, slurEnd);

			}
		private:
			void initialize(array<Articulation^, 1>^ articulations, bool tied, bool slurBeg, bool slurEnd) {
				
				// Initialisations
				_articulations = copy(articulations);
				_tied          =      tied          ;
				_slurBeg       =      slurBeg       ;
				_slurEnd       =      slurEnd       ;

				// Vérifications
				if ((Object^)_articulations == nullptr) throw gcnew ArgumentNullException;
				for each (Articulation^ articulation in _articulations) if ((Object^)articulation == nullptr) throw gcnew ArgumentNullException;

				// Simplifications sur '_articulations' : éviter absolument la redite d'une même articulation
				{
					// Classer
					Array::Sort(_articulations, gcnew Articulation::ArticulationsComparer);
					// Ajouter les articulations une à une, en retirant les redites
					List<Articulation^>^ new_articulations = gcnew List<Articulation^>; {
						Articulation^ last = nullptr;
						for each (Articulation^ articulation in _articulations) {
							if (articulation != last) {
								new_articulations->Add(articulation);
								last = articulation;
							}
						}
					}
					// Enregistrer
					_articulations = new_articulations->ToArray();
				}

			}

		// Accesseurs
		public:
			property array<Articulation^, 1>^ articulations {
				array<Articulation^, 1>^ get() {
					return copy(_articulations);
				}
			}
			property bool tied    { bool get() { return _tied   ; } }
			property bool slurBeg { bool get() { return _slurBeg; } }
			property bool slurEnd { bool get() { return _slurEnd; } }

		// Opérateurs
		public:
			static NoteDecorations^ operator + (NoteDecorations^ a, NoteDecorations^ b) {
				
				return gcnew NoteDecorations(
					concat(a->_articulations, b->_articulations), // les articulations sont cumulées
					b->_tied,                                     // le groupe de notes est lié à la suite si la deuxième note du groupe l'est
					a->_slurBeg || b->_slurBeg,
					a->_slurEnd || b->_slurEnd
				);

			}

		// Fonctions
		public:
			static LeftRight<array<Articulation^, 1>^>^ cutArticulations(array<Articulation^, 1>^ articulations) {
				
				// Séparer les articulations restant à gauche et celles restant à droite
				List<Articulation^> ^left, ^right; {
					left = gcnew List<Articulation^>;
					right = gcnew List<Articulation^>;
					for each (Articulation^ articulation in articulations) {
						if (articulation->whenCutShouldStayWithRightNote) right->Add(articulation);
						else                                              left ->Add(articulation);
					}
				}

				// Retourner
				return gcnew LeftRight<array<Articulation^, 1>^>(left->ToArray(), right->ToArray());

			}
			LeftRight<NoteDecorations^>^ cut() {
				
				bool tiedLeft     = true;
				bool tiedRight    = _tied;
				bool slurBegLeft  = _slurBeg;
				bool slurBegRight = false;
				bool slurEndLeft  = false;
				bool slurEndRight = _slurEnd;
				LeftRight<array<Articulation^, 1>^>^ articulationsLeftRight = cutArticulations(_articulations);

				return gcnew LeftRight<NoteDecorations^>(
					gcnew NoteDecorations(articulationsLeftRight->left , tiedLeft , slurBegLeft , slurEndLeft ),
					gcnew NoteDecorations(articulationsLeftRight->right, tiedRight, slurBegRight, slurEndRight)
				);

			}

		// Conversion en texte MusicXML
		public:
			String^ convertToMusicXML(bool lastNoteWasTied) {
				
				// Cas où il n'y a pas besoin de balise <notations>
				if (!lastNoteWasTied && !_tied && !_slurBeg && !_slurEnd && _articulations->Length == 0) return "";

				// Articulations
				String^ articulations_str; {
					if (_articulations->Length == 0) {
						articulations_str = "";
					} else {
						articulations_str = "<articulations>";
						for each (Articulation^ articulation in _articulations) {
							articulations_str += articulation->convertToMusicXML();
						}
						articulations_str += "</articulations>";
					}
				}

				// Retour
				return
					"<notations>" +
						(lastNoteWasTied ? "<tied type=\"stop\" />"  : "") +
						(_tied           ? "<tied type=\"start\" />" : "") +
						(_slurEnd        ? "<slur type=\"stop\" />"  : "") +
						(_slurBeg        ? "<slur type=\"start\" />" : "") +
						articulations_str +
					"</notations>";

			}

	};

}
}
