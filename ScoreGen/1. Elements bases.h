#pragma region head

	#pragma once

	using namespace System;

	#include "1. Tools.h"
	#include "1. Pitch.h"
	#include "2. Rhythms.h"
	#include "3. Words.h"
	#include "4. Notes decorations.h"

#pragma endregion

namespace ScoreGenLib {
namespace Music {

	ref class Tuplet; ref class Group; ref class Block;

	// Eléments : (notes) - (silences) - (n-uplets contenant notes, silences)
	public ref class Element abstract {
	
		// Champs
		protected:
			Tuplet^ _parentTuplet;

		// A implémenter
		public:
			virtual bool isNote  () = 0;
			virtual bool isRest  () = 0;
			virtual bool isTuplet() = 0;
			property int duration_1024th { virtual int get() = 0; }
			virtual LeftRight<array<Element^, 1>^>^ cutAt(int pos_1024th) = 0;
			virtual Element^ transposition(double semitones) = 0;
		
		// Champs statiques et gestion des 'tiedFromPrev'
		protected:
			static bool lastNoteWasTied;
		public:
			static void resetTied() { lastNoteWasTied = false; }

		// Opérateurs
		public:
			// Conversion en 'Group' et en 'Block'
			operator Group^();
			operator Block^();
		public:
			// Conversion en tableau singleton (--> pouvoir toujours donner un élément unique là où plusieurs sont attendus)
			operator array<Element^, 1>^ () {
			
				return gcnew array<Element^, 1>{ this };

			}
		public:
			// Concaténation
			static Group^ operator + (Element^ element1, Element^ element2);
			static Group^ operator * (Element^ element, int nbRepetitions);
			static Group^ operator * (int nbRepetitions, Element^ element) {
				
				return element * nbRepetitions;

			}

		// Conversion en texte MusicXML
		public:
			virtual String^ convertToMusicXML(int instrumentIndex) = 0;

	};

	// Spécialisation : (note) ou (silence)
	public ref class NoteOrRest abstract : public Element {

		// Champs
		protected:
			Rhythm^                  _rhythm;
			array<Words::Words^, 1>^ _words;
			bool                     _isGrace;

		// Constructeurs
		public:
			NoteOrRest(Rhythm^ rhythm, array<Words::Words^, 1>^ words) :
				_rhythm (rhythm),
				_words  (words == nullptr ? gcnew array<Words::Words^, 1>(0) : copy(words)),
				_isGrace(false)
			{
			
				if (((Object^)_rhythm) == nullptr) throw gcnew ArgumentNullException;
				
				if (_words->Length == 1 && (((Object^)(_words[0])) == nullptr)) _words = gcnew array<Words::Words^, 1>(0);
				else for each (Words::Words^ w in _words) if (((Object^)w) == nullptr) throw gcnew ArgumentNullException;

			}
			NoteOrRest(Rhythm^ rhythm) :
				_rhythm (rhythm),
				_words  (gcnew array<Words::Words^, 1>(0)),
				_isGrace(rhythm->isGrace)
			{
			
				if (((Object^)_rhythm) == nullptr) throw gcnew ArgumentNullException;

			}
			NoteOrRest(NoteOrRest^ noteOrRest, Tuplet^ parentTuplet) :
				_rhythm(noteOrRest->_rhythm),
				_words (copy(noteOrRest->_words)),
				_isGrace(noteOrRest->_isGrace)
			{
			
				_parentTuplet = parentTuplet;
			
			}
		protected:
			NoteOrRest(NoteOrRest^ noteOrRest) :
				_rhythm(noteOrRest->_rhythm),
				_words (copy(noteOrRest->_words)),
				_isGrace(noteOrRest->_isGrace)
			{ }

		// Accesseurs
		public:
			property Rhythm^ rhythm {
				Rhythm^ get() {
					return _rhythm;
				}
			}
			property array<Words::Words^, 1>^ words {
				array<Words::Words^, 1>^ get() {
					return copy(_words);
				}
			}
			property Tuplet^ parentTuplet {
				Tuplet^ get() {
					return _parentTuplet;
				}
			}

		// Implémentations
		public:
			virtual bool isTuplet() override {
				
				return false;

			}
			property int duration_1024th {
				virtual int get() override {
					return _rhythm->duration_1024th;
				}
			}

	};

}
}
