#pragma region head

	#pragma once

	using namespace System;
	using namespace System::Collections;
	using namespace System::Collections::Generic;

	#include "2. Note, Rest, Tuplet.h"

#pragma endregion

namespace ScoreGenLib {
namespace Music {
	
	// Séquence d'éléments, par exemple une figure musicale ou le contenu d'une mesure
	public ref class Group {
	
		// Champs
		private:
			int                 _nbElements;
			array<Element^, 1>^ _elements;
			int                 _duration_1024th;

		// Constructeurs
		public:
			Group(List<Element^>^ elements) :
				_nbElements(elements->Count),
				_elements(elements->ToArray()),
				_duration_1024th(calcDuration_1024th(elements))
			{
				
				// Vérifications
				for each (Element^ element in _elements) if (((Object^)element) == nullptr) throw gcnew ArgumentNullException;
			
			}
			Group(array<Element^, 1>^ elements) :
				_nbElements(elements->Length),
				_elements(copy(elements)),
				_duration_1024th(calcDuration_1024th(elements))
			{
				
				// Vérifications
				for each (Element^ element in _elements) if (((Object^)element) == nullptr) throw gcnew ArgumentNullException;
			
			}
			Group(Element^ element) :
				_nbElements(1),
				_elements(newArray(element)),
				_duration_1024th(element->duration_1024th)
			{ }
			Group(Group^ group, array<array<Pitch^, 1>^, 1>^ newChords) :
				_nbElements(group->_nbElements),
				_duration_1024th(group->_duration_1024th)
			{

				int nbReplacementChords = newChords->Length;
				
				// Nouveau tableau d'éléments
				array<Element^, 1>^ newElements = gcnew array<Element^, 1>(_nbElements);
				int cpt = 0;
				for (int i = 0; i < _nbElements; i++) {
					Element^ element = group->_elements[i];
					if (element->isTuplet()) {
						Tuplet^ tuplet = (Tuplet^)element;
						int tupletNbNotes = tuplet->countNotes();
						if (cpt + tupletNbNotes - 1 >= nbReplacementChords) throw gcnew Exception("There are not enough replacement chords.");
						array<array<Pitch^, 1>^, 1>^ replacementChords = gcnew array<array<Pitch^, 1>^, 1>(tupletNbNotes);
						for (int j = 0; j < tupletNbNotes; j++) replacementChords[j] = newChords[cpt + j];
						newElements[i] = gcnew Tuplet(tuplet, replacementChords);
						cpt += tupletNbNotes;
					} else if (element->isNote()) {
						if (cpt >= nbReplacementChords) throw gcnew Exception("There are not enough replacement chords.");
						newElements[i] = gcnew Note((Note^)element, newChords[cpt++]);
					} else {
						newElements[i] = element;
					}
				}
				if (cpt != nbReplacementChords) throw gcnew Exception("Some replacement chords weren't used.");
				
				// Terminer la construction
				_elements = newElements;

			}
			Group(Group^ group, array<Pitch^, 1>^ newPitches) :
				_nbElements(group->_nbElements),
				_duration_1024th(group->_duration_1024th)
			{

				int nbReplacementPitches = newPitches->Length;
				
				// Nouveau tableau d'éléments
				array<Element^, 1>^ newElements = gcnew array<Element^, 1>(_nbElements);
				int cpt = 0;
				for (int i = 0; i < _nbElements; i++) {
					Element^ element = group->_elements[i];
					if (element->isTuplet()) {
						Tuplet^ tuplet = (Tuplet^)element;
						int tupletNbNotes = tuplet->countNotes();
						if (cpt + tupletNbNotes - 1 >= nbReplacementPitches) throw gcnew Exception("There are not enough replacement pitches.");
						array<Pitch^, 1>^ replacementPitches = gcnew array<Pitch^, 1>(tupletNbNotes);
						for (int j = 0; j < tupletNbNotes; j++) replacementPitches[j] = newPitches[cpt + j];
						newElements[i] = gcnew Tuplet(tuplet, replacementPitches);
						cpt += tupletNbNotes;
					} else if (element->isNote()) {
						if (cpt >= nbReplacementPitches) throw gcnew Exception("There are not enough replacement pitches.");
						newElements[i] = gcnew Note((Note^)element, newPitches[cpt++]);
					} else {
						newElements[i] = element;
					}
				}
				if (cpt != nbReplacementPitches) throw gcnew Exception("Some replacement pitches weren't used.");
				
				// Terminer la construction
				_elements = newElements;

			}
			Group(Group^ group, Pitch^ newPitch) :
				_nbElements(group->_nbElements),
				_duration_1024th(group->_duration_1024th)
			{
				
				// Nouveau tableau d'éléments
				array<Element^, 1>^ newElements = gcnew array<Element^, 1>(_nbElements);
				for (int i = 0; i < _nbElements; i++) {
					Element^ element = group->_elements[i];
					if      (element->isTuplet()) newElements[i] = gcnew Tuplet((Tuplet^)element, newPitch);
					else if (element->isNote  ()) newElements[i] = gcnew Note((Note^)element, newPitch);
					else                          newElements[i] = element;
				}
				
				// Terminer la construction
				_elements = newElements;

			}

		// Accesseurs
		public:
			property int nbElements {
				int get() {
					return _nbElements;
				}
			}
			property Element^ default[int] {
				Element^ get(int index) {
					return _elements[index];
				}
			}
			property array<Element^, 1>^ elements {
				array<Element^, 1>^ get() {
					return copy(_elements);
				}
			}
			property int duration_1024th {
				int get() {
					return _duration_1024th;
				}
			}
			int countNotes() {
				
				// Compter
				int ans = 0;
				for each (Element^ element in _elements) {
					if (element->isTuplet()) ans += ((Tuplet^)element)->countNotes();
					else if (element->isNote()) ans++;
				}

				// Retourner
				return ans;

			}

		// Opérateurs
		public:
			// Conversion implicite en 'Block^', en 'array<Element^, 1>^', etc.
			operator Block^();
			operator array<Element^, 1>^() {
				
				return copy(_elements);

			}
		public:
			// Concaténateurs
			static Group^ operator + (Group^ group, Element^ element) {
				
				return gcnew Group(concat(group->_elements, newArray(element)));

			}
			static Group^ operator + (Element^ element, Group^ group) {
				
				return gcnew Group(concat(newArray(element), group->_elements));

			}
			static Group^ operator + (Group^ group1, Group^ group2) {
				
				return gcnew Group(concat(group1->_elements, group2->_elements));

			}
			static Group^ operator + (Group^ group, array<Element^, 1>^ elements) {
				
				return gcnew Group(concat(group->_elements, elements));

			}
			static Group^ operator + (array<Element^, 1>^ elements, Group^ group) {
				
				return gcnew Group(concat(elements, group->_elements));

			}
			static Group^ operator + (Group^ group, List<Element^>^ elements) {
				
				return gcnew Group(concat(group->_elements, elements->ToArray()));

			}
			static Group^ operator + (List<Element^>^ elements, Group^ group) {
				
				return gcnew Group(concat(elements->ToArray(), group->_elements));

			}
			static Group^ operator * (Group^ group, int nbRepetitions) {
				
				int len_init = group->_nbElements;
				int len_new = len_init * nbRepetitions;

				array<Element^, 1>^ elements = gcnew array<Element^, 1>(len_new);
				for (int n = 0; n < nbRepetitions; n++) {
					for (int i = 0; i < len_init; i++) {
						elements[n * len_init + i] = group->_elements[i];
					}
				}

				return gcnew Group(elements);

			}
			static Group^ operator * (int nbRepetitions, Group^ group) {
				
				return group * nbRepetitions;

			}
			static Group^ join(array<Group^, 1>^ groups) {
				
				List<Element^>^ elements = gcnew List<Element^>;

				for each (Group^ group in groups) {
					for each (Element^ element in group->_elements) {
						elements->Add(element);
					}
				}

				return gcnew Group(elements);

			}
			static Group^ join(List<Group^>^ groups) {
				
				List<Element^>^ elements = gcnew List<Element^>;

				for each (Group^ group in groups) {
					for each (Element^ element in group->_elements) {
						elements->Add(element);
					}
				}

				return gcnew Group(elements);

			}

		// Fonctions publiques
		public:
			Group^ transposition(double semitones) {
				
				array<Element^, 1>^ new_elements = gcnew array<Element^, 1>(_nbElements);
				
				for (int i = 0; i < _nbElements; i++) {
					Element^ element = _elements[i];
					if      (element->isNote  ()) new_elements[i] = ((Note  ^)element)->transposition(semitones);
					else if (element->isRest  ()) new_elements[i] =           element                             ;
					else if (element->isTuplet()) new_elements[i] = ((Tuplet^)element)->transposition(semitones);
				}

				return gcnew Group(new_elements);

			}
			LeftRight<Group^>^ cutAt(int pos_1024th) {
				
				// Positions incorrectes ou triviales : retour immédiat
				{
					if (pos_1024th < 0 || pos_1024th > _duration_1024th) {
						throw gcnew Exception("Can't cut out of the element duration.");
					} else if (pos_1024th == 0) {
						return gcnew LeftRight<Group^>(
							gcnew Group(gcnew array<Element^, 1>(0)),
							this
						);
					} else if (pos_1024th == _duration_1024th) {
						return gcnew LeftRight<Group^>(
							this,
							gcnew Group(gcnew array<Element^, 1>(0))
						);
					}
				}

				// Chercher l'index de l'élément à couper, et la position de coupure dans cet élément
				int indexElementToCut;
				int posInElementToCut_1024th;
				{
					indexElementToCut = -1;
					int cptPos = 0;
					for (int i = 0; i < _nbElements; i++) {
						int thisElementDuration = _elements[i]->duration_1024th;
						int posCut_inRelationToThisElement = pos_1024th - cptPos;
						if (posCut_inRelationToThisElement <= thisElementDuration) {
							indexElementToCut = i;
							posInElementToCut_1024th = posCut_inRelationToThisElement;
							break;
						}
						cptPos += thisElementDuration;
					}
					if (indexElementToCut == -1) throw gcnew Exception;
				}

				// Récupérer l'élément au milieu à couper, sous forme d'une partie gauche et d'une partie droite
				LeftRight<array<Element^, 1>^>^ leftRight = _elements[indexElementToCut]->cutAt(posInElementToCut_1024th);

				// Séparer tous les éléments de gauche et tous les éléments de droite
				List<Element^>^ elementsLeft  = gcnew List<Element^>;
				List<Element^>^ elementsRight = gcnew List<Element^>;
				{
					for      (int i = 0; i < indexElementToCut; i++              ) elementsLeft ->Add(_elements[i]);
					for each (Element^ element in leftRight->left                ) elementsLeft ->Add( element    );
					for each (Element^ element in leftRight->right               ) elementsRight->Add( element    );
					for      (int i = indexElementToCut + 1; i < _nbElements; i++) elementsRight->Add(_elements[i]);
				}

				// Retourner
				return gcnew LeftRight<Group^>(gcnew Group(elementsLeft), gcnew Group(elementsRight));

			}

		// Fonctions
		private:
			template <typename T> static int calcDuration_1024th(T elementsCollection) {
			
				// Faire la somme des éléments
				int sum = 0;
				for each (Element^ element in elementsCollection) {
					sum += element->duration_1024th;
				}

				// Retourner
				return sum;

			}
			static void throwIfVoid(List<Element^>^ elements) {

				if (elements->Count == 0) throw gcnew Exception("There should be at least one element.");

			}
			static void throwIfVoid(array<Element^, 1>^ elements) {
		
				if (elements->Length == 0) throw gcnew Exception("There should be at least one element.");

			}

	};

}
}
